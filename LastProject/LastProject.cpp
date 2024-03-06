#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <Tlhelp32.h>
#include <locale.h>
#include <conio.h>
#include "Resource.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace std;

HWND hEdit1, hList;



void InitEdit(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{


}



LRESULT CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static PROCESSENTRY32 pe32;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        InitEdit(hDlg, uMsg, wParam, lParam);
        _getch();
        break;

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, 0);
        }

        ////INFO BUTTON
        //else if (LOWORD(wParam) == IDC_BUTTON3)
        //{
        //    ShowInfo(hDlg);
        //}

        ////STOP PROCESS
        //else if (LOWORD(wParam) == IDC_BUTTON2)
        //{
        //    DeleteProcess(hDlg);
        //}

        ////UPDATE PROCESS
        //else if (LOWORD(wParam) == IDC_BUTTON1)
        //{
        //    ProcessList();
        //    _getch();
        //}

        ////ADD PROCESS
        //else if (LOWORD(wParam) == IDC_BUTTON4)
        //{
        //    AddProcess(hDlg);
        //}

    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    }
    return 0;
}

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpszCmdLine, int nCmdShow)
{
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}