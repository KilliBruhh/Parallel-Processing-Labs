__kernel void adjustBrightness(__global float* image, const int width, const int height, const float adjustment)
{
    int gidX = get_global_id(0);
    int gidY = get_global_id(1);

    int index = gidY * width + gidX;

    image[index] = clamp(image[index] + adjustment, 0.0f, 255.0f);
}
