// ChildView.cpp: CChildView 类的实现
//

#include "pch.h"
#include "framework.h"
#include "BMPView.h"
#include "ChildView.h"
#include "ImageAlgorithm.h"
#include "CResizeDialog.h"
#include "CProcessedImageDialog.h"
#include "CMirrorDialog.h"
#include "CEdgeDetection.h"
#include "MainFrm.h"
#include "opencv2/opencv.hpp"
#include "CHistogramDialog.h"
#include "GammaCurveDialog.h"
#include "MalvarRaw.h"
#include "DenoisingAlgorithm.h"
#include "ISP.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_WIDTH  3840
#define MAX_HEIGHT 2160
#define IMG_MAX_SIZE (MAX_WIDTH*MAX_HEIGHT)


static unsigned char img_buf[IMG_MAX_SIZE * 3] = { 0 };
static unsigned char img_buf2[IMG_MAX_SIZE * 3] = { 0 };
static unsigned char img_buf3[IMG_MAX_SIZE * 3] = { 0 };
static unsigned char img_buf4[IMG_MAX_SIZE * 3] = { 0 };
static unsigned char img_buf5[IMG_MAX_SIZE * 3] = { 0 };
static unsigned char img_buf6[IMG_MAX_SIZE * 3] = { 0 };
static unsigned char img_temp[IMG_MAX_SIZE * 3] = { 0 };
static unsigned char img_rgb[IMG_MAX_SIZE * 3] = { 0 };
static unsigned char img_gary[IMG_MAX_SIZE] = { 0 };
static unsigned char img_yuv[IMG_MAX_SIZE * 3] = { 0 };
static unsigned short img_raw[IMG_MAX_SIZE] = { 0 };
static unsigned short img_raw1[IMG_MAX_SIZE] = { 0 };
static unsigned short img_raw2[IMG_MAX_SIZE] = { 0 };
static unsigned short img_raw3[IMG_MAX_SIZE] = { 0 };
static unsigned short img_raw4[IMG_MAX_SIZE] = { 0 };
static unsigned short img_raw5[IMG_MAX_SIZE] = { 0 };
static unsigned short img_raw6[IMG_MAX_SIZE] = { 0 };
static unsigned short img_rgb_12bits[IMG_MAX_SIZE * 3] = { 0 };
static unsigned short img_sharpen_12bits[IMG_MAX_SIZE * 3] = { 0 };
static unsigned short img_laplace_12bits[IMG_MAX_SIZE * 3] = { 0 };
static unsigned short img_sobel_12bits[IMG_MAX_SIZE * 3] = { 0 };
static unsigned short img_edge_12bits[IMG_MAX_SIZE * 3] = { 0 };
static unsigned short img_rgb1_12bits[IMG_MAX_SIZE * 3] = { 0 };
static short img_laplace_edge[IMG_MAX_SIZE] = { 0 };
static short img_sobel_edge[IMG_MAX_SIZE] = { 0 };
static short img_edge[IMG_MAX_SIZE] = { 0 };
static short img_uv1[IMG_MAX_SIZE >> 1] = { 0 };
static short img_uv2[IMG_MAX_SIZE >> 1] = { 0 };
static short img_uv3[IMG_MAX_SIZE >> 1] = { 0 };
static int l_fileopenflag = 0;
static int l_img_width = 0;
static int l_img_height = 0;
static int l_img_bitdepth = 0;
static CString strPath;


// CChildView

CChildView::CChildView() 
	: m_isDragging(FALSE), m_imgOffset(0, 0), m_currentImageIndex(-1)
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_OPENBMP, &CChildView::OnFileOpenbmp)
	ON_COMMAND(ID_FILE_CONVERTTOGRAY, &CChildView::OnFileConvertToGray)
	ON_COMMAND(ID_RETURN_RGB, &CChildView::OnReturnRgb)
	ON_COMMAND(ID_FILE_SAVE, &CChildView::OnFileSavebmp)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_COMMAND(ID_SHOW_HISTOGRAM, &CChildView::OnShowHistogram)
	//ON_COMMAND(IDD_AREA_HISTOGRAM_DIALOG, &CChildView::OnShowAreaHistogram)
	ON_COMMAND(ID_SHOW_GAMMA_CURVE, &CChildView::OnShowGammaCurve)
	ON_COMMAND(ID_IMAGE_MIRRORING, &CChildView::OnImageMirroring)
	ON_COMMAND(ID_IMAGE_ROTATION, &CChildView::OnImageRotation)
	ON_COMMAND(ID_IMAGE_RESIZE, &CChildView::OnImageResize)
	ON_COMMAND(ID_GAMMA_TRANSFORM, &CChildView::OnImageGammaTransform)
	ON_COMMAND(ID_EDGE_DETECTION, &CChildView::OnImageEdgeDetection)
	ON_COMMAND(ID_GAUSSIAN_REDUCTION, &CChildView::OnImageGussianFilter)
	ON_COMMAND(ID_MEDIAN_REDUCTION, &CChildView::OnImageMedianFilter)
	ON_COMMAND(ID_IMAGE_SHARPENING, &CChildView::OnImageSharpening)
	ON_COMMAND(ID_GLOBAL_THRESHOLD, &CChildView::OnImageGlobalThresholding)
	ON_COMMAND(ID_OTSU_THRESHOLD, &CChildView::OnImageOtsuThresholding)
	ON_COMMAND(ID_TARGET_COUNT, &CChildView::OnImageTargetCounting)
	ON_COMMAND(ID_AREA_DISTRIBUTION, &CChildView::OnImageDrawTargetHistogram)
	ON_COMMAND(ID_TARGET_LABELING, &CChildView::OnImageTargetCounting2)
	ON_COMMAND(ID_BOUNDARY_EXTRACTION, &CChildView::OnImageBoundaryExtraction)
	ON_COMMAND(ID_CORROSION, &CChildView::OnImageCorrosion)
	ON_COMMAND(ID_EXPANSION, &CChildView::OnImageExpansion)
	ON_COMMAND(ID_GRAY_CORROSION, &CChildView::OnImageGrayCorrosion)
	ON_COMMAND(ID_GRAY_EXPANSION, &CChildView::OnImageGrayExpansion)
	ON_COMMAND(ID_OPEN_OPERATION, &CChildView::OnImageOpenOperation)
	ON_COMMAND(ID_CLOSED_OPERATION, &CChildView::OnImageCloseOperation)
	ON_COMMAND(ID_SKELETON_EXTRACTION, &CChildView::OnImageSkeletonExtraction)
	ON_COMMAND(ID_SINGLE_SCALE_RETINEX, &CChildView::OnImageSingleScaleRetinex)
	ON_COMMAND(ID_LINEAR_STRETCHING, &CChildView::OnImageLinearStretching)
	ON_COMMAND(ID_PIECEWISE_LINEAR_STRETCHING, &CChildView::OnImagePiecewiseLinearStretching)
	ON_COMMAND(ID_HISTOGRAM_EQUALIZATION, &CChildView::OnImageHistogramEqualization)
	ON_COMMAND(ID_WEIGHTED_HE, &CChildView::OnImageWeightedHistogramEqualization)
	ON_COMMAND(ID_LOGARITHMIC_TRANSFORM, &CChildView::OnImageLogarithmicTransform)
	ON_COMMAND(ID_BILATERAL_FILTER, &CChildView::OnImageBilateralFilter)
	ON_COMMAND(ID_MUTI_FRAME_NOISE_REDUCTION, &CChildView::OnImageMutiFrameNoiseReduction)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()




