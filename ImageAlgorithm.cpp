#include "pch.h"
#include "ImageAlgorithm.h"
#include "CProcessedImageDialog.h"

#define MAX_WIDTH 3840
#define SHIFT_AMOUNT 10
#define SHIFT_MASK ((1 << SHIFT_AMOUNT) - 1)
#define FIXED_POINT_MULTIPLIER (1 << SHIFT_AMOUNT)
#define SHIFT_ROUNDING_BIT 20
#define rgbToy(r, g , b) (unsigned char)((r * 77 + g * 150 + b * 29) >> 8)
#define NEIGHBORHOOD_SIZE  3



//计算平均亮度
double CalculateAverageBrightness(const RGB* img_data, int width, int height) {
    long long sumBrightness = 0;
    int total_pixels = width * height;

    for (int i = 0; i < total_pixels; i++) {
        sumBrightness += (img_data[i].R + img_data[i].G + img_data[i].B) / 3;
    }

    return static_cast<double>(sumBrightness) / total_pixels;
}

void CalculateHistogram(const RGB* img_data, int width, int height, int histogram[4][256]) {
    // 初始化直方图
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 256; j++) {
            histogram[i][j] = 0;
        }
    }

    int total_pixels = width * height;
    //定义三个指针分别指向三个直方图
    int* phR = histogram[0];
    int* phG = histogram[1];
    int* phB = histogram[2];
    int* phBrightness = histogram[3];
    const RGB* prgb = img_data;

    for (int i = 0; i < total_pixels; i++) {
        phR[prgb->R]++;
        phG[prgb->G]++;
        phB[prgb->B]++;

        // 计算亮度并增加频次
        int brightness = (299 * prgb->R + 587 * prgb->G + 114 * prgb->B + 500) / 1000;
        phBrightness[brightness]++;

        prgb++;
    }
}

void CalculateBlockBrightness(const RGB* img_data, int width, int height, int blockWidth, int blockHeight, double** blockBrightness) {
    int blocksX = width / blockWidth;  // 计算X方向上的块数
    int blocksY = height / blockHeight;  // 计算Y方向上的块数

    for (int y = 0; y < blocksY; y++) {
        for (int x = 0; x < blocksX; x++) {
            long long sumBrightness = 0;  // 存储当前块的亮度总和
            int pixels = 0;  // 存储当前块的像素总数

            for (int j = y * blockHeight; j < (y + 1) * blockHeight; j++) {
                for (int i = x * blockWidth; i < (x + 1) * blockWidth; i++) {
                    int index = j * width + i;  // 计算像素索引
                    sumBrightness += (img_data[index].R + img_data[index].G + img_data[index].B) / 3;
                    pixels++;
                }
            }

            blockBrightness[y][x] = (double)sumBrightness / pixels;  // 计算当前块的平均亮度
        }
    }
}

void MirrorImageHorizontally(int width, int height, const unsigned char* img_data, unsigned char* out_img)
{
    int channels = 3;  // RGB图像3个通道

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int originalIndex = (y * width + x) * channels;
            int mirroredIndex = (y * width + (width - x - 1)) * channels;

            out_img[mirroredIndex] = img_data[originalIndex];
            out_img[mirroredIndex + 1] = img_data[originalIndex + 1];
            out_img[mirroredIndex + 2] = img_data[originalIndex + 2];
        }
    }
}

void MirrorImageVertically(int width, int height, const unsigned char* img_data, unsigned char* out_img)
{
    int channels = 3;  // RGB图像3个通道

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int originalIndex = (y * width + x) * channels;
            int mirroredIndex = ((height - y - 1) * width + x) * channels;

            out_img[mirroredIndex] = img_data[originalIndex];
            out_img[mirroredIndex + 1] = img_data[originalIndex + 1];
            out_img[mirroredIndex + 2] = img_data[originalIndex + 2];
        }
    }
}

void MirrorImageDiagonally(int width, int height, const unsigned char* img_data, unsigned char* out_img)
{
    int channels = 3;  // RGB图像3个通道

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int originalIndex = (y * width + x) * channels;
            int mirroredIndex = ((height - y - 1) * width + (width - x - 1)) * channels;

            out_img[mirroredIndex] = img_data[originalIndex];
            out_img[mirroredIndex + 1] = img_data[originalIndex + 1];
            out_img[mirroredIndex + 2] = img_data[originalIndex + 2];
        }
    }
}

void RotateImage90(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    int newRow, newCol;
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            newRow = x;
            newCol = h - 1 - y;

            int inputIndex = (y * w + x) * 3;
            int outputIndex = (newRow * h + newCol) * 3;

            outImg[outputIndex] = inputImg[inputIndex];
            outImg[outputIndex + 1] = inputImg[inputIndex + 1];
            outImg[outputIndex + 2] = inputImg[inputIndex + 2];
        } 
    }
}

void ResizeNearestNeighbor(int width, int height, int newWidth, int newHeight, const unsigned char* src, unsigned char* dest)
{
    // 计算水平和垂直缩放比例
    float x_ratio = (float)(width) / newWidth;  
    float y_ratio = (float)(height) / newHeight;

    for (int y = 0; y < newHeight; y++)
    {
        for (int x = 0; x < newWidth; x++)
        {
            // 计算源图像中的位置
            int nearestX = (int)(x * x_ratio);
            int nearestY = (int)(y * y_ratio);

            // 计算目标和源图像中的像素索引
            int destIndex = (y * newWidth + x) * 3;
            int srcIndex = (nearestY * width + nearestX) * 3;

            // 复制 RGB 分量
            dest[destIndex] = src[srcIndex];       // R 分量
            dest[destIndex + 1] = src[srcIndex + 1]; // G 分量
            dest[destIndex + 2] = src[srcIndex + 2]; // B 分量
        }
    }
}

// 双线性插值(最终版)
void ResizeBilinear(int width, int height, const unsigned char* src, int newWidth, int newHeight, unsigned char* dest)
{
    int x_ratio = (width << 10) / newWidth;//计算水平和垂直缩放比例
    int y_ratio = (height << 10) / newHeight;

    int* x_ratio_table = (int*)malloc(newWidth * sizeof(int));
    int* x_frac_table = (int*)malloc(newWidth * sizeof(int));
    if (x_ratio_table == NULL || x_frac_table == NULL) {
        return;
    }

    for (int x = 0; x < newWidth; x++)
    {
        int valx = x * x_ratio;
		x_ratio_table[x] = valx >> SHIFT_AMOUNT;
        x_frac_table[x] = valx & SHIFT_MASK;
	}

    int w3 = width * 3;
    int x_offset = 0, y_offset = 0;//计算源图像中的位置
    int srcIndex = 0, destIndex = 0;//计算目标和源图像中的像素索引
    int x_frac = 0, y_frac = 0;//计算源图像中的位置
    int r_val1 =0, r_val2 = 0, g_val1 = 0, g_val2 = 0, b_val1 = 0, b_val2 = 0;//计算源图像中的位置

    int bottomEdgeIndex = (newHeight - 1) * newWidth * 3;
    int rightEdgeIndex = (newWidth - 1) * 3;
    int srcBottomEdgeIndex = (height - 1) * w3;

    // 处理边界像素
    for (int x = 0; x < newWidth; x++)
    {
        x_offset = x_ratio_table[x];
        srcIndex = srcBottomEdgeIndex + x_offset * 3;
        destIndex = bottomEdgeIndex + x * 3;

        //底部边缘
        dest[destIndex] = src[srcIndex];
        dest[destIndex + 1] = src[srcIndex + 1];
        dest[destIndex + 2] = src[srcIndex + 2];
    }

    for (int y = 0; y < newHeight; y++)
    {
        y_offset = (y * y_ratio) >> SHIFT_AMOUNT;
        srcIndex = (y_offset * width + (width - 1)) * 3;
        destIndex = y * newWidth * 3 + rightEdgeIndex;

        // 右边缘
        dest[destIndex] = src[srcIndex];
        dest[destIndex + 1] = src[srcIndex + 1];
        dest[destIndex + 2] = src[srcIndex + 2];
    }

    int x_compt = 0, y_compt = 0;
    // 处理内部像素
    for (int y = 0; y < newHeight - 1; y++)
    {
        y_offset = (y * y_ratio) >> SHIFT_AMOUNT;
        y_frac = (y * y_ratio) & SHIFT_MASK;
        y_compt = 1024 - y_frac;

        for (int x = 0; x < newWidth - 1; x++)
        {
            x_offset = x_ratio_table[x];
            x_frac = x_frac_table[x];
            x_compt = 1024 - x_frac;

            srcIndex = ((y_offset * width) + x_offset) * 3;//计算源图像中的像素索引
            destIndex = (y * newWidth + x) * 3;//计算目标和源图像中的像素索引
            int Index1 = srcIndex + w3;

            // Red channel
            r_val1 = x_compt * src[srcIndex] + x_frac * src[srcIndex + 3];
            r_val2 = x_compt * src[srcIndex + w3] + x_frac * src[Index1 + 3];
            dest[destIndex] = (unsigned char)((y_compt * r_val1 + y_frac * r_val2) >> SHIFT_ROUNDING_BIT);

            // Green channel
            g_val1 = x_compt * src[srcIndex + 1] + x_frac * src[srcIndex + 4];
            g_val2 = x_compt * src[srcIndex + w3 + 1] + x_frac * src[Index1 + 4];
            dest[destIndex + 1] = (unsigned char)((y_compt * g_val1 + y_frac * g_val2) >> SHIFT_ROUNDING_BIT);
            // Blue channel
            b_val1 = x_compt * src[srcIndex + 2] + x_frac * src[srcIndex + 5];
            b_val2 = x_compt * src[srcIndex + w3 + 2] + x_frac * src[Index1 + 5];
            dest[destIndex + 2] = (unsigned char)((y_compt * b_val1 + y_frac * b_val2) >> SHIFT_ROUNDING_BIT);
        }
    }

    free(x_ratio_table);
    free(x_frac_table);
}

