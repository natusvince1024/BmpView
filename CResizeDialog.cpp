// CResizeDialog.cpp: 实现文件
//

#include "pch.h"
#include "BmpView.h"
#include "afxdialogex.h"
#include "CResizeDialog.h"


// CResizeDialog 对话框

IMPLEMENT_DYNAMIC(CResizeDialog, CDialogEx)

CResizeDialog::CResizeDialog(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent), m_lastClickedButton(0)
	, m_scaleWidth(0)
	, m_scaleHeight(0)
{

}

CResizeDialog::~CResizeDialog()
{
}

void CResizeDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_scaleWidth);
	DDX_Text(pDX, IDC_EDIT2, m_scaleHeight);
}

void CResizeDialog::OnOK()
{
    UpdateData(TRUE);  // 更新 m_scaleWidth 和 m_scaleHeight 的值

    // 验证缩放因子是否合法
    if (m_scaleWidth <= 0 || m_scaleHeight <= 0)
    {
        AfxMessageBox(_T("缩放因子必须大于0"));
        return;
    }

    m_lastClickedButton = IDOK;
    CDialogEx::OnOK();
}

void CResizeDialog::OnCancel()
{
	CDialogEx::OnCancel();
}

void CResizeDialog::OnBilinear()
{
    UpdateData(TRUE);  // 更新 m_scaleWidth 和 m_scaleHeight 的值
	// 验证缩放因子是否合法
    if (m_scaleWidth <= 0 || m_scaleHeight <= 0)
    {
		AfxMessageBox(_T("缩放因子必须大于0"));
		return;
	}

	m_lastClickedButton = ID_BILINEAR;
	EndDialog(IDOK);
}

BEGIN_MESSAGE_MAP(CResizeDialog, CDialogEx)
	ON_BN_CLICKED(ID_BILINEAR, &CResizeDialog::OnBilinear)
END_MESSAGE_MAP()


// CResizeDialog 消息处理程序
