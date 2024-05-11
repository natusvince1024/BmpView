#include "pch.h"
#include "SimpleBMP.h"


PBYTE ReadFileToBuffer(const char* filename, long* pFileLen) {
    FILE* pFile = fopen(filename, "rb");
    if (!pFile) {
        return NULL; // 文件打开失败
    }

    // 获取文件长度
    fseek(pFile, 0, SEEK_END);
    long filelen = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);

    PBYTE pBuf = (PBYTE)malloc(filelen);
    if (!pBuf) {
        fclose(pFile);
        return NULL; // 内存分配失败
    }

    fread(pBuf, 1, filelen, pFile);
    fclose(pFile);

    *pFileLen = filelen;
    return pBuf;
}

// bpp : bits per pixel
int ReadYuv(char* filename, int* width, int* height, unsigned char* img_rgb) {
    long filelen;
    PBYTE pYuvBuf = ReadFileToBuffer(filename, &filelen);
    if (!pYuvBuf) {
        return -1; // 文件读取或内存分配失败
    }

    int total_pixels = filelen * 2 / 3;
    int img_height = 2160;
    int img_width = total_pixels / img_height;

    Yuv2Rgb(pYuvBuf, img_width, img_height, img_rgb);

    free(pYuvBuf);
    *width = img_width;
    *height = img_height;

    return 0; // 成功
}

int ReadBmp(char* filename, int* pWidth, int* pHeight, int* pBitDepth, unsigned char* img_rgb) {
    long filelen;
    PBYTE pBuf = ReadFileToBuffer(filename, &filelen);
    if (!pBuf) {
        return -1; // 文件读取或内存分配失败
    }
    LPBITMAPINFOHEADER lpbih;
    lpbih = (LPBITMAPINFOHEADER)(pBuf + sizeof(BITMAPFILEHEADER)); // 获取位图信息头指针

    *pWidth = lpbih->biWidth;
    *pHeight = lpbih->biHeight;
    *pBitDepth = lpbih->biBitCount;

    LPBITMAPFILEHEADER lpbfh = (LPBITMAPFILEHEADER)pBuf;
    PBYTE lpBits = pBuf + lpbfh->bfOffBits;
    RGBQUAD* palette = (RGBQUAD*)(pBuf + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));

    if (*pBitDepth == 8) {
        int paletteSize = 256;
        for (int y = 0; y < *pHeight; y++) {
            for (int x = 0; x < *pWidth; x++) {
                int index = lpBits[(*pHeight - y - 1) * (*pWidth) + x];
                if (index >= paletteSize) {
                    free(pBuf);
                    return -2; // Palette index out of range
                }
                int offset = (y * (*pWidth) + x) * 3;
                img_rgb[offset] = palette[index].rgbRed;
                img_rgb[offset + 1] = palette[index].rgbGreen;
                img_rgb[offset + 2] = palette[index].rgbBlue;
            }
        }
    }
    else if (*pBitDepth == 1) {
        int paletteSize = 2;
        for (int y = 0; y < *pHeight; y++) {
            for (int x = 0; x < *pWidth; x++) {
                int byteIndex = (*pHeight - y - 1) * ((*pWidth + 7) / 8) + x / 8;
                int bitIndex = x % 8;
                int index = (lpBits[byteIndex] >> (7 - bitIndex)) & 1;
                if (index >= paletteSize) {
                    free(pBuf);
                    return -2; // Palette index out of range
                }
                int offset = (y * (*pWidth) + x) * 3;
                img_rgb[offset] = palette[index].rgbRed;
                img_rgb[offset + 1] = palette[index].rgbGreen;
                img_rgb[offset + 2] = palette[index].rgbBlue;
            }
        }
    }
    else if (*pBitDepth == 24) {
        int w3 = (*pWidth) * 3;
        int w4 = w3 + ((*pWidth) % 4); // 每行的填充字节数
        PBYTE pDst = img_rgb;
        PBYTE pSrc = lpBits + ((*pHeight) - 1) * w4; // 源指针指向最后一行

        for (int y = *pHeight; y > 0; y--) {
            memmove(pDst, pSrc, w3); // 复制像素数据，不包括填充字节
            pSrc -= w4; // 移动源指针到下一行
            pDst += w3; // 移动目标指针到下一行
        }
    }
    else {
        free(pBuf);
        return -3; // Unsupported bit depth
    }

    free(pBuf);
    return 0;  // Success
}

