#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h> // ����Pthread��ͷ�ļ�

// ����BFS����ṹ��
struct BFSTask {
    std::vector<int>& graph;
    std::vector<bool>& visited;
    std::queue<int>& bfsQueue;
    pthread_mutex_t& queueMutex;
};

void bfs(std::vector<int>& graph, std::vector<bool>& visited, std::queue<int>& bfsQueue, pthread_mutex_t& queueMutex) {
    while (!bfsQueue.empty()) {
        pthread_mutex_lock(&queueMutex);
        int currentNode = bfsQueue.front();
        bfsQueue.pop();
        pthread_mutex_unlock(&queueMutex);

        visited[currentNode] = true;
        // ִ��BFS���������罫�ھӽڵ�������
        for (int neighbor : graph[currentNode]) {
            if (!visited[neighbor]) {
                pthread_mutex_lock(&queueMutex);
                bfsQueue.push(neighbor);
                pthread_mutex_unlock(&queueMutex);
            }
        }
    }
}

void* parallelBFS(void* arg) {
    BFSTask* task = static_cast<BFSTask*>(arg);
    std::vector<int>& graph = task->graph;
    std::vector<bool>& visited = task->visited;
    std::queue<int>& bfsQueue = task->bfsQueue;
    pthread_mutex_t& queueMutex = task->queueMutex;

    bfs(graph, visited, bfsQueue, queueMutex);

    pthread_exit(nullptr);
}

int main() {
    std::vector<int> graph = { /* ͼ���ڽӱ� */ };
    std::vector<bool> visited(graph.size(), false);
    std::queue<int> bfsQueue;
    pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
    int startNode = 0;

    bfsQueue.push(startNode);

    // ����Pthread�̲߳���ִ��BFS
    pthread_t thread;
    BFSTask task = { graph, visited, bfsQueue, queueMutex };
    pthread_create(&thread, nullptr, parallelBFS, &task);

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
