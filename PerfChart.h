#pragma once

#include <deque>

class CPerfChart
{
public:
	// Structure passed to the WM_DRAWITEM message for the owner-drawn button
	DRAWITEMSTRUCT m_DIS;
	// Window handle to the dialog owning the chart control
	HWND m_hParentDlg;
private:
	// Device context handle for in-memory drawing operations
	HDC m_hDCMem;
	// Bitmaps used during bitblt operations
	HBITMAP m_hBM, m_hBMStore;
	// Window handle to the button control within the parent dialog
	HWND m_hControlID;
	// Position on the x-axis where first gridline should be drawn, used for side-scrolling
	int m_xAxisPosition;
	// Number of device units to scroll along the x-axis each time a data point is added
	int m_nScrollIncrement;
	// The device unit dimension of each square grid box
	int m_nGridDimension;
	// The size of the deque holding the points data
	int m_nPoints;
	// Toggle to determine if BitBlt() is performed. This should be set to TRUE for charts that 
	// update more than once a second to prevent flicker. A small memory increase is incurred
	BOOL m_bInMemory;
	// Colors for the gridlines and data plot
	COLORREF m_crGridlinePenColor, m_crChartPenColor;
	// Width for each pen
	int m_nGridlinePenSize, m_nChartPenSize;
	// STL deque to hold the data points
	std::deque<long> m_dequeDataPoints;
public:
	CPerfChart(HWND hParentDlg, HWND hControlID);
	CPerfChart(HWND hParentDlg, HWND hControlID, unsigned int gridDimension, unsigned int scrollIncrement,
				unsigned int gridlinePenSize, COLORREF gridlinePenColor, unsigned int chartPenSize, COLORREF chartPenColor,
				BOOL bInMemory);
	~CPerfChart(void);
	BOOL DrawGrid(BOOL bPaint);
	BOOL AddDataPoint(unsigned int nPercentage);
	void CopyDrawItemStruct(LPDRAWITEMSTRUCT lpDIS);
private:
	BOOL DrawChart(void);
	BOOL Paint(void);
	BOOL ScrollIncrement(void);
};
