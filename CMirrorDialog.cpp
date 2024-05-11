// CMirrorDialog.cpp: 实现文件
//

#include "pch.h"
#include "BmpView.h"
#include "afxdialogex.h"
#include "CMirrorDialog.h"

#define MAX_WIDTH  3840
#define MAX_HEIGHT 2160
#define IMG_MAX_SIZE (MAX_WIDTH*MAX_HEIGHT)

static unsigned char img_temp[IMG_MAX_SIZE * 3] = { 0 };

// CMirrorDialog 对话框

IMPLEMENT_DYNAMIC(CMirrorDialog, CDialogEx)

CMirrorDialog::CMirrorDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_IMAGE_MIRRIOR_DIALOG, pParent), m_imgWidth(0), m_imgHeight(0), m_imgData(nullptr)
{

}

CMirrorDialog::~CMirrorDialog()
{
}

void CMirrorDialog::OnHorizontalMirror()
{
	if (m_imgData == nullptr) {
		AfxMessageBox(_T("图像数据为空"));
		return;
	}

	MirrorImageHorizontally(m_imgWidth, m_imgHeight, m_imgData, img_temp);

	// 创建一个新的 CProcessedImageDialog 对象
	CProcessedImageDialog dlgProcessed;

	// 设置处理后的图像数据
	dlgProcessed.SetImageData(m_imgWidth, m_imgHeight, img_temp, "水平镜像");

	// 显示对话框
	dlgProcessed.DoModal();
}

void CMirrorDialog::OnVerticalMirror()
{
	if (m_imgData == nullptr) {
		AfxMessageBox(_T("图像数据为空"));
		return;
	}

	MirrorImageVertically(m_imgWidth, m_imgHeight, m_imgData, img_temp);

	// 创建一个新的 CProcessedImageDialog 对象
	CProcessedImageDialog dlgProcessed;

	// 设置处理后的图像数据
	dlgProcessed.SetImageData(m_imgWidth, m_imgHeight, img_temp, "垂直镜像");

	// 显示对话框
	dlgProcessed.DoModal();
}

void CMirrorDialog::OnDiagonalMirror()
{
	if (m_imgData == nullptr) {
		AfxMessageBox(_T("图像数据为空"));
		return;
	}

	MirrorImageDiagonally(m_imgWidth, m_imgHeight, m_imgData, img_temp);

	// 创建一个新的 CProcessedImageDialog 对象
	CProcessedImageDialog dlgProcessed;

	// 设置处理后的图像数据
	dlgProcessed.SetImageData(m_imgWidth, m_imgHeight, img_temp, "对角镜像");

	// 显示对话框
	dlgProcessed.DoModal();
}

void CMirrorDialog::SetImageData(int width, int height, unsigned char* imgData)
{
	m_imgWidth = width;
	m_imgHeight = height;
	m_imgData = imgData;
}

void CMirrorDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CMirrorDialog, CDialogEx)
	ON_BN_CLICKED(ID_HORIZONTAL_MIRROR, &CMirrorDialog::OnHorizontalMirror)
	ON_BN_CLICKED(ID_VERTICAL_MIRROR, &CMirrorDialog::OnVerticalMirror)
	ON_BN_CLICKED(ID_DIAGONAL_MIRROR, &CMirrorDialog::OnDiagonalMirror)
END_MESSAGE_MAP()


// CMirrorDialog 消息处理程序
