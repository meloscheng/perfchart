# perfchart - Clone of side-scrolling charts and meter bars in Windows Task Manager
Automatically exported from code.google.com/p/perfchart (from Keith Jarvis)
Code license : MIT License 

Two C++ classes that utilize the Windows GDI API to recreate the graphs and meters used in the Windows Task Manager application. 


Background

Many people have likely identified a need to include simple charting for performance monitoring or other reasons in their programs. Since most Windows users are familiar with the Task Manager interface they will often assume that this application uses some standard interface or library published by Microsoft. A quick look at taskmgr.exe with Spy++ or a resource editor will reveal the graph and meter controls are implemented by DavesFrameClass. Sadly, this source code is not publicly available as discussed on Microsoft's blog. This project attempts to closely recreate this interface and allow some extensibility.
License

This source code is released into the public domain and may be used, with or without attribution, in any project commercial or otherwise.
Usage

1. Include the files PerfChart.cpp, PerfChart.h, PerfMeter.cpp and PerfMeter.h into your project.

2. Either programmatically or using the resource layout editor create a Button Control inside the target dialog with the Client Edge (WS_EX_CLIENTEDGE), Owner Draw (BS_OWNERDRAW), and Disabled (WS_DISABLED) properties set to True and take note of the controls ID (e.g. IDC_BUTTON1).

3. In the dialog's WM_INITDIALOG message handler instantiate a CPerfChart or CPerfMeter object:

        HWND hButton1Wnd = ::GetDlgItem( hDlg, IDC_BUTTON1 );
        g_pButton1 = new CPerfChart( hDlg, hButton1Wnd );

in this case the object is assigned to a global CPerfChart pointer.

4. In the dialog's WM_DRAWITEM message handler copy the DRAWITEMSTRUCT object into the CPerfChart or CPerfMeter object and draw the grid:

if ( (wParam == IDC_BUTTON1) ) 
{
        LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) lParam;
        g_pButton1->CopyDrawItemStruct( lpDIS );
        g_pButton1->DrawGrid( FALSE );
}

Calls to DrawGrid() and DrawMeter() from within this handler should always be called with bPaint = FALSE.

5. Now that the object is created and drawn a CPerfChart object can be updated with AddDataPoint() and a CPerfMeter object with UpdateMeter(). Both methods take input as an integer percentage from 0 to 100, inclusive. Just how and when these methods are called is up to the designer. For example, they could be called regularly from within a thread or upon each expiration of a timer. 
