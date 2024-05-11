#pragma once

#include "pch.h"
#include <afxwin.h>

class CImageDialog : public CDialog
{
public:
	CImageDialog(unsigned char* rgb, unsigned char* gary, unsigned char* yuv, CWnd* pParent = nullptr);
	virtual ~CImageDialog();

	void OnPaint();
	void SetImageData(int width, int height, unsigned char* data, int imageType, const CString& title);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
	int m_width;
	int m_height;
	unsigned char* m_data;
	int m_imageType;  // 1: BMP, 2: Gray, 3: YUV
	CString m_strTitle;  // 用于存储文件名
private:
	unsigned char* m_rgb;
	unsigned char* m_gary;
	unsigned char* m_yuv;
};
