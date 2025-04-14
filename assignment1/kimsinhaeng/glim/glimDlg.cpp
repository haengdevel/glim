
// glimDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "glim.h"
#include "glimDlg.h"
#include "afxdialogex.h"
#include <iostream>
#include "process.h"
#include <chrono>
#include <future>
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW

#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif


const int nMainWidth = 1280;
const int nMainHeight = 1280;
const int nUmThreads = std::thread::hardware_concurrency();
// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
public:
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{

	CDialogEx::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CglimDlg 대화 상자
CglimDlg::CglimDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GLIM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pBackgroundDlg = new CBackgroundDlg(this);
}

void CglimDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_pBackgroundDlg->m_nRadius);
	DDV_MinMaxInt(pDX, m_pBackgroundDlg->m_nRadius, 1, nMainWidth);

	DDX_Text(pDX, IDC_EDIT_THICKNESS, m_pBackgroundDlg->m_nThickness);
	DDV_MinMaxInt(pDX, m_pBackgroundDlg->m_nThickness, 1, nMainWidth);
}

BEGIN_MESSAGE_MAP(CglimDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_EN_CHANGE(IDC_EDIT_RADIUS, &CglimDlg::OnEnChangeEditRadius)
	ON_EN_CHANGE(IDC_EDIT_THICKNESS, &CglimDlg::OnEnChangeEditThickness)
	ON_BN_CLICKED(IDC_RESET, &CglimDlg::OnBnClickedReset)
	ON_BN_CLICKED(IDC_BTN_RANDOM, &CglimDlg::OnBnClickedBtnRandom)
	ON_WM_TIMER()
END_MESSAGE_MAP()


using namespace std;
using namespace chrono;
BOOL CglimDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	cout << "CPU 코어: " << nUmThreads << endl;
	cout << "UI 넓이: " << nMainWidth << endl;
	cout << "UI 높이: " << nMainHeight << endl;

	SetIcon(m_hIcon, TRUE);	
	SetIcon(m_hIcon, FALSE);
	m_pBackgroundDlg->Create(IDD_BACKGROUND, this);
	m_pBackgroundDlg->ShowWindow(SW_SHOW);
	this->GetDlgItem(IDC_BTN_RANDOM)->EnableWindow(FALSE);

	VariableInitialization();
	return TRUE; 
}


void CglimDlg::VariableInitialization()
{
	GetDlgItem(IDC_EDIT_RADIUS)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_RANDOM)->EnableWindow(FALSE);
	SetDlgItemText(IDC_STATIC_TIMER, _T(""));
	SetDlgItemText(IDC_BTN_RANDOM, _T("랜덤 이동"));
	OnEnChangeEditRadius();
	OnEnChangeEditThickness();

	KillTimer(1);

	for (int i = 0; i < 3; i++)
		CoordinateSystem(CPoint(0, 0), i);
}

void CglimDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CglimDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CglimDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CglimDlg::OnBnClickedReset()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.3
	VariableInitialization();
	m_pBackgroundDlg->VariableInitialization();

	Invalidate();
}

void CglimDlg::OnEnChangeEditRadius()
{
	CString str;
	GetDlgItemText(IDC_EDIT_RADIUS, str);
	str.Trim();

	if (str.IsEmpty())
	{
		m_pBackgroundDlg->m_nRadius = 1;
		SetDlgItemInt(IDC_EDIT_RADIUS, m_pBackgroundDlg->m_nRadius);
	}
	else
		UpdateData(TRUE);

}

void CglimDlg::OnEnChangeEditThickness()
{
	CString str;
	GetDlgItemText(IDC_EDIT_THICKNESS, str);
	str.Trim();

	if (str.IsEmpty())
	{
		m_pBackgroundDlg->m_nThickness = 1;
		SetDlgItemInt(IDC_EDIT_THICKNESS, m_pBackgroundDlg->m_nThickness);
	}
	else
		UpdateData(TRUE);
}

void CglimDlg::OnBnClickedBtnRandom()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	GetDlgItem(IDC_BTN_RANDOM)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_RADIUS)->EnableWindow(FALSE);
	m_pBackgroundDlg->m_bCancelThreadWork = true;
	m_pBackgroundDlg->RandomPlay();
	SetTimer(1, 50, nullptr);  // 100ms 간격
}

void CglimDlg::RandomFinish()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	KillTimer(1);
	SetDlgItemText(IDC_BTN_RANDOM, _T("랜덤 이동"));
	GetDlgItem(IDC_BTN_RANDOM)->EnableWindow(TRUE);
	SetDlgItemText(IDC_STATIC_TIMER, _T(""));
}

void CglimDlg::CoordinateSystem(CPoint point, int nIndex)
{
	CString str;
	str.Format(_T("%d : x %d / y %d"), nIndex + 1, point.x, point.y);
	if (nIndex == 0)
		SetDlgItemText(IDC_STATIC_ONE, str);
	else if (nIndex == 1)
		SetDlgItemText(IDC_STATIC_TWO, str);
	else if (nIndex == 2)
		SetDlgItemText(IDC_STATIC_THREE, str);
}

void CglimDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	delete m_pBackgroundDlg;
}

void CglimDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nIDEvent == 1)
	{
		CString str;
		switch (m_nLodingCount % 7)
		{
		case 0: str = _T("●      "); break;
		case 1: str = _T(" ●     "); break;
		case 2: str = _T("  ●    "); break;
		case 3: str = _T("   ●   "); break;
		case 4: str = _T("    ●  "); break;
		case 5: str = _T("     ● "); break;
		case 6: str = _T("      ●"); break;
		}
		SetDlgItemText(IDC_STATIC_TIMER, str);
		m_nLodingCount++;
	}
	CDialogEx::OnTimer(nIDEvent);
}
