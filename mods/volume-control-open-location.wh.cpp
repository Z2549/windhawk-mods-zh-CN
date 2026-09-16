// ==WindhawkMod==
// @id              volume-control-open-location
// @name            Volume control open location
// @name:zh-CN      音量控件打开位置
// @description     Shows the volume control on the monitor where the mouse cursor is located, or on a custom monitor of choice
// @description:zh-CN 在鼠标光标所在的显示器上显示音量控件，也可指定使用固定的显示器
// @version         1.0
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lshcore
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
# 音量控制打开位置

在鼠标光标所在的显示器上，或在指定的自定义显示器上显示音量控制。

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
5. 调节音量以触发此 mod 的逻辑。
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
    The monitor number that the volume control will appear on. Set to zero to
    use the monitor where the mouse cursor is located.
  $description:zh-CN: 音量控件将要显示的显示器编号。设为 0 则使用鼠标光标所在的显示器。
- monitorInterfaceName: ""
  $name: Monitor interface name
  $name:zh-CN: 显示器接口名称
  $description: >-
    If not empty, the given monitor interface name (can also be an interface
    name substring) will be used instead of the monitor number. Can be useful if
    the monitor numbers change often. To see all available interface names, set
    any interface name, enable mod logs, change the volume and look for "Found
    display device" messages.
  $description:zh-CN: 若非空，将使用给定的显示器接口名称（也可以是接口名称的一部分）来代替显示器编号。如果显示器编号经常变化，这个选项会很有用。要查看所有可用的接口名称，请设置任意接口名称，启用 mod 日志，调节音量，然后查找“Found display device”消息。
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <shellscalingapi.h>

struct {
    int monitor;
    WindhawkUtils::StringSetting monitorInterfaceName;
} g_settings;

HMODULE g_hardwareConfirmatorModule;

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

using MonitorFromPoint_t = decltype(&MonitorFromPoint);
MonitorFromPoint_t MonitorFromPoint_Original;

using EnumDisplayDevicesW_t = decltype(&EnumDisplayDevicesW);
EnumDisplayDevicesW_t EnumDisplayDevicesW_Original;

