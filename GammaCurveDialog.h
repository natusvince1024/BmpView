#pragma once
#include "afxdialogex.h"


// GammaCurveDialog 对话框

class GammaCurveDialog : public CDialogEx
{
	DECLARE_DYNAMIC(GammaCurveDialog)

public:
	GammaCurveDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~GammaCurveDialog();
	void SetGamma(float gamma) { m_gamma = gamma; }

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GAMMA_CURVE_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	void OnPaint();

private:
	float m_gamma;

	DECLARE_MESSAGE_MAP()
};
