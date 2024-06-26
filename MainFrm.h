﻿
// MainFrm.h: CMainFrame 类的接口
//

#pragma once
#include "ChildView.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame() noexcept;
protected: 
	DECLARE_DYNAMIC(CMainFrame)

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual ~CMainFrame();
	void UpdateStatusBar(const CString& info, int partIndex);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
protected:  // 控件条嵌入成员
	CToolBar          m_wndToolBar;
	CStatusBar        m_wndStatusBar;
	CChildView    m_wndView;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()

};


