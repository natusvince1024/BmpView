// CHistogramDialog.cpp: 实现文件
//

#include "pch.h"
#include "BmpView.h"
#include "afxdialogex.h"
#include "CHistogramDialog.h"


// CHistogramDialog 对话框

IMPLEMENT_DYNAMIC(CHistogramDialog, CDialogEx)

CHistogramDialog::CHistogramDialog(int rgbHistogram[3][256], int brightnessHistogram[256], CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HISTOGRAM_DIALOG, pParent)
{
	memcpy(m_rgbHistogram, rgbHistogram, sizeof(m_rgbHistogram));
	memcpy(m_brightnessHistogram, brightnessHistogram, sizeof(m_brightnessHistogram));
}

CHistogramDialog::~CHistogramDialog()
{
}

void CHistogramDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHistogramDialog, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()

BOOL CHistogramDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CHistogramDialog::OnPaint()
{
	CPaintDC dc(this); // 设备上下文用于绘画

	// 获取绘图区的尺寸
	CRect rect;
	GetClientRect(&rect);

	// 计算最大频率值以便进行归一化
	int maxVal = 0;
	for (int i = 0; i < 256; i++) {
		maxVal = max(maxVal, max(m_rgbHistogram[0][i], max(m_rgbHistogram[1][i], m_rgbHistogram[2][i])));
	}

	// 如果最大值为0，避免除以0的错误
	if (maxVal == 0) maxVal = 1;

	// 定义直方图的宽度和高度
	int histWidth = 256; // 直方图宽度
	int histHeight = rect.Height() - 50; // 直方图高度，留出50个像素来绘制坐标轴和刻度

	// 计算直方图的起始位置，使其居中显示
	int startX = (rect.Width() - histWidth) / 2;
	int startY = (rect.Height() - histHeight) / 2;

	// 定义不同的颜色画笔
	CPen redPen(PS_SOLID, 1, RGB(255, 0, 0));
	CPen greenPen(PS_SOLID, 1, RGB(0, 255, 0));
	CPen bluePen(PS_SOLID, 1, RGB(0, 0, 255));
	CPen brightnessPen(PS_SOLID, 1, RGB(128, 0, 128));
	CPen axisPen(PS_SOLID, 3, RGB(0, 0, 0)); // 坐标轴和刻度的颜色

	// 绘制坐标轴
	dc.SelectObject(&axisPen);
	dc.MoveTo(startX, startY + histHeight);
	dc.LineTo(startX + histWidth, startY + histHeight);
	dc.MoveTo(startX, startY);
	dc.LineTo(startX, startY + histHeight);

	// 绘制坐标轴刻度和标签
	const int numTicks = 5;
	for (int i = 0; i <= numTicks; ++i) {
		int x = startX + i * histWidth / numTicks;
		int y = startY + histHeight;
		dc.MoveTo(x, y);
		dc.LineTo(x, y - 5); // 刻度线

		CString strTick;
		strTick.Format(_T("%d"), i * 255 / numTicks);
		dc.TextOutW(x - 10, y + 10, strTick); // 刻度值
	}


	// 绘制R通道直方图
	dc.SelectObject(&redPen);
	dc.MoveTo(startX, startY + histHeight - m_rgbHistogram[0][0] * histHeight / maxVal);
	for (int i = 1; i < 256; i++) {
		dc.LineTo(startX + i, startY + histHeight - m_rgbHistogram[0][i] * histHeight / maxVal);
	}

	// 绘制G通道直方图
	dc.SelectObject(&greenPen);
	dc.MoveTo(startX, startY + histHeight - m_rgbHistogram[1][0] * histHeight / maxVal);
	for (int i = 1; i < 256; i++) {
		dc.LineTo(startX + i, startY + histHeight - m_rgbHistogram[1][i] * histHeight / maxVal);
	}

	// 绘制B通道直方图
	dc.SelectObject(&bluePen);
	dc.MoveTo(startX, startY + histHeight - m_rgbHistogram[2][0] * histHeight / maxVal);
	for (int i = 1; i < 256; i++) {
		dc.LineTo(startX + i, startY + histHeight - m_rgbHistogram[2][i] * histHeight / maxVal);
	}

	// 绘制亮度直方图
	dc.SelectObject(&brightnessPen);
	dc.MoveTo(startX, startY + histHeight - m_brightnessHistogram[0] * histHeight / maxVal);
	for (int i = 1; i < 256; i++) {
		dc.LineTo(startX + i, startY + histHeight - m_brightnessHistogram[i] * histHeight / maxVal);
	}
}


// CHistogramDialog 消息处理程序
