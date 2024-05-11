#include "pch.h"
#include "ISP.h"


inline unsigned short RGB2Y(int R, int G, int B)
{
    unsigned short Y = (38 * R + 75 * G + 15 * B) >> 7;
    return Y;
}

inline short RGB2U(int R, int G, int B)
{
    short U = (-43 * R - 85 * G + 128 * B) >> 8;
    return U;
}

inline short RGB2V(int R, int G, int B)
{
    short V = (128 * R - 107 * G - 21 * B) >> 8;
    return V;
}

inline unsigned short saturate_12bit(int v)
{
    if (v > 4095) v = 4095;
    else if (v < 0) v = 0;
    return v;
}

static float WB_Gain[120 * 135 * 3] = { 0 };

void GI(unsigned short* img_raw)
{
    int w = 3840;
    int h = 2160;

    unsigned short* raw_up = img_raw;
    unsigned short* raw_down = img_raw + w;

    for (int y = 0; y < (h >> 1); y++)
    {
        for (int x = 0; x < (w >> 1); x++)
        {
            unsigned short G1 = raw_up[1];
            unsigned short G2 = raw_down[0];

            unsigned short G = (G1 + G2) / 2;

            //raw_up[0] = raw_up[0] - 186;
            raw_up[1] = G;
            raw_down[0] = G;
            /*raw_up[1] = raw_up[1] - 186;
            raw_down[0] = raw_down[0] - 186;*/
            //raw_down[1] = raw_down[1] - 186;

            raw_up += 2;
            raw_down += 2;
        }
        raw_up += w;
        raw_down += w;
    }
}

void Gaussian_BNR(unsigned short* in_img)
{
    static unsigned short raw_bnr[3840 * 2160] = { 0 };

    int w = 3840;
    int h = 2160;

    unsigned short* p_raw1 = in_img;
    unsigned short* p_raw2 = p_raw1 + w;
    unsigned short* p_raw3 = p_raw2 + w;
    unsigned short* p_raw4 = p_raw3 + w;
    unsigned short* p_raw5 = p_raw4 + w;

    unsigned short* p_bnr1 = raw_bnr + w * 2;
    unsigned short* p_bnr2 = raw_bnr + w * 3;

    for (int j = 0; j < ((h - 4) >> 1); j++)
    {
        p_bnr1[0] = p_raw3[0];
        p_bnr1[1] = p_raw3[1];
        p_bnr2[0] = p_raw4[0];
        p_bnr2[1] = p_raw4[1];

        p_bnr1 += 2;
        p_bnr2 += 2;
        for (int i = 0; i < ((w - 4) >> 1); i++)
        {
            unsigned short v1 = p_raw1[0];
            unsigned short v2 = p_raw1[2];
            unsigned short v3 = p_raw1[3];
            unsigned short v4 = p_raw1[4];
            unsigned short v5 = p_raw2[1];
            unsigned short v6 = p_raw2[2];
            unsigned short v7 = p_raw2[3];
            unsigned short v8 = p_raw2[4];
            unsigned short v9 = p_raw2[5];
            unsigned short v10 = p_raw3[0];
            unsigned short v11 = p_raw3[1];
            unsigned short v12 = p_raw3[2];
            unsigned short v13 = p_raw3[3];
            unsigned short v14 = p_raw3[4];
            unsigned short v15 = p_raw3[5];
            unsigned short v16 = p_raw4[0];
            unsigned short v17 = p_raw4[1];
            unsigned short v18 = p_raw4[2];
            unsigned short v19 = p_raw4[3];
            unsigned short v20 = p_raw4[4];
            unsigned short v21 = p_raw4[5];
            unsigned short v22 = p_raw5[0];
            unsigned short v23 = p_raw5[1];
            unsigned short v24 = p_raw5[2];
            unsigned short v25 = p_raw5[3];
            unsigned short v26 = p_raw5[4];
            unsigned short v27 = p_raw5[1];
            unsigned short v28 = p_raw5[2];
            unsigned short v29 = p_raw5[3];
            unsigned short v30 = p_raw5[5];

            p_bnr1[0] = (v1 + v2 * 2 + v4 + v10 * 2 + v12 * 4 + v14 * 2 + v22 + v24 * 2 + v26) >> 4;
            p_bnr1[1] = (v3 + v6 * 2 + v8 + v11 * 2 + v13 * 4 + v15 * 2 + v18 + v20 * 2 + v25) >> 4;
            p_bnr2[0] = (v6 + v11 * 2 + v13 + v16 * 2 + v18 * 4 + v20 * 2 + v23 + v25 * 2 + v28) >> 4;
            p_bnr2[1] = (v5 + v7 * 2 + v9 + v17 * 2 + v19 * 4 + v21 * 2 + v27 + v29 * 2 + v30) >> 4;

            /*p_bnr1[0] = (v1 + v2 * 2 + v4 + v10 * 2 + v12 * 4 + v14 * 2 + v22 + v24 * 2 + v26) >> 4;
            p_bnr1[1] = (v3 * 2 + v6 * 3 + v8 * 3 + v11 * 2 + v13 * 4 + v15 * 2 + v18 * 3 + v20 * 3 + v25 * 2) /24;
            p_bnr2[0] = (v6 * 2 + v11 * 3 + v13 * 3 + v16 * 2 + v18 * 4 + v20 * 2 + v23 * 3 + v25 * 3 + v28 * 2)/24;
            p_bnr2[1] = (v5 + v7 * 2 + v9 + v17 * 2 + v19 * 4 + v21 * 2 + v27 + v29 * 2 + v30) >> 4;*/

            p_bnr1 += 2;
            p_bnr2 += 2;

            p_raw1 += 2;
            p_raw2 += 2;
            p_raw3 += 2;
            p_raw4 += 2;
            p_raw5 += 2;
        }

        p_bnr1[0] = p_raw3[2];
        p_bnr1[1] = p_raw3[3];
        p_bnr2[0] = p_raw4[2];
        p_bnr2[1] = p_raw4[3];

        p_bnr1 += (2 + w);
        p_bnr2 += (2 + w);

        p_raw1 += (4 + w);
        p_raw2 += (4 + w);
        p_raw3 += (4 + w);
        p_raw4 += (4 + w);
        p_raw5 += (4 + w);
    }

    memmove(in_img, raw_bnr, w * h * 2);
    memset(raw_bnr, 0, w * h * 2);
}