int Read_12Bits_Raw(char* filename, int* pWidth, int* pHeight, unsigned short* img_data)
{
    FILE* file;

    *pWidth = 3840;
    *pHeight = 2160;
    //*pWidth = 2048;
    //*pHeight = 3072;
    int total_pixel = *pWidth * *pHeight;

    fopen_s(&file, filename, "rb");

    if (file != 0)
    {
        fread(img_data, 2, total_pixel, file);

        printf("Get Raw success!");

        fclose(file);
    }

    return 0;
}

void Raw2Gray(int width, int height, const unsigned short* raw_data, unsigned char* img_gray)
{
    int total_pixels = width * height;
    for (int i = 0; i < total_pixels; ++i)
    {
		unsigned char gray = (unsigned char)(raw_data[i] >> 4);
        img_gray[i] = gray;
	}
}

void Raw2Rgb(int width, int height, unsigned short* raw_data, unsigned char* img_rgb)
{
    int w3 = 3 * width;
    int half_h = height >> 1;
    int half_w = width >> 1;

    unsigned char* pRgb = img_rgb;
    unsigned char* pRgb_sec = img_rgb + w3;
    unsigned short* pRaw = raw_data;
    unsigned short* pRaw_sec = raw_data + width;

    for (int i = 0; i < half_h; i++)
    {
        for (int j = 0; j < half_w; j++)
        {
            unsigned char R = pRaw[0] >> 4;
            unsigned char G1 = pRaw[1] >> 4;
            unsigned char G2 = pRaw_sec[0] >> 4;
            unsigned char avg_G = ((pRaw[1] + pRaw_sec[0]) >> 4) / 3;
            unsigned char B = pRaw_sec[1] >> 4;

            unsigned char G = (G1 + G2 + avg_G + avg_G) >> 2;

            pRgb[0] = B;
            pRgb[1] = G;
            pRgb[2] = R;

            pRgb[3] = B;
            pRgb[4] = G;
            pRgb[5] = R;

            pRgb_sec[0] = B;
            pRgb_sec[1] = G;
            pRgb_sec[2] = R;

            pRgb_sec[3] = B;
            pRgb_sec[4] = G;
            pRgb_sec[5] = R;

            pRgb += 6;
            pRaw += 2;

            pRgb_sec += 6;
            pRaw_sec += 2;
        }
        pRgb += w3;
        pRaw += width;

        pRgb_sec += w3;
        pRaw_sec += width;
    }
}

void CopyImageBits(int w, int h, unsigned char* in_img, unsigned char* out_img)
{
    unsigned char* pDst = out_img;
    unsigned char* pSrc = in_img + (h - 1) * w;

    for (int y = h; y > 0; y--) {
        memmove(pDst, pSrc, w);

        pSrc -= w;
        pDst += w;
    }
}

