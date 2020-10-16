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

__kernel void CeaserShift(__global char* charArray, __global char16* outputArray) {
    int global_id = get_global_id(0);
    int offset = get_global_offset(0);

    int index = (global_id - offset);
    char16 msg = (char16)(charArray[index], charArray[index+1], charArray[index+2], charArray[index+3], charArray[index+4],
        charArray[index+5], charArray[index+6], charArray[index+7], charArray[index+8], charArray[index+9], charArray[index+10], charArray[index+11],
        charArray[index+12], charArray[index+13], charArray[index+14], charArray[index+15]);

    msg += 5;

    *outputArray = msg;
    
}