//void Gaussian_gradient_BNR(unsigned short* in_img)
//{
//    static unsigned short raw_bnr[3840 * 2160] = { 0 };
//
//    int w = 3840;
//    int h = 2160;
//
//    unsigned short* p_raw1 = in_img;
//    unsigned short* p_raw2 = p_raw1 + w;
//    unsigned short* p_raw3 = p_raw2 + w;
//    unsigned short* p_raw4 = p_raw3 + w;
//    unsigned short* p_raw5 = p_raw4 + w;
//    unsigned short* p_raw6 = p_raw5 + w;
//
//    unsigned short* p_bnr1 = raw_bnr + w * 2;
//    unsigned short* p_bnr2 = raw_bnr + w * 3;
//
//    for (int j = 0; j < ((h - 4) >> 1); j++)
//    {
//        p_bnr1[0] = p_raw3[0];
//        p_bnr1[1] = p_raw3[1];
//        p_bnr2[0] = p_raw4[0];
//        p_bnr2[1] = p_raw4[1];
//
//        p_bnr1 += 2;
//        p_bnr2 += 2;
//        for (int i = 0; i < ((w - 4) >> 1); i++)
//        {
//            unsigned short v1 = p_raw1[2];
//            unsigned short v2 = p_raw1[3];
//            unsigned short v3 = p_raw2[2];
//            unsigned short v4 = p_raw2[3];
//            unsigned short v5 = p_raw3[0];
//            unsigned short v6 = p_raw3[1];
//            unsigned short v7 = p_raw3[2];
//            unsigned short v8 = p_raw3[3];
//            unsigned short v9 = p_raw3[4];
//            unsigned short v10 = p_raw3[5];
//            unsigned short v11 = p_raw4[0];
//            unsigned short v12 = p_raw4[1];
//            unsigned short v13 = p_raw4[2];
//            unsigned short v14 = p_raw4[3];
//            unsigned short v15 = p_raw4[4];
//            unsigned short v16 = p_raw4[5];
//            unsigned short v17 = p_raw5[2];
//            unsigned short v18 = p_raw5[3];
//            unsigned short v19 = p_raw6[2];
//            unsigned short v20 = p_raw6[3];
//
//
//            p_bnr1[0] = (v1 + v2 * 2 + v4 + v10 * 2 + v12 * 4 + v14 * 2 + v22 + v24 * 2 + v26) >> 4;
//            p_bnr1[1] = (v3 + v6 * 2 + v8 + v11 * 2 + v13 * 4 + v15 * 2 + v18 + v20 * 2 + v25) >> 4;
//            p_bnr2[0] = (v6 + v11 * 2 + v13 + v16 * 2 + v18 * 4 + v20 * 2 + v23 + v25 * 2 + v28) >> 4;
//            p_bnr2[1] = (v5 + v7 * 2 + v9 + v17 * 2 + v19 * 4 + v21 * 2 + v27 + v29 * 2 + v30) >> 4;
//
//            /*p_bnr1[0] = (v1 + v2 * 2 + v4 + v10 * 2 + v12 * 4 + v14 * 2 + v22 + v24 * 2 + v26) >> 4;
//            p_bnr1[1] = (v3 * 2 + v6 * 3 + v8 * 3 + v11 * 2 + v13 * 4 + v15 * 2 + v18 * 3 + v20 * 3 + v25 * 2) /24;
//            p_bnr2[0] = (v6 * 2 + v11 * 3 + v13 * 3 + v16 * 2 + v18 * 4 + v20 * 2 + v23 * 3 + v25 * 3 + v28 * 2)/24;
//            p_bnr2[1] = (v5 + v7 * 2 + v9 + v17 * 2 + v19 * 4 + v21 * 2 + v27 + v29 * 2 + v30) >> 4;*/
//
//            p_bnr1 += 2;
//            p_bnr2 += 2;
//
//            p_raw1 += 2;
//            p_raw2 += 2;
//            p_raw3 += 2;
//            p_raw4 += 2;
//            p_raw5 += 2;
//        }
//
//        p_bnr1[0] = p_raw3[2];
//        p_bnr1[1] = p_raw3[3];
//        p_bnr2[0] = p_raw4[2];
//        p_bnr2[1] = p_raw4[3];
//
//        p_bnr1 += (2 + w);
//        p_bnr2 += (2 + w);
//
//        p_raw1 += (4 + w);
//        p_raw2 += (4 + w);
//        p_raw3 += (4 + w);
//        p_raw4 += (4 + w);
//        p_raw5 += (4 + w);
//    }
//
//    memmove(in_img, raw_bnr, w * h * 2);
//    memset(raw_bnr, 0, w * h * 2);
//}

//int BNR_3D(const char* Path, int* w, int* h, unsigned short* in_img)
//{
//    Path = "E:\\test_image\\928\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229162153.raw";
//    static unsigned short img_raw1[3840 * 2160] = { 0 };
//    static unsigned short img_raw2[3840 * 2160] = { 0 };
//    static unsigned short img_raw3[3840 * 2160] = { 0 };
//    static unsigned short img_raw4[3840 * 2160] = { 0 };
//    static unsigned short img_raw5[3840 * 2160] = { 0 };
//    static unsigned short img_raw6[3840 * 2160] = { 0 };
//    static unsigned short img_raw7[3840 * 2160] = { 0 };
//    static unsigned short img_raw8[3840 * 2160] = { 0 };
//
//    ReadRaw12BPP("E:\\test_image\\928\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229162153.raw", w, h, img_raw1);
//    ReadRaw12BPP("E:\\test_image\\928\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229162159.raw", w, h, img_raw2);
//    ReadRaw12BPP("E:\\test_image\\928\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229162206.raw", w, h, img_raw3);
//    ReadRaw12BPP("E:\\test_image\\928\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229162212.raw", w, h, img_raw4);
//    ReadRaw12BPP("E:\\test_image\\928\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229162219.raw", w, h, img_raw5);
//    ReadRaw12BPP("E:\\test_image\\928\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229162146.raw", w, h, img_raw6);
//    ReadRaw12BPP("E:\\test_image\\928\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229162140.raw", w, h, img_raw7);
//    ReadRaw12BPP("E:\\test_image\\928\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229162133.raw", w, h, img_raw8);
//
//    for (int i = 0; i < *w * *h; i++)
//    {
//        //in_img[i] = (img_raw1[i] + img_raw2[i] + img_raw3[i] + img_raw4[i] + img_raw5[i] + img_raw6[i] + img_raw7[i] + img_raw8[i]) >> 3;
//        //in_img[i] = (img_raw1[i] + img_raw2[i] + img_raw3[i] + img_raw4[i]) >> 2;
//        in_img[i] = (img_raw1[i] + img_raw2[i]) >> 1;
//    }
//
//    return 0;
//}


void RGB12bits2Y(unsigned short* img_rgb, unsigned short* img_Y)
{
    int size = 3840 * 2160;

    unsigned short* pRgb = img_rgb;

    for (int i = 0; i < size; i++)
    {
        unsigned short R = pRgb[2];
        unsigned short G = pRgb[1];
        unsigned short B = pRgb[0];

        img_Y[i] = RGB2Y(R, G, B);

        pRgb += 3;
    }
}


void Laplace_Y(unsigned short* img_Y, short* img_Laplace)
{
    int w = 3840;
    int h = 2160;

    unsigned short* pY_up = img_Y;
    unsigned short* pY = img_Y + w;
    unsigned short* pY_down = img_Y + 2 * w;
    short* pLaplace = img_Laplace + w;

    for (int j = 0; j < (h - 2); j++)
    {
        pLaplace[0] = pY[0];
        pLaplace++;
        for (int i = 0; i < (w - 2); i++)
        {
            pLaplace[0] = 8 * pY[1] - (pY_up[0] + pY_up[1] + pY_up[2] + pY[0] + pY[2] + pY_down[0] + pY_down[1] + pY_down[2]);

            pY_up++;
            pY++;
            pY_down++;
            pLaplace++;
        }
        pLaplace[0] = pY[0];

        pLaplace++;
        pY_up += 2;
        pY += 2;
        pY_down += 2;
    }
}

void Sobel_Y(unsigned short* img_Y, short* img_Sobel)
{
    /*int gx[3][3] = {
        { -1, 0, 1 },
        { -2, 0, 2 },
        { -1, 0, 1 }
    };
    int gy[3][3] = {
        { -1, -2, -1 },
        {  0,  0,  0 },
        {  1,  2,  1 }
    };*/

    int w = 3840;
    int h = 2160;

    unsigned short* pY_up = img_Y;
    unsigned short* pY = img_Y + w;
    unsigned short* pY_down = img_Y + 2 * w;
    short* pSobel = img_Sobel + w;

    for (int j = 0; j < (h - 2); j++)
    {
        pSobel[0] = pY[0];
        pSobel++;
        for (int i = 0; i < (w - 2); i++)
        {
            int sum_x = pY_down[0] + 2 * pY_down[1] + pY_down[2] - (pY_up[0] + 2 * pY_up[1] + pY_up[2]);
            int sum_y = pY_up[2] + 2 * pY[2] + pY_down[2] - (pY_up[0] + 2 * pY[0] + pY_down[0]);

            int sum_xy = sum_x * sum_x + sum_y * sum_y;
            pSobel[0] = saturate_12bit((int)sqrt(sum_xy));

            pY_up++;
            pY++;
            pY_down++;
            pSobel++;
        }
        pSobel[0] = pY[0];

        pSobel++;
        pY_up += 2;
        pY += 2;
        pY_down += 2;
    }

}