void ResizeBilinearTest(int width, int height, const unsigned char* src, int newWidth, int newHeight, unsigned char* dest)
{
    float x_ratio = (float)(width - 1) / newWidth;
    float y_ratio = (float)(height - 1) / newHeight;

    for (int y = 0; y < newHeight; y++)
    {
        for (int x = 0; x < newWidth; x++)
        {
            float x_src = x * x_ratio;
            float y_src = y * y_ratio;

            int x_int = (int)x_src;
            int y_int = (int)y_src;

            float x_frac = x_src - x_int;
            float y_frac = y_src - y_int;

            int srcIndex1 = (y_int * width + x_int) * 3;
            int srcIndex2 = (y_int * width + x_int + 1) * 3;
            int srcIndex3 = ((y_int + 1) * width + x_int) * 3;
            int srcIndex4 = ((y_int + 1) * width + x_int + 1) * 3;

            for (int c = 0; c < 3; c++)  // 循环处理 RGB 三个通道
            {
                float val1 = src[srcIndex1 + c];
                float val2 = src[srcIndex2 + c];
                float val3 = src[srcIndex3 + c];
                float val4 = src[srcIndex4 + c];

                float val = val1 * (1 - x_frac) * (1 - y_frac)
                    + val2 * x_frac * (1 - y_frac)
                    + val3 * (1 - x_frac) * y_frac
                    + val4 * x_frac * y_frac;

                int destIndex = (y * newWidth + x) * 3;
                dest[destIndex + c] = (unsigned char)val;
            }
        }
    }
}

void GammaTransform(int width, int height, float gamma, const unsigned char* inputImg, unsigned char* outImg)
{
    unsigned char gammaTable[256];
    float gammaCorrection = 1.0f / gamma;//计算伽马校正值

    for (int i = 0; i < 256; i++)
    {
        gammaTable[i] = (unsigned char)(pow(i / 255.0f, gammaCorrection) * 255.0f);
    }

    int totalPixels = width * height * 3;

    for(int i = 0; i < totalPixels; i++)
	{
		outImg[i] = gammaTable[inputImg[i]];
	}
}

void EdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg)
{
    for (int y = 0; y < height - 1; y++)
    {
        for (int x = 0; x < width - 1; x++)
        {
            int value = inputImg[(y * width + x) * 3];
            int value1 = inputImg[(y * width + x + 1) * 3];//右边像素
            int value2 = inputImg[((y + 1) * width + x) * 3];//下边像素

            int gradientX = abs(value - value1);
            int gradientY = abs(value - value2);
            int gradient = gradientX + gradientY;

            if (20 >= gradient)
            {
                outImg[(y * width + x) * 3] = 0;
                outImg[(y * width + x) * 3 + 1] = 0;
                outImg[(y * width + x) * 3 + 2] = 0;
            }
			else
			{
				outImg[(y * width + x) * 3] = 255;
				outImg[(y * width + x) * 3 + 1] = 255;
				outImg[(y * width + x) * 3 + 2] = 255;
			}

        }
    }
}

void EightEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg)
{
    //设置阈值
    int threshold = 0;

    for (int y = 0; y < height - 1; y++)
    {
        for (int x = 0; x < width - 1; x++)
        {
            int value = inputImg[(y * width + x) * 3];
            int value1 = inputImg[(y * width + x + 1) * 3];//右边像素
            int value2 = inputImg[((y + 1) * width + x) * 3];//下边像素
            int value3 = inputImg[((y + 1) * width + x + 1) * 3];//右下角像素
            int value4 = inputImg[((y + 1) * width + x - 1) * 3];//左下角像素
            int value5 = inputImg[(y * width + x - 1) * 3];//左边像素
            int value6 = inputImg[((y - 1) * width + x) * 3];//上边像素
            int value7 = inputImg[((y - 1) * width + x + 1) * 3];//右上角像素
            int value8 = inputImg[((y - 1) * width + x - 1) * 3];//左上角像素

            // 计算水平和垂直方向上的梯度
            int gradientX = abs(value - value1) + abs(value2 - value3) + abs(value4 - value5) + abs(value6 - value7);
            int gradientY = abs(value - value2) + abs(value1 - value3) + abs(value5 - value4) + abs(value6 - value8);

            // 计算梯度的平方和
            int gradient = gradientX * gradientX + gradientY * gradientY;

            // 计算梯度的平方根
            gradient = (int)sqrt(gradient);

            // 如果梯度大于阈值，则将像素设置为黑色，否则设置为白色
            if (gradient > threshold)
			{
				outImg[(y * width + x) * 3] = 0;
				outImg[(y * width + x) * 3 + 1] = 0;
				outImg[(y * width + x) * 3 + 2] = 0;
			}
			else
			{
				outImg[(y * width + x) * 3] = 255;
				outImg[(y * width + x) * 3 + 1] = 255;
				outImg[(y * width + x) * 3 + 2] = 255;
			}
        }
    }
}

void ThreeEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg)
{
    int totalPixels = width * height * 3;
    //设置阈值
    int threshold = 10;

    for (int i = 0; i < totalPixels; i++)
    {
        int value = inputImg[i];
        int value1 = inputImg[i + 3];
        int value2 = inputImg[i + width * 3];
        int value3 = inputImg[i + width * 3 + 3];

        // 计算水平和垂直方向上的梯度
        int gradientX = abs(value - value1) + abs(value2 - value3);
        int gradientY = abs(value - value2) + abs(value1 - value3);

        // 计算梯度的平方和
        int gradient = gradientX * gradientX + gradientY * gradientY;

        // 计算梯度的平方根
        gradient = (int)sqrt(gradient);

        // 如果梯度大于阈值，则将像素设置为黑色，否则设置为白色
        if (gradient > threshold)
        {
            outImg[i] = 0;
        }
	    else
		{
			outImg[i] = 255;
		}
    }
}

void GradientEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg)
{
    int totalPixels = width * height * 3;
    //设置阈值
    int threshold = 0;

    for(int i = 0; i < totalPixels; i++)
	{
	    // 计算当前像素的索引
		int value = inputImg[i];
        int value1 = inputImg[i + 3];
        int value2 = inputImg[i + width * 3];
        
        // 计算水平和垂直方向上的梯度
        int gradientX = abs(value - value1);
        int gradientY = abs(value - value2);

        // 计算梯度的平方和
        int gradient = gradientX * gradientX + gradientY * gradientY;

        // 计算梯度的平方根
        gradient = (int)sqrt(gradient);

        // 如果梯度大于阈值，则将像素设置为黑色，否则设置为白色
        if(gradient > threshold)
		{
			outImg[i] = 0;
		}
		else
		{
			outImg[i] = 255;
		}
	}
}

void LaplacianEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg)
{
    int kernel[3][3] = 
    {
		    { -1, -1, -1 },
		    { -1,  8, -1 },
		    { -1, -1, -1 }
	};

    for (int y = 1; y < height - 1; y++) 
    {
		    for (int x = 1; x < width - 1; x++) 
            {
			    int sumR = 0;
			    int sumG = 0;
			    int sumB = 0;

			    // 循环处理卷积核中的每个元素
			    for (int j = -1; j <= 1; j++) 
                {
				    for (int i = -1; i <= 1; i++) 
                    {
					    // 计算卷积核中的像素索引
					    int index = ((y + j) * width + (x + i)) * 3;

					    // 计算卷积核中的像素值
					    int kernelValue = kernel[j + 1][i + 1];

					    // 累加卷积核中的像素值
					    sumR += inputImg[index] * kernelValue;
					    sumG += inputImg[index + 1] * kernelValue;
					    sumB += inputImg[index + 2] * kernelValue;
				    }
			    }

			    // 计算输出图像中的像素索引
			    int outIndex = (y * width + x) * 3;

                outImg[outIndex] = (unsigned char)max(0, min(255, sumR));
			    outImg[outIndex + 1] = (unsigned char)max(0, min(255, sumG));
			    outImg[outIndex + 2] = (unsigned char)max(0, min(255, sumB));
		    }
	}

    // 处理图像的边缘
    for (int y = 0; y < height; y++) 
    {
        for (int x = 0; x < width; x++) 
        {
		    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) 
            {
			    int index = (y * width + x) * 3;
				outImg[index] = 0;
				outImg[index + 1] = 0;
				outImg[index + 2] = 0;
			}
		}
	}
}

void SobelEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg) {
    int gx[3][3] = {
        { -1, 0, 1 },
        { -2, 0, 2 },
        { -1, 0, 1 }
    };
    int gy[3][3] = {
        { -1, -2, -1 },
        {  0,  0,  0 },
        {  1,  2,  1 }
    };

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sumX = 0, sumY = 0;

            for (int j = -1; j <= 1; j++) {
                for (int i = -1; i <= 1; i++) {
                    int index = ((y + j) * width + (x + i)) * 3;

                    int r = inputImg[index];
                    int g = inputImg[index + 1];
                    int b = inputImg[index + 2];
                    double gray = 0.299 * r + 0.587 * g + 0.114 * b;

                    sumX += gx[j + 1][i + 1] * gray;
                    sumY += gy[j + 1][i + 1] * gray;
                }
            }

            int magnitude = sqrt(sumX * sumX + sumY * sumY);
            magnitude = max(0, min(255, magnitude));

            int outIndex = (y * width + x) * 3;
            outImg[outIndex] = magnitude;
            outImg[outIndex + 1] = magnitude;
            outImg[outIndex + 2] = magnitude;
        }
    }
}

void PrewittEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg) {
    int gx[3][3] = {
        { -1, 0, 1 },
        { -1, 0, 1 },
        { -1, 0, 1 }
    };
    int gy[3][3] = {
        { -1, -1, -1 },
        {  0,  0,  0 },
        {  1,  1,  1 }
    };

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sumX = 0, sumY = 0;

            for (int j = -1; j <= 1; j++) {
                for (int i = -1; i <= 1; i++) {
                    int index = ((y + j) * width + (x + i)) * 3;

                    int r = inputImg[index];
                    int g = inputImg[index + 1];
                    int b = inputImg[index + 2];
                    int gray = 0.299 * r + 0.587 * g + 0.114 * b;

                    sumX += gx[j + 1][i + 1] * gray;
                    sumY += gy[j + 1][i + 1] * gray;
                }
            }

            int magnitude = sqrt(sumX * sumX + sumY * sumY);
            magnitude = max(0, min(255, magnitude));

            int outIndex = (y * width + x) * 3;
            outImg[outIndex] = magnitude;
            outImg[outIndex + 1] = magnitude;
            outImg[outIndex + 2] = magnitude;
        }
    }
}

void ScharrEdgeDetection(int width, int height, const unsigned char* inputImg, unsigned char* outImg) {
    int gx[3][3] = {
        { -3, 0, 3 },
        { -10, 0, 10 },
        { -3, 0, 3 }
    };
    int gy[3][3] = {
        { -3, -10, -3 },
        { 0, 0, 0 },
        { 3, 10, 3 }
    };

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sumX = 0, sumY = 0;

            for (int j = -1; j <= 1; j++) {
                for (int i = -1; i <= 1; i++) {
                    int index = ((y + j) * width + (x + i)) * 3;

                    int r = inputImg[index];
                    int g = inputImg[index + 1];
                    int b = inputImg[index + 2];
                    double gray = 0.299 * r + 0.587 * g + 0.114 * b;

                    sumX += gx[j + 1][i + 1] * gray;
                    sumY += gy[j + 1][i + 1] * gray;
                }
            }

            int magnitude = sqrt(sumX * sumX + sumY * sumY);
            magnitude = max(0, min(255, magnitude));

            int outIndex = (y * width + x) * 3;
            outImg[outIndex] = magnitude;
            outImg[outIndex + 1] = magnitude;
            outImg[outIndex + 2] = magnitude;
        }
    }
}

void GaussianFilter(int w, int h, const unsigned char* inputImg, unsigned char* outImg) {
    int w3 = w * 3;
    
    //高斯核
    int kernel[9] = { 1, 2, 1, 
                      2, 4, 2, 
                      1, 2, 1 };
    int offsets[9] = { -w3 - 3, -w3, -w3 + 3, 
                       -3, 0, 3, 
                       w3 - 3, w3, w3 + 3 };

    for (int y = 1; y < h - 1; y++) {
        for (int x = 1; x < w - 1; x++) {
            int sumR = 0;
            int sumG = 0;
            int sumB = 0;

            int centerIndex = (y * w + x) * 3;

            int indexTL = centerIndex + offsets[0];
            int indexTC = centerIndex + offsets[1];
            int indexTR = centerIndex + offsets[2];
            int indexCL = centerIndex + offsets[3];
            int indexCC = centerIndex + offsets[4];
            int indexCR = centerIndex + offsets[5];
            int indexBL = centerIndex + offsets[6];
            int indexBC = centerIndex + offsets[7];
            int indexBR = centerIndex + offsets[8];

            sumR += inputImg[indexTL] + (inputImg[indexTC] << 1) + inputImg[indexTR] +
                    (inputImg[indexCL] << 1) + (inputImg[indexCC] << 2) + (inputImg[indexCR] << 1) +
                    inputImg[indexBL] + (inputImg[indexBC] << 1) + inputImg[indexBR];

            sumG += inputImg[indexTL + 1] + (inputImg[indexTC + 1] << 1) + inputImg[indexTR + 1] +
                    (inputImg[indexCL + 1] << 1) + (inputImg[indexCC + 1] << 2) + (inputImg[indexCR + 1] << 1) +
					inputImg[indexBL + 1] + (inputImg[indexBC + 1] << 1) + inputImg[indexBR + 1];

            sumB += inputImg[indexTL + 2] + (inputImg[indexTC + 2] << 1) + inputImg[indexTR + 2] +
					(inputImg[indexCL + 2] << 1) + (inputImg[indexCC + 2] << 2) + (inputImg[indexCR + 2] << 1) +
                    inputImg[indexBL + 2] + (inputImg[indexBC + 2] << 1) + inputImg[indexBR + 2];

            int outIndex = (y * w + x) * 3;

            outImg[outIndex] = (unsigned char)(sumR >> 4);
            outImg[outIndex + 1] = (unsigned char)(sumG >> 4);
            outImg[outIndex + 2] = (unsigned char)(sumB >> 4);
        }
    }

    // 处理边界
    for (int x = 0; x < w; x++) {
        int indexTop = x * 3;  // 顶部边界
        int indexBottom = (h - 1) * w3 + x * 3;  // 底部边界

        outImg[indexTop] = inputImg[indexTop];
        outImg[indexTop + 1] = inputImg[indexTop + 1];
        outImg[indexTop + 2] = inputImg[indexTop + 2];

        outImg[indexBottom] = inputImg[indexBottom];
        outImg[indexBottom + 1] = inputImg[indexBottom + 1];
        outImg[indexBottom + 2] = inputImg[indexBottom + 2];
    }

    for (int y = 0; y < h; y++) {
        int indexLeft = y * w3;  // 左边界
        int indexRight = (y + 1) * w3 - 3;  // 右边界

        outImg[indexLeft] = inputImg[indexLeft];
        outImg[indexLeft + 1] = inputImg[indexLeft + 1];
        outImg[indexLeft + 2] = inputImg[indexLeft + 2];

        outImg[indexRight] = inputImg[indexRight];
        outImg[indexRight + 1] = inputImg[indexRight + 1];
        outImg[indexRight + 2] = inputImg[indexRight + 2];
    }
}

inline int InsertionSort(unsigned char* array)
{
    int length = 9;
    for (int i = 1; i < length; i++)
    {
		unsigned char temp = array[i];
        int j = i - 1;
        while (j >= 0 && array[j] > temp)
        {
			array[j + 1] = array[j];
			j--;
		}
        array[j + 1] = temp;
	}
    return array[4];
}
void MedianFilter(int w, int h, const unsigned char* inputImg, unsigned char* outImg) 
{
    int w3 = w * 3;

    int offsets[9] = { -w3 - 3, -w3, -w3 + 3, 
                       -3, 0, 3, 
                       w3 - 3, w3, w3 + 3 };

    for (int y = 1; y < h - 1; y++)
    {
        for (int x = 1; x < w - 1; x++)
        {
            unsigned char R[9];
            unsigned char G[9];
            unsigned char B[9];

            int centerIndex = (y * w + x) * 3;

            int indexTL = centerIndex + offsets[0];
            int indexTC = centerIndex + offsets[1];
            int indexTR = centerIndex + offsets[2];
            int indexCL = centerIndex + offsets[3];
            int indexCC = centerIndex + offsets[4];
            int indexCR = centerIndex + offsets[5];
            int indexBL = centerIndex + offsets[6];
            int indexBC = centerIndex + offsets[7];
            int indexBR = centerIndex + offsets[8];

            R[0] = inputImg[indexTL];
            R[1] = inputImg[indexTC];
            R[2] = inputImg[indexTR];
            R[3] = inputImg[indexCL];
            R[4] = inputImg[indexCC];
            R[5] = inputImg[indexCR];
            R[6] = inputImg[indexBL];
            R[7] = inputImg[indexBC];
            R[8] = inputImg[indexBR];

            G[0] = inputImg[indexTL + 1];
            G[1] = inputImg[indexTC + 1];
            G[2] = inputImg[indexTR + 1];
            G[3] = inputImg[indexCL + 1];
            G[4] = inputImg[indexCC + 1];
            G[5] = inputImg[indexCR + 1];
            G[6] = inputImg[indexBL + 1];
            G[7] = inputImg[indexBC + 1];
            G[8] = inputImg[indexBR + 1];

            B[0] = inputImg[indexTL + 2];
            B[1] = inputImg[indexTC + 2];
            B[2] = inputImg[indexTR + 2];
            B[3] = inputImg[indexCL + 2];
            B[4] = inputImg[indexCC + 2];
            B[5] = inputImg[indexCR + 2];
            B[6] = inputImg[indexBL + 2];
            B[7] = inputImg[indexBC + 2];
            B[8] = inputImg[indexBR + 2];

            int outIndex = (y * w + x) * 3;

            outImg[outIndex] = InsertionSort(R);
            outImg[outIndex + 1] = InsertionSort(G);
            outImg[outIndex + 2] = InsertionSort(B);
        }
    }

    // 处理边界
    for (int x = 0; x < w; x++)
    {
		int indexTop = x * 3;  // 顶部边界
		int indexBottom = (h - 1) * w3 + x * 3;  // 底部边界

		outImg[indexTop] = inputImg[indexTop];
		outImg[indexTop + 1] = inputImg[indexTop + 1];
		outImg[indexTop + 2] = inputImg[indexTop + 2];

		outImg[indexBottom] = inputImg[indexBottom];
		outImg[indexBottom + 1] = inputImg[indexBottom + 1];
		outImg[indexBottom + 2] = inputImg[indexBottom + 2];
	}

    for (int y = 0; y < h; y++)
    {
		int indexLeft = y * w3;  // 左边界
		int indexRight = (y + 1) * w3 - 3;  // 右边界

		outImg[indexLeft] = inputImg[indexLeft];
		outImg[indexLeft + 1] = inputImg[indexLeft + 1];
		outImg[indexLeft + 2] = inputImg[indexLeft + 2];

		outImg[indexRight] = inputImg[indexRight];
		outImg[indexRight + 1] = inputImg[indexRight + 1];
		outImg[indexRight + 2] = inputImg[indexRight + 2];
	}
}

