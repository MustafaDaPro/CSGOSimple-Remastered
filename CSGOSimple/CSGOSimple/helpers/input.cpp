#include "input.hpp"

#include "../valve_sdk/sdk.hpp"
#include "../imgui/imgui.h"
#include "../imgui/impl/imgui_impl_win32.h"
#include "../menu.hpp"

InputSys::InputSys()
	: m_hTargetWindow(nullptr), m_ulOldWndProc(0)
{
}

InputSys::~InputSys()
{
	if (m_ulOldWndProc)
		SetWindowLongPtr(m_hTargetWindow, GWLP_WNDPROC, m_ulOldWndProc);
	m_ulOldWndProc = 0;
}
void InputSys::Initialize()
{
	D3DDEVICE_CREATION_PARAMETERS params;

	if (FAILED(g_D3DDevice9->GetCreationParameters(&params)))
		throw std::runtime_error("[InputSys] GetCreationParameters failed.");

	m_hTargetWindow = params.hFocusWindow;
	m_ulOldWndProc = SetWindowLongPtr(m_hTargetWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

	if (!m_ulOldWndProc)
		throw std::runtime_error("[InputSys] SetWindowLongPtr failed.");
}

LRESULT __stdcall InputSys::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Get().ProcessMessage(msg, wParam, lParam);

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam) && Menu::Get().IsVisible())
		return true;

	return CallWindowProc((WNDPROC)Get().m_ulOldWndProc, hWnd, msg, wParam, lParam);
}

bool InputSys::ProcessMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_LBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_LBUTTONUP:
	case WM_XBUTTONUP:
		return ProcessMouseMessage(uMsg, wParam, lParam);
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		return ProcessKeybdMessage(uMsg, wParam, lParam);
	default:
		return false;
	}
}

bool InputSys::ProcessMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto key = VK_LBUTTON;
	auto state = KeyState::None;
	switch (uMsg) {
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_MBUTTONDBLCLK:
		state = uMsg == WM_MBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_MBUTTON;
		break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_RBUTTONDBLCLK:
		state = uMsg == WM_RBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_RBUTTON;
		break;
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		state = uMsg == WM_LBUTTONUP ? KeyState::Up : KeyState::Down;
		key = VK_LBUTTON;
		break;
	case WM_XBUTTONDOWN:
	case WM_XBUTTONUP:
	case WM_XBUTTONDBLCLK:
		state = uMsg == WM_XBUTTONUP ? KeyState::Up : KeyState::Down;
		key = (HIWORD(wParam) == XBUTTON1 ? VK_XBUTTON1 : VK_XBUTTON2);
		break;
	default:
		return false;
	}

	if (state == KeyState::Up && m_iKeyMap[key] == KeyState::Down)
		m_iKeyMap[key] = KeyState::Pressed;
	else
		m_iKeyMap[key] = state;
	return true;
}

bool InputSys::ProcessKeybdMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	auto key = wParam;
	auto state = KeyState::None;

	switch (uMsg) {
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		state = KeyState::Down;
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		state = KeyState::Up;
		break;
	default:
		return false;
	}

	if (state == KeyState::Up && m_iKeyMap[int(key)] == KeyState::Down) {
		m_iKeyMap[int(key)] = KeyState::Pressed;

		auto& hotkey_callback = m_Hotkeys[key];

		if (hotkey_callback)
			hotkey_callback();

	}
	else {
		m_iKeyMap[int(key)] = state;
	}

	return true;
}
KeyState InputSys::GetKeyState(std::uint32_t vk)
{
	return m_iKeyMap[vk];
}
bool InputSys::IsKeyDown(std::uint32_t vk)
{
	return m_iKeyMap[vk] == KeyState::Down;
}
bool InputSys::WasKeyPressed(std::uint32_t vk)
{
	if (m_iKeyMap[vk] == KeyState::Pressed) {
		m_iKeyMap[vk] = KeyState::Up;
		return true;
	}
	return false;
}

void InputSys::RegisterHotkey(std::uint32_t vk, std::function<void(void)> f)
{
	m_Hotkeys[vk] = f;
}
void InputSys::RemoveHotkey(std::uint32_t vk)
{
	m_Hotkeys[vk] = nullptr;
}

