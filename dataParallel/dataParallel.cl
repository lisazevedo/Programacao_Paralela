__kernel void dataParallel(
    __global float* A, 
    __global float* B, 
    __global float* C)
{
    int base = 3*get_global_id(0);
    for (int inc=0; inc<3; inc++) {
        C[base + inc] = A[base + inc] + B[base + inc]; 
    }
}