int SaveBmp(char* BmpPath, int w_img, int h_img, unsigned char* img_rgb)
{
    FILE* fp;
    int FileOpenFlag = fopen_s(&fp, BmpPath, "wb");
    if (0 != FileOpenFlag)
    {
        return 1;
    }
    int w3 = w_img * 3;
    int bytes_img = w_img * h_img * 3;
    //unsigned char* pImg = NULL;
    static unsigned char img_buffer[2160 * 4096 * 3] = { 0 };

    if (w_img % 4) {
        int padding = (4 - (3 * w_img) % 4) % 4;
        unsigned char* p_buf = img_buffer;
        unsigned char* p_rgb = img_rgb;
        for (int i = 0; i < h_img; i++)
        {
            for (int j = 0; j < w_img; j++)
            {
                p_buf[0] = p_rgb[0];
                p_buf[1] = p_rgb[1];
                p_buf[2] = p_rgb[2];
                p_buf += 3;
                p_rgb += 3;
            }
            p_buf += padding;
        }
    }


    BITMAPINFOHEADER bih;
    memset(&bih, 0, sizeof(bih));
    bih.biSize = sizeof(bih);
    bih.biWidth = w_img;
    bih.biHeight = h_img;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = BI_RGB;
    bih.biSizeImage = 0;

    // fill the bitmap file header struct

    BITMAPFILEHEADER bfh;
    memset(&bfh, 0, sizeof(bfh));

    bfh.bfType = 0x4d42;	// "BM"
    bfh.bfSize = sizeof(bfh) + bih.biSize + bytes_img;
    bfh.bfOffBits = sizeof(bfh) + bih.biSize;

    fwrite(&bfh, sizeof(bfh), 1, fp);
    fwrite(&bih, sizeof(bih), 1, fp);

    unsigned char* out_img = (unsigned char*)malloc(bytes_img);
    ASSERT(out_img);

    CopyImageBits(w3, h_img, img_rgb, out_img);
    //ShowImage("out_img", w_img, h_img, out_img);
    fwrite(out_img, bytes_img, 1, fp);

    free(out_img);
    fclose(fp);

    return 0;
}

void Gray2Rgb(int width, int height, const unsigned char* img_data, unsigned char* img_rgb)
{
	int total_pixels = width * height;
	
    for (int i = 0; i < total_pixels; i++)
    {
        img_rgb[i * 3] = img_data[i];
        img_rgb[i * 3 + 1] = img_data[i];
        img_rgb[i * 3 + 2] = img_data[i];
    }
}

void Rgb2Gray(int width, int height, const unsigned char* img_data, unsigned char* img_gary)
{
    // 计算图像中的像素总数
    int total_pixels = width * height;
    // 计算每行的填充字节数
    int padding = (4 - (width * 3) % 4) % 4;//大于等于它的值的4的倍数

    int img_size = width * height;

    for (int i = 0; i < img_size; i++)
    {
        int idx = i * 3 + (i / width) * padding;
        const unsigned char* pRgb = &img_data[idx];
        unsigned char* pGray = &img_gary[idx];
        unsigned char B = *pRgb;
        unsigned char G = *(pRgb + 1);
        unsigned char R = *(pRgb + 2);
        short gray = (R * 19595 + G * 38469 + B * 7472) >> 16;
        if (gray > 255)
            gray = 255;
        *pGray = (unsigned char)gray;
        *(pGray + 1) = (unsigned char)gray;
        *(pGray + 2) = (unsigned char)gray;
    }
}

void RgbToGary(int width, int height, const unsigned char* img_data, unsigned char* img_gary)
{
    const unsigned char* pRgb = img_data;
    unsigned char* pGray = img_gary;
    int img_size = width * height;

    for (int i = 0; i < img_size; i++)
    {
        unsigned char B = pRgb[0];
        unsigned char G = pRgb[1];
        unsigned char R = pRgb[2];
        short gray = (R * 19595 + G * 38469 + B * 7472) >> 16;
        if (gray > 255)
            gray = 255;
        pGray[0] = (unsigned char)gray;
        pRgb += 3;
        pGray ++;
    }
}

