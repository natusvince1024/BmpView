#include "pch.h"
#include "CImageDialog.h"
#include "resource.h"
#include "ImageAlgorithm.h"

#define MAX_WIDTH  3840
#define MAX_HEIGHT 2160
#define IMG_MAX_SIZE (MAX_WIDTH*MAX_HEIGHT)
static unsigned char img_rgb[IMG_MAX_SIZE * 3] = { 0 };
static unsigned char img_gary[IMG_MAX_SIZE * 3] = { 0 };
static unsigned char img_yuv[IMG_MAX_SIZE * 3] = { 0 };

// CImageDialog 实现
CImageDialog::CImageDialog(unsigned char* rgb, unsigned char* gary, unsigned char* yuv, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_IMAGE_DIALOG, pParent), m_width(0), m_height(0), m_data(nullptr), m_imageType(0), m_rgb(rgb), m_gary(gary), m_yuv(yuv)
{
}

CImageDialog::~CImageDialog()
{
}

void CImageDialog::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	if (m_width > 0 && m_height > 0 && m_data)
	{
		CClientDC dc(this);
		CRect rect;

		GetClientRect(&rect);
		// 创建BITMAPINFO结构体
		BITMAPINFO* pbmi = { 0 };
		int nInfoSize = sizeof(BITMAPINFOHEADER);
		pbmi = (BITMAPINFO*)malloc(nInfoSize);
		if (!pbmi) {
			return;
		}

		memset(pbmi, 0, nInfoSize);
		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pbmi->bmiHeader.biWidth = m_width;
		pbmi->bmiHeader.biHeight = -m_height; // 顶至底的DIB
		pbmi->bmiHeader.biPlanes = 1;
		pbmi->bmiHeader.biBitCount = 24;
		pbmi->bmiHeader.biCompression = BI_RGB;

		if (m_data) {
			SetDIBitsToDevice(
				dc.GetSafeHdc(),
				0, 0,
				m_width, m_height,
				0, 0,
				0, m_height,
				m_data,  // 直接使用 m_data 进行绘图
				pbmi,
				DIB_RGB_COLORS
			);
		}

		free(pbmi);
	}
}


void CImageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImageDialog, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CImageDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetWindowText(m_strTitle);

		// 获取对话框当前的窗口矩形和客户区矩形
		CRect windowRect, clientRect;
		GetWindowRect(&windowRect);
		GetClientRect(&clientRect);

		// 计算边框和标题栏的大小
		int borderHeight = windowRect.Height() - clientRect.Height();
		int borderWidth = windowRect.Width() - clientRect.Width();

		// 获取屏幕尺寸
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// 保证图像大小不超过屏幕尺寸
		int newWidth = min(screenWidth, m_width + borderWidth);
		int newHeight = min(screenHeight, m_height + borderHeight);

		// 调整对话框大小
		SetWindowPos(NULL, 0, 0, newWidth, newHeight, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	
		return TRUE;
}

void CImageDialog::SetImageData(int width, int height, unsigned char* data, int imageType, const CString& title)
{
	m_width = width;
	m_height = height;
	m_data = data;
	m_imageType = imageType;
	m_strTitle = title;

	int newWidth = 1920;
	int newHeight = 1080;
	
	switch (imageType) {
	case 1:
		m_data = m_rgb;
		break;
	case 2:
		m_data = m_gary;
		break;
	case 3:
		m_data = m_yuv;
		break;
	default:
		m_data = nullptr;
		break;
	}
}