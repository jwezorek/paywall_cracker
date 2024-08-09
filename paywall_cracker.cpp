
#include "src/gui.h"
#include <commctrl.h>

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_ LPSTR lpCmdLine,
        _In_ int nCmdShow) {

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;  // Or specify other control classes as needed
    InitCommonControlsEx(&icex);

    HFONT hFont = CreateFont(
        -MulDiv(9, GetDeviceCaps(GetDC(NULL), LOGPIXELSY), 72),
        0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,
        "Segoe UI");

    constexpr const char* wnd_class = "paywall_cracker_class";
    pwc::register_main_window(hInstance, wnd_class);
    if (! pwc::init_instance(hInstance, nCmdShow, hFont, wnd_class, "paywall cracker")) {
        return FALSE;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    DeleteObject(hFont);

    return 0;
}


