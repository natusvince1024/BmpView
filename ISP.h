void GI(unsigned short* img_raw);

void Gaussian_BNR(unsigned short* in_img);

//int BNR_3D(const char* Path, int* w, int* h, unsigned short* in_img);

void RGB12bits2Y(unsigned short* img_rgb, unsigned short* img_Y);

void Laplace_Y(unsigned short* img_Y, short* img_Laplace);

void Sobel_Y(unsigned short* img_Y, short* img_Sobel);

void Vague_Sobel_Y(short* img_Sobel);

void Laplace_Sobel_Y(short* img_Laplace, short* img_Sobel, short* img_edge);

void Show_edge(short* img_Y, unsigned short* img_edge);

void Gaussian_NR(unsigned short* img_rgb);

void Show_12bit_Sharpen(unsigned short* img_rgb, short* img_edge, unsigned short* out_img);

void CCM(unsigned short* in_img, unsigned short* out_img);

void RawWB(unsigned short* img_raw);

void Raw12bit_Y_Square_HE(unsigned short* img_raw);

void Raw_Y_gamma(float gamma, unsigned short* img_raw);

void High_quality_interpolation(unsigned short* img_raw, unsigned short* img_rgb);

void Rgb_12bit_to_8bit(unsigned short* img_rgb_12bit, unsigned char* img_rgb_8bit);

void Y2RGB(int w, int h, unsigned short* img_src, unsigned short* img_dst, unsigned short* img_rgb);

void RGB12bit_to_UV(int w, int h, unsigned short* img_rgb, short* img_uv);

void YUV12bit_to_RGB12bit(int w, int h, unsigned short* img_Y, short* img_uv, unsigned short* img_rgb);

void UV_NR(int w, int h, short* in_uv, short* out_uv);

void HLD_Demosaic(int w, int h, unsigned short* img_raw, unsigned short* img_rgb);

void WB_Manual(int w, int h, unsigned short* img_raw, float gain_r, float gain_b);

void AWB(int w, int h, int nx, int ny, unsigned short* img_raw);

void GammaTransform_RGB12bit(int w, int h, float gamma, unsigned short* in_img, unsigned short* out_img);

void MeanFiltering(int w, int h, unsigned short* in_img, unsigned short* out_img);

void EightNeighborhoodAssignment(int w, int h, unsigned short* in_img, unsigned short* out_img);

void FourNeighborhoodAssignment(int w, int h, unsigned short* in_img, unsigned short* out_img);

// Laplace»ÒªØ
void LaplaceSharpening_12bits(int w, int h, const unsigned short* in_img, unsigned short* out_img);

extern "C" void cudaISP(int w, int h, unsigned short* img_bayer, unsigned short* img_rgb, float* elapsedTime);


