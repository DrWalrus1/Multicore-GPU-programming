__kernel void add(__global int *a, __global int *b) {
    int global_id = get_global_id(0);
    int offset = get_global_offset(0);

    int index = (global_id - offset + 1);

    b[index] = 3 + (index * *a);
    b[index+1] = 3 + (index+1 * *a);
    b[index+2] = 3 + (index+2 * *a);
    b[index+3] = 3 + (index+3 * *a);
}

__kernel void task1b(__global int4* vec1, __global int* vec2, __global int* output) {
    
}