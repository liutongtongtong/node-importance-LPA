#include "network.h"

Network::Network()
{
    numberOfNodes = 0;
}

bool Network::initialize(string inputPath)
{
    try
    {

        QFile inputFile(QString::fromStdString(inputPath));

        if (!inputFile.open(QFile::ReadOnly)) // try to open the file
            return false;




        // first loop to get the number of nodes
        while (!inputFile.atEnd()) // read until EOF
        {
            QString line = inputFile.readLine(); // read line by line
            line = line.replace('\n', "\0"); // remove the endl

            QStringList list = line.split('\t'); // get two nodes

            int v1 = list.at(0).toInt(),
                    v2 = list.at(1).toInt();

            int value = (v2 > v1 ? v2 : v1);

            if (value > numberOfNodes)
                numberOfNodes = value;
        }


        // now get the edges
        inputFile.seek(0); // put the cursor at the beginning of the file


        // node labels start from zero so we should initializr from 1 to n
        edges = new SparseMatrix<int>(numberOfNodes);

        nodes = new Node[numberOfNodes + 1];
        for (int i = 1; i <= numberOfNodes; ++i)
            nodes[i].label = i;



        // second loop to get the edges
        while (!inputFile.atEnd()) // get number of nodes
        {
            QString line = inputFile.readLine();
            line = line.replace('\n', "\0");


            QStringList list = line.split('\t');


            int v1 = list.at(0).toInt(),
                    v2 = list.at(1).toInt();


            // indirected graph; having edge in both direction
            edges->set(v1, v2, 1);
            edges->set(v2, v1, 1);

            // add the corresponding degree
            nodes[v1].degree++;
            nodes[v2].degree++;

        }



        // close the reading file
        inputFile.close();
    }
    catch(exception e)
    {
        cerr << e.what() << endl;
    }


}

bool Network::computeNodeInfluence()
{
    srand(QTime::currentTime().second());
    float alpha = (float) rand() / RAND_MAX;
    for (int i = 1; i <= numberOfNodes; ++i)
    {
        nodes[i].nodeInfluence = nodes[i].degree; // NI(i) = Ks(i) + ...

        float tmpSum = 0;
        for (int i = 1; i <= numberOfNodes; ++i)
            tmpSum += (float) (nodes[i].kShell / nodes[i].degree);
        tmpSum *= alpha;

        nodes[i].nodeInfluence += tmpSum;

    }




    return true;
}

bool Network::computeLabelInfluence()
{
    for (int i = 1; i <= numberOfNodes; ++i)
    {
        for (int j = 1; j <= numberOfNodes; ++j)
        {
            if (edges->get(i, j) != 0) // they are neighbors
                nodes[i].labelInfluence += (float) (nodes[j].nodeInfluence
                                                    / nodes[j].degree);
        }
    }


    return true;
}

bool Network::computeNewLabels()
{
    for (int i = 1; i <= numberOfNodes; ++i)
    {
        float maxLi;
        int j, indexMaxLi;
        for (j = 1; j <= numberOfNodes; ++j)
        {
            if (edges->get(i, j) != 0)
            {
                maxLi = nodes[j].labelInfluence;
                indexMaxLi = j;
                break;
            }
        }
        for (; j <= numberOfNodes; ++j)
        {
            if (edges->get(i, j) != 0)
            {
                if (nodes[j].nodeInfluence > maxLi)
                {
                    maxLi = nodes[i].labelInfluence;
                    indexMaxLi = j;
                }

            }
        }

        nodes[i].newLabel = indexMaxLi; // finish
    }


    return true;
}

bool Network::writeResultsToFile(string resultPath)
{
    fstream output;
    try
    {
        output.open(resultPath, ios::app);
        for (int i = 1; i <= numberOfNodes; i++)
            output << i << "\t" << nodes[i].newLabel << endl;
        output.flush();
        output.close();
        return true;
    }
    catch (exception ex)
    {
        cerr << ex.what() << endl;
    }

}