inline unsigned char saturate(int v)
{
    if (v > 255)
        return 255;
    else if (v < 0)
        return 0;
    else
        return v;
}
void Yuv2Rgb(PBYTE pYuvBuf, int w, int h, PBYTE img_rgb) 
{
    //short w = img_width;
    //short h = img_height;
    short w_rgb = w * 3;

    PBYTE p1 = img_rgb;
    PBYTE p2 = img_rgb + w_rgb;

    PBYTE py1 = pYuvBuf;
    PBYTE py2 = py1 + w;
    PBYTE pvu = pYuvBuf + w * h;//uv分量的地址

    /*YUV转换RGB公式
    R=Y+1.402*(V-128)
    G=Y-0.344136*(U-128)-0.714136*(V-128)
    B=Y+1.772*(U-128)
    */

    for (int y = 0; y < h; y += 2) {
        for (int x = 0; x < w; x += 2) {
            short y0 = py1[0];
            short y1 = py1[1];
            short y2 = py2[0];
            short y3 = py2[1];

            short v = (short)(pvu[0] - 128);
            short u = (short)(pvu[1] - 128);

            int vr = (359 * v) >> 8;
            int vg = ((88 * u) + (183 * v)) >> 8;
            int vb = (454 * u) >> 8;

            p1[0] = saturate(y0 + vb); p1[1] = saturate(y0 - vg); p1[2] = saturate(y0 + vr);
            p1[3] = saturate(y1 + vb); p1[4] = saturate(y1 - vg); p1[5] = saturate(y1 + vr);

            p2[0] = saturate(y2 + vb); p2[1] = saturate(y2 - vg); p2[2] = saturate(y2 + vr);
            p2[3] = saturate(y3 + vb); p2[4] = saturate(y3 - vg); p2[5] = saturate(y3 + vr);

            py1 += 2;
            py2 += 2;
            pvu += 2;

            p1 += 6;
            p2 += 6;
        }
        py1 += w;
        py2 += w;
        p1 += w_rgb;
        p2 += w_rgb;
    }

}


/*
函数名: Rgb2Yuv420P
RGB转YUV公式:
Y = 0.299R + 0.587G + 0.114B;
U = -0.1684R - 0.3316G + 0.5B + 128;
V = 0.5R - 0.4187G - 0.0813B + 128;

YUV存储格式为 Y..., V..., U...
*/

#define rgb2y(p)		(BYTE)((*(p+2) * 77 + *(p+1) * 150 + *(p) * 29) >> 8)
#define rgb2u8(p)		(32768 - *(p+2) * 43 - *(p+1) * 85 + *(p) * 128)
#define rgb2v8(p)		(32768 + *(p+2) * 128 - *(p+1) * 107 - *(p) * 21)

void Rgb2Yuv420P(SIZE sz, PBYTE pRgb, PBYTE pYuv)
{
    int w = sz.cx;
    int h = sz.cy;
    ASSERT(0 == (w & 0x03) + (h & 0x03));

    int size = w * h;
    int w3 = w * 3;

    PBYTE p1 = pRgb;
    PBYTE p2 = pRgb + w3;

    PBYTE py1 = pYuv;
    PBYTE py2 = py1 + w;
    PBYTE pv = pYuv + size;
    PBYTE pu = pv + (size >> 2);	// div 4

    for (int y=0; y < h; y += 2)
    {
        for (int x = 0; x < w; w += 2)
        {
            py1[0] = rgb2y(p1);
            py1[1] = rgb2y(p1 + 3);
            py2[0] = rgb2y(p2);
            py2[1] = rgb2y(p2 + 3);

            int v = (rgb2v8(p1) + rgb2v8(p1 + 3) + rgb2v8(p2) + rgb2v8(p2 + 3)) >> 10;
            int u = (rgb2u8(p1) + rgb2u8(p1 + 3) + rgb2u8(p2) + rgb2u8(p2 + 3)) >> 10;

            *pv++ = (BYTE)v;
            *pu++ = (BYTE)u;

            p1 += 6;
            p2 += 6;

            py1 += 2;
            py2 += 2;
        }

        py1 += w;
        py2 += w;

        p1 += w3;
        p2 += w3;
    }

}


