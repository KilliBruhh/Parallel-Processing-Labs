__kernel void matrix_avarages(
    __global float* A,
    __global float* B,
    __global float* C,
    int N
) {
    int row = get_global_id(0);
    int col = get_global_id(1);
    float sum = 0.0f;

    for(int i = 0; i < N; i++) {
        sum += A[row * N + i] + B[i * N + col];
    }

    C[row * N + col] = sum;
}