// ==WindhawkMod==
// @id              taskbar-primary-on-secondary-monitor
// @name            Primary taskbar on secondary monitor
// @name:zh-CN      主任务栏移至副显示器
// @description     Move the primary taskbar, including the tray icons, notifications, action center, etc. to another monitor
// @description:zh-CN 将主任务栏（包括托盘图标、通知、操作中心等）移动到另一台显示器
// @version         1.2.1
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @include         ShellHost.exe
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -loleaut32 -lruntimeobject -lversion -lwtsapi32
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
# 主任务栏显示在副显示器上

把主任务栏（包括托盘图标、通知、操作中心等）移到另一台显示器。

可选地，可以通过点击其空白区域来切换主任务栏（仅 Windows 11）。

![演示](https://i.imgur.com/hFU9oyK.gif)

## 选择显示器

你可以在 mod 设置中按编号或按接口名称选择显示器。

### 按显示器编号

把 **Monitor** 设置设为所需的显示器编号（1、2、3 等）。注意该编号可能与 Windows
显示设置中显示的显示器编号不同。

### 按接口名称

如果显示器编号经常变化（例如锁定电脑或重启之后），可以改用显示器的接口名称。
查找接口名称的方法：

1. 前往此 mod 的 **Advanced**（高级）选项卡。
2. 把 **Debug logging**（调试日志）设为 **Mod logs**。
3. 点击 **Show log output**（显示日志输出）。
4. 在 mod 设置中，于 **Monitor interface
   name**（显示器接口名称）字段输入任意文本（例如 `TEST`）。
5. 保存设置并等待其生效。
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
- monitor: 2
  $name: Monitor
  $name:zh-CN: 显示器
  $description: >-
    The monitor number to have the primary taskbar on
  $description:zh-CN: 主任务栏所在的显示器编号
- monitorInterfaceName: ""
  $name: Monitor interface name
  $name:zh-CN: 显示器接口名称
  $description: >-
    If not empty, the given monitor interface name (can also be an interface
    name substring) will be used instead of the monitor number. Can be useful if
    the monitor numbers change often. To see all available interface names, set
    any interface name, enable mod logs and look for "Found display device"
    messages.
  $description:zh-CN: 若非空，将使用给定的显示器接口名称（也可以是接口名称的一部分）来代替显示器编号。如果显示器编号经常变化，这个选项会很有用。要查看所有可用的接口名称，请设置任意接口名称，启用 mod 日志，然后查找“Found display device”消息。
- clickToSwitchMonitor: disabled
  $name: Click taskbar to switch monitor
  $name:zh-CN: 单击任务栏以切换显示器
  $description: >-
    Choose how clicking on a taskbar's empty space switches the primary taskbar
    to the clicked monitor (Windows 11 only).
  $description:zh-CN: 选择单击任务栏空白区域时如何将主任务栏切换到被单击的显示器（仅 Windows 11）。
  $options:
  - disabled: Disabled
  - doubleClick: Double click
  - middleClick: Middle click
  $options:zh-CN:
  - disabled: 禁用
  - doubleClick: 双击
  - middleClick: 中键单击
- moveAdditionalElements: false
  $name: Move additional elements
  $name:zh-CN: 移动附加元素
  $description: >-
    Move additional elements such as desktop icons to the target monitor.
  $description:zh-CN: 将桌面图标等附加元素移动到目标显示器。
- oldTaskbarOnWin11: false
  $name: Customize the old taskbar on Windows 11
  $name:zh-CN: 自定义 Windows 11 上的旧版任务栏
  $description: >-
    Enable this option to customize the old taskbar on Windows 11 (if using
    ExplorerPatcher or a similar tool)
  $description:zh-CN: 启用此选项以自定义 Windows 11 上的旧版任务栏（如果使用了 ExplorerPatcher 或类似工具）
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#include <psapi.h>
#include <windowsx.h>
#include <wtsapi32.h>

#undef GetCurrentTime

#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>

#include <atomic>

enum class ClickToSwitchMonitor {
    disabled,
    doubleClick,
    middleClick,
};

struct {
    int monitor;
    WindhawkUtils::StringSetting monitorInterfaceName;
    ClickToSwitchMonitor clickToSwitchMonitor;
    bool moveAdditionalElements;
    bool oldTaskbarOnWin11;
} g_settings;

enum class Target {
    Explorer,
    ShellHost,  // Win11 24H2.
    // Only for the start menu alignment, see MonitorFromWindow_Hook.
    StartMenuExperienceHost,
};

Target g_target;

enum class WinVersion {
    Unsupported,
    Win10,
    Win11,
    Win11_24H2,
};

WinVersion g_winVersion;

std::atomic<bool> g_taskbarViewDllLoaded;
std::atomic<bool> g_initialized;
std::atomic<bool> g_explorerPatcherInitialized;

std::atomic<bool> g_unloading;

// Defines data shared by all instances of the library, even across processes.
#define SHARED_SECTION __attribute__((section(".shared")))
asm(".section .shared,\"dws\"\n");

volatile HMONITOR g_overrideMonitor SHARED_SECTION = nullptr;

DWORD g_lastPressTime;
HMONITOR g_lastPressMonitor;
std::atomic<bool> g_lastIsSessionLocked;

// Each target only hooks the functions it needs, while some of the code calling
// the originals runs in all of them, so they start out as the unhooked
// functions and are replaced by the trampolines of the hooks that are set.
using MonitorFromPoint_t = decltype(&MonitorFromPoint);
MonitorFromPoint_t MonitorFromPoint_Original = MonitorFromPoint;

using MonitorFromRect_t = decltype(&MonitorFromRect);
MonitorFromRect_t MonitorFromRect_Original = MonitorFromRect;

using MonitorFromWindow_t = decltype(&MonitorFromWindow);
MonitorFromWindow_t MonitorFromWindow_Original = MonitorFromWindow;

using EnumDisplayDevicesW_t = decltype(&EnumDisplayDevicesW);
EnumDisplayDevicesW_t EnumDisplayDevicesW_Original = EnumDisplayDevicesW;

HWND FindCurrentProcessTaskbarWnd() {
    HWND hTaskbarWnd = nullptr;

    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            DWORD dwProcessId;
            WCHAR className[32];
            if (GetWindowThreadProcessId(hWnd, &dwProcessId) &&
                dwProcessId == GetCurrentProcessId() &&
                GetClassName(hWnd, className, ARRAYSIZE(className)) &&
                _wcsicmp(className, L"Shell_TrayWnd") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hWnd;
                return FALSE;
            }
            return TRUE;
        },
        reinterpret_cast<LPARAM>(&hTaskbarWnd));

    return hTaskbarWnd;
}

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

