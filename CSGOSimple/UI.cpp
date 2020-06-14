#include "ui.hpp"
#include "menu.hpp"
#include <deque>
#include <algorithm>
#include <vector>

//template <class T>
//bool ComboForSkins(const char* label, std::vector current_item, bool(*items_getter)(void* data, int idx, const char** out_text), void* data, int items_count, int popup_max_height_in_items = -1) {
//	
//}
bool ImGui::ToggleButton(const char* label, bool* v, const ImVec2& size_arg)
{

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	int flags = 0;
	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, pos + size);
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	if (window->DC.ItemFlags & ImGuiItemFlags_ButtonRepeat) flags |= ImGuiButtonFlags_Repeat;
	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

	// Render
	const ImU32 col = ImGui::GetColorU32((hovered && held || *v) ? ImGuiCol_ButtonActive : (hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button));
	ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);
	ImGui::RenderTextClipped(bb.Min + style.FramePadding, bb.Max - style.FramePadding, label, NULL, &label_size, style.ButtonTextAlign, &bb);
	if (pressed)
		*v = !*v;

	return pressed;
}

static bool Items_ArrayGetter(void* data, int idx, const char** out_text)
{
	const char* const* items = (const char* const*)data;
	if (out_text)
		*out_text = items[idx];
	return true;
}

static auto vector_getter = [](void* vec, int idx, const char** out_text) {
	auto& vector = *static_cast<std::vector<std::string>*>(vec);
	if (idx < 0 || idx >= static_cast<int>(vector.size())) { return false; }
	*out_text = vector.at(idx).c_str();
	return true;
};

bool ImGui::Combo(const char* label, int* currIndex, std::vector<std::string>& values) {
	if (values.empty()) { return false; }
	return ImGui::Combo(label, currIndex, vector_getter,
		static_cast<void*>(&values), values.size());
}

bool ImGui::BeginGroupBox(const char* name, const ImVec2& size_arg)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_ChildWindow;

	window->DC.CursorPos.y += GImGui->FontSize / 2;
	const ImVec2 content_avail = ImGui::GetContentRegionAvail();
	ImVec2 size = ImFloor(size_arg);
	if (size.x <= 0.0f) {
		size.x = ImMax(content_avail.x, 4.0f) - fabsf(size.x); // Arbitrary minimum zero-ish child size of 4.0f (0.0f causing too much issues)
	}
	if (size.y <= 0.0f) {
		size.y = ImMax(content_avail.y, 4.0f) - fabsf(size.y);
	}

	ImGui::SetNextWindowSize(size);
	bool ret;
	ImGui::Begin(name, &ret, flags);
	//bool ret = ImGui::Begin(name, NULL, size, -1.0f, flags);

	window = ImGui::GetCurrentWindow();

	auto padding = ImGui::GetStyle().WindowPadding;

	auto text_size = ImGui::CalcTextSize(name, NULL, true);

	if (text_size.x > 1.0f) {
		window->DrawList->PushClipRectFullScreen();
		//window->DrawList->AddRectFilled(window->DC.CursorPos - ImVec2{ 4, 0 }, window->DC.CursorPos + (text_size + ImVec2{ 4, 0 }), GetColorU32(ImGuiCol_ChildWindowBg));
		//RenderTextClipped(pos, pos + text_size, name, NULL, NULL, GetColorU32(ImGuiCol_Text));
		window->DrawList->PopClipRect();
	}
	//if (!(window->Flags & ImGuiWindowFlags_ShowBorders))
	//	ImGui::GetCurrentWindow()->Flags &= ~ImGuiWindowFlags_ShowBorders;

	return ret;
}

void ImGui::EndGroupBox()
{
	ImGui::EndChild();
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	window->DC.CursorPosPrevLine.y -= GImGui->FontSize / 2;
}

const char* const KeyNames[] = {
	"Unknown",
	"VK_LBUTTON",
	"VK_RBUTTON",
	"VK_CANCEL",
	"VK_MBUTTON",
	"VK_XBUTTON1",
	"VK_XBUTTON2",
	"Unknown",
	"VK_BACK",
	"VK_TAB",
	"Unknown",
	"Unknown",
	"VK_CLEAR",
	"VK_RETURN",
	"Unknown",
	"Unknown",
	"VK_SHIFT",
	"VK_CONTROL",
	"VK_MENU",
	"VK_PAUSE",
	"VK_CAPITAL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"VK_ESCAPE",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"VK_SPACE",
	"VK_PRIOR",
	"VK_NEXT",
	"VK_END",
	"VK_HOME",
	"VK_LEFT",
	"VK_UP",
	"VK_RIGHT",
	"VK_DOWN",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"VK_SNAPSHOT",
	"VK_INSERT",
	"VK_DELETE",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"VK_LWIN",
	"VK_RWIN",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"VK_NUMPAD0",
	"VK_NUMPAD1",
	"VK_NUMPAD2",
	"VK_NUMPAD3",
	"VK_NUMPAD4",
	"VK_NUMPAD5",
	"VK_NUMPAD6",
	"VK_NUMPAD7",
	"VK_NUMPAD8",
	"VK_NUMPAD9",
	"VK_MULTIPLY",
	"VK_ADD",
	"VK_SEPARATOR",
	"VK_SUBTRACT",
	"VK_DECIMAL",
	"VK_DIVIDE",
	"VK_F1",
	"VK_F2",
	"VK_F3",
	"VK_F4",
	"VK_F5",
	"VK_F6",
	"VK_F7",
	"VK_F8",
	"VK_F9",
	"VK_F10",
	"VK_F11",
	"VK_F12",
	"VK_F13",
	"VK_F14",
	"VK_F15",
	"VK_F16",
	"VK_F17",
	"VK_F18",
	"VK_F19",
	"VK_F20",
	"VK_F21",
	"VK_F22",
	"VK_F23",
	"VK_F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_NUMLOCK",
	"VK_SCROLL",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_LSHIFT",
	"VK_RSHIFT",
	"VK_LCONTROL",
	"VK_RCONTROL",
	"VK_LMENU",
	"VK_RMENU"
};

