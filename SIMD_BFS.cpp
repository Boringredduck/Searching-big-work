#include <iostream>
#include <vector>
#include <immintrin.h> // ����SIMDָ�ͷ�ļ�

void parallelBFS(const std::vector<int>& graph, std::vector<bool>& visited, int startNode) {
    int numNodes = graph.size();
    __m256i queue = _mm256_set1_epi32(startNode); // ʹ��SIMD������Ϊ����
    while (!_mm256_testz_si256(queue, queue)) { // �����зǿ�ʱ
        __m256i currentNodeVec = _mm256_set1_epi32(0);
        _mm256_maskstore_epi32(reinterpret_cast<int*>(&currentNodeVec), _mm256_set1_epi32(-1), queue); // �Ӷ����л�ȡ��ǰ�ڵ�

        for (int i = 0; i < 8; ++i) { // ����SIMD�����е�8���ڵ�
            int currentNode = _mm256_extract_epi32(currentNodeVec, i);
            if (!visited[currentNode]) {
                visited[currentNode] = true;
                // �ڴ˴���ӶԵ�ǰ�ڵ�Ĵ����߼�

                // ��δ���ʵ��ھӽڵ�������
                __m256i neighborNodesVec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&graph[currentNode * numNodes]));
                __m256i unvisitedNeighbors = _mm256_andnot_si256(_mm256_loadu_si256(reinterpret_cast<const __m256i*>(&visited[0])), neighborNodesVec);
                _mm256_maskstore_epi32(reinterpret_cast<int*>(&queue), _mm256_set1_epi32(-1), unvisitedNeighbors);
            }
        }
    }
}

int main() {
    std::vector<int> graph = { /* ͼ���ڽӾ�����ڽӱ� */ };
    std::vector<bool> visited(graph.size(), false);
    int startNode = 0;

    parallelBFS(graph, visited, startNode);

    // ��ӡ���ʹ��Ľڵ�
    for (int i = 0; i < graph.size(); ++i) {
        if (visited[i]) {
            std::cout << "Node " << i << " is visited." << std::endl;
        }
    }

    return 0;
}
