__kernel void img_rotate(
    __global float* src_data, 
    __global float* dest_data,
    int W, int H,               // Image Dimensions
    float sinTheta, float cosTheta // Rotation Parameters
) {
    // Work item gets its index within index space
    const int ix = get_global_id(0); 
    const int iy = get_global_id(1);

    // Calculate location of data to move into (ix,iy)
    float x0 = W / 2.0f;
    float y0 = H / 2.0f;
    float xOff = ix - x0;
    float yOff = iy - y0;

    // Apply the rotation transformation
    int xpos = (int)((xOff * cosTheta) - (yOff * sinTheta) + x0);
    int ypos = (int)((xOff * sinTheta) + (yOff * cosTheta) + y0);

    // Bound checking
    if (((int)xpos >= 0) && ((int)xpos < W) && ((int)ypos >= 0) && ((int)ypos < H)) {
        // Read (ix, iy) src_data and store at (xpos,ypos) in dest_data
        dest_data[iy * W + ix] = src_data[ypos * W + xpos];
    }
}