inline int saturate(int value)
{
    if(value < 0)
        return 0;
    else if(value > 255)
        return 255;
	else
		return value;
}
void Sharpening(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    int kernel[9] = {0, -1, 0,
                    -1, 5, -1, 
                     0, -1, 0};
    int offsets[9] = {-w*3 - 3, -w*3, -w*3 + 3,
                      -3, 0, 3, 
                      w*3 - 3, w*3, w*3 + 3};

    for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {
            int sumR = 0;
            int sumG = 0;
            int sumB = 0;

            int centerIndex = (y * w + x) * 3;

            int indexTC = centerIndex + offsets[1];
            int indexCL = centerIndex + offsets[3];
            int indexCC = centerIndex + offsets[4];
            int indexCR = centerIndex + offsets[5];
            int indexBC = centerIndex + offsets[7];

            sumR += inputImg[indexCC] * 5 - (inputImg[indexTC] + inputImg[indexCL] + inputImg[indexCR] + inputImg[indexBC]);
            sumG += inputImg[indexCC + 1] * 5 - (inputImg[indexTC + 1] + inputImg[indexCL + 1] + inputImg[indexCR + 1] + inputImg[indexBC + 1]);
            sumB += inputImg[indexCC + 2] * 5 - (inputImg[indexTC + 2] + inputImg[indexCL + 2] + inputImg[indexCR + 2] + inputImg[indexBC + 2]);

            outImg[centerIndex] = saturate(sumR);
            outImg[centerIndex + 1] = saturate(sumG);
            outImg[centerIndex + 2] = saturate(sumB);
        }
    }

    for (int x = 0; x < w; x++) {
        int indexTop = x * 3;  // 顶部边界
        int indexBottom = (h - 1) * w * 3 + x * 3;  // 底部边界

        outImg[indexTop] = inputImg[indexTop];
        outImg[indexTop + 1] = inputImg[indexTop + 1];
        outImg[indexTop + 2] = inputImg[indexTop + 2];

        outImg[indexBottom] = inputImg[indexBottom];
        outImg[indexBottom + 1] = inputImg[indexBottom + 1];
        outImg[indexBottom + 2] = inputImg[indexBottom + 2];
    }
    for (int y = 0; y < h; y++) {
        int indexLeft = y * w * 3;  // 左边界
        int indexRight = (y + 1) * w * 3 - 3;  // 右边界

        outImg[indexLeft] = inputImg[indexLeft];
        outImg[indexLeft + 1] = inputImg[indexLeft + 1];
        outImg[indexLeft + 2] = inputImg[indexLeft + 2];

        outImg[indexRight] = inputImg[indexRight];
        outImg[indexRight + 1] = inputImg[indexRight + 1];
        outImg[indexRight + 2] = inputImg[indexRight + 2];
    }
}

void RGB2Gray(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
}

inline int clamp(int value, int threshold)
{
    if(value > threshold)
		return 255;
	else
		return 0;
}
void GlobalThresholdSegmentation(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    int totalPixels = w * h;

    for (int i = 0; i < totalPixels; i++)
    {
        unsigned char value = clamp(inputImg[i], 150);
        outImg[0] = value;
        outImg[1] = value;
        outImg[2] = value;
        outImg += 3;
    }
}

unsigned char calculateOtsuThreshold(const unsigned char* image, int totalPixels)
{
    int histogram[256] = { 0 };
    float sum = 0, sumB = 0, wB = 0, wF = 0, mB, mF, max = 0, between;
    unsigned char threshold = 0;

    for (int i = 0; i < totalPixels; i++) {
        histogram[image[i]]++;
    }

    for (int i = 0; i < 256; i++) {
        sum += i * histogram[i];
    }

    for (int t = 0; t < 256; t++) {
        wB += histogram[t];
        if (wB == 0) continue;
        wF = totalPixels - wB;
        if (wF == 0) break;

        sumB += t * histogram[t];
        mB = sumB / wB;
        mF = (sum - sumB) / wF;
        between = wB * wF * (mB - mF) * (mB - mF);

        if (between > max) {
            max = between;
            threshold = t;
        }
    }

    return threshold;
}

void OtsuThresholdSegmentation(int w, int h, const unsigned char* inputImg, unsigned char* outImg) 
{
    int totalPixels = w * h;
    unsigned char threshold = calculateOtsuThreshold(inputImg, totalPixels);

    for (int i = 0; i < totalPixels; i++) {
        unsigned char vlaue = clamp(inputImg[i], threshold);
        outImg[0] = vlaue;
        outImg ++;
    }
}

void DoubleThresholdSegmentation(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    int totalPixels = w * h;
	unsigned char lowerThreshold = 0;
	unsigned char upperThreshold = 0;
	unsigned char threshold = calculateOtsuThreshold(inputImg, totalPixels);

	lowerThreshold = threshold * 0.13;
	upperThreshold = threshold;

    for (int i = 0; i < totalPixels; i++) {
		unsigned char value = inputImg[i];
		unsigned char outputValue;

		if (value > upperThreshold)
			outputValue = 255;
		else if (value < lowerThreshold)
			outputValue = 0;
		else
			outputValue = 127;

		outImg[0] = outputValue;
		outImg[1] = outputValue;
		outImg[2] = outputValue;
		outImg += 3;
	}
}

float localmean(const unsigned char* image, int w, int h, int x, int y) {
    float sum = 0;
    int count = 0;

    for (int i = -NEIGHBORHOOD_SIZE / 2; i <= NEIGHBORHOOD_SIZE / 2; i++) {
        for (int j = -NEIGHBORHOOD_SIZE / 2; j <= NEIGHBORHOOD_SIZE / 2; j++) {
            int newX = x + j;
            int newY = y + i;

            if (newX >= 0 && newX < w && newY >= 0 && newY < h) {
                sum += image[newY * w + newX];
                count++;
            }
        }
    }

    return sum / count;
}

float localstd(const unsigned char* image, int w, int h, int x, int y, float mean) {
    float sum = 0;
    int count = 0;

    for (int i = -NEIGHBORHOOD_SIZE / 2; i <= NEIGHBORHOOD_SIZE / 2; i++) {
        for (int j = -NEIGHBORHOOD_SIZE / 2; j <= NEIGHBORHOOD_SIZE / 2; j++) {
            int newX = x + j;
            int newY = y + i;

            if (newX >= 0 && newX < w && newY >= 0 && newY < h) {
                float diff = image[newY * w + newX] - mean;
                sum += diff * diff;
                count++;
            }
        }
    }

    return sqrt(sum / count);
}

void LocalThresholdSegmentation(int w, int h, const unsigned char* inputImg, unsigned char* outImg, float a, float b) {
    int totalPixels = w * h;

    for (int i = 0; i < totalPixels; i++) {
		int x = i % w;
		int y = i / w;

		float mean = localmean(inputImg, w, h, x, y);
		float std = localstd(inputImg, w, h, x, y, mean);

		float threshold = mean * (1 + a * (std / b - 1));

		unsigned char value = clamp(inputImg[i], threshold);
		outImg[0] = value;
		outImg[1] = value;
		outImg[2] = value;
		outImg += 3;
	}
}


void DoubleThresholdSegmentation1(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    int totalPixels = w * h;
    unsigned char lowerThreshold = 0;
    unsigned char upperThreshold = 0;
    unsigned char threshold = calculateOtsuThreshold(inputImg, totalPixels);

    lowerThreshold = threshold * 0.13;
    upperThreshold = threshold;

    for (int i = 0; i < totalPixels; i++) {
        unsigned char value = inputImg[i];
        unsigned char outputValue;

        if (value > upperThreshold)
            outputValue = 255;
        else if (value < lowerThreshold)
            outputValue = 0;
        else
            outputValue = 127;

        outImg[0] = outputValue;
        outImg[1] = outputValue;
        outImg[2] = outputValue;
        outImg += 3;
    }
}

void ConstArraySegment(int w, int h, unsigned char* in_img, int thresholds[], int thresholdsNum, unsigned char* out_img)
{
    int len = w * h;
    for (int i = 0; i < len; i++)
    {
        unsigned char value = 0;
        for (int j = 0; j < thresholdsNum; j++)
        {
            if (in_img[i] >= thresholds[j])
            {
                value = 255;
                break;
            }
        }
        out_img[i] = value;
    }
}

