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

// CImageDialog ʵ��
CImageDialog::CImageDialog(unsigned char* rgb, unsigned char* gary, unsigned char* yuv, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_IMAGE_DIALOG, pParent), m_width(0), m_height(0), m_data(nullptr), m_imageType(0), m_rgb(rgb), m_gary(gary), m_yuv(yuv)
{
}

CImageDialog::~CImageDialog()
{
}

void CImageDialog::OnPaint()
{
	CPaintDC dc(this); // ���ڻ��Ƶ��豸������

	if (m_width > 0 && m_height > 0 && m_data)
	{
		CClientDC dc(this);
		CRect rect;

		GetClientRect(&rect);
		// ����BITMAPINFO�ṹ��
		BITMAPINFO* pbmi = { 0 };
		int nInfoSize = sizeof(BITMAPINFOHEADER);
		pbmi = (BITMAPINFO*)malloc(nInfoSize);
		if (!pbmi) {
			return;
		}

		memset(pbmi, 0, nInfoSize);
		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pbmi->bmiHeader.biWidth = m_width;
		pbmi->bmiHeader.biHeight = -m_height; // �����׵�DIB
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
				m_data,  // ֱ��ʹ�� m_data ���л�ͼ
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

		// ��ȡ�Ի���ǰ�Ĵ��ھ��κͿͻ�������
		CRect windowRect, clientRect;
		GetWindowRect(&windowRect);
		GetClientRect(&clientRect);

		// ����߿�ͱ������Ĵ�С
		int borderHeight = windowRect.Height() - clientRect.Height();
		int borderWidth = windowRect.Width() - clientRect.Width();

		// ��ȡ��Ļ�ߴ�
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// ��֤ͼ���С��������Ļ�ߴ�
		int newWidth = min(screenWidth, m_width + borderWidth);
		int newHeight = min(screenHeight, m_height + borderHeight);

		// �����Ի����С
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