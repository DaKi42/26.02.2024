#include <windows.h>
#include <windowsX.h>
#include <tchar.h>
#include <fstream>
#include "resource.h"
#include <string>

using namespace std;

class CodingThreadDlg
{
public:
    CodingThreadDlg(void);
public:
    ~CodingThreadDlg(void);
    static BOOL CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp);
    static CodingThreadDlg* ptr;
    void Cls_OnClose(HWND hwnd);
    BOOL Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
    void Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    HWND hDialog;
};

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpszCmdLine, int nCmdShow)
{
    CodingThreadDlg dlg;
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, CodingThreadDlg::DlgProc);
}

CodingThreadDlg* CodingThreadDlg::ptr = NULL;
HANDLE hMutex;

CodingThreadDlg::CodingThreadDlg(void)
{
    ptr = this;
}

CodingThreadDlg::~CodingThreadDlg(void)
{

}

void CodingThreadDlg::Cls_OnClose(HWND hwnd)
{
    ReleaseMutex(hMutex); 
    EndDialog(hwnd, 0);
}

DWORD WINAPI Coding_Thread(LPVOID lp)
{
    CodingThreadDlg* ptr = (CodingThreadDlg*)lp;
    char buf[100];
    wifstream in(TEXT("HW.txt"));
    if (!in)
    {
        MessageBox(ptr->hDialog, TEXT("Ошибка открытия файла!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);
        return 1;
    }

    hMutex = OpenMutex(MUTEX_ALL_ACCESS, false, TEXT("{B8A2C367-10FE-494d-A869-841B2AF972E0}"));

    DWORD dwAnswer = WaitForSingleObject(hMutex, INFINITE);

    wstring fileContent;
    wstring line;
    while (getline(in, line))
    {
        fileContent += line;
        fileContent += L"\r\n";
    }
    in.close();
    ReleaseMutex(hMutex);

    wchar_t wbuf[100];
    MultiByteToWideChar(CP_UTF8, 0, buf, -1, wbuf, 100);

    HWND hEdit = GetDlgItem(ptr->hDialog, IDC_EDIT1);
    SetWindowTextW(hEdit, fileContent.c_str());

    MessageBox(ptr->hDialog, TEXT("Чтение данных из файла coding.txt завершено!"), TEXT("Мьютекс"), MB_OK | MB_ICONINFORMATION);

    return 0;
}



BOOL CodingThreadDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam)
{
    hDialog = hwnd;
    CreateThread(NULL, 0, Coding_Thread, this, 0, NULL);
    return TRUE;
}

void CodingThreadDlg::Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case IDC_BUTTON1:
        ReleaseMutex(hMutex);
        Cls_OnClose(hwnd);
        break;
    }
}

BOOL CALLBACK CodingThreadDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CLOSE:
        ptr->Cls_OnClose(hwnd);
        return TRUE;
    case WM_INITDIALOG:
        return ptr->Cls_OnInitDialog(hwnd, (HWND)wParam, lParam);
    case WM_COMMAND:
        ptr->Cls_OnCommand(hwnd, LOWORD(wParam), (HWND)lParam, HIWORD(wParam));
        return TRUE;
    }
    return FALSE;
}
