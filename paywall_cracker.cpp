
#include "src/gui.h"

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_ LPSTR lpCmdLine,
        _In_ int nCmdShow) {

    constexpr const char* wnd_class = "paywall_cracker_class";
    pwc::register_main_window(hInstance, wnd_class);
    if (! pwc::init_instance(hInstance, nCmdShow, wnd_class, "paywall cracker")) {
        return FALSE;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


