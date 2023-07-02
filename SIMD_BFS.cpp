#include <iostream>
#include <vector>
#include <immintrin.h> // 包含SIMD指令集头文件

void parallelBFS(const std::vector<int>& graph, std::vector<bool>& visited, int startNode) {
    int numNodes = graph.size();
    __m256i queue = _mm256_set1_epi32(startNode); // 使用SIMD向量作为队列
    while (!_mm256_testz_si256(queue, queue)) { // 当队列非空时
        __m256i currentNodeVec = _mm256_set1_epi32(0);
        _mm256_maskstore_epi32(reinterpret_cast<int*>(&currentNodeVec), _mm256_set1_epi32(-1), queue); // 从队列中获取当前节点

        for (int i = 0; i < 8; ++i) { // 处理SIMD向量中的8个节点
            int currentNode = _mm256_extract_epi32(currentNodeVec, i);
            if (!visited[currentNode]) {
                visited[currentNode] = true;
                // 在此处添加对当前节点的处理逻辑

                // 将未访问的邻居节点加入队列
                __m256i neighborNodesVec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&graph[currentNode * numNodes]));
                __m256i unvisitedNeighbors = _mm256_andnot_si256(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(&visited[0])), neighborNodesVec);
                _mm256_maskstore_epi32(reinterpret_cast<int*>(&queue), _mm256_set1_epi32(-1), unvisitedNeighbors);
            }
        }
    }
}

int main() {
    std::vector<int> graph = { /* 图的邻接矩阵或邻接表 */ };
    std::vector<bool> visited(graph.size(), false);
    int startNode = 0;

    parallelBFS(graph, visited, startNode);

    // 打印访问过的节点
    for (int i = 0; i < graph.size(); ++i) {
        if (visited[i]) {
            std::cout << "Node " << i << " is visited." << std::endl;
        }
    }

    return 0;
}
