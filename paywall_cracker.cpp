#include "framework.h"
#include "paywall_cracker.h"
#include "cracker_thread.h"
#include "chrome.h"
#include <tuple>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>

#define MAX_LOADSTRING 100

HINSTANCE hInst;                                
CHAR szTitle[MAX_LOADSTRING];                
CHAR szWindowClass[MAX_LOADSTRING]; 

ATOM register_main_window(HINSTANCE hInstance);
BOOL init_instance(HINSTANCE, int);
LRESULT CALLBACK wnd_proc(HWND, UINT, WPARAM, LPARAM);

int64_t k_button_id = 103;
int k_default_preamble = 300;
int k_default_interval = 80;
int k_default_count = 3;

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_PAYWALLCRACKER, szWindowClass, MAX_LOADSTRING);
    register_main_window(hInstance);

    // Perform application initialization:
    if (!init_instance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PAYWALLCRACKER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


ATOM register_main_window(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = wnd_proc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAYWALLCRACKER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_3DFACE+1);
    wcex.lpszMenuName = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

BOOL init_instance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; 

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_POPUPWINDOW | WS_CAPTION,
      CW_USEDEFAULT, CW_USEDEFAULT, 200, 250, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

HWND create_labeled_edit_ctrl(HWND parent, const char* txt, int x, int y, int wd, int row_hgt) {
    CreateWindow("static", txt, WS_CHILD | WS_VISIBLE, x, y, wd, row_hgt, parent, (HMENU) 101, hInst, NULL);
    return CreateWindowEx(WS_EX_CLIENTEDGE, "edit", "0", WS_CHILD | WS_VISIBLE | ES_NUMBER, x, y+row_hgt, wd, row_hgt, parent, (HMENU)102, hInst, NULL);
}

void set_edit_value(HWND ctrl, int val) {
    auto str = std::to_string(val);
    SetWindowText(ctrl, str.c_str());
}

std::tuple<HWND, HWND, HWND, HWND> create_ctrls(HWND parent) {
    RECT r;
    GetClientRect(parent, &r);

    constexpr int k_row_hgt = 25;
    constexpr int k_space_hgt = 6;
    constexpr int k_marg = 6;

    auto x = r.left + k_marg;
    auto y = r.top + k_marg;
    auto wd = r.right - r.left - 2 * k_marg;
 
    auto preamble = create_labeled_edit_ctrl(parent, "preamble duration", 
        x, y, wd, k_row_hgt);
    auto interval = create_labeled_edit_ctrl(parent, "interval duration", 
        x, y + 2*k_row_hgt + k_space_hgt, wd, k_row_hgt);
    auto count = create_labeled_edit_ctrl(parent, "count", 
        x, y + 4 * k_row_hgt + 2*k_space_hgt, wd, k_row_hgt);

    auto btn = CreateWindow("button", "crack it", WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE, 
        x, y + 6 * k_row_hgt + 3 * k_space_hgt, wd, k_row_hgt, parent, 
        reinterpret_cast<HMENU>(k_button_id), hInst, NULL);

    set_edit_value(preamble, k_default_preamble);
    set_edit_value(interval, k_default_interval);
    set_edit_value(count, k_default_count);

    return { preamble, interval, count, btn };
}
 
LRESULT CALLBACK wnd_proc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static state state(wnd);

    switch (message) {
            case WM_CREATE: {
                auto [preamble, interval, count, btn] = create_ctrls(wnd);
                state.set_ctrls(preamble, interval, count, btn);
            }
            break;
        case WM_COMMAND:
            if (LOWORD(wParam) == k_button_id) {
                handle_button_click( state );
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case MSG_COMPLETE:
            handle_thread_complete( state );
            break;
        default:
            return DefWindowProc(wnd, message, wParam, lParam);
    }
    return 0;
}