// CChildView 消息处理程序

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), nullptr);

	return TRUE;
}

BOOL CChildView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == 'C' && GetKeyState(VK_CONTROL) & 0x8000)
		{
			CopyImageToClipboard();
			return TRUE;
		}
	}
	return FALSE;
}

void CChildView::CopyImageToClipboard()
{
	if (OpenClipboard()) {
		EmptyClipboard();

		// 获取当前窗口的CDC
		CDC* pDC = GetDC();

		CDC memDC;
		memDC.CreateCompatibleDC(pDC);
		HBITMAP hBitmap = CreateCompatibleBitmap(pDC->GetSafeHdc(), l_img_width, l_img_height);
		HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC.GetSafeHdc(), hBitmap);

		BITMAPINFO bi;
		memset(&bi, 0, sizeof(BITMAPINFO));
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth = l_img_width;
		bi.bmiHeader.biHeight = -l_img_height;
		bi.bmiHeader.biPlanes = 1;
		bi.bmiHeader.biBitCount = 24;
		bi.bmiHeader.biCompression = BI_RGB;

		if (1 == l_fileopenflag)
		{
			SetDIBits(memDC.GetSafeHdc(), hBitmap, 0, l_img_height, img_rgb, &bi, DIB_RGB_COLORS);
		}
		else if (2 == l_fileopenflag)
		{
			SetDIBits(memDC.GetSafeHdc(), hBitmap, 0, l_img_height, img_gary, &bi, DIB_RGB_COLORS);
		}

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

void CChildView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CDC memDC;
	CBitmap memBitmap;// 位图对象
	memDC.CreateCompatibleDC(&dc);
	CRect rect;
	GetClientRect(&rect);
	memBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	CBitmap* pOldBitmap = memDC.SelectObject(&memBitmap);// 将位图选入内存DC

	memDC.FillSolidRect(&rect, RGB(172, 130, 149));// 填充背景色

	if (l_img_width > 0 && l_img_height > 0) // 确保有有效的图像数据
	{
		// 创建BITMAPINFO结构体
		BITMAPINFO* pbmi = { 0 };
		int nInfoSize = sizeof(BITMAPINFOHEADER);
		pbmi = (BITMAPINFO*)malloc(nInfoSize);
		if (!pbmi) {
			return;
		}

		memset(pbmi, 0, nInfoSize);
		pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		pbmi->bmiHeader.biWidth = l_img_width;
		pbmi->bmiHeader.biHeight = -l_img_height; // 顶至底的DIB
		pbmi->bmiHeader.biPlanes = 1;
		pbmi->bmiHeader.biBitCount = 24;
		pbmi->bmiHeader.biCompression = BI_RGB;


		if (1 == l_fileopenflag) {
			SetDIBitsToDevice(
				memDC.GetSafeHdc(),
				0 + m_imgOffset.x, 0 + m_imgOffset.y,
				l_img_width, l_img_height,
				0, 0,
				0, l_img_height,
				img_temp,
				pbmi,
				DIB_RGB_COLORS
			);
		}
		else if (2 == l_fileopenflag) {
			SetDIBitsToDevice(
				memDC.GetSafeHdc(),
				0, 0,
				l_img_width, l_img_height,
				0, 0,
				0, l_img_height,
				img_gary,
				pbmi,
				DIB_RGB_COLORS
			);
		}
		else if (3 == l_fileopenflag) {
			SetDIBitsToDevice(
				memDC.GetSafeHdc(),
				0 + m_imgOffset.x, 0 + m_imgOffset.y,
				l_img_width, l_img_height,
				0, 0,
				0, l_img_height,
				img_yuv,
				pbmi,
				DIB_RGB_COLORS
			);
		}
		else if (4 == l_fileopenflag) {
			SetDIBitsToDevice(
				memDC.GetSafeHdc(),
				0, 0,
				l_img_width, l_img_height,
				0, 0,
				0, l_img_height,
				img_buf,
				pbmi,
				DIB_RGB_COLORS
			);
		}
		free(pbmi);
	}

	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOldBitmap);
	memBitmap.DeleteObject();
	memDC.DeleteDC();
}

void CChildView::UpdateImgTemp()
{
	// 计算每行的填充字节
	int padding = (4 - (l_img_width * 3) % 4) % 4;
	int stride = (l_img_width * 3) + padding;

	// 使用 img_temp 缓冲区来处理字节填充
	for (int y = 0; y < l_img_height; y++)
	{
		PBYTE pSrc = img_rgb + y * l_img_width * 3; // 源指针
		PBYTE pDst = img_temp + y * stride; // 目标指针
		memcpy(pDst, pSrc, l_img_width * 3); // 复制像素数据
		memset(pDst + l_img_width * 3, 0, padding); // 添加填充字节
	}
}