void Vague_Sobel_Y(short* img_Sobel)
{
    static short img_VSobel[3840 * 2160] = { 0 };

    int w = 3840;
    int h = 2160;
    int size = w * h;

    short* pY_1 = img_Sobel;
    short* pY_2 = img_Sobel + w;
    short* pY_3 = img_Sobel + 2 * w;
    short* pY_4 = img_Sobel + 3 * w;
    short* pY_5 = img_Sobel + 4 * w;

    short* pVSobel = img_VSobel + 2 * w;

    for (int j = 0; j < (h - 4); j++)
    {
        pVSobel[0] = pY_3[0];
        pVSobel[1] = pY_3[1];
        pVSobel += 2;
        for (int i = 0; i < (w - 4); i++)
        {
            pVSobel[0] = (pY_1[0] + pY_1[1] + pY_1[2] * 2 + pY_1[3] + pY_1[4]
                + pY_2[0] + pY_2[1] * 3 + pY_2[2] * 3 + pY_2[3] * 3 + pY_2[4]
                + pY_3[0] * 2 + pY_3[1] * 3 + pY_3[2] * 4 + pY_3[3] * 3 + pY_3[4] * 2
                + pY_4[0] + pY_4[1] * 3 + pY_4[2] * 3 + pY_4[3] * 3 + pY_4[4]
                + pY_5[0] + pY_5[1] + pY_5[2] * 2 + pY_5[3] + pY_5[4]) / 48;

            pY_1++;
            pY_2++;
            pY_3++;
            pY_4++;
            pY_5++;

            pVSobel++;
        }
        pVSobel[0] = pY_3[2];
        pVSobel[1] = pY_3[3];

        pVSobel += 2;

        pY_1 += 4;
        pY_2 += 4;
        pY_3 += 4;
        pY_4 += 4;
        pY_5 += 4;
    }

    memmove(img_Sobel, img_VSobel, size * sizeof(short));
    memset(img_VSobel, 0, size * sizeof(short));
}

void Laplace_Sobel_Y(short* img_Laplace, short* img_Sobel, short* img_edge)
{
    int size = 3840 * 2160;

    for (int i = 0; i < size; i++)
    {
        int fusion = img_Laplace[0] * img_Sobel[0];
        img_edge[0] = (short)pow(fusion, 0.5);
        /*int fusion = 0;
        if (img_Laplace[0] == 0 || img_Sobel[0] == 0)
            fusion = 0;
        else
            fusion = (int)(0.5f * (float)img_Laplace[0] + 0.5f * (float)img_Sobel[0]);
        img_edge[0] = fusion;*/

        img_Laplace++;
        img_Sobel++;
        img_edge++;
    }
}

void Show_edge(short* img_Y, unsigned short* img_edge)
{
    int size = 3840 * 2160;

    unsigned short* p_edge = img_edge;

    for (int i = 0; i < size; i++)
    {
        unsigned short value = abs(img_Y[i]);
        //unsigned short value = saturate_12bit(img_Y[i]);
        p_edge[0] = value;
        p_edge[1] = value;
        p_edge[2] = value;

        p_edge += 3;
        //img_Y++;
    }
}

void Gaussian_NR(unsigned short* img_rgb)
{
    int w = 3840;
    int h = 2160;
    int size = w * h;

    static unsigned short img_Gau[3840 * 2160 * 3] = { 0 };

    unsigned short* pRgb_1 = img_rgb;
    unsigned short* pRgb_2 = pRgb_1 + w * 3;
    unsigned short* pRgb_3 = pRgb_2 + w * 3;
    unsigned short* pRgb_4 = pRgb_3 + w * 3;
    unsigned short* pRgb_5 = pRgb_4 + w * 3;

    unsigned short* pGau = img_Gau + w * 3 * 2;

    for (int j = 0; j < (h - 4); j++)
    {
        pGau[0] = pRgb_1[0];
        pGau[1] = pRgb_1[1];
        pGau[2] = pRgb_1[2];
        pGau[3] = pRgb_1[3];
        pGau[4] = pRgb_1[4];
        pGau[5] = pRgb_1[5];
        pGau += 6;

        for (int i = 0; i < (w - 4); i++)
        {
            pGau[0] = (pRgb_1[0] + pRgb_1[3] + pRgb_1[6] * 2 + pRgb_1[9] + pRgb_1[12]
                + pRgb_2[0] + pRgb_2[3] * 3 + pRgb_2[6] * 3 + pRgb_2[9] * 3 + pRgb_2[12]
                + pRgb_3[0] * 2 + pRgb_3[3] * 3 + pRgb_3[6] * 4 + pRgb_3[9] * 3 + pRgb_3[12] * 2
                + pRgb_4[0] + pRgb_4[3] * 3 + pRgb_4[6] * 3 + pRgb_4[9] * 3 + pRgb_4[12]
                + pRgb_5[0] + pRgb_5[3] + pRgb_5[6] * 2 + pRgb_5[9] + pRgb_5[12]) / 48;

            pGau[1] = (pRgb_1[1] + pRgb_1[3] + pRgb_1[6] * 2 + pRgb_1[9] + pRgb_1[12]
                + pRgb_2[1] + pRgb_2[3] * 3 + pRgb_2[6] * 3 + pRgb_2[9] * 3 + pRgb_2[12]
                + pRgb_3[1] * 2 + pRgb_3[3] * 3 + pRgb_3[6] * 4 + pRgb_3[9] * 3 + pRgb_3[12] * 2
                + pRgb_4[1] + pRgb_4[3] * 3 + pRgb_4[6] * 3 + pRgb_4[9] * 3 + pRgb_4[12]
                + pRgb_5[1] + pRgb_5[3] + pRgb_5[6] * 2 + pRgb_5[9] + pRgb_5[12]) / 48;

            pGau[2] = (pRgb_1[0] + pRgb_1[3] + pRgb_1[6] * 2 + pRgb_1[9] + pRgb_1[12]
                + pRgb_2[0] + pRgb_2[3] * 3 + pRgb_2[6] * 3 + pRgb_2[9] * 3 + pRgb_2[12]
                + pRgb_3[0] * 2 + pRgb_3[3] * 3 + pRgb_3[6] * 4 + pRgb_3[9] * 3 + pRgb_3[12] * 2
                + pRgb_4[0] + pRgb_4[3] * 3 + pRgb_4[6] * 3 + pRgb_4[9] * 3 + pRgb_4[12]
                + pRgb_5[0] + pRgb_5[3] + pRgb_5[6] * 2 + pRgb_5[9] + pRgb_5[12]) / 48;

            pGau += 3;
            pRgb_1 += 3;
            pRgb_2 += 3;
            pRgb_3 += 3;
            pRgb_4 += 3;
            pRgb_5 += 3;
        }

        pGau[0] = pRgb_1[0];
        pGau[1] = pRgb_1[1];
        pGau[2] = pRgb_1[2];
        pGau[3] = pRgb_1[3];
        pGau[4] = pRgb_1[4];
        pGau[5] = pRgb_1[5];

        pGau += 6;
        pRgb_1 += 15;
        pRgb_2 += 15;
        pRgb_3 += 15;
        pRgb_4 += 15;
        pRgb_5 += 15;
    }

    memmove(img_rgb, img_Gau, size * sizeof(unsigned short));
    memset(img_Gau, 0, size * sizeof(unsigned short));

    //Show12bitImage("Gau_NR", 3840, 2160, img_rgb);
}

//img_rgb:rgb通道,img_edge:Y通道,out_img:rgb通道
void Show_12bit_Sharpen(unsigned short* img_rgb, short* img_edge, unsigned short* out_img)
{
    int size = 3840 * 2160;

    unsigned short* p_out = out_img;

    for (int i = 0; i < size; i++)
    {
        p_out[0] = saturate_12bit(img_rgb[0] + img_edge[0]);
        p_out[1] = saturate_12bit(img_rgb[1] + img_edge[0]);
        p_out[2] = saturate_12bit(img_rgb[2] + img_edge[0]);

        img_rgb += 3;
        img_edge++;

        p_out += 3;
    }
}