unsigned char calculateOtsuThreshold(int* histogram, int totalPixels)
{
    float sum = 0, sumB = 0, wB = 0, wF = 0, mB, mF, max = 0, between;
    unsigned char threshold = 0;

    for (int i = 0; i < 256; i++)
    {
        sum += i * histogram[i];
    }

    for (int t = 0; t < 256; t++)
    {
        wB += histogram[t];
        if (wB == 0) continue;
        wF = totalPixels - wB;
        if (wF == 0) break;

        sumB += t * histogram[t];
        mB = sumB / wB;
        mF = (sum - sumB) / wF;
        between = wB * wF * (mB - mF) * (mB - mF);

        if (between > max)
        {
            max = between;
            threshold = t;
        }
    }

    return threshold;
}

void IterateOtsuSegment(int w, int h, unsigned char* in_img, unsigned char* out_img)
{
    int histogram[256] = { 0 };
    int len = w * h;
    for (int i = 0; i < len; i++)
    {
        histogram[in_img[i]]++;
    }

    int thresholds[50] = { 0 };
    int thresholdsNum = 0;

    unsigned char currentThreshold = calculateOtsuThreshold(histogram, len);
    thresholds[thresholdsNum++] = currentThreshold;

    while (thresholdsNum < 50)
    {
        int newHistogram[256] = { 0 };
        for (int i = 0; i < len; i++)
        {
            if (in_img[i] > currentThreshold)
            {
                newHistogram[in_img[i]]++;
            }
        }
        currentThreshold = calculateOtsuThreshold(newHistogram, len);
        if (currentThreshold <= thresholds[thresholdsNum - 1])
        {
            break;
        }
        thresholds[thresholdsNum++] = currentThreshold;
    }

    ConstArraySegment(w, h, in_img, thresholds, thresholdsNum, out_img);
}

typedef struct {
    int parent;
    int rank;
} UFNode;

int find(UFNode* nodes, int i) {
    if (nodes[i].parent != i) {
        nodes[i].parent = find(nodes, nodes[i].parent);
    }
    return nodes[i].parent;
}

void unionSets(UFNode* nodes, int i, int j) {
    int rootI = find(nodes, i);
    int rootJ = find(nodes, j);
    if (rootI != rootJ) {
        if (nodes[rootI].rank < nodes[rootJ].rank) {
            nodes[rootI].parent = rootJ;
        }
        else if (nodes[rootI].rank > nodes[rootJ].rank) {
            nodes[rootJ].parent = rootI;
        }
        else {
            nodes[rootJ].parent = rootI;
            nodes[rootI].rank++;
        }
    }
}

int CountTargets(int w, int h, const unsigned char* inputImg) {
    UFNode* nodes = (UFNode*)malloc(w * h * sizeof(UFNode));
    if (nodes == NULL) {
		return -1;
	}
    int* label = (int*)malloc(w * h * sizeof(int));
    if (label == NULL) {
		free(nodes);
        return -1;
    }
    int newLabel = 1;
    for (int i = 0; i < w * h; i++) {
        nodes[i].parent = i;
        nodes[i].rank = 0;
        label[i] = 0;
    }

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            if (inputImg[y * w + x] == 255) {
                int top = (y - 1) * w + x;
                int left = y * w + x - 1;

                if (x > 0 && inputImg[y * w + x - 1] == 255) { // left
                    if (label[left] == 0) {
                        label[left] = newLabel++;
                    }
                    label[y * w + x] = label[left];
                }

                if (y > 0 && inputImg[(y - 1) * w + x] == 255) { // top
                    if (label[top] == 0) {
                        label[top] = newLabel++;
                    }
                    if (label[y * w + x] == 0) {
                        label[y * w + x] = label[top];
                    }
                    else {
                        unionSets(nodes, label[y * w + x], label[top]);
                    }
                }

                if (label[y * w + x] == 0) {
                    label[y * w + x] = newLabel++;
                }
            }
        }
    }

    int count = 0;
    for (int i = 1; i < newLabel; i++) {
        if (i >= w * h) {
			break;
		}
        if (nodes[i].parent == i) {
            count++;
        }
    }

    free(nodes);
    free(label);

    return count;
}

void FindTargets(int w, int h, unsigned char* inputImg, int pos, PTARGET p_target)
{
    if (0 == inputImg[pos])
		return;
	
    (p_target->area)++;

    int x_pos = pos % w;
    int y_pos = pos / w;

    if(x_pos < p_target->minX) p_target->minX = x_pos;
    if(x_pos > p_target->maxX) p_target->maxX = x_pos;
    if(y_pos < p_target->minY) p_target->minY = y_pos;
    if(y_pos > p_target->maxY) p_target->maxY = y_pos;

    inputImg[pos] = 0;

    if(x_pos > 0)//左边
		FindTargets(w, h, inputImg, pos - 1, p_target);
    if(y_pos > 0)//上边
        FindTargets(w, h, inputImg, pos - w, p_target);
    if(x_pos < w - 1)//右边
        FindTargets(w, h, inputImg, pos + 1, p_target);
    if(y_pos < h - 1)//下边
        FindTargets(w, h, inputImg, pos + w, p_target);
}

int CountTargets1(int w, int h, const unsigned char* inputImg, PTARGET pTarget)
{
    int count = 0;
    int len = w * h;
    unsigned char* tempImg = (unsigned char*)malloc(len);
    if(NULL == tempImg)
        return -1;
    memcpy(tempImg, inputImg, len);

    for (int i = 0; i < len; i++)
    {
        if (tempImg[i] == 255)
        {
            PTARGET p_target = pTarget + count;//指向第count个目标
            p_target->minX = w;
            p_target->minY = h;
            p_target->maxX = 0;
            p_target->maxY = 0;
            p_target->area = 0;

            FindTargets(w, h, tempImg, i, p_target);

            if (p_target->area > 50)
            {
                count++;
            }
        }
    }

    free(tempImg);
    return count;
}

void BoundaryExtraction(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    //左上角
    if (inputImg[0] == 255)
    {
        if (inputImg[1] == 0 || inputImg[w] == 0)
        {
			outImg[0] = 255;
		}
	}

    //右上角
    if (inputImg[w - 1] == 255)
    {
        if (inputImg[w - 2] == 0 || inputImg[w * 2 - 1] == 0)
        {
            outImg[w - 1] = 255;
        }
    }

    //左下角
    if (inputImg[w * (h - 1)] == 255)
    {
        if (inputImg[w * (h - 2)] == 0 || inputImg[w * (h - 1) + 1] == 0)
        {
			outImg[w * (h - 1)] = 255;
		}
	}

    //右下角
    if (inputImg[w * h - 1] == 255)
    {
		if (inputImg[w * h - 2] == 0 || inputImg[w * (h - 1) - 1] == 0)
		{
            outImg[w * h - 1] = 255;
        }
    }

    //上边
    for (int i = 1; i < w - 1; i++)
    {
        if (inputImg[i] == 255)
        {
            if (inputImg[i - 1] == 0 || inputImg[i + 1] == 0 || inputImg[i + w] == 0)
            {
                outImg[i] = 255;
            }
        }
    }

    //下边
    for (int i = w * (h - 1) + 1; i < w * h - 1; i++)
    {
        if (inputImg[i] == 255)
        {
            if (inputImg[i - 1] == 0 || inputImg[i + 1] == 0 || inputImg[i - w] == 0)
            {
				outImg[i] = 255;
			}
		}
	}

    //左边
    for (int i = w; i < w * (h - 1); i += w)
    {
        if (inputImg[i] == 255)
        {
            if (inputImg[i - w] == 0 || inputImg[i + 1] == 0 || inputImg[i + w] == 0)
            {
                outImg[i] = 255;
            }
        }
    }

    //右边
    for (int i = w * 2 - 1; i < w * h - 1; i += w)
    {
        if (inputImg[i] == 255)
        {
            if (inputImg[i - w] == 0 || inputImg[i - 1] == 0 || inputImg[i + w] == 0)
            {
				outImg[i] = 255;
			}
		}
	}

    //内部像素
    for (int y = 0; y < h - 1; y++)
    {
        for (int x = 0; x < w - 1; x++)
        {
            int i = y * w + x;

            if (inputImg[i] == 255)
            {
                if (inputImg[i - 1] == 0 || inputImg[i + 1] == 0 || inputImg[i - w] == 0 || inputImg[i + w] == 0)
                {
					outImg[i] = 255;
				}
			}
        }
    }
}

void BinaryCorrosion(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    int len = w * h;

    for (int i = 0; i < len; i++)
    {
        if (inputImg[i] == 255)
        {
			int x_pos = i % w;
			int y_pos = i / w;

            if (x_pos > 0 && inputImg[i - 1] == 0) // left
            {
				outImg[i] = 0;
				continue;
			}
            if (y_pos > 0 && inputImg[i - w] == 0) // top
            {
				outImg[i] = 0;
				continue;
			}
            if (x_pos < w - 1 && inputImg[i + 1] == 0) // right
            {
				outImg[i] = 0;
				continue;
			}
            if (y_pos < h - 1 && inputImg[i + w] == 0) // bottom
            {
				outImg[i] = 0;
				continue;
			}
            else
            {
                outImg[i] = 255;
            }
		}
	}
}

