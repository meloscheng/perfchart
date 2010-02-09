#pragma once

class CPerfMeter
{
public:
	DRAWITEMSTRUCT m_DIS;
	HWND m_hControlID;
	HWND m_hParentDlg;
private:
	COLORREF m_crActivePenColor, m_crInactivePenColor;
	unsigned int m_nBars;
	unsigned int m_nPercentage;
	wchar_t m_cwCaption[100];
public:
	CPerfMeter(HWND, HWND);
	~CPerfMeter(void);
	BOOL DrawMeter(BOOL);
	BOOL UpdateMeter(unsigned int, LPCWSTR);
	void CopyDrawItemStruct(LPDRAWITEMSTRUCT);
	BOOL SetActiveColor(COLORREF);
	BOOL SetInactiveColor(COLORREF);
	BOOL SetBars(unsigned int);
private:
	BOOL Paint();
};