void CCM(unsigned short* in_img, unsigned short* out_img)
{
    /*float CCM[9] = { 1.613, -0.871, 0.258,
                    -0.223, 1.254, 0.031,
                    0.059, -0.766, 1.707 };*/

                    /*float CCM[9] = { 1.949, -0.855, -0.094,
                                    -0.352, 1.465, -0.113,
                                    0.258, -1.625, 2.367 };*/

    float CCM[9] = { 1.672f, -0.984f, 0.313f,
                    -0.262f, 1.305f, -0.043f,
                    0.145f, -1.047f, 1.902f };

    int size = 3840 * 2160;

    for (int i = 0; i < size; i++)
    {
        float R = (float)in_img[2];
        float G = (float)in_img[1];
        float B = (float)in_img[0];
        out_img[0] = saturate_12bit((short)(CCM[6] * R + CCM[7] * G + CCM[8] * B));
        out_img[1] = saturate_12bit((short)(CCM[3] * R + CCM[4] * G + CCM[5] * B));
        out_img[2] = saturate_12bit((short)(CCM[0] * R + CCM[1] * G + CCM[2] * B));

        in_img += 3;
        out_img += 3;
    }
}

void RawWB(unsigned short* img_raw)
{
    int w = 3840;
    int h = 2160;

    int half_h = h >> 1;
    int half_w = w >> 1;

    unsigned short* pRaw = img_raw;
    unsigned short* pRaw_sec = img_raw + w;

    unsigned int sum_r = 0;
    unsigned int sum_g = 0;
    unsigned int sum_b = 0;
    unsigned int sum_y = 0;
    unsigned short Y = 0;

    for (int i = 0; i < half_h; i++)
    {
        for (int j = 0; j < half_w; j++)
        {
            unsigned short R = pRaw[0];
            unsigned short G = (pRaw[1] + pRaw_sec[0]) >> 1;
            unsigned short B = pRaw_sec[1];

            Y = RGB2Y(R, G, B);

            sum_r += pRaw[0];
            sum_g += pRaw[1];
            sum_g += pRaw_sec[0];
            sum_b += pRaw_sec[1];
            sum_y += Y;

            pRaw += 2;
            pRaw_sec += 2;
        }
        pRaw += w;
        pRaw_sec += w;
    }

    int size = w * h;
    int avg_r = sum_r / (size >> 2);
    int avg_g = sum_g / (size >> 1);
    int avg_b = sum_b / (size >> 2);

    float gain_R = (float)avg_g / (float)avg_r;
    float gain_B = (float)avg_g / (float)avg_b;

    int avg_y = sum_y / (size >> 2);

    float gain_r = (float)avg_y / (float)avg_r;
    float gain_g = (float)avg_y / (float)avg_g;
    float gain_b = (float)avg_y / (float)avg_b;

    pRaw = img_raw;
    pRaw_sec = img_raw + w;

    for (int i = 0; i < half_h; i++)
    {
        for (int j = 0; j < half_w; j++)
        {
            /*pRaw[0] = (unsigned short)((float)pRaw[0] * gain_r);

            pRaw[1] = (unsigned short)((float)pRaw[1] * gain_g);

            pRaw_sec[0] = (unsigned short)((float)pRaw_sec[0] * gain_g);

            pRaw_sec[1] = (unsigned short)((float)pRaw_sec[1] * gain_b);*/


            pRaw[0] = saturate_12bit((unsigned short)((float)pRaw[0] * gain_r));

            pRaw[1] = saturate_12bit((unsigned short)((float)pRaw[1] * gain_g));

            pRaw_sec[0] = saturate_12bit((unsigned short)((float)pRaw_sec[0] * gain_g));

            pRaw_sec[1] = saturate_12bit((unsigned short)((float)pRaw_sec[1] * gain_b));


            /*pRaw[0] = saturate_12bit((unsigned short)((float)pRaw[0] * 1.204f));

            pRaw[1] = saturate_12bit((unsigned short)((float)pRaw[1] * 0.857f));

            pRaw_sec[0] = saturate_12bit((unsigned short)((float)pRaw_sec[0] * 0.857f));

            pRaw_sec[1] = saturate_12bit((unsigned short)((float)pRaw_sec[1] * 1.651f));*/

            pRaw += 2;
            pRaw_sec += 2;
        }
        pRaw += w;
        pRaw_sec += w;
    }
}

void Raw12bit_Y_Square_HE(unsigned short* img_raw)
{
    //Show12bitImage("Original", 3840, 2160, img_raw);

    unsigned short* pRaw = img_raw;

    static int Hist[4096] = { 0 };
    static int s_Hist[4096] = { 0 };
    static float w_Hist[4096] = { 0 };
    static float Pi_sum[4096] = { 0 };
    static unsigned short Y[3840 * 2160] = { 0 };
    static int Pi_tab[4096] = { 0 };

    int size = 3840 * 2160;

    for (int i = 0; i < size; i++)
    {
        unsigned short R = pRaw[2];
        unsigned short G = pRaw[1];
        unsigned short B = pRaw[0];

        Y[i] = RGB2Y(R, G, B);

        Hist[Y[i]]++;
        pRaw += 3;
    }

    int sum_s = 0;
    for (int i = 0; i < 4096; i++)
    {
        int y_num = Hist[i];
        s_Hist[i] = (int)pow(y_num, 0.333);
        sum_s += s_Hist[i];//开方后的像素总数
    }

    for (int i = 0; i < 4096; i++)
    {
        w_Hist[i] = (float)s_Hist[i] / (float)sum_s;
    }

    Pi_sum[0] = w_Hist[0];
    Pi_tab[0] = (int)(Pi_sum[0] * 4096.0f - 0.5f);

    for (int i = 1; i < 4096; i++)
    {
        Pi_sum[i] = Pi_sum[i - 1] + w_Hist[i];
        Pi_tab[i] = ((int)(Pi_sum[i] * 4096.0f - 0.5f));
    }

    //ShowSingleChart("HistCurve", Hist);
    //ShowSingleChart("ObjectHistCurve", s_Hist);
    //ShowSingleChart("HE_Chart", Pi_tab);

    pRaw = img_raw;
    for (int i = 0; i < size; i++)
    {
        int Y_HE = Pi_tab[Y[i]];
        int Y_diff = Y_HE - Y[i];

        pRaw[2] = saturate_12bit(pRaw[2] + Y_diff);
        pRaw[1] = saturate_12bit(pRaw[1] + Y_diff);
        pRaw[0] = saturate_12bit(pRaw[0] + Y_diff);

        pRaw += 3;
    }

    //Show12bitImage("Y_HE", 3840, 2160, img_raw);

    memset(Hist, 0, 4096 * sizeof(int));
    memset(s_Hist, 0, 4096 * sizeof(int));
    memset(w_Hist, 0, 4096 * sizeof(float));
    memset(Pi_sum, 0, 4096 * sizeof(float));
    memset(Y, 0, 3840 * 2160 * sizeof(unsigned short));
    memset(Pi_tab, 0, 4096 * sizeof(int));

}

void Raw_Y_gamma(float gamma, unsigned short* img_raw)
{
    int size = 3840 * 2160;

    unsigned short* pRaw = img_raw;
    unsigned short gamma_tab[4096] = { 0 };

    for (int i = 0; i < 4096; i++)
    {
        gamma_tab[i] = (unsigned short)(pow(i / 4096.0f, gamma) * 4096);
    }

    for (int i = 0; i < size; i++)
    {
        unsigned short R = pRaw[2];
        unsigned short G = pRaw[1];
        unsigned short B = pRaw[0];

        int Y = RGB2Y(R, G, B);
        int Y_Gamma = gamma_tab[Y];
        int Y_diff = Y_Gamma - Y;

        pRaw[2] = saturate_12bit(pRaw[2] + Y_diff);
        pRaw[1] = saturate_12bit(pRaw[1] + Y_diff);
        pRaw[0] = saturate_12bit(pRaw[0] + Y_diff);

        pRaw += 3;
    }
}

