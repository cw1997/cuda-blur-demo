__kernel void blurKernel(__global const uchar* input, __global uchar* output,
                         int width, int height, int channels, int radius,
                         __local uchar* shared) {
    int tileW = get_local_size(0) + 2 * radius;
    int tileH = get_local_size(1) + 2 * radius;

    int totalPixels = tileW * tileH;
    int threads = get_local_size(0) * get_local_size(1);
    int tid = get_local_id(1) * get_local_size(0) + get_local_id(0);

    for (int i = tid; i < totalPixels; i += threads) {
        int sx = i % tileW;
        int sy = i / tileW;

        int gx = get_group_id(0) * get_local_size(0) + sx - radius;
        int gy = get_group_id(1) * get_local_size(1) + sy - radius;

        gx = clamp(gx, 0, width - 1);
        gy = clamp(gy, 0, height - 1);

        int srcIdx = (gy * width + gx) * channels;
        int dstIdx = (sy * tileW + sx) * channels;

        shared[dstIdx]     = input[srcIdx];
        shared[dstIdx + 1] = input[srcIdx + 1];
        shared[dstIdx + 2] = input[srcIdx + 2];
    }

    barrier(CLK_LOCAL_MEM_FENCE);

    int x = get_group_id(0) * get_local_size(0) + get_local_id(0);
    int y = get_group_id(1) * get_local_size(1) + get_local_id(1);

    if (x < width && y < height) {
        int sumR = 0, sumG = 0, sumB = 0;
        int sx = get_local_id(0) + radius;
        int sy = get_local_id(1) + radius;

        for (int dy = -radius; dy <= radius; dy++) {
            for (int dx = -radius; dx <= radius; dx++) {
                int idx = ((sy + dy) * tileW + (sx + dx)) * channels;
                sumR += shared[idx];
                sumG += shared[idx + 1];
                sumB += shared[idx + 2];
            }
        }

        int count = (2 * radius + 1) * (2 * radius + 1);
        int dstIdx = (y * width + x) * channels;
        output[dstIdx]     = (uchar)(sumR / count);
        output[dstIdx + 1] = (uchar)(sumG / count);
        output[dstIdx + 2] = (uchar)(sumB / count);
    }
}
