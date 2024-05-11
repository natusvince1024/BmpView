#include "pch.h"
#include "DenoisingAlgorithm.h"


#define MAX_KERNEL_SIZE 100
double gaussianSpatialLUT[MAX_KERNEL_SIZE];
//double gaussianRangeLUT[256];
double gaussianRangeLUT[4096];

inline unsigned short saturate_12bit(int v)
{
    if (v > 4095) v = 4095;
    else if (v < 0) v = 0;
    return v;
}

//// 创建空间高斯查找表
//void createGaussianSpatialLUT(double sigma, int radius) 
//{
//    for (int i = 0; i <= radius; i++) {
//        gaussianSpatialLUT[i] = exp(-(i * i) / (2.0 * sigma * sigma));
//    }
//}
//
//// 创建强度高斯查找表
//void createGaussianRangeLUT(double sigma) 
//{
//    for (int i = 0; i < 256; i++) {
//        gaussianRangeLUT[i] = exp(-(i * i) / (2.0 * sigma * sigma));
//    }
//}
//
//// 双边滤波
//void bilateralFilter(unsigned char* src, unsigned char* dst, int width, int height, int radius, double sigmaI, double sigmaS) 
//{
//    createGaussianSpatialLUT(sigmaS, radius);
//    createGaussianRangeLUT(sigmaI);
//
//    for (int i = 0; i < height; i++) {
//        for (int j = 0; j < width; j++) {
//            double val = 0.0;
//            double sum = 0.0;
//            for (int k = -radius; k <= radius; k++) {
//                for (int l = -radius; l <= radius; l++) {
//                    int x = j + l;
//                    int y = i + k;
//                    if (x >= 0 && x < width && y >= 0 && y < height) {
//                        double gs = gaussianSpatialLUT[abs(k)] * gaussianSpatialLUT[abs(l)];
//                        int intensityDiff = abs(src[y * width + x] - src[i * width + j]);
//                        double gr = gaussianRangeLUT[intensityDiff];
//                        double w = gs * gr;
//                        val += src[y * width + x] * w;
//                        sum += w;
//                    }
//                }
//            }
//            dst[i * width + j] = (unsigned char)(sum == 0 ? 0 : (val / sum));
//        }
//    }
//}


// 创建空间高斯查找表
void createGaussianSpatialLUT(double sigma, int radius)
{
    for (int i = 0; i <= radius; i++) {
        gaussianSpatialLUT[i] = exp(-(i * i) / (2.0 * sigma * sigma));
    }
}

// 创建强度高斯查找表
void createGaussianRangeLUT(double sigma)
{
    for (int i = 0; i < 4096; i++) {
        gaussianRangeLUT[i] = exp(-(i * i) / (2.0 * sigma * sigma));
    }
}

void bilateralFilter(int width, int height, int radius, double sigmaI, double sigmaS, unsigned short* inputImg, unsigned short* outputImg)
{
    createGaussianSpatialLUT(sigmaS, radius);
    createGaussianRangeLUT(sigmaI);

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            double val = 0.0;
            double sum = 0.0;
            for (int k = -radius; k <= radius; k++) {
                for (int l = -radius; l <= radius; l++) {
                    int x = j + l;
                    int y = i + k;
                    if (x >= 0 && x < width && y >= 0 && y < height) {
                        double gs = gaussianSpatialLUT[abs(k)] * gaussianSpatialLUT[abs(l)];
                        int intensityDiff = abs(inputImg[y * width + x] - inputImg[i * width + j]);
                        double gr = gaussianRangeLUT[intensityDiff];
                        double w = gs * gr;
                        val += inputImg[y * width + x] * w;
                        sum += w;
                    }
                }
            }
            outputImg[i * width + j] = saturate_12bit(sum == 0 ? 0 : (val / sum));
        }
    }

}

