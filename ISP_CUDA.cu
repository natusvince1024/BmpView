#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <assert.h>

#define HANDLE_ERROR(A) assert(A == cudaSuccess) 


__global__ void BayerProcess(int w, int h, unsigned short* in_img, float wb_r, float wb_b, int BlackLevel, unsigned short* out_img)
{
    int x = threadIdx.x + blockIdx.x * blockDim.x;//当前行
    int y = threadIdx.y + blockIdx.y * blockDim.y;//当前列


    int idx = x + y * w;

    out_img[idx] = in_img[idx] - BlackLevel;

    int xidx = x % 2;
    int yidx = y % 2;

    //R
    if ((xidx == 0) && (yidx == 0))
        out_img[idx] = out_img[idx] * wb_r;

    //B
    if ((xidx == 1) && (yidx == 1))
        out_img[idx] = out_img[idx] * wb_b;

}

__global__ void BayerGray(int w, int h, unsigned short* in_img, unsigned short* out_img)
{
    int x = threadIdx.x + blockIdx.x * blockDim.x;//当前行
    int y = threadIdx.y + blockIdx.y * blockDim.y;//当前列


    int idx = x + y * w;

    unsigned short pixel = in_img[idx];
    unsigned short* pout = out_img + (idx * 3);

    pout[0] = pixel;
    pout[1] = pixel;
    pout[2] = pixel;

    /*out_img[idx] = pixel;
    out_img[idx + 1] = pixel;
    out_img[idx + 2] = pixel;*/

}

__inline__ __device__ unsigned short saturate12bits(int value)
{
    if (value < 0)
        return 0;
    else if (value > 4095)
        return 4095;
    else
        return value;
}
__global__ void GPUMalvarDemosaicing(int w, int h, unsigned short* img_raw, unsigned short* img_data)
{
	int x = 2 * threadIdx.x + (blockIdx.x * blockDim.x << 1);
	int y = 2 * threadIdx.y + (blockIdx.y * blockDim.y << 1);

	int w3 = w * 3;

	int offsets[24] = { -w * 2,-w * 2 + 1,
					-w - 1,-w,-w + 1,-w + 2,
						 -2,-1,0,1,2,3,
				  w - 2,w - 1,w,w + 1,w + 2,w + 3,
				 w * 2 - 1,w * 2,w * 2 + 1,w * 2 + 2,
						w * 3,w * 3 + 1 };

	int centerIndex = x + y * w;
	int outIndex = (y * w + x) * 3;

	if ((x > 1) && (x < w - 1) && (y > 1) && (y < h - 1))
	{
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

		unsigned short outR1, outG1, outB1;
		unsigned short outR2, outG2, outB2;
		unsigned short outR3, outG3, outB3;
		unsigned short outR4, outG4, outB4;

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

		outR1 = saturate12bits(R1);
		outR2 = saturate12bits(R2);
		outR3 = saturate12bits(R3);
		outR4 = saturate12bits(R4);
		outG1 = saturate12bits(G1);
		outG2 = saturate12bits(G2);
		outG3 = saturate12bits(G3);
		outG4 = saturate12bits(G4);
		outB1 = saturate12bits(B1);
		outB2 = saturate12bits(B2);
		outB3 = saturate12bits(B3);
		outB4 = saturate12bits(B4);

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
	}
}

extern "C" cudaError_t cudaISP(int w, int h, unsigned short* img_bayer, unsigned short* img_rgb, float* elapsedTime)
{
    unsigned short* dev_img_bayer = 0;
    unsigned short* dev_img_rgb = 0;
    cudaError_t cudaStatus;

    dim3 blocks((w + 15) / 16, (h + 15) / 16);
    dim3 threads(16, 16);

	dim3 blocks2((w + 31) / 32, (h + 31) / 32);

    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    HANDLE_ERROR(cudaStatus);

    // Allocate GPU buffers for three vectors (two input, one output)    .
    cudaStatus = cudaMalloc((void**)&dev_img_bayer, w * h * sizeof(unsigned short));
    HANDLE_ERROR(cudaStatus);

    cudaStatus = cudaMalloc((void**)&dev_img_rgb, w * h * sizeof(unsigned short) * 3);
    HANDLE_ERROR(cudaStatus);

    cudaStatus = cudaMemcpy(dev_img_bayer, img_bayer, w * h * sizeof(unsigned short), cudaMemcpyHostToDevice);
    HANDLE_ERROR(cudaStatus);

	//开始记录时间
	cudaEvent_t e_start, e_stop;
	cudaEventCreate(&e_start);
	cudaEventCreate(&e_stop);
	cudaEventRecord(e_start, 0);

    // Launch a kernel on the GPU with one thread for each element.
    //BayerGray << <blocks, threads >> > (w, h, dev_img_bayer, dev_img_rgb);
    GPUMalvarDemosaicing << <blocks2, threads >> > (w, h, dev_img_bayer, dev_img_rgb);

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    HANDLE_ERROR(cudaStatus);

    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    HANDLE_ERROR(cudaStatus);

	//计算耗时
	cudaEventRecord(e_stop, 0);
	cudaEventSynchronize(e_stop);
	cudaEventElapsedTime(elapsedTime, e_start, e_stop);

	

    // Copy output vector from GPU buffer to host memory.
    cudaStatus = cudaMemcpy(img_rgb, dev_img_rgb, w * h * sizeof(unsigned short) * 3, cudaMemcpyDeviceToHost);
    HANDLE_ERROR(cudaStatus);

    cudaStatus = cudaFree(dev_img_bayer);
    HANDLE_ERROR(cudaStatus);

    cudaStatus = cudaFree(dev_img_rgb);
    HANDLE_ERROR(cudaStatus);

    return cudaStatus;
}