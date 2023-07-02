#include <iostream>
#include <vector>
#include <pthread.h> // 包含Pthread库头文件

// 定义DFS任务结构体
struct DFSTask {
    std::vector<int>& graph;
    std::vector<bool>& visited;
    int startNode;
};

void dfs(std::vector<int>& graph, std::vector<bool>& visited, int currentNode) {
    visited[currentNode] = true;
    // 执行DFS操作，例如遍历邻居节点
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
    std::vector<int> graph = { /* 图的邻接表 */ };
    std::vector<bool> visited(graph.size(), false);
    int startNode = 0;

    // 创建Pthread线程并行执行DFS
    pthread_t thread;
    DFSTask task = { graph, visited, startNode };
    pthread_create(&thread, nullptr, parallelDFS, &task);

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
