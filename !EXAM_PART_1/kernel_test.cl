#define LOCAL_SIZE 16

__kernel void matrix_multiplication(
    __global float* A,
    __global float* B,
    __global float* C,
    int N
) {
    int row = get_global_id(0);
    int col = get_global_id(1);
    int localId = get_local_id(0);
    int group_size = get_local_size(0);

    float sum = 0.0f;

    __local float local_A[LOCAL_SIZE][LOCAL_SIZE];
    __local float local_B[LOCAL_SIZE][LOCAL_SIZE];

    for (int i = 0; i < N; i += group_size) {

        local_A[localId][localId] = A[row * N + i + localId];
        local_B[localId][localId] = B[(i + localId) * N + col];

        barrier(CLK_LOCAL_MEM_FENCE);

        for (int k = 0; k < group_size; k++) {
            sum += local_A[localId][k] * local_B[k][localId];
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    C[row * N + col] = sum;
}
