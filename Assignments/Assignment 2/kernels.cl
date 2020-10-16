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

__kernel void CeaserShift(__global char* charArray, __global char* outputArray, __global int *shift) {
    int global_id = get_global_id(0);
    int offset = get_global_offset(0);
    
    int index = (global_id - offset);
    
    int Apos = 65, Zpos = 90;
    char current = charArray[index];
    int remainingShift = *shift;
    //If not A-Z
    if ((int)current > Zpos || (int)current < Apos) {
        outputArray[index] = current;
    }
    else {
        while (remainingShift != 0) {
            if (((int)current + remainingShift) <= Zpos && ((int)current + remainingShift) >= Apos) {
                outputArray[index] = (char)((int)current + remainingShift);
                remainingShift = 0;
                
            } else if (((int)current + remainingShift) > Zpos) {
                int difference = Zpos - (int)current;
                current = Apos - 1;
                remainingShift = remainingShift - difference;
                
            } else if (((int)current + remainingShift) < Apos) {
                int difference = (int)current - Apos;
                current = Zpos + 1;
                remainingShift = remainingShift + difference;
            }
            else {
                outputArray[index] = (char)((int)current + *shift);
                remainingShift = 0;
            }
        }
    }
}

__kernel void CustomEncrypt(__global char* fromChar, __global char* toChar, __global char* charArray, __global char* outputArray) {
    int global_id = get_global_id(0);
    int offset = get_global_offset(0);

    int index = (global_id - offset);
    
    for (int i = 0; i < 26; i++) {
        if (charArray[index] == fromChar[i]) {
            outputArray[index] = toChar[i];
        }
    }
}