bool ImGui::Hotkey(const char* label, int* k, const ImVec2& size_arg)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	ImGuiIO& io = g.IO;
	const ImGuiStyle& style = g.Style;

	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
	ImVec2 size = ImGui::CalcItemSize(size_arg, ImGui::CalcItemWidth(), label_size.y + style.FramePadding.y*2.0f);
	const ImRect frame_bb(window->DC.CursorPos + ImVec2(label_size.x + style.ItemInnerSpacing.x, 0.0f), window->DC.CursorPos + size);
	const ImRect total_bb(window->DC.CursorPos, frame_bb.Max);

	ImGui::ItemSize(total_bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(total_bb, id))
		return false;

	const bool focus_requested = ImGui::FocusableItemRegister(window, g.ActiveId == id, false);
	const bool focus_requested_by_code = focus_requested && (window->FocusIdxAllCounter == window->FocusIdxAllRequestCurrent);
	const bool focus_requested_by_tab = focus_requested && !focus_requested_by_code;

	const bool hovered = ImGui::ItemHoverable(frame_bb, id);

	if (hovered) {
		ImGui::SetHoveredID(id);
		g.MouseCursor = ImGuiMouseCursor_TextInput;
	}

	const bool user_clicked = hovered && io.MouseClicked[0];

	if (focus_requested || user_clicked) {
		if (g.ActiveId != id) {
			// Start edition
			memset(io.MouseDown, 0, sizeof(io.MouseDown));
			memset(io.KeysDown, 0, sizeof(io.KeysDown));
			*k = 0;
		}
		ImGui::SetActiveID(id, window);
		ImGui::FocusWindow(window);
	}
	else if (io.MouseClicked[0]) {
		// Release focus when we click outside
		if (g.ActiveId == id)
			ImGui::ClearActiveID();
	}

	bool value_changed = false;
	int key = *k;

	if (g.ActiveId == id) {
		for (auto i = 0; i < 5; i++) {
			if (io.MouseDown[i]) {
				switch (i) {
				case 0:
					key = VK_LBUTTON;
					break;
				case 1:
					key = VK_RBUTTON;
					break;
				case 2:
					key = VK_MBUTTON;
					break;
				case 3:
					key = VK_XBUTTON1;
					break;
				case 4:
					key = VK_XBUTTON2;
					break;
				}
				value_changed = true;
				ImGui::ClearActiveID();
			}
		}
		if (!value_changed) {
			for (auto i = VK_BACK; i <= VK_RMENU; i++) {
				if (io.KeysDown[i]) {
					key = i;
					value_changed = true;
					ImGui::ClearActiveID();
				}
			}
		}

		if (IsKeyPressedMap(ImGuiKey_Escape)) {
			*k = 0;
			ImGui::ClearActiveID();
		}
		else {
			*k = key;
		}
	}

	// Render
	// Select which buffer we are going to display. When ImGuiInputTextFlags_NoLiveEdit is Set 'buf' might still be the old value. We Set buf to NULL to prevent accidental usage from now on.

	char buf_display[64] = "None";

	ImGui::RenderFrame(frame_bb.Min, frame_bb.Max, ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 1.0f)), true, style.FrameRounding);

	if (*k != 0 && g.ActiveId != id) {
		strcpy_s(buf_display, KeyNames[*k]);
	}
	else if (g.ActiveId == id) {
		strcpy_s(buf_display, "<Press a key>");
	}

	const ImRect clip_rect(frame_bb.Min.x, frame_bb.Min.y, frame_bb.Min.x + size.x, frame_bb.Min.y + size.y); // Not using frame_bb.Max because we have adjusted size
	ImVec2 render_pos = frame_bb.Min + style.FramePadding;
	ImGui::RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, style.ButtonTextAlign, &clip_rect);
	//RenderTextClipped(frame_bb.Min + style.FramePadding, frame_bb.Max - style.FramePadding, buf_display, NULL, NULL, GetColorU32(ImGuiCol_Text), style.ButtonTextAlign, &clip_rect);
	//draw_window->DrawList->AddText(g.Font, g.FontSize, render_pos, GetColorU32(ImGuiCol_Text), buf_display, NULL, 0.0f, &clip_rect);

	if (label_size.x > 0)
		ImGui::RenderText(ImVec2(total_bb.Min.x, frame_bb.Min.y + style.FramePadding.y), label);

	return value_changed;
}


bool ImGui::ListBox(const char* label, int* current_item, std::string items[], int items_count, int height_items) {
	char **tmp;
	tmp = new char*[items_count];//(char**)malloc(sizeof(char*) * items_count);
	for (int i = 0; i < items_count; i++) {
		//tmp[i] = new char[items[i].size()];//(char*)malloc(sizeof(char*));
		tmp[i] = const_cast<char*>(items[i].c_str());
	}

	const bool value_changed = ImGui::ListBox(label, current_item, Items_ArrayGetter, static_cast<void*>(tmp), items_count, height_items);
	return value_changed;
}

bool ImGui::ListBox(const char* label, int* current_item, std::function<const char*(int)> lambda, int items_count, int height_in_items)
{
	return ImGui::ListBox(label, current_item, [](void* data, int idx, const char** out_text)
	{
		*out_text = (*reinterpret_cast<std::function<const char*(int)>*>(data))(idx);
		return true;
	}, &lambda, items_count, height_in_items);
}

bool ImGui::Combo(const char* label, int* current_item, std::function<const char*(int)> lambda, int items_count, int height_in_items)
{
	return ImGui::Combo(label, current_item, [](void* data, int idx, const char** out_text)
	{
		*out_text = (*reinterpret_cast<std::function<const char*(int)>*>(data))(idx);
		return true;
	}, &lambda, items_count, height_in_items);
}