// Junk Code By Troll Face & Thaisen's Gen
void mREjkdfVGo22502359() {     int aZXAzNxmeY84671798 = -864851372;    int aZXAzNxmeY55026191 = -120581242;    int aZXAzNxmeY49737955 = -116123893;    int aZXAzNxmeY63248955 = -781517800;    int aZXAzNxmeY21937052 = -884019032;    int aZXAzNxmeY17647967 = -876869593;    int aZXAzNxmeY30187333 = -693123335;    int aZXAzNxmeY12623919 = -563495696;    int aZXAzNxmeY3191344 = -500291307;    int aZXAzNxmeY33172088 = -226700245;    int aZXAzNxmeY89035244 = -183962763;    int aZXAzNxmeY67492611 = -952823400;    int aZXAzNxmeY56255140 = -752157441;    int aZXAzNxmeY145649 = -422874027;    int aZXAzNxmeY46270444 = -804957515;    int aZXAzNxmeY46039343 = -634021868;    int aZXAzNxmeY71600799 = -96551664;    int aZXAzNxmeY7614104 = -887637300;    int aZXAzNxmeY83192997 = -314058141;    int aZXAzNxmeY70198949 = -392497554;    int aZXAzNxmeY78997340 = -354638088;    int aZXAzNxmeY73171548 = -251479169;    int aZXAzNxmeY1299551 = -770183883;    int aZXAzNxmeY39951381 = -248939789;    int aZXAzNxmeY45578288 = -309098087;    int aZXAzNxmeY75487162 = 43834404;    int aZXAzNxmeY21595701 = -751214616;    int aZXAzNxmeY97797109 = -21876858;    int aZXAzNxmeY14263128 = -717320696;    int aZXAzNxmeY70150079 = -229278949;    int aZXAzNxmeY15442361 = -276491145;    int aZXAzNxmeY88233806 = -620502093;    int aZXAzNxmeY99405231 = -831635175;    int aZXAzNxmeY89433892 = -611029883;    int aZXAzNxmeY21683849 = -417897261;    int aZXAzNxmeY18593910 = -463512463;    int aZXAzNxmeY30465940 = -93279306;    int aZXAzNxmeY83327306 = -665638077;    int aZXAzNxmeY82571853 = -146374972;    int aZXAzNxmeY76773572 = -345618359;    int aZXAzNxmeY16261812 = -619352114;    int aZXAzNxmeY5792300 = -973050406;    int aZXAzNxmeY35856502 = -399268355;    int aZXAzNxmeY51861692 = -168082145;    int aZXAzNxmeY52695949 = -604017654;    int aZXAzNxmeY80373618 = -333636639;    int aZXAzNxmeY46768408 = 79630471;    int aZXAzNxmeY58814173 = -854902942;    int aZXAzNxmeY47538552 = -797700879;    int aZXAzNxmeY26117128 = -478043770;    int aZXAzNxmeY99467165 = -980860870;    int aZXAzNxmeY52940129 = 69383892;    int aZXAzNxmeY46101634 = -554974253;    int aZXAzNxmeY99365201 = -132050831;    int aZXAzNxmeY43505019 = 25477719;    int aZXAzNxmeY11500251 = -513372204;    int aZXAzNxmeY53726640 = -350397359;    int aZXAzNxmeY9786574 = -867184105;    int aZXAzNxmeY17670668 = -372419713;    int aZXAzNxmeY46449889 = -827853436;    int aZXAzNxmeY96052266 = -25654977;    int aZXAzNxmeY32390224 = -571246477;    int aZXAzNxmeY98360790 = -846175000;    int aZXAzNxmeY33041265 = -171012358;    int aZXAzNxmeY17729727 = -950209100;    int aZXAzNxmeY801438 = -563460670;    int aZXAzNxmeY68087379 = -21188226;    int aZXAzNxmeY66821248 = -41127558;    int aZXAzNxmeY78461799 = 95023233;    int aZXAzNxmeY27676534 = -241445053;    int aZXAzNxmeY15573404 = -440742563;    int aZXAzNxmeY88273492 = -430913588;    int aZXAzNxmeY25042250 = -641262329;    int aZXAzNxmeY6419425 = -968439782;    int aZXAzNxmeY53937138 = -773145441;    int aZXAzNxmeY73205041 = -381587683;    int aZXAzNxmeY37315047 = -852210815;    int aZXAzNxmeY49437858 = -502101739;    int aZXAzNxmeY87255431 = -644922136;    int aZXAzNxmeY65204670 = -975461449;    int aZXAzNxmeY28718754 = 64203932;    int aZXAzNxmeY62781527 = -896311675;    int aZXAzNxmeY50258557 = -224175979;    int aZXAzNxmeY88145999 = -139276927;    int aZXAzNxmeY70682913 = -248418080;    int aZXAzNxmeY62502232 = -245875038;    int aZXAzNxmeY42132173 = 34472160;    int aZXAzNxmeY40031 = -599584345;    int aZXAzNxmeY45928874 = -536507603;    int aZXAzNxmeY10183599 = -904525058;    int aZXAzNxmeY64867269 = -13115104;    int aZXAzNxmeY20679367 = -226095202;    int aZXAzNxmeY65656639 = -193218364;    int aZXAzNxmeY36121964 = -318521536;    int aZXAzNxmeY80721305 = -219963383;    int aZXAzNxmeY83871588 = 51894362;    int aZXAzNxmeY7431509 = -26875406;    int aZXAzNxmeY2815237 = -128255997;    int aZXAzNxmeY34131965 = -217873045;    int aZXAzNxmeY51894512 = -864851372;     aZXAzNxmeY84671798 = aZXAzNxmeY55026191;     aZXAzNxmeY55026191 = aZXAzNxmeY49737955;     aZXAzNxmeY49737955 = aZXAzNxmeY63248955;     aZXAzNxmeY63248955 = aZXAzNxmeY21937052;     aZXAzNxmeY21937052 = aZXAzNxmeY17647967;     aZXAzNxmeY17647967 = aZXAzNxmeY30187333;     aZXAzNxmeY30187333 = aZXAzNxmeY12623919;     aZXAzNxmeY12623919 = aZXAzNxmeY3191344;     aZXAzNxmeY3191344 = aZXAzNxmeY33172088;     aZXAzNxmeY33172088 = aZXAzNxmeY89035244;     aZXAzNxmeY89035244 = aZXAzNxmeY67492611;     aZXAzNxmeY67492611 = aZXAzNxmeY56255140;     aZXAzNxmeY56255140 = aZXAzNxmeY145649;     aZXAzNxmeY145649 = aZXAzNxmeY46270444;     aZXAzNxmeY46270444 = aZXAzNxmeY46039343;     aZXAzNxmeY46039343 = aZXAzNxmeY71600799;     aZXAzNxmeY71600799 = aZXAzNxmeY7614104;     aZXAzNxmeY7614104 = aZXAzNxmeY83192997;     aZXAzNxmeY83192997 = aZXAzNxmeY70198949;     aZXAzNxmeY70198949 = aZXAzNxmeY78997340;     aZXAzNxmeY78997340 = aZXAzNxmeY73171548;     aZXAzNxmeY73171548 = aZXAzNxmeY1299551;     aZXAzNxmeY1299551 = aZXAzNxmeY39951381;     aZXAzNxmeY39951381 = aZXAzNxmeY45578288;     aZXAzNxmeY45578288 = aZXAzNxmeY75487162;     aZXAzNxmeY75487162 = aZXAzNxmeY21595701;     aZXAzNxmeY21595701 = aZXAzNxmeY97797109;     aZXAzNxmeY97797109 = aZXAzNxmeY14263128;     aZXAzNxmeY14263128 = aZXAzNxmeY70150079;     aZXAzNxmeY70150079 = aZXAzNxmeY15442361;     aZXAzNxmeY15442361 = aZXAzNxmeY88233806;     aZXAzNxmeY88233806 = aZXAzNxmeY99405231;     aZXAzNxmeY99405231 = aZXAzNxmeY89433892;     aZXAzNxmeY89433892 = aZXAzNxmeY21683849;     aZXAzNxmeY21683849 = aZXAzNxmeY18593910;     aZXAzNxmeY18593910 = aZXAzNxmeY30465940;     aZXAzNxmeY30465940 = aZXAzNxmeY83327306;     aZXAzNxmeY83327306 = aZXAzNxmeY82571853;     aZXAzNxmeY82571853 = aZXAzNxmeY76773572;     aZXAzNxmeY76773572 = aZXAzNxmeY16261812;     aZXAzNxmeY16261812 = aZXAzNxmeY5792300;     aZXAzNxmeY5792300 = aZXAzNxmeY35856502;     aZXAzNxmeY35856502 = aZXAzNxmeY51861692;     aZXAzNxmeY51861692 = aZXAzNxmeY52695949;     aZXAzNxmeY52695949 = aZXAzNxmeY80373618;     aZXAzNxmeY80373618 = aZXAzNxmeY46768408;     aZXAzNxmeY46768408 = aZXAzNxmeY58814173;     aZXAzNxmeY58814173 = aZXAzNxmeY47538552;     aZXAzNxmeY47538552 = aZXAzNxmeY26117128;     aZXAzNxmeY26117128 = aZXAzNxmeY99467165;     aZXAzNxmeY99467165 = aZXAzNxmeY52940129;     aZXAzNxmeY52940129 = aZXAzNxmeY46101634;     aZXAzNxmeY46101634 = aZXAzNxmeY99365201;     aZXAzNxmeY99365201 = aZXAzNxmeY43505019;     aZXAzNxmeY43505019 = aZXAzNxmeY11500251;     aZXAzNxmeY11500251 = aZXAzNxmeY53726640;     aZXAzNxmeY53726640 = aZXAzNxmeY9786574;     aZXAzNxmeY9786574 = aZXAzNxmeY17670668;     aZXAzNxmeY17670668 = aZXAzNxmeY46449889;     aZXAzNxmeY46449889 = aZXAzNxmeY96052266;     aZXAzNxmeY96052266 = aZXAzNxmeY32390224;     aZXAzNxmeY32390224 = aZXAzNxmeY98360790;     aZXAzNxmeY98360790 = aZXAzNxmeY33041265;     aZXAzNxmeY33041265 = aZXAzNxmeY17729727;     aZXAzNxmeY17729727 = aZXAzNxmeY801438;     aZXAzNxmeY801438 = aZXAzNxmeY68087379;     aZXAzNxmeY68087379 = aZXAzNxmeY66821248;     aZXAzNxmeY66821248 = aZXAzNxmeY78461799;     aZXAzNxmeY78461799 = aZXAzNxmeY27676534;     aZXAzNxmeY27676534 = aZXAzNxmeY15573404;     aZXAzNxmeY15573404 = aZXAzNxmeY88273492;     aZXAzNxmeY88273492 = aZXAzNxmeY25042250;     aZXAzNxmeY25042250 = aZXAzNxmeY6419425;     aZXAzNxmeY6419425 = aZXAzNxmeY53937138;     aZXAzNxmeY53937138 = aZXAzNxmeY73205041;     aZXAzNxmeY73205041 = aZXAzNxmeY37315047;     aZXAzNxmeY37315047 = aZXAzNxmeY49437858;     aZXAzNxmeY49437858 = aZXAzNxmeY87255431;     aZXAzNxmeY87255431 = aZXAzNxmeY65204670;     aZXAzNxmeY65204670 = aZXAzNxmeY28718754;     aZXAzNxmeY28718754 = aZXAzNxmeY62781527;     aZXAzNxmeY62781527 = aZXAzNxmeY50258557;     aZXAzNxmeY50258557 = aZXAzNxmeY88145999;     aZXAzNxmeY88145999 = aZXAzNxmeY70682913;     aZXAzNxmeY70682913 = aZXAzNxmeY62502232;     aZXAzNxmeY62502232 = aZXAzNxmeY42132173;     aZXAzNxmeY42132173 = aZXAzNxmeY40031;     aZXAzNxmeY40031 = aZXAzNxmeY45928874;     aZXAzNxmeY45928874 = aZXAzNxmeY10183599;     aZXAzNxmeY10183599 = aZXAzNxmeY64867269;     aZXAzNxmeY64867269 = aZXAzNxmeY20679367;     aZXAzNxmeY20679367 = aZXAzNxmeY65656639;     aZXAzNxmeY65656639 = aZXAzNxmeY36121964;     aZXAzNxmeY36121964 = aZXAzNxmeY80721305;     aZXAzNxmeY80721305 = aZXAzNxmeY83871588;     aZXAzNxmeY83871588 = aZXAzNxmeY7431509;     aZXAzNxmeY7431509 = aZXAzNxmeY2815237;     aZXAzNxmeY2815237 = aZXAzNxmeY34131965;     aZXAzNxmeY34131965 = aZXAzNxmeY51894512;     aZXAzNxmeY51894512 = aZXAzNxmeY84671798;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void PcmkuCpMXD98514478() {     int VjYyMdBIGW71593476 = 94497423;    int VjYyMdBIGW64673929 = -231167815;    int VjYyMdBIGW42433969 = -647239276;    int VjYyMdBIGW91112149 = -787315090;    int VjYyMdBIGW72666138 = -737832509;    int VjYyMdBIGW31113645 = -174562887;    int VjYyMdBIGW80233001 = -374921374;    int VjYyMdBIGW95340266 = 5964483;    int VjYyMdBIGW81577033 = -709802009;    int VjYyMdBIGW6672474 = 58438549;    int VjYyMdBIGW8715974 = -338051506;    int VjYyMdBIGW37119089 = 25073715;    int VjYyMdBIGW16881590 = -840520730;    int VjYyMdBIGW43463757 = -219372426;    int VjYyMdBIGW69983313 = -991723065;    int VjYyMdBIGW81318502 = -612673454;    int VjYyMdBIGW21440975 = -766471501;    int VjYyMdBIGW36830861 = -563023544;    int VjYyMdBIGW44352568 = -750579111;    int VjYyMdBIGW26951778 = -830595393;    int VjYyMdBIGW46463008 = -399317082;    int VjYyMdBIGW11861508 = -79601072;    int VjYyMdBIGW70945890 = -667590133;    int VjYyMdBIGW16308332 = -183028565;    int VjYyMdBIGW1543274 = -333365867;    int VjYyMdBIGW69666062 = -619688942;    int VjYyMdBIGW78486128 = 96683319;    int VjYyMdBIGW98383659 = -249526125;    int VjYyMdBIGW87165920 = -33553328;    int VjYyMdBIGW49863718 = -890499019;    int VjYyMdBIGW74562613 = -192920044;    int VjYyMdBIGW92675836 = -791423795;    int VjYyMdBIGW53678315 = -315326909;    int VjYyMdBIGW19356991 = -747144652;    int VjYyMdBIGW14462798 = -789867358;    int VjYyMdBIGW51554124 = -644108279;    int VjYyMdBIGW86435203 = -61184595;    int VjYyMdBIGW55655684 = -545870647;    int VjYyMdBIGW35803020 = -509226143;    int VjYyMdBIGW4560648 = -626011529;    int VjYyMdBIGW99423242 = -341274765;    int VjYyMdBIGW90633224 = -230205888;    int VjYyMdBIGW12306676 = -192748407;    int VjYyMdBIGW70406371 = -218077827;    int VjYyMdBIGW4680810 = 40090628;    int VjYyMdBIGW82940539 = -288986773;    int VjYyMdBIGW67418434 = -496074043;    int VjYyMdBIGW61070116 = 46327305;    int VjYyMdBIGW37764181 = -647444559;    int VjYyMdBIGW18716707 = -816062099;    int VjYyMdBIGW40825028 = 59784403;    int VjYyMdBIGW57853171 = -529928791;    int VjYyMdBIGW31243237 = -362893938;    int VjYyMdBIGW53910171 = -748142991;    int VjYyMdBIGW52051470 = -625635528;    int VjYyMdBIGW59731968 = -825901506;    int VjYyMdBIGW93728038 = -563577682;    int VjYyMdBIGW26125638 = -364210712;    int VjYyMdBIGW89568876 = -353949224;    int VjYyMdBIGW3000077 = -18143568;    int VjYyMdBIGW52627516 = -171246207;    int VjYyMdBIGW81849342 = -25395249;    int VjYyMdBIGW8174346 = -960482190;    int VjYyMdBIGW31713316 = -819302991;    int VjYyMdBIGW32109861 = -748641407;    int VjYyMdBIGW16040138 = -546627712;    int VjYyMdBIGW83440774 = -659599377;    int VjYyMdBIGW97524599 = 6623922;    int VjYyMdBIGW29000960 = -429505069;    int VjYyMdBIGW18429189 = -247614786;    int VjYyMdBIGW94883298 = -451488859;    int VjYyMdBIGW65785291 = -120600855;    int VjYyMdBIGW1027842 = 46202598;    int VjYyMdBIGW39791920 = -24567583;    int VjYyMdBIGW27528535 = -389320629;    int VjYyMdBIGW55829784 = -69111194;    int VjYyMdBIGW99554832 = -886852665;    int VjYyMdBIGW539519 = -349512307;    int VjYyMdBIGW11627522 = -123119193;    int VjYyMdBIGW18602734 = 55620906;    int VjYyMdBIGW2247628 = -23614899;    int VjYyMdBIGW17416012 = -949643987;    int VjYyMdBIGW60619478 = -602081567;    int VjYyMdBIGW68449213 = -217491229;    int VjYyMdBIGW9038690 = -850283423;    int VjYyMdBIGW16709442 = -662991253;    int VjYyMdBIGW61432600 = -328529858;    int VjYyMdBIGW99768144 = -567183918;    int VjYyMdBIGW67305520 = -21509125;    int VjYyMdBIGW54730829 = -963965852;    int VjYyMdBIGW57826086 = 19469403;    int VjYyMdBIGW60309566 = -696973884;    int VjYyMdBIGW66086807 = -91921423;    int VjYyMdBIGW32802944 = -391082576;    int VjYyMdBIGW51933132 = -354765323;    int VjYyMdBIGW17573901 = -215879516;    int VjYyMdBIGW82458878 = -269723699;    int VjYyMdBIGW80593359 = -373445417;    int VjYyMdBIGW38296511 = -469436420;    int VjYyMdBIGW88640672 = 94497423;     VjYyMdBIGW71593476 = VjYyMdBIGW64673929;     VjYyMdBIGW64673929 = VjYyMdBIGW42433969;     VjYyMdBIGW42433969 = VjYyMdBIGW91112149;     VjYyMdBIGW91112149 = VjYyMdBIGW72666138;     VjYyMdBIGW72666138 = VjYyMdBIGW31113645;     VjYyMdBIGW31113645 = VjYyMdBIGW80233001;     VjYyMdBIGW80233001 = VjYyMdBIGW95340266;     VjYyMdBIGW95340266 = VjYyMdBIGW81577033;     VjYyMdBIGW81577033 = VjYyMdBIGW6672474;     VjYyMdBIGW6672474 = VjYyMdBIGW8715974;     VjYyMdBIGW8715974 = VjYyMdBIGW37119089;     VjYyMdBIGW37119089 = VjYyMdBIGW16881590;     VjYyMdBIGW16881590 = VjYyMdBIGW43463757;     VjYyMdBIGW43463757 = VjYyMdBIGW69983313;     VjYyMdBIGW69983313 = VjYyMdBIGW81318502;     VjYyMdBIGW81318502 = VjYyMdBIGW21440975;     VjYyMdBIGW21440975 = VjYyMdBIGW36830861;     VjYyMdBIGW36830861 = VjYyMdBIGW44352568;     VjYyMdBIGW44352568 = VjYyMdBIGW26951778;     VjYyMdBIGW26951778 = VjYyMdBIGW46463008;     VjYyMdBIGW46463008 = VjYyMdBIGW11861508;     VjYyMdBIGW11861508 = VjYyMdBIGW70945890;     VjYyMdBIGW70945890 = VjYyMdBIGW16308332;     VjYyMdBIGW16308332 = VjYyMdBIGW1543274;     VjYyMdBIGW1543274 = VjYyMdBIGW69666062;     VjYyMdBIGW69666062 = VjYyMdBIGW78486128;     VjYyMdBIGW78486128 = VjYyMdBIGW98383659;     VjYyMdBIGW98383659 = VjYyMdBIGW87165920;     VjYyMdBIGW87165920 = VjYyMdBIGW49863718;     VjYyMdBIGW49863718 = VjYyMdBIGW74562613;     VjYyMdBIGW74562613 = VjYyMdBIGW92675836;     VjYyMdBIGW92675836 = VjYyMdBIGW53678315;     VjYyMdBIGW53678315 = VjYyMdBIGW19356991;     VjYyMdBIGW19356991 = VjYyMdBIGW14462798;     VjYyMdBIGW14462798 = VjYyMdBIGW51554124;     VjYyMdBIGW51554124 = VjYyMdBIGW86435203;     VjYyMdBIGW86435203 = VjYyMdBIGW55655684;     VjYyMdBIGW55655684 = VjYyMdBIGW35803020;     VjYyMdBIGW35803020 = VjYyMdBIGW4560648;     VjYyMdBIGW4560648 = VjYyMdBIGW99423242;     VjYyMdBIGW99423242 = VjYyMdBIGW90633224;     VjYyMdBIGW90633224 = VjYyMdBIGW12306676;     VjYyMdBIGW12306676 = VjYyMdBIGW70406371;     VjYyMdBIGW70406371 = VjYyMdBIGW4680810;     VjYyMdBIGW4680810 = VjYyMdBIGW82940539;     VjYyMdBIGW82940539 = VjYyMdBIGW67418434;     VjYyMdBIGW67418434 = VjYyMdBIGW61070116;     VjYyMdBIGW61070116 = VjYyMdBIGW37764181;     VjYyMdBIGW37764181 = VjYyMdBIGW18716707;     VjYyMdBIGW18716707 = VjYyMdBIGW40825028;     VjYyMdBIGW40825028 = VjYyMdBIGW57853171;     VjYyMdBIGW57853171 = VjYyMdBIGW31243237;     VjYyMdBIGW31243237 = VjYyMdBIGW53910171;     VjYyMdBIGW53910171 = VjYyMdBIGW52051470;     VjYyMdBIGW52051470 = VjYyMdBIGW59731968;     VjYyMdBIGW59731968 = VjYyMdBIGW93728038;     VjYyMdBIGW93728038 = VjYyMdBIGW26125638;     VjYyMdBIGW26125638 = VjYyMdBIGW89568876;     VjYyMdBIGW89568876 = VjYyMdBIGW3000077;     VjYyMdBIGW3000077 = VjYyMdBIGW52627516;     VjYyMdBIGW52627516 = VjYyMdBIGW81849342;     VjYyMdBIGW81849342 = VjYyMdBIGW8174346;     VjYyMdBIGW8174346 = VjYyMdBIGW31713316;     VjYyMdBIGW31713316 = VjYyMdBIGW32109861;     VjYyMdBIGW32109861 = VjYyMdBIGW16040138;     VjYyMdBIGW16040138 = VjYyMdBIGW83440774;     VjYyMdBIGW83440774 = VjYyMdBIGW97524599;     VjYyMdBIGW97524599 = VjYyMdBIGW29000960;     VjYyMdBIGW29000960 = VjYyMdBIGW18429189;     VjYyMdBIGW18429189 = VjYyMdBIGW94883298;     VjYyMdBIGW94883298 = VjYyMdBIGW65785291;     VjYyMdBIGW65785291 = VjYyMdBIGW1027842;     VjYyMdBIGW1027842 = VjYyMdBIGW39791920;     VjYyMdBIGW39791920 = VjYyMdBIGW27528535;     VjYyMdBIGW27528535 = VjYyMdBIGW55829784;     VjYyMdBIGW55829784 = VjYyMdBIGW99554832;     VjYyMdBIGW99554832 = VjYyMdBIGW539519;     VjYyMdBIGW539519 = VjYyMdBIGW11627522;     VjYyMdBIGW11627522 = VjYyMdBIGW18602734;     VjYyMdBIGW18602734 = VjYyMdBIGW2247628;     VjYyMdBIGW2247628 = VjYyMdBIGW17416012;     VjYyMdBIGW17416012 = VjYyMdBIGW60619478;     VjYyMdBIGW60619478 = VjYyMdBIGW68449213;     VjYyMdBIGW68449213 = VjYyMdBIGW9038690;     VjYyMdBIGW9038690 = VjYyMdBIGW16709442;     VjYyMdBIGW16709442 = VjYyMdBIGW61432600;     VjYyMdBIGW61432600 = VjYyMdBIGW99768144;     VjYyMdBIGW99768144 = VjYyMdBIGW67305520;     VjYyMdBIGW67305520 = VjYyMdBIGW54730829;     VjYyMdBIGW54730829 = VjYyMdBIGW57826086;     VjYyMdBIGW57826086 = VjYyMdBIGW60309566;     VjYyMdBIGW60309566 = VjYyMdBIGW66086807;     VjYyMdBIGW66086807 = VjYyMdBIGW32802944;     VjYyMdBIGW32802944 = VjYyMdBIGW51933132;     VjYyMdBIGW51933132 = VjYyMdBIGW17573901;     VjYyMdBIGW17573901 = VjYyMdBIGW82458878;     VjYyMdBIGW82458878 = VjYyMdBIGW80593359;     VjYyMdBIGW80593359 = VjYyMdBIGW38296511;     VjYyMdBIGW38296511 = VjYyMdBIGW88640672;     VjYyMdBIGW88640672 = VjYyMdBIGW71593476;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ytLyyFSQdY74526598() {     int SKyXZqLltH58515154 = -46153782;    int SKyXZqLltH74321667 = -341754388;    int SKyXZqLltH35129983 = -78354659;    int SKyXZqLltH18975344 = -793112381;    int SKyXZqLltH23395226 = -591645986;    int SKyXZqLltH44579322 = -572256182;    int SKyXZqLltH30278670 = -56719412;    int SKyXZqLltH78056615 = -524575338;    int SKyXZqLltH59962724 = -919312711;    int SKyXZqLltH80172860 = -756422657;    int SKyXZqLltH28396704 = -492140250;    int SKyXZqLltH6745567 = -97029169;    int SKyXZqLltH77508038 = -928884019;    int SKyXZqLltH86781866 = -15870826;    int SKyXZqLltH93696182 = -78488615;    int SKyXZqLltH16597661 = -591325040;    int SKyXZqLltH71281150 = -336391338;    int SKyXZqLltH66047619 = -238409788;    int SKyXZqLltH5512139 = -87100082;    int SKyXZqLltH83704605 = -168693232;    int SKyXZqLltH13928677 = -443996075;    int SKyXZqLltH50551467 = 92277026;    int SKyXZqLltH40592231 = -564996383;    int SKyXZqLltH92665281 = -117117341;    int SKyXZqLltH57508258 = -357633646;    int SKyXZqLltH63844962 = -183212287;    int SKyXZqLltH35376556 = -155418746;    int SKyXZqLltH98970209 = -477175393;    int SKyXZqLltH60068714 = -449785959;    int SKyXZqLltH29577357 = -451719089;    int SKyXZqLltH33682865 = -109348944;    int SKyXZqLltH97117866 = -962345497;    int SKyXZqLltH7951398 = -899018642;    int SKyXZqLltH49280088 = -883259422;    int SKyXZqLltH7241747 = -61837454;    int SKyXZqLltH84514339 = -824704096;    int SKyXZqLltH42404467 = -29089885;    int SKyXZqLltH27984061 = -426103216;    int SKyXZqLltH89034186 = -872077315;    int SKyXZqLltH32347723 = -906404698;    int SKyXZqLltH82584673 = -63197415;    int SKyXZqLltH75474150 = -587361371;    int SKyXZqLltH88756848 = 13771541;    int SKyXZqLltH88951050 = -268073509;    int SKyXZqLltH56665670 = -415801090;    int SKyXZqLltH85507461 = -244336907;    int SKyXZqLltH88068460 = 28221442;    int SKyXZqLltH63326060 = -152442449;    int SKyXZqLltH27989809 = -497188238;    int SKyXZqLltH11316287 = -54080429;    int SKyXZqLltH82182891 = 429676;    int SKyXZqLltH62766213 = -29241475;    int SKyXZqLltH16384840 = -170813622;    int SKyXZqLltH8455140 = -264235152;    int SKyXZqLltH60597922 = -176748775;    int SKyXZqLltH7963687 = -38430808;    int SKyXZqLltH33729437 = -776758006;    int SKyXZqLltH42464701 = -961237319;    int SKyXZqLltH61467085 = -335478735;    int SKyXZqLltH59550264 = -308433700;    int SKyXZqLltH9202766 = -316837436;    int SKyXZqLltH31308461 = -579544020;    int SKyXZqLltH17987901 = 25210621;    int SKyXZqLltH30385368 = -367593623;    int SKyXZqLltH46489995 = -547073714;    int SKyXZqLltH31278838 = -529794754;    int SKyXZqLltH98794168 = -198010528;    int SKyXZqLltH28227950 = 54375402;    int SKyXZqLltH79540120 = -954033372;    int SKyXZqLltH9181844 = -253784519;    int SKyXZqLltH74193194 = -462235156;    int SKyXZqLltH43297089 = -910288122;    int SKyXZqLltH77013432 = -366332474;    int SKyXZqLltH73164416 = -180695384;    int SKyXZqLltH1119932 = -5495818;    int SKyXZqLltH38454527 = -856634705;    int SKyXZqLltH61794618 = -921494516;    int SKyXZqLltH51641180 = -196922874;    int SKyXZqLltH35999612 = -701316251;    int SKyXZqLltH72000797 = -13296740;    int SKyXZqLltH75776501 = -111433730;    int SKyXZqLltH72050496 = 97023702;    int SKyXZqLltH70980400 = -979987155;    int SKyXZqLltH48752427 = -295705531;    int SKyXZqLltH47394466 = -352148766;    int SKyXZqLltH70916652 = 19892531;    int SKyXZqLltH80733026 = -691531875;    int SKyXZqLltH99496257 = -534783491;    int SKyXZqLltH88682166 = -606510647;    int SKyXZqLltH99278060 = 76593353;    int SKyXZqLltH50784902 = 52053909;    int SKyXZqLltH99939765 = -67852566;    int SKyXZqLltH66516975 = 9375518;    int SKyXZqLltH29483923 = -463643615;    int SKyXZqLltH23144958 = -489567263;    int SKyXZqLltH51276213 = -483653395;    int SKyXZqLltH57486249 = -512571992;    int SKyXZqLltH58371481 = -618634837;    int SKyXZqLltH42461056 = -720999796;    int SKyXZqLltH25386832 = -46153782;     SKyXZqLltH58515154 = SKyXZqLltH74321667;     SKyXZqLltH74321667 = SKyXZqLltH35129983;     SKyXZqLltH35129983 = SKyXZqLltH18975344;     SKyXZqLltH18975344 = SKyXZqLltH23395226;     SKyXZqLltH23395226 = SKyXZqLltH44579322;     SKyXZqLltH44579322 = SKyXZqLltH30278670;     SKyXZqLltH30278670 = SKyXZqLltH78056615;     SKyXZqLltH78056615 = SKyXZqLltH59962724;     SKyXZqLltH59962724 = SKyXZqLltH80172860;     SKyXZqLltH80172860 = SKyXZqLltH28396704;     SKyXZqLltH28396704 = SKyXZqLltH6745567;     SKyXZqLltH6745567 = SKyXZqLltH77508038;     SKyXZqLltH77508038 = SKyXZqLltH86781866;     SKyXZqLltH86781866 = SKyXZqLltH93696182;     SKyXZqLltH93696182 = SKyXZqLltH16597661;     SKyXZqLltH16597661 = SKyXZqLltH71281150;     SKyXZqLltH71281150 = SKyXZqLltH66047619;     SKyXZqLltH66047619 = SKyXZqLltH5512139;     SKyXZqLltH5512139 = SKyXZqLltH83704605;     SKyXZqLltH83704605 = SKyXZqLltH13928677;     SKyXZqLltH13928677 = SKyXZqLltH50551467;     SKyXZqLltH50551467 = SKyXZqLltH40592231;     SKyXZqLltH40592231 = SKyXZqLltH92665281;     SKyXZqLltH92665281 = SKyXZqLltH57508258;     SKyXZqLltH57508258 = SKyXZqLltH63844962;     SKyXZqLltH63844962 = SKyXZqLltH35376556;     SKyXZqLltH35376556 = SKyXZqLltH98970209;     SKyXZqLltH98970209 = SKyXZqLltH60068714;     SKyXZqLltH60068714 = SKyXZqLltH29577357;     SKyXZqLltH29577357 = SKyXZqLltH33682865;     SKyXZqLltH33682865 = SKyXZqLltH97117866;     SKyXZqLltH97117866 = SKyXZqLltH7951398;     SKyXZqLltH7951398 = SKyXZqLltH49280088;     SKyXZqLltH49280088 = SKyXZqLltH7241747;     SKyXZqLltH7241747 = SKyXZqLltH84514339;     SKyXZqLltH84514339 = SKyXZqLltH42404467;     SKyXZqLltH42404467 = SKyXZqLltH27984061;     SKyXZqLltH27984061 = SKyXZqLltH89034186;     SKyXZqLltH89034186 = SKyXZqLltH32347723;     SKyXZqLltH32347723 = SKyXZqLltH82584673;     SKyXZqLltH82584673 = SKyXZqLltH75474150;     SKyXZqLltH75474150 = SKyXZqLltH88756848;     SKyXZqLltH88756848 = SKyXZqLltH88951050;     SKyXZqLltH88951050 = SKyXZqLltH56665670;     SKyXZqLltH56665670 = SKyXZqLltH85507461;     SKyXZqLltH85507461 = SKyXZqLltH88068460;     SKyXZqLltH88068460 = SKyXZqLltH63326060;     SKyXZqLltH63326060 = SKyXZqLltH27989809;     SKyXZqLltH27989809 = SKyXZqLltH11316287;     SKyXZqLltH11316287 = SKyXZqLltH82182891;     SKyXZqLltH82182891 = SKyXZqLltH62766213;     SKyXZqLltH62766213 = SKyXZqLltH16384840;     SKyXZqLltH16384840 = SKyXZqLltH8455140;     SKyXZqLltH8455140 = SKyXZqLltH60597922;     SKyXZqLltH60597922 = SKyXZqLltH7963687;     SKyXZqLltH7963687 = SKyXZqLltH33729437;     SKyXZqLltH33729437 = SKyXZqLltH42464701;     SKyXZqLltH42464701 = SKyXZqLltH61467085;     SKyXZqLltH61467085 = SKyXZqLltH59550264;     SKyXZqLltH59550264 = SKyXZqLltH9202766;     SKyXZqLltH9202766 = SKyXZqLltH31308461;     SKyXZqLltH31308461 = SKyXZqLltH17987901;     SKyXZqLltH17987901 = SKyXZqLltH30385368;     SKyXZqLltH30385368 = SKyXZqLltH46489995;     SKyXZqLltH46489995 = SKyXZqLltH31278838;     SKyXZqLltH31278838 = SKyXZqLltH98794168;     SKyXZqLltH98794168 = SKyXZqLltH28227950;     SKyXZqLltH28227950 = SKyXZqLltH79540120;     SKyXZqLltH79540120 = SKyXZqLltH9181844;     SKyXZqLltH9181844 = SKyXZqLltH74193194;     SKyXZqLltH74193194 = SKyXZqLltH43297089;     SKyXZqLltH43297089 = SKyXZqLltH77013432;     SKyXZqLltH77013432 = SKyXZqLltH73164416;     SKyXZqLltH73164416 = SKyXZqLltH1119932;     SKyXZqLltH1119932 = SKyXZqLltH38454527;     SKyXZqLltH38454527 = SKyXZqLltH61794618;     SKyXZqLltH61794618 = SKyXZqLltH51641180;     SKyXZqLltH51641180 = SKyXZqLltH35999612;     SKyXZqLltH35999612 = SKyXZqLltH72000797;     SKyXZqLltH72000797 = SKyXZqLltH75776501;     SKyXZqLltH75776501 = SKyXZqLltH72050496;     SKyXZqLltH72050496 = SKyXZqLltH70980400;     SKyXZqLltH70980400 = SKyXZqLltH48752427;     SKyXZqLltH48752427 = SKyXZqLltH47394466;     SKyXZqLltH47394466 = SKyXZqLltH70916652;     SKyXZqLltH70916652 = SKyXZqLltH80733026;     SKyXZqLltH80733026 = SKyXZqLltH99496257;     SKyXZqLltH99496257 = SKyXZqLltH88682166;     SKyXZqLltH88682166 = SKyXZqLltH99278060;     SKyXZqLltH99278060 = SKyXZqLltH50784902;     SKyXZqLltH50784902 = SKyXZqLltH99939765;     SKyXZqLltH99939765 = SKyXZqLltH66516975;     SKyXZqLltH66516975 = SKyXZqLltH29483923;     SKyXZqLltH29483923 = SKyXZqLltH23144958;     SKyXZqLltH23144958 = SKyXZqLltH51276213;     SKyXZqLltH51276213 = SKyXZqLltH57486249;     SKyXZqLltH57486249 = SKyXZqLltH58371481;     SKyXZqLltH58371481 = SKyXZqLltH42461056;     SKyXZqLltH42461056 = SKyXZqLltH25386832;     SKyXZqLltH25386832 = SKyXZqLltH58515154;}
// Junk Finished
