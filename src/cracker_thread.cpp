#include "cracker_thread.h"
#include "chrome.h"
#include <chrono>
#include <string>
#include <format>
#include <limits>

namespace chron = std::chrono;

namespace {

    constexpr DWORD k_keypress_duration = 15;
    constexpr DWORD k_repetitions_in_pulse_mode = 4;
    constexpr auto k_crack_it_str = "crack it";
    constexpr auto k_cancel_str = "cancel";
    constexpr auto k_unique_chrome_window_str = "Could not find a unique Chrome window";
    constexpr auto k_err_msg = "paywall cracker error";

    int ellapsed_milliseconds(chron::steady_clock::time_point begin) {
        return static_cast<int>(
            chron::duration_cast<chron::milliseconds>(
                chron::steady_clock::now() - begin
            ).count()
            );
    }

    bool pass_time(int duration, pwc::state& state) {
        auto begin = chron::steady_clock::now();
        while (ellapsed_milliseconds(begin) < duration) {
            if (state.cancelled()) {
                return false;
            }
        }
        return true;
    }

    BOOL CALLBACK ChangeFont(HWND hwndChild, LPARAM lParam)
    {
        HFONT hFont = (HFONT)lParam;
        SendMessage(hwndChild, WM_SETFONT, (WPARAM)hFont, TRUE);

        return TRUE;
    }

    void simulate_key_press(UINT virtualKeyCode, DWORD duration) {
        // Setup the INPUT structure for the key down event
        INPUT input[2] = {};
        input[0].type = INPUT_KEYBOARD;
        input[0].ki.wVk = virtualKeyCode;
        input[0].ki.dwFlags = 0; // 0 for key down

        // Setup the INPUT structure for the key up event
        input[1].type = INPUT_KEYBOARD;
        input[1].ki.wVk = virtualKeyCode;
        input[1].ki.dwFlags = KEYEVENTF_KEYUP;

        // Send the key down event
        SendInput(1, &input[0], sizeof(INPUT));

        // Wait for the specified duration
        Sleep(duration);

        // Send the key up event
        SendInput(1, &input[1], sizeof(INPUT));
    }

    int get_edit_value(HWND ctrl) {
        char buffer[32];
        GetWindowText(ctrl, buffer, 32);
        return std::stoi(buffer);
    }

    bool get_check_value(HWND ctrl) {
        LRESULT chk_state = SendMessage(ctrl, BM_GETCHECK, 0, 0);
        return (chk_state == BST_CHECKED);
    }

}

/*--------------------------------------------------------------------------------------*/

pwc::state::state(HWND wnd) :
    wnd_{ wnd },
    preamble_ctrl_{ NULL },
    interval_ctrl_{ NULL },
    pulse_mode_ctrl_{ NULL },
    btn_{ NULL },
    chrome_wnd_{ NULL },
    is_running_{ false },
    cancelled_{ false },
    font_{ NULL }
{}

void pwc::state::set_ctrls(HWND preamble, HWND interval, HWND pulse, HWND btn) {
    preamble_ctrl_ = preamble;
    interval_ctrl_ = interval;
    pulse_mode_ctrl_ = pulse;
    btn_ = btn;
}

void pwc::state::toggle_check_state() {
    auto check_state = SendMessage(pulse_mode_ctrl_, BM_GETCHECK, 0, 0);
    SendMessage(pulse_mode_ctrl_, BM_SETCHECK, !check_state, 0);
}

void pwc::state::set_font(HFONT font) {
    font_ = font;
    EnumChildWindows(wnd_, ChangeFont, (LPARAM)font);
}

HFONT pwc::state::font() const {
    return font_;
}

bool pwc::state::is_running() const {
    return is_running_;
}

void pwc::state::set_cancelled() {
    cancelled_ = true;
}

void pwc::state::handle_start_of_thread() {
    cancelled_ = false;
    is_running_ = true;
    SetForegroundWindow(chrome_wnd_);
}

void pwc::state::handle_end_of_thread() {
    is_running_ = false;
    PostMessage(wnd_, MSG_COMPLETE, 0, 0);
}

void pwc::state::wait_for_thread() {
    thread_.join();
}

void pwc::state::launch_thread() {
    SetWindowText(btn_, k_cancel_str);
    auto chrome = find_all_toplevel_chrome_windows();

    if (chrome.size() != 1) {
        MessageBox(wnd_,
            k_unique_chrome_window_str,
            k_err_msg,
            MB_OK | MB_ICONERROR
        );
        SetWindowText(btn_, k_crack_it_str);
        return;
    }
    chrome_wnd_ = chrome.front();

    preamble_ = get_edit_value( preamble_ctrl_ );
    interval_ = get_edit_value(interval_ctrl_);
    count_ = get_check_value(pulse_mode_ctrl_) ? 
        k_repetitions_in_pulse_mode : 
        std::numeric_limits<int>::max();

    thread_ = std::thread(
        thread_body,
        this
    );
}

HWND pwc::state::button() const {
    return btn_;
}

bool pwc::state::cancelled() const {
    return cancelled_;
}

void pwc::state::thread_body(pwc::state* state) {
    state->handle_start_of_thread();

    simulate_key_press(VK_F5, k_keypress_duration);
    if (!pass_time(state->preamble_, *state)) {
        state->handle_end_of_thread();
        return;
    }

    for (int i = 0; i < state->count_; ++i) {
        simulate_key_press(VK_ESCAPE, k_keypress_duration);
        if (!pass_time(state->interval_, *state)) {
            state->handle_end_of_thread();
            return;
        }
    }

    state->handle_end_of_thread();
}

/*--------------------------------------------------------------------------------------*/

void pwc::handle_main_button_click(state& state) {
    if (state.is_running()) {
        state.set_cancelled();
    } else {
        state.launch_thread();
    }
}

void pwc::handle_thread_complete(state& state)
{
    state.wait_for_thread();
    SetWindowText(state.button(), k_crack_it_str);
}
