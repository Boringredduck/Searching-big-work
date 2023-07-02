#include <iostream>
#include <vector>
#include <omp.h>

using namespace std;

// Function to perform parallel DFS
void parallelDFS(vector<vector<int>>& graph, vector<bool>& visited, int currentNode) {
    cout << "Visited Node: " << currentNode << endl;
    visited[currentNode] = true;

#pragma omp parallel for
    for (int i = 0; i < graph.size(); i++) {
        if (graph[currentNode][i] == 1 && !visited[i]) {
#pragma omp task
            parallelDFS(graph, visited, i);
        }
    }
}

// Function to initiate parallel DFS
void initiateParallelDFS(vector<vector<int>>& graph, int startNode) {
    int numNodes = graph.size();
    vector<bool> visited(numNodes, false);

#pragma omp parallel
    {
#pragma omp single
        {
#pragma omp task
            parallelDFS(graph, visited, startNode);
        }
    }
}

int main() {
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
    initiateParallelDFS(graph, startNode);

    return 0;
}
