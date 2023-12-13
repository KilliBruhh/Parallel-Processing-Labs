__kernel void invertColors(__global uchar* inputImage, __global uchar* outputImage, const int width, const int height) {
    int gidX = get_global_id(0);
    int gidY = get_global_id(1);
    int index = gidY * width + gidX;

    uchar pixelValue = inputImage[index];
    uchar invertedPixelValue = 255 - pixelValue;

    outputImage[index] = invertedPixelValue;
}
