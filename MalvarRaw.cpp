#include "pch.h"
#include "MalvarRaw.h"

inline static unsigned short saturate12bits(unsigned short value)
{
	if (value < 0)
		return 0;
	else if (value > 4095)
		return 4095;
	else
		return value;
}

void MalvarDemosaicing(int w, int h, const unsigned short* img_raw, unsigned char* img_data)
{
	int size = w * h;
	int w3 = w * 3;

	int offsets[24] = { -w * 2,-w * 2 + 1,
					-w - 1,-w,-w + 1,-w + 2,
						 -2,-1,0,1,2,3,
				  w - 2,w - 1,w,w + 1,w + 2,w + 3,
				 w * 2 - 1,w * 2,w * 2 + 1,w * 2 + 2,
						w * 3,w * 3 + 1 };

	for (int y = 2; y < h - 2; y+=2)
	{
		for (int x = 2; x < w - 2; x+=2)
		{

			int centerIndex = y * w + x;

			int indexTT1 = centerIndex + offsets[0];
			int indexTT2 = centerIndex + offsets[1];
			
			int indexT1 = centerIndex + offsets[2];
			int indexT2 = centerIndex + offsets[3];
			int indexT3 = centerIndex + offsets[4];
			int indexT4 = centerIndex + offsets[5];
			
			int indexLL = centerIndex + offsets[6];
			int indexL = centerIndex + offsets[7];
			int indexC = centerIndex + offsets[8];
			int indexR = centerIndex + offsets[9];
			int indexRR = centerIndex + offsets[10];
			int indexRRR = centerIndex + offsets[11];

			int indexBLL = centerIndex + offsets[12];
			int indexBL = centerIndex + offsets[13];
			int indexBC = centerIndex + offsets[14];
			int indexBR = centerIndex + offsets[15];
			int indexBRR = centerIndex + offsets[16];
			int indexBRRR = centerIndex + offsets[17];

			int indexBB1 = centerIndex + offsets[18];
			int indexBB2 = centerIndex + offsets[19];
			int indexBB3 = centerIndex + offsets[20];
			int indexBB4 = centerIndex + offsets[21];

			int indexBBB1 = centerIndex + offsets[22];
			int indexBBB2 = centerIndex + offsets[23];
			
			unsigned short R[6];
			unsigned short G[12];
			unsigned short B[6];

			R[0] = img_raw[indexTT1];
			R[1] = img_raw[indexLL];
			R[2] = img_raw[indexC];
			R[3] = img_raw[indexRR];
			R[4] = img_raw[indexBB2];
			R[5] = img_raw[indexBB4];

			G[0] = img_raw[indexTT2];
			G[1] = img_raw[indexT2];
			G[2] = img_raw[indexT4];
			G[3] = img_raw[indexL];
			G[4] = img_raw[indexR];
			G[5] = img_raw[indexRRR];
			G[6] = img_raw[indexBLL];
			G[7] = img_raw[indexBC];
			G[8] = img_raw[indexBRR];
			G[9] = img_raw[indexBB1];
			G[10] = img_raw[indexBB3];
			G[11] = img_raw[indexBBB1];

			B[0] = img_raw[indexT1];
			B[1] = img_raw[indexT3];
			B[2] = img_raw[indexBL];
			B[3] = img_raw[indexBR];
			B[4] = img_raw[indexBRRR];
			B[5] = img_raw[indexBBB2];

			unsigned char outR1, outG1, outB1;
			unsigned char outR2, outG2, outB2;
			unsigned char outR3, outG3, outB3;
			unsigned char outR4, outG4, outB4;
			int outIndex = (y * w + x) * 3;

			int R1, G1, B1;
			int R2, G2, B2;
			int R3, G3, B3;
			int R4, G4, B4;

			R1 = R[2];
			G1 = ((R[2] << 2) - (R[0] + R[1] + R[3] + R[4]) + ((G[1] + G[3] + G[4] + G[7]) << 1)) >> 3;
			B1 = (((R[2] * 12) - ((R[0] + R[1] + R[3] + R[4]) * 3) + ((B[0] + B[1] + B[2] + B[3]) << 2)) >> 1) >> 3;

			R2 = (((R[2] + R[3]) << 2) - (G[1] + G[2] + G[3] + G[5] + G[7] + G[8]) + ((G[0] + G[10]) >> 1) + (G[4] * 5)) >> 3;
			G2 = G[4];
			B2 = (((B[1] + B[3]) << 2) - (G[0] + G[1] + G[2] + G[7] + G[8] + G[10]) + ((G[3] + G[5]) >> 1) + (G[4] * 5)) >> 3;

			R3 = (((R[2] + R[4]) << 2) - (G[1] + G[3] + G[4] + G[9] + G[10] + G[11]) + ((G[6] + G[8]) >> 1) + (G[7] * 5)) >> 3;
			G3 = G[7];
			B3 = (((B[2] + B[3]) << 2) - (G[3] + G[4] + G[6] + G[8] + G[9] + G[10]) + ((G[1] + G[11]) >> 1) + (G[7] * 5)) >> 3;

			R4 = ((((R[2] + R[3] + R[4] + R[5]) << 2) - ((B[1] + B[2] + B[4] + B[5]) * 3) + (B[3] * 12)) >> 1) >> 3;
			G4 = ((B[3] << 2) - (B[1] + B[2] + B[4] + B[5]) + ((G[4] + G[7] + G[8] + G[10]) << 1)) >> 3;
			B4 = B[3];

			outR1 = (unsigned char)(saturate12bits(R1) >> 4);
			outR2 = (unsigned char)(saturate12bits(R2) >> 4);
			outR3 = (unsigned char)(saturate12bits(R3) >> 4);
			outR4 = (unsigned char)(saturate12bits(R4) >> 4);
			outG1 = (unsigned char)(saturate12bits(G1) >> 4);
			outG2 = (unsigned char)(saturate12bits(G2) >> 4);
			outG3 = (unsigned char)(saturate12bits(G3) >> 4);
			outG4 = (unsigned char)(saturate12bits(G4) >> 4);
			outB1 = (unsigned char)(saturate12bits(B1) >> 4);
			outB2 = (unsigned char)(saturate12bits(B2) >> 4);
			outB3 = (unsigned char)(saturate12bits(B3) >> 4);
			outB4 = (unsigned char)(saturate12bits(B4) >> 4);

			img_data[outIndex] = outB1;
			img_data[outIndex + 1] = outG1;
			img_data[outIndex + 2] = outR1;

			img_data[outIndex + 3] = outB2;
			img_data[outIndex + 4] = outG2;
			img_data[outIndex + 5] = outR2;

			img_data[outIndex + w3] = outB3;
			img_data[outIndex + w3 + 1] = outG3;
			img_data[outIndex + w3 + 2] = outR3;

			img_data[outIndex + w3 + 3] = outB4;
			img_data[outIndex + w3 + 4] = outG4;
			img_data[outIndex + w3 + 5] = outR4;


			////对第一个像素插值（R）
			//outR1 = (unsigned char)(R[2] >> 4);
			//outG1 = (unsigned char)((((R[2] << 2) - (R[0] + R[1] + R[3] + R[4]) + ((G[1] + G[3] + G[4] + G[7]) << 1)) >> 3) >> 4);
			//outB1 = (unsigned char)(((((R[2] * 12) - ((R[0] + R[1] + R[3] + R[4]) * 3) + ((B[0] + B[1] + B[2] + B[3]) << 2)) >> 1) >> 3) >> 4);

			////对第二个像素插值（G）
			//outR2 = (unsigned char)(((((R[2] + R[3]) << 2) - (G[1] + G[2] + G[3] + G[5] + G[7] + G[8]) + ((G[0] + G[10]) >> 1) + (G[4] * 5)) >> 3) >> 4);
			//outG2 = (unsigned char)(G[4] >> 4);
			//outB2 = (unsigned char)(((((B[1] + B[3]) << 2) - (G[0] + G[1] + G[2] + G[7] + G[8] + G[10]) + ((G[3] + G[5]) >> 1) + (G[4] * 5)) >> 3) >> 4);

			////对第三个像素插值（G）
			//outR3 = (unsigned char)(((((R[2] + R[4]) << 2) - (G[1] + G[3] + G[4] + G[9] + G[10] + G[11]) + ((G[6] + G[8]) >> 1) + (G[7] * 5)) >> 3) >> 4);
			//outG3 = (unsigned char)(G[7] >> 4);
			//outB3 = (unsigned char)(((((B[2] + B[3]) << 2) - (G[3] + G[4] + G[6] + G[8] + G[9] + G[10]) + ((G[1] + G[11]) >> 1) + (G[7] * 5)) >> 3) >> 4);

			////对第四个像素插值（B）
			//outR4 = (unsigned char)((((((R[2] + R[3] + R[4] + R[5]) << 2) - ((B[1] + B[2] + B[4] + B[5]) * 3) + (B[3] * 12)) >> 1) >> 3) >> 4);
			//outG4 = (unsigned char)((((B[3] << 2) - (B[1] + B[2] + B[4] + B[5]) + ((G[4] + G[7] + G[8] + G[10]) << 1)) >> 3) >> 4);
			//outB4 = (unsigned char)(B[3] >> 4);

			//img_data[outIndex] = saturate(outB1);
			//img_data[outIndex + 1] = saturate(outG1);
			//img_data[outIndex + 2] = saturate(outR1);

			//img_data[outIndex + 3] = saturate(outB2);
			//img_data[outIndex + 4] = saturate(outG2);
			//img_data[outIndex + 5] = saturate(outR2);

			//img_data[outIndex + w3] = saturate(outB3);
			//img_data[outIndex + w3 + 1] = saturate(outG3);
			//img_data[outIndex + w3 + 2] = saturate(outR3);

			//img_data[outIndex + w3 + 3] = saturate(outB4);
			//img_data[outIndex + w3 + 4] = saturate(outG4);
			//img_data[outIndex + w3 + 5] = saturate(outR4);

		}
	}

}

