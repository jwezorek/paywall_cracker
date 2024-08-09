#include "gui.h"
#include "cracker_thread.h"
#include "../resources/Resource.h"
#include <string>
#include <tuple>

namespace {

    constexpr int64_t k_pulse_button_id = 103;
    constexpr int64_t k_main_button_id = 104;
    constexpr int k_default_preamble = 130;
    constexpr int k_default_interval = 65;
    constexpr auto k_preamble_str = "preamble duration";
    constexpr auto k_interval_str = "interval duration";
    constexpr auto k_pulse_mode_str = "pulse mode";

    HINSTANCE g_inst;

    HWND create_labeled_edit_ctrl(HWND parent, const char* txt, int x, int y, int wd, int row_hgt, HINSTANCE inst) {
        CreateWindow(
            "static",
            txt,
            WS_CHILD | WS_VISIBLE,
            x, y, wd, row_hgt,
            parent, (HMENU)101,
            inst,
            NULL
        );
        return CreateWindowEx(
            WS_EX_CLIENTEDGE, "edit", "0",
            WS_CHILD | WS_VISIBLE | ES_NUMBER,
            x, y + row_hgt, wd, row_hgt, parent,
            (HMENU)102,
            inst,
            NULL
        );
    }

    HWND create_labeled_checkbox(HWND parent, const char* txt, int x, int y, int wd, int row_hgt, HINSTANCE inst) {
        return CreateWindowEx(
            0, "button", txt,
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            x, y + row_hgt, wd, row_hgt, parent,
            reinterpret_cast<HMENU>(k_pulse_button_id),
            inst,
            NULL
        );
    }

    void set_edit_value(HWND ctrl, int val) {
        auto str = std::to_string(val);
        SetWindowText(ctrl, str.c_str());
    }

    std::tuple<HWND, HWND, HWND, HWND> create_ctrls(HWND parent, HINSTANCE h_inst) {
        RECT r;
        GetClientRect(parent, &r);

        constexpr int k_row_hgt = 25;
        constexpr int k_space_hgt = 6;
        constexpr int k_marg = 6;

        auto x = r.left + k_marg;
        auto y = r.top + k_marg;
        auto wd = r.right - r.left - 2 * k_marg;

        auto preamble = create_labeled_edit_ctrl(parent, k_preamble_str,
            x, y, wd, k_row_hgt, h_inst);
        auto interval = create_labeled_edit_ctrl(parent, k_interval_str,
            x, y + 2 * k_row_hgt + k_space_hgt, wd, k_row_hgt, h_inst);
        auto pulse = create_labeled_checkbox(parent, k_pulse_mode_str,
            x, y + 3 * k_row_hgt + k_space_hgt + k_marg, wd, k_row_hgt, h_inst);

        auto btn = CreateWindow(
            "button",
            "crack it",
            WS_CHILD | BS_PUSHBUTTON | WS_VISIBLE,
            x, y + 5 * k_row_hgt + 3 * k_space_hgt, wd, k_row_hgt,
            parent,
            reinterpret_cast<HMENU>(k_main_button_id),
            h_inst,
            NULL
        );

        set_edit_value(preamble, k_default_preamble);
        set_edit_value(interval, k_default_interval);

        return { preamble, interval, pulse, btn };
    }

    LRESULT CALLBACK wnd_proc(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        static pwc::state state(wnd);

        switch (message) {
        case WM_CREATE: {
            auto [preamble, interval, pulse, btn] = create_ctrls(wnd, g_inst);
            state.set_ctrls(preamble, interval, pulse, btn);
        }
                      break;
        case WM_COMMAND:
            if (LOWORD(wParam) == k_main_button_id) {
                handle_main_button_click(state);
            } 
            break;

        case WM_SETFONT: {
            auto result = DefWindowProc(wnd, message, wParam, lParam);
            state.set_font(reinterpret_cast<HFONT>(wParam));
            return result;
        }

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case pwc::MSG_COMPLETE:
            handle_thread_complete(state);
            break;
        default:
            return DefWindowProc(wnd, message, wParam, lParam);
        }
        return 0;
    }
}

ATOM pwc::register_main_window(HINSTANCE hInstance, const std::string& wnd_class)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wnd_proc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PAYWALLCRACKER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_3DFACE + 1);
    wcex.lpszMenuName = 0;

    char wc[255];
    strcpy_s(&wc[0], 255, wnd_class.c_str());
    wcex.lpszClassName = wc;

    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassEx(&wcex);
}

BOOL pwc::init_instance(HINSTANCE hInstance, int nCmdShow, HFONT font,
    const std::string& wnd_class, const std::string& title) {
    g_inst = hInstance;
    HWND hWnd = CreateWindow(wnd_class.c_str(), title.c_str(), WS_POPUPWINDOW | WS_CAPTION,
        CW_USEDEFAULT, CW_USEDEFAULT, 200, 220, nullptr, nullptr, hInstance, nullptr);

    SendMessage(hWnd, WM_SETFONT, reinterpret_cast<WPARAM>(font), 0);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}