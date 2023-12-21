#define LOCAL_SIZE 16

__kernel void matrix_multiplication(
    __global float* A,  // arr
    __global float* B,  // arr2
    __global float* C,  // arr5
    int N
) {
    int row = get_global_id(0);
    int col = get_global_id(1);
    int localId = get_local_id(0);
    int group_size = get_local_size(0);

    float sum = 0.0f;

    __local float local_A[LOCAL_SIZE][LOCAL_SIZE];
    __local float local_B[LOCAL_SIZE][LOCAL_SIZE];

    //for (int i = 0; i < N; i += group_size) {
    for (int i = 0; i < N; i ++) {

        local_B[localId][localId] = B[row * N + i + localId];
        local_A[localId][localId] = A[(i + localId) * N + col];

        barrier(CLK_LOCAL_MEM_FENCE);

        for (int k = 0; k < group_size; k++) {
            // sum += local_A[localId][k] * local_B[k][localId];
            sum += local_A[k][localId] * local_B[localId][k];
        }

        barrier(CLK_LOCAL_MEM_FENCE);
    }

    C[row * N + col] = sum;
}
