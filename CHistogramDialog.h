#pragma once
#include "afxdialogex.h"


// CHistogramDialog 对话框

class CHistogramDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CHistogramDialog)

public:
	CHistogramDialog(int rgbHistogram[3][256], int brightnessHistogram[256], CWnd* pParent = nullptr);
	virtual ~CHistogramDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HISTOGRAM_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()

private:
	int m_rgbHistogram[3][256];
	int m_brightnessHistogram[256];
};
