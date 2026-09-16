// ==WindhawkMod==
// @id           timer-resolution-control
// @name         Timer Resolution Control
// @name:zh-CN      计时器分辨率控制
// @description  Prevent programs from changing the Windows timer resolution and increasing power consumption
// @description:zh-CN 阻止程序修改 Windows 计时器分辨率，避免因此增加系统功耗与耗电
// @version      1.0
// @author       m417z
// @github       https://github.com/m417z
// @twitter      https://twitter.com/m417z
// @homepage     https://m417z.com/
// @include      *
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
# 计时器分辨率控制

Windows 上默认的计时器分辨率为 15.6 毫秒。程序提高计时器频率会增加功耗并损害
电池续航。此 mod 提供配置，用以决定哪些程序可以更改计时器分辨率。

更多细节：
[Windows Timer Resolution: Megawatts Wasted](https://randomascii.wordpress.com/2013/07/08/windows-timer-resolution-megawatts-wasted/)

更改会在目标程序下次更改计时器分辨率时生效。为确保更改生效，建议重启目标程序或
重启计算机。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- DefaultConfig: allow
  $name: Default configuration
  $name:zh-CN: 默认配置
  $description: Configuration for all programs, can be overridden for specific programs below
  $description:zh-CN: 应用于所有程序的配置，可通过下面的特定程序配置覆盖
  $options:
  - allow: Allow changing the timer resolution
  - block: Disallow changing the timer resolution
  - limit: Limit changing the timer resolution
  $options:zh-CN:
  - allow: 允许更改计时器分辨率
  - block: 禁止更改计时器分辨率
  - limit: 限制更改计时器分辨率
- DefaultLimit: 10
  $name: Default timer resolution limit (for the limit configuration)
  $name:zh-CN: 默认计时器分辨率下限（用于“限制”配置）
  $description: The lowest possible delay between timer events, in milliseconds
  $description:zh-CN: 计时器事件之间可能的最小延迟，单位为毫秒
- PerProgramConfig:
  - - Name: notepad.exe
      $name: Program name or path
      $name:zh-CN: 程序名称或路径
    - Config: allow
      $name: Program configuration
      $name:zh-CN: 程序配置
      $options:
      - allow: Allow changing the timer resolution
      - block: Disallow changing the timer resolution
      - limit: Limit changing the timer resolution
      $options:zh-CN:
      - allow: 允许更改计时器分辨率
      - block: 禁止更改计时器分辨率
      - limit: 限制更改计时器分辨率
    - Limit: 10
      $name: Program timer resolution limit (for the limit configuration)
      $name:zh-CN: 程序计时器分辨率下限（用于“限制”配置）
  $name: Per-program configuration
  $name:zh-CN: 按程序配置
*/
// ==/WindhawkModSettings==

#include <ntdef.h>
#include <ntstatus.h>

enum class Config {
    allow,
    block,
    limit
};

ULONG g_minimumResolution;
ULONG g_maximumResolution;
ULONG g_limitResolution;

Config ConfigFromString(PCWSTR string) {
    if (wcscmp(string, L"block") == 0) {
        return Config::block;
    }

    if (wcscmp(string, L"limit") == 0) {
        return Config::limit;
    }

    return Config::allow;
}

typedef NTSTATUS (WINAPI *NtSetTimerResolution_t)(ULONG, BOOLEAN, PULONG);
NtSetTimerResolution_t pOriginalNtSetTimerResolution;

NTSTATUS WINAPI NtSetTimerResolutionHook(ULONG DesiredResolution, BOOLEAN SetResolution, PULONG CurrentResolution)
{
    if (!SetResolution) {
        Wh_Log(L"< SetResolution is FALSE");
        return pOriginalNtSetTimerResolution(DesiredResolution, SetResolution, CurrentResolution);
    }

    Wh_Log(L"> DesiredResolution: %f milliseconds", (double)DesiredResolution / 10000.0);

    ULONG limitResolution = g_limitResolution;

    if (limitResolution == ULONG_MAX) {
        Wh_Log(L"* Blocking resolution change");
        return STATUS_SUCCESS;
    }

    if (DesiredResolution < limitResolution) {
        Wh_Log(L"* Overriding resolution: %f milliseconds", (double)limitResolution / 10000.0);
        DesiredResolution = limitResolution;
    }

    return pOriginalNtSetTimerResolution(DesiredResolution, SetResolution, CurrentResolution);
}

void LoadSettings()
{
    WCHAR programPath[1024];
    DWORD dwSize = ARRAYSIZE(programPath);
    if (!QueryFullProcessImageName(GetCurrentProcess(), 0, programPath, &dwSize)) {
        *programPath = L'\0';
    }

    PCWSTR programFileName = wcsrchr(programPath, L'\\');
    if (programFileName) {
        programFileName++;
        if (!*programFileName) {
            programFileName = nullptr;
        }
    }

    bool matched = false;
    Config config = Config::allow;
    int limit = 0;

    for (int i = 0; ; i++) {
        PCWSTR name = Wh_GetStringSetting(L"PerProgramConfig[%d].Name", i);
        bool hasName = *name;
        if (hasName) {
            if (programFileName && wcsicmp(programFileName, name) == 0) {
                matched = true;
            }
            else if (wcsicmp(programPath, name) == 0) {
                matched = true;
            }
        }

        Wh_FreeStringSetting(name);

        if (!hasName) {
            break;
        }

        if (matched) {
            PCWSTR configString = Wh_GetStringSetting(L"PerProgramConfig[%d].Config", i);
            config = ConfigFromString(configString);
            Wh_FreeStringSetting(configString);

            if (config == Config::limit) {
                limit = Wh_GetIntSetting(L"PerProgramConfig[%d].Limit", i);
            }

            break;
        }
    }

    if (!matched) {
        PCWSTR configString = Wh_GetStringSetting(L"DefaultConfig");
        config = ConfigFromString(configString);
        Wh_FreeStringSetting(configString);

        if (config == Config::limit) {
            limit = Wh_GetIntSetting(L"DefaultLimit");
        }
    }

    if (config == Config::block) {
        Wh_Log(L"Config loaded: Disallowing changes");
        g_limitResolution = ULONG_MAX;
    }
    else if (config == Config::limit) {
        ULONG limitResolution = limit * 10000;
        if (limitResolution > g_minimumResolution) {
            limitResolution = g_minimumResolution;
        }
        else if (limitResolution < g_maximumResolution) {
            limitResolution = g_maximumResolution;
        }

        Wh_Log(L"Config loaded: Limiting to %f milliseconds", (double)limitResolution / 10000.0);
        g_limitResolution = limitResolution;
    }
    else {
        Wh_Log(L"Config loaded: Allowing changes");
        g_limitResolution = 0;
    }
}

BOOL Wh_ModInit()
{
    Wh_Log(L"Init");

    HMODULE hNtdll = GetModuleHandle(L"ntdll.dll");
    if (!hNtdll) {
        return FALSE;
    }

    FARPROC pNtQueryTimerResolution = GetProcAddress(hNtdll, "NtQueryTimerResolution");
    if (!pNtQueryTimerResolution) {
        return FALSE;
    }

    FARPROC pNtSetTimerResolution = GetProcAddress(hNtdll, "NtSetTimerResolution");
    if (!pNtSetTimerResolution) {
        return FALSE;
    }

    ULONG MinimumResolution;
    ULONG MaximumResolution;
    ULONG CurrentResolution;
    NTSTATUS status = ((NTSTATUS(WINAPI*)(PULONG, PULONG, PULONG))pNtQueryTimerResolution)(
        &MinimumResolution, &MaximumResolution, &CurrentResolution);
    if (NT_SUCCESS(status)) {
        Wh_Log(L"NtQueryTimerResolution: min=%f, max=%f, current=%f",
            (double)MinimumResolution / 10000.0,
            (double)MaximumResolution / 10000.0,
            (double)CurrentResolution / 10000.0);
        g_minimumResolution = MinimumResolution;
        g_maximumResolution = MaximumResolution;
    }

    LoadSettings();

    Wh_SetFunctionHook((void*)pNtSetTimerResolution, (void*)NtSetTimerResolutionHook, (void**)&pOriginalNtSetTimerResolution);

    return TRUE;
}

void Wh_ModSettingsChanged()
{
    Wh_Log(L"SettingsChanged");

    LoadSettings();
}