void High_quality_interpolation(unsigned short* img_raw, unsigned short* img_rgb)
{
    int w = 3840;
    int h = 2160;
    //int w = 2048;
    //int h = 3072;

    unsigned short* pRaw1 = img_raw;
    unsigned short* pRaw2 = img_raw + w;
    unsigned short* pRaw3 = img_raw + 2 * w;
    unsigned short* pRaw4 = img_raw + 3 * w;
    unsigned short* pRaw5 = img_raw + 4 * w;
    unsigned short* pRaw6 = img_raw + 5 * w;

    unsigned short* pRgb = img_rgb + (2 * w + 2) * 3;
    unsigned short* pRgb_sec = pRgb + w * 3;

    int half_h_4 = (h - 4) >> 1;
    int half_w_4 = (w - 4) >> 1;

    int w_add_4 = w + 4;

    for (int j = 0; j < half_h_4; j++)
    {
        for (int i = 0; i < half_w_4; i++)
        {
            unsigned short v1 = pRaw1[2];
            unsigned short v2 = pRaw1[3];
            unsigned short v3 = pRaw2[1];
            unsigned short v4 = pRaw2[2];
            unsigned short v5 = pRaw2[3];
            unsigned short v6 = pRaw2[4];
            unsigned short v7 = pRaw3[0];
            unsigned short v8 = pRaw3[1];
            unsigned short v9 = pRaw3[2];
            unsigned short v10 = pRaw3[3];
            unsigned short v11 = pRaw3[4];
            unsigned short v12 = pRaw3[5];
            unsigned short v13 = pRaw4[0];
            unsigned short v14 = pRaw4[1];
            unsigned short v15 = pRaw4[2];
            unsigned short v16 = pRaw4[3];
            unsigned short v17 = pRaw4[4];
            unsigned short v18 = pRaw4[5];
            unsigned short v19 = pRaw5[1];
            unsigned short v20 = pRaw5[2];
            unsigned short v21 = pRaw5[3];
            unsigned short v22 = pRaw5[4];
            unsigned short v23 = pRaw6[2];
            unsigned short v24 = pRaw6[3];

            int R = 0;
            int G = 0;
            int B = 0;

            B = (((v3 + v5 + v14 + v16) << 2) - 3 * (v1 + v7 + v11 + v20) + 12 * v9) >> 4;
            G = (((v4 + v8 + v10 + v15) << 1) - (v1 + v7 + v11 + v20) + (v9 << 2)) >> 3;
            R = v9;

            pRgb[0] = saturate_12bit(B);
            pRgb[1] = saturate_12bit(G);
            pRgb[2] = saturate_12bit(R);

            /*pRgb[0] = B;
            pRgb[1] = G;
            pRgb[2] = R;*/

            B = ((v8 + v12) - ((v2 + v4 + v6 + v15 + v17 + v21) << 1) + ((v5 + v16) << 3) + 10 * v10) >> 4;
            G = v10;
            R = ((v2 + v21) - ((v4 + v6 + v8 + v12 + v15 + v17) << 1) + ((v9 + v11) << 3) + 10 * v10) >> 4;

            pRgb[3] = saturate_12bit(B);
            pRgb[4] = saturate_12bit(G);
            pRgb[5] = saturate_12bit(R);

            /*pRgb[3] = B;
            pRgb[4] = G;
            pRgb[5] = R;*/

            B = ((v4 + v23) - ((v8 + v10 + v13 + v17 + v19 + v21) << 1) + ((v14 + v16) << 3) + 10 * v15) >> 4;
            G = v15;
            R = ((v13 + v17) - ((v4 + v8 + v10 + v19 + v21 + v23) << 1) + ((v9 + v20) << 3) + 10 * v15) >> 4;

            pRgb_sec[0] = saturate_12bit(B);
            pRgb_sec[1] = saturate_12bit(G);
            pRgb_sec[2] = saturate_12bit(R);

            /*pRgb_sec[0] = B;
            pRgb_sec[1] = G;
            pRgb_sec[2] = R;*/

            B = v16;
            G = (((v10 + v15 + v17 + v21) << 1) - (v5 + v14 + v18 + v24) + (v16 << 2)) >> 3;
            R = (((v9 + v11 + v20 + v22) << 2) - 3 * (v5 + v14 + v18 + v24) + 12 * v16) >> 4;

            pRgb_sec[3] = saturate_12bit(B);
            pRgb_sec[4] = saturate_12bit(G);
            pRgb_sec[5] = saturate_12bit(R);

            /* pRgb_sec[3] = B;
             pRgb_sec[4] = G;
             pRgb_sec[5] = R;*/

            pRaw1 += 2;
            pRaw2 += 2;
            pRaw3 += 2;
            pRaw4 += 2;
            pRaw5 += 2;
            pRaw6 += 2;

            pRgb += 6;
            pRgb_sec += 6;
        }

        pRaw1 += w_add_4;
        pRaw2 += w_add_4;
        pRaw3 += w_add_4;
        pRaw4 += w_add_4;
        pRaw5 += w_add_4;
        pRaw6 += w_add_4;

        pRgb += w_add_4 * 3;
        pRgb_sec += w_add_4 * 3;
    }
}

void Rgb_12bit_to_8bit(unsigned short* img_rgb_12bit, unsigned char* img_rgb_8bit)
{
    int w = 3840;
    int h = 2160;

    for (int i = 0; i < w * h * 3; i++)
    {
        //img_rgb_8bit[i] = saturate(img_rgb_12bit[i] >> 4);
        img_rgb_8bit[i] = img_rgb_12bit[i] >> 4;
    }
}

void Y2RGB(int w, int h, unsigned short* img_src, unsigned short* img_dst, unsigned short* img_rgb)
{
    int size = w * h;
    unsigned short* pRgb = img_rgb;

    for (int i = 0; i < size; i++)
    {
        int Y_diff = img_dst[i] - img_src[i];
        pRgb[0] = saturate_12bit(pRgb[0] + Y_diff);
        pRgb[1] = saturate_12bit(pRgb[1] + Y_diff);
        pRgb[2] = saturate_12bit(pRgb[2] + Y_diff);
        pRgb += 3;
    }
}

void RGB12bit_to_UV(int w, int h, unsigned short* img_rgb, short* img_uv)
{
    unsigned short* pRgb1 = img_rgb;
    unsigned short* pRgb2 = img_rgb + w * 3;

    short* pUV = img_uv;

    for (int j = 0; j < (h >> 1); j++)
    {
        for (int i = 0; i < (w >> 1); i++)
        {
            unsigned short R1 = pRgb1[2];
            unsigned short G1 = pRgb1[1];
            unsigned short B1 = pRgb1[0];

            unsigned short R2 = pRgb1[5];
            unsigned short G2 = pRgb1[4];
            unsigned short B2 = pRgb1[3];

            unsigned short R3 = pRgb2[2];
            unsigned short G3 = pRgb2[1];
            unsigned short B3 = pRgb2[0];

            unsigned short R4 = pRgb2[5];
            unsigned short G4 = pRgb2[4];
            unsigned short B4 = pRgb2[3];

            int u1 = RGB2U(R1, G1, B1);
            int u2 = RGB2U(R2, G2, B2);
            int u3 = RGB2U(R3, G3, B3);
            int u4 = RGB2U(R4, G4, B4);

            int v1 = RGB2V(R1, G1, B1);
            int v2 = RGB2V(R2, G2, B2);
            int v3 = RGB2V(R3, G3, B3);
            int v4 = RGB2V(R4, G4, B4);

            pUV[0] = (u1 + u2 + u3 + u4) >> 2;
            pUV[1] = (v1 + v2 + v3 + v4) >> 2;

            pRgb1 += 6;
            pRgb2 += 6;
            pUV += 2;
        }
        pRgb1 += 3 * w;
        pRgb2 += 3 * w;
    }
}

