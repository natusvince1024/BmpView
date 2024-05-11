#pragma once
#include "afxdialogex.h"


// CResizeDialog 对话框

class CResizeDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CResizeDialog)

public:
	CResizeDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CResizeDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void OnOK();
	void OnCancel();
	void OnBilinear();

	DECLARE_MESSAGE_MAP()
public:
	float m_scaleWidth;
	float m_scaleHeight;
public:
	float GetScaleWidth() const { return m_scaleWidth; }
	void SetScaleWidth(float scaleWidth) { m_scaleWidth = scaleWidth; }

	float GetScaleHeight() const { return m_scaleHeight; }
	void SetScaleHeight(float scaleHeight) { m_scaleHeight = scaleHeight; }

	int GetLastClickedButton() const { return m_lastClickedButton; }
private:
	int m_lastClickedButton;
};
