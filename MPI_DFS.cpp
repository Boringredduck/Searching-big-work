#include <iostream>
#include <vector>
#include <mpi.h>

using namespace std;

// Function to perform parallel DFS
void parallelDFS(vector<vector<int>>& graph, vector<bool>& visited, int currentNode, int rank, int numProcesses) {
    cout << "Process " << rank << " visited Node: " << currentNode << endl;
    visited[currentNode] = true;

    for (int i = 0; i < graph.size(); i++) {
        if (graph[currentNode][i] == 1 && !visited[i]) {
            int destination = i % numProcesses;
            if (destination == rank) {
                parallelDFS(graph, visited, i, rank, numProcesses);
            }
            else {
                MPI_Send(&i, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
            }
        }
    }
}

// Function to initiate parallel DFS
void initiateParallelDFS(vector<vector<int>>& graph, int startNode, int rank, int numProcesses) {
    int numNodes = graph.size();
    vector<bool> visited(numNodes, false);

    if (rank == 0) {
        parallelDFS(graph, visited, startNode, rank, numProcesses);
        int terminationFlag = -1;
        for (int i = 1; i < numProcesses; i++) {
            MPI_Send(&terminationFlag, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        int currentNode;
        while (true) {
            MPI_Recv(&currentNode, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (currentNode == -1) {
                break;
            }
            parallelDFS(graph, visited, currentNode, rank, numProcesses);
        }
    }
}

int main(int argc, char** argv) {
    int rank, numProcesses;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    int numNodes = 6;
    vector<vector<int>> graph = {
        {0, 1, 1, 0, 0, 0},
        {1, 0, 0, 1, 0, 0},
        {1, 0, 0, 1, 1, 0},
        {0, 1, 1, 0, 1, 1},
        {0, 0, 1, 1, 0, 1},
        {0, 0, 0, 1, 1, 0}
    };

    int startNode = 0;

    // Perform parallel DFS
    initiateParallelDFS(graph, startNode, rank, numProcesses);

    MPI_Finalize();

    return 0;
}
