// ==WindhawkMod==
// @id              no-flash-window
// @name            NoFlashWindow
// @description     Prevent programs from flashing their windows on the taskbar
// @description:zh-CN 阻止程序在任务栏上闪烁窗口以吸引注意，避免打扰正在进行的其他工作
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         *
// ==/WindhawkMod==

// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# NoFlashWindow

阻止程序在任务栏上闪烁其窗口。

灵感来自 Duncan Ogilvie 的 [NoFlashWindow](https://github.com/mrexodia/NoFlashWindow)。

**注意**：Windows 11 似乎已有[原生选项](https://www.elevenforum.com/t/enable-or-disable-show-flashing-on-taskbar-apps-in-windows-11.9968/)。

![截图](https://i.imgur.com/nVmyZrM.png)
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- mode: prevent
  $name: Mode
  $name:zh-CN: 模式
  $options:
  - prevent: Prevent flashing
  - limitToOne: Limit flashing to one time
  $options:zh-CN:
  - prevent: 阻止闪烁
  - limitToOne: 限制为只闪烁一次
*/
// ==/WindhawkModSettings==

enum class Mode {
    prevent,
    limitToOne,
};

struct {
    Mode mode;
} g_settings;

using FlashWindow_t = decltype(&FlashWindow);
FlashWindow_t FlashWindow_Original;
BOOL WINAPI FlashWindow_Hook(HWND hWnd, BOOL bInvert) {
    Wh_Log(L">");

    if (g_settings.mode == Mode::prevent) {
        return TRUE;
    }

    return FlashWindow_Original(hWnd, bInvert);
}

using FlashWindowEx_t = decltype(&FlashWindowEx);
FlashWindowEx_t FlashWindowEx_Original;
BOOL WINAPI FlashWindowEx_Hook(PFLASHWINFO pfwi) {
    Wh_Log(L">");

    if (g_settings.mode == Mode::prevent) {
        return TRUE;
    }

    FLASHWINFO newFwi = *pfwi;
    newFwi.dwFlags &= ~(FLASHW_TIMER | FLASHW_TIMERNOFG);
    if (newFwi.uCount > 1) {
        newFwi.uCount = 1;
    }

    return FlashWindowEx_Original(&newFwi);
}

void LoadSettings() {
    PCWSTR mode = Wh_GetStringSetting(L"mode");
    g_settings.mode = Mode::prevent;
    if (wcscmp(mode, L"limitToOne") == 0) {
        g_settings.mode = Mode::limitToOne;
    }
    Wh_FreeStringSetting(mode);
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    Wh_SetFunctionHook((void*)FlashWindow, (void*)FlashWindow_Hook,
                       (void**)&FlashWindow_Original);

    Wh_SetFunctionHook((void*)FlashWindowEx, (void*)FlashWindowEx_Hook,
                       (void**)&FlashWindowEx_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
