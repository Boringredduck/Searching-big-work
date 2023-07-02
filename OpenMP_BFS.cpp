#include <iostream>
#include <queue>
#include <omp.h>

#define NUM_THREADS 4

struct Graph {
    int num_nodes;
    std::vector<std::vector<int>> neighbors;
};

struct SharedData {
    std::vector<bool> visited;
    std::vector<int> distance;
    std::queue<int> queue;
    omp_lock_t queue_lock;
};

void parallel_BFS(Graph& graph, int start_node, SharedData& shared_data) {
    // 将起始节点添加到队列，并标记为已访问
    shared_data.queue.push(start_node);
    shared_data.visited[start_node] = true;
    shared_data.distance[start_node] = 0;

    while (!shared_data.queue.empty()) {
        int num_elements = shared_data.queue.size();

#pragma omp parallel num_threads(NUM_THREADS)
        {
#pragma omp for
            for (int i = 0; i < num_elements; i++) {
                int current_node;

#pragma omp critical
                {
                    current_node = shared_data.queue.front();
                    shared_data.queue.pop();
                }

                // 遍历当前节点的邻居节点
                for (int neighbor_node : graph.neighbors[current_node]) {
                    if (!shared_data.visited[neighbor_node]) {
#pragma omp critical
                        {
                            shared_data.visited[neighbor_node] = true;
                            shared_data.distance[neighbor_node] = shared_data.distance[current_node] + 1;
                            shared_data.queue.push(neighbor_node);
                        }
                    }
                }
            }
        }
    }
}

int main() {
    Graph graph;

    // 初始化图的数据结构

    SharedData shared_data;
    shared_data.visited.resize(graph.num_nodes, false);
    shared_data.distance.resize(graph.num_nodes, -1);
    omp_init_lock(&shared_data.queue_lock);

    parallel_BFS(graph, 0, shared_data);

    // 打印节点距离信息
    for (int i = 0; i < graph.num_nodes; i++) {
        std::cout << "Distance from node 0 to node " << i << ": " << shared_data.distance[i] << std::endl;
    }

    omp_destroy_lock(&shared_data.queue_lock);

    return 0;
}