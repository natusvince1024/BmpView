#pragma once

#define MAX_BPP 12
#define MAX_HISTTSIZE (1 << MAX_BPP)

// 图像的RGB结构体
struct RGB {
    unsigned char R;
    unsigned char G;
    unsigned char B;
};

typedef struct Target
{
    int minX;
    int minY;
    int maxX;
    int maxY;
    int area;
}TARGET, *PTARGET;

typedef struct Kernel
{
    int width;
    int height;
    int anchorX;
    int anchorY;
    unsigned char* data;
}KERNEL, *PKERNEL;

// 计算图像的平均亮度
double CalculateAverageBrightness(const RGB* img_data, int width, int height);

// 计算RGB直方图
void CalculateHistogram(const RGB* img_data, int width, int height, int histogram[4][256]);

void CalculateBlockBrightness(const RGB* img_data, int width, int height, int blockWidth, int blockHeight, double** blockBrightness);

//水平镜像
void MirrorImageHorizontally(int width, int height, const unsigned char* img_data, unsigned char* out_img);

//垂直镜像
void MirrorImageVertically(int width, int height, const unsigned char* img_data, unsigned char* out_img);

//对角镜像
void MirrorImageDiagonally(int width, int height, const unsigned char* img_data, unsigned char* out_img);

//旋转90度
void RotateImage90(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void ResizeNearestNeighbor(int width, int height, int newWidth, int newHeight, const unsigned char* src, unsigned char* dest);

void ResizeBilinear(int width, int height, const unsigned char* src, int newWidth, int newHeight, unsigned char* dest);

void ResizeBilinearTest(int width, int height, const unsigned char* src, int newWidth, int newHeight, unsigned char* dest);

//gamma变换
void GammaTransform(int width, int height, float gamma, const unsigned char* inputImg, unsigned char* outImg);

//图像边缘检测
void EdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void EightEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void ThreeEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void GradientEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void LaplacianEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void SobelEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void PrewittEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void ScharrEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

//图像降噪（高斯滤波）
void GaussianFilter(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//图像降噪（中值滤波）
void MedianFilter(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//图像锐化
void Sharpening(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void RGB2Gray(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//全局阈值分割
void GlobalThresholdSegmentation(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void OtsuThresholdSegmentation(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void DoubleThresholdSegmentation(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void LocalThresholdSegmentation(int w, int h, const unsigned char* inputImg, unsigned char* outImg, float a, float b);

void DoubleThresholdSegmentation1(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void IterateOtsuSegment(int w, int h, unsigned char* in_img, unsigned char* out_img);

//目标计数
int CountTargets(int w, int h, const unsigned char* inputImg);

//目标计数
int CountTargets1(int w, int h, const unsigned char* inputImg, PTARGET pTarget);

//边界提取
void BoundaryExtraction(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//二值图像腐蚀
void BinaryCorrosion(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//二值图像膨胀
void BinaryExpansion(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//灰度图像腐蚀
void GrayCorrosion(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//rgb图像腐蚀
void RGBCorrosion(int w, int h, const unsigned char* inputImg, KERNEL kernel, unsigned char* outImg);

//rgb图像膨胀
void RGBExpansion(int w, int h, const unsigned char* inputImg, KERNEL kernel, unsigned char* outImg);

//rgb图像开运算
void RGBOpening(int w, int h, const unsigned char* inputImg, KERNEL kernel, unsigned char* outImg);

//rgb图像闭运算
void RGBClosing(int w, int h, const unsigned char* inputImg, KERNEL kernel, unsigned char* outImg);

//zhang-suen细化算法
void ZhangSuenThinning(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//高斯滤波灰度图像
void GaussianFilterGray(int w, int h, const unsigned char* inputImg, unsigned char* outImg, float sigma);

//单尺度Retinex算法
void SingleScaleRetinex(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//线性拉伸
void LinearStretching(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//分段线性拉伸
void PiecewiseLinearStretching(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void log_transform(int w, int h, unsigned char* in_img, unsigned char* out_img);

//对数变换
void LogarithmicTransform(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//直方图均衡化
void HistogramEqualization(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//加权直方图均衡化
void WeightedHistogramEqualization(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//直方图开根号
void HistogramSqrt(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//局部直方图均衡化
void LocalHistogramEqualization(int w, int h, const unsigned char* inputImg, unsigned char* outImg, int blockWidth, int blockHeight);

//CLAHE
void CLAHE(int w, int h, int blockWidth, int blockHeight, float clipLimit, const unsigned char* inputImg, unsigned char* outImg);