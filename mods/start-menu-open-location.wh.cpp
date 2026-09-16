// ==WindhawkMod==
// @id              start-menu-open-location
// @name            Start menu open location
// @name:zh-CN      开始菜单打开位置
// @description     When clicking the Start button, opens the Start Menu on the monitor where the mouse cursor is located, or in a custom monitor of choice
// @description:zh-CN 点击开始按钮时，在鼠标光标所在的显示器上打开开始菜单，也可指定固定显示器
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
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
# 开始菜单打开位置

点击开始按钮时，在鼠标光标所在的显示器上，或在指定的自定义显示器上打开开始菜单。

## 选择显示器

你可以在 mod 设置中按编号或按接口名称选择显示器。

### 按显示器编号

把 **Monitor** 设置设为所需的显示器编号（1、2、3 等）。注意该编号可能与 Windows
显示设置中显示的显示器编号不同。

把 **Monitor** 设置为 0，即使用鼠标光标当前所在的显示器。

### 按接口名称

如果显示器编号经常变化（例如锁定电脑或重启之后），可以改用显示器的接口名称。
查找接口名称的方法：

1. 前往此 mod 的 **Advanced**（高级）选项卡。
2. 把 **Debug logging**（调试日志）设为 **Mod logs**。
3. 点击 **Show log output**（显示日志输出）。
4. 在 mod 设置中，于 **Monitor interface
   name**（显示器接口名称）字段输入任意文本（例如 `TEST`）。
5. 打开开始菜单以触发此 mod 的逻辑。
6. 在日志输出中查找包含 `Found display device` 的行。每个显示器会出现一行，例如：
   ```
   Found display device \\.\DISPLAY1, interface name: \\?\DISPLAY#DELA1D2#5&abc123#0#{e6f07b5f-ee97-4a90-b076-33f57bf4eaa7}
   Found display device \\.\DISPLAY2, interface name: \\?\DISPLAY#GSM5B09#4&def456#0#{e6f07b5f-ee97-4a90-b076-33f57bf4eaa7}
   ```
   使用「interface name:」之后的接口名称。你可能需要试验才能确定哪个接口名称对应
   哪台物理显示器。
7. 把相关接口名称（或其中一段唯一的子串）复制到 **Monitor interface name** 设置中。
8. 完成后把 **Debug logging** 设回 **None**。

当两者都配置时，**Monitor interface name** 设置优先于 **Monitor** 编号。
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- monitor: 0
  $name: Monitor
  $name:zh-CN: 显示器
  $description: >-
    The monitor number that the start menu will appear on. Set to zero to use
    the monitor where the mouse cursor is located.
  $description:zh-CN: 开始菜单将要显示的显示器编号。设为 0 则使用鼠标光标所在的显示器。
- monitorInterfaceName: ""
  $name: Monitor interface name
  $name:zh-CN: 显示器接口名称
  $description: >-
    If not empty, the given monitor interface name (can also be an interface
    name substring) will be used instead of the monitor number. Can be useful if
    the monitor numbers change often. To see all available interface names, set
    any interface name, enable mod logs, open the start menu and look for "Found
    display device" messages.
  $description:zh-CN: 若非空，将使用给定的显示器接口名称（也可以是接口名称的一部分）来代替显示器编号。如果显示器编号经常变化，这个选项会很有用。要查看所有可用的接口名称，请设置任意接口名称，启用 mod 日志，打开开始菜单，然后查找“Found display device”消息。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

struct {
    int monitor;
    WindhawkUtils::StringSetting monitorInterfaceName;
} g_settings;

thread_local bool g_inShowStartView;

HMONITOR GetMonitorById(int monitorId) {
    HMONITOR monitorResult = nullptr;
    int currentMonitorId = 0;

    auto monitorEnumProc = [&](HMONITOR hMonitor) -> BOOL {
        if (currentMonitorId == monitorId) {
            monitorResult = hMonitor;
            return FALSE;
        }
        currentMonitorId++;
        return TRUE;
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor,
           LPARAM dwData) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(monitorEnumProc)*>(dwData);
            return proc(hMonitor);
        },
        reinterpret_cast<LPARAM>(&monitorEnumProc));

    return monitorResult;
}

HMONITOR GetMonitorByInterfaceNameSubstr(PCWSTR interfaceNameSubstr) {
    HMONITOR monitorResult = nullptr;

    auto monitorEnumProc = [&](HMONITOR hMonitor) -> BOOL {
        MONITORINFOEX monitorInfo = {};
        monitorInfo.cbSize = sizeof(monitorInfo);

        if (GetMonitorInfo(hMonitor, &monitorInfo)) {
            DISPLAY_DEVICE displayDevice = {
                .cb = sizeof(displayDevice),
            };

            if (EnumDisplayDevices(monitorInfo.szDevice, 0, &displayDevice,
                                   EDD_GET_DEVICE_INTERFACE_NAME)) {
                Wh_Log(L"Found display device %s, interface name: %s",
                       monitorInfo.szDevice, displayDevice.DeviceID);

                if (wcsstr(displayDevice.DeviceID, interfaceNameSubstr)) {
                    Wh_Log(L"Matched display device");
                    monitorResult = hMonitor;
                    return FALSE;
                }
            }
        }
        return TRUE;
    };

    EnumDisplayMonitors(
        nullptr, nullptr,
        [](HMONITOR hMonitor, HDC hdc, LPRECT lprcMonitor,
           LPARAM dwData) -> BOOL {
            auto& proc = *reinterpret_cast<decltype(monitorEnumProc)*>(dwData);
            return proc(hMonitor);
        },
        reinterpret_cast<LPARAM>(&monitorEnumProc));

    return monitorResult;
}

