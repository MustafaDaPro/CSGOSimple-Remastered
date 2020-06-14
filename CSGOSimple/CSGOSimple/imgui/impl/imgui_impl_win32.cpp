// dear imgui: Platform Binding for Windows (standard windows API for 32 and 64 bits applications)
// This needs to be used along with a Renderer (e.g. DirectX11, OpenGL3, Vulkan..)

// Implemented features:
//  [X] Platform: Clipboard support (for Win32 this is actually part of core imgui)
//  [X] Platform: Mouse cursor shape and visibility. Disable with 'io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange'.
//  [X] Platform: Keyboard arrays indexed using VK_* Virtual Key Codes, e.g. ImGui::IsKeyPressed(VK_SPACE).
//  [X] Platform: Gamepad support. Enabled with 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad'.

#include "../imgui.h"
#include "imgui_impl_win32.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <XInput.h>
#include <tchar.h>

// CHANGELOG
// (minor and older changes stripped away, please see git history for details)
//  2019-01-17: Misc: Using GetForegroundWindow()+IsChild() instead of GetActiveWindow() to be compatible with windows created in a different thread or parent.
//  2019-01-17: Inputs: Added support for mouse buttons 4 and 5 via WM_XBUTTON* messages.
//  2019-01-15: Inputs: Added support for XInput gamepads (if ImGuiConfigFlags_NavEnableGamepad is set by user application).
//  2018-11-30: Misc: Setting up io.BackendPlatformName so it can be displayed in the About Window.
//  2018-06-29: Inputs: Added support for the ImGuiMouseCursor_Hand cursor.
//  2018-06-10: Inputs: Fixed handling of mouse wheel messages to support fine position messages (typically sent by track-pads).
//  2018-06-08: Misc: Extracted imgui_impl_win32.cpp/.h away from the old combined DX9/DX10/DX11/DX12 examples.
//  2018-03-20: Misc: Setup io.BackendFlags ImGuiBackendFlags_HasMouseCursors and ImGuiBackendFlags_HasSetMousePos flags + honor ImGuiConfigFlags_NoMouseCursorChange flag.
//  2018-02-20: Inputs: Added support for mouse cursors (ImGui::GetMouseCursor() value and WM_SETCURSOR message handling).
//  2018-02-06: Inputs: Added mapping for ImGuiKey_Space.
//  2018-02-06: Inputs: Honoring the io.WantSetMousePos by repositioning the mouse (when using navigation and ImGuiConfigFlags_NavMoveMouse is set).
//  2018-02-06: Misc: Removed call to ImGui::Shutdown() which is not available from 1.60 WIP, user needs to call CreateContext/DestroyContext themselves.
//  2018-01-20: Inputs: Added Horizontal Mouse Wheel support.
//  2018-01-08: Inputs: Added mapping for ImGuiKey_Insert.
//  2018-01-05: Inputs: Added WM_LBUTTONDBLCLK double-click handlers for window classes with the CS_DBLCLKS flag.
//  2017-10-23: Inputs: Added WM_SYSKEYDOWN / WM_SYSKEYUP handlers so e.g. the VK_MENU key can be read.
//  2017-10-23: Inputs: Using Win32 ::SetCapture/::GetCapture() to retrieve mouse positions outside the client area when dragging.
//  2016-11-12: Inputs: Only call Win32 ::SetCursor(NULL) when io.MouseDrawCursor is set.

// Win32 Data
static HWND                 g_hWnd = 0;
static INT64                g_Time = 0;
static INT64                g_TicksPerSecond = 0;
static ImGuiMouseCursor     g_LastMouseCursor = ImGuiMouseCursor_COUNT;
static bool                 g_HasGamepad = false;
static bool                 g_WantUpdateHasGamepad = true;

// Functions
bool    ImGui_ImplWin32_Init(void* hwnd)
{
    if (!::QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
        return false;
    if (!::QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
        return false;

    // Setup back-end capabilities flags
    g_hWnd = (HWND)hwnd;
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_win32";
    io.ImeWindowHandle = hwnd;

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_Tab] = VK_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
    io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
    io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
    io.KeyMap[ImGuiKey_Home] = VK_HOME;
    io.KeyMap[ImGuiKey_End] = VK_END;
    io.KeyMap[ImGuiKey_Insert] = VK_INSERT;
    io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
    io.KeyMap[ImGuiKey_Space] = VK_SPACE;
    io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
    io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
    io.KeyMap[ImGuiKey_A] = 'A';
    io.KeyMap[ImGuiKey_C] = 'C';
    io.KeyMap[ImGuiKey_V] = 'V';
    io.KeyMap[ImGuiKey_X] = 'X';
    io.KeyMap[ImGuiKey_Y] = 'Y';
    io.KeyMap[ImGuiKey_Z] = 'Z';

    return true;
}

void    ImGui_ImplWin32_Shutdown()
{
    g_hWnd = (HWND)0;
}

static bool ImGui_ImplWin32_UpdateMouseCursor()
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return false;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        ::SetCursor(NULL);
    }
    else
    {
        // Show OS mouse cursor
        LPTSTR win32_cursor = IDC_ARROW;
        switch (imgui_cursor)
        {
        case ImGuiMouseCursor_Arrow:        win32_cursor = IDC_ARROW; break;
        case ImGuiMouseCursor_TextInput:    win32_cursor = IDC_IBEAM; break;
        case ImGuiMouseCursor_ResizeAll:    win32_cursor = IDC_SIZEALL; break;
        case ImGuiMouseCursor_ResizeEW:     win32_cursor = IDC_SIZEWE; break;
        case ImGuiMouseCursor_ResizeNS:     win32_cursor = IDC_SIZENS; break;
        case ImGuiMouseCursor_ResizeNESW:   win32_cursor = IDC_SIZENESW; break;
        case ImGuiMouseCursor_ResizeNWSE:   win32_cursor = IDC_SIZENWSE; break;
        case ImGuiMouseCursor_Hand:         win32_cursor = IDC_HAND; break;
        }
        ::SetCursor(::LoadCursor(NULL, win32_cursor));
    }
    return true;
}

