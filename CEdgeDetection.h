#pragma once
#include "afxdialogex.h"
#include "ImageAlgorithm.h"
#include "CProcessedImageDialog.h"


// CEdgeDetection 对话框

class CEdgeDetection : public CDialogEx
{
	DECLARE_DYNAMIC(CEdgeDetection)

public:
	CEdgeDetection(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CEdgeDetection();

	afx_msg void OnLaplacianEdgeDetection();
	afx_msg void OnSobelEdgeDetection();
	afx_msg void OnPrewittEdgeDetection();
	afx_msg void OnScharrEdgeDetection();

	void SetImageData(int width, int height, unsigned char* imgData);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_EDGE_DETECTION_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
	int m_nThreshold;
	int m_imgWidth;
	int m_imgHeight;
	unsigned char* m_imgData;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLaplacianEdgeDetection();
};