void BinaryExpansion(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    int len = w * h;

    for (int i = 0; i < len; i++)
    {
        if (inputImg[i] == 0)
        {
			int x_pos = i % w;
			int y_pos = i / w;

            if (x_pos > 0 && inputImg[i - 1] == 255) // right
            {
				outImg[i] = 255;
				continue;
			}
            if (y_pos > 0 && inputImg[i - w] == 255) // bottom
            {
				outImg[i] = 255;
				continue;
			}
            if (x_pos < w - 1 && inputImg[i + 1] == 255) // left
            {
				outImg[i] = 255;
				continue;
			}
            if (y_pos < h - 1 && inputImg[i + w] == 255) // top
            {
				outImg[i] = 255;
				continue;
			}
            else
            {
				outImg[i] = 0;
			}
		}
        else
        {
            outImg[i] = 255;
        }
	}
}

void GrayCorrosion(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    int len = w * h;

    for (int i = 0; i < len; i++)
    {
		int x_pos = i % w;
		int y_pos = i / w;

        if (x_pos > 0 && inputImg[i - 1] < inputImg[i]) // left
        {
			outImg[i] = inputImg[i - 1];
			continue;
		}
        if (y_pos > 0 && inputImg[i - w] < inputImg[i]) // top
        {
			outImg[i] = inputImg[i - w];
			continue;
		}
        if (x_pos < w - 1 && inputImg[i + 1] < inputImg[i]) // right
        {
			outImg[i] = inputImg[i + 1];
			continue;
		}
        if (y_pos < h - 1 && inputImg[i + w] < inputImg[i]) // bottom
        {
			outImg[i] = inputImg[i + w];
			continue;
		}
        else
        {
			outImg[i] = inputImg[i];
		}
	}
}

void RGBCorrosion(int w, int h, const unsigned char* inputImg, KERNEL kernel, unsigned char* outImg)
{
    memset(outImg, 0, w * h * 3);

    // 遍历图像的每个像素
    for (int y = 0; y < h; ++y) 
    {
        for (int x = 0; x < w; ++x) 
        {
            // 初始化最小值为最大可能值
            unsigned char minVal[3] = { UCHAR_MAX, UCHAR_MAX, UCHAR_MAX };

            // 遍历核内的每个位置
            for (int ky = -kernel.anchorY; ky <= kernel.height - kernel.anchorY - 1; ++ky) 
            {
                for (int kx = -kernel.anchorX; kx <= kernel.width - kernel.anchorX - 1; ++kx) 
                {
                    // 计算当前像素在核内的位置
                    int imgX = x + kx;
                    int imgY = y + ky;

                    // 检查位置是否在图像边界内
                    if (imgX >= 0 && imgX < w && imgY >= 0 && imgY < h) 
                    {
                        // 只有在结构元素为1的位置才进行处理
                        if (kernel.data[(ky + kernel.anchorY) * kernel.width + (kx + kernel.anchorX)]) 
                        {
                            // 遍历每个颜色通道
                            for (int c = 0; c < 3; ++c) 
                            {
                                int imgIndex = (imgY * w + imgX) * 3 + c;

                                if (inputImg[imgIndex] < minVal[c]) 
                                {
                                    minVal[c] = inputImg[imgIndex];
                                }
                            }
                        }
                    }
                }
            }

            // 将计算出的最小值应用到输出图像
            for (int c = 0; c < 3; ++c) 
            {
                outImg[(y * w + x) * 3 + c] = minVal[c];
            }
        }
    }
}

void RGBExpansion(int w, int h, const unsigned char* inputImg, KERNEL kernel, unsigned char* outImg)
{
    memset(outImg, 0, w * h * 3);

	// 遍历图像的每个像素
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
			// 初始化最大值为最小可能值
			unsigned char maxVal[3] = { 0, 0, 0 };

			// 遍历核内的每个位置
            for (int ky = -kernel.anchorY; ky <= kernel.height - kernel.anchorY - 1; ++ky)
            {
                for (int kx = -kernel.anchorX; kx <= kernel.width - kernel.anchorX - 1; ++kx)
                {
					// 计算当前像素在核内的位置
					int imgX = x + kx;
					int imgY = y + ky;

					// 检查位置是否在图像边界内
                    if (imgX >= 0 && imgX < w && imgY >= 0 && imgY < h)
                    {
						// 只有在结构元素为1的位置才进行处理
                        if (kernel.data[(ky + kernel.anchorY) * kernel.width + (kx + kernel.anchorX)])
                        {
                            for (int c = 0; c < 3; ++c)
                            {
                                int imgIndex = (imgY * w + imgX) * 3 + c;

                                if (inputImg[imgIndex] > maxVal[c])
                                {
									maxVal[c] = inputImg[imgIndex];
								}
                            }
                        }
                    }
                }
            }

            // 将计算出的最大值应用到输出图像
            for (int c = 0; c < 3; ++c)
            {
				outImg[(y * w + x) * 3 + c] = maxVal[c];
			}
        }
    }
}

void RGBOpening(int w, int h, const unsigned char* inputImg, KERNEL kernel, unsigned char* outImg)
{
    unsigned char* tempImg = (unsigned char*)malloc(w * h * 3);
	if (NULL == tempImg)
		return;
	memset(tempImg, 0, w * h * 3);

	RGBCorrosion(w, h, inputImg, kernel, tempImg);
	RGBExpansion(w, h, tempImg, kernel, outImg);

	free(tempImg);
}

void RGBClosing(int w, int h, const unsigned char* inputImg, KERNEL kernel, unsigned char* outImg)
{
    unsigned char* tempImg = (unsigned char*)malloc(w * h * 3);
	if (NULL == tempImg)
		return;
	memset(tempImg, 0, w * h * 3);

	RGBExpansion(w, h, inputImg, kernel, tempImg);
	RGBCorrosion(w, h, tempImg, kernel, outImg);

	free(tempImg);
}

int CountNeighborTransitions(int x, int y, int w, int h, const unsigned char* inputImg) {
    int count = 0;
    int neighbors[8] = { 0 };
    int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
    int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

    // 对于每个邻居，检查是否在图像边界内
    for (int i = 0; i < 8; i++) {
        int newX = x + dx[i];
        int newY = y + dy[i];

        // 只有当新坐标在图像边界内时，才读取像素值
        if (newX >= 0 && newX < w && newY >= 0 && newY < h) {
            neighbors[i] = inputImg[newY * w + newX];
        }
        else {
            neighbors[i] = 0; // 边界外的像素视为0
        }
    }

    // 计算从0到255的转变次数
    for (int i = 0; i < 7; i++) {
        if (neighbors[i] == 0 && neighbors[i + 1] == 255) {
            count++;
        }
    }

    if (neighbors[7] == 0 && neighbors[0] == 255) {
		count++;
	}

    return count;
}


int CountForegroundNeighbors(int x, int y, int w, int h, const unsigned char* inputImg) {
    int count = 0;
    int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
    int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

    // 对于每个邻居，检查是否在图像边界内
    for (int i = 0; i < 8; i++) {
        int newX = x + dx[i];
        int newY = y + dy[i];

        // 只有当新坐标在图像边界内时，才计算像素值
        if (newX >= 0 && newX < w && newY >= 0 && newY < h && inputImg[newY * w + newX] == 255) {
            count++;
        }
    }

    return count;
}

// 应用标记并删除像素
void applyMarker(int w, int h, unsigned char* img, int* marker) {
    for (int i = 0; i < w * h; ++i) {
        if (marker[i]) {
            img[i] = 0; // 删除像素
            marker[i] = 0; // 重置标记
        }
    }
}

// 决定是否应删除像素
int shouldBeRemoved(int x, int y, int w, int h, const unsigned char* img, int* marker, int step) {
    int idx = y * w + x;
    if (img[idx] == 0) return 0;

    // 8邻域像素
    int p2 = img[idx - w];
    int p3 = img[idx - w + 1];
    int p4 = img[idx + 1];
    int p6 = img[idx + w];
    int p7 = img[idx + w - 1];
    int p8 = img[idx - 1];
    int p9 = img[idx - w - 1];

    int A = CountNeighborTransitions(x, y, w, h, img);
    int B = CountForegroundNeighbors(x, y, w, h, img);

    // 条件m1和m2
    int m1, m2;
    if (step == 1) {
        m1 = p2 * p4 * p6;
        m2 = p4 * p6 * p8;
    }
    else {
        m1 = p2 * p4 * p8;
        m2 = p2 * p6 * p8;
    }

    // 应用 Zhang-Suen 条件
    if (B >= 2 && B <= 6 && A == 1 && m1 == 0 && m2 == 0) {
        marker[idx] = 1; // 标记为移除
        return 1;
    }
    return 0;
}

void ZhangSuenThinning(int w, int h, const unsigned char* inputImg, unsigned char* outImg) {

    memcpy(outImg, inputImg, w * h * sizeof(unsigned char));

    // 分配一个标记数组，用于标记需要被移除的像素
    int* marker = (int*)calloc(w * h, sizeof(int));
    int hasChange;
    int n = 1;

    do {
        hasChange = 0;

        // 子迭代1
        for (int y = 1; y < h - 1; ++y) {
            for (int x = 1; x < w - 1; ++x) {
                if (shouldBeRemoved(x, y, w, h, outImg, marker, 1)) {
                    hasChange = 1;
                }
            }
        }

        // 应用标记并移除像素
        applyMarker(w, h, outImg, marker);

        // 子迭代2
        for (int y = 1; y < h - 1; ++y) {
            for (int x = 1; x < w - 1; ++x) {
                if (shouldBeRemoved(x, y, w, h, outImg, marker, 2)) {
                    hasChange = 1;
                }
            }
        }

        // 再次应用标记并移除像素
        applyMarker(w, h, outImg, marker);


        /*char name[20] = { 0 };
        sprintf_s(name, "骨架提取: %d", n++);
        ShowBinaryImage(name, w, h, outImg);*/

    } while (hasChange);

    free(marker);
}

