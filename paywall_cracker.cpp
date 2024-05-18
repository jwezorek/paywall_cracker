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

    constexpr int MAX_LOADSTRING = 100;
    CHAR szTitle[MAX_LOADSTRING];
    CHAR szWindowClass[MAX_LOADSTRING];
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_PAYWALLCRACKER, szWindowClass, MAX_LOADSTRING);

    register_main_window(hInstance, szWindowClass);
    if (!init_instance(hInstance, nCmdShow, szWindowClass, szTitle)) {
        return FALSE;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}