void BroadcastShellHookDisplayChange() {
    // Broadcast display change SHELLHOOK message, handled by
    // CImmersiveMonitorManager::_HandleDisplayChangeMessage in twinui.dll.
    // lParam is related to desktop rotation, later read by
    // CImmersiveMonitorManager::_HandleDeferredDesktopRotation.
    UINT shellhookMessage = RegisterWindowMessage(L"SHELLHOOK");
    PostMessage(HWND_BROADCAST, shellhookMessage, 35, 0);
}

bool IsSessionLocked() {
    WTSINFOEX* sessionInfoEx = nullptr;
    DWORD bytesReturned = 0;
    if (!WTSQuerySessionInformation(
            WTS_CURRENT_SERVER_HANDLE, WTS_CURRENT_SESSION, WTSSessionInfoEx,
            reinterpret_cast<LPWSTR*>(&sessionInfoEx), &bytesReturned)) {
        Wh_Log(L"WTSQuerySessionInformation failed: %d", GetLastError());
        return false;
    }

    bool locked = sessionInfoEx->Level == 1 &&
                  sessionInfoEx->Data.WTSInfoExLevel1.SessionFlags ==
                      WTS_SESSIONSTATE_LOCK;

    WTSFreeMemory(sessionInfoEx);
    return locked;
}