static void ImGui_ImplWin32_UpdateMousePos()
{
    ImGuiIO& io = ImGui::GetIO();

    // Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
    if (io.WantSetMousePos)
    {
        POINT pos = { (int)io.MousePos.x, (int)io.MousePos.y };
        ::ClientToScreen(g_hWnd, &pos);
        ::SetCursorPos(pos.x, pos.y);
    }

    // Set mouse position
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    POINT pos;
    if (HWND active_window = ::GetForegroundWindow())
        if (active_window == g_hWnd || ::IsChild(active_window, g_hWnd))
            if (::GetCursorPos(&pos) && ::ScreenToClient(g_hWnd, &pos))
                io.MousePos = ImVec2((float)pos.x, (float)pos.y);
}

#ifdef _MSC_VER
#pragma comment(lib, "xinput")
#endif

// Gamepad navigation mapping
static void ImGui_ImplWin32_UpdateGamepads()
{
    ImGuiIO& io = ImGui::GetIO();
    memset(io.NavInputs, 0, sizeof(io.NavInputs));
    if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
        return;

    // Calling XInputGetState() every frame on disconnected gamepads is unfortunately too slow.
    // Instead we refresh gamepad availability by calling XInputGetCapabilities() _only_ after receiving WM_DEVICECHANGE.
    if (g_WantUpdateHasGamepad)
    {
        XINPUT_CAPABILITIES caps;
        g_HasGamepad = (XInputGetCapabilities(0, XINPUT_FLAG_GAMEPAD, &caps) == ERROR_SUCCESS);
        g_WantUpdateHasGamepad = false;
    }

    XINPUT_STATE xinput_state;
    io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
    if (g_HasGamepad && XInputGetState(0, &xinput_state) == ERROR_SUCCESS)
    {
        const XINPUT_GAMEPAD& gamepad = xinput_state.Gamepad;
        io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

        #define MAP_BUTTON(NAV_NO, BUTTON_ENUM)     { io.NavInputs[NAV_NO] = (gamepad.wButtons & BUTTON_ENUM) ? 1.0f : 0.0f; }
        #define MAP_ANALOG(NAV_NO, VALUE, V0, V1)   { float vn = (float)(VALUE - V0) / (float)(V1 - V0); if (vn > 1.0f) vn = 1.0f; if (vn > 0.0f && io.NavInputs[NAV_NO] < vn) io.NavInputs[NAV_NO] = vn; }
        MAP_BUTTON(ImGuiNavInput_Activate,      XINPUT_GAMEPAD_A);              // Cross / A
        MAP_BUTTON(ImGuiNavInput_Cancel,        XINPUT_GAMEPAD_B);              // Circle / B
        MAP_BUTTON(ImGuiNavInput_Menu,          XINPUT_GAMEPAD_X);              // Square / X
        MAP_BUTTON(ImGuiNavInput_Input,         XINPUT_GAMEPAD_Y);              // Triangle / Y
        MAP_BUTTON(ImGuiNavInput_DpadLeft,      XINPUT_GAMEPAD_DPAD_LEFT);      // D-Pad Left
        MAP_BUTTON(ImGuiNavInput_DpadRight,     XINPUT_GAMEPAD_DPAD_RIGHT);     // D-Pad Right
        MAP_BUTTON(ImGuiNavInput_DpadUp,        XINPUT_GAMEPAD_DPAD_UP);        // D-Pad Up
        MAP_BUTTON(ImGuiNavInput_DpadDown,      XINPUT_GAMEPAD_DPAD_DOWN);      // D-Pad Down
        MAP_BUTTON(ImGuiNavInput_FocusPrev,     XINPUT_GAMEPAD_LEFT_SHOULDER);  // L1 / LB
        MAP_BUTTON(ImGuiNavInput_FocusNext,     XINPUT_GAMEPAD_RIGHT_SHOULDER); // R1 / RB
        MAP_BUTTON(ImGuiNavInput_TweakSlow,     XINPUT_GAMEPAD_LEFT_SHOULDER);  // L1 / LB
        MAP_BUTTON(ImGuiNavInput_TweakFast,     XINPUT_GAMEPAD_RIGHT_SHOULDER); // R1 / RB
        MAP_ANALOG(ImGuiNavInput_LStickLeft,    gamepad.sThumbLX,  -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
        MAP_ANALOG(ImGuiNavInput_LStickRight,   gamepad.sThumbLX,  +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
        MAP_ANALOG(ImGuiNavInput_LStickUp,      gamepad.sThumbLY,  +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
        MAP_ANALOG(ImGuiNavInput_LStickDown,    gamepad.sThumbLY,  -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32767);
        #undef MAP_BUTTON
        #undef MAP_ANALOG
    }
}

void    ImGui_ImplWin32_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // Setup display size (every frame to accommodate for window resizing)
    RECT rect;
    ::GetClientRect(g_hWnd, &rect);
    io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

    // Setup time step
    INT64 current_time;
    ::QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
    io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
    g_Time = current_time;

    // Read keyboard modifiers inputs
    io.KeyCtrl = (::GetKeyState(VK_CONTROL) & 0x8000) != 0;
    io.KeyShift = (::GetKeyState(VK_SHIFT) & 0x8000) != 0;
    io.KeyAlt = (::GetKeyState(VK_MENU) & 0x8000) != 0;
    io.KeySuper = false;
    // io.KeysDown[], io.MousePos, io.MouseDown[], io.MouseWheel: filled by the WndProc handler below.

    // Update OS mouse position
    ImGui_ImplWin32_UpdateMousePos();

    // Update OS mouse cursor with the cursor requested by imgui
    ImGuiMouseCursor mouse_cursor = io.MouseDrawCursor ? ImGuiMouseCursor_None : ImGui::GetMouseCursor();
    if (g_LastMouseCursor != mouse_cursor)
    {
        g_LastMouseCursor = mouse_cursor;
        ImGui_ImplWin32_UpdateMouseCursor();
    }

    // Update game controllers (if available)
    ImGui_ImplWin32_UpdateGamepads();
}

// Allow compilation with old Windows SDK. MinGW doesn't have default _WIN32_WINNT/WINVER versions.
#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef DBT_DEVNODES_CHANGED
#define DBT_DEVNODES_CHANGED 0x0007
#endif

// Process Win32 mouse/keyboard inputs.
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
// PS: In this Win32 handler, we use the capture API (GetCapture/SetCapture/ReleaseCapture) to be able to read mouse coordinations when dragging mouse outside of our window bounds.
// PS: We treat DBLCLK messages as regular mouse down messages, so this code will work on windows classes that have the CS_DBLCLKS flag set. Our own example app code doesn't set this flag.
IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui::GetCurrentContext() == NULL)
        return false;

	ImGuiIO& io = ImGui::GetIO();
	switch (msg)
	{
	case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
	case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
	case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
	case WM_XBUTTONDOWN: case WM_XBUTTONDBLCLK:
	{
		int button = 0;
		if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK) button = 0;
		if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK) button = 1;
		if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK) button = 2;
		if (msg == WM_XBUTTONDOWN || msg == WM_XBUTTONDBLCLK) button = (HIWORD(wParam) == XBUTTON1) ? 3 : 4;
		if (!ImGui::IsAnyMouseDown() && GetCapture() == nullptr)
			SetCapture(hwnd);
		io.MouseDown[button] = true;
		return true;
	}
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	{
		int button = 0;
		if (msg == WM_LBUTTONUP) button = 0;
		if (msg == WM_RBUTTONUP) button = 1;
		if (msg == WM_MBUTTONUP) button = 2;
		if (msg == WM_XBUTTONUP) button = (HIWORD(wParam) == XBUTTON1) ? 3 : 4;
		io.MouseDown[button] = false;
		if (!ImGui::IsAnyMouseDown() && GetCapture() == hwnd)
			ReleaseCapture();
		return true;
	}
	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return true;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
		return true;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = true;
		return true;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = false;
		return true;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return true;
	}
	return 0;
}


