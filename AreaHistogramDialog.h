#pragma once
#include "afxdialogex.h"
#include <vector>


// AreaHistogramDialog 对话框

class AreaHistogramDialog : public CDialogEx
{
	DECLARE_DYNAMIC(AreaHistogramDialog)

public:
	AreaHistogramDialog(const std::vector<int>& areaData, CWnd* pParent = nullptr);
	virtual ~AreaHistogramDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AREA_HISTOGRAM_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void OnPaint();

	std::vector<int> m_areaData;

	DECLARE_MESSAGE_MAP()
};
