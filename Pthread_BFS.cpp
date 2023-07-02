#include <iostream>
#include <vector>
#include <queue>
#include <pthread.h> // 包含Pthread库头文件

// 定义BFS任务结构体
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
        // 执行BFS操作，例如将邻居节点加入队列
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
    std::vector<int> graph = { /* 图的邻接表 */ };
    std::vector<bool> visited(graph.size(), false);
    std::queue<int> bfsQueue;
    pthread_mutex_t queueMutex = PTHREAD_MUTEX_INITIALIZER;
    int startNode = 0;

    bfsQueue.push(startNode);

    // 创建Pthread线程并行执行BFS
    pthread_t thread;
    BFSTask task = { graph, visited, bfsQueue, queueMutex };
    pthread_create(&thread, nullptr, parallelBFS, &task);

    // 等待线程完成
    pthread_join(thread, nullptr);

    // 打印访问过的节点
    for (int i = 0; i < graph.size(); ++i) {
        if (visited[i]) {
            std::cout << "Node " << i << " is visited." << std::endl;
        }
    }

    return 0;
}
