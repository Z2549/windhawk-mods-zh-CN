// ==WindhawkMod==
// @id              disable-rounded-corners
// @name            Disable rounded corners in Windows 11
// @name:zh-CN      关闭 Windows 11 窗口圆角
// @description     A simple mod to disable window rounded corners in Windows 11
// @description:zh-CN 一个简单的模组，用于关闭 Windows 11 中窗口的圆角效果，让窗口回归直角
// @version         1.0.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         dwm.exe
// @architecture    x86-64
// ==/WindhawkMod==

// Source code is published under The GNU General Public License v3.0.
//
// For bug reports and feature requests, please open an issue here:
// https://github.com/ramensoftware/windhawk-mods/issues
//
// For pull requests, development takes place here:
// https://github.com/m417z/my-windhawk-mods

// ==WindhawkModReadme==
/*
# 禁用 Windows 11 圆角

一个用于禁用 Windows 11 窗口圆角的简单 mod。

基于 Valentin Radu 的
[Win11DisableRoundedCorners](https://github.com/valinet/Win11DisableRoundedCorners)
项目。

![截图](https://i.imgur.com/ez0jyuW.png)

## ⚠ 重要使用说明 ⚠

此 mod 需要挂钩 `dwm.exe` 才能工作。请前往 Windhawk 的
设置 > 高级设置 > 更多高级设置 > 进程包含列表，确认 `dwm.exe` 已在列表中。

![高级设置截图](https://i.imgur.com/LRhREtJ.png)
*/
// ==/WindhawkModReadme==

#include <windhawk_utils.h>

int (*WINAPI GetEffectiveCornerStyle_Original)();
int WINAPI GetEffectiveCornerStyle_Hook() {
    return 0;
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    HMODULE udwm = GetModuleHandle(L"udwm.dll");
    if (!udwm) {
        Wh_Log(L"udwm.dll isn't loaded");
        return FALSE;
    }

    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: enum CORNER_STYLE __cdecl CTopLevelWindow::GetEffectiveCornerStyle(void))"},
            (void**)&GetEffectiveCornerStyle_Original,
            (void*)GetEffectiveCornerStyle_Hook,
        },
    };

    return HookSymbols(udwm, symbolHooks, ARRAYSIZE(symbolHooks));
}

void Wh_ModUninit() {
    Wh_Log(L">");
}
