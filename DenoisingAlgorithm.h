#pragma once

void createGaussianSpatialLUT(double sigma, int radius);

void createGaussianRangeLUT(double sigma);

//˫���˲�
void bilateralFilter(int width, int height, int radius, double sigmaI, double sigmaS, unsigned short* inputImg, unsigned short* outputImg);


