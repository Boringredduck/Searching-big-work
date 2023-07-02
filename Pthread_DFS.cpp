#include <iostream>
#include <vector>
#include <pthread.h> // ����Pthread��ͷ�ļ�

// ����DFS����ṹ��
struct DFSTask {
    std::vector<int>& graph;
    std::vector<bool>& visited;
    int startNode;
};

void dfs(std::vector<int>& graph, std::vector<bool>& visited, int currentNode) {
    visited[currentNode] = true;
    // ִ��DFS��������������ھӽڵ�
    for (int neighbor : graph[currentNode]) {
        if (!visited[neighbor]) {
            dfs(graph, visited, neighbor);
        }
    }
}

void* parallelDFS(void* arg) {
    DFSTask* task = static_cast<DFSTask*>(arg);
    std::vector<int>& graph = task->graph;
    std::vector<bool>& visited = task->visited;
    int startNode = task->startNode;

    dfs(graph, visited, startNode);

    pthread_exit(nullptr);
}

int main() {
    std::vector<int> graph = { /* ͼ���ڽӱ� */ };
    std::vector<bool> visited(graph.size(), false);
    int startNode = 0;

    // ����Pthread�̲߳���ִ��DFS
    pthread_t thread;
    DFSTask task = { graph, visited, startNode };
    pthread_create(&thread, nullptr, parallelDFS, &task);

    // �ȴ��߳����
    pthread_join(thread, nullptr);

    // ��ӡ���ʹ��Ľڵ�
    for (int i = 0; i < graph.size(); ++i) {
        if (visited[i]) {
            std::cout << "Node " << i << " is visited." << std::endl;
        }
    }

    return 0;
}