void GenerateGaussianKernel(float* kernel, int kernelSize, float sigma) {
    float sum = 0.0;
    int center = kernelSize >> 1;
    for (int i = 0; i < kernelSize; ++i) {
        for (int j = 0; j < kernelSize; ++j) {
            float x = i - center;
            float y = j - center;
            kernel[i * kernelSize + j] = exp(-(x * x + y * y) / (2 * sigma * sigma));
            sum += kernel[i * kernelSize + j];
        }
    }
    // 归一化核
    for (int i = 0; i < kernelSize * kernelSize; ++i) {
        kernel[i] /= sum;
    }
}

void GaussianFilterGray(int w, int h, const unsigned char* inputImg, unsigned char* outImg, float sigma) {
    int kernelSize = (int)(6 * sigma) | 1;
    int halfKernel = kernelSize >> 1;
    float* kernel = (float*)malloc(kernelSize * kernelSize * sizeof(float));

    GenerateGaussianKernel(kernel, kernelSize, sigma);

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            float sum = 0.0;
            for (int ky = -halfKernel; ky <= halfKernel; ky++) {
                for (int kx = -halfKernel; kx <= halfKernel; kx++) {
                    int iy = y + ky;
                    int ix = x + kx;
                    if (iy >= 0 && iy < h && ix >= 0 && ix < w) {
                        sum += inputImg[iy * w + ix] * kernel[(ky + halfKernel) * kernelSize + (kx + halfKernel)];
                    }
                }
            }
            outImg[y * w + x] = (unsigned char)sum;
        }
    }

    free(kernel);
}

void SingleScaleRetinex(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    unsigned char* tempImg = (unsigned char*)malloc(w * h * sizeof(unsigned char));
    if (NULL == tempImg)
		return;

    float sigma = 80;
    int size = w * h;

    GaussianFilterGray(w, h, inputImg, tempImg, sigma);

    float maxVal = -FLT_MAX;
    float minVal = FLT_MAX;

    static float outImg_float[2160 * 4096] = { 0 };
    static unsigned char img_tp[2160 * 4096] = { 0 };

    for (int i = 0; i < size; i++)
    {
		float logOriginal = log(inputImg[i] + 1);
        float logFiltered = log(tempImg[i] + 1);
        outImg_float[i] = logOriginal - logFiltered;
        if(outImg_float[i] > maxVal) maxVal = outImg_float[i];
        if(outImg_float[i] < minVal) minVal = outImg_float[i];
	}

    float diff_log = maxVal - minVal;

    for (int i = 0; i < size; i++)
    {
		outImg[i] = (unsigned char)((outImg_float[i] - minVal) / diff_log * 255.0f);
	}
    ShowBinaryImage("SingleScaleRetinex", w, h, outImg);

    free(tempImg);
}

void LinearStretching(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    unsigned char minVal = 0xff;
	unsigned char maxVal = 0;
	int size = w * h;

	for (int i = 0; i < size; i++)
	{
		if (inputImg[i] > maxVal)
		{
			maxVal = inputImg[i];
		}
		if (inputImg[i] < minVal)
		{
			minVal = inputImg[i];
		}
	}

	float diff = maxVal - minVal;

	for (int i = 0; i < size; i++)
	{
		outImg[i] = (unsigned char)((inputImg[i] - minVal) / diff * 255.0f);
	}
}

void PiecewiseLinearStretching(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
unsigned char minVal = 0xff;
	unsigned char maxVal = 0;
	int size = w * h;

	for (int i = 0; i < size; i++)
	{
		if (inputImg[i] > maxVal)
		{
			maxVal = inputImg[i];
		}
		if (inputImg[i] < minVal)
		{
			minVal = inputImg[i];
		}
	}

	float diff = maxVal - minVal;

	for (int i = 0; i < size; i++)
	{
		if (inputImg[i] < minVal + diff / 3)
		{
			outImg[i] = (unsigned char)((inputImg[i] - minVal) / (diff / 3) * 64.0f);
		}
		else if (inputImg[i] < minVal + diff * 2 / 3)
		{
			outImg[i] = (unsigned char)((inputImg[i] - minVal - diff / 3) / (diff / 3) * 128.0f + 64.0f);
		}
		else
		{
			outImg[i] = (unsigned char)((inputImg[i] - minVal - diff * 2 / 3) / (diff / 3) * 64.0f + 192.0f);
            if (0 == outImg[i])
                outImg[i] = 255;
		}
	}
}

void log_transform(int w, int h, unsigned char* in_img, unsigned char* out_img)
{
    static float log_v[2160 * 4096] = { 0 };
    unsigned char* p_in = in_img;
    float min_log = 255;
    float max_log = 0;

    for (int i = 0; i < w * h; i++)
    {
        int pi_v = p_in[0] + 1;
        log_v[i] = (float)log((double)pi_v);

        if (min_log > log_v[i] && log_v[i] != 0)
            min_log = log_v[i];
        if (max_log < log_v[i])
            max_log = log_v[i];

        p_in += 3;
    }

    float diff_log = max_log - min_log;
    unsigned char* p_out = out_img;

    for (int i = 0; i < w * h; i++)
    {
        int pi_fv = (int)((log_v[i] - min_log) / diff_log * 255.0f);

        p_out[0] = (unsigned char)saturate(pi_fv);
        p_out[1] = (unsigned char)saturate(pi_fv);
        p_out[2] = (unsigned char)saturate(pi_fv);
        p_out += 3;
    }

    ShowRGBImage("logStrech", w, h, out_img);
}

void LogarithmicTransform(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    int size = w * h;
    static float img_log[2160 * 4096] = { 0 };
    float maxVal = 0;
    float minVal = 255;

    for (int i = 0; i < size; i++)
    {
        float logOriginal = (float)log(inputImg[i] + 1);

        img_log[i] = logOriginal;

        if (img_log[i] > maxVal) maxVal = logOriginal;
        if (img_log[i] < minVal && inputImg[i] != 0) minVal = logOriginal;
    }

    float diff_log = maxVal - minVal;

    for (int i = 0; i < size; i++)
    {
        int pi_fv = (int)((img_log[i] - minVal) / diff_log * 255.0f);

        outImg[i] = (unsigned char)saturate(pi_fv);
    }

    ShowBinaryImage("LogarithmicTransform", w, h, outImg);
}

void HistogramEqualization(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    int size = w * h;
	int histogram[256] = { 0 };
	float probability[256] = { 0 };
	float cdf[256] = { 0 };
	int equalized[256] = { 0 };

    for (int i = 0; i < size; i++)
    {
		histogram[inputImg[i]]++;
	}

    for (int i = 0; i < 256; i++)
    {
		probability[i] = (float)histogram[i] / size;
	}

	cdf[0] = probability[0];
    for (int i = 1; i < 256; i++)
    {
		cdf[i] = cdf[i - 1] + probability[i];
	}

    for (int i = 0; i < 256; i++)
    {
		equalized[i] = (int)(cdf[i] * 255);
	}

    for (int i = 0; i < size; i++)
    {
		outImg[i] = equalized[inputImg[i]];
	}

    ShowBinaryImage("HistogramEqualization", w, h, outImg);
}

void WeightedHistogramEqualization(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    int size = w * h;
    int histogram[256] = { 0 };
    float normalizedHistogram[256] = { 0 };
    float cdf[256] = { 0 };
    int mappedValues[256] = { 0 };

    // 计算原始直方图
    for (int i = 0; i < size; i++) {
        histogram[inputImg[i]]++;
    }

    // 归一化直方图
    for (int i = 0; i < 256; i++) {
        normalizedHistogram[i] = (float)histogram[i] / size;
    }

    // 计算累积分布函数（CDF）
    cdf[0] = normalizedHistogram[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + normalizedHistogram[i];
    }

    // 根据CDF计算映射值，并应用加权均衡化
    for (int i = 0; i < 256; i++) {
        float weightedValue = 0.6 * cdf[i] + 0.4 * (i / 255.0);
        mappedValues[i] = (int)(weightedValue * 255);
    }

    // 应用加权映射到输出图像
    for (int i = 0; i < size; i++) {
        outImg[i] = mappedValues[inputImg[i]];
    }

    // 显示处理后的图像
    ShowBinaryImage("WeightedHistogramEqualization", w, h, outImg);
}