inline int saturate(int value)
{
	if (value < 0)
		return 0;
	else if (value > 255)
		return 255;
	else
		return value;
}

void AWB(int w, int h, unsigned char* in_img, unsigned char* out_img)
{
	unsigned long long sumB = 0, sumG = 0, sumR = 0;
	int size = w * h;

	for (int i = 0; i < size; i++)
	{
		sumB += in_img[i * 3];
		sumG += in_img[i * 3 + 1];
		sumR += in_img[i * 3 + 2];
	}

	unsigned long avgB = sumB / size;
	unsigned long avgG = sumG / size;
	unsigned long avgR = sumR / size;

	double gainR = (double)avgG / avgR;
	double gainB = (double)avgG / avgB;

	for (int i = 0; i < size; i++)
	{
		/*int B = in_img[i * 3] * gainB;
		int G = in_img[i * 3 + 1];
		int R = in_img[i * 3 + 2] * gainR;*/

		int B = in_img[i * 3] * 1.9503;
		int G = in_img[i * 3 + 1];
		int R = in_img[i * 3 + 2] * 1.6973;

		out_img[i * 3] = (unsigned char)saturate(B);
		out_img[i * 3 + 1] = (unsigned char)saturate(G);
		out_img[i * 3 + 2] = (unsigned char)saturate(R);

	}

}


