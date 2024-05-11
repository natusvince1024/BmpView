#pragma once


void MalvarDemosaicing(int w, int h, const unsigned short* img_raw, unsigned char* img_data);

void AWB(int w, int h, unsigned char* in_img, unsigned char* out_img);

//gamma±ä»»
void Raw_GammaTransform(int width, int height, float gamma, const unsigned char* inputImg, unsigned char* outImg);