__kernel void helloWorld(__global char *output) {
    int globalId = get_global_id(0);

    if (globalId == 0) {
        output[0] = 'H';
        output[1] = 'e';
        output[2] = 'l';
        output[3] = 'l';
        output[4] = 'o';
        output[5] = ',';
        output[6] = ' ';
        output[7] = 'W';
        output[8] = 'o';
        output[9] = 'r';
        output[10] = 'l';
        output[11] = 'd';
        output[12] = '!';
        output[13] = '\0'; // Null-terminate the string
    }
}