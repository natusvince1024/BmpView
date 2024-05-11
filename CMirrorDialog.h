#pragma once
#include "afxdialogex.h"
#include "ImageAlgorithm.h"
#include "CProcessedImageDialog.h"

// CMirrorDialog 对话框

class CMirrorDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CMirrorDialog)

public:
	CMirrorDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CMirrorDialog();

	afx_msg void OnHorizontalMirror();
	afx_msg void OnVerticalMirror();
	afx_msg void OnDiagonalMirror();

	void SetImageData(int width, int height, unsigned char* imgData);

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMAGE_MIRRIOR_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

private:
	int m_imgWidth;
	int m_imgHeight;
	unsigned char* m_imgData;

	DECLARE_MESSAGE_MAP()
};
