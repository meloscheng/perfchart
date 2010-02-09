#include "StdAfx.h"
#include "PerfChart.h"
#include "gditest.h"
#include <vector>

// CPerfChart() - basic constructor
//
// hParentDlg			-	handle to parent dialog holding the control
// hControlID			-	handle to owner drawn control used for chart
CPerfChart::CPerfChart(HWND hParentDlg, HWND hControlID)
{
	m_hParentDlg = hParentDlg;
	m_hControlID = hControlID;
	m_xAxisPosition = 0;

	// Default chart parameters that mimick Task Manager
	m_nScrollIncrement = 2;
	m_bInMemory = TRUE;
	m_nGridDimension = 12;
	m_crGridlinePenColor = RGB(0, 128, 64);
	m_crChartPenColor = RGB(0, 255, 0);
	m_nGridlinePenSize = 1;
	m_nChartPenSize = 1;
}

// CPerfChart() - custom constructor
//
// hParentDlg			-	handle to parent dialog holding the control
// hControlID			-	handle to owner drawn control used for chart
// gridDimension		-	number of device units each grid should be in height and width
// gridlinePenSize		-	width of the pen used to draw the grid lines
// gridlinePenColor		-	color value for pen used to draw the grid lines, built with RGB() macro
// chartPenSize			-	width of the pen used to draw the data chart
// chartPenColor		-	color value for the pen used to draw the data chart, built with RGB() macro
CPerfChart::CPerfChart(HWND hParentDlg, HWND hControlID, unsigned int gridDimension, unsigned int scrollIncrement,
					   unsigned int gridlinePenSize, COLORREF gridlinePenColor, unsigned int chartPenSize, COLORREF chartPenColor,
					   BOOL bInMemory)
{
	m_hParentDlg = hParentDlg;
	m_hControlID = hControlID;
	m_xAxisPosition = 0;

	// Custom chart parameters
	m_nScrollIncrement = scrollIncrement;
	m_bInMemory = bInMemory;
	m_nGridDimension = gridDimension;
	m_crGridlinePenColor = gridlinePenColor;
	m_crChartPenColor = chartPenColor;
	m_nGridlinePenSize = gridlinePenSize;
	m_nChartPenSize = chartPenSize;
}

CPerfChart::~CPerfChart(void)
{
}

// DrawGrid - Draw a Windows Task Manager "CPU Usage History"-style side-scrolling graph with gridlines
//
// bPaint				-	when true, invalidate the chart rectangle and have it redrawn
//
BOOL CPerfChart::DrawGrid(BOOL bPaint)
{
	HPEN hGridlinePen;

	// Extraneous since the client area is typically reported with a (0, 0) origin
	int verticalHeight  = m_DIS.rcItem.bottom - m_DIS.rcItem.top;
	int horizontalWidth = m_DIS.rcItem.right - m_DIS.rcItem.left;
	int numberOfVerticalGridlines   = horizontalWidth / m_nGridDimension;
	int numberOfHorizontalGridlines = verticalHeight / m_nGridDimension;

	// Set the number of data points to be charted
	m_nPoints = horizontalWidth / m_nScrollIncrement;

	//int nSavedDC = SaveDC(m_DIS.hDC);

	// If BitBlt() usage is toggled then perform preliminary steps
	if ( m_bInMemory ) {
		m_hDCMem = CreateCompatibleDC( m_DIS.hDC );
		m_hBMStore = CreateCompatibleBitmap( m_DIS.hDC, horizontalWidth, verticalHeight );
		m_hBM = (HBITMAP)SelectObject( m_hDCMem, m_hBMStore );
	} else {
		m_hDCMem = m_DIS.hDC;
	}

	FillRect( m_hDCMem, &m_DIS.rcItem, (HBRUSH)GetStockObject(BLACK_BRUSH) );

	SetBkMode( m_hDCMem, TRANSPARENT);

	// Select the pen to use for the grid lines
	hGridlinePen = CreatePen( PS_SOLID, m_nGridlinePenSize, m_crGridlinePenColor );
	SelectObject( m_hDCMem, hGridlinePen );

	if (m_xAxisPosition != 0)
		numberOfVerticalGridlines++;

	// Reminder: In the GDI API the origin (0,0) is in the upper left corner and the y-axis grows descendant
	// Draw the vertical grid lines
	for(int i = 0, currentPosition = (0 - m_xAxisPosition); i < numberOfVerticalGridlines; i++) {
		// TODO: Catch a negative result here
		currentPosition += m_nGridDimension;
		MoveToEx( m_hDCMem, currentPosition, verticalHeight, NULL );
		LineTo( m_hDCMem, currentPosition, 0 );
	}

	// Draw the horizontal grid lines
	for(int i = 0, currentPosition = 0; i < numberOfHorizontalGridlines; i++) {
		currentPosition += m_nGridDimension;
		MoveToEx( m_hDCMem, horizontalWidth, currentPosition, NULL );
		LineTo( m_hDCMem, 0, currentPosition );
	}

	DeleteObject( hGridlinePen );

	DrawChart( );

	//RestoreDC(m_DIS.hDC, nSavedDC);

	// Generally, if called from a WM_DRAWITEM message do not repaint the rectangle, if called from a WM_APP
	// message then do repaint the rectangle
	if ( bPaint )
		this->Paint();

	return TRUE;
}