HMONITOR GetMonitorByInterfaceNameSubstr(PCWSTR interfaceNameSubstr) {
    HMONITOR monitorResult = nullptr;

    auto monitorEnumProc = [&](HMONITOR hMonitor) -> BOOL {
        MONITORINFOEX monitorInfo = {};
        monitorInfo.cbSize = sizeof(monitorInfo);

        if (GetMonitorInfo(hMonitor, &monitorInfo)) {
            DISPLAY_DEVICE displayDevice = {
                .cb = sizeof(displayDevice),
            };

            if (EnumDisplayDevicesW_Original(monitorInfo.szDevice, 0,
                                             &displayDevice,
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

HMONITOR GetDestMonitor() {
    if (*g_settings.monitorInterfaceName.get()) {
        return GetMonitorByInterfaceNameSubstr(
            g_settings.monitorInterfaceName.get());
    } else if (g_settings.monitor == 0) {
        POINT pt;
        GetCursorPos(&pt);
        return MonitorFromPoint_Original(pt, MONITOR_DEFAULTTONEAREST);
    } else if (g_settings.monitor >= 1) {
        return GetMonitorById(g_settings.monitor - 1);
    }

    return nullptr;
}

bool IsCallerFromHardwareConfirmator(void* retAddress) {
    HMODULE module;
    if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                              GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                          (PCWSTR)retAddress, &module) &&
        module == g_hardwareConfirmatorModule) {
        return true;
    }

    return false;
}

HMONITOR WINAPI MonitorFromPoint_Hook(POINT pt, DWORD dwFlags) {
    auto original = [=] { return MonitorFromPoint_Original(pt, dwFlags); };

    if (pt.x != 0 || pt.y != 0) {
        return original();
    }

    if (!IsCallerFromHardwareConfirmator(__builtin_return_address(0))) {
        return original();
    }

    Wh_Log(L">");

    HMONITOR monitor = GetDestMonitor();
    if (!monitor) {
        return original();
    }

    return monitor;
}

using MonitorFromRect_t = decltype(&MonitorFromRect);
MonitorFromRect_t MonitorFromRect_Original;
HMONITOR WINAPI MonitorFromRect_Hook(LPCRECT lprc, DWORD dwFlags) {
    auto original = [=] { return MonitorFromRect_Original(lprc, dwFlags); };

    if (!lprc || lprc->left != 0 || lprc->top != 0 || lprc->right != 0 ||
        lprc->bottom != 0) {
        return original();
    }

    if (!IsCallerFromHardwareConfirmator(__builtin_return_address(0))) {
        return original();
    }

    Wh_Log(L">");

    HMONITOR monitor = GetDestMonitor();
    if (!monitor) {
        return original();
    }

    return monitor;
}

BOOL WINAPI EnumDisplayDevicesW_Hook(LPCWSTR lpDevice,
                                     DWORD iDevNum,
                                     PDISPLAY_DEVICEW lpDisplayDevice,
                                     DWORD dwFlags) {
    BOOL result = EnumDisplayDevicesW_Original(lpDevice, iDevNum,
                                               lpDisplayDevice, dwFlags);

    if (!result || !lpDisplayDevice || lpDevice) {
        return result;
    }

    if (!IsCallerFromHardwareConfirmator(__builtin_return_address(0))) {
        return result;
    }

    Wh_Log(L">");

    HMONITOR monitor = GetDestMonitor();
    if (!monitor) {
        return result;
    }

    MONITORINFOEX monitorInfo = {};
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (!GetMonitorInfo(monitor, &monitorInfo)) {
        return result;
    }

    if (wcscmp(lpDisplayDevice->DeviceName, monitorInfo.szDevice) == 0) {
        lpDisplayDevice->StateFlags |= DISPLAY_DEVICE_PRIMARY_DEVICE;
    } else {
        lpDisplayDevice->StateFlags &= ~DISPLAY_DEVICE_PRIMARY_DEVICE;
    }

    return result;
}

struct WinrtRect {
    float X;
    float Y;
    float Width;
    float Height;
};

using HardwareConfirmatorHost_GetPositionRect_t =
    WinrtRect*(WINAPI*)(void* pThis, WinrtRect* retval, const WinrtRect* rect);
HardwareConfirmatorHost_GetPositionRect_t
    HardwareConfirmatorHost_GetPositionRect_Original;
WinrtRect* WINAPI
HardwareConfirmatorHost_GetPositionRect_Hook(void* pThis,
                                             WinrtRect* retval,
                                             const WinrtRect* rect) {
    Wh_Log(L">");

    // Shift the input rect to 0,0 since the original function assumes that.
    WinrtRect shiftedRect = *rect;
    float offsetX = shiftedRect.X;
    float offsetY = shiftedRect.Y;
    shiftedRect.X = 0;
    shiftedRect.Y = 0;

    WinrtRect* result = HardwareConfirmatorHost_GetPositionRect_Original(
        pThis, retval, &shiftedRect);

    // Shift the result back.
    result->X += offsetX;
    result->Y += offsetY;

    return result;
}

using ScaleRelativePixelsForDevice_t = int(WINAPI*)(int deviceType,
                                                    float pixels);
ScaleRelativePixelsForDevice_t ScaleRelativePixelsForDevice_Original;
int WINAPI ScaleRelativePixelsForDevice_Hook(int deviceType, float pixels) {
    if (IsCallerFromHardwareConfirmator(__builtin_return_address(0))) {
        HMONITOR monitor = GetDestMonitor();
        if (monitor) {
            Wh_Log(L">");

            UINT dpiX = 0, dpiY = 0;
            if (SUCCEEDED(GetDpiForMonitor(monitor, MDT_EFFECTIVE_DPI, &dpiX,
                                           &dpiY))) {
                return (int)(pixels * (float)dpiX / 96.0f);
            }
        }
    }

    return ScaleRelativePixelsForDevice_Original(deviceType, pixels);
}

void LoadSettings() {
    g_settings.monitor = Wh_GetIntSetting(L"monitor");
    g_settings.monitorInterfaceName =
        WindhawkUtils::StringSetting::make(L"monitorInterfaceName");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_hardwareConfirmatorModule =
        LoadLibraryEx(L"Windows.Internal.HardwareConfirmator.dll", nullptr,
                      LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!g_hardwareConfirmatorModule) {
        Wh_Log(L"Couldn't load Windows.Internal.HardwareConfirmator.dll");
        return FALSE;
    }

    // Windows.Internal.HardwareConfirmator.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: struct winrt::Windows::Foundation::Rect __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::GetPositionRect(struct winrt::Windows::Foundation::Rect const &))"},
            &HardwareConfirmatorHost_GetPositionRect_Original,
            HardwareConfirmatorHost_GetPositionRect_Hook,
        },
    };

    if (!HookSymbols(g_hardwareConfirmatorModule, symbolHooks,
                     ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return FALSE;
    }

    HMODULE shcoreModule = GetModuleHandle(L"Shcore.dll");
    if (shcoreModule) {
        auto pScaleRelativePixelsForDevice =
            (ScaleRelativePixelsForDevice_t)GetProcAddress(
                shcoreModule, MAKEINTRESOURCEA(222));
        if (pScaleRelativePixelsForDevice) {
            WindhawkUtils::SetFunctionHook(
                pScaleRelativePixelsForDevice,
                ScaleRelativePixelsForDevice_Hook,
                &ScaleRelativePixelsForDevice_Original);
        }
    }

    WindhawkUtils::SetFunctionHook(MonitorFromPoint, MonitorFromPoint_Hook,
                                   &MonitorFromPoint_Original);

    WindhawkUtils::SetFunctionHook(MonitorFromRect, MonitorFromRect_Hook,
                                   &MonitorFromRect_Original);

    WindhawkUtils::SetFunctionHook(EnumDisplayDevicesW,
                                   EnumDisplayDevicesW_Hook,
                                   &EnumDisplayDevicesW_Original);

    return TRUE;
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();
}
