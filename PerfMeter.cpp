#include "StdAfx.h"
#include "PerfMeter.h"

CPerfMeter::CPerfMeter(HWND hDlg, HWND hCtrl)
{
	m_hParentDlg = hDlg;
	m_hControlID = hCtrl;
	m_nPercentage = 0;

	// Mimick Task Manager default colors
	SetActiveColor( RGB(0, 255, 0) );
	SetInactiveColor( RGB(0, 128, 64) );
	SetBars( 2 );
}

CPerfMeter::~CPerfMeter(void)
{
}

//	CPerfMeter::DrawMeter - Draw a Windows Task Manager "CPU Usage" and "Memory"-style segmented bar meter 
//
// Task manager dimensions:
//	Horiz: 18 black pixels, meter, 18 black pixels
//	Vert:  6 black pixels, meter, 8 black pixels, 10 pixel percentage, 6 black pixels
BOOL CPerfMeter::DrawMeter(BOOL bPaint)
{
	if ( m_nPercentage > 100 )
		return FALSE;

	HPEN hPenActive, hPenInactive;
	// 30 is the offset from the bottom to allow room for text, 3 is the 2-wide bar pen and 1-wide black spacer between bars
	int numberOfVerticalBars = (m_DIS.rcItem.bottom - 30) / 3;
	int activeBars = static_cast<int>( (m_nPercentage / 100.0) * numberOfVerticalBars );
	int inactiveBars = numberOfVerticalBars - activeBars;
	// 18 pixel offset on each side
	int meterWidth = m_DIS.rcItem.right - 36;

	int startXPosition = 18;

	hPenActive = CreatePen( PS_SOLID, 2, m_crActivePenColor );
	// TODO: This pen actually needs to be dimmed ("greyed-out") with ExtCreatePen() and PS_USERSTYLE to accurately emulate Task Manager
	hPenInactive = CreatePen( PS_SOLID, 2, m_crInactivePenColor );

	FillRect( m_DIS.hDC, &m_DIS.rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH) );

	SelectObject( m_DIS.hDC, hPenInactive );

	// Start with a 6 device unit offset from the top of the client area and draw the inactive bars
	int currentPosition = 6;
	for(int i = 0; i < inactiveBars; i++) {
		MoveToEx( m_DIS.hDC, startXPosition, currentPosition, NULL );
		LineTo( m_DIS.hDC, startXPosition + meterWidth, currentPosition );
		currentPosition += 3;
	}
	DeleteObject(hPenInactive);

	SelectObject( m_DIS.hDC, hPenActive );
	MoveToEx( m_DIS.hDC, startXPosition, currentPosition, NULL );

	// Using the current position from the last loop draw the remaining (active) bars
	for(int i = 0; i < activeBars; i++) {
		MoveToEx( m_DIS.hDC, startXPosition, currentPosition, NULL );
		LineTo( m_DIS.hDC, startXPosition + meterWidth, currentPosition );
		currentPosition += 3;
	}

	DeleteObject(hPenActive);

	HPEN hPenBlack = CreatePen( PS_SOLID, 1, RGB(0, 0, 0) );
	SelectObject( m_DIS.hDC, hPenBlack );
	MoveToEx( m_DIS.hDC, startXPosition, 0, NULL);

	// Now create the vertical black score lines through the meter bars to make them appear separate
	// TODO: Iterate one extra time and even up meter bar widths
	for(unsigned int i = 0, currentPosition = startXPosition; i < m_nBars; i++) {
		MoveToEx( m_DIS.hDC, currentPosition, 0, NULL);
		LineTo( m_DIS.hDC, currentPosition, m_DIS.rcItem.bottom );
		currentPosition += meterWidth / m_nBars;
	}

	DeleteObject( hPenBlack );

	hPenBlack = CreatePen( PS_SOLID, (meterWidth % m_nBars), RGB(0, 0, 0) );
	SelectObject( m_DIS.hDC, hPenBlack );
	MoveToEx( m_DIS.hDC, startXPosition + meterWidth, 0, NULL);
	LineTo( m_DIS.hDC, startXPosition + meterWidth, m_DIS.rcItem.bottom);
	DeleteObject( hPenBlack );

	// Draw the text at the bottom of the meter
	SetBkColor( m_DIS.hDC, RGB(0, 0, 0) );
	SetTextColor( m_DIS.hDC, m_crActivePenColor );

	int nCaptionLen = wcslen( m_cwCaption );

	RECT textRect;
	textRect.bottom = m_DIS.rcItem.bottom - 6;
	textRect.left	= 0;
	textRect.right	= m_DIS.rcItem.right;
	textRect.top	= m_DIS.rcItem.bottom - 24;
	DrawText( m_DIS.hDC, m_cwCaption, nCaptionLen, &textRect, DT_SINGLELINE | DT_CENTER | DT_BOTTOM );

	if (bPaint)
		this->Paint();

	return TRUE;
}

BOOL CPerfMeter::UpdateMeter(unsigned int nPercentActive, LPCWSTR sCaption)
{
	m_nPercentage = nPercentActive;
	int size = sizeof(m_cwCaption) / 2;
	if(sCaption) {
		wcscpy_s(m_cwCaption, size, sCaption);
	} else {
		wcscpy_s(m_cwCaption, size, L"");
	}

	return TRUE;
}

BOOL CPerfMeter::Paint() 
{
	InvalidateRect( m_hControlID, &m_DIS.rcItem, FALSE );
	UpdateWindow( m_hControlID );

	return TRUE;
}

// See http://msdn.microsoft.com/en-us/library/bb775802%28VS.85%29.aspx
void CPerfMeter::CopyDrawItemStruct(LPDRAWITEMSTRUCT lpDIS)
{
	// Control ID from the resource file
	m_DIS.CtlID = lpDIS->CtlID;
	// Control type defined in resource file, should be ODT_BUTTON for this class
	m_DIS.CtlType = lpDIS->CtlType;
	// Device context handle for drawing region
	m_DIS.hDC = lpDIS->hDC;
	// Control handle for the button
	m_DIS.hwndItem = lpDIS->hwndItem;
	m_DIS.itemAction = lpDIS->itemAction;
	m_DIS.itemData = lpDIS->itemData;
	m_DIS.itemID = lpDIS->itemID;
	m_DIS.itemState = lpDIS->itemState;
	m_DIS.rcItem = lpDIS->rcItem;

	return;
}

BOOL CPerfMeter::SetActiveColor(COLORREF activePenColor)
{
	m_crActivePenColor = activePenColor;

	return TRUE;
}

BOOL CPerfMeter::SetInactiveColor(COLORREF inactivePenColor)
{
	m_crInactivePenColor = inactivePenColor;

	return TRUE;
}

BOOL CPerfMeter::SetBars(unsigned int nNumberOfBars)
{
	m_nBars = nNumberOfBars;

	return TRUE;
}