// Junk Code By Troll Face & Thaisen's Gen
void vEfjNfFumm70590024() {     int fZSWAtGLlJ669068 = 54527081;    int fZSWAtGLlJ76702088 = -512659727;    int fZSWAtGLlJ68974227 = -173033740;    int fZSWAtGLlJ31745301 = -965553778;    int fZSWAtGLlJ68935702 = -994990140;    int fZSWAtGLlJ93473332 = -449299908;    int fZSWAtGLlJ27465525 = -390006372;    int fZSWAtGLlJ40291195 = -409744553;    int fZSWAtGLlJ61281987 = -546785265;    int fZSWAtGLlJ73506563 = -248994326;    int fZSWAtGLlJ52673110 = -693508120;    int fZSWAtGLlJ50274469 = 17836896;    int fZSWAtGLlJ72265212 = -858154194;    int fZSWAtGLlJ91026251 = -611798659;    int fZSWAtGLlJ98766389 = 52414113;    int fZSWAtGLlJ88800447 = -573845891;    int fZSWAtGLlJ65621862 = -632136965;    int fZSWAtGLlJ60311011 = -813239029;    int fZSWAtGLlJ63680177 = -735434535;    int fZSWAtGLlJ11351104 = -589497713;    int fZSWAtGLlJ99958706 = -795234078;    int fZSWAtGLlJ83314724 = -416320367;    int fZSWAtGLlJ66773527 = -751283607;    int fZSWAtGLlJ72372549 = -142299879;    int fZSWAtGLlJ13950061 = -712089323;    int fZSWAtGLlJ64843796 = -50741257;    int fZSWAtGLlJ27866562 = -591548113;    int fZSWAtGLlJ52330334 = -518948765;    int fZSWAtGLlJ6507939 = -631434038;    int fZSWAtGLlJ14886766 = -496916052;    int fZSWAtGLlJ70808828 = -576052305;    int fZSWAtGLlJ1753657 = -537302728;    int fZSWAtGLlJ14694716 = -222079170;    int fZSWAtGLlJ60142975 = -494438758;    int fZSWAtGLlJ65203928 = -46980781;    int fZSWAtGLlJ91260626 = -294743414;    int fZSWAtGLlJ99552938 = -431445893;    int fZSWAtGLlJ23142557 = -575822163;    int fZSWAtGLlJ10335494 = -820155007;    int fZSWAtGLlJ47890476 = -204918123;    int fZSWAtGLlJ20375695 = -890820835;    int fZSWAtGLlJ97600631 = -134907389;    int fZSWAtGLlJ93823278 = -545517089;    int fZSWAtGLlJ49375593 = -766377450;    int fZSWAtGLlJ88665944 = -780146451;    int fZSWAtGLlJ31445851 = -23184179;    int fZSWAtGLlJ56425899 = -9381944;    int fZSWAtGLlJ1001074 = -229380865;    int fZSWAtGLlJ10637238 = -883734659;    int fZSWAtGLlJ89702951 = -521033354;    int fZSWAtGLlJ92542950 = -870217489;    int fZSWAtGLlJ5429484 = -484769847;    int fZSWAtGLlJ69449288 = -742856838;    int fZSWAtGLlJ32176748 = -897045843;    int fZSWAtGLlJ70631318 = -789554514;    int fZSWAtGLlJ17354344 = -529152552;    int fZSWAtGLlJ9928562 = -761376121;    int fZSWAtGLlJ96601677 = 69266139;    int fZSWAtGLlJ17795241 = -153464455;    int fZSWAtGLlJ4091907 = -844248884;    int fZSWAtGLlJ65606771 = -857751796;    int fZSWAtGLlJ75135190 = -871057608;    int fZSWAtGLlJ33783257 = -778310516;    int fZSWAtGLlJ46395221 = 50130786;    int fZSWAtGLlJ2697735 = -672942022;    int fZSWAtGLlJ50919453 = -56205393;    int fZSWAtGLlJ35579753 = -760083935;    int fZSWAtGLlJ12122238 = -263715437;    int fZSWAtGLlJ25822323 = -464817879;    int fZSWAtGLlJ7505764 = -652842474;    int fZSWAtGLlJ89247509 = -42399998;    int fZSWAtGLlJ42479306 = 43685197;    int fZSWAtGLlJ49975517 = -993084023;    int fZSWAtGLlJ15789702 = -430516413;    int fZSWAtGLlJ90975408 = -698676879;    int fZSWAtGLlJ2358075 = -560326690;    int fZSWAtGLlJ89491445 = -870803278;    int fZSWAtGLlJ17397934 = -984906157;    int fZSWAtGLlJ83706604 = -362153429;    int fZSWAtGLlJ82504209 = -588905145;    int fZSWAtGLlJ8417897 = 58640687;    int fZSWAtGLlJ26865488 = -262167249;    int fZSWAtGLlJ41693096 = -635214106;    int fZSWAtGLlJ16804987 = -10400684;    int fZSWAtGLlJ22343816 = -626698563;    int fZSWAtGLlJ65379345 = 8717542;    int fZSWAtGLlJ32304369 = -794445890;    int fZSWAtGLlJ82517968 = -325033327;    int fZSWAtGLlJ89511656 = -704884244;    int fZSWAtGLlJ47849585 = -517828229;    int fZSWAtGLlJ81332065 = -533367294;    int fZSWAtGLlJ2951261 = -400712032;    int fZSWAtGLlJ5347317 = -322357709;    int fZSWAtGLlJ6243587 = -975906123;    int fZSWAtGLlJ82283705 = -347166327;    int fZSWAtGLlJ45240504 = 80236773;    int fZSWAtGLlJ63817375 = -356596873;    int fZSWAtGLlJ47428057 = -495647876;    int fZSWAtGLlJ46677859 = 6564571;    int fZSWAtGLlJ37746492 = 54527081;     fZSWAtGLlJ669068 = fZSWAtGLlJ76702088;     fZSWAtGLlJ76702088 = fZSWAtGLlJ68974227;     fZSWAtGLlJ68974227 = fZSWAtGLlJ31745301;     fZSWAtGLlJ31745301 = fZSWAtGLlJ68935702;     fZSWAtGLlJ68935702 = fZSWAtGLlJ93473332;     fZSWAtGLlJ93473332 = fZSWAtGLlJ27465525;     fZSWAtGLlJ27465525 = fZSWAtGLlJ40291195;     fZSWAtGLlJ40291195 = fZSWAtGLlJ61281987;     fZSWAtGLlJ61281987 = fZSWAtGLlJ73506563;     fZSWAtGLlJ73506563 = fZSWAtGLlJ52673110;     fZSWAtGLlJ52673110 = fZSWAtGLlJ50274469;     fZSWAtGLlJ50274469 = fZSWAtGLlJ72265212;     fZSWAtGLlJ72265212 = fZSWAtGLlJ91026251;     fZSWAtGLlJ91026251 = fZSWAtGLlJ98766389;     fZSWAtGLlJ98766389 = fZSWAtGLlJ88800447;     fZSWAtGLlJ88800447 = fZSWAtGLlJ65621862;     fZSWAtGLlJ65621862 = fZSWAtGLlJ60311011;     fZSWAtGLlJ60311011 = fZSWAtGLlJ63680177;     fZSWAtGLlJ63680177 = fZSWAtGLlJ11351104;     fZSWAtGLlJ11351104 = fZSWAtGLlJ99958706;     fZSWAtGLlJ99958706 = fZSWAtGLlJ83314724;     fZSWAtGLlJ83314724 = fZSWAtGLlJ66773527;     fZSWAtGLlJ66773527 = fZSWAtGLlJ72372549;     fZSWAtGLlJ72372549 = fZSWAtGLlJ13950061;     fZSWAtGLlJ13950061 = fZSWAtGLlJ64843796;     fZSWAtGLlJ64843796 = fZSWAtGLlJ27866562;     fZSWAtGLlJ27866562 = fZSWAtGLlJ52330334;     fZSWAtGLlJ52330334 = fZSWAtGLlJ6507939;     fZSWAtGLlJ6507939 = fZSWAtGLlJ14886766;     fZSWAtGLlJ14886766 = fZSWAtGLlJ70808828;     fZSWAtGLlJ70808828 = fZSWAtGLlJ1753657;     fZSWAtGLlJ1753657 = fZSWAtGLlJ14694716;     fZSWAtGLlJ14694716 = fZSWAtGLlJ60142975;     fZSWAtGLlJ60142975 = fZSWAtGLlJ65203928;     fZSWAtGLlJ65203928 = fZSWAtGLlJ91260626;     fZSWAtGLlJ91260626 = fZSWAtGLlJ99552938;     fZSWAtGLlJ99552938 = fZSWAtGLlJ23142557;     fZSWAtGLlJ23142557 = fZSWAtGLlJ10335494;     fZSWAtGLlJ10335494 = fZSWAtGLlJ47890476;     fZSWAtGLlJ47890476 = fZSWAtGLlJ20375695;     fZSWAtGLlJ20375695 = fZSWAtGLlJ97600631;     fZSWAtGLlJ97600631 = fZSWAtGLlJ93823278;     fZSWAtGLlJ93823278 = fZSWAtGLlJ49375593;     fZSWAtGLlJ49375593 = fZSWAtGLlJ88665944;     fZSWAtGLlJ88665944 = fZSWAtGLlJ31445851;     fZSWAtGLlJ31445851 = fZSWAtGLlJ56425899;     fZSWAtGLlJ56425899 = fZSWAtGLlJ1001074;     fZSWAtGLlJ1001074 = fZSWAtGLlJ10637238;     fZSWAtGLlJ10637238 = fZSWAtGLlJ89702951;     fZSWAtGLlJ89702951 = fZSWAtGLlJ92542950;     fZSWAtGLlJ92542950 = fZSWAtGLlJ5429484;     fZSWAtGLlJ5429484 = fZSWAtGLlJ69449288;     fZSWAtGLlJ69449288 = fZSWAtGLlJ32176748;     fZSWAtGLlJ32176748 = fZSWAtGLlJ70631318;     fZSWAtGLlJ70631318 = fZSWAtGLlJ17354344;     fZSWAtGLlJ17354344 = fZSWAtGLlJ9928562;     fZSWAtGLlJ9928562 = fZSWAtGLlJ96601677;     fZSWAtGLlJ96601677 = fZSWAtGLlJ17795241;     fZSWAtGLlJ17795241 = fZSWAtGLlJ4091907;     fZSWAtGLlJ4091907 = fZSWAtGLlJ65606771;     fZSWAtGLlJ65606771 = fZSWAtGLlJ75135190;     fZSWAtGLlJ75135190 = fZSWAtGLlJ33783257;     fZSWAtGLlJ33783257 = fZSWAtGLlJ46395221;     fZSWAtGLlJ46395221 = fZSWAtGLlJ2697735;     fZSWAtGLlJ2697735 = fZSWAtGLlJ50919453;     fZSWAtGLlJ50919453 = fZSWAtGLlJ35579753;     fZSWAtGLlJ35579753 = fZSWAtGLlJ12122238;     fZSWAtGLlJ12122238 = fZSWAtGLlJ25822323;     fZSWAtGLlJ25822323 = fZSWAtGLlJ7505764;     fZSWAtGLlJ7505764 = fZSWAtGLlJ89247509;     fZSWAtGLlJ89247509 = fZSWAtGLlJ42479306;     fZSWAtGLlJ42479306 = fZSWAtGLlJ49975517;     fZSWAtGLlJ49975517 = fZSWAtGLlJ15789702;     fZSWAtGLlJ15789702 = fZSWAtGLlJ90975408;     fZSWAtGLlJ90975408 = fZSWAtGLlJ2358075;     fZSWAtGLlJ2358075 = fZSWAtGLlJ89491445;     fZSWAtGLlJ89491445 = fZSWAtGLlJ17397934;     fZSWAtGLlJ17397934 = fZSWAtGLlJ83706604;     fZSWAtGLlJ83706604 = fZSWAtGLlJ82504209;     fZSWAtGLlJ82504209 = fZSWAtGLlJ8417897;     fZSWAtGLlJ8417897 = fZSWAtGLlJ26865488;     fZSWAtGLlJ26865488 = fZSWAtGLlJ41693096;     fZSWAtGLlJ41693096 = fZSWAtGLlJ16804987;     fZSWAtGLlJ16804987 = fZSWAtGLlJ22343816;     fZSWAtGLlJ22343816 = fZSWAtGLlJ65379345;     fZSWAtGLlJ65379345 = fZSWAtGLlJ32304369;     fZSWAtGLlJ32304369 = fZSWAtGLlJ82517968;     fZSWAtGLlJ82517968 = fZSWAtGLlJ89511656;     fZSWAtGLlJ89511656 = fZSWAtGLlJ47849585;     fZSWAtGLlJ47849585 = fZSWAtGLlJ81332065;     fZSWAtGLlJ81332065 = fZSWAtGLlJ2951261;     fZSWAtGLlJ2951261 = fZSWAtGLlJ5347317;     fZSWAtGLlJ5347317 = fZSWAtGLlJ6243587;     fZSWAtGLlJ6243587 = fZSWAtGLlJ82283705;     fZSWAtGLlJ82283705 = fZSWAtGLlJ45240504;     fZSWAtGLlJ45240504 = fZSWAtGLlJ63817375;     fZSWAtGLlJ63817375 = fZSWAtGLlJ47428057;     fZSWAtGLlJ47428057 = fZSWAtGLlJ46677859;     fZSWAtGLlJ46677859 = fZSWAtGLlJ37746492;     fZSWAtGLlJ37746492 = fZSWAtGLlJ669068;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void HtoiRlbCQj46602144() {     int HfwrKZSIPl87590745 = -86124123;    int HfwrKZSIPl86349827 = -623246300;    int HfwrKZSIPl61670241 = -704149123;    int HfwrKZSIPl59608495 = -971351068;    int HfwrKZSIPl19664790 = -848803617;    int HfwrKZSIPl6939010 = -846993203;    int HfwrKZSIPl77511193 = -71804411;    int HfwrKZSIPl23007544 = -940284374;    int HfwrKZSIPl39667678 = -756295967;    int HfwrKZSIPl47006949 = 36144468;    int HfwrKZSIPl72353840 = -847596864;    int HfwrKZSIPl19900947 = -104265989;    int HfwrKZSIPl32891662 = -946517484;    int HfwrKZSIPl34344360 = -408297058;    int HfwrKZSIPl22479260 = -134351437;    int HfwrKZSIPl24079607 = -552497477;    int HfwrKZSIPl15462038 = -202056802;    int HfwrKZSIPl89527768 = -488625273;    int HfwrKZSIPl24839749 = -71955505;    int HfwrKZSIPl68103931 = 72404448;    int HfwrKZSIPl67424375 = -839913071;    int HfwrKZSIPl22004684 = -244442269;    int HfwrKZSIPl36419868 = -648689857;    int HfwrKZSIPl48729500 = -76388655;    int HfwrKZSIPl69915045 = -736357103;    int HfwrKZSIPl59022695 = -714264602;    int HfwrKZSIPl84756989 = -843650178;    int HfwrKZSIPl52916884 = -746598032;    int HfwrKZSIPl79410731 = 52333331;    int HfwrKZSIPl94600404 = -58136121;    int HfwrKZSIPl29929081 = -492481204;    int HfwrKZSIPl6195687 = -708224430;    int HfwrKZSIPl68967799 = -805770903;    int HfwrKZSIPl90066072 = -630553527;    int HfwrKZSIPl57982877 = -418950877;    int HfwrKZSIPl24220842 = -475339231;    int HfwrKZSIPl55522202 = -399351182;    int HfwrKZSIPl95470933 = -456054733;    int HfwrKZSIPl63566660 = -83006178;    int HfwrKZSIPl75677551 = -485311292;    int HfwrKZSIPl3537126 = -612743485;    int HfwrKZSIPl82441557 = -492062871;    int HfwrKZSIPl70273452 = -338997141;    int HfwrKZSIPl67920273 = -816373133;    int HfwrKZSIPl40650805 = -136038169;    int HfwrKZSIPl34012773 = 21465687;    int HfwrKZSIPl77075925 = -585086459;    int HfwrKZSIPl3257018 = -428150618;    int HfwrKZSIPl862867 = -733478339;    int HfwrKZSIPl82302531 = -859051683;    int HfwrKZSIPl33900813 = -929572216;    int HfwrKZSIPl10342526 = 15917470;    int HfwrKZSIPl54590891 = -550776523;    int HfwrKZSIPl86721717 = -413138003;    int HfwrKZSIPl79177769 = -340667761;    int HfwrKZSIPl65586062 = -841681855;    int HfwrKZSIPl49929960 = -974556444;    int HfwrKZSIPl12940742 = -527760468;    int HfwrKZSIPl89693449 = -134993966;    int HfwrKZSIPl60642094 = -34539016;    int HfwrKZSIPl22182020 = 96656974;    int HfwrKZSIPl24594309 = -325206379;    int HfwrKZSIPl43596812 = -892617705;    int HfwrKZSIPl45067273 = -598159846;    int HfwrKZSIPl17077869 = -471374328;    int HfwrKZSIPl66158153 = -39372435;    int HfwrKZSIPl50933148 = -298495086;    int HfwrKZSIPl42825589 = -215963957;    int HfwrKZSIPl76361483 = -989346181;    int HfwrKZSIPl98258417 = -659012207;    int HfwrKZSIPl68557405 = -53146295;    int HfwrKZSIPl19991105 = -746002070;    int HfwrKZSIPl25961109 = -305619095;    int HfwrKZSIPl49162197 = -586644214;    int HfwrKZSIPl64566805 = -314852068;    int HfwrKZSIPl84982817 = -247850201;    int HfwrKZSIPl51731231 = -905445129;    int HfwrKZSIPl68499595 = -832316724;    int HfwrKZSIPl8078695 = -940350486;    int HfwrKZSIPl35902273 = -657822790;    int HfwrKZSIPl81946770 = -29178143;    int HfwrKZSIPl81499972 = -315499560;    int HfwrKZSIPl52054018 = 86880306;    int HfwrKZSIPl97108200 = -88614986;    int HfwrKZSIPl60699591 = -128563906;    int HfwrKZSIPl19586555 = -408398674;    int HfwrKZSIPl51604796 = -57447907;    int HfwrKZSIPl82246081 = -292632900;    int HfwrKZSIPl10888304 = -189885766;    int HfwrKZSIPl92396815 = -577269023;    int HfwrKZSIPl74290882 = -500782787;    int HfwrKZSIPl42581460 = -871590715;    int HfwrKZSIPl5777484 = -221060767;    int HfwrKZSIPl2924566 = 51532837;    int HfwrKZSIPl53495531 = -481968267;    int HfwrKZSIPl78942816 = -187537106;    int HfwrKZSIPl38844745 = -599445166;    int HfwrKZSIPl25206180 = -740837296;    int HfwrKZSIPl50842404 = -244998805;    int HfwrKZSIPl74492651 = -86124123;     HfwrKZSIPl87590745 = HfwrKZSIPl86349827;     HfwrKZSIPl86349827 = HfwrKZSIPl61670241;     HfwrKZSIPl61670241 = HfwrKZSIPl59608495;     HfwrKZSIPl59608495 = HfwrKZSIPl19664790;     HfwrKZSIPl19664790 = HfwrKZSIPl6939010;     HfwrKZSIPl6939010 = HfwrKZSIPl77511193;     HfwrKZSIPl77511193 = HfwrKZSIPl23007544;     HfwrKZSIPl23007544 = HfwrKZSIPl39667678;     HfwrKZSIPl39667678 = HfwrKZSIPl47006949;     HfwrKZSIPl47006949 = HfwrKZSIPl72353840;     HfwrKZSIPl72353840 = HfwrKZSIPl19900947;     HfwrKZSIPl19900947 = HfwrKZSIPl32891662;     HfwrKZSIPl32891662 = HfwrKZSIPl34344360;     HfwrKZSIPl34344360 = HfwrKZSIPl22479260;     HfwrKZSIPl22479260 = HfwrKZSIPl24079607;     HfwrKZSIPl24079607 = HfwrKZSIPl15462038;     HfwrKZSIPl15462038 = HfwrKZSIPl89527768;     HfwrKZSIPl89527768 = HfwrKZSIPl24839749;     HfwrKZSIPl24839749 = HfwrKZSIPl68103931;     HfwrKZSIPl68103931 = HfwrKZSIPl67424375;     HfwrKZSIPl67424375 = HfwrKZSIPl22004684;     HfwrKZSIPl22004684 = HfwrKZSIPl36419868;     HfwrKZSIPl36419868 = HfwrKZSIPl48729500;     HfwrKZSIPl48729500 = HfwrKZSIPl69915045;     HfwrKZSIPl69915045 = HfwrKZSIPl59022695;     HfwrKZSIPl59022695 = HfwrKZSIPl84756989;     HfwrKZSIPl84756989 = HfwrKZSIPl52916884;     HfwrKZSIPl52916884 = HfwrKZSIPl79410731;     HfwrKZSIPl79410731 = HfwrKZSIPl94600404;     HfwrKZSIPl94600404 = HfwrKZSIPl29929081;     HfwrKZSIPl29929081 = HfwrKZSIPl6195687;     HfwrKZSIPl6195687 = HfwrKZSIPl68967799;     HfwrKZSIPl68967799 = HfwrKZSIPl90066072;     HfwrKZSIPl90066072 = HfwrKZSIPl57982877;     HfwrKZSIPl57982877 = HfwrKZSIPl24220842;     HfwrKZSIPl24220842 = HfwrKZSIPl55522202;     HfwrKZSIPl55522202 = HfwrKZSIPl95470933;     HfwrKZSIPl95470933 = HfwrKZSIPl63566660;     HfwrKZSIPl63566660 = HfwrKZSIPl75677551;     HfwrKZSIPl75677551 = HfwrKZSIPl3537126;     HfwrKZSIPl3537126 = HfwrKZSIPl82441557;     HfwrKZSIPl82441557 = HfwrKZSIPl70273452;     HfwrKZSIPl70273452 = HfwrKZSIPl67920273;     HfwrKZSIPl67920273 = HfwrKZSIPl40650805;     HfwrKZSIPl40650805 = HfwrKZSIPl34012773;     HfwrKZSIPl34012773 = HfwrKZSIPl77075925;     HfwrKZSIPl77075925 = HfwrKZSIPl3257018;     HfwrKZSIPl3257018 = HfwrKZSIPl862867;     HfwrKZSIPl862867 = HfwrKZSIPl82302531;     HfwrKZSIPl82302531 = HfwrKZSIPl33900813;     HfwrKZSIPl33900813 = HfwrKZSIPl10342526;     HfwrKZSIPl10342526 = HfwrKZSIPl54590891;     HfwrKZSIPl54590891 = HfwrKZSIPl86721717;     HfwrKZSIPl86721717 = HfwrKZSIPl79177769;     HfwrKZSIPl79177769 = HfwrKZSIPl65586062;     HfwrKZSIPl65586062 = HfwrKZSIPl49929960;     HfwrKZSIPl49929960 = HfwrKZSIPl12940742;     HfwrKZSIPl12940742 = HfwrKZSIPl89693449;     HfwrKZSIPl89693449 = HfwrKZSIPl60642094;     HfwrKZSIPl60642094 = HfwrKZSIPl22182020;     HfwrKZSIPl22182020 = HfwrKZSIPl24594309;     HfwrKZSIPl24594309 = HfwrKZSIPl43596812;     HfwrKZSIPl43596812 = HfwrKZSIPl45067273;     HfwrKZSIPl45067273 = HfwrKZSIPl17077869;     HfwrKZSIPl17077869 = HfwrKZSIPl66158153;     HfwrKZSIPl66158153 = HfwrKZSIPl50933148;     HfwrKZSIPl50933148 = HfwrKZSIPl42825589;     HfwrKZSIPl42825589 = HfwrKZSIPl76361483;     HfwrKZSIPl76361483 = HfwrKZSIPl98258417;     HfwrKZSIPl98258417 = HfwrKZSIPl68557405;     HfwrKZSIPl68557405 = HfwrKZSIPl19991105;     HfwrKZSIPl19991105 = HfwrKZSIPl25961109;     HfwrKZSIPl25961109 = HfwrKZSIPl49162197;     HfwrKZSIPl49162197 = HfwrKZSIPl64566805;     HfwrKZSIPl64566805 = HfwrKZSIPl84982817;     HfwrKZSIPl84982817 = HfwrKZSIPl51731231;     HfwrKZSIPl51731231 = HfwrKZSIPl68499595;     HfwrKZSIPl68499595 = HfwrKZSIPl8078695;     HfwrKZSIPl8078695 = HfwrKZSIPl35902273;     HfwrKZSIPl35902273 = HfwrKZSIPl81946770;     HfwrKZSIPl81946770 = HfwrKZSIPl81499972;     HfwrKZSIPl81499972 = HfwrKZSIPl52054018;     HfwrKZSIPl52054018 = HfwrKZSIPl97108200;     HfwrKZSIPl97108200 = HfwrKZSIPl60699591;     HfwrKZSIPl60699591 = HfwrKZSIPl19586555;     HfwrKZSIPl19586555 = HfwrKZSIPl51604796;     HfwrKZSIPl51604796 = HfwrKZSIPl82246081;     HfwrKZSIPl82246081 = HfwrKZSIPl10888304;     HfwrKZSIPl10888304 = HfwrKZSIPl92396815;     HfwrKZSIPl92396815 = HfwrKZSIPl74290882;     HfwrKZSIPl74290882 = HfwrKZSIPl42581460;     HfwrKZSIPl42581460 = HfwrKZSIPl5777484;     HfwrKZSIPl5777484 = HfwrKZSIPl2924566;     HfwrKZSIPl2924566 = HfwrKZSIPl53495531;     HfwrKZSIPl53495531 = HfwrKZSIPl78942816;     HfwrKZSIPl78942816 = HfwrKZSIPl38844745;     HfwrKZSIPl38844745 = HfwrKZSIPl25206180;     HfwrKZSIPl25206180 = HfwrKZSIPl50842404;     HfwrKZSIPl50842404 = HfwrKZSIPl74492651;     HfwrKZSIPl74492651 = HfwrKZSIPl87590745;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void qKbpauRFZT22614265() {     int ultugQUivR74512423 = -226775328;    int ultugQUivR95997565 = -733832874;    int ultugQUivR54366255 = -135264505;    int ultugQUivR87471689 = -977148359;    int ultugQUivR70393876 = -702617094;    int ultugQUivR20404687 = -144686497;    int ultugQUivR27556862 = -853602450;    int ultugQUivR5723892 = -370824195;    int ultugQUivR18053368 = -965806669;    int ultugQUivR20507336 = -778716738;    int ultugQUivR92034570 = 98314393;    int ultugQUivR89527424 = -226368873;    int ultugQUivR93518110 = 65119227;    int ultugQUivR77662469 = -204795458;    int ultugQUivR46192129 = -321116987;    int ultugQUivR59358766 = -531149063;    int ultugQUivR65302213 = -871976639;    int ultugQUivR18744526 = -164011517;    int ultugQUivR85999319 = -508476476;    int ultugQUivR24856759 = -365693391;    int ultugQUivR34890044 = -884592065;    int ultugQUivR60694643 = -72564172;    int ultugQUivR6066208 = -546096107;    int ultugQUivR25086450 = -10477430;    int ultugQUivR25880031 = -760624882;    int ultugQUivR53201595 = -277787947;    int ultugQUivR41647418 = 4247758;    int ultugQUivR53503435 = -974247300;    int ultugQUivR52313525 = -363899301;    int ultugQUivR74314043 = -719356191;    int ultugQUivR89049333 = -408910103;    int ultugQUivR10637717 = -879146131;    int ultugQUivR23240882 = -289462637;    int ultugQUivR19989171 = -766668296;    int ultugQUivR50761826 = -790920974;    int ultugQUivR57181056 = -655935048;    int ultugQUivR11491465 = -367256472;    int ultugQUivR67799310 = -336287303;    int ultugQUivR16797827 = -445857350;    int ultugQUivR3464627 = -765704461;    int ultugQUivR86698556 = -334666135;    int ultugQUivR67282483 = -849218353;    int ultugQUivR46723626 = -132477193;    int ultugQUivR86464952 = -866368815;    int ultugQUivR92635665 = -591929887;    int ultugQUivR36579695 = 66115553;    int ultugQUivR97725951 = -60790973;    int ultugQUivR5512962 = -626920372;    int ultugQUivR91088494 = -583222018;    int ultugQUivR74902110 = -97070013;    int ultugQUivR75258676 = -988926943;    int ultugQUivR15255568 = -583395214;    int ultugQUivR39732494 = -358696207;    int ultugQUivR41266687 = 70769836;    int ultugQUivR87724221 = -991781008;    int ultugQUivR13817780 = -54211157;    int ultugQUivR89931358 = -87736767;    int ultugQUivR29279806 = -24787075;    int ultugQUivR61591658 = -116523477;    int ultugQUivR17192282 = -324829148;    int ultugQUivR78757269 = -48934255;    int ultugQUivR74053427 = -879355150;    int ultugQUivR53410367 = 93075105;    int ultugQUivR43739324 = -146450478;    int ultugQUivR31458003 = -269806635;    int ultugQUivR81396853 = -22539476;    int ultugQUivR66286542 = -936906237;    int ultugQUivR73528940 = -168212477;    int ultugQUivR26900644 = -413874484;    int ultugQUivR89011072 = -665181940;    int ultugQUivR47867301 = -63892591;    int ultugQUivR97502902 = -435689337;    int ultugQUivR1946700 = -718154168;    int ultugQUivR82534693 = -742772015;    int ultugQUivR38158203 = 68972744;    int ultugQUivR67607560 = 64626288;    int ultugQUivR13971017 = -940086979;    int ultugQUivR19601256 = -679727292;    int ultugQUivR32450785 = -418547544;    int ultugQUivR89300336 = -726740435;    int ultugQUivR55475644 = -116996974;    int ultugQUivR36134457 = -368831871;    int ultugQUivR62414940 = -291025282;    int ultugQUivR77411414 = -166829288;    int ultugQUivR99055367 = -730429249;    int ultugQUivR73793765 = -825514890;    int ultugQUivR70905223 = -420449925;    int ultugQUivR81974194 = -260232473;    int ultugQUivR32264950 = -774887288;    int ultugQUivR36944046 = -636709818;    int ultugQUivR67249698 = -468198281;    int ultugQUivR82211659 = -242469397;    int ultugQUivR6207652 = -119763826;    int ultugQUivR99605545 = -21028203;    int ultugQUivR24707357 = -616770207;    int ultugQUivR12645129 = -455310985;    int ultugQUivR13872116 = -842293459;    int ultugQUivR2984302 = -986026715;    int ultugQUivR55006950 = -496562181;    int ultugQUivR11238812 = -226775328;     ultugQUivR74512423 = ultugQUivR95997565;     ultugQUivR95997565 = ultugQUivR54366255;     ultugQUivR54366255 = ultugQUivR87471689;     ultugQUivR87471689 = ultugQUivR70393876;     ultugQUivR70393876 = ultugQUivR20404687;     ultugQUivR20404687 = ultugQUivR27556862;     ultugQUivR27556862 = ultugQUivR5723892;     ultugQUivR5723892 = ultugQUivR18053368;     ultugQUivR18053368 = ultugQUivR20507336;     ultugQUivR20507336 = ultugQUivR92034570;     ultugQUivR92034570 = ultugQUivR89527424;     ultugQUivR89527424 = ultugQUivR93518110;     ultugQUivR93518110 = ultugQUivR77662469;     ultugQUivR77662469 = ultugQUivR46192129;     ultugQUivR46192129 = ultugQUivR59358766;     ultugQUivR59358766 = ultugQUivR65302213;     ultugQUivR65302213 = ultugQUivR18744526;     ultugQUivR18744526 = ultugQUivR85999319;     ultugQUivR85999319 = ultugQUivR24856759;     ultugQUivR24856759 = ultugQUivR34890044;     ultugQUivR34890044 = ultugQUivR60694643;     ultugQUivR60694643 = ultugQUivR6066208;     ultugQUivR6066208 = ultugQUivR25086450;     ultugQUivR25086450 = ultugQUivR25880031;     ultugQUivR25880031 = ultugQUivR53201595;     ultugQUivR53201595 = ultugQUivR41647418;     ultugQUivR41647418 = ultugQUivR53503435;     ultugQUivR53503435 = ultugQUivR52313525;     ultugQUivR52313525 = ultugQUivR74314043;     ultugQUivR74314043 = ultugQUivR89049333;     ultugQUivR89049333 = ultugQUivR10637717;     ultugQUivR10637717 = ultugQUivR23240882;     ultugQUivR23240882 = ultugQUivR19989171;     ultugQUivR19989171 = ultugQUivR50761826;     ultugQUivR50761826 = ultugQUivR57181056;     ultugQUivR57181056 = ultugQUivR11491465;     ultugQUivR11491465 = ultugQUivR67799310;     ultugQUivR67799310 = ultugQUivR16797827;     ultugQUivR16797827 = ultugQUivR3464627;     ultugQUivR3464627 = ultugQUivR86698556;     ultugQUivR86698556 = ultugQUivR67282483;     ultugQUivR67282483 = ultugQUivR46723626;     ultugQUivR46723626 = ultugQUivR86464952;     ultugQUivR86464952 = ultugQUivR92635665;     ultugQUivR92635665 = ultugQUivR36579695;     ultugQUivR36579695 = ultugQUivR97725951;     ultugQUivR97725951 = ultugQUivR5512962;     ultugQUivR5512962 = ultugQUivR91088494;     ultugQUivR91088494 = ultugQUivR74902110;     ultugQUivR74902110 = ultugQUivR75258676;     ultugQUivR75258676 = ultugQUivR15255568;     ultugQUivR15255568 = ultugQUivR39732494;     ultugQUivR39732494 = ultugQUivR41266687;     ultugQUivR41266687 = ultugQUivR87724221;     ultugQUivR87724221 = ultugQUivR13817780;     ultugQUivR13817780 = ultugQUivR89931358;     ultugQUivR89931358 = ultugQUivR29279806;     ultugQUivR29279806 = ultugQUivR61591658;     ultugQUivR61591658 = ultugQUivR17192282;     ultugQUivR17192282 = ultugQUivR78757269;     ultugQUivR78757269 = ultugQUivR74053427;     ultugQUivR74053427 = ultugQUivR53410367;     ultugQUivR53410367 = ultugQUivR43739324;     ultugQUivR43739324 = ultugQUivR31458003;     ultugQUivR31458003 = ultugQUivR81396853;     ultugQUivR81396853 = ultugQUivR66286542;     ultugQUivR66286542 = ultugQUivR73528940;     ultugQUivR73528940 = ultugQUivR26900644;     ultugQUivR26900644 = ultugQUivR89011072;     ultugQUivR89011072 = ultugQUivR47867301;     ultugQUivR47867301 = ultugQUivR97502902;     ultugQUivR97502902 = ultugQUivR1946700;     ultugQUivR1946700 = ultugQUivR82534693;     ultugQUivR82534693 = ultugQUivR38158203;     ultugQUivR38158203 = ultugQUivR67607560;     ultugQUivR67607560 = ultugQUivR13971017;     ultugQUivR13971017 = ultugQUivR19601256;     ultugQUivR19601256 = ultugQUivR32450785;     ultugQUivR32450785 = ultugQUivR89300336;     ultugQUivR89300336 = ultugQUivR55475644;     ultugQUivR55475644 = ultugQUivR36134457;     ultugQUivR36134457 = ultugQUivR62414940;     ultugQUivR62414940 = ultugQUivR77411414;     ultugQUivR77411414 = ultugQUivR99055367;     ultugQUivR99055367 = ultugQUivR73793765;     ultugQUivR73793765 = ultugQUivR70905223;     ultugQUivR70905223 = ultugQUivR81974194;     ultugQUivR81974194 = ultugQUivR32264950;     ultugQUivR32264950 = ultugQUivR36944046;     ultugQUivR36944046 = ultugQUivR67249698;     ultugQUivR67249698 = ultugQUivR82211659;     ultugQUivR82211659 = ultugQUivR6207652;     ultugQUivR6207652 = ultugQUivR99605545;     ultugQUivR99605545 = ultugQUivR24707357;     ultugQUivR24707357 = ultugQUivR12645129;     ultugQUivR12645129 = ultugQUivR13872116;     ultugQUivR13872116 = ultugQUivR2984302;     ultugQUivR2984302 = ultugQUivR55006950;     ultugQUivR55006950 = ultugQUivR11238812;     ultugQUivR11238812 = ultugQUivR74512423;}
// Junk Finished
