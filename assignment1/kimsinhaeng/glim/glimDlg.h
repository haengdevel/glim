
// glimDlg.h: 헤더 파일
//
#include "CBackgroundDlg.h"

#pragma once

extern const int nMainWidth;
extern const int nMainHeight;
extern const int nUmThreads;

// CglimDlg 대화 상자
class CglimDlg : public CDialogEx
{
// 생성입니다.
public:
	CglimDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.
	int m_nLodingCount = 0;
private:
	CBackgroundDlg* m_pBackgroundDlg;

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GLIM_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void VariableInitialization();
	void RandomFinish();
	afx_msg void OnDestroy();
	afx_msg void OnEnChangeEditRadius();
	afx_msg void OnEnChangeEditThickness();
	afx_msg void OnBnClickedReset();
	afx_msg void OnBnClickedBtnRandom();

	void CoordinateSystem(CPoint point, int nIndex);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