void CChildView::OnFileOpenbmp()
{
	CFileDialog dlg(TRUE, _T(".bmp"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("All Files (*.*)|*.*|BMP Files (*.bmp)|*.bmp|YUV Files (*.yuv)|*.yuv|PNG Files (*.png)|*.png|JPG Files (*.jpg)|*.jpg|RAW Files (*.raw)|*.raw||")); // 构造文件对话框

	if (IDOK == dlg.DoModal())
	{
		strPath = dlg.GetPathName(); // 获取文件路径名
		CT2A path(strPath);
		CString strExt = dlg.GetFileExt();   // 获取文件扩展名

		// 获取文件所在的目录
		CString directory = strPath.Left(strPath.ReverseFind('\\'));

		// 清空之前的文件列表
		m_imageFiles.clear();

		// 使用CFileFind列出目录下的所有.bmp和.yuv文件
		CFileFind finder;
		BOOL bWorking = finder.FindFile(directory + _T("\\*.*"));
		while (bWorking)
		{
			bWorking = finder.FindNextFile();
			if (!finder.IsDots() && !finder.IsDirectory())
			{
				CString fileExt = finder.GetFileName().Right(3);
				if (fileExt.CompareNoCase(_T("bmp")) == 0 || fileExt.CompareNoCase(_T("yuv")) == 0 || fileExt.CompareNoCase(_T("png")) == 0 || fileExt.CompareNoCase(_T("jpg")) == 0 || fileExt.CompareNoCase(_T("raw")) == 0)
				{
					m_imageFiles.push_back(finder.GetFilePath());// 将文件路径添加到列表中
				}
			}
		}

		// 找到用户选择的文件在列表中的索引
		for (size_t i = 0; i < m_imageFiles.size(); i++)
		{
			if (m_imageFiles[i] == strPath)
			{
				m_currentImageIndex = static_cast<int>(i);
				break;
			}
		}
		// 加载并显示用户选择的图像
		LoadAndShowImage(strPath);

		// 更新图像大小信息
		UpdateImageSizeInfo();

		//更改窗口标题
		GetParent()->SetWindowText(strPath);
		UpdateImageInfo();
	}
	else
	{
		GetParent()->SetWindowText(_T("Image Processing"));
	}
}

void CChildView::OnFileConvertToGray()
{
	if (1 == l_fileopenflag) {
		memset(img_gary, 0, IMG_MAX_SIZE);
		Rgb2Gray(l_img_width, l_img_height, img_rgb, img_gary);
		l_fileopenflag = 2;
		// 重绘视图以显示灰度图像
		Invalidate();
	}
	else
		MessageBox(_T("Please open a BMP file first!"));
}

void CChildView::OnReturnRgb()
{
	if (2 == l_fileopenflag) {
		l_fileopenflag = 1;
		// 重绘视图以显示灰度图像
		Invalidate();
	}
	else
		MessageBox(_T("Please open a BMP file first!"));
}

void CChildView::OnFileSavebmp()
{
	CFileDialog dlg(FALSE, _T("bmp"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Bmp Files (*.bmp)|*.bmp|All Files (*.*)|*.*||"));
	if (dlg.DoModal() == IDOK)
	{
		if (1 == l_fileopenflag)
		{
			CString filePath = dlg.GetPathName();

			SaveBmp(CT2A(filePath.GetString()), l_img_width, l_img_height, img_gary);
		}
		else 
		{
			CString filePath = dlg.GetPathName();

			SaveBmp(CT2A(filePath.GetString()), l_img_width, l_img_height, img_rgb);
		}
	}
}

void CChildView::StartDragging(CPoint point) {
	if (l_img_width < 1920 && l_img_height < 1080) {
		return;
	}
	m_lastMousePos = point;
	m_isDragging = TRUE;
	SetCapture();  // 捕获鼠标，即使鼠标移出窗口，也能接收其消息
}

void CChildView::DragImage(CPoint point) {
	if (m_isDragging) {
		CRect rect;
		GetClientRect(&rect); // 获取窗口的大小

		// 根据鼠标的移动计算图像的偏移量
		if (l_img_width > rect.Width()) {
			m_imgOffset.x += point.x - m_lastMousePos.x;
			// 限制图像的水平偏移量
			if (m_imgOffset.x > 0) m_imgOffset.x = 0;
			if (m_imgOffset.x < rect.Width() - l_img_width) m_imgOffset.x = rect.Width() - l_img_width;
		}

		if (l_img_height > rect.Height()) {
			m_imgOffset.y += point.y - m_lastMousePos.y;
			// 限制图像的垂直偏移量
			if (m_imgOffset.y > 0) m_imgOffset.y = 0;
			if (m_imgOffset.y < rect.Height() - l_img_height) m_imgOffset.y = rect.Height() - l_img_height;
		}

		m_lastMousePos = point;
		Invalidate(FALSE); // 重绘窗口
	}
}

void CChildView::StopDragging() {
	if (m_isDragging) {
		m_isDragging = FALSE;
		ReleaseCapture();  // 释放鼠标捕获
	}
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point) {
	StartDragging(point);
	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point) {
	// 如果鼠标左键被按下，则拖动图像
	if (nFlags & MK_LBUTTON) {
		DragImage(point);
	}

	//获取鼠标位置的像素颜色
	CString colorInfo;
	GetPixelColorAt(point, colorInfo);

	// 如果获取到了颜色信息，则更新状态栏
	if (!colorInfo.IsEmpty()) {
		UpdateRGBInfo(colorInfo); 
	}

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrame != nullptr) {
		pMainFrame->UpdateStatusBar(colorInfo,1);
	}

	UpdateCursorPositionInfo(point);

	CWnd::OnMouseMove(nFlags, point);
}

void CChildView::OnLButtonUp(UINT nFlags, CPoint point) {
	StopDragging();
	CWnd::OnLButtonUp(nFlags, point);
}

// ChildView.cpp
void CChildView::LoadAndShowImage(const CString& imagePath)
{
	strPath = imagePath;
	CString strExt = imagePath.Right(3); // 获取文件扩展名

	// 根据文件扩展名决定调用哪个函数来读取图像
	if (strExt.CompareNoCase(_T("bmp")) == 0)
	{
		CT2A path(imagePath);
		// 调用ReadBmp函数读取BMP文件
		int result = ReadBmp(path, &l_img_width, &l_img_height, &l_img_bitdepth, img_rgb);
		if (0 == result) {
			UpdateImgTemp();
			l_fileopenflag = 1;
		}
	}
	else if (strExt.CompareNoCase(_T("yuv")) == 0)
	{
		CT2A path(imagePath);
		// 调用ReadYuv函数读取YUV文件
		int result = ReadYuv(path, &l_img_width, &l_img_height, img_yuv);
		if (0 == result) {
			l_fileopenflag = 3;
		}
	}
	else if (strExt.CompareNoCase(_T("png")) == 0 || strExt.CompareNoCase(_T("jpg")) == 0 || strExt.CompareNoCase(_T("gif")) == 0)
	{
		if (!m_image.IsNull()) {
			m_image.Destroy();
		}

		HRESULT hr = m_image.Load(imagePath);
		if (SUCCEEDED(hr)) {
			l_img_width = m_image.GetWidth();
			l_img_height = m_image.GetHeight();

			int pitch = m_image.GetPitch();// 每行的字节数
			BYTE* pBits = (BYTE*)m_image.GetBits();
			int bytesPerPixel = m_image.GetBPP() >> 3;

			for (int y = 0; y < l_img_height; y++)
			{
				for (int x = 0; x < l_img_width; x++)
				{
					BYTE* pPixel = pBits + y * pitch + x * bytesPerPixel;
					BYTE b = pPixel[0];
					BYTE g = pPixel[1];
					BYTE r = pPixel[2];

					int index = (y * l_img_width + x) * 3;
					img_rgb[index] = b;
					img_rgb[index + 1] = g;
					img_rgb[index + 2] = r;
				}
			}
			UpdateImgTemp();
			l_fileopenflag = 1;
		}
	}
	else if (strExt.CompareNoCase(_T("raw")) == 0)
	{
		CT2A path(imagePath);
		// 调用ReadRaw函数读取RAW文件
		int result = Read_12Bits_Raw(path, &l_img_width, &l_img_height, img_raw);
		if (0 == result) {
			float time = 0;
			cudaISP(l_img_width, l_img_height, img_raw, img_rgb_12bits, &time);
			Rgb_12bit_to_8bit(img_rgb_12bits, img_rgb);
			//MalvarDemosaicing(l_img_width, l_img_height, img_raw, img_rgb);
			char title[50] = { 0 };
			sprintf(title, "Demosaic: %.4f ms", time);
			ShowRGBImage(title, l_img_width, l_img_height, img_rgb);

			///*MalvarDemosaicing(l_img_width, l_img_height, img_raw, img_temp);
			//ShowRGBImage("去马赛克", l_img_width, l_img_height, img_temp);
			//AWB(l_img_width, l_img_height, img_temp, img_rgb);*/

			////WB_Manual(l_img_width, l_img_height, img_raw, 1.5598f, 2.3653f);
			////RawWB(img_raw);
			//AWB(l_img_width, l_img_height, 120, 135, img_raw);
			//HLD_Demosaic(l_img_width, l_img_height, img_raw, img_rgb_12bits);
			////High_quality_interpolation(img_raw, img_rgb_12bits);
			//Raw_Y_gamma(0.45f, img_rgb_12bits);
			//Raw12bit_Y_Square_HE(img_rgb_12bits);
			//Rgb_12bit_to_8bit(img_rgb_12bits, img_rgb);
			//ShowRGBImage("去马赛克", l_img_width, l_img_height, img_rgb);

			////Raw_Y_gamma(0.45f, img_rgb_12bits);
			//GammaTransform_RGB12bit(l_img_width, l_img_height, 2.2, img_rgb_12bits, img_rgb1_12bits);
			//Rgb_12bit_to_8bit(img_rgb1_12bits, img_buf);
			//ShowRGBImage("Gamma", l_img_width, l_img_height, img_buf);

			//RGB12bits2Y(img_rgb_12bits, img_raw1);
			//RGB12bit_to_UV(l_img_width, l_img_height, img_rgb_12bits, img_uv1);
			////UV_NR(l_img_width, l_img_height, img_uv1, img_uv2);
			////UV_NR(l_img_width, l_img_height, img_uv2, img_uv3);
			////bilateralFilter(l_img_width, l_img_height, 3, 320, 160, img_raw1, img_raw2);
			////MeanFiltering(l_img_width, l_img_height, img_raw1, img_raw2);
			////EightNeighborhoodAssignment(l_img_width, l_img_height, img_raw1, img_raw2);
			///*FourNeighborhoodAssignment(l_img_width, l_img_height, img_raw1, img_raw2);
			//LaplaceSharpening_12bits(l_img_width, l_img_height, img_raw2, img_raw3);
			//memset(img_uv1, 0, 3840 * 1280);
			//YUV12bit_to_RGB12bit(l_img_width, l_img_height, img_raw3, img_uv1, img_rgb_12bits);
			//Rgb_12bit_to_8bit(img_rgb_12bits, img_rgb);*/
			////ShowRGBImage("YUV双边滤波", l_img_width, l_img_height, img_rgb);
			////ShowRGBImage("均值滤波", l_img_width, l_img_height, img_rgb);
			////ShowRGBImage("八邻域滤波", l_img_width, l_img_height, img_rgb);
			////ShowRGBImage("四邻域滤波", l_img_width, l_img_height, img_rgb);

			///*Laplace_Y(img_raw1, img_laplace_edge);
			//Show_edge(img_laplace_edge, img_laplace_12bits);
			//Raw12bit_Y_Square_HE(img_laplace_12bits);
			//Rgb_12bit_to_8bit(img_laplace_12bits, img_buf);
			//ShowRGBImage("Laplace", l_img_width, l_img_height, img_buf);


			//Sobel_Y(img_raw1, img_sobel_edge);
			//Show_edge(img_sobel_edge, img_sobel_12bits);
			//Raw12bit_Y_Square_HE(img_sobel_12bits);
			//Rgb_12bit_to_8bit(img_sobel_12bits, img_buf2);
			//ShowRGBImage("Sobel", l_img_width, l_img_height, img_buf2);*/

			////Vague_Sobel_Y(img_sobel_edge);

			///*Laplace_Sobel_Y(img_laplace_edge, img_sobel_edge, img_edge);
			//Show_edge(img_edge, img_edge_12bits);
			//Raw12bit_Y_Square_HE(img_edge_12bits);
			//Rgb_12bit_to_8bit(img_edge_12bits, img_buf3);
			//ShowRGBImage("edge", l_img_width, l_img_height, img_buf3);*/

			///*Show_12bit_Sharpen(img_rgb_12bits, img_edge, img_sharpen_12bits);
			//Rgb_12bit_to_8bit(img_sharpen_12bits, img_buf5);
			//ShowRGBImage("Sharpen", l_img_width, l_img_height, img_buf5);*/

			///*Raw_Y_gamma(0.45f, img_rgb_12bits);
			//Raw12bit_Y_Square_HE(img_rgb_12bits);
			//Rgb_12bit_to_8bit(img_rgb_12bits, img_rgb);
			//ShowRGBImage("HE", l_img_width, l_img_height, img_rgb);*/

			////CCM(img_rgb_12bits, img_sharpen_12bits);

			////Rgb_12bit_to_8bit(img_sharpen_12bits, img_rgb);

			////ShowRGBImage("CCM", l_img_width, l_img_height, img_rgb);

			////Raw2Rgb(l_img_width, l_img_height, img_raw, img_rgb);
			UpdateImgTemp();
			l_fileopenflag = 1;
		}
	}

	// 重绘视图以显示新加载的图像
	Invalidate();
}

void CChildView::ShowPreviousImage()
{
	if (m_currentImageIndex > 0)
	{
		m_imgOffset.x = 0;
		m_imgOffset.y = 0;
		m_currentImageIndex--;
		LoadAndShowImage(m_imageFiles[m_currentImageIndex]);
		GetParent()->SetWindowText(m_imageFiles[m_currentImageIndex]);
		UpdateImageInfo();
		UpdateImageSizeInfo();
	}
}

void CChildView::ShowNextImage()
{
	if (m_currentImageIndex < m_imageFiles.size() - 1)
	{
		m_imgOffset.x = 0;
		m_imgOffset.y = 0;
		m_currentImageIndex++;
		LoadAndShowImage(m_imageFiles[m_currentImageIndex]);
		GetParent()->SetWindowText(m_imageFiles[m_currentImageIndex]);
		UpdateImageInfo();
		UpdateImageSizeInfo();
	}
}

void CChildView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_LEFT) 
	{
		ShowPreviousImage();
	}
	else if (nChar == VK_RIGHT) 
	{
		ShowNextImage();
	}
	else if (nChar == VK_SPACE) 
	{
		/*CImageDialog dlg(img_rgb, img_gary, img_yuv);
		dlg.SetImageData(l_img_width, l_img_height, img_rgb, l_fileopenflag,strPath);
		dlg.DoModal();*/
		ResizeBilinear(l_img_width, l_img_height, img_rgb, 1920, 1080, img_buf);

		// 创建CProcessedImageDialog实例
		CProcessedImageDialog dlgProcessed;
		dlgProcessed.SetImageData(1920, 1080, img_buf, "全屏显示");
		dlgProcessed.DoModal();
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CChildView::GetPixelColorAt(CPoint point, CString& colorInfo)
{
	// 考虑图像的偏移量
	point.x -= m_imgOffset.x;
	point.y -= m_imgOffset.y;

	if (point.x < 0 || point.x >= l_img_width || point.y < 0 || point.y >= l_img_height) {
		colorInfo.Empty();
		return;
	}

	int index = (point.y * l_img_width + point.x) * 3;
	BYTE b = img_rgb[index];
	BYTE g = img_rgb[index + 1];
	BYTE r = img_rgb[index + 2];

	colorInfo.Format(_T("RGB(%d, %d, %d)"), r, g, b);
}

void CChildView::UpdateImageInfo()
{ 
	if (m_currentImageIndex >= 0 && !m_imageFiles.empty()) {
		CString info;
		info.Format(_T("%d/%d"), m_currentImageIndex + 1, m_imageFiles.size());

		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if (pMainFrame != nullptr) {
			pMainFrame->UpdateStatusBar(info,4);  // 更新状态栏
		}
	}
}

void CChildView::UpdateRGBInfo(const CString& info)
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrame != nullptr) {
		pMainFrame->UpdateStatusBar(info, 1);  // 更新RGB值
	}
}

