// CEdgeDetection.cpp: 实现文件
//

#include "pch.h"
#include "BmpView.h"
#include "afxdialogex.h"
#include "CEdgeDetection.h"

#define MAX_WIDTH  3840
#define MAX_HEIGHT 2160
#define IMG_MAX_SIZE (MAX_WIDTH*MAX_HEIGHT)

static unsigned char img_temp[IMG_MAX_SIZE * 3] = { 0 };

// CEdgeDetection 对话框

IMPLEMENT_DYNAMIC(CEdgeDetection, CDialogEx)

CEdgeDetection::CEdgeDetection(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_EDGE_DETECTION_DIALOG, pParent),m_nThreshold(0), m_imgWidth(0), m_imgHeight(0), m_imgData(nullptr)
{

}

CEdgeDetection::~CEdgeDetection()
{
}

void CEdgeDetection::OnLaplacianEdgeDetection()
{
	if(m_imgData == nullptr)
	{
		AfxMessageBox(_T("请先打开一张图片"));
		return;
	}

	LaplacianEdgeDetection(m_imgWidth, m_imgHeight, m_imgData, img_temp);

	CProcessedImageDialog dlg;

	dlg.SetImageData(m_imgWidth, m_imgHeight, img_temp,"Laplacian");

	dlg.DoModal();
}

void CEdgeDetection::OnSobelEdgeDetection()
{
	if (m_imgData == nullptr)
	{
		AfxMessageBox(_T("请先打开一张图片"));
		return;
	}

	SobelEdgeDetection(m_imgWidth, m_imgHeight, m_imgData, img_temp);

	CProcessedImageDialog dlg;

	dlg.SetImageData(m_imgWidth, m_imgHeight, img_temp, "SobelEdgeDetection");

	dlg.DoModal();
}

void CEdgeDetection::OnPrewittEdgeDetection()
{
	if (m_imgData == nullptr)
	{
		AfxMessageBox(_T("请先打开一张图片"));
		return;
	}

	PrewittEdgeDetection(m_imgWidth, m_imgHeight, m_imgData, img_temp);

	CProcessedImageDialog dlg;

	dlg.SetImageData(m_imgWidth, m_imgHeight, img_temp, "PrewittEdgeDetection");

	dlg.DoModal();
}

void CEdgeDetection::OnScharrEdgeDetection()
{
	if (m_imgData == nullptr)
	{
		AfxMessageBox(_T("请先打开一张图片"));
		return;
	}

	ScharrEdgeDetection(m_imgWidth, m_imgHeight, m_imgData, img_temp);

	CProcessedImageDialog dlg;

	dlg.SetImageData(m_imgWidth, m_imgHeight, img_temp, "ScharrEdgeDetection");

	dlg.DoModal();

	memset(img_temp, 0, IMG_MAX_SIZE * 3);
}

void CEdgeDetection::SetImageData(int width, int height, unsigned char* data)
{
	m_imgWidth = width;
	m_imgHeight = height;
	m_imgData = data;
}

void CEdgeDetection::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEdgeDetection, CDialogEx)
	ON_BN_CLICKED(IDC_LAPLACIAN_EDGE_DETECTION_BUTTON, &CEdgeDetection::OnLaplacianEdgeDetection)
	ON_BN_CLICKED(IDC_SOBEL_EDGE_DETECTION_BUTTON, &CEdgeDetection::OnSobelEdgeDetection)
	ON_BN_CLICKED(IDC_PREWITT_EDGE_DETECTION_BUTTON, &CEdgeDetection::OnPrewittEdgeDetection)
	ON_BN_CLICKED(IDC_SCHARR_EDGE_DETECTION_BUTTON, &CEdgeDetection::OnScharrEdgeDetection)
END_MESSAGE_MAP()


// CEdgeDetection 消息处理程序


void CEdgeDetection::OnBnClickedLaplacianEdgeDetection()
{
	// TODO: 在此添加控件通知处理程序代码
}
