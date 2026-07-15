// ==WindhawkMod==
// @id              liquid-glass
// @name            LiquidGlass
// @description     Brings a universal acrylic liquid glass effect with adjustable dark tint to Windows 11
// @version         1.0.0
// @author          Samuel Mwaandwe
// @github          https://github.com/YOUR_USERNAME/LiquidGlass
// @include         *
// @compilerOptions -luser32
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Universal Liquid Glass Tint

Applies a translucent, blurred "liquid glass" look to **every** top-level
window on the system (not just File Explorer), using the same acrylic-blur
accent policy that Windows itself uses internally, tinted with a
configurable dark overlay (default 35%).

## Notes / limitations
- Uses the undocumented `SetWindowCompositionAttribute` API, so behavior can
  change between Windows builds/updates.
- Apps that render their own background (Chrome/Electron apps, some UWP
  apps, games) may partially or fully ignore the effect.
- Run Windhawk as Administrator to affect elevated windows (e.g. Task
  Manager running as admin, elevated installers).
- Because this mod injects into every process (`@include *`), it's heavier
  than a typical single-app mod. If you notice slowdowns or crashes in a
  specific app, exclude it from the mod's "Advanced" include/exclude list
  in the Windhawk UI.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- tintOpacity: 35
  $name: Tint opacity
  $description: >-
    How dark the tint overlay is, from 0 (blur only, no tint) to 100 (solid
    black)
*/
// ==/WindhawkModSettings==

#include <windows.h>

// ---------------------------------------------------------------------
// Undocumented composition attribute API (used internally by DWM for
// the Acrylic/Mica/blur effects seen in Windows 10/11)
// ---------------------------------------------------------------------
enum ACCENT_STATE {
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3,
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4,
    ACCENT_ENABLE_HOSTBACKDROP = 5,
    ACCENT_INVALID_STATE = 6
};

struct ACCENT_POLICY {
    int AccentState;
    int AccentFlags;
    int GradientColor;  // 0xAABBGGRR
    int AnimationId;
};

enum WINDOWCOMPOSITIONATTRIB {
    WCA_ACCENT_POLICY = 19
};

struct WINDOWCOMPOSITIONATTRIBDATA {
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
};

typedef BOOL(WINAPI* SetWindowCompositionAttribute_t)(HWND, WINDOWCOMPOSITIONATTRIBDATA*);
SetWindowCompositionAttribute_t pSetWindowCompositionAttribute;

// ---------------------------------------------------------------------
// Settings
// ---------------------------------------------------------------------
int g_tintOpacityPercent = 35;

void LoadSettings() {
    g_tintOpacityPercent = Wh_GetIntSetting(L"tintOpacity");
    if (g_tintOpacityPercent < 0) g_tintOpacityPercent = 0;
    if (g_tintOpacityPercent > 100) g_tintOpacityPercent = 100;
}

// ---------------------------------------------------------------------
// Apply / remove the effect
// ---------------------------------------------------------------------
bool IsCandidateWindow(HWND hWnd) {
    if (!IsWindow(hWnd)) return false;

    // Skip tooltips, dropdowns, and other non-app popups
    LONG_PTR exStyle = GetWindowLongPtrW(hWnd, GWL_EXSTYLE);
    if (exStyle & WS_EX_TOOLWINDOW) return false;

    // Only touch real top-level windows, not child controls
    if (GetAncestor(hWnd, GA_ROOT) != hWnd) return false;

    return true;
}

void ApplyGlassTint(HWND hWnd) {
    if (!pSetWindowCompositionAttribute || !IsCandidateWindow(hWnd)) return;

    BYTE alpha = (BYTE)((g_tintOpacityPercent * 255) / 100);

    ACCENT_POLICY accent{};
    accent.AccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
    accent.AccentFlags = 0;
    accent.GradientColor = (alpha << 24) | 0x00000000;  // black tint, AABBGGRR
    accent.AnimationId = 0;

    WINDOWCOMPOSITIONATTRIBDATA data{};
    data.Attrib = WCA_ACCENT_POLICY;
    data.pvData = &accent;
    data.cbData = sizeof(accent);

    pSetWindowCompositionAttribute(hWnd, &data);
}

void RemoveGlassTint(HWND hWnd) {
    if (!pSetWindowCompositionAttribute || !IsWindow(hWnd)) return;

    ACCENT_POLICY accent{};
    accent.AccentState = ACCENT_DISABLED;

    WINDOWCOMPOSITIONATTRIBDATA data{};
    data.Attrib = WCA_ACCENT_POLICY;
    data.pvData = &accent;
    data.cbData = sizeof(accent);

    pSetWindowCompositionAttribute(hWnd, &data);
}

BOOL CALLBACK EnumWindowsProc_Apply(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        ApplyGlassTint(hWnd);
    }
    return TRUE;
}

BOOL CALLBACK EnumWindowsProc_Remove(HWND hWnd, LPARAM) {
    DWORD pid = 0;
    GetWindowThreadProcessId(hWnd, &pid);
    if (pid == GetCurrentProcessId()) {
        RemoveGlassTint(hWnd);
    }
    return TRUE;
}

// ---------------------------------------------------------------------
// Hook CreateWindowExW so every newly created top-level window gets the
// effect too, in every process this mod is injected into
// ---------------------------------------------------------------------
using CreateWindowExW_t = decltype(&CreateWindowExW);
CreateWindowExW_t CreateWindowExW_Original;

HWND WINAPI CreateWindowExW_Hook(
    DWORD dwExStyle,
    LPCWSTR lpClassName,
    LPCWSTR lpWindowName,
    DWORD dwStyle,
    int X, int Y, int nWidth, int nHeight,
    HWND hWndParent,
    HMENU hMenu,
    HINSTANCE hInstance,
    LPVOID lpParam
) {
    HWND hWnd = CreateWindowExW_Original(
        dwExStyle, lpClassName, lpWindowName, dwStyle,
        X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam
    );

    if (hWnd && hWndParent == NULL) {
        ApplyGlassTint(hWnd);
    }

    return hWnd;
}

// ---------------------------------------------------------------------
// Mod lifecycle
// ---------------------------------------------------------------------
BOOL Wh_ModInit() {
    LoadSettings();

    HMODULE hUser32 = LoadLibraryW(L"user32.dll");
    if (!hUser32) return FALSE;

    pSetWindowCompositionAttribute = (SetWindowCompositionAttribute_t)GetProcAddress(
        hUser32, "SetWindowCompositionAttribute"
    );
    if (!pSetWindowCompositionAttribute) {
        Wh_Log(L"SetWindowCompositionAttribute not found - unsupported Windows version");
        return FALSE;
    }

    Wh_SetFunctionHook(
        (void*)CreateWindowExW,
        (void*)CreateWindowExW_Hook,
        (void**)&CreateWindowExW_Original
    );

    // Apply to windows that already exist in this process
    EnumWindows(EnumWindowsProc_Apply, 0);

    return TRUE;
}

void Wh_ModAfterInit() {
    // Catch anything created between hook install and now
    EnumWindows(EnumWindowsProc_Apply, 0);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    EnumWindows(EnumWindowsProc_Apply, 0);
}

void Wh_ModUninit() {
    EnumWindows(EnumWindowsProc_Remove, 0);
}