void YUV12bit_to_RGB12bit(int w, int h, unsigned short* img_Y, short* img_uv, unsigned short* img_rgb)
{
    unsigned short* pY1 = img_Y;
    unsigned short* pY2 = img_Y + w;
    short* pUV = img_uv;

    unsigned short* pRgb1 = img_rgb;
    unsigned short* pRgb2 = img_rgb + w * 3;

    for (int j = 0; j < (h >> 1); j++)
    {
        for (int i = 0; i < (w >> 1); i++)
        {
            int Y1 = pY1[0];
            int Y2 = pY1[1];
            int Y3 = pY2[0];
            int Y4 = pY2[1];

            int u = pUV[0];
            int v = pUV[1];

            int R_offset = (180 * v) >> 7;
            int G_offset = (44 * u + 92 * v) >> 7;
            int B_offset = (228 * u) >> 7;

            short R1 = Y1 + R_offset;
            short G1 = Y1 - G_offset;
            short B1 = Y1 + B_offset;

            short R2 = Y2 + R_offset;
            short G2 = Y2 - G_offset;
            short B2 = Y2 + B_offset;

            short R3 = Y3 + R_offset;
            short G3 = Y3 - G_offset;
            short B3 = Y3 + B_offset;

            short R4 = Y4 + R_offset;
            short G4 = Y4 - G_offset;
            short B4 = Y4 + B_offset;

            pRgb1[0] = saturate_12bit(B1);
            pRgb1[1] = saturate_12bit(G1);
            pRgb1[2] = saturate_12bit(R1);

            pRgb1[3] = saturate_12bit(B2);
            pRgb1[4] = saturate_12bit(G2);
            pRgb1[5] = saturate_12bit(R2);

            pRgb2[0] = saturate_12bit(B3);
            pRgb2[1] = saturate_12bit(G3);
            pRgb2[2] = saturate_12bit(R3);

            pRgb2[3] = saturate_12bit(B4);
            pRgb2[4] = saturate_12bit(G4);
            pRgb2[5] = saturate_12bit(R4);

            pY1 += 2;
            pY2 += 2;
            pUV += 2;
            pRgb1 += 6;
            pRgb2 += 6;
        }
        pY1 += w;
        pY2 += w;
        pRgb1 += w * 3;
        pRgb2 += w * 3;
    }
}

void UV_NR(int w, int h, short* in_uv, short* out_uv)
{
    short* pIn1 = in_uv;
    short* pIn2 = pIn1 + w;
    short* pIn3 = pIn2 + w;

    short* pOut = out_uv;

    //上边缘
    for (int i = 0; i < w; i++)
    {
        pOut[0] = in_uv[i];
        pOut++;
    }

    for (int j = 0; j < ((h - 2) >> 1); j++)
    {
        pOut[0] = pIn2[0];
        pOut[1] = pIn2[1];
        pOut += 2;

        for (int i = 0; i < ((w - 4) >> 1); i++)
        {
            int u1 = pIn1[2];
            int v1 = pIn1[3];

            int u2 = pIn2[0];
            int v2 = pIn2[1];

            int u3 = pIn2[2];
            int v3 = pIn2[3];

            int u4 = pIn2[4];
            int v4 = pIn2[5];

            int u5 = pIn3[2];
            int v5 = pIn3[3];

            int diff1 = abs(u1 - u3) + abs(v1 - v3);
            int diff2 = abs(u2 - u3) + abs(v2 - v3);
            int diff4 = abs(u4 - u3) + abs(v4 - v3);
            int diff5 = abs(u5 - u3) + abs(v5 - v3);

            int w1 = 0;
            int w2 = 0;
            int w4 = 0;
            int w5 = 0;

            int threshold = 500;

            if (diff1 > threshold)
                w1 = 0;
            else
                w1 = 1;

            if (diff2 > threshold)
                w2 = 0;
            else
                w2 = 1;

            if (diff4 > threshold)
                w4 = 0;
            else
                w4 = 1;

            if (diff5 > threshold)
                w5 = 0;
            else
                w5 = 1;

            int u = (w1 * u1 + w2 * u2 + u3 + w4 * u4 + w5 * u5) / (1 + w1 + w2 + w4 + w5);
            int v = (w1 * v1 + w2 * v2 + v3 + w4 * v4 + w5 * v5) / (1 + w1 + w2 + w4 + w5);

            pOut[0] = u;
            pOut[1] = v;

            pIn1 += 2;
            pIn2 += 2;
            pIn3 += 2;
            pOut += 2;
        }
        pOut[0] = pIn2[2];
        pOut[1] = pIn2[3];
        pOut += 2;

        pIn1 += 4;
        pIn2 += 4;
        pIn3 += 4;
    }

    //底边缘
    pOut = out_uv + (h - 1) * w;
    for (int i = 0; i < w; i++)
    {
        pOut[i] = pIn2[i];
    }
}

