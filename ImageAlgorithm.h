#pragma once

#define MAX_BPP 12
#define MAX_HISTTSIZE (1 << MAX_BPP)

// ͼ���RGB�ṹ��
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

// ����ͼ���ƽ������
double CalculateAverageBrightness(const RGB* img_data, int width, int height);

// ����RGBֱ��ͼ
void CalculateHistogram(const RGB* img_data, int width, int height, int histogram[4][256]);

void CalculateBlockBrightness(const RGB* img_data, int width, int height, int blockWidth, int blockHeight, double** blockBrightness);

//ˮƽ����
void MirrorImageHorizontally(int width, int height, const unsigned char* img_data, unsigned char* out_img);

//��ֱ����
void MirrorImageVertically(int width, int height, const unsigned char* img_data, unsigned char* out_img);

//�ԽǾ���
void MirrorImageDiagonally(int width, int height, const unsigned char* img_data, unsigned char* out_img);

//��ת90��
void RotateImage90(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void ResizeNearestNeighbor(int width, int height, int newWidth, int newHeight, const unsigned char* src, unsigned char* dest);

void ResizeBilinear(int width, int height, const unsigned char* src, int newWidth, int newHeight, unsigned char* dest);

void ResizeBilinearTest(int width, int height, const unsigned char* src, int newWidth, int newHeight, unsigned char* dest);

//gamma�任
void GammaTransform(int width, int height, float gamma, const unsigned char* inputImg, unsigned char* outImg);

//ͼ���Ե���
void EdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void EightEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void ThreeEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void GradientEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void LaplacianEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void SobelEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void PrewittEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

void ScharrEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg);

//ͼ���루��˹�˲���
void GaussianFilter(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//ͼ���루��ֵ�˲���
void MedianFilter(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//ͼ����
void Sharpening(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void RGB2Gray(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//ȫ����ֵ�ָ�
void GlobalThresholdSegmentation(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void OtsuThresholdSegmentation(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void DoubleThresholdSegmentation(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void LocalThresholdSegmentation(int w, int h, const unsigned char* inputImg, unsigned char* outImg, float a, float b);

void DoubleThresholdSegmentation1(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void IterateOtsuSegment(int w, int h, unsigned char* in_img, unsigned char* out_img);

//Ŀ�����
int CountTargets(int w, int h, const unsigned char* inputImg);

//Ŀ�����
int CountTargets1(int w, int h, const unsigned char* inputImg, PTARGET pTarget);

//�߽���ȡ
void BoundaryExtraction(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//��ֵͼ��ʴ
void BinaryCorrosion(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//��ֵͼ������
void BinaryExpansion(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//�Ҷ�ͼ��ʴ
void GrayCorrosion(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//rgbͼ��ʴ
void RGBCorrosion(int w, int h, const unsigned char* inputImg, KERNEL kernel, unsigned char* outImg);

//rgbͼ������
void RGBExpansion(int w, int h, const unsigned char* inputImg, KERNEL kernel, unsigned char* outImg);

//rgbͼ������
void RGBOpening(int w, int h, const unsigned char* inputImg, KERNEL kernel, unsigned char* outImg);

//rgbͼ�������
void RGBClosing(int w, int h, const unsigned char* inputImg, KERNEL kernel, unsigned char* outImg);

//zhang-suenϸ���㷨
void ZhangSuenThinning(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//��˹�˲��Ҷ�ͼ��
void GaussianFilterGray(int w, int h, const unsigned char* inputImg, unsigned char* outImg, float sigma);

//���߶�Retinex�㷨
void SingleScaleRetinex(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//��������
void LinearStretching(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//�ֶ���������
void PiecewiseLinearStretching(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

void log_transform(int w, int h, unsigned char* in_img, unsigned char* out_img);

//�����任
void LogarithmicTransform(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//ֱ��ͼ���⻯
void HistogramEqualization(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//��Ȩֱ��ͼ���⻯
void WeightedHistogramEqualization(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//ֱ��ͼ������
void HistogramSqrt(int w, int h, const unsigned char* inputImg, unsigned char* outImg);

//�ֲ�ֱ��ͼ���⻯
void LocalHistogramEqualization(int w, int h, const unsigned char* inputImg, unsigned char* outImg, int blockWidth, int blockHeight);

//CLAHE
void CLAHE(int w, int h, int blockWidth, int blockHeight, float clipLimit, const unsigned char* inputImg, unsigned char* outImg);