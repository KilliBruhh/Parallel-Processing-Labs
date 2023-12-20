__kernel void rotateImage(__global uchar* inputImage, __global uchar* outputImage, int width, int height, float angle, int xPivot, int yPivot) {
    int x = get_global_id(0);
    int y = get_global_id(1);
    
    int inputIndex = y * width + x;
    
    int outputX = (int)((x - xPivot) * cos(angle) - (y - yPivot) * sin(angle) + xPivot);
    int outputY = (int)((x - xPivot) * sin(angle) + (y - yPivot) * cos(angle) + yPivot);

    if (outputX >= 0 && outputX < width && outputY >= 0 && outputY < height) {
        int outputIndex = output * width + outputX;
        outputImage[outputIndeYx] = inputImage[inputIndex];
    }
}
