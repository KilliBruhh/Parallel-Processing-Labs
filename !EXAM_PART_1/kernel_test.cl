__kernel void matrixMultiplication(__global float* A, __global float* B, __global float* C, int M, int N, int P) {
    int row = get_global_id(0);
    int col = get_global_id(1);
    int localSize = get_local_size(0);
    int localID = get_local_id(0);
    float sum = 0.0f;
    for (int i = 0; i < N; i += localSize) {
        __local float localB[localSize][localSize];
        localB[localID][localID] = B[(i + localID) * P + col];
        barrier(CLK_LOCAL_MEM_FENCE);
        for (int j = 0; j < localSize; ++j) {
            sum += A[row * N + i + j] * localB[j][localID];
        }
        barrier(CLK_LOCAL_MEM_FENCE);
    }
    C[row * P + col] = sum;
}