void CChildView::UpdateCursorPositionInfo(CPoint point)
{
	CString info;
	info.Format(_T("x,y(%d,%d)"), point.x, point.y);

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrame != nullptr) {
		pMainFrame->UpdateStatusBar(info, 2);  // 更新光标位置信息
	}
}

void CChildView::UpdateImageSizeInfo()
{
	if (l_img_width > 0 && l_img_height > 0) {
		CString info;
		info.Format(_T("宽×高：%d×%d"), l_img_width, l_img_height);

		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		if (pMainFrame != nullptr) {
			pMainFrame->UpdateStatusBar(info, 3);  // 更新图像大小信息
		}
	}
}

void CChildView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);
	CMenu* pContextMenu = menu.GetSubMenu(0);
	if (pContextMenu) {
		pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}
}

void CChildView::OnShowHistogram() {
	int histogram[4][256];
	CalculateHistogram((RGB*)img_rgb, l_img_width, l_img_height, histogram);

	CHistogramDialog dlg(histogram, histogram[3], this);
	dlg.DoModal();
}

void CChildView::OnShowGammaCurve()
{
	GammaCurveDialog dlg;
	dlg.SetGamma(0.75);
	dlg.DoModal();
}

void CChildView::OnImageMirroring()
{
	if (img_rgb == nullptr) {
		AfxMessageBox(_T("请先打开一张图片"));
		return;
	}

	CMirrorDialog dlg;
	dlg.SetImageData(l_img_width, l_img_height, img_rgb);  // 将图像数据传递给对话框
	dlg.DoModal();  // 显示对话框
}

