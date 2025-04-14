#pragma once
#include "afxdialogex.h"
#include "CThreadPool.h"

struct CircleData {
	int x = 0;
	int y = 0;
};

class CBackgroundDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBackgroundDlg)

public:
	CBackgroundDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CBackgroundDlg();
	int m_nRadius = 10;
	int m_nThickness = 2;
	std::atomic<bool> m_bCancelThreadWork = false;

private:
	CImage m_Image;
	CWnd* m_pParent;
	CircleData m_Circles[3] = {};
	int m_nClickIndex = -1;
	int m_nCircleCount = 0;
	bool m_bDragging = false;
	CPoint m_pLastMouse;
	CThreadPool m_pThreadPool;

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BACKGROUND };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual LRESULT OnRedrawRequest(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void VariableInitialization();
	void CleanBackground();
	afx_msg void OnPaint();
	int HitChecking(CPoint pt);
	void DrawCircle(CPoint point);
	void DrawCircumCircleToBuffer();
	void RandomPlay();
	
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);


};
