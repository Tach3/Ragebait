#include <iostream>

#include <Windows.h>

#define GREEK_QUESTION_MARK 0x037E //';'

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;

        BYTE keyboardState[256];
        if (!GetKeyboardState(keyboardState)) {
            return 1;
        }

        WCHAR buffer[5];
        HKL layout = GetKeyboardLayout(0);

        int result = ToUnicodeEx(
            p->vkCode,
            p->scanCode,
            keyboardState,
            buffer,
            4,
            0,
            layout
        );

        if (result > 0) {
            if (buffer[0] == L';') {
                INPUT input = { 0 };
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = 0;
                input.ki.wScan = GREEK_QUESTION_MARK;
                input.ki.dwFlags = KEYEVENTF_UNICODE;

                SendInput(1, &input, sizeof(INPUT));

                return 1;
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

int APIENTRY WinMain(_In_ HINSTANCE hInst, _In_opt_ HINSTANCE hInstPrev, _In_ PSTR cmdline, _In_ int cmdshow) {
	HHOOK hSemiHook = SetWindowsHookExA(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!hSemiHook) {
        return 1;
    }
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(hSemiHook);
	return 0;
}