void CChildView::OnImageRotation()
{
	if (img_rgb == nullptr) {
		AfxMessageBox(_T("请先打开一张图片"));
		return;
	}

	RotateImage90(l_img_width, l_img_height, img_rgb, img_temp);

	ShowRGBImage("旋转", l_img_height, l_img_width, img_temp);
}

void CChildView::OnImageResize()
{
	CResizeDialog dlg;
	int ret = dlg.DoModal();
	if (IDOK == ret || ID_BILINEAR == ret)  // 先弹出对话框获取用户输入
	{
		// 获取用户输入的缩放因子
		float scaleWidth = dlg.GetScaleWidth();
		float scaleHeight = dlg.GetScaleHeight();

		// 计算新的图像尺寸
		int newWidth = (int)(l_img_width * scaleWidth);
		int newHeight = (int)(l_img_height * scaleHeight);

		if (newWidth * newHeight * 3 > IMG_MAX_SIZE) {
			AfxMessageBox(_T("The scaled image is too large!"));
			return;
		}

		if (dlg.GetLastClickedButton() == IDOK)  // 根据用户点击的按钮选择不同的缩放算法
		{
			// 调用之前写的邻近法缩放函数
			ResizeNearestNeighbor(l_img_width, l_img_height, newWidth, newHeight, img_rgb, img_buf);

			// 创建CProcessedImageDialog实例
			CProcessedImageDialog dlgProcessed;

			// 设置图像数据
			dlgProcessed.SetImageData(newWidth, newHeight, img_buf, "近邻法");

			// 显示对话框
			dlgProcessed.DoModal();
		}
		else if (dlg.GetLastClickedButton() == ID_BILINEAR)
		{
			clock_t start, end;
			double cpu_time_used;

			cv::Mat srcImage(l_img_height, l_img_width, CV_8UC3, img_rgb);  // 假设img_rgb是你的源图像数据
			cv::Mat dstImage;

			start = clock();

			// 调用之前写的双线性缩放函数
			ResizeBilinear(l_img_width, l_img_height, img_rgb, newWidth, newHeight, img_buf);
			//调用opencv的双线性缩放函数			
			//cv::resize(srcImage, dstImage, cv::Size(newWidth, newHeight), 0, 0, cv::INTER_LINEAR);

			end = clock();
			//memcpy(img_buf, dstImage.data, newWidth * newHeight * 3);  // 将结果复制到你的图像缓冲区

			cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

			CString message;
			message.Format(_T("算法运行时间：%f秒"), cpu_time_used);
			AfxMessageBox(message);

			// 创建CProcessedImageDialog实例
			CProcessedImageDialog dlgProcessed;

			// 设置图像数据
			dlgProcessed.SetImageData(newWidth, newHeight, img_buf, "双线性");

			// 显示对话框
			dlgProcessed.DoModal();
		}
	}
}