struct GetTargetMonitorParams {
    void* retAddress = nullptr;
    bool ignoreLockedState = false;
};

HMONITOR GetTargetMonitor(GetTargetMonitorParams params = {}) {
    bool sessionLocked = IsSessionLocked();

    bool sessionLockStateChanged =
        g_lastIsSessionLocked.exchange(sessionLocked) != sessionLocked;

    if (sessionLockStateChanged && g_target == Target::Explorer &&
        FindCurrentProcessTaskbarWnd()) {
        Wh_Log(L"Session lock state changed: %s",
               sessionLocked ? L"locked" : L"unlocked");

        // Notify the system about display change so that it can re-evaluate
        // which monitor is primary. This is needed for the lock screen to show
        // up on the real primary monitor.
        BroadcastShellHookDisplayChange();
    }

    if (!params.ignoreLockedState && sessionLocked) {
        return nullptr;
    }

    if (!g_settings.moveAdditionalElements && params.retAddress) {
        HMODULE shell32Module = GetModuleHandle(L"shell32.dll");
        if (shell32Module) {
            // If the caller is in shell32.dll, which mainly does things such as
            // handling desktop icons and wallpapers.
            HMODULE module;
            if (GetModuleHandleEx(
                    GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                        GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                    (PCWSTR)params.retAddress, &module) &&
                module == shell32Module) {
                return nullptr;
            }
        }
    }

    if (g_unloading) {
        // Note: Returning nullptr doesn't restore the taskbar to the primary
        // monitor on unload.
        return MonitorFromPoint_Original({0, 0}, MONITOR_DEFAULTTONEAREST);
    }

    HMONITOR monitor = g_overrideMonitor;
    if (monitor) {
        // The override monitor handle becomes invalid when the display
        // configuration changes, e.g. when a remote desktop session adds or
        // removes a virtual display. Since it takes priority over the
        // configured monitor and is never revalidated, a stale handle would
        // keep winning and the primary taskbar would end up stuck to a monitor
        // that no longer exists. Drop it and fall back to the settings.
        MONITORINFO monitorInfo = {};
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (!GetMonitorInfo(monitor, &monitorInfo)) {
            Wh_Log(L"Override monitor is stale, falling back to settings");
            g_overrideMonitor = nullptr;
            monitor = nullptr;
        }
    }

    if (!monitor) {
        if (*g_settings.monitorInterfaceName.get()) {
            monitor = GetMonitorByInterfaceNameSubstr(
                g_settings.monitorInterfaceName.get());
        } else if (g_settings.monitor >= 1) {
            monitor = GetMonitorById(g_settings.monitor - 1);
        }
    }

    return monitor;
}

HMONITOR WINAPI MonitorFromPoint_Hook(POINT pt, DWORD dwFlags) {
    auto original = [=] { return MonitorFromPoint_Original(pt, dwFlags); };

    if (pt.x != 0 || pt.y != 0) {
        return original();
    }

    Wh_Log(L">");

    HMONITOR monitor = GetTargetMonitor({
        .retAddress = __builtin_return_address(0),
    });
    if (!monitor) {
        return original();
    }

    return monitor;
}

HMONITOR WINAPI MonitorFromRect_Hook(LPCRECT lprc, DWORD dwFlags) {
    auto original = [=] { return MonitorFromRect_Original(lprc, dwFlags); };

    if (!lprc || lprc->left != 0 || lprc->top != 0 || lprc->right != 0 ||
        lprc->bottom != 0) {
        return original();
    }

    Wh_Log(L">");

    HMONITOR monitor = GetTargetMonitor({
        .retAddress = __builtin_return_address(0),
    });
    if (!monitor) {
        return original();
    }

    return monitor;
}

