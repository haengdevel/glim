// CBackgroundDlg.cpp: 구현 파일
//

#include "pch.h"
#include "glim.h"
#include "afxdialogex.h"
#include "CBackgroundDlg.h"
#include "glimDlg.h"

// CBackgroundDlg 대화 상자

IMPLEMENT_DYNAMIC(CBackgroundDlg, CDialogEx)

CBackgroundDlg::CBackgroundDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BACKGROUND, pParent)
	, m_pThreadPool(nUmThreads)
{
	m_pParent = pParent;
}

CBackgroundDlg::~CBackgroundDlg()
{
}
int m_iClickRadiusa;
void CBackgroundDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CBackgroundDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_USER + 1, &CBackgroundDlg::OnRedrawRequest)
ON_WM_MOUSEMOVE()
ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CBackgroundDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.


	int nBpp = 8;
	m_Image.Create(nMainWidth, -nMainHeight, nBpp);
	if (nBpp == 8) {
		RGBQUAD rgb[256];
		for (int i = 0; i < 256; i++) {
			rgb[i].rgbRed = rgb[i].rgbGreen = rgb[i].rgbBlue = i;
		}
		m_Image.SetColorTable(0, 256, rgb);
	}
	CleanBackground();
	MoveWindow(0, 50, 1280, 800);
	VariableInitialization();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CBackgroundDlg::VariableInitialization()
{
	CleanBackground();
	for (int i = 0; i < 3; i++)
		m_Circles[i] = { 0, 0 };
	
	m_nClickIndex = -1;
	m_nCircleCount = 0;
	m_bDragging = false;
	m_bCancelThreadWork = false;
	m_nRadius = 10;
	m_nThickness = 2;


}

void CBackgroundDlg::CleanBackground()
{
	int nPitch = m_Image.GetPitch();
	unsigned char* fm = (unsigned char*)m_Image.GetBits();
	memset(fm, 0xff, nMainHeight * abs(nPitch));
}

void CBackgroundDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.

	if (m_Image) {
		m_Image.Draw(dc, 0, 0);
	}
}


#include <future>
#include <vector>
using namespace std;
using namespace chrono;
void CBackgroundDlg::DrawCircle(CPoint point)
{
	int nPitch = m_Image.GetPitch();
	unsigned char* fm = (unsigned char*)m_Image.GetBits();

	for (int y = -m_nRadius; y <= m_nRadius; y++)
	{
		for (int x = -m_nRadius; x <= m_nRadius; x++)
		{
			if (x * x + y * y <= m_nRadius * m_nRadius)
			{
				int px = point.x + x;
				int py = point.y + y;

				if (px >= 0 && px < nMainWidth && py >= 0 && py < nMainHeight)
				{
					unsigned char* pixel = fm + py * nPitch + px;
					*pixel = 0;
				}
			}
		}
	}
}


void CBackgroundDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	int hit = HitChecking(point);
	if (hit >= 0)
	{
		m_nClickIndex = hit;
		m_bDragging = true;
		m_pLastMouse = point;
		SetCapture();
	}
	else  if (m_nCircleCount < 3 && m_nRadius > 0 && m_nRadius <= nMainWidth)
	{
		int radius = m_nRadius;
		CglimDlg* glim = (CglimDlg*)m_pParent;
		glim->GetDlgItem(IDC_EDIT_RADIUS)->EnableWindow(FALSE);

		m_Circles[m_nCircleCount].x = point.x;
		m_Circles[m_nCircleCount].y = point.y;

		DrawCircle(point);
		glim->CoordinateSystem(point, m_nCircleCount);
		m_nCircleCount++;
		if (m_nCircleCount == 3) {
			glim->GetDlgItem(IDC_BTN_RANDOM)->EnableWindow(TRUE);
			DrawCircumCircleToBuffer();
		}

		Invalidate(FALSE);
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


#define _USE_MATH_DEFINES
#include "math.h"
void CBackgroundDlg::DrawCircumCircleToBuffer()
{
	if (m_nCircleCount < 3)
		return;

	CircleData a = m_Circles[0];
	CircleData b = m_Circles[1];
	CircleData c = m_Circles[2];

	double A1 = b.x - a.x;
	double B1 = b.y - a.y;
	double C1 = (A1 * (a.x + b.x) + B1 * (a.y + b.y)) * 0.5f;

	double A2 = c.x - a.x;
	double B2 = c.y - a.y;
	double C2 = (A2 * (a.x + c.x) + B2 * (a.y + c.y)) * 0.5f;

	double D = A1 * B2 - A2 * B1;
	if (D == 0) return;

	double cx = (C1 * B2 - C2 * B1) / D;
	double cy = (A1 * C2 - A2 * C1) / D;

	double dx = a.x - cx;
	double dy = a.y - cy;
	int radius = (int)sqrt(dx * dx + dy * dy);

	int nPitch = m_Image.GetPitch();
	unsigned char* buffer = (unsigned char*)m_Image.GetBits();

	int nPrevX = -1, nPrevY = -1;
	for (int j = 0; j <= 1080; j++)
	{
		double rad = j * M_PI / 360.0f;
		int px = (int)(cx + radius * cos(rad));
		int py = (int)(cy + radius * sin(rad));

		if (nPrevX != -1 && nPrevY != -1)
		{
			int dx = px - nPrevX;
			int dy = py - nPrevY;
			int steps = max(abs(dx), abs(dy));
			if (steps == 0)
				continue;

			for (int i = 0; i <= steps; ++i)
			{
				int x = nPrevX + i * dx / steps;
				int y = nPrevY + i * dy / steps;

				for (int m = -m_nThickness / 2; m <= m_nThickness / 2; m++)
				{
					for (int z = -m_nThickness / 2; z <= m_nThickness / 2; z++)
					{
						int drawX = x + z;
						int drawY = y + m;

						if (drawX >= 0 && drawX < nMainWidth && drawY >= 0 && drawY < nMainHeight)
						{
							unsigned char* pixel = buffer + drawY * nPitch + drawX;
							*pixel = 0;
						}
					}
				}
			}
		}

		nPrevX = px;
		nPrevY = py;
	}
}

void CBackgroundDlg::RandomPlay()
{
	m_pThreadPool.enqueue([this]() {
		auto start = steady_clock::now();

		for (int k = 0; k < 10; ++k) {
			if (!m_bCancelThreadWork) return;

			auto frameStart = steady_clock::now();

			CleanBackground();
			m_nCircleCount = 0;

			for (int i = 0; i < 3; ++i)
			{
				CPoint pt(rand() % nMainWidth, rand() % nMainHeight);
				m_Circles[i].x = pt.x;
				m_Circles[i].y = pt.y;
				DrawCircle(pt);
				((CglimDlg*)m_pParent)->CoordinateSystem(pt, i);
				m_nCircleCount++;
			}

			DrawCircumCircleToBuffer();
			::PostMessage(m_hWnd, WM_USER + 1, 0, 0);

			auto nextFrame = frameStart + milliseconds(500);
			this_thread::sleep_until(nextFrame);

			if (k == 9)
			{
				((CglimDlg*)m_pParent)->RandomFinish();
				m_bCancelThreadWork = false;
			}

		}
		});
}

int CBackgroundDlg::HitChecking(CPoint pt)
{
	for (int i = 0; i < m_nCircleCount; ++i)
	{
		int nX = pt.x - m_Circles[i].x;
		int nY = pt.y - m_Circles[i].y;
		int ndistSq = nX * nX + nY * nY;
		if (ndistSq <= m_nRadius * m_nRadius)
			return i;
	}
	return -1;
}

void CBackgroundDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (m_bDragging)
	{
		m_bDragging = false;
		m_nClickIndex = -1;
		ReleaseCapture();
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CBackgroundDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (m_bDragging && m_nClickIndex >= 0)
	{
		steady_clock::time_point lastDrawTime;
		auto now = steady_clock::now();
		if (duration_cast<milliseconds>(now - lastDrawTime).count() < 16) {
			return;
		}

		int dx = point.x - m_pLastMouse.x;
		int dy = point.y - m_pLastMouse.y;
		m_pLastMouse = point;

		m_Circles[m_nClickIndex].x += dx;
		m_Circles[m_nClickIndex].y += dy;

		unsigned char* fm = (unsigned char*)m_Image.GetBits();
		if (fm)
		{
			memset(fm, 0xFF, nMainWidth * nMainHeight);
			for (int i = 0; i < m_nCircleCount; ++i)
			{
				CPoint pt(m_Circles[i].x, m_Circles[i].y);
				DrawCircle(pt);
			}
			DrawCircumCircleToBuffer();
		}

		((CglimDlg*)m_pParent)->CoordinateSystem(point, m_nClickIndex);
		RedrawWindow(NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

LRESULT CBackgroundDlg::OnRedrawRequest(WPARAM, LPARAM)
{
	Invalidate(FALSE);  // 화면 갱신 요청
	return 0;
}