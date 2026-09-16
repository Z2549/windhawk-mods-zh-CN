// ==WindhawkMod==
// @id              notifications-placement
// @name            Customize Windows notifications placement
// @name:zh-CN      自定义 Windows 通知位置
// @description     Move notifications to another monitor or another corner of the screen
// @description:zh-CN 可将系统通知移动到其他显示器，或移动到屏幕的其他角落，方便集中查看
// @version         1.2.4
// @author          m417z
// @github          https://github.com/m417z
// @twitter         https://twitter.com/m417z
// @homepage        https://m417z.com/
// @include         explorer.exe
// @include         ShellExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -lole32 -loleaut32 -lruntimeobject -lshcore
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
# 自定义 Windows 通知位置

把通知移到另一台显示器或屏幕的另一个角落。

仅支持 Windows 10 64 位与 Windows 11。

![截图](https://i.imgur.com/4PxMvLg.png)

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
5. 触发一条通知出现。
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
- monitor: 1
  $name: Monitor
  $name:zh-CN: 显示器
  $description: >-
    The monitor number that notifications will appear on, set to zero to use the
    monitor where the mouse cursor is located
  $description:zh-CN: 通知将要显示的显示器编号，设为 0 则使用鼠标光标所在的显示器
- monitorInterfaceName: ""
  $name: Monitor interface name
  $name:zh-CN: 显示器接口名称
  $description: >-
    If not empty, the given monitor interface name (can also be an interface
    name substring) will be used instead of the monitor number. Can be useful if
    the monitor numbers change often. To see all available interface names, set
    any interface name, enable mod logs, trigger a notification and look for
    "Found display device" messages.
  $description:zh-CN: 若非空，将使用给定的显示器接口名称（也可以是接口名称的一部分）来代替显示器编号。如果显示器编号经常变化，这个选项会很有用。要查看所有可用的接口名称，请设置任意接口名称，启用 mod 日志，触发一次通知，然后查找“Found display device”消息。
- horizontalPlacement: right
  $name: Horizontal placement on the screen
  $name:zh-CN: 屏幕上水平方向的位置
  $options:
  - right: Right
  - left: Left
  - center: Center
  $options:zh-CN:
  - right: 靠右
  - left: 靠左
  - center: 居中
- horizontalDistanceFromScreenEdge: 0
  $name: Distance from the right/left side of the screen
  $name:zh-CN: 距屏幕右/左侧的距离
- verticalPlacement: bottom
  $name: Vertical placement on the screen
  $name:zh-CN: 屏幕上垂直方向的位置
  $options:
  - bottom: Bottom
  - top: Top
  - center: Center
  $options:zh-CN:
  - bottom: 靠下
  - top: 靠上
  - center: 居中
- verticalDistanceFromScreenEdge: 0
  $name: Distance from the bottom/top side of the screen
  $name:zh-CN: 距屏幕下/上侧的距离
- animationDirection: automatic
  $name: Notification appearance animation direction
  $name:zh-CN: 通知出现动画的方向
  $options:
  - automatic: Automatic
  - fromLeft: From left
  - fromRight: From right
  - fromTop: From top
  - fromBottom: From bottom
  $options:zh-CN:
  - automatic: 自动
  - fromLeft: 从左侧
  - fromRight: 从右侧
  - fromTop: 从顶部
  - fromBottom: 从底部
*/
// ==/WindhawkModSettings==

#include <windhawk_utils.h>

#undef GetCurrentTime

#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/base.h>

#include <atomic>
#include <functional>
#include <string>
#include <unordered_set>
#include <vector>

using namespace winrt::Windows::UI::Xaml;

std::atomic<bool> g_unloading;

enum class HorizontalPlacement {
    right,
    left,
    center,
};

enum class VerticalPlacement {
    bottom,
    top,
    center,
};

enum class AnimationDirection {
    automatic,
    fromLeft,
    fromRight,
    fromTop,
    fromBottom,
};

struct {
    int monitor;
    WindhawkUtils::StringSetting monitorInterfaceName;
    HorizontalPlacement horizontalPlacement;
    int horizontalDistanceFromScreenEdge;
    VerticalPlacement verticalPlacement;
    int verticalDistanceFromScreenEdge;
    AnimationDirection animationDirection;
} g_settings;

enum class Target {
    Explorer,
    ShellExperienceHost,
};

Target g_target;

bool g_inCToastCenterExperienceManager_PositionView;

bool g_customAnimationDirectionApplied;

WINUSERAPI UINT WINAPI GetDpiForWindow(HWND hwnd);
typedef enum MONITOR_DPI_TYPE {
    MDT_EFFECTIVE_DPI = 0,
    MDT_ANGULAR_DPI = 1,
    MDT_RAW_DPI = 2,
    MDT_DEFAULT = MDT_EFFECTIVE_DPI
} MONITOR_DPI_TYPE;
STDAPI GetDpiForMonitor(HMONITOR hmonitor,
                        MONITOR_DPI_TYPE dpiType,
                        UINT* dpiX,
                        UINT* dpiY);

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

bool GetMonitorWorkArea(HMONITOR monitor, RECT* rc) {
    MONITORINFO monitorInfo{
        .cbSize = sizeof(MONITORINFO),
    };
    return GetMonitorInfo(monitor, &monitorInfo) &&
           CopyRect(rc, &monitorInfo.rcWork);
}

std::wstring GetProcessFileName(DWORD dwProcessId) {
    HANDLE hProcess =
        OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
    if (!hProcess) {
        return std::wstring{};
    }

    WCHAR processPath[MAX_PATH];

    DWORD dwSize = ARRAYSIZE(processPath);
    if (!QueryFullProcessImageName(hProcess, 0, processPath, &dwSize)) {
        CloseHandle(hProcess);
        return std::wstring{};
    }

    CloseHandle(hProcess);

    PCWSTR processFileName = wcsrchr(processPath, L'\\');
    if (!processFileName) {
        return std::wstring{};
    }

    processFileName++;
    return processFileName;
}

FrameworkElement EnumChildElements(
    FrameworkElement element,
    std::function<bool(FrameworkElement)> enumCallback) {
    int childrenCount = Media::VisualTreeHelper::GetChildrenCount(element);

    for (int i = 0; i < childrenCount; i++) {
        auto child = Media::VisualTreeHelper::GetChild(element, i)
                         .try_as<FrameworkElement>();
        if (!child) {
            Wh_Log(L"Failed to get child %d of %d", i + 1, childrenCount);
            continue;
        }

        if (enumCallback(child)) {
            return child;
        }
    }

    return nullptr;
}

FrameworkElement FindChildByName(FrameworkElement element, PCWSTR name) {
    return EnumChildElements(element, [name](FrameworkElement child) {
        return child.Name() == name;
    });
}

FrameworkElement FindChildByClassName(FrameworkElement element,
                                      PCWSTR className) {
    return EnumChildElements(element, [className](FrameworkElement child) {
        return winrt::get_class_name(child) == className;
    });
}

bool IsTargetCoreWindow(HWND hWnd) {
    DWORD processId = 0;
    if (!hWnd || !GetWindowThreadProcessId(hWnd, &processId)) {
        return false;
    }

    if (_wcsicmp(GetProcessFileName(processId).c_str(),
                 L"ShellExperienceHost.exe") != 0) {
        return false;
    }

    WCHAR szClassName[32];
    if (GetClassName(hWnd, szClassName, ARRAYSIZE(szClassName)) == 0 ||
        _wcsicmp(szClassName, L"Windows.UI.Core.CoreWindow") != 0) {
        return false;
    }

    // The window title is locale-dependent, and unfortunately I didn't find a
    // simpler way to identify the target window.
    // String source: Windows.UI.ShellCommon.<locale>.pri
    // String resource: \ActionCenter\AC_ToastCenter_Title
    // The strings were collected from here:
    // https://github.com/m417z/windows-language-files

    // clang-format off
    static const std::unordered_set<std::wstring> newNotificationStrings = {
        L"Nuwe kennisgewing", // AF-ZA
        L"አዲስ ማሳወቂያ", // AM-ET
        L"নতুন জাননী", // AS-IN
        L"Yeni bildiriş", // AZ-LATN-AZ
        L"Новае апавяшчэнне", // BE-BY
        L"নতুন বিজ্ঞপ্তি", // BN-IN
        L"Novo obavještenje", // BS-LATN-BA
        L"Notificació nova", // CA-ES-VALENCIA
        L"ᎢᏤᎢ ᎧᏃᎮᏓ", // CHR-CHER-US
        L"Hysbysiad newydd", // CY-GB
        L"اعلان جدید", // FA-IR
        L"Bagong notification", // FIL-PH
        L"Fógra nua", // GA-IE
        L"Brath ùr", // GD-GB
        L"નવી સૂચના", // GU-IN
        L"नई अधिसूचना", // HI-IN
        L"Նոր ծանուցում", // HY-AM
        L"Ný tilkynning", // IS-IS
        L"ახალი შეტყობინება", // KA-GE
        L"Жаңа хабарландыру", // KK-KZ
        L"ការ\u200bជូន\u200bដំណឹង\u200bថ្មី", // KM-KH
        L"ಹೊಸ ಪ್ರಕಟಣೆ", // KN-IN
        L"नवी अधिसुचोवणी", // KOK-IN
        L"Nei Notifikatioun", // LB-LU
        L"ການແຈ້ງເຕືອນໃໝ່", // LO-LA
        L"Whakamōhiotanga hōu", // MI-NZ
        L"Ново известување", // MK-MK
        L"പുതിയ അറിയിപ്പ്", // ML-IN
        L"नवीन सूचना", // MR-IN
        L"Pemberitahuan baharu", // MS-MY
        L"Notifika ġdida", // MT-MT
        L"नयाँ सूचना", // NE-NP
        L"Nytt varsel", // NN-NO
        L"ନୂତନ ବିଜ୍ଞପ୍ତି", // OR-IN
        L"ਨਵੀਂ ਸੂਚਨਾ", // PA-IN
        L"Musuq willana", // QUZ-PE
        L"Njoftim i ri", // SQ-AL
        L"Ново обавјештење", // SR-CYRL-BA
        L"Ново обавештење", // SR-CYRL-RS
        L"புதிய அறிவிப்பு", // TA-IN
        L"కొత్త నోటిఫికేషన్", // TE-IN
        L"Яңа белдерү", // TT-RU
        L"يېڭى ئۇقتۇرۇش", // UG-CN
        L"نئی اطلاع", // UR-PK
        L"Yangi xabarnoma", // UZ-LATN-UZ
        L"\u200f\u200fإعلام جديد", // AR-SA
        L"Ново известие", // BG-BG
        L"Notificació nova", // CA-ES
        L"Nové oznámení", // CS-CZ
        L"Ny meddelelse", // DA-DK
        L"Neue Benachrichtigung", // DE-DE
        L"Νέα ειδοποίηση", // EL-GR
        L"New notification", // EN-GB
        L"New notification", // EN-US
        L"Notificación nueva", // ES-ES
        L"Nueva notificación", // ES-MX
        L"Uus teatis", // ET-EE
        L"Jakinarazpen berria", // EU-ES
        L"Uusi ilmoitus", // FI-FI
        L"Nouvelle notification", // FR-CA
        L"Nouvelle notification", // FR-FR
        L"Nova notificación", // GL-ES
        L"הודעה חדשה", // HE-IL
        L"Nova obavijesti", // HR-HR
        L"Új értesítés", // HU-HU
        L"Pemberitahuan baru", // ID-ID
        L"Nuova notifica", // IT-IT
        L"新しい通知", // JA-JP
        L"새 알림", // KO-KR
        L"Naujas pranešimas", // LT-LT
        L"Jauns paziņojums", // LV-LV
        L"Ny varsling", // NB-NO
        L"Nieuwe melding", // NL-NL
        L"Nowe powiadomienie", // PL-PL
        L"Nova notificação", // PT-BR
        L"Nova notificação", // PT-PT
        L"Notificare nouă", // RO-RO
        L"Новое уведомление", // RU-RU
        L"Nové oznámenie", // SK-SK
        L"Novo obvestilo", // SL-SI
        L"Novo obaveštenje", // SR-LATN-RS
        L"Nytt meddelande", // SV-SE
        L"การแจ้งให้ทราบใหม่", // TH-TH
        L"Yeni bildirim", // TR-TR
        L"Нове сповіщення", // UK-UA
        L"Thông báo mới", // VI-VN
        L"新通知", // ZH-CN
        L"新通知", // ZH-TW
    };
    // clang-format on

    WCHAR szWindowText[256]{};
    if (GetWindowText(hWnd, szWindowText, ARRAYSIZE(szWindowText)) == 0 ||
        !newNotificationStrings.contains(szWindowText)) {
        Wh_Log(L"Not targeting CoreWindow, window text: %s", szWindowText);
        return false;
    }

    return true;
}

std::vector<HWND> GetCoreWindows() {
    struct ENUM_WINDOWS_PARAM {
        std::vector<HWND>* hWnds;
    };

    std::vector<HWND> hWnds;
    ENUM_WINDOWS_PARAM param = {&hWnds};
    EnumWindows(
        [](HWND hWnd, LPARAM lParam) -> BOOL {
            ENUM_WINDOWS_PARAM& param = *(ENUM_WINDOWS_PARAM*)lParam;

            if (IsTargetCoreWindow(hWnd)) {
                param.hWnds->push_back(hWnd);
            }

            return TRUE;
        },
        (LPARAM)&param);

    return hWnds;
}

void AdjustCoreWindowPos(int* x, int* y, int* cx, int* cy) {
    Wh_Log(L"Before: %dx%d %dx%d", *x, *y, *cx, *cy);

    RECT rc{
        .left = *x,
        .top = *y,
        .right = *x + *cx,
        .bottom = *y + *cy,
    };
    HMONITOR srcMonitor = MonitorFromRect(&rc, MONITOR_DEFAULTTONEAREST);

    UINT srcMonitorDpiX = 96;
    UINT srcMonitorDpiY = 96;
    GetDpiForMonitor(srcMonitor, MDT_DEFAULT, &srcMonitorDpiX, &srcMonitorDpiY);

    RECT srcMonitorWorkArea;
    if (!GetMonitorWorkArea(srcMonitor, &srcMonitorWorkArea)) {
        return;
    }

    HMONITOR destMonitor = nullptr;

    if (!g_unloading) {
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
    }

    if (!destMonitor) {
        HMONITOR primaryMonitor =
            MonitorFromPoint({0, 0}, MONITOR_DEFAULTTONEAREST);

        destMonitor = primaryMonitor;
    }

    RECT destMonitorWorkArea;
    int horizontalDistanceFromScreenEdge = 0;
    int verticalDistanceFromScreenEdge = 0;

    Wh_Log(L"Monitor %p->%p", srcMonitor, destMonitor);

    if (destMonitor != srcMonitor) {
        UINT destMonitorDpiX = 96;
        UINT destMonitorDpiY = 96;
        GetDpiForMonitor(destMonitor, MDT_DEFAULT, &destMonitorDpiX,
                         &destMonitorDpiY);

        if (!GetMonitorWorkArea(destMonitor, &destMonitorWorkArea)) {
            return;
        }

        *cx = MulDiv(*cx, destMonitorDpiX, srcMonitorDpiX);
        if (*y + *cy == srcMonitorWorkArea.bottom) {
            *y = destMonitorWorkArea.bottom -
                 MulDiv(*cy, destMonitorDpiY, srcMonitorDpiY);
            *cy = MulDiv(*cy, destMonitorDpiY, srcMonitorDpiY);
        } else {
            *cy = MulDiv(*cy, destMonitorDpiY, srcMonitorDpiY);
        }

        if (*y == destMonitorWorkArea.top &&
            *y + *cy > destMonitorWorkArea.bottom) {
            *cy = destMonitorWorkArea.bottom - destMonitorWorkArea.top;
        }

        if (!g_unloading) {
            horizontalDistanceFromScreenEdge =
                MulDiv(g_settings.horizontalDistanceFromScreenEdge,
                       destMonitorDpiX, 96);
            verticalDistanceFromScreenEdge = MulDiv(
                g_settings.verticalDistanceFromScreenEdge, destMonitorDpiY, 96);
        }
    } else {
        CopyRect(&destMonitorWorkArea, &srcMonitorWorkArea);

        if (!g_unloading) {
            horizontalDistanceFromScreenEdge =
                MulDiv(g_settings.horizontalDistanceFromScreenEdge,
                       srcMonitorDpiX, 96);
            verticalDistanceFromScreenEdge = MulDiv(
                g_settings.verticalDistanceFromScreenEdge, srcMonitorDpiY, 96);
        }
    }

    switch (g_unloading ? HorizontalPlacement::right
                        : g_settings.horizontalPlacement) {
        case HorizontalPlacement::right:
            *x = destMonitorWorkArea.right - *cx -
                 horizontalDistanceFromScreenEdge;
            break;

        case HorizontalPlacement::left:
            *x = destMonitorWorkArea.left + horizontalDistanceFromScreenEdge;
            break;

        case HorizontalPlacement::center:
            *x = destMonitorWorkArea.left +
                 (destMonitorWorkArea.right - destMonitorWorkArea.left - *cx) /
                     2 +
                 horizontalDistanceFromScreenEdge;
            break;
    }

    switch (g_unloading ? VerticalPlacement::bottom
                        : g_settings.verticalPlacement) {
        case VerticalPlacement::bottom:
            *y = destMonitorWorkArea.bottom - *cy -
                 verticalDistanceFromScreenEdge;
            break;

        case VerticalPlacement::top:
            *y = destMonitorWorkArea.top + verticalDistanceFromScreenEdge;
            break;

        case VerticalPlacement::center:
            *y = destMonitorWorkArea.top +
                 (destMonitorWorkArea.bottom - destMonitorWorkArea.top - *cy) /
                     2 +
                 verticalDistanceFromScreenEdge;
            break;
    }

    Wh_Log(L"After: %dx%d %dx%d", *x, *y, *cx, *cy);
}

using CToastCenterExperienceManager_PositionView_t =
    HRESULT(WINAPI*)(void* pThis);
CToastCenterExperienceManager_PositionView_t
    CToastCenterExperienceManager_PositionView_Original;
HRESULT WINAPI CToastCenterExperienceManager_PositionView_Hook(void* pThis) {
    Wh_Log(L">");

    g_inCToastCenterExperienceManager_PositionView = true;
    HRESULT ret = CToastCenterExperienceManager_PositionView_Original(pThis);
    g_inCToastCenterExperienceManager_PositionView = false;

    return ret;
}

using MonitorFromPoint_t = decltype(&MonitorFromPoint);
MonitorFromPoint_t MonitorFromPoint_Original;
HMONITOR WINAPI MonitorFromPoint_Hook(POINT pt, DWORD dwFlags) {
    Wh_Log(L">");

    if (g_inCToastCenterExperienceManager_PositionView && !g_unloading &&
        pt.x == 0 && pt.y == 0) {
        HMONITOR monitor = nullptr;

        if (*g_settings.monitorInterfaceName.get()) {
            monitor = GetMonitorByInterfaceNameSubstr(
                g_settings.monitorInterfaceName.get());
        } else if (g_settings.monitor == 0) {
            POINT cursorPt;
            GetCursorPos(&cursorPt);
            monitor =
                MonitorFromPoint_Original(cursorPt, MONITOR_DEFAULTTONEAREST);
        } else if (g_settings.monitor >= 1) {
            monitor = GetMonitorById(g_settings.monitor - 1);
        }

        if (monitor) {
            return monitor;
        }
    }

    return MonitorFromPoint_Original(pt, dwFlags);
}

void UpdateAnimationDirectionStyle() {
    int angle = 0;

    switch (g_unloading ? AnimationDirection::fromRight
                        : g_settings.animationDirection) {
        case AnimationDirection::automatic:
            if (g_settings.horizontalPlacement == HorizontalPlacement::center) {
                if (g_settings.verticalPlacement == VerticalPlacement::bottom) {
                    angle = 90;
                } else {
                    angle = -90;
                }
            } else if (g_settings.horizontalPlacement ==
                       HorizontalPlacement::left) {
                angle = 180;
            }
            break;

        case AnimationDirection::fromLeft:
            angle = 180;
            break;

        case AnimationDirection::fromRight:
            break;

        case AnimationDirection::fromTop:
            angle = -90;
            break;

        case AnimationDirection::fromBottom:
            angle = 90;
            break;
    }

    if (!g_customAnimationDirectionApplied && !angle) {
        return;
    }

    auto window = Window::Current();
    if (!window) {
        Wh_Log(L"Failed to get current window");
        return;
    }

    FrameworkElement windowContent = window.Content().as<FrameworkElement>();
    if (!windowContent) {
        Wh_Log(L"Failed to get window content");
        return;
    }

    FrameworkElement launcherFrame = nullptr;

    FrameworkElement child = windowContent;
    if ((child = FindChildByClassName(
             child, L"Windows.UI.Xaml.Controls.ContentPresenter")) &&
        (child =
             FindChildByClassName(child, L"ActionCenter.ToastCenterPage")) &&
        (child = FindChildByName(child, L"ToastCenterMainGrid")) &&
        (child = FindChildByName(child, L"ToastCenterView")) &&
        (child = FindChildByName(child, L"ToastCenterScrollViewer")) &&
        (child = FindChildByName(child, L"Root")) &&
        (child =
             FindChildByClassName(child, L"Windows.UI.Xaml.Controls.Grid")) &&
        (child = FindChildByName(child, L"ScrollContentPresenter")) &&
        (child = FindChildByName(child, L"ToastCenterGrid"))) {
        launcherFrame = child;
    }

    if (!launcherFrame) {
        Wh_Log(L"Failed to find launcher frame");
        return;
    }

    auto origin = winrt::Windows::Foundation::Point{0.5, 0.5};

    bool foundAnyRootGridContent = false;
    EnumChildElements(launcherFrame, [&](FrameworkElement toastView) {
        auto name = toastView.Name();
        if (name.empty()) {
            return false;  // continue enumeration
        }

        if (!name.starts_with(L"FlexibleNormalToastView") &&
            !name.starts_with(L"FlexiblePriorityToastView")) {
            return false;  // continue enumeration
        }

        FrameworkElement mainGrid = FindChildByName(toastView, L"MainGrid");
        if (!mainGrid) {
            return false;  // continue enumeration
        }

        if (FrameworkElement revealGrid =
                FindChildByName(mainGrid, L"RevealGrid")) {
            Wh_Log(L"Applying transform to toast view %s", name.c_str());

            Media::RotateTransform transform;
            transform.Angle(-angle);
            revealGrid.RenderTransform(transform);
            revealGrid.RenderTransformOrigin(origin);

            foundAnyRootGridContent = true;
        }

        // Older Windows 11 versions have both RevealGrid and RevealGrid2
        // (before ~Jul 2026). Newer builds only have RevealGrid.
        if (FrameworkElement revealGrid2 =
                FindChildByName(mainGrid, L"RevealGrid2")) {
            Wh_Log(L"Applying transform to toast view %s", name.c_str());

            Media::RotateTransform transform;
            transform.Angle(-angle);
            revealGrid2.RenderTransform(transform);
            revealGrid2.RenderTransformOrigin(origin);

            foundAnyRootGridContent = true;
        }

        return false;  // continue enumeration to find all matching children
    });

    if (!foundAnyRootGridContent) {
        Wh_Log(L"Failed to find root grid content");
        return;
    }

    Media::RotateTransform transform;
    transform.Angle(angle);
    launcherFrame.RenderTransform(transform);
    launcherFrame.RenderTransformOrigin(origin);

    g_customAnimationDirectionApplied = (angle != 0);
}

using SetWindowPos_t = decltype(&SetWindowPos);
SetWindowPos_t SetWindowPos_Original;
BOOL WINAPI SetWindowPos_Hook(HWND hWnd,
                              HWND hWndInsertAfter,
                              int X,
                              int Y,
                              int cx,
                              int cy,
                              UINT uFlags) {
    auto original = [&]() {
        return SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy,
                                     uFlags);
    };

    if (!IsTargetCoreWindow(hWnd)) {
        return original();
    }

    Wh_Log(L"%08X %08X", (DWORD)(ULONG_PTR)hWnd, uFlags);

    RECT rc{};
    GetWindowRect(hWnd, &rc);

    // Skip if no size or empty size.
    if ((uFlags & SWP_NOSIZE) || cx == 0 || cy == 0) {
        Wh_Log(L"Skipping");
        uFlags |= SWP_NOMOVE | SWP_NOSIZE;
        return original();
    }

    if (uFlags & SWP_NOMOVE) {
        uFlags &= ~SWP_NOMOVE;
        X = rc.left;
        Y = rc.top;
    }

    if (uFlags & SWP_NOSIZE) {
        uFlags &= ~SWP_NOSIZE;
        cx = rc.right - rc.left;
        cy = rc.bottom - rc.top;
    }

    AdjustCoreWindowPos(&X, &Y, &cx, &cy);

    BOOL ret =
        SetWindowPos_Original(hWnd, hWndInsertAfter, X, Y, cx, cy, uFlags);

    if (g_target == Target::ShellExperienceHost &&
        GetWindowThreadProcessId(hWnd, nullptr) == GetCurrentThreadId()) {
        UpdateAnimationDirectionStyle();
    }

    return ret;
}

namespace ShellExperienceHost {

void ApplySettings() {
    for (HWND hCoreWnd : GetCoreWindows()) {
        Wh_Log(L"Adjusting core window %08X", (DWORD)(ULONG_PTR)hCoreWnd);

        RECT rc;
        if (!GetWindowRect(hCoreWnd, &rc)) {
            continue;
        }

        int x = rc.left;
        int y = rc.top;
        int cx = rc.right - rc.left;
        int cy = rc.bottom - rc.top;

        AdjustCoreWindowPos(&x, &y, &cx, &cy);

        SetWindowPos_Original(hCoreWnd, nullptr, x, y, cx, cy,
                              SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

}  // namespace ShellExperienceHost

bool HookTwinuiPcshellSymbols() {
    HMODULE module = LoadLibraryEx(L"twinui.pcshell.dll", nullptr,
                                   LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (!module) {
        Wh_Log(L"Loading twinui.pcshell.dll failed");
        return false;
    }

    // twinui.pcshell.dll
    WindhawkUtils::SYMBOL_HOOK symbolHooks[] = {
        {
            {LR"(private: long __cdecl CToastCenterExperienceManager::PositionView(void))"},
            &CToastCenterExperienceManager_PositionView_Original,
            CToastCenterExperienceManager_PositionView_Hook,
        },
    };

    if (!HookSymbols(module, symbolHooks, ARRAYSIZE(symbolHooks))) {
        Wh_Log(L"HookSymbols failed");
        return false;
    }

    return true;
}

void LoadSettings() {
    g_settings.monitor = Wh_GetIntSetting(L"monitor");
    g_settings.monitorInterfaceName =
        WindhawkUtils::StringSetting::make(L"monitorInterfaceName");

    PCWSTR horizontalPlacement = Wh_GetStringSetting(L"horizontalPlacement");
    g_settings.horizontalPlacement = HorizontalPlacement::right;
    if (wcscmp(horizontalPlacement, L"left") == 0) {
        g_settings.horizontalPlacement = HorizontalPlacement::left;
    } else if (wcscmp(horizontalPlacement, L"center") == 0) {
        g_settings.horizontalPlacement = HorizontalPlacement::center;
    }
    Wh_FreeStringSetting(horizontalPlacement);

    g_settings.horizontalDistanceFromScreenEdge =
        Wh_GetIntSetting(L"horizontalDistanceFromScreenEdge");

    PCWSTR verticalPlacement = Wh_GetStringSetting(L"verticalPlacement");
    g_settings.verticalPlacement = VerticalPlacement::bottom;
    if (wcscmp(verticalPlacement, L"top") == 0) {
        g_settings.verticalPlacement = VerticalPlacement::top;
    } else if (wcscmp(verticalPlacement, L"center") == 0) {
        g_settings.verticalPlacement = VerticalPlacement::center;
    }
    Wh_FreeStringSetting(verticalPlacement);

    g_settings.verticalDistanceFromScreenEdge =
        Wh_GetIntSetting(L"verticalDistanceFromScreenEdge");

    PCWSTR animationDirection = Wh_GetStringSetting(L"animationDirection");
    g_settings.animationDirection = AnimationDirection::automatic;
    if (wcscmp(animationDirection, L"fromLeft") == 0) {
        g_settings.animationDirection = AnimationDirection::fromLeft;
    } else if (wcscmp(animationDirection, L"fromRight") == 0) {
        g_settings.animationDirection = AnimationDirection::fromRight;
    } else if (wcscmp(animationDirection, L"fromTop") == 0) {
        g_settings.animationDirection = AnimationDirection::fromTop;
    } else if (wcscmp(animationDirection, L"fromBottom") == 0) {
        g_settings.animationDirection = AnimationDirection::fromBottom;
    }
    Wh_FreeStringSetting(animationDirection);
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
                if (_wcsicmp(moduleFileName, L"ShellExperienceHost.exe") == 0) {
                    g_target = Target::ShellExperienceHost;
                }
            } else {
                Wh_Log(L"GetModuleFileName returned an unsupported path");
                return FALSE;
            }
            break;
    }

    WindhawkUtils::SetFunctionHook(SetWindowPos, SetWindowPos_Hook,
                                   &SetWindowPos_Original);

    if (g_target == Target::Explorer) {
        if (!HookTwinuiPcshellSymbols()) {
            return FALSE;
        }

        WindhawkUtils::SetFunctionHook(MonitorFromPoint, MonitorFromPoint_Hook,
                                       &MonitorFromPoint_Original);
    }

    return TRUE;
}

void Wh_ModAfterInit() {
    Wh_Log(L">");

    if (g_target == Target::ShellExperienceHost) {
        ShellExperienceHost::ApplySettings();
    }
}

void Wh_ModBeforeUninit() {
    Wh_Log(L">");

    g_unloading = true;

    if (g_target == Target::ShellExperienceHost) {
        ShellExperienceHost::ApplySettings();
    }
}

void Wh_ModUninit() {
    Wh_Log(L">");
}

void Wh_ModSettingsChanged() {
    Wh_Log(L">");

    LoadSettings();

    if (g_target == Target::ShellExperienceHost) {
        ShellExperienceHost::ApplySettings();
    }
}