// Draws the actual chart by building a POINT array from the deque and calling Polyline()
BOOL CPerfChart::DrawChart()
{
	POINT *pChart = NULL;
	POINT *pIter  = NULL;
	int xAxis = m_DIS.rcItem.right;

	pChart = new POINT[ m_dequeDataPoints.size() ];
	pIter  = pChart;

	double factor = static_cast<double>( m_DIS.rcItem.bottom / 100.0 );

	std::deque<long>::reverse_iterator ril;

	for(ril=m_dequeDataPoints.rbegin(); ril!=m_dequeDataPoints.rend(); ril++) {
		pIter->x = xAxis;
		pIter->y = static_cast<long>( factor * (100 - *ril) );
		pIter++;
		// TODO: Check for underflow of xAxis
		xAxis -= m_nScrollIncrement;
	}

	HPEN hChartPen = CreatePen( PS_SOLID, m_nChartPenSize, m_crChartPenColor );
	SelectObject( m_hDCMem, hChartPen );

	Polyline( m_hDCMem, pChart, m_dequeDataPoints.size() );

	DeleteObject(hChartPen);

	delete [] pChart;

	if ( m_bInMemory ) {
		BitBlt(m_DIS.hDC, m_DIS.rcItem.left, m_DIS.rcItem.top, 
			m_DIS.rcItem.right-m_DIS.rcItem.left, m_DIS.rcItem.bottom-m_DIS.rcItem.top,
			m_hDCMem, 0, 0, SRCCOPY);
		SelectObject(m_hDCMem, m_hBMStore);
		DeleteObject(m_hBM);
			DeleteObject(m_hBMStore);
		DeleteDC(m_hDCMem);
	}
	return TRUE;
}

// See http://msdn.microsoft.com/en-us/library/bb775802%28VS.85%29.aspx
void CPerfChart::CopyDrawItemStruct(LPDRAWITEMSTRUCT lpDIS)
{
	// Control ID from the resource file, e.g. IDC_BUTTON1
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
	// Control rectangle dimensions
	m_DIS.rcItem = lpDIS->rcItem;

	return;
}

// Invalidates the rectangle that was updated and forces a WM_PAINT
BOOL CPerfChart::Paint() 
{
	InvalidateRect( m_hControlID, &m_DIS.rcItem, FALSE );
	UpdateWindow( m_hControlID );

	return TRUE;
}

BOOL CPerfChart::AddDataPoint(unsigned int nPercentage)
{
	if ( nPercentage > 100 )
		return FALSE;

	unsigned long nDataPoints = static_cast<unsigned long>( m_dequeDataPoints.size() );

	// Automatically grow and shrink the deque to fit the present chart dimensions
	if ( nDataPoints >= m_nPoints ) {
		for (int i = nDataPoints - m_nPoints; i != 0; i--) {
			m_dequeDataPoints.pop_front();
		}
	}

	m_dequeDataPoints.push_back( nPercentage );

	ScrollIncrement( );

	DrawGrid( TRUE );

	return TRUE;
}

BOOL CPerfChart::ScrollIncrement()
{
	m_xAxisPosition += m_nScrollIncrement;

	if ( m_xAxisPosition >= m_nGridDimension )
		m_xAxisPosition = 0;

	return TRUE;
}