void CChildView::OnImageGammaTransform()
{
	if (img_rgb == nullptr) 
	{
			AfxMessageBox(_T("请先打开一张图片"));
			return;
	}
	memset(img_buf, 0, IMG_MAX_SIZE * 3);

	GammaTransform(l_img_width, l_img_height, 1.25, img_rgb, img_buf);

	CProcessedImageDialog dlg;

	dlg.SetImageData(l_img_width, l_img_height, img_buf,"Gamma变换");  // 将图像数据传递给对话框

	dlg.DoModal();  // 显示对话框
}

void CChildView::OnImageEdgeDetection()
{
	if (img_rgb == nullptr)
	{
		AfxMessageBox(_T("请先打开一张图片"));
		return;
	}

	CEdgeDetection dlg;

	dlg.SetImageData(l_img_width, l_img_height, img_rgb);  // 将图像数据传递给对话框

	dlg.DoModal();  // 显示对话框
}

void CChildView::OnImageGussianFilter()
{

	GaussianFilter(l_img_width, l_img_height, img_rgb, img_temp);

	CProcessedImageDialog dlg;

	dlg.SetImageData(l_img_width, l_img_height, img_temp, "高斯滤波");  // 将图像数据传递给对话框

	dlg.DoModal();  // 显示对话框
}

void CChildView::OnImageMedianFilter()
{
	MedianFilter(l_img_width, l_img_height, img_rgb, img_temp);

	CProcessedImageDialog dlg;

	dlg.SetImageData(l_img_width, l_img_height, img_temp, "中值滤波");  // 将图像数据传递给对话框

	dlg.DoModal();  // 显示对话框
}

void CChildView::OnImageSharpening()
{

	// 调用之前写的图像锐化函数
	//MedianFilter(l_img_width, l_img_height, img_rgb, img_temp);
	memset(img_temp, 0, IMG_MAX_SIZE * 3);
	Sharpening(l_img_width, l_img_height, img_rgb, img_temp);

	// 创建CProcessedImageDialog实例
	CProcessedImageDialog dlgProcessed;

	// 设置图像数据
	dlgProcessed.SetImageData(l_img_width, l_img_height, img_temp, "锐化");

	// 显示对话框
	dlgProcessed.DoModal();
}

void CChildView::OnImageGlobalThresholding()
{
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	GlobalThresholdSegmentation(l_img_width, l_img_height, img_gary, img_temp);

	// 创建CProcessedImageDialog实例
	CProcessedImageDialog dlgProcessed;

	// 设置图像数据
	dlgProcessed.SetImageData(l_img_width, l_img_height, img_temp, "全局阈值处理");

	// 显示对话框
	dlgProcessed.DoModal();
}

void CChildView::OnImageOtsuThresholding()
{
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	OtsuThresholdSegmentation(l_img_width, l_img_height, img_gary, img_buf);

	//DoubleThresholdSegmentation(l_img_width, l_img_height, img_gary, img_temp);

	//LocalThresholdSegmentation(l_img_width, l_img_height, img_gary, img_temp,12,0.5);

	//DoubleThresholdSegmentation1(l_img_width, l_img_height, img_gary, img_temp);

	//IterateOtsuSegment(l_img_width, l_img_height, img_gary, img_temp);

	Gray2Rgb(l_img_width, l_img_height, img_buf, img_buf2);

	// 创建CProcessedImageDialog实例
	CProcessedImageDialog dlgProcessed;

	// 设置图像数据
	dlgProcessed.SetImageData(l_img_width, l_img_height, img_buf2, "Otsu阈值处理");

	// 显示对话框
	dlgProcessed.DoModal();
}

void CChildView::OnImageTargetCounting()
{
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	OtsuThresholdSegmentation(l_img_width, l_img_height, img_gary, img_buf);

	static TARGET count[1000] = { 0 };

	int targetCount = CountTargets1(l_img_width, l_img_height, img_buf, count);

	//对数组内的目标面积存储到数组中
	for (int i = 0; i < targetCount; i++)
	{
		areaData.push_back(count[i].area);
	}

	Gray2Rgb(l_img_width, l_img_height, img_buf, img_buf2);

	// 创建CProcessedImageDialog实例
	CProcessedImageDialog dlgProcessed;

	// 设置图像数据
	CString title;
	title.Format(_T("目标计数: %d"), targetCount);  // 设置标题为 "目标计数: [计数值]"
	CT2A pszConverted(title);
	dlgProcessed.SetImageData(l_img_width, l_img_height, img_buf2, pszConverted);

	// 显示对话框
	dlgProcessed.DoModal();
}

