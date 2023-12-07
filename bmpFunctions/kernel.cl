__kernel void boxBlur(__global float* inputImage, __global float* outputImage, const int width, const int height) {
    int gidX = get_global_id(0);
    int gidY = get_global_id(1);
    int blurRadius = 1;
    int index = gidY * width + gidX;
    float blurColor = 0.0f;
    int numPixels = 0;
    for (int i = -blurRadius; i <= blurRadius; ++i) {
        for (int j = -blurRadius; j <= blurRadius; ++j) {
            int currentX = gidX + j;
            int currentY = gidY + i;
            if (currentX >= 0 && currentX < width && currentY >= 0 && currentY < height) {
                int currentIndex = currentY * width + currentX;
                blurColor += inputImage[currentIndex];
                numPixels++;
            }
        }
    }
    outputImage[index] = blurColor / numPixels;
}