using ImmersiveMonitorHelper_ConnectToMonitor_t = bool(WINAPI*)(void* pThis,
                                                                HWND hWnd,
                                                                POINT point);
ImmersiveMonitorHelper_ConnectToMonitor_t
    ImmersiveMonitorHelper_ConnectToMonitor_Original;

using XamlLauncher_ShowStartView_t =
    HRESULT(WINAPI*)(void* pThis,
                     int immersiveLauncherShowMethod,
                     int immersiveLauncherShowFlags);
XamlLauncher_ShowStartView_t XamlLauncher_ShowStartView_Original;
HRESULT WINAPI XamlLauncher_ShowStartView_Hook(void* pThis,
                                               int immersiveLauncherShowMethod,
                                               int immersiveLauncherShowFlags) {
    Wh_Log(L">");

    g_inShowStartView = true;

    HRESULT ret = XamlLauncher_ShowStartView_Original(
        pThis, immersiveLauncherShowMethod, immersiveLauncherShowFlags);

    g_inShowStartView = false;

    return ret;
}

using ImmersiveMonitorHelper_AdjustMonitorConnectedIfNeeded_t =
    HRESULT(WINAPI*)(void* pThis);
ImmersiveMonitorHelper_AdjustMonitorConnectedIfNeeded_t
    ImmersiveMonitorHelper_AdjustMonitorConnectedIfNeeded_Original;
HRESULT WINAPI
ImmersiveMonitorHelper_AdjustMonitorConnectedIfNeeded_Hook(void* pThis) {
    Wh_Log(L">");

    auto original = [=]() {
        return ImmersiveMonitorHelper_AdjustMonitorConnectedIfNeeded_Original(
            pThis);
    };

    if (!g_inShowStartView) {
        return original();
    }

    HMONITOR destMonitor = nullptr;

    if (*g_settings.monitorInterfaceName.get()) {
        destMonitor = GetMonitorByInterfaceNameSubstr(
            g_settings.monitorInterfaceName.get());
    } else if (g_settings.monitor == 0) {
        POINT pt;
        GetCursorPos(&pt);
        destMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTONEAREST);
    } else if (g_settings.monitor >= 1) {
        destMonitor = GetMonitorById(g_settings.monitor - 1);
    }

    if (!destMonitor) {
        return original();
    }

    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    GetMonitorInfo(destMonitor, &monitorInfo);

    RECT rc = monitorInfo.rcMonitor;

    POINT pt = {
        rc.left + (rc.right - rc.left) / 2,
        rc.top + (rc.bottom - rc.top) / 2,
    };

    ImmersiveMonitorHelper_ConnectToMonitor_Original(pThis, nullptr, pt);

    return S_OK;
}

void LoadSettings() {
    g_settings.monitor = Wh_GetIntSetting(L"monitor");
    g_settings.monitorInterfaceName =
        WindhawkUtils::StringSetting::make(L"monitorInterfaceName");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK twinuiPcshellSymbolHooks[] = {
        {
            {LR"(public: bool __cdecl ImmersiveMonitorHelper::ConnectToMonitor(struct HWND__ *,struct tagPOINT))"},
            &ImmersiveMonitorHelper_ConnectToMonitor_Original,
        },
        {
            {LR"(public: virtual long __cdecl XamlLauncher::ShowStartView(enum IMMERSIVELAUNCHERSHOWMETHOD,enum IMMERSIVELAUNCHERSHOWFLAGS))"},
            &XamlLauncher_ShowStartView_Original,
            XamlLauncher_ShowStartView_Hook,
        },
        {
            {LR"(public: long __cdecl ImmersiveMonitorHelper::AdjustMonitorConnectedIfNeeded(void))"},
            &ImmersiveMonitorHelper_AdjustMonitorConnectedIfNeeded_Original,
            ImmersiveMonitorHelper_AdjustMonitorConnectedIfNeeded_Hook,
        },
    };

    HMODULE twinuiPcshellModule = LoadLibraryEx(L"twinui.pcshell.dll", nullptr,
                                                LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!twinuiPcshellModule) {
        Wh_Log(L"Couldn't load twinui.pcshell.dll");
        return FALSE;
    }

    if (!HookSymbols(twinuiPcshellModule, twinuiPcshellSymbolHooks,
                     ARRAYSIZE(twinuiPcshellSymbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