void HLD_Demosaic(int w, int h, unsigned short* img_raw, unsigned short* img_rgb)
{
    unsigned short* pRaw1 = img_raw + w;
    unsigned short* pRaw2 = img_raw + 2 * w;
    unsigned short* pRaw3 = img_raw + 3 * w;
    unsigned short* pRaw4 = img_raw + 4 * w;
    unsigned short* pRaw5 = img_raw + 5 * w;
    unsigned short* pRaw6 = img_raw + 6 * w;
    unsigned short* pRaw7 = img_raw + 7 * w;
    unsigned short* pRaw8 = img_raw + 8 * w;

    unsigned short* pRgb = img_rgb + (4 * w + 4) * 3;
    unsigned short* pRgb_sec = pRgb + w * 3;

    int half_h_8 = (h - 8) >> 1;
    int half_w_8 = (w - 8) >> 1;

    int w_add_8 = w + 8;

    for (int j = 0; j < half_h_8; j++)
    {
        for (int i = 0; i < half_w_8; i++)
        {
            float w_h = 0;
            float w_v = 0;

            int R_h = 0;
            int R_v = 0;
            int G_h = 0;
            int G_v = 0;
            int B_h = 0;
            int B_v = 0;

            int R = 0;
            int G = 0;
            int B = 0;

            unsigned short v1 = pRaw1[3];
            unsigned short v2 = pRaw1[4];
            unsigned short v3 = pRaw1[5];
            unsigned short v4 = pRaw1[6];
            unsigned short v5 = pRaw2[3];
            unsigned short v6 = pRaw2[4];
            unsigned short v7 = pRaw2[5];
            unsigned short v8 = pRaw2[6];
            unsigned short v9 = pRaw3[1];
            unsigned short v10 = pRaw3[2];
            unsigned short v11 = pRaw3[3];
            unsigned short v12 = pRaw3[4];
            unsigned short v13 = pRaw3[5];
            unsigned short v14 = pRaw3[6];
            unsigned short v15 = pRaw3[7];
            unsigned short v16 = pRaw3[8];
            unsigned short v17 = pRaw4[1];
            unsigned short v18 = pRaw4[2];
            unsigned short v19 = pRaw4[3];
            unsigned short v20 = pRaw4[4];
            unsigned short v21 = pRaw4[5];
            unsigned short v22 = pRaw4[6];
            unsigned short v23 = pRaw4[7];
            unsigned short v24 = pRaw4[8];
            unsigned short v25 = pRaw5[1];
            unsigned short v26 = pRaw5[2];
            unsigned short v27 = pRaw5[3];
            unsigned short v28 = pRaw5[4];
            unsigned short v29 = pRaw5[5];
            unsigned short v30 = pRaw5[6];
            unsigned short v31 = pRaw5[7];
            unsigned short v32 = pRaw5[8];
            unsigned short v33 = pRaw6[2];
            unsigned short v34 = pRaw6[3];
            unsigned short v35 = pRaw6[4];
            unsigned short v36 = pRaw6[5];
            unsigned short v37 = pRaw6[6];
            unsigned short v38 = pRaw6[7];
            unsigned short v39 = pRaw7[3];
            unsigned short v40 = pRaw7[4];
            unsigned short v41 = pRaw7[5];
            unsigned short v42 = pRaw7[6];
            unsigned short v43 = pRaw8[3];
            unsigned short v44 = pRaw8[4];
            unsigned short v45 = pRaw8[5];
            unsigned short v46 = pRaw8[6];

            unsigned short v47 = pRaw6[1];
            unsigned short v48 = pRaw6[8];

            //计算水平梯度与垂直梯度
            int h_11 = abs(2 * v11 - v9 - v13);
            int h_12 = abs(2 * v12 - v10 - v14);
            int h_13 = abs(2 * v13 - v11 - v15);
            int h_14 = abs(2 * v14 - v12 - v16);

            int h_19 = abs(2 * v19 - v17 - v21);
            int h_20 = abs(2 * v20 - v18 - v22);
            int h_21 = abs(2 * v21 - v19 - v23);
            int h_22 = abs(2 * v22 - v20 - v24);

            int h_27 = abs(2 * v27 - v25 - v29);
            int h_28 = abs(2 * v28 - v26 - v30);
            int h_29 = abs(2 * v29 - v27 - v31);
            int h_30 = abs(2 * v30 - v28 - v32);

            int h_34 = abs(2 * v34 - v47 - v36);
            int h_35 = abs(2 * v35 - v33 - v37);
            int h_36 = abs(2 * v36 - v34 - v38);
            int h_37 = abs(2 * v37 - v35 - v48);

            int v_11 = abs(2 * v11 - v1 - v27);
            int v_12 = abs(2 * v12 - v2 - v28);
            int v_13 = abs(2 * v13 - v3 - v29);
            int v_14 = abs(2 * v14 - v4 - v30);

            int v_19 = abs(2 * v19 - v5 - v34);
            int v_20 = abs(2 * v20 - v6 - v35);
            int v_21 = abs(2 * v21 - v7 - v36);
            int v_22 = abs(2 * v22 - v8 - v37);

            int v_27 = abs(2 * v27 - v11 - v39);
            int v_28 = abs(2 * v28 - v12 - v40);
            int v_29 = abs(2 * v29 - v13 - v41);
            int v_30 = abs(2 * v30 - v14 - v42);

            int v_34 = abs(2 * v34 - v19 - v43);
            int v_35 = abs(2 * v35 - v20 - v44);
            int v_36 = abs(2 * v36 - v21 - v45);
            int v_37 = abs(2 * v37 - v22 - v46);

            //R:插值G,B
            int V_v20 = ((v_11 + v_13 + v_27 + v_29) + 2 * (v_12 + v_19 + v_21 + v_28) + 4 * v_20) >> 4;
            int V_h20 = ((h_11 + h_13 + h_27 + h_29) + 2 * (h_12 + h_19 + h_21 + h_28) + 4 * h_20) >> 4;

            w_h = 1.0f / (1.0f + (float)(exp(0.05f * (V_h20 - V_v20))));
            w_v = 1.0f - w_h;

            G_h = (8 * v20 + 7 * (v19 + v21) - 4 * (v18 + v22) + (v17 + v23)) >> 4;
            G_v = (8 * v20 + 7 * (v12 + v28) - 4 * (v6 + v35) + (v2 + v40)) >> 4;

            B_h = (8 * v20 + 7 * (v19 + v21) - 4 * (v18 + v22) + (v17 + v23)
                - 2 * (v10 + v14 + v26 + v30) - 4 * (v12 + v28) + 4 * (v11 + v13 + v27 + v29)) >> 4;
            B_v = (8 * v20 + 7 * (v12 + v28) - 4 * (v6 + v35) + (v2 + v40)
                - 2 * (v5 + v7 + v34 + v36) - 4 * (v19 + v21) + 4 * (v11 + v13 + v27 + v29)) >> 4;

            R = v20;
            G = (int)((float)G_h * w_h + (float)G_v * w_v);
            B = (int)((float)B_h * w_h + (float)B_v * w_v);

            pRgb[0] = saturate_12bit(B);
            pRgb[1] = saturate_12bit(G);
            pRgb[2] = saturate_12bit(R);

            //G1:插值R,B
            int V_v21 = ((v_12 + v_14 + v_28 + v_30) + 2 * (v_13 + v_20 + v_22 + v_29) + 4 * v_21) >> 4;
            int V_h21 = ((h_12 + h_14 + h_28 + h_30) + 2 * (h_13 + h_20 + h_22 + h_29) + 4 * h_21) >> 4;

            w_h = 1.0f / (1.0f + (float)(exp(0.05f * (V_h21 - V_v21))));
            w_v = 1.0f - w_h;

            B_h = (((v11 + v15 + v27 + v31) >> 1) - (v12 + v14 + v28 + v30) + (v13 + v29) + 4 * v21) >> 2;
            B_v = ((v3 + v41) - 4 * (v7 + v36) + 7 * (v13 + v29) + 8 * v21) >> 4;

            R_h = ((v18 + v24) - 4 * (v19 + v23) + 7 * (v20 + v22) + 8 * v21) >> 4;
            R_v = (((v6 + v8 + v35 + v37) >> 1) - (v12 + v14 + v28 + v30) + (v20 + v22) + 4 * v21) >> 2;

            R = (int)((float)R_h * w_h + (float)R_v * w_v);
            G = v21;
            B = (int)((float)B_h * w_h + (float)B_v * w_v);

            pRgb[3] = saturate_12bit(B);
            pRgb[4] = saturate_12bit(G);
            pRgb[5] = saturate_12bit(R);

            //G2:插值R,B
            int V_v28 = ((v_19 + v_21 + v_34 + v_36) + 2 * (v_20 + v_27 + v_29 + v_35) + 4 * v_28) >> 4;
            int V_h28 = ((h_19 + h_21 + h_34 + h_36) + 2 * (h_20 + h_27 + h_29 + h_35) + 4 * h_28) >> 4;

            w_h = 1.0f / (1.0f + (float)(exp(0.05f * (V_h28 - V_v28))));
            w_v = 1.0f - w_h;

            B_h = ((v25 + v31) - 4 * (v26 + v30) + 7 * (v27 + v29) + 8 * v28) >> 4;
            B_v = (((v11 + v13 + v39 + v41) >> 1) - (v19 + v21 + v34 + v36) + (v27 + v29) + 4 * v28) >> 2;

            R_h = (((v18 + v22 + v33 + v37) >> 1) - (v19 + v21 + v34 + v36) + (v20 + v35) + 4 * v28) >> 2;
            R_v = ((v6 + v44) - 4 * (v12 + v40) + 7 * (v20 + v35) + 8 * v28) >> 4;

            R = (int)((float)R_h * w_h + (float)R_v * w_v);
            G = v28;
            B = (int)((float)B_h * w_h + (float)B_v * w_v);

            pRgb_sec[0] = saturate_12bit(B);
            pRgb_sec[1] = saturate_12bit(G);
            pRgb_sec[2] = saturate_12bit(R);

            //B:插值R,G
            int V_v29 = ((v_20 + v_22 + v_35 + v_37) + 2 * (v_21 + v_28 + v_30 + v_36) + 4 * v_29) >> 4;
            int V_h29 = ((h_20 + h_22 + h_35 + h_37) + 2 * (h_21 + h_28 + h_30 + h_36) + 4 * h_29) >> 4;

            w_h = 1.0f / (1.0f + (float)(exp(0.05f * (V_h29 - V_v29))));
            w_v = 1.0f - w_h;

            G_h = (8 * v29 + 7 * (v28 + v30) - 4 * (v27 + v31) + (v26 + v32)) >> 4;
            G_v = (8 * v29 + 7 * (v21 + v36) - 4 * (v13 + v41) + (v7 + v45)) >> 4;

            R_h = (8 * v29 + 7 * (v28 + v30) - 4 * (v27 + v31) + (v26 + v32)
                - 2 * (v19 + v23 + v34 + v38) - 4 * (v21 + v36) + 4 * (v20 + v22 + v35 + v37)) >> 4;
            R_v = (8 * v29 + 7 * (v21 + v36) - 4 * (v13 + v41) + (v7 + v45)
                - 2 * (v12 + v14 + v40 + v42) - 4 * (v28 + v30) + 4 * (v20 + v22 + v35 + v37)) >> 4;

            R = (int)((float)R_h * w_h + (float)R_v * w_v);
            G = (int)((float)G_h * w_h + (float)G_v * w_v);
            B = v29;

            pRgb_sec[3] = saturate_12bit(B);
            pRgb_sec[4] = saturate_12bit(G);
            pRgb_sec[5] = saturate_12bit(R);

            pRaw1 += 2;
            pRaw2 += 2;
            pRaw3 += 2;
            pRaw4 += 2;
            pRaw5 += 2;
            pRaw6 += 2;
            pRaw7 += 2;
            pRaw8 += 2;

            pRgb += 6;
            pRgb_sec += 6;
        }

        pRaw1 += w_add_8;
        pRaw2 += w_add_8;
        pRaw3 += w_add_8;
        pRaw4 += w_add_8;
        pRaw5 += w_add_8;
        pRaw6 += w_add_8;
        pRaw7 += w_add_8;
        pRaw8 += w_add_8;

        pRgb += w_add_8 * 3;
        pRgb_sec += w_add_8 * 3;
    }
}

