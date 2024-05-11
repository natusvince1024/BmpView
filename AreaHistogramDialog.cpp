// AreaHistogramDialog.cpp: 实现文件
//

#include "pch.h"
#include "BmpView.h"
#include "afxdialogex.h"
#include "AreaHistogramDialog.h"
#include <algorithm>


// AreaHistogramDialog 对话框

IMPLEMENT_DYNAMIC(AreaHistogramDialog, CDialogEx)

AreaHistogramDialog::AreaHistogramDialog(const std::vector<int>& areaData, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AREA_HISTOGRAM_DIALOG, pParent), m_areaData(areaData)
{
}

AreaHistogramDialog::~AreaHistogramDialog()
{
}

void AreaHistogramDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(AreaHistogramDialog, CDialogEx)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void AreaHistogramDialog::OnPaint()
{
    CPaintDC dc(this); // 设备上下文用于绘画

    CRect rect;
    GetClientRect(&rect);

    // 直方图参数
    const int binCount = 50; // 例如，使用50个bins
    std::vector<int> hist(binCount, 0);
    int maxArea = *max_element(m_areaData.begin(), m_areaData.end());
    int minArea = *min_element(m_areaData.begin(), m_areaData.end());
    int range = maxArea - minArea;
    int binSize = range / binCount;

    for (int area : m_areaData)
    {
        int binIndex = (area - minArea) / binSize;
        if (binIndex >= 0 && binIndex < binCount)
        {
            hist[binIndex]++;
        }
    }

    // 计算最大频率值以便进行归一化
    int maxVal = *max_element(hist.begin(), hist.end());
    if (maxVal == 0) maxVal = 1;

    // 定义直方图的宽度和高度
    int histWidth = rect.Width(); // 直方图宽度
    int histHeight = rect.Height() - 50; 
    // 定义笔和颜色
    CPen histPen(PS_SOLID, 1, RGB(0, 0, 255));
    CPen axisPen(PS_SOLID, 1, RGB(0, 0, 0));

    // 绘制坐标轴
    dc.SelectObject(&axisPen);
    dc.MoveTo(0, histHeight);
    dc.LineTo(histWidth, histHeight);
    dc.MoveTo(0, 0);
    dc.LineTo(0, histHeight);

    // 绘制直方图
    dc.SelectObject(&histPen);
    int binWidth = histWidth / binCount;
    for (int i = 0; i < binCount; i++)
    {
        int binHeight = hist[i] * histHeight / maxVal;
        dc.Rectangle(i * binWidth, histHeight - binHeight, (i + 1) * binWidth, histHeight);
    }
}


// AreaHistogramDialog 消息处理程序
