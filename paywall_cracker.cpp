#include "resources/Resource.h"
#include "src/cracker_thread.h"
#include "src/chrome.h"
#include "src/gui.h"
#include <tuple>
#include <string>
#include <thread>
#include <chrono>
#include <atomic>     

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
        _In_opt_ HINSTANCE hPrevInstance,
        _In_ LPSTR lpCmdLine,
        _In_ int nCmdShow) {

    constexpr const char* wnd_class = "paywall_cracker_class";
    register_main_window(hInstance, wnd_class);
    if (!init_instance(hInstance, nCmdShow, wnd_class, "paywall cracker")) {
        return FALSE;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