void WB_Manual(int w, int h, unsigned short* img_raw, float gain_r, float gain_b)
{
    int half_h = h >> 1;
    int half_w = w >> 1;

    unsigned short* pRaw = img_raw;
    unsigned short* pRaw_sec = img_raw + w;

    for (int i = 0; i < half_h; i++)
    {
        for (int j = 0; j < half_w; j++)
        {
            pRaw[0] = saturate_12bit((unsigned short)((float)pRaw[0] * gain_r));

            pRaw[1] = pRaw[1];

            pRaw_sec[0] = pRaw_sec[0];

            pRaw_sec[1] = saturate_12bit((unsigned short)((float)pRaw_sec[1] * gain_b));

            pRaw += 2;
            pRaw_sec += 2;
        }
        pRaw += w;
        pRaw_sec += w;
    }
}

void AWB(int w, int h, int nx, int ny, unsigned short* img_raw)
{

    int nw = (int)((float)w / (float)nx + 0.5f);
    int nh = (int)((float)h / (float)ny + 0.5f);
    int nsize = nw * nh;
    unsigned short* p_raw = img_raw;

    FILE* fp;
    int FileOpenFlag = fopen_s(&fp, "E:\\MFCImage_cfg\\AWB_Data.txt", "rb");
    if (fp)
    {
        fread(WB_Gain, sizeof(float), 120 * 135 * 3, fp);
        fclose(fp);
    }
    float* pGain = WB_Gain;

    for (int j = 0; j < ny; j++)
    {
        for (int i = 0; i < nx; i++)
        {
            for (int y = 0; y < nh; y += 2)
            {
                for (int x = 0; x < nw; x += 2)
                {
                    int pos = j * nh * w + i * nw + y * w + x;
                    float gain_r = pGain[0];
                    float gain_g = pGain[1];
                    float gain_b = pGain[2];
                    img_raw[pos] = (unsigned short)((float)img_raw[pos] * gain_r);
                    img_raw[pos + 1] = (unsigned short)((float)img_raw[pos + 1] * gain_g);
                    img_raw[pos + w] = (unsigned short)((float)img_raw[pos + w] * gain_g);
                    img_raw[pos + w + 1] = (unsigned short)((float)img_raw[pos + w + 1] * gain_b);
                }
            }

            pGain += 3;
        }
    }
}

void GammaTransform_RGB12bit(int w, int h, float gamma, unsigned short* in_img, unsigned short* out_img)
{
    unsigned short GammaTable[4096];
    float gammaCorrection = 1.0f / gamma;

    for (int i = 0; i < 4096; i++)
    {
        GammaTable[i] = (unsigned short)(pow(i / 4095.0f, gammaCorrection) * 4095.0f);
    }

    int totalPixels = w * h * 3;

    for (int i = 0; i < totalPixels; i++)
    {
        out_img[i] = GammaTable[in_img[i]];
    }

}

inline int MeanSort(unsigned short* value)
{
    int lenth = 9;
    int sum = 0;

    for (int i = 0; i < lenth; i++)
    {
        sum += value[i];
    }

    int result = sum / 9;

    return result;
}
void MeanFiltering(int w, int h, unsigned short* in_img, unsigned short* out_img)
{
    int offsets[9] = { -w - 1,-w,-w + 1,
                           -1,0,1,
                        w - 1,w,w + 1 };

    for (int y = 1; y < h - 1; y++)
    {
        for (int x = 1; x < w - 1; x++)
        {
            unsigned short Y[9] = { 0 };

            int centerIndex = y * w + x;

            int indexTL = centerIndex + offsets[0];
            int indexTC = centerIndex + offsets[1];
            int indexTR = centerIndex + offsets[2];
            int indexCL = centerIndex + offsets[3];
            int indexCC = centerIndex + offsets[4];
            int indexCR = centerIndex + offsets[5];
            int indexBL = centerIndex + offsets[6];
            int indexBC = centerIndex + offsets[7];
            int indexBR = centerIndex + offsets[8];

            Y[0] = in_img[indexTL];
            Y[1] = in_img[indexTC];
            Y[2] = in_img[indexTR];
            Y[3] = in_img[indexCL];
            Y[4] = in_img[indexCC];
            Y[5] = in_img[indexCR];
            Y[6] = in_img[indexBL];
            Y[7] = in_img[indexBC];
            Y[8] = in_img[indexBR];

            int outIndex = y * w + x;

            out_img[outIndex] = MeanSort(Y);
        }
    }
}

inline int AssignmentSort(unsigned short* value)
{
    int lenth = 8;
    int sum = 0;

    for (int i = 0; i < lenth; i++)
    {
        sum += value[i];
    }

    int result = sum >> 3;

    return result;
}
void EightNeighborhoodAssignment(int w, int h, unsigned short* in_img, unsigned short* out_img)
{
    int offsets[9] = { -w - 1,-w,-w + 1,
                           -1,0,1,
                        w - 1,w,w + 1 };

    for (int y = 1; y < h - 1; y++)
    {
        for (int x = 1; x < w - 1; x++)
        {
            unsigned short Y[8] = { 0 };

            int centerIndex = y * w + x;

            int indexTL = centerIndex + offsets[0];
            int indexTC = centerIndex + offsets[1];
            int indexTR = centerIndex + offsets[2];
            int indexCL = centerIndex + offsets[3];
            //int indexCC = centerIndex + offsets[4];
            int indexCR = centerIndex + offsets[5];
            int indexBL = centerIndex + offsets[6];
            int indexBC = centerIndex + offsets[7];
            int indexBR = centerIndex + offsets[8];

            Y[0] = in_img[indexTL];
            Y[1] = in_img[indexTC];
            Y[2] = in_img[indexTR];
            Y[3] = in_img[indexCL];
            Y[4] = in_img[indexCR];
            Y[5] = in_img[indexBL];
            Y[6] = in_img[indexBC];
            Y[7] = in_img[indexBR];

            int outIndex = y * w + x;

            out_img[outIndex] = AssignmentSort(Y);
        }
    }
}

inline int FourAssignmentSort(unsigned short* value)
{
    int lenth = 4;
    int sum = 0;

    for (int i = 0; i < lenth; i++)
    {
        sum += value[i];
    }

    int result = sum >> 2;

    return result;
}
void FourNeighborhoodAssignment(int w, int h, unsigned short* in_img, unsigned short* out_img)
{
    int offsets[5] = { -w,
                     -1,0,1,
                        w,};

    for (int y = 1; y < h - 1; y++)
    {
        for (int x = 1; x < w - 1; x++)
        {
            unsigned short Y[4] = { 0 };

            int centerIndex = y * w + x;

            int indexT = centerIndex + offsets[0];
            int indexL = centerIndex + offsets[1];
            int indexR = centerIndex + offsets[3];
            int indexB = centerIndex + offsets[4];


            Y[0] = in_img[indexT];
            Y[1] = in_img[indexL];
            Y[2] = in_img[indexR];
            Y[3] = in_img[indexB];


            int outIndex = y * w + x;

            out_img[outIndex] = FourAssignmentSort(Y);
        }
    }
}

/*
* K = [0,1,0,
       1,-4,1,
       0,1,0]
*/
void LaplaceSharpening_12bits(int w, int h, const unsigned short* in_img, unsigned short* out_img)
{
    int kernel[9] = { 0,1,0,
                     1,-4,1,
                     0,1,0 };

    int offsets[5] = {  -w,
                      -1,0,1,
                         w };

    for (int y = 1; y < h - 1; y++)
    {
        for (int x = 1; x < w - 1; x++)
        {
            int sumY = 0;

            int centerIndex = y * w + x;

            int indexT = centerIndex + offsets[0];
            int indexL = centerIndex + offsets[1];
            int indexC = centerIndex + offsets[2];
            int indexR = centerIndex + offsets[3];
            int indexB = centerIndex + offsets[4];

            sumY = ((in_img[indexC] * 4 - (in_img[indexT] + in_img[indexL] + in_img[indexR] + in_img[indexB]))<<3);

            out_img[indexC] = saturate_12bit(sumY);

        }
    }

}
