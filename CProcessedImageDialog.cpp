// CProcessedImageDialog.cpp: 实现文件
//

#include "pch.h"
#include "BmpView.h"
#include "afxdialogex.h"
#include "CProcessedImageDialog.h"
#include "SimpleBMP.h"
#include "ImageAlgorithm.h"
#include "CHistogramDialog.h"


#define MAX_WIDTH  3840
#define MAX_HEIGHT 2160
#define IMG_MAX_SIZE (MAX_WIDTH*MAX_HEIGHT*3)

static unsigned char img_temp[IMG_MAX_SIZE * 3] = { 0 };

// CProcessedImageDialog 对话框

IMPLEMENT_DYNAMIC(CProcessedImageDialog, CDialogEx)

CProcessedImageDialog::CProcessedImageDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(ID_DIALOG_PROCESSED_IMAGE, pParent), m_width(0), m_height(0), m_data(nullptr), m_title(_T(""))
{

}

CProcessedImageDialog::~CProcessedImageDialog()
{
}

void CProcessedImageDialog::SetImageData(int width, int height, unsigned char* data, const char* title)
{
	m_width = width;
	m_height = height;
	m_data = data;
	m_title = title;

	// 计算每行需要的填充字节
	int padding = (4 - (m_width * 3) % 4) % 4;

	// 计算填充后的每行字节数
	int stride = m_width * 3 + padding;

	// 检查 img_temp 是否足够大
	if (stride * m_height > IMG_MAX_SIZE) {
		// Handle error: img_temp is not large enough
		AfxMessageBox(_T("img_temp is not large enough!"));
		return;
	}

	for (int i = 0; i < m_height; i++) {
		// 复制像素数据
		memmove(img_temp + i * stride, m_data + i * m_width * 3, m_width * 3);

		// 添加填充字节
		memset(img_temp + i * stride + m_width * 3, 0, padding);
	}
}

BOOL CProcessedImageDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// 设置对话框标题
	if (!m_title.IsEmpty())
		SetWindowText(m_title);

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

void CProcessedImageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BOOL CProcessedImageDialog::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == 'S' && GetKeyState(VK_CONTROL) & 0x8000)
		{
			SaveCurrentImage();
			return TRUE;
		}
		else if (pMsg->wParam == 'C' && GetKeyState(VK_CONTROL) & 0x8000)
		{
			CopyImageToClipboard();
			return TRUE;
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}

void CProcessedImageDialog::CopyImageToClipboard()
{
	if (OpenClipboard()) {
		EmptyClipboard();

		// 获取当前窗口的CDC
		CDC* pDC = GetDC();

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		HBITMAP hBitmap = CreateCompatibleBitmap(pDC->GetSafeHdc(), m_width, m_height);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC.GetSafeHdc(), hBitmap);

		BITMAPINFO bi;
		memset(&bi, 0, sizeof(BITMAPINFO));
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth = m_width;
		bi.bmiHeader.biHeight = -m_height;
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biBitCount = 24;
		bi.bmiHeader.biCompression = BI_RGB;

		SetDIBits(memDC.GetSafeHdc(), hBitmap, 0, m_height, m_data, &bi, DIB_RGB_COLORS);

		// 设置剪切板数据
		SetClipboardData(CF_BITMAP, hBitmap);

		// 清理
		SelectObject(memDC.GetSafeHdc(), hOldBitmap);
		DeleteObject(hBitmap);
		memDC.DeleteDC();
		ReleaseDC(pDC);
		CloseClipboard();
	}
}


BEGIN_MESSAGE_MAP(CProcessedImageDialog, CDialogEx)
	ON_WM_PAINT()
	ON_COMMAND(ID_SHOW_HISTOGRAM, &CProcessedImageDialog::OnShowHistogram)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CProcessedImageDialog::OnPaint()
{
	// device context for painting
	CPaintDC dc(this); 

	if (m_width > 0 && m_height > 0 && m_data) // 确保有图像数据
	{
		// 创建BITMAPINFO结构体
		BITMAPINFO* bmi = (BITMAPINFO*)malloc(sizeof(BITMAPINFOHEADER));
		if (!bmi) return;

		ZeroMemory(bmi, sizeof(BITMAPINFOHEADER));
		bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi->bmiHeader.biWidth = m_width;
		bmi->bmiHeader.biHeight = -m_height; // 使图像从顶部开始绘制
		bmi->bmiHeader.biPlanes = 1;
		bmi->bmiHeader.biBitCount = 24;
		bmi->bmiHeader.biCompression = BI_RGB;

		SetDIBitsToDevice(dc.m_hDC, 0, 0, m_width, m_height, 0, 0, 0, m_height, img_temp, bmi, DIB_RGB_COLORS);

		free(bmi);
	}
}

void CProcessedImageDialog::SaveCurrentImage()
{
	unsigned char* img_rgb = m_data;

	CFileDialog dlg(FALSE, _T("bmp"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("Bitmap Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"), this);
	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		SaveBmp(CT2A(filePath), m_width, m_height, img_rgb);
	}
}

void CProcessedImageDialog::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);
	CMenu* pContextMenu = menu.GetSubMenu(0);
	if (pContextMenu) {
		pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

void CProcessedImageDialog::OnShowHistogram()
{
	int histogram[4][256];
	CalculateHistogram((RGB*)m_data, m_width, m_height, histogram);

	CHistogramDialog dlg(histogram, histogram[3], this);
	dlg.DoModal();
}

void ShowBinaryImage(const char* title, int w, int h, unsigned char* outImg)
{
	static unsigned char img_temp[IMG_MAX_SIZE * 3] = { 0 };
	unsigned char* pTemp = img_temp;

	for (int i = 0; i < w * h; i++)
	{
		pTemp[0] = outImg[0];
		pTemp[1] = outImg[0];
		pTemp[2] = outImg[0];
		pTemp += 3;
		outImg++;
	}

	CProcessedImageDialog dlg;
	dlg.SetImageData(w, h, img_temp, title);
	dlg.DoModal();
}

void ShowRGBImage(const char* title, int w, int h, unsigned char* outImg)
{
	CProcessedImageDialog dlg;
	dlg.SetImageData(w, h, outImg, title);
	dlg.DoModal();
}
