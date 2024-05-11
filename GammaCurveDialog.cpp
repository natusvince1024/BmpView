// GammaCurveDialog.cpp: 实现文件
//

#include "pch.h"
#include "BmpView.h"
#include "afxdialogex.h"
#include "GammaCurveDialog.h"


// GammaCurveDialog 对话框

IMPLEMENT_DYNAMIC(GammaCurveDialog, CDialogEx)

GammaCurveDialog::GammaCurveDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GAMMA_CURVE_DIALOG, pParent)
{
	m_gamma = 1.0;
}

GammaCurveDialog::~GammaCurveDialog()
{
}

void GammaCurveDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

void GammaCurveDialog::OnPaint()
{
	CPaintDC dc(this); // 设备上下文用于绘画

	// 获取绘图区的尺寸
	CRect rect;
	GetClientRect(&rect);

	// 设置伽马值
	double gamma = m_gamma; 

	// 定义绘图区域
	int width = rect.Width();
	int height = rect.Height();
	CPen gammaPen(PS_SOLID, 2, RGB(0, 0, 255)); // 蓝色画笔绘制伽马曲线

	// 绘制伽马曲线
	dc.SelectObject(&gammaPen);
	for (int i = 0; i < width; ++i)
	{
		double normalizedInput = i / (double)width;
		double normalizedOutput = pow(normalizedInput, gamma);
		int y = height - (int)(normalizedOutput * height);

		if (i == 0)
			dc.MoveTo(i, y);
		else
			dc.LineTo(i, y);
	}
}


BEGIN_MESSAGE_MAP(GammaCurveDialog, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()


// GammaCurveDialog 消息处理程序
