#pragma once

PBYTE ReadFileToBuffer(const char* filename, long* pFileLen);

int ReadYuv(char* filename, int* width, int* height, unsigned char* img_rgb);

int ReadBmp(char* filename, int* pWidth, int* pHeight, int* pBitDepth, unsigned char* img_rgb);

int Read_12Bits_Raw(char* filename, int* pWidth, int* pHeight, unsigned short* img_data);

void Raw2Gray(int width, int height, const unsigned short* raw_data, unsigned char* img_gray);

void Raw2Rgb(int width, int height, unsigned short* raw_data, unsigned char* img_rgb);

void CopyImageBits(int w, int h, unsigned char* in_img, unsigned char* out_img);

int SaveBmp(char* BmpPath, int w_img, int h_img, unsigned char* img_rgb);

void Gray2Rgb(int width, int height, const unsigned char* img_data, unsigned char* img_rgb);

void Rgb2Gray(int width, int height, const unsigned char* img_data, unsigned char* img_gary);

void RgbToGary(int width, int height, const unsigned char* img_data, unsigned char* img_gary);

void Yuv2Rgb(PBYTE pYuvBuf, int w, int h, PBYTE img_rgb);

void Rgb2Yuv420P(SIZE sz, PBYTE pRgb, PBYTE pYuv);