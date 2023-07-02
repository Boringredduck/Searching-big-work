#include <iostream>
#include <vector>
#include <queue>
#include <mpi.h>

using namespace std;

// Function to perform parallel BFS
void parallelBFS(vector<vector<int>>& graph, vector<bool>& visited, queue<int>& q, int rank, int numProcesses) {
    while (!q.empty()) {
        int currentNode = q.front();
        q.pop();
        cout << "Process " << rank << " visited Node: " << currentNode << endl;
        visited[currentNode] = true;

        for (int i = 0; i < graph.size(); i++) {
            if (graph[currentNode][i] == 1 && !visited[i]) {
                int destination = i % numProcesses;
                if (destination == rank) {
                    q.push(i);
                    visited[i] = true;
                }
                else {
                    MPI_Send(&i, 1, MPI_INT, destination, 0, MPI_COMM_WORLD);
                }
            }
        }
    }
}

// Function to initiate parallel BFS
void initiateParallelBFS(vector<vector<int>>& graph, int startNode, int rank, int numProcesses) {
    int numNodes = graph.size();
    vector<bool> visited(numNodes, false);
    queue<int> q;

    if (rank == 0) {
        q.push(startNode);
        parallelBFS(graph, visited, q, rank, numProcesses);
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
            q.push(currentNode);
            parallelBFS(graph, visited, q, rank, numProcesses);
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

    // Perform parallel BFS
    initiateParallelBFS(graph, startNode, rank, numProcesses);

    MPI_Finalize();

    return 0;
}
