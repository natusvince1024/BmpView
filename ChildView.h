#pragma once
#include "pch.h"
#include "SimpleBMP.h"
#include "CImageDialog.h"
#include <vector>


// CChildView 窗口
class CChildView : public CWnd
{
	// 构造
public:
	CChildView();
	virtual ~CChildView();

	afx_msg void OnFileOpenbmp();
	afx_msg void OnFileConvertToGray();
	afx_msg void OnReturnRgb();
	afx_msg void OnFileSavebmp();


protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void CopyImageToClipboard(); // 复制剪切板
	afx_msg void OnPaint();
	afx_msg void UpdateImgTemp();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);//鼠标左键按下
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);//鼠标移动
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);//鼠标左键抬起
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void GetPixelColorAt(CPoint point, CString& colorInfo);
	void UpdateImageInfo();
	void UpdateRGBInfo(const CString& info);
	void UpdateCursorPositionInfo(CPoint point);
	void UpdateImageSizeInfo();
	//获取鼠标位置的像素颜色
	DECLARE_MESSAGE_MAP()

private:
	CPoint m_lastMousePos;  // 上次鼠标的位置
	CPoint m_imgOffset;     // 图像的偏移量
	BOOL m_isDragging;      // 是否正在拖拽图像
	std::vector<CString> m_imageFiles;
	int m_currentImageIndex;

	void StartDragging(CPoint point);  // 开始拖拽
	void DragImage(CPoint point);      // 拖拽图像
	void StopDragging();               // 停止拖拽
	void LoadAndShowImage(const CString& imagePath); // 加载并显示图像
	void ShowPreviousImage(); // 显示前一个图像
	void ShowNextImage();     // 显示下一个图像


public:
	afx_msg void OnImageMirroring();
	afx_msg void OnImageRotation();
	afx_msg void OnImageResize();
	afx_msg void OnImageGammaTransform();
	afx_msg void OnImageEdgeDetection();
	afx_msg void OnImageGussianFilter();
	afx_msg void OnImageMedianFilter();
	afx_msg void OnImageSharpening();
	afx_msg void OnImageGlobalThresholding();
	afx_msg void OnImageOtsuThresholding();
	afx_msg void OnImageTargetCounting();
	afx_msg void OnImageDrawTargetHistogram();
	afx_msg void OnImageTargetCounting2();
	afx_msg void OnImageBoundaryExtraction();
	afx_msg void OnImageCorrosion();
	afx_msg void OnImageExpansion();
	afx_msg void OnImageGrayCorrosion();
	afx_msg void OnImageGrayExpansion();
	afx_msg void OnImageOpenOperation();
	afx_msg void OnImageCloseOperation();
	afx_msg void OnImageSkeletonExtraction();
	void OnContextMenu(CWnd*, CPoint point);
	void OnShowHistogram();
	void OnShowGammaCurve();
	//void OnShowAreaHistogram();
	afx_msg void OnImageSingleScaleRetinex();
	afx_msg void OnImageLinearStretching();
	afx_msg void OnImagePiecewiseLinearStretching();
	afx_msg void OnImageHistogramEqualization();
	afx_msg void OnImageWeightedHistogramEqualization();
	afx_msg void OnImageLogarithmicTransform();
	afx_msg void OnImageBilateralFilter();
	afx_msg void OnImageMutiFrameNoiseReduction();
private:
	CImage m_image;
	std::vector<int> areaData;
};