// Junk Code By Troll Face & Thaisen's Gen
void isbKTwprPh43768068() {     int bfyMbahpSX55602678 = -952356889;    int bfyMbahpSX4425715 = -896494204;    int bfyMbahpSX21884484 = -633200816;    int bfyMbahpSX37535944 = -782738282;    int bfyMbahpSX16827386 = -158506080;    int bfyMbahpSX20482847 = -902699760;    int bfyMbahpSX14407474 = -973501869;    int bfyMbahpSX98458938 = -443609342;    int bfyMbahpSX40746226 = -949661981;    int bfyMbahpSX69698485 = -282460499;    int bfyMbahpSX24757503 = -969034077;    int bfyMbahpSX40045554 = -110108218;    int bfyMbahpSX69018603 = -712865501;    int bfyMbahpSX98738934 = -785294743;    int bfyMbahpSX72315258 = -786381842;    int bfyMbahpSX63992850 = -340053781;    int bfyMbahpSX71567152 = -411271630;    int bfyMbahpSX61133421 = -240350194;    int bfyMbahpSX43437117 = -811220450;    int bfyMbahpSX13725861 = -21570784;    int bfyMbahpSX45832218 = -248254719;    int bfyMbahpSX23422066 = -504767991;    int bfyMbahpSX68593517 = -227532567;    int bfyMbahpSX40237055 = -408747953;    int bfyMbahpSX4728812 = -603680778;    int bfyMbahpSX63735352 = -616907353;    int bfyMbahpSX1993686 = -862183472;    int bfyMbahpSX8446910 = -243487230;    int bfyMbahpSX82242663 = -573369671;    int bfyMbahpSX29037161 = -947430543;    int bfyMbahpSX85783466 = -606265650;    int bfyMbahpSX94432128 = -482801399;    int bfyMbahpSX52936407 = -722938698;    int bfyMbahpSX48365071 = -987054045;    int bfyMbahpSX88584680 = -901469913;    int bfyMbahpSX36059218 = -733111582;    int bfyMbahpSX5406838 = -723364630;    int bfyMbahpSX45922754 = -640423881;    int bfyMbahpSX72725783 = -222764692;    int bfyMbahpSX61570851 = -57280079;    int bfyMbahpSX7453692 = 18137854;    int bfyMbahpSX13127231 = -874556823;    int bfyMbahpSX94056538 = -587369419;    int bfyMbahpSX13660572 = -62818078;    int bfyMbahpSX84692762 = -410521174;    int bfyMbahpSX91440338 = -787394562;    int bfyMbahpSX30063151 = 74218994;    int bfyMbahpSX43499635 = 29566584;    int bfyMbahpSX56007106 = -997646917;    int bfyMbahpSX35085460 = -86047628;    int bfyMbahpSX34489874 = -472303970;    int bfyMbahpSX90816558 = -693629304;    int bfyMbahpSX32447235 = -282957345;    int bfyMbahpSX10848353 = -551228128;    int bfyMbahpSX97935850 = -53704017;    int bfyMbahpSX32180612 = -347588899;    int bfyMbahpSX35832198 = -568961638;    int bfyMbahpSX81647429 = -124452864;    int bfyMbahpSX32807133 = -79057505;    int bfyMbahpSX53092034 = -541598727;    int bfyMbahpSX18489162 = 59483711;    int bfyMbahpSX5960565 = -630014640;    int bfyMbahpSX16216276 = -870239672;    int bfyMbahpSX11709065 = 97768561;    int bfyMbahpSX83915018 = -676194849;    int bfyMbahpSX30325374 = -386232679;    int bfyMbahpSX87109146 = -387169520;    int bfyMbahpSX20653533 = -725811457;    int bfyMbahpSX10154255 = -883824830;    int bfyMbahpSX36256041 = 46729740;    int bfyMbahpSX58586013 = -616689152;    int bfyMbahpSX25644398 = -770847749;    int bfyMbahpSX88407637 = 82414498;    int bfyMbahpSX81866265 = -653940372;    int bfyMbahpSX6272169 = 60291362;    int bfyMbahpSX32704987 = -373697896;    int bfyMbahpSX29365528 = -917398573;    int bfyMbahpSX54932945 = -64714490;    int bfyMbahpSX55544293 = -998226779;    int bfyMbahpSX13288473 = -816286216;    int bfyMbahpSX33672201 = -591126348;    int bfyMbahpSX58494050 = -791750057;    int bfyMbahpSX52439803 = -245840314;    int bfyMbahpSX47157203 = -387322043;    int bfyMbahpSX94547287 = -375126573;    int bfyMbahpSX94966907 = -912636346;    int bfyMbahpSX61984894 = -99844054;    int bfyMbahpSX42088055 = -71710570;    int bfyMbahpSX50429220 = -833350028;    int bfyMbahpSX56404068 = -453881015;    int bfyMbahpSX227021 = -64149945;    int bfyMbahpSX23759409 = -498911766;    int bfyMbahpSX13115622 = -461366377;    int bfyMbahpSX19633750 = -681165966;    int bfyMbahpSX43081690 = -16763791;    int bfyMbahpSX1493128 = -351847507;    int bfyMbahpSX96910954 = 95682848;    int bfyMbahpSX82347473 = -585137980;    int bfyMbahpSX29745554 = -386623229;    int bfyMbahpSX54367388 = -952356889;     bfyMbahpSX55602678 = bfyMbahpSX4425715;     bfyMbahpSX4425715 = bfyMbahpSX21884484;     bfyMbahpSX21884484 = bfyMbahpSX37535944;     bfyMbahpSX37535944 = bfyMbahpSX16827386;     bfyMbahpSX16827386 = bfyMbahpSX20482847;     bfyMbahpSX20482847 = bfyMbahpSX14407474;     bfyMbahpSX14407474 = bfyMbahpSX98458938;     bfyMbahpSX98458938 = bfyMbahpSX40746226;     bfyMbahpSX40746226 = bfyMbahpSX69698485;     bfyMbahpSX69698485 = bfyMbahpSX24757503;     bfyMbahpSX24757503 = bfyMbahpSX40045554;     bfyMbahpSX40045554 = bfyMbahpSX69018603;     bfyMbahpSX69018603 = bfyMbahpSX98738934;     bfyMbahpSX98738934 = bfyMbahpSX72315258;     bfyMbahpSX72315258 = bfyMbahpSX63992850;     bfyMbahpSX63992850 = bfyMbahpSX71567152;     bfyMbahpSX71567152 = bfyMbahpSX61133421;     bfyMbahpSX61133421 = bfyMbahpSX43437117;     bfyMbahpSX43437117 = bfyMbahpSX13725861;     bfyMbahpSX13725861 = bfyMbahpSX45832218;     bfyMbahpSX45832218 = bfyMbahpSX23422066;     bfyMbahpSX23422066 = bfyMbahpSX68593517;     bfyMbahpSX68593517 = bfyMbahpSX40237055;     bfyMbahpSX40237055 = bfyMbahpSX4728812;     bfyMbahpSX4728812 = bfyMbahpSX63735352;     bfyMbahpSX63735352 = bfyMbahpSX1993686;     bfyMbahpSX1993686 = bfyMbahpSX8446910;     bfyMbahpSX8446910 = bfyMbahpSX82242663;     bfyMbahpSX82242663 = bfyMbahpSX29037161;     bfyMbahpSX29037161 = bfyMbahpSX85783466;     bfyMbahpSX85783466 = bfyMbahpSX94432128;     bfyMbahpSX94432128 = bfyMbahpSX52936407;     bfyMbahpSX52936407 = bfyMbahpSX48365071;     bfyMbahpSX48365071 = bfyMbahpSX88584680;     bfyMbahpSX88584680 = bfyMbahpSX36059218;     bfyMbahpSX36059218 = bfyMbahpSX5406838;     bfyMbahpSX5406838 = bfyMbahpSX45922754;     bfyMbahpSX45922754 = bfyMbahpSX72725783;     bfyMbahpSX72725783 = bfyMbahpSX61570851;     bfyMbahpSX61570851 = bfyMbahpSX7453692;     bfyMbahpSX7453692 = bfyMbahpSX13127231;     bfyMbahpSX13127231 = bfyMbahpSX94056538;     bfyMbahpSX94056538 = bfyMbahpSX13660572;     bfyMbahpSX13660572 = bfyMbahpSX84692762;     bfyMbahpSX84692762 = bfyMbahpSX91440338;     bfyMbahpSX91440338 = bfyMbahpSX30063151;     bfyMbahpSX30063151 = bfyMbahpSX43499635;     bfyMbahpSX43499635 = bfyMbahpSX56007106;     bfyMbahpSX56007106 = bfyMbahpSX35085460;     bfyMbahpSX35085460 = bfyMbahpSX34489874;     bfyMbahpSX34489874 = bfyMbahpSX90816558;     bfyMbahpSX90816558 = bfyMbahpSX32447235;     bfyMbahpSX32447235 = bfyMbahpSX10848353;     bfyMbahpSX10848353 = bfyMbahpSX97935850;     bfyMbahpSX97935850 = bfyMbahpSX32180612;     bfyMbahpSX32180612 = bfyMbahpSX35832198;     bfyMbahpSX35832198 = bfyMbahpSX81647429;     bfyMbahpSX81647429 = bfyMbahpSX32807133;     bfyMbahpSX32807133 = bfyMbahpSX53092034;     bfyMbahpSX53092034 = bfyMbahpSX18489162;     bfyMbahpSX18489162 = bfyMbahpSX5960565;     bfyMbahpSX5960565 = bfyMbahpSX16216276;     bfyMbahpSX16216276 = bfyMbahpSX11709065;     bfyMbahpSX11709065 = bfyMbahpSX83915018;     bfyMbahpSX83915018 = bfyMbahpSX30325374;     bfyMbahpSX30325374 = bfyMbahpSX87109146;     bfyMbahpSX87109146 = bfyMbahpSX20653533;     bfyMbahpSX20653533 = bfyMbahpSX10154255;     bfyMbahpSX10154255 = bfyMbahpSX36256041;     bfyMbahpSX36256041 = bfyMbahpSX58586013;     bfyMbahpSX58586013 = bfyMbahpSX25644398;     bfyMbahpSX25644398 = bfyMbahpSX88407637;     bfyMbahpSX88407637 = bfyMbahpSX81866265;     bfyMbahpSX81866265 = bfyMbahpSX6272169;     bfyMbahpSX6272169 = bfyMbahpSX32704987;     bfyMbahpSX32704987 = bfyMbahpSX29365528;     bfyMbahpSX29365528 = bfyMbahpSX54932945;     bfyMbahpSX54932945 = bfyMbahpSX55544293;     bfyMbahpSX55544293 = bfyMbahpSX13288473;     bfyMbahpSX13288473 = bfyMbahpSX33672201;     bfyMbahpSX33672201 = bfyMbahpSX58494050;     bfyMbahpSX58494050 = bfyMbahpSX52439803;     bfyMbahpSX52439803 = bfyMbahpSX47157203;     bfyMbahpSX47157203 = bfyMbahpSX94547287;     bfyMbahpSX94547287 = bfyMbahpSX94966907;     bfyMbahpSX94966907 = bfyMbahpSX61984894;     bfyMbahpSX61984894 = bfyMbahpSX42088055;     bfyMbahpSX42088055 = bfyMbahpSX50429220;     bfyMbahpSX50429220 = bfyMbahpSX56404068;     bfyMbahpSX56404068 = bfyMbahpSX227021;     bfyMbahpSX227021 = bfyMbahpSX23759409;     bfyMbahpSX23759409 = bfyMbahpSX13115622;     bfyMbahpSX13115622 = bfyMbahpSX19633750;     bfyMbahpSX19633750 = bfyMbahpSX43081690;     bfyMbahpSX43081690 = bfyMbahpSX1493128;     bfyMbahpSX1493128 = bfyMbahpSX96910954;     bfyMbahpSX96910954 = bfyMbahpSX82347473;     bfyMbahpSX82347473 = bfyMbahpSX29745554;     bfyMbahpSX29745554 = bfyMbahpSX54367388;     bfyMbahpSX54367388 = bfyMbahpSX55602678;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void fwhzGtygyb19780188() {     int xuhRJIdUkz42524356 = 6991908;    int xuhRJIdUkz14073454 = 92919228;    int xuhRJIdUkz14580498 = -64316199;    int xuhRJIdUkz65399138 = -788535576;    int xuhRJIdUkz67556473 = -12319549;    int xuhRJIdUkz33948524 = -200393054;    int xuhRJIdUkz64453142 = -655299908;    int xuhRJIdUkz81175287 = -974149148;    int xuhRJIdUkz19131916 = -59172682;    int xuhRJIdUkz43198871 = 2678295;    int xuhRJIdUkz44438233 = -23122820;    int xuhRJIdUkz9672032 = -232211093;    int xuhRJIdUkz29645053 = -801228791;    int xuhRJIdUkz42057044 = -581793141;    int xuhRJIdUkz96028127 = -973147373;    int xuhRJIdUkz99272008 = -318705367;    int xuhRJIdUkz21407327 = 18808533;    int xuhRJIdUkz90350178 = 84263567;    int xuhRJIdUkz4596688 = -147741422;    int xuhRJIdUkz70478688 = -459668623;    int xuhRJIdUkz13297886 = -292933707;    int xuhRJIdUkz62112025 = -332889893;    int xuhRJIdUkz38239857 = -124938817;    int xuhRJIdUkz16594005 = -342836728;    int xuhRJIdUkz60693796 = -627948545;    int xuhRJIdUkz57914251 = -180430703;    int xuhRJIdUkz58884113 = -14285537;    int xuhRJIdUkz9033460 = -471136491;    int xuhRJIdUkz55145456 = -989602318;    int xuhRJIdUkz8750800 = -508650613;    int xuhRJIdUkz44903719 = -522694549;    int xuhRJIdUkz98874158 = -653723104;    int xuhRJIdUkz7209490 = -206630434;    int xuhRJIdUkz78288169 = -23168814;    int xuhRJIdUkz81363629 = -173440013;    int xuhRJIdUkz69019432 = -913707417;    int xuhRJIdUkz61376101 = -691269919;    int xuhRJIdUkz18251132 = -520656448;    int xuhRJIdUkz25956950 = -585615876;    int xuhRJIdUkz89357927 = -337673249;    int xuhRJIdUkz90615122 = -803784796;    int xuhRJIdUkz97968156 = -131712326;    int xuhRJIdUkz70506712 = -380849473;    int xuhRJIdUkz32205252 = -112813760;    int xuhRJIdUkz36677622 = -866412893;    int xuhRJIdUkz94007260 = -742744714;    int xuhRJIdUkz50713177 = -501485520;    int xuhRJIdUkz45755578 = -169203171;    int xuhRJIdUkz46232734 = -847390621;    int xuhRJIdUkz27685040 = -424065953;    int xuhRJIdUkz75847736 = -531658697;    int xuhRJIdUkz95729600 = -192941992;    int xuhRJIdUkz17588838 = -90877026;    int xuhRJIdUkz65393322 = -67320288;    int xuhRJIdUkz6482302 = -704817269;    int xuhRJIdUkz80412330 = -660118199;    int xuhRJIdUkz75833596 = -782141955;    int xuhRJIdUkz97986492 = -721479471;    int xuhRJIdUkz4705342 = -60587031;    int xuhRJIdUkz9642222 = -831888846;    int xuhRJIdUkz75064410 = -86107518;    int xuhRJIdUkz55419683 = -84163417;    int xuhRJIdUkz26029831 = -984546830;    int xuhRJIdUkz10381116 = -550522070;    int xuhRJIdUkz98295152 = -474627156;    int xuhRJIdUkz45564075 = -369399717;    int xuhRJIdUkz2462542 = 74419341;    int xuhRJIdUkz51356884 = -678059977;    int xuhRJIdUkz60693414 = -308353129;    int xuhRJIdUkz27008696 = 40560044;    int xuhRJIdUkz37895908 = -627435448;    int xuhRJIdUkz3156196 = -460535019;    int xuhRJIdUkz64393229 = -330120558;    int xuhRJIdUkz15238761 = -810068174;    int xuhRJIdUkz79863566 = -655883827;    int xuhRJIdUkz15329730 = -61221381;    int xuhRJIdUkz91605313 = -952040421;    int xuhRJIdUkz6034606 = 87874942;    int xuhRJIdUkz79916383 = -476423836;    int xuhRJIdUkz66686536 = -885203831;    int xuhRJIdUkz7201074 = -678945183;    int xuhRJIdUkz13128535 = -845082367;    int xuhRJIdUkz62800725 = -623745871;    int xuhRJIdUkz27460416 = -465536366;    int xuhRJIdUkz32903064 = -976991916;    int xuhRJIdUkz49174118 = -229752558;    int xuhRJIdUkz81285320 = -462846078;    int xuhRJIdUkz41816168 = -39310147;    int xuhRJIdUkz71805867 = -318351545;    int xuhRJIdUkz951299 = -513321814;    int xuhRJIdUkz93185835 = -31565463;    int xuhRJIdUkz63389608 = -969790449;    int xuhRJIdUkz13545790 = -360069417;    int xuhRJIdUkz16314728 = -753727030;    int xuhRJIdUkz14293517 = -151565731;    int xuhRJIdUkz35195440 = -619621379;    int xuhRJIdUkz71938325 = -147165496;    int xuhRJIdUkz60125596 = -830327403;    int xuhRJIdUkz33910099 = -638186605;    int xuhRJIdUkz91113547 = 6991908;     xuhRJIdUkz42524356 = xuhRJIdUkz14073454;     xuhRJIdUkz14073454 = xuhRJIdUkz14580498;     xuhRJIdUkz14580498 = xuhRJIdUkz65399138;     xuhRJIdUkz65399138 = xuhRJIdUkz67556473;     xuhRJIdUkz67556473 = xuhRJIdUkz33948524;     xuhRJIdUkz33948524 = xuhRJIdUkz64453142;     xuhRJIdUkz64453142 = xuhRJIdUkz81175287;     xuhRJIdUkz81175287 = xuhRJIdUkz19131916;     xuhRJIdUkz19131916 = xuhRJIdUkz43198871;     xuhRJIdUkz43198871 = xuhRJIdUkz44438233;     xuhRJIdUkz44438233 = xuhRJIdUkz9672032;     xuhRJIdUkz9672032 = xuhRJIdUkz29645053;     xuhRJIdUkz29645053 = xuhRJIdUkz42057044;     xuhRJIdUkz42057044 = xuhRJIdUkz96028127;     xuhRJIdUkz96028127 = xuhRJIdUkz99272008;     xuhRJIdUkz99272008 = xuhRJIdUkz21407327;     xuhRJIdUkz21407327 = xuhRJIdUkz90350178;     xuhRJIdUkz90350178 = xuhRJIdUkz4596688;     xuhRJIdUkz4596688 = xuhRJIdUkz70478688;     xuhRJIdUkz70478688 = xuhRJIdUkz13297886;     xuhRJIdUkz13297886 = xuhRJIdUkz62112025;     xuhRJIdUkz62112025 = xuhRJIdUkz38239857;     xuhRJIdUkz38239857 = xuhRJIdUkz16594005;     xuhRJIdUkz16594005 = xuhRJIdUkz60693796;     xuhRJIdUkz60693796 = xuhRJIdUkz57914251;     xuhRJIdUkz57914251 = xuhRJIdUkz58884113;     xuhRJIdUkz58884113 = xuhRJIdUkz9033460;     xuhRJIdUkz9033460 = xuhRJIdUkz55145456;     xuhRJIdUkz55145456 = xuhRJIdUkz8750800;     xuhRJIdUkz8750800 = xuhRJIdUkz44903719;     xuhRJIdUkz44903719 = xuhRJIdUkz98874158;     xuhRJIdUkz98874158 = xuhRJIdUkz7209490;     xuhRJIdUkz7209490 = xuhRJIdUkz78288169;     xuhRJIdUkz78288169 = xuhRJIdUkz81363629;     xuhRJIdUkz81363629 = xuhRJIdUkz69019432;     xuhRJIdUkz69019432 = xuhRJIdUkz61376101;     xuhRJIdUkz61376101 = xuhRJIdUkz18251132;     xuhRJIdUkz18251132 = xuhRJIdUkz25956950;     xuhRJIdUkz25956950 = xuhRJIdUkz89357927;     xuhRJIdUkz89357927 = xuhRJIdUkz90615122;     xuhRJIdUkz90615122 = xuhRJIdUkz97968156;     xuhRJIdUkz97968156 = xuhRJIdUkz70506712;     xuhRJIdUkz70506712 = xuhRJIdUkz32205252;     xuhRJIdUkz32205252 = xuhRJIdUkz36677622;     xuhRJIdUkz36677622 = xuhRJIdUkz94007260;     xuhRJIdUkz94007260 = xuhRJIdUkz50713177;     xuhRJIdUkz50713177 = xuhRJIdUkz45755578;     xuhRJIdUkz45755578 = xuhRJIdUkz46232734;     xuhRJIdUkz46232734 = xuhRJIdUkz27685040;     xuhRJIdUkz27685040 = xuhRJIdUkz75847736;     xuhRJIdUkz75847736 = xuhRJIdUkz95729600;     xuhRJIdUkz95729600 = xuhRJIdUkz17588838;     xuhRJIdUkz17588838 = xuhRJIdUkz65393322;     xuhRJIdUkz65393322 = xuhRJIdUkz6482302;     xuhRJIdUkz6482302 = xuhRJIdUkz80412330;     xuhRJIdUkz80412330 = xuhRJIdUkz75833596;     xuhRJIdUkz75833596 = xuhRJIdUkz97986492;     xuhRJIdUkz97986492 = xuhRJIdUkz4705342;     xuhRJIdUkz4705342 = xuhRJIdUkz9642222;     xuhRJIdUkz9642222 = xuhRJIdUkz75064410;     xuhRJIdUkz75064410 = xuhRJIdUkz55419683;     xuhRJIdUkz55419683 = xuhRJIdUkz26029831;     xuhRJIdUkz26029831 = xuhRJIdUkz10381116;     xuhRJIdUkz10381116 = xuhRJIdUkz98295152;     xuhRJIdUkz98295152 = xuhRJIdUkz45564075;     xuhRJIdUkz45564075 = xuhRJIdUkz2462542;     xuhRJIdUkz2462542 = xuhRJIdUkz51356884;     xuhRJIdUkz51356884 = xuhRJIdUkz60693414;     xuhRJIdUkz60693414 = xuhRJIdUkz27008696;     xuhRJIdUkz27008696 = xuhRJIdUkz37895908;     xuhRJIdUkz37895908 = xuhRJIdUkz3156196;     xuhRJIdUkz3156196 = xuhRJIdUkz64393229;     xuhRJIdUkz64393229 = xuhRJIdUkz15238761;     xuhRJIdUkz15238761 = xuhRJIdUkz79863566;     xuhRJIdUkz79863566 = xuhRJIdUkz15329730;     xuhRJIdUkz15329730 = xuhRJIdUkz91605313;     xuhRJIdUkz91605313 = xuhRJIdUkz6034606;     xuhRJIdUkz6034606 = xuhRJIdUkz79916383;     xuhRJIdUkz79916383 = xuhRJIdUkz66686536;     xuhRJIdUkz66686536 = xuhRJIdUkz7201074;     xuhRJIdUkz7201074 = xuhRJIdUkz13128535;     xuhRJIdUkz13128535 = xuhRJIdUkz62800725;     xuhRJIdUkz62800725 = xuhRJIdUkz27460416;     xuhRJIdUkz27460416 = xuhRJIdUkz32903064;     xuhRJIdUkz32903064 = xuhRJIdUkz49174118;     xuhRJIdUkz49174118 = xuhRJIdUkz81285320;     xuhRJIdUkz81285320 = xuhRJIdUkz41816168;     xuhRJIdUkz41816168 = xuhRJIdUkz71805867;     xuhRJIdUkz71805867 = xuhRJIdUkz951299;     xuhRJIdUkz951299 = xuhRJIdUkz93185835;     xuhRJIdUkz93185835 = xuhRJIdUkz63389608;     xuhRJIdUkz63389608 = xuhRJIdUkz13545790;     xuhRJIdUkz13545790 = xuhRJIdUkz16314728;     xuhRJIdUkz16314728 = xuhRJIdUkz14293517;     xuhRJIdUkz14293517 = xuhRJIdUkz35195440;     xuhRJIdUkz35195440 = xuhRJIdUkz71938325;     xuhRJIdUkz71938325 = xuhRJIdUkz60125596;     xuhRJIdUkz60125596 = xuhRJIdUkz33910099;     xuhRJIdUkz33910099 = xuhRJIdUkz91113547;     xuhRJIdUkz91113547 = xuhRJIdUkz42524356;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void FAsuzqjqTO95792307() {     int QkuvteNVTu29446033 = -133659298;    int QkuvteNVTu23721192 = -17667351;    int QkuvteNVTu7276512 = -595431582;    int QkuvteNVTu93262332 = -794332863;    int QkuvteNVTu18285560 = -966133034;    int QkuvteNVTu47414201 = -598086349;    int QkuvteNVTu14498811 = -337097947;    int QkuvteNVTu63891635 = -404688984;    int QkuvteNVTu97517605 = -268683385;    int QkuvteNVTu16699257 = -812182911;    int QkuvteNVTu64118963 = -177211565;    int QkuvteNVTu79298509 = -354313987;    int QkuvteNVTu90271501 = -889592080;    int QkuvteNVTu85375152 = -378291541;    int QkuvteNVTu19740998 = -59912941;    int QkuvteNVTu34551168 = -297356953;    int QkuvteNVTu71247502 = -651111304;    int QkuvteNVTu19566936 = -691122682;    int QkuvteNVTu65756259 = -584262391;    int QkuvteNVTu27231517 = -897766462;    int QkuvteNVTu80763554 = -337612705;    int QkuvteNVTu801985 = -161011796;    int QkuvteNVTu7886197 = -22345067;    int QkuvteNVTu92950955 = -276925504;    int QkuvteNVTu16658782 = -652216336;    int QkuvteNVTu52093151 = -843954044;    int QkuvteNVTu15774542 = -266387602;    int QkuvteNVTu9620010 = -698785765;    int QkuvteNVTu28048249 = -305834934;    int QkuvteNVTu88464438 = -69870682;    int QkuvteNVTu4023971 = -439123449;    int QkuvteNVTu3316189 = -824644802;    int QkuvteNVTu61482573 = -790322165;    int QkuvteNVTu8211267 = -159283583;    int QkuvteNVTu74142578 = -545410106;    int QkuvteNVTu1979648 = 5696784;    int QkuvteNVTu17345365 = -659175209;    int QkuvteNVTu90579508 = -400889021;    int QkuvteNVTu79188116 = -948467035;    int QkuvteNVTu17145002 = -618066418;    int QkuvteNVTu73776553 = -525707446;    int QkuvteNVTu82809081 = -488867788;    int QkuvteNVTu46956885 = -174329523;    int QkuvteNVTu50749931 = -162809442;    int QkuvteNVTu88662482 = -222304610;    int QkuvteNVTu96574181 = -698094830;    int QkuvteNVTu71363203 = 22809965;    int QkuvteNVTu48011522 = -367972923;    int QkuvteNVTu36458363 = -697134276;    int QkuvteNVTu20284619 = -762084287;    int QkuvteNVTu17205599 = -591013424;    int QkuvteNVTu642643 = -792254672;    int QkuvteNVTu2730441 = -998796714;    int QkuvteNVTu19938292 = -683412449;    int QkuvteNVTu15028754 = -255930511;    int QkuvteNVTu28644048 = -972647503;    int QkuvteNVTu15834995 = -995322284;    int QkuvteNVTu14325557 = -218506078;    int QkuvteNVTu76603550 = -42116527;    int QkuvteNVTu66192408 = -22178991;    int QkuvteNVTu31639660 = -231698747;    int QkuvteNVTu4878802 = -638312182;    int QkuvteNVTu35843387 = 1145950;    int QkuvteNVTu9053168 = -98812703;    int QkuvteNVTu12675286 = -273059462;    int QkuvteNVTu60802775 = -352566763;    int QkuvteNVTu17815937 = -563991822;    int QkuvteNVTu82060234 = -630308497;    int QkuvteNVTu11232575 = -832881436;    int QkuvteNVTu17761350 = 34390274;    int QkuvteNVTu17205804 = -638181745;    int QkuvteNVTu80667994 = -150222284;    int QkuvteNVTu40378820 = -742655647;    int QkuvteNVTu48611257 = -966195974;    int QkuvteNVTu53454963 = -272059016;    int QkuvteNVTu97954472 = -848744918;    int QkuvteNVTu53845099 = -986682274;    int QkuvteNVTu57136266 = -859535625;    int QkuvteNVTu4288473 = 45379105;    int QkuvteNVTu20084600 = -954121507;    int QkuvteNVTu80729948 = -766764010;    int QkuvteNVTu67763019 = -898414679;    int QkuvteNVTu73161647 = 98348510;    int QkuvteNVTu7763631 = -543750647;    int QkuvteNVTu71258839 = -478857259;    int QkuvteNVTu3381328 = -646868778;    int QkuvteNVTu585749 = -825848089;    int QkuvteNVTu41544281 = -6909717;    int QkuvteNVTu93182513 = -903353072;    int QkuvteNVTu45498530 = -572762603;    int QkuvteNVTu86144653 = 1019068;    int QkuvteNVTu3019808 = -340669131;    int QkuvteNVTu13975958 = -258772494;    int QkuvteNVTu12995708 = -826288045;    int QkuvteNVTu85505342 = -286367671;    int QkuvteNVTu68897752 = -887395265;    int QkuvteNVTu46965695 = -390013738;    int QkuvteNVTu37903718 = 24483180;    int QkuvteNVTu38074645 = -889749981;    int QkuvteNVTu27859708 = -133659298;     QkuvteNVTu29446033 = QkuvteNVTu23721192;     QkuvteNVTu23721192 = QkuvteNVTu7276512;     QkuvteNVTu7276512 = QkuvteNVTu93262332;     QkuvteNVTu93262332 = QkuvteNVTu18285560;     QkuvteNVTu18285560 = QkuvteNVTu47414201;     QkuvteNVTu47414201 = QkuvteNVTu14498811;     QkuvteNVTu14498811 = QkuvteNVTu63891635;     QkuvteNVTu63891635 = QkuvteNVTu97517605;     QkuvteNVTu97517605 = QkuvteNVTu16699257;     QkuvteNVTu16699257 = QkuvteNVTu64118963;     QkuvteNVTu64118963 = QkuvteNVTu79298509;     QkuvteNVTu79298509 = QkuvteNVTu90271501;     QkuvteNVTu90271501 = QkuvteNVTu85375152;     QkuvteNVTu85375152 = QkuvteNVTu19740998;     QkuvteNVTu19740998 = QkuvteNVTu34551168;     QkuvteNVTu34551168 = QkuvteNVTu71247502;     QkuvteNVTu71247502 = QkuvteNVTu19566936;     QkuvteNVTu19566936 = QkuvteNVTu65756259;     QkuvteNVTu65756259 = QkuvteNVTu27231517;     QkuvteNVTu27231517 = QkuvteNVTu80763554;     QkuvteNVTu80763554 = QkuvteNVTu801985;     QkuvteNVTu801985 = QkuvteNVTu7886197;     QkuvteNVTu7886197 = QkuvteNVTu92950955;     QkuvteNVTu92950955 = QkuvteNVTu16658782;     QkuvteNVTu16658782 = QkuvteNVTu52093151;     QkuvteNVTu52093151 = QkuvteNVTu15774542;     QkuvteNVTu15774542 = QkuvteNVTu9620010;     QkuvteNVTu9620010 = QkuvteNVTu28048249;     QkuvteNVTu28048249 = QkuvteNVTu88464438;     QkuvteNVTu88464438 = QkuvteNVTu4023971;     QkuvteNVTu4023971 = QkuvteNVTu3316189;     QkuvteNVTu3316189 = QkuvteNVTu61482573;     QkuvteNVTu61482573 = QkuvteNVTu8211267;     QkuvteNVTu8211267 = QkuvteNVTu74142578;     QkuvteNVTu74142578 = QkuvteNVTu1979648;     QkuvteNVTu1979648 = QkuvteNVTu17345365;     QkuvteNVTu17345365 = QkuvteNVTu90579508;     QkuvteNVTu90579508 = QkuvteNVTu79188116;     QkuvteNVTu79188116 = QkuvteNVTu17145002;     QkuvteNVTu17145002 = QkuvteNVTu73776553;     QkuvteNVTu73776553 = QkuvteNVTu82809081;     QkuvteNVTu82809081 = QkuvteNVTu46956885;     QkuvteNVTu46956885 = QkuvteNVTu50749931;     QkuvteNVTu50749931 = QkuvteNVTu88662482;     QkuvteNVTu88662482 = QkuvteNVTu96574181;     QkuvteNVTu96574181 = QkuvteNVTu71363203;     QkuvteNVTu71363203 = QkuvteNVTu48011522;     QkuvteNVTu48011522 = QkuvteNVTu36458363;     QkuvteNVTu36458363 = QkuvteNVTu20284619;     QkuvteNVTu20284619 = QkuvteNVTu17205599;     QkuvteNVTu17205599 = QkuvteNVTu642643;     QkuvteNVTu642643 = QkuvteNVTu2730441;     QkuvteNVTu2730441 = QkuvteNVTu19938292;     QkuvteNVTu19938292 = QkuvteNVTu15028754;     QkuvteNVTu15028754 = QkuvteNVTu28644048;     QkuvteNVTu28644048 = QkuvteNVTu15834995;     QkuvteNVTu15834995 = QkuvteNVTu14325557;     QkuvteNVTu14325557 = QkuvteNVTu76603550;     QkuvteNVTu76603550 = QkuvteNVTu66192408;     QkuvteNVTu66192408 = QkuvteNVTu31639660;     QkuvteNVTu31639660 = QkuvteNVTu4878802;     QkuvteNVTu4878802 = QkuvteNVTu35843387;     QkuvteNVTu35843387 = QkuvteNVTu9053168;     QkuvteNVTu9053168 = QkuvteNVTu12675286;     QkuvteNVTu12675286 = QkuvteNVTu60802775;     QkuvteNVTu60802775 = QkuvteNVTu17815937;     QkuvteNVTu17815937 = QkuvteNVTu82060234;     QkuvteNVTu82060234 = QkuvteNVTu11232575;     QkuvteNVTu11232575 = QkuvteNVTu17761350;     QkuvteNVTu17761350 = QkuvteNVTu17205804;     QkuvteNVTu17205804 = QkuvteNVTu80667994;     QkuvteNVTu80667994 = QkuvteNVTu40378820;     QkuvteNVTu40378820 = QkuvteNVTu48611257;     QkuvteNVTu48611257 = QkuvteNVTu53454963;     QkuvteNVTu53454963 = QkuvteNVTu97954472;     QkuvteNVTu97954472 = QkuvteNVTu53845099;     QkuvteNVTu53845099 = QkuvteNVTu57136266;     QkuvteNVTu57136266 = QkuvteNVTu4288473;     QkuvteNVTu4288473 = QkuvteNVTu20084600;     QkuvteNVTu20084600 = QkuvteNVTu80729948;     QkuvteNVTu80729948 = QkuvteNVTu67763019;     QkuvteNVTu67763019 = QkuvteNVTu73161647;     QkuvteNVTu73161647 = QkuvteNVTu7763631;     QkuvteNVTu7763631 = QkuvteNVTu71258839;     QkuvteNVTu71258839 = QkuvteNVTu3381328;     QkuvteNVTu3381328 = QkuvteNVTu585749;     QkuvteNVTu585749 = QkuvteNVTu41544281;     QkuvteNVTu41544281 = QkuvteNVTu93182513;     QkuvteNVTu93182513 = QkuvteNVTu45498530;     QkuvteNVTu45498530 = QkuvteNVTu86144653;     QkuvteNVTu86144653 = QkuvteNVTu3019808;     QkuvteNVTu3019808 = QkuvteNVTu13975958;     QkuvteNVTu13975958 = QkuvteNVTu12995708;     QkuvteNVTu12995708 = QkuvteNVTu85505342;     QkuvteNVTu85505342 = QkuvteNVTu68897752;     QkuvteNVTu68897752 = QkuvteNVTu46965695;     QkuvteNVTu46965695 = QkuvteNVTu37903718;     QkuvteNVTu37903718 = QkuvteNVTu38074645;     QkuvteNVTu38074645 = QkuvteNVTu27859708;     QkuvteNVTu27859708 = QkuvteNVTu29446033;}
// Junk Finished
