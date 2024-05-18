#include "cracker_thread.h"
#include "chrome.h"
#include <chrono>

namespace chron = std::chrono;

namespace {

    constexpr DWORD k_keypress_duration = 15;

    int ellapsed_milliseconds(chron::steady_clock::time_point begin) {
        return static_cast<int>(
            chron::duration_cast<chron::milliseconds>(
                chron::steady_clock::now() - begin
            ).count()
            );
    }

    bool pass_time(int duration, state& state) {
        auto begin = chron::steady_clock::now();
        while (ellapsed_milliseconds(begin) < duration) {
            if (state.cancelled()) {
                return false;
            }
        }
        return true;
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

    void thread_body(state* state) {
        state->handle_start_of_thread();

        simulate_key_press(VK_F5, k_keypress_duration);
        if (!pass_time(state->preamble(), *state)) {
            state->handle_end_of_thread();
            return;
        }

        for (int i = 0; i < state->count(); ++i) {
            simulate_key_press(VK_ESCAPE, k_keypress_duration);
            if (!pass_time(state->interval(), *state)) {
                state->handle_end_of_thread();
                return;
            }
        }

        state->handle_end_of_thread();
    }

    int get_edit_value(HWND ctrl) {
        char buffer[32];
        GetWindowText(ctrl, buffer, 32);
        return std::stoi(buffer);
    }

}

/*--------------------------------------------------------------------------------------*/

state::state(HWND wnd) :
    wnd_{ wnd },
    preamble_ctrl_{ NULL },
    interval_ctrl_{ NULL },
    count_ctrl_{ NULL },
    btn_{ NULL },
    chrome_wnd_{ NULL },
    is_running_{ false },
    cancelled_{ false }
{}

void state::set_ctrls(HWND preamble, HWND interval, HWND count, HWND btn) {
    preamble_ctrl_ = preamble;
    interval_ctrl_ = interval;
    count_ctrl_ = count;
    btn_ = btn;
}
void state::set_chrome_wnd(HWND wnd) {
    chrome_wnd_ = wnd;
}

bool state::is_running() const {
    return is_running_;
}

void state::set_cancelled() {
    cancelled_ = true;
}

void state::handle_start_of_thread() {
    cancelled_ = false;
    is_running_ = true;
    SetForegroundWindow(chrome_wnd_);
}

void state::handle_end_of_thread() {
    is_running_ = false;
    PostMessage(wnd_, MSG_COMPLETE, 0, 0);
}

void state::wait_for_thread() {
    thread_.join();
}

void state::launch_thread() {
    SetWindowText(btn_, "cancel");
    auto chrome = find_all_toplevel_chrome_windows();

    if (chrome.size() != 1) {
        MessageBox(wnd_,
            "Could not find a unique Chrome window",
            "paywall cracker error",
            MB_OK | MB_ICONERROR
        );
        return;
    }
    chrome_wnd_ = chrome.front();

    preamble_ = get_edit_value( preamble_ctrl_ );
    interval_ = get_edit_value(interval_ctrl_);
    count_ = get_edit_value(count_ctrl_);

    thread_ = std::thread(
        thread_body,
        this
    );
}

HWND state::button() const {
    return btn_;
}

bool state::cancelled() const {
    return cancelled_;
}

int state::preamble() const {
    return preamble_;
}

int state::interval() const {
    return interval_;
}

int state::count() const {
    return count_;
}

/*--------------------------------------------------------------------------------------*/

void handle_button_click(state& state) {
    if (state.is_running()) {
        state.set_cancelled();
    } else {
        state.launch_thread();
    }
}

void handle_thread_complete(state& state)
{
    state.wait_for_thread();
    SetWindowText(state.button(), "crack it");
}
