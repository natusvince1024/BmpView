#pragma once
#include "afxdialogex.h"


// CProcessedImageDialog 对话框

class CProcessedImageDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CProcessedImageDialog)

public:
	CProcessedImageDialog(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~CProcessedImageDialog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = ID_DIALOG_PROCESSED_IMAGE };
#endif

public:
	void SetImageData(int width, int height, unsigned char* data, const char* title);
	BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void CopyImageToClipboard(); // 复制剪切板
	afx_msg void OnPaint();
	afx_msg void SaveCurrentImage();
	void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnShowHistogram();
private:
	int m_width;
	int m_height;
	unsigned char* m_data;
	CString m_title;  // 用于存储文件名

	DECLARE_MESSAGE_MAP()
};

void ShowBinaryImage(const char* title, int w, int h, unsigned char* outImg);

void ShowRGBImage(const char* title, int w, int h, unsigned char* outImg);