void HistogramSqrt(int w, int h, const unsigned char* inputImg, unsigned char* outImg)
{
    int size = w * h;
    int histogram[256] = { 0 };
    float rootHistogram[256] = { 0 };
    float normalizedHistogram[256] = { 0 };
    float cdf[256] = { 0 };
    int mapping[256] = { 0 };

    // 计算原始图像的直方图
    for (int i = 0; i < size; i++) {
        histogram[inputImg[i]]++;
    }

    // 应用开根运算（例如三次根）来拉伸直方图
    for (int i = 0; i < 256; i++) {
        rootHistogram[i] = pow(histogram[i], 0.333);
    }

    // 归一化拉伸后的直方图
    float total = 0;
    for (int i = 0; i < 256; i++) {
        total += rootHistogram[i];
    }
    for (int i = 0; i < 256; i++) {
        normalizedHistogram[i] = rootHistogram[i] / total;
    }

    // 计算归一化直方图的累积分布函数 (CDF)
    cdf[0] = normalizedHistogram[0];
    for (int i = 1; i < 256; i++) {
        cdf[i] = cdf[i - 1] + normalizedHistogram[i];
    }

    // 根据CDF映射拉伸后的直方图回图像
    for (int i = 0; i < 256; i++) {
        mapping[i] = (int)(cdf[i] * 255);
    }
    for (int i = 0; i < size; i++) {
        outImg[i] = mapping[inputImg[i]];
    }

	ShowBinaryImage("HistogramSqrt", w, h, outImg);
}

void LocalHistogramEqualization(int w, int h, const unsigned char* inputImg, unsigned char* outImg, int blockWidth, int blockHeight)
{
    int size = w * h;
	int histogram[256] = { 0 };
	float probability[256] = { 0 };
	float cdf[256] = { 0 };
	int equalized[256] = { 0 };

	// 计算原始直方图
    for (int i = 0; i < size; i++) {
		histogram[inputImg[i]]++;
	}

	// 计算原始直方图的概率
    for (int i = 0; i < 256; i++) {
		probability[i] = (float)histogram[i] / size;
	}

	// 计算原始直方图的累积分布函数（CDF）
	cdf[0] = probability[0];
    for (int i = 1; i < 256; i++) {
		cdf[i] = cdf[i - 1] + probability[i];
	}

	// 计算每个块的局部直方图
	int blockCountX = w / blockWidth;
	int blockCountY = h / blockHeight;
	int blockSize = blockWidth * blockHeight;
	int blockHistogram[256] = { 0 };
	float blockProbability[256] = { 0 };
	float blockCDF[256] = { 0 };
	int blockEqualized[256] = { 0 };

    for (int blockY = 0; blockY < blockCountY; blockY++) 
    {
        for (int blockX = 0; blockX < blockCountX; blockX++) 
        {
			// 计算当前块的直方图
			memset(blockHistogram, 0, 256 * sizeof(int));
            for (int y = 0; y < blockHeight; y++) 
            {
                for (int x = 0; x < blockWidth; x++) 
                {
					int idx = (blockY * blockHeight + y) * w + (blockX * blockWidth + x);
					blockHistogram[inputImg[idx]]++;
				}
			}

			// 计算当前块的概率
            for (int i = 0; i < 256; i++) 
            {
				blockProbability[i] = (float)blockHistogram[i] / blockSize;
			}

			// 计算当前块的累积分布函数（CDF）
            blockCDF[0] = blockProbability[0];
            for (int i = 1; i < 256; i++) 
            {
                blockCDF[i] = blockCDF[i - 1] + blockProbability[i];
            }

            // 计算当前块的映射值
            for (int i = 0; i < 256; i++) 
            {
				blockEqualized[i] = (int)(blockCDF[i] * 255);
			}

            // 应用映射值到当前块
            for (int y = 0; y < blockHeight; y++) 
            {
                for (int x = 0; x < blockWidth; x++) 
                {
                    int idx = (blockY * blockHeight + y) * w + (blockX * blockWidth + x);
                    outImg[idx] = blockEqualized[inputImg[idx]];
                }
            }
        }
    }
    ShowBinaryImage("LocalHistogramEqualization", w, h, outImg);
}

void ClipHistogram(unsigned long* histogram, int numBins, float clipLimit) {
    unsigned long totalExcess = 0;

    // 计算超出限制的像素总数
    for (int i = 0; i < numBins; ++i) {
        int excess = (int)(histogram[i] - clipLimit);
        if (excess > 0) {
            totalExcess += excess;
        }
    }

    // 平均分配超出限制的像素
    int averageIncrement = totalExcess / numBins;
    float upperLimit = (float)(clipLimit - averageIncrement);

    for (int i = 0; i < numBins; ++i) {
        if (histogram[i] > clipLimit) {
            histogram[i] = clipLimit;
        }
        else {
            if (histogram[i] > upperLimit) {
                totalExcess -= histogram[i] - upperLimit;
                histogram[i] = upperLimit;
            }
        }
    }

    // 将剩余的像素均匀分配
    while (totalExcess > 0) {
        for (int i = 0; i < numBins && totalExcess > 0; ++i) {
            if (histogram[i] < clipLimit) {
                histogram[i]++;
                totalExcess--;
            }
        }
    }
}

void ComputeCDF(const unsigned long* histogram, float* cdf, int numPixels) {
    cdf[0] = (float)histogram[0] / numPixels;
    for (int i = 1; i < 256; ++i) {
        cdf[i] = cdf[i - 1] + (float)histogram[i] / numPixels;
    }
}

//void CLAHE(int w, int h, int blockWidth, int blockHeight, float clipLimit, const unsigned char* inputImg, unsigned char* outImg) {
//    int blockCountX = w / blockWidth;
//    int blockCountY = h / blockHeight;
//    int blockSize = blockWidth * blockHeight;
//    unsigned long histogram[256];
//
//    // 分块处理图像
//    for (int by = 0; by < blockCountY; ++by) {
//        for (int bx = 0; bx < blockCountX; ++bx) {
//            // 初始化直方图
//            memset(histogram, 0, sizeof(histogram));
//
//            // 计算每个块的直方图
//            for (int y = 0; y < blockHeight; ++y) {
//                for (int x = 0; x < blockWidth; ++x) {
//                    int index = (by * blockHeight + y) * w + (bx * blockWidth + x);
//                    histogram[inputImg[index]]++;
//                }
//            }
//
//            // 裁剪和归一化直方图
//            ClipHistogram(histogram, 256, clipLimit);
//
//            // 计算直方图的累积分布函数 (CDF)
//            float cdf[256];
//            ComputeCDF(histogram, cdf, blockSize);
//
//            // 应用CLAHE到每个像素
//            for (int y = 0; y < blockHeight; ++y) {
//                for (int x = 0; x < blockWidth; ++x) {
//                    int index = (by * blockHeight + y) * w + (bx * blockWidth + x);
//                    outImg[index] = (unsigned char)(cdf[inputImg[index]] * 255);
//                }
//            }
//        }
//    }
//}

void CLAHE(int w, int h, int blockWidth, int blockHeight, float clipLimit, const unsigned char* inputImg, unsigned char* outImg) 
{
    int blockCountX = w / blockWidth;
    int blockCountY = h / blockHeight;
    int blockSize = blockWidth * blockHeight;

    // 存储每个区块的映射
    unsigned long(*pulMapArray)[256] = new unsigned long[blockCountX * blockCountY][256];
    unsigned long histogram[256];

    // 分块处理图像
    for (int by = 0; by < blockCountY; ++by)
    {
        for (int bx = 0; bx < blockCountX; ++bx)
        {
            // 初始化直方图
            memset(histogram, 0, sizeof(histogram));

            // 计算每个块的直方图
            for (int y = 0; y < blockHeight; ++y)
            {
                for (int x = 0; x < blockWidth; ++x)
                {
                    int index = (by * blockHeight + y) * w + (bx * blockWidth + x);
                    histogram[inputImg[index]]++;
                }
            }

            // 裁剪和归一化直方图
            ClipHistogram(histogram, 256, clipLimit);

            // 计算直方图的累积分布函数 (CDF)
            float cdf[256];
            ComputeCDF(histogram, cdf, blockSize);

            for (int i = 0; i < 256; ++i) {
                pulMapArray[by * blockCountX + bx][i] = (unsigned long)(cdf[i] * 255.0f);
            }
        }
    }

    // 对整个图像进行双线性插值
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            // 计算区块索引及相对位置
            int bx = min(x / blockWidth, blockCountX - 1);
            int by = min(y / blockHeight, blockCountY - 1);
            float fx = (float)(x % blockWidth) / blockWidth;
            float fy = (float)(y % blockHeight) / blockHeight;

            // 获取相邻四个区块的CDF映射
            unsigned long* lb = pulMapArray[by * blockCountX + bx];
            unsigned long* rb = (bx < blockCountX - 1) ? pulMapArray[by * blockCountX + bx + 1] : lb;
            unsigned long* lu = (by < blockCountY - 1) ? pulMapArray[(by + 1) * blockCountX + bx] : lb;
            unsigned long* ru = (bx < blockCountX - 1 && by < blockCountY - 1) ? pulMapArray[(by + 1) * blockCountX + bx + 1] : lb;


            // 获取当前像素值
            unsigned char pixelVal = inputImg[y * w + x];

            // 双线性插值
            float valLB = lb[pixelVal] / 255.0f;
            float valRB = rb[pixelVal] / 255.0f;
            float valLU = lu[pixelVal] / 255.0f;
            float valRU = ru[pixelVal] / 255.0f;

            // 计算插值
            float interpVal = (valLB * (1 - fx) * (1 - fy) + valRB * fx * (1 - fy) +
                valLU * (1 - fx) * fy + valRU * fx * fy) * 255.0f;

            // 应用插值结果到输出图像
            float clampedVal = max(0.0f, min(interpVal, 255.0f));
            outImg[y * w + x] = (unsigned char)clampedVal;
        }
    }

    // 清理资源
    delete[] pulMapArray;
}