void CChildView::OnImageDrawTargetHistogram()
{
	CPaintDC dc(this); // 设备上下文用于绘画

	// 获取绘图区的尺寸
	CRect rect;
	GetClientRect(&rect);

	// 假设我们已经有了一个包含面积数据的数组
	// 例如: std::vector<int> areaData;

	// 计算直方图数据
	const int binCount = 50; // 例如，我们使用50个bins
	int hist[binCount] = { 0 };
	int maxArea = 8300; // 假设最大面积为8300
	int minArea = 350;  // 假设最小面积为350
	int range = maxArea - minArea;
	int binSize = range / binCount;

	for (int area : areaData)
	{
		int binIndex = (area - minArea) / binSize;
		if (binIndex >= 0 && binIndex < binCount)
		{
			hist[binIndex]++;
		}
	}

	// 计算最大频率值以便进行归一化
	int maxVal = *std::max_element(hist, hist + binCount);
	if (maxVal == 0) maxVal = 1; // 避免除以0

	// 定义直方图的宽度和高度
	int histWidth = rect.Width(); // 直方图宽度
	int histHeight = rect.Height() - 50; // 直方图高度，留出空间用于坐标轴和标签

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

void CChildView::OnImageTargetCounting2()
{
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	OtsuThresholdSegmentation(l_img_width, l_img_height, img_gary, img_buf);
	
	static TARGET count[1000] = { 0 };
	char area[20] = { 0 };

	int targetCount = CountTargets1(l_img_width, l_img_height, img_buf, count);

	PTARGET pTarget = count;
	CClientDC dcClient(this);
	CPen greenPen(PS_SOLID, 2, RGB(0, 255, 0));
	CPen* pOldPen = dcClient.SelectObject(&greenPen);

	for (int i = 0; i < targetCount; i++) 
	{
		// 绘制矩形
		MoveToEx(dcClient.GetSafeHdc(), pTarget[0].minX + m_imgOffset.x, pTarget[0].minY + m_imgOffset.y, NULL);
		LineTo(dcClient.GetSafeHdc(), pTarget[0].maxX + m_imgOffset.x, pTarget[0].minY + m_imgOffset.y);
		LineTo(dcClient.GetSafeHdc(), pTarget[0].maxX + m_imgOffset.x, pTarget[0].maxY + m_imgOffset.y);
		LineTo(dcClient.GetSafeHdc(), pTarget[0].minX + m_imgOffset.x, pTarget[0].maxY + m_imgOffset.y);
		LineTo(dcClient.GetSafeHdc(), pTarget[0].minX + m_imgOffset.x, pTarget[0].minY + m_imgOffset.y);

		//显示目标面积
		sprintf_s(area, "%d", pTarget[0].area);
		SetBkMode(dcClient.GetSafeHdc(), TRANSPARENT);
		RECT rect = { pTarget[0].minX, pTarget[0].minY - 55, pTarget[0].maxX + 20, pTarget[0].maxY };
		DrawTextA(dcClient.GetSafeHdc(), area, -1, &rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

		pTarget++;
	}
	dcClient.SelectObject(&pOldPen);
}

void CChildView::OnImageBoundaryExtraction()
{
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	OtsuThresholdSegmentation(l_img_width, l_img_height, img_gary, img_buf);

	memset(img_buf2, 0, IMG_MAX_SIZE * 3);
	memset(img_buf3, 0, IMG_MAX_SIZE * 3);

	BoundaryExtraction(l_img_width, l_img_height, img_buf, img_buf2);

	Gray2Rgb(l_img_width, l_img_height, img_buf2, img_buf3);

	// 创建CProcessedImageDialog实例
	CProcessedImageDialog dlgProcessed;

	// 设置图像数据
	dlgProcessed.SetImageData(l_img_width, l_img_height, img_buf3, "边界提取");

	// 显示对话框
	dlgProcessed.DoModal();
}

void CChildView::OnImageCorrosion()
{
	// 创建CProcessedImageDialog实例
	CProcessedImageDialog dlgProcessed;

	memset(img_buf, 0, IMG_MAX_SIZE * 3);
	memset(img_buf2, 0, IMG_MAX_SIZE * 3);
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	GrayCorrosion(l_img_width, l_img_height, img_gary, img_buf);

	
	Gray2Rgb(l_img_width, l_img_height, img_buf, img_buf2);

	dlgProcessed.SetImageData(l_img_width, l_img_height, img_buf2, "二值化图像腐蚀");

	dlgProcessed.DoModal();
}

void CChildView::OnImageExpansion()
{
	// 创建CProcessedImageDialog实例
	CProcessedImageDialog dlgProcessed;

	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	OtsuThresholdSegmentation(l_img_width, l_img_height, img_gary, img_buf);

	memset(img_buf2, 0, IMG_MAX_SIZE * 3);
	memset(img_buf3, 0, IMG_MAX_SIZE * 3);

	BinaryExpansion(l_img_width, l_img_height, img_buf, img_buf2);

	Gray2Rgb(l_img_width, l_img_height, img_buf2, img_buf3);

	// 设置图像数据
	dlgProcessed.SetImageData(l_img_width, l_img_height, img_buf3, "膨胀");

	// 显示对话框
	dlgProcessed.DoModal();
}

void CChildView::OnImageGrayCorrosion()
{
	unsigned char crossKernelData[9] = {
	0, 1, 0,
	1, 1, 1,
	0, 1, 0
	};

	KERNEL crossKernel = {
		3,          
		3,          
		1,          
		1,          
		crossKernelData
	};

	CProcessedImageDialog dlgProcessed;

	RGBCorrosion(l_img_width, l_img_height, img_rgb, crossKernel, img_buf);

	dlgProcessed.SetImageData(l_img_width, l_img_height, img_buf, "灰度腐蚀");

	dlgProcessed.DoModal();
}

void CChildView::OnImageGrayExpansion()
{
	unsigned char crossKernelData[9] = {
	0, 1, 0,
	1, 1, 1,
	0, 1, 0
	};

	KERNEL crossKernel = {
		3,          
		3,          
		1,          
		1,          
		crossKernelData
	};

	CProcessedImageDialog dlgProcessed;

	RGBExpansion(l_img_width, l_img_height, img_rgb, crossKernel, img_buf);

	dlgProcessed.SetImageData(l_img_width, l_img_height, img_buf, "灰度膨胀");

	dlgProcessed.DoModal();
}

void CChildView::OnImageOpenOperation()
{
	unsigned char crossKernelData[9] = {
	0, 1, 0,
	1, 1, 1,
	0, 1, 0
	};

	KERNEL crossKernel = {
		3,          
		3,          
		1,          
		1,          
		crossKernelData
	};

	CProcessedImageDialog dlgProcessed;

	/*RGBCorrosion(l_img_width, l_img_height, img_rgb, crossKernel, img_buf);

	RGBExpansion(l_img_width, l_img_height, img_buf, crossKernel, img_buf2);*/

	RGBOpening(l_img_width, l_img_height, img_rgb, crossKernel, img_buf);

	dlgProcessed.SetImageData(l_img_width, l_img_height, img_buf, "开运算");

	dlgProcessed.DoModal();
}

void CChildView::OnImageCloseOperation()
{
	unsigned char crossKernelData[9] = {
	0, 1, 0,
	1, 1, 1,
	0, 1, 0
	};

	KERNEL crossKernel = {
		3,          
		3,          
		1,          
		1,          
		crossKernelData
	};

	CProcessedImageDialog dlgProcessed;

	/*RGBExpansion(l_img_width, l_img_height, img_rgb, crossKernel, img_buf);

	RGBCorrosion(l_img_width, l_img_height, img_buf, crossKernel, img_buf2);*/

	RGBClosing(l_img_width, l_img_height, img_rgb, crossKernel, img_buf);

	dlgProcessed.SetImageData(l_img_width, l_img_height, img_buf, "闭运算");

	dlgProcessed.DoModal();
}

void CChildView::OnImageSkeletonExtraction()
{
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	OtsuThresholdSegmentation(l_img_width, l_img_height, img_gary, img_buf);

	memset(img_buf2, 0, IMG_MAX_SIZE * 3);
	memset(img_buf3, 0, IMG_MAX_SIZE * 3);

	ZhangSuenThinning(l_img_width, l_img_height, img_buf, img_buf2);

	Gray2Rgb(l_img_width, l_img_height, img_buf2, img_buf3);

	CProcessedImageDialog dlgProcessed;

	dlgProcessed.SetImageData(l_img_width, l_img_height, img_buf3, "骨架提取");

	dlgProcessed.DoModal();
}

void CChildView::OnImageSingleScaleRetinex()
{
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	memset(img_buf, 0, IMG_MAX_SIZE * 3);

	//GaussianFilterGray(l_img_width, l_img_height, img_gary, img_buf, 80);

	//ShowBinaryImage("高斯滤波", l_img_width, l_img_height, img_buf);
	
	SingleScaleRetinex(l_img_width, l_img_height, img_gary, img_buf);

	ShowBinaryImage("单尺度Retinex算法", l_img_width, l_img_height, img_buf);

	//锐化
	Sharpening(l_img_width, l_img_height, img_buf, img_buf2);

	ShowBinaryImage("锐化", l_img_width, l_img_height, img_buf2);
}

void CChildView::OnImageLinearStretching()
{
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	memset(img_buf, 0, IMG_MAX_SIZE * 3);

	HistogramSqrt(l_img_width, l_img_height, img_gary, img_buf);

	//ShowBinaryImage("线性变换", l_img_width, l_img_height, img_buf);
}

void CChildView::OnImagePiecewiseLinearStretching()
{
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	memset(img_buf, 0, IMG_MAX_SIZE * 3);

	//CLAHE(l_img_width, l_img_height, l_img_width / 8, l_img_height / 8, 1296, img_gary, img_buf);

	LocalHistogramEqualization(l_img_width, l_img_height, img_gary, img_buf, 480, 270);

	ShowBinaryImage("CLAHE", l_img_width, l_img_height, img_buf);

	//ShowBinaryImage("分段线性变换", l_img_width, l_img_height, img_buf);
}

void CChildView::OnImageHistogramEqualization()
{
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	memset(img_buf, 0, IMG_MAX_SIZE * 3);

	HistogramEqualization(l_img_width, l_img_height, img_gary, img_buf);

	//ShowBinaryImage("直方图均衡化", l_img_width, l_img_height, img_buf);
}

void CChildView::OnImageWeightedHistogramEqualization()
{
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	memset(img_buf, 0, IMG_MAX_SIZE * 3);

	WeightedHistogramEqualization(l_img_width, l_img_height, img_gary, img_buf);
}

void CChildView::OnImageLogarithmicTransform()
{
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	memset(img_buf, 0, IMG_MAX_SIZE * 3);

	LogarithmicTransform(l_img_width, l_img_height, img_gary, img_buf);

	//ShowBinaryImage("对数变换", l_img_width, l_img_height, img_buf);
}

void CChildView::OnImageBilateralFilter()
{
	CProcessedImageDialog dlgProcessed;

	memset(img_buf, 0, IMG_MAX_SIZE * 3);
	memset(img_buf2, 0, IMG_MAX_SIZE * 3);
	RgbToGary(l_img_width, l_img_height, img_rgb, img_gary);

	//bilateralFilter(img_gary, img_buf, l_img_width, l_img_height, 3, 10, 5);

	Gray2Rgb(l_img_width, l_img_height, img_buf, img_buf2);

	dlgProcessed.SetImageData(l_img_width, l_img_height, img_buf2, "双边滤波");

	dlgProcessed.DoModal();
}

void CChildView::OnImageMutiFrameNoiseReduction()
{
	l_img_width = 3840;
	l_img_height = 2160;

	int total_pixel = l_img_width * l_img_height;

	Read_12Bits_Raw("E:\\采集测试图像\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229161258.raw", &l_img_width, &l_img_height, img_raw1);
	Read_12Bits_Raw("E:\\采集测试图像\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229161310.raw", &l_img_width, &l_img_height, img_raw2);
	Read_12Bits_Raw("E:\\采集测试图像\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229161317.raw", &l_img_width, &l_img_height, img_raw3);
	Read_12Bits_Raw("E:\\采集测试图像\\2024-2-29\\raw\\RAW_3840x2160_12bits_RGGB_Linear_20240229161324.raw", &l_img_width, &l_img_height, img_raw4);

	for (int i = 0; i < total_pixel; i++)
	{
		img_raw[i] = (img_raw1[i] + img_raw2[i] + img_raw3[i] + img_raw4[i]) >> 2;
	}

	MalvarDemosaicing(l_img_width, l_img_height, img_raw, img_temp);
	ShowRGBImage("去马赛克", l_img_width, l_img_height, img_temp);
	AWB(l_img_width, l_img_height, img_temp, img_rgb);
	ShowRGBImage("AWB", l_img_width, l_img_height, img_rgb);
	GammaTransform(l_img_width, l_img_height, 2.2, img_rgb, img_rgb);
	ShowRGBImage("Gamma:2.2", l_img_width, l_img_height, img_rgb);
	//Raw2Rgb(l_img_width, l_img_height, img_raw, img_rgb);
	UpdateImgTemp();
	l_fileopenflag = 1;
	Invalidate();
}