// Returns whether the address belongs to the start menu UI, which asks for the
// monitor to read the taskbar alignment for. StartMenu.dll is the redesigned
// start menu, StartDocked.dll the older one.
bool IsStartMenuUIAddress(void* address) {
    HMODULE module;
    if (!address ||
        !GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                               GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           (PCWSTR)address, &module)) {
        return false;
    }

    return module == GetModuleHandle(L"StartMenu.dll") ||
           module == GetModuleHandle(L"StartDocked.dll");
}

// The start menu reads the taskbar alignment from
// WindowsUdk.UI.Shell.TaskbarLayout for the monitor its core window is on,
// resolved with MONITOR_DEFAULTTOPRIMARY, and keeps its centered default if the
// layout has no alignment for that monitor. On startup it reads it while the
// window is still on the real primary monitor, which has no taskbar once the
// primary taskbar is moved away, and it isn't read again when the window moves
// to the taskbar's monitor. Point it at the taskbar's monitor instead, which is
// the one whose alignment the start menu follows.
HMONITOR WINAPI MonitorFromWindow_Hook(HWND hWnd, DWORD dwFlags) {
    auto original = [=] { return MonitorFromWindow_Original(hWnd, dwFlags); };

    if (dwFlags != MONITOR_DEFAULTTOPRIMARY) {
        return original();
    }

    void* retAddress = __builtin_return_address(0);
    if (!IsStartMenuUIAddress(retAddress)) {
        return original();
    }

    Wh_Log(L">");

    HMONITOR monitor = GetTargetMonitor({
        .retAddress = retAddress,
    });
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

    Wh_Log(L">");

    HMONITOR monitor = GetTargetMonitor({
        .retAddress = __builtin_return_address(0),
    });
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

void ApplySettings();

using TaskbarFrame_OnPointerPressed_t = int(WINAPI*)(void* pThis, void* pArgs);
TaskbarFrame_OnPointerPressed_t TaskbarFrame_OnPointerPressed_Original;
int TaskbarFrame_OnPointerPressed_Hook(void* pThis, void* pArgs) {
    Wh_Log(L">");

    auto original = [=]() {
        return TaskbarFrame_OnPointerPressed_Original(pThis, pArgs);
    };

    winrt::Windows::UI::Xaml::UIElement taskbarFrame = nullptr;
    ((IUnknown*)pThis)
        ->QueryInterface(winrt::guid_of<winrt::Windows::UI::Xaml::UIElement>(),
                         winrt::put_abi(taskbarFrame));

    if (!taskbarFrame) {
        return original();
    }

    auto className = winrt::get_class_name(taskbarFrame);
    Wh_Log(L"%s", className.c_str());

    if (className != L"Taskbar.TaskbarFrame") {
        return original();
    }

    winrt::Windows::UI::Xaml::Input::PointerRoutedEventArgs args{nullptr};
    winrt::copy_from_abi(args, pArgs);
    if (!args) {
        return original();
    }

    auto properties = args.GetCurrentPoint(taskbarFrame).Properties();

    // Using GetMessagePos sometimes returns incorrect coordinates, especially
    // in the corner of the taskbar.
    POINT pt;
    GetCursorPos(&pt);
    HMONITOR pressedMonitor =
        MonitorFromPoint_Original(pt, MONITOR_DEFAULTTONEAREST);

    if (!pressedMonitor || pressedMonitor == g_overrideMonitor) {
        return original();
    }

    bool shouldSwitch = false;

    if (g_settings.clickToSwitchMonitor == ClickToSwitchMonitor::middleClick) {
        if (properties.IsMiddleButtonPressed()) {
            shouldSwitch = true;
        }
    } else if (g_settings.clickToSwitchMonitor ==
               ClickToSwitchMonitor::doubleClick) {
        DWORD now = GetTickCount();
        if (g_lastPressMonitor == pressedMonitor &&
            now - g_lastPressTime <= GetDoubleClickTime()) {
            g_lastPressTime = 0;
            g_lastPressMonitor = nullptr;
            shouldSwitch = true;
        } else {
            g_lastPressTime = now;
            g_lastPressMonitor = pressedMonitor;
        }
    }

    if (!shouldSwitch) {
        return original();
    }

    g_overrideMonitor = pressedMonitor;
    ApplySettings();

    // Mark event as handled to prevent normal click behavior.
    args.Handled(true);
    return 0;
}

using TrayUI__SetStuckMonitor_t = HRESULT(WINAPI*)(void* pThis,
                                                   HMONITOR monitor);
TrayUI__SetStuckMonitor_t TrayUI__SetStuckMonitor_Original;
HRESULT WINAPI TrayUI__SetStuckMonitor_Hook(void* pThis, HMONITOR monitor) {
    Wh_Log(L">");

    HMONITOR targetMonitor = GetTargetMonitor({
        .ignoreLockedState = true,
    });
    if (targetMonitor) {
        monitor = targetMonitor;
    }

    return TrayUI__SetStuckMonitor_Original(pThis, monitor);
}

VS_FIXEDFILEINFO* GetModuleVersionInfo(HMODULE hModule, UINT* puPtrLen) {
    void* pFixedFileInfo = nullptr;
    UINT uPtrLen = 0;

    HRSRC hResource =
        FindResource(hModule, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (hResource) {
        HGLOBAL hGlobal = LoadResource(hModule, hResource);
        if (hGlobal) {
            void* pData = LockResource(hGlobal);
            if (pData) {
                if (!VerQueryValue(pData, L"\\", &pFixedFileInfo, &uPtrLen) ||
                    uPtrLen == 0) {
                    pFixedFileInfo = nullptr;
                    uPtrLen = 0;
                }
            }
        }
    }

    if (puPtrLen) {
        *puPtrLen = uPtrLen;
    }

    return (VS_FIXEDFILEINFO*)pFixedFileInfo;
}

WinVersion GetExplorerVersion() {
    VS_FIXEDFILEINFO* fixedFileInfo = GetModuleVersionInfo(nullptr, nullptr);
    if (!fixedFileInfo) {
        return WinVersion::Unsupported;
    }

    WORD major = HIWORD(fixedFileInfo->dwFileVersionMS);
    WORD minor = LOWORD(fixedFileInfo->dwFileVersionMS);
    WORD build = HIWORD(fixedFileInfo->dwFileVersionLS);
    WORD qfe = LOWORD(fixedFileInfo->dwFileVersionLS);

    Wh_Log(L"Version: %u.%u.%u.%u", major, minor, build, qfe);

    switch (major) {
        case 10:
            if (build < 22000) {
                return WinVersion::Win10;
            } else if (build < 26100) {
                return WinVersion::Win11;
            } else {
                return WinVersion::Win11_24H2;
            }
            break;
    }

    return WinVersion::Unsupported;
}

struct EXPLORER_PATCHER_HOOK {
    PCSTR symbol;
    void** pOriginalFunction;
    void* hookFunction = nullptr;
    bool optional = false;

    template <typename Prototype>
    EXPLORER_PATCHER_HOOK(
        PCSTR symbol,
        Prototype** originalFunction,
        std::type_identity_t<Prototype*> hookFunction = nullptr,
        bool optional = false)
        : symbol(symbol),
          pOriginalFunction(reinterpret_cast<void**>(originalFunction)),
          hookFunction(reinterpret_cast<void*>(hookFunction)),
          optional(optional) {}
};

bool HookExplorerPatcherSymbols(HMODULE explorerPatcherModule) {
    if (g_explorerPatcherInitialized.exchange(true)) {
        return true;
    }

    if (g_winVersion >= WinVersion::Win11) {
        g_winVersion = WinVersion::Win10;
    }

    EXPLORER_PATCHER_HOOK hooks[] = {
        {R"(?_SetStuckMonitor@TrayUI@@QEAAJPEAUHMONITOR__@@@Z)",
         &TrayUI__SetStuckMonitor_Original, TrayUI__SetStuckMonitor_Hook},
    };

    bool succeeded = true;

    for (const auto& hook : hooks) {
        void* ptr = (void*)GetProcAddress(explorerPatcherModule, hook.symbol);
        if (!ptr) {
            Wh_Log(L"ExplorerPatcher symbol%s doesn't exist: %S",
                   hook.optional ? L" (optional)" : L"", hook.symbol);
            if (!hook.optional) {
                succeeded = false;
            }
            continue;
        }

        if (hook.hookFunction) {
            Wh_SetFunctionHook(ptr, hook.hookFunction, hook.pOriginalFunction);
        } else {
            *hook.pOriginalFunction = ptr;
        }
    }

    if (!succeeded) {
        Wh_Log(L"HookExplorerPatcherSymbols failed");
    } else if (g_initialized) {
        Wh_ApplyHookOperations();
    }

    return succeeded;
}

bool IsExplorerPatcherModule(HMODULE module) {
    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(module, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            return false;
    }

    PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\');
    if (!moduleFileName) {
        return false;
    }

    moduleFileName++;

    if (_wcsnicmp(L"ep_taskbar.", moduleFileName, sizeof("ep_taskbar.") - 1) ==
        0) {
        Wh_Log(L"ExplorerPatcher taskbar module: %s", moduleFileName);
        return true;
    }

    return false;
}

bool HandleLoadedExplorerPatcher() {
    HMODULE hMods[1024];
    DWORD cbNeeded;
    if (EnumProcessModules(GetCurrentProcess(), hMods, sizeof(hMods),
                           &cbNeeded)) {
        for (size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++) {
            if (IsExplorerPatcherModule(hMods[i])) {
                return HookExplorerPatcherSymbols(hMods[i]);
            }
        }
    }

    return true;
}

void HandleLoadedModuleIfExplorerPatcher(HMODULE module) {
    if (module && !((ULONG_PTR)module & 3) && !g_explorerPatcherInitialized) {
        if (IsExplorerPatcherModule(module)) {
            HookExplorerPatcherSymbols(module);
        }
    }
}

HMODULE GetTaskbarViewModuleHandle() {
    HMODULE module = GetModuleHandle(L"Taskbar.View.dll");
    if (!module) {
        module = GetModuleHandle(L"ExplorerExtensions.dll");
    }

    return module;
}

bool HookTaskbarViewDllSymbols(HMODULE module) {
    // Taskbar.View.dll, ExplorerExtensions.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(public: virtual int __cdecl winrt::impl::produce<struct winrt::Taskbar::implementation::TaskbarFrame,struct winrt::Windows::UI::Xaml::Controls::IControlOverrides>::OnPointerPressed(void *))"},
            &TaskbarFrame_OnPointerPressed_Original,
            TaskbarFrame_OnPointerPressed_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool ShouldHookTaskbarViewDllSymbols() {
    return g_winVersion >= WinVersion::Win11 &&
           g_settings.clickToSwitchMonitor != ClickToSwitchMonitor::disabled;
}

void HandleLoadedModuleIfTaskbarView(HMODULE module, LPCWSTR lpLibFileName) {
    if (ShouldHookTaskbarViewDllSymbols() && !g_taskbarViewDllLoaded &&
        GetTaskbarViewModuleHandle() == module &&
        !g_taskbarViewDllLoaded.exchange(true)) {
        Wh_Log(L"Loaded %s", lpLibFileName);

        if (HookTaskbarViewDllSymbols(module)) {
            Wh_ApplyHookOperations();
        }
    }
}

using LoadLibraryExW_t = decltype(&LoadLibraryExW);
LoadLibraryExW_t LoadLibraryExW_Original;
HMODULE WINAPI LoadLibraryExW_Hook(LPCWSTR lpLibFileName,
                                   HANDLE hFile,
                                   DWORD dwFlags) {
    HMODULE module = LoadLibraryExW_Original(lpLibFileName, hFile, dwFlags);
    if (module) {
        HandleLoadedModuleIfExplorerPatcher(module);
        HandleLoadedModuleIfTaskbarView(module, lpLibFileName);
    }

    return module;
}

bool HookTaskbarSymbols() {
    HMODULE module;
    if (g_winVersion <= WinVersion::Win10) {
        module = GetModuleHandle(nullptr);
    } else {
        module = LoadLibraryEx(L"taskbar.dll", nullptr,
                               LOAD_LIBRARY_SEARCH_SYSTEM32);
        if (!module) {
            Wh_Log(L"Couldn't load taskbar.dll");
            return false;
        }
    }

    // Taskbar.dll, explorer.exe
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {
                // Windows 11.
                LR"(public: long __cdecl TrayUI::_SetStuckMonitor(struct HMONITOR__ *))",

                // Windows 10.
                LR"(public: void __cdecl TrayUI::_SetStuckMonitor(struct HMONITOR__ *))",
            },
            &TrayUI__SetStuckMonitor_Original,
            TrayUI__SetStuckMonitor_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

bool HookHardwareConfirmatorSymbols() {
    HMODULE module = LoadLibraryEx(L"Windows.Internal.HardwareConfirmator.dll",
                                   nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Couldn't load Windows.Internal.HardwareConfirmator.dll");
        return false;
    }

    // Windows.Internal.HardwareConfirmator.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: struct winrt::Windows::Foundation::Rect __cdecl winrt::Windows::Internal::HardwareConfirmator::implementation::HardwareConfirmatorHost::GetPositionRect(struct winrt::Windows::Foundation::Rect const &))"},
            &HardwareConfirmatorHost_GetPositionRect_Original,
            HardwareConfirmatorHost_GetPositionRect_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

void ApplySettings() {
    HWND hTaskbarWnd = FindCurrentProcessTaskbarWnd();
    if (!hTaskbarWnd) {
        return;
    }

    // Trigger CTray::_HandleDisplayChange.
    SendMessage(hTaskbarWnd, 0x5B8, 0, 0);

    BroadcastShellHookDisplayChange();
}

void LoadSettings() {
    g_settings.monitor = Wh_GetIntSetting(L"monitor");
    g_settings.monitorInterfaceName =
        WindhawkUtils::StringSetting::make(L"monitorInterfaceName");

    PCWSTR clickToSwitchMonitor = Wh_GetStringSetting(L"clickToSwitchMonitor");
    g_settings.clickToSwitchMonitor = ClickToSwitchMonitor::disabled;
    if (wcscmp(clickToSwitchMonitor, L"doubleClick") == 0) {
        g_settings.clickToSwitchMonitor = ClickToSwitchMonitor::doubleClick;
    } else if (wcscmp(clickToSwitchMonitor, L"middleClick") == 0) {
        g_settings.clickToSwitchMonitor = ClickToSwitchMonitor::middleClick;
    }
    Wh_FreeStringSetting(clickToSwitchMonitor);

    g_settings.moveAdditionalElements =
        Wh_GetIntSetting(L"moveAdditionalElements");
    g_settings.oldTaskbarOnWin11 = Wh_GetIntSetting(L"oldTaskbarOnWin11");
}

BOOL Wh_ModInit() {
    Wh_Log(L">");

    LoadSettings();

    g_target = Target::Explorer;

    WCHAR moduleFilePath[MAX_PATH];
    switch (
        GetModuleFileName(nullptr, moduleFilePath, ARRAYSIZE(moduleFilePath))) {
        case 0:
        case ARRAYSIZE(moduleFilePath):
            Wh_Log(L"GetModuleFileName failed");
            return FALSE;

        default:
            if (PCWSTR moduleFileName = wcsrchr(moduleFilePath, L'\\')) {
                moduleFileName++;
                if (_wcsicmp(moduleFileName, L"ShellHost.exe") == 0) {
                    g_target = Target::ShellHost;
                } else if (_wcsicmp(moduleFileName,
                                    L"StartMenuExperienceHost.exe") == 0) {
                    g_target = Target::StartMenuExperienceHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
                return FALSE;
            }
            break;
    }

    if (g_target == Target::Explorer) {
        g_winVersion = GetExplorerVersion();
        if (g_winVersion == WinVersion::Unsupported) {
            Wh_Log(L"Unsupported Windows version");
            return FALSE;
        }

        if (g_settings.oldTaskbarOnWin11) {
            bool hasWin10Taskbar = g_winVersion < WinVersion::Win11_24H2;

            if (g_winVersion >= WinVersion::Win11) {
                g_winVersion = WinVersion::Win10;
            }

            if (hasWin10Taskbar && !HookTaskbarSymbols()) {
                return FALSE;
            }
        } else {
            if (ShouldHookTaskbarViewDllSymbols()) {
                if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
                    g_taskbarViewDllLoaded = true;
                    HookTaskbarViewDllSymbols(taskbarViewModule);
                }
            }

            if (!HookTaskbarSymbols()) {
                return FALSE;
            }
        }

        if (!HandleLoadedExplorerPatcher()) {
            Wh_Log(L"HandleLoadedExplorerPatcher failed");
            return FALSE;
        }

        HMODULE kernelBaseModule = GetModuleHandle(L"kernelbase.dll");
        auto pKernelBaseLoadLibraryExW =
            (decltype(&LoadLibraryExW))GetProcAddress(kernelBaseModule,
                                                      "LoadLibraryExW");
        WindhawkUtils::SetFunctionHook(pKernelBaseLoadLibraryExW,
                                       LoadLibraryExW_Hook,
                                       &LoadLibraryExW_Original);

        HookHardwareConfirmatorSymbols();
    }

    if (g_target == Target::StartMenuExperienceHost) {
        WindhawkUtils::SetFunctionHook(MonitorFromWindow,
                                       MonitorFromWindow_Hook,
                                       &MonitorFromWindow_Original);
    } else {
        WindhawkUtils::SetFunctionHook(MonitorFromPoint, MonitorFromPoint_Hook,
                                       &MonitorFromPoint_Original);

        WindhawkUtils::SetFunctionHook(MonitorFromRect, MonitorFromRect_Hook,
                                       &MonitorFromRect_Original);

        WindhawkUtils::SetFunctionHook(EnumDisplayDevicesW,
                                       EnumDisplayDevicesW_Hook,
                                       &EnumDisplayDevicesW_Original);
    }

    g_initialized = true;

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_target == Target::Explorer) {
        if (ShouldHookTaskbarViewDllSymbols() && !g_taskbarViewDllLoaded) {
            if (HMODULE taskbarViewModule = GetTaskbarViewModuleHandle()) {
                if (!g_taskbarViewDllLoaded.exchange(true)) {
                    Wh_Log(L"Got Taskbar.View.dll");

                    if (HookTaskbarViewDllSymbols(taskbarViewModule)) {
                        Wh_ApplyHookOperations();
                    }
                }
            }
        }

        // Try again in case there's a race between the previous attempt and the
        // LoadLibraryExW hook.
        if (!g_explorerPatcherInitialized) {
            HandleLoadedExplorerPatcher();
        }

        ApplySettings();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_target == Target::Explorer) {
        ApplySettings();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

BOOL Wh_ModSettingsChanged(BOOL* bReload) {
    Wh_Log(L">");

    bool prevOldTaskbarOnWin11 = g_settings.oldTaskbarOnWin11;
    bool prevClickToSwitchMonitorEnabled =
        g_settings.clickToSwitchMonitor != ClickToSwitchMonitor::disabled;

    LoadSettings();

    bool clickToSwitchMonitorEnabled =
        g_settings.clickToSwitchMonitor != ClickToSwitchMonitor::disabled;

    if (g_target == Target::Explorer) {
        if (!clickToSwitchMonitorEnabled) {
            g_overrideMonitor = nullptr;
        }

        *bReload =
            g_settings.oldTaskbarOnWin11 != prevOldTaskbarOnWin11 ||
            clickToSwitchMonitorEnabled != prevClickToSwitchMonitorEnabled;
        if (!*bReload) {
            ApplySettings();
        }
    }

    return TRUE;
}
