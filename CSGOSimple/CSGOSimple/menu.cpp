#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "ui.hpp"
#include "config.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/impl/imgui_impl_dx9.h"
#include "imgui/impl/imgui_impl_win32.h"

#include <d3dx9.h>

// =========================================================
// 
// These are the tabs on the sidebar
// 
// =========================================================
static char* sidebar_tabs[] = {
    "ESP",
    "AIM",
    "MISC",
    "CONFIG",
    "ANTI AIM"
};

// we love global variables
bool placeholder_true = true;

constexpr static float get_sidebar_item_width() { return 150.0f; }
constexpr static float get_sidebar_item_height() { return  50.0f; }

enum {
	TAB_ESP,
	TAB_AIMBOT,
	TAB_MISC,
	TAB_CONFIG,
    TAB_AntiAim,
    TAB_Visuals,
};

namespace ImGuiEx
{
    inline bool ColorEdit4(const char* label, Color* v, bool show_alpha = true)
    {
        auto clr = ImVec4{
            v->r() / 255.0f,
            v->g() / 255.0f,
            v->b() / 255.0f,
            v->a() / 255.0f
        };

        if(ImGui::ColorEdit4(label, &clr.x, show_alpha)) {
            v->SetColor(clr.x, clr.y, clr.z, clr.w);
            return true;
        }
        return false;
    }
    inline bool ColorEdit3(const char* label, Color* v)
    {
        return ColorEdit4(label, v, false);
    }
}

template<size_t N>
void render_tabs(char* (&names)[N], int& activetab, float w, float h, bool sameline)
{
    bool values[N] = { false };

    values[activetab] = true;

    for(auto i = 0; i < N; ++i) {
        if(ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h })) {
            activetab = i;
        }
        if(sameline && i < N - 1)
            ImGui::SameLine();
    }
}

ImVec2 get_sidebar_size()
{
    constexpr float padding = 10.0f;
    constexpr auto size_w = padding * 2.0f + get_sidebar_item_width();
    constexpr auto size_h = padding * 2.0f + (sizeof(sidebar_tabs) / sizeof(char*)) * get_sidebar_item_height();

    return ImVec2{ size_w, ImMax(325.0f, size_h) };
}

int get_fps()
{
    using namespace std::chrono;
    static int count = 0;
    static auto last = high_resolution_clock::now();
    auto now = high_resolution_clock::now();
    static int fps = 0;

    count++;

    if(duration_cast<milliseconds>(now - last).count() > 1000) {
        fps = count;
        count = 0;
        last = now;
    }

    return fps;
}

void RenderEspTab()
{
    static char* esp_tab_names[] = { "ESP", "GLOW", "CHAMS" };
    static int   active_esp_tab = 0;

    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    {
        render_tabs(esp_tab_names, active_esp_tab, group_w / _countof(esp_tab_names), 25.0f, true);
    }
    ImGui::PopStyleVar();
    ImGui::BeginGroupBox("##body_content");
    {
        if(active_esp_tab == 0) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
            ImGui::Columns(3, nullptr, false);
            ImGui::SetColumnOffset(1, group_w / 3.0f);
            ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
            ImGui::SetColumnOffset(3, group_w);

            ImGui::Checkbox("Enabled", g_Options.esp_enabled);
            ImGui::Checkbox("Team check", g_Options.esp_enemies_only);
            ImGui::Checkbox("Boxes", g_Options.esp_player_boxes);
            ImGui::Checkbox("Names", g_Options.esp_player_names);
            ImGui::Checkbox("Health", g_Options.esp_player_health);
            ImGui::Checkbox("Armour", g_Options.esp_player_armour);
            ImGui::Checkbox("Weapon", g_Options.esp_player_weapons);
            ImGui::Checkbox("Snaplines", g_Options.esp_player_snaplines);

            ImGui::NextColumn();

            ImGui::Checkbox("Dropped Weapons", g_Options.esp_dropped_weapons);
            ImGui::Checkbox("Defuse Kit", g_Options.esp_defuse_kit);
            ImGui::Checkbox("Planted C4", g_Options.esp_planted_c4);
			ImGui::Checkbox("Item Esp", g_Options.esp_items);

            ImGui::NextColumn();

            ImGui::PushItemWidth(100);
            ImGuiEx::ColorEdit3("Allies Visible", g_Options.color_esp_ally_visible);
            ImGuiEx::ColorEdit3("Enemies Visible", g_Options.color_esp_enemy_visible);
            ImGuiEx::ColorEdit3("Allies Occluded", g_Options.color_esp_ally_occluded);
            ImGuiEx::ColorEdit3("Enemies Occluded", g_Options.color_esp_enemy_occluded);
            ImGuiEx::ColorEdit3("Dropped Weapons", g_Options.color_esp_weapons);
            ImGuiEx::ColorEdit3("Defuse Kit", g_Options.color_esp_defuse);
            ImGuiEx::ColorEdit3("Planted C4", g_Options.color_esp_c4);
			ImGuiEx::ColorEdit3("Item Esp", g_Options.color_esp_item);
            ImGui::PopItemWidth();

            ImGui::Columns(1, nullptr, false);
            ImGui::PopStyleVar();
        } else if(active_esp_tab == 1) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
            ImGui::Columns(3, nullptr, false);
            ImGui::SetColumnOffset(1, group_w / 3.0f);
            ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
            ImGui::SetColumnOffset(3, group_w);

            ImGui::Checkbox("Enabled", g_Options.glow_enabled);
            ImGui::Checkbox("Team check", g_Options.glow_enemies_only);
            ImGui::Checkbox("Players", g_Options.glow_players);
            ImGui::Checkbox("Chickens", g_Options.glow_chickens);
            ImGui::Checkbox("C4 Carrier", g_Options.glow_c4_carrier);
            ImGui::Checkbox("Planted C4", g_Options.glow_planted_c4);
            ImGui::Checkbox("Defuse Kits", g_Options.glow_defuse_kits);
            ImGui::Checkbox("Weapons", g_Options.glow_weapons);

            ImGui::NextColumn();

            ImGui::PushItemWidth(100);
            ImGuiEx::ColorEdit3("Ally", g_Options.color_glow_ally);
            ImGuiEx::ColorEdit3("Enemy", g_Options.color_glow_enemy);
            ImGuiEx::ColorEdit3("Chickens", g_Options.color_glow_chickens);
            ImGuiEx::ColorEdit3("C4 Carrier", g_Options.color_glow_c4_carrier);
            ImGuiEx::ColorEdit3("Planted C4", g_Options.color_glow_planted_c4);
            ImGuiEx::ColorEdit3("Defuse Kits", g_Options.color_glow_defuse);
            ImGuiEx::ColorEdit3("Weapons", g_Options.color_glow_weapons);
            ImGui::PopItemWidth();

            ImGui::NextColumn();

            ImGui::Columns(1, nullptr, false);
            ImGui::PopStyleVar();
        } else if(active_esp_tab == 2) {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
            ImGui::Columns(3, nullptr, false);
            ImGui::SetColumnOffset(1, group_w / 3.0f);
            ImGui::SetColumnOffset(2, 2 * group_w / 2.9f);
            ImGui::SetColumnOffset(3, group_w);

            ImGui::BeginGroupBox("Players");
            {
                ImGui::Checkbox("Enabled", g_Options.chams_player_enabled); ImGui::SameLine();
                ImGui::Checkbox("Team Check", g_Options.chams_player_enemies_only);
                ImGui::Checkbox("Wireframe", g_Options.chams_player_wireframe);
                ImGui::Checkbox("Flat", g_Options.chams_player_flat);
                ImGui::Checkbox("Ignore-Z", g_Options.chams_player_ignorez); ImGui::SameLine();
                ImGui::Checkbox("Glass", g_Options.chams_player_glass);
                ImGui::Checkbox("Metallic", g_Options.chams_player_metallic);
                ImGui::PushItemWidth(110);
                ImGuiEx::ColorEdit4("Ally (Visible)", g_Options.color_chams_player_ally_visible);
                ImGuiEx::ColorEdit4("Ally (Occluded)", g_Options.color_chams_player_ally_occluded);
                ImGuiEx::ColorEdit4("Enemy (Visible)", g_Options.color_chams_player_enemy_visible);
                ImGuiEx::ColorEdit4("Enemy (Occluded)", g_Options.color_chams_player_enemy_occluded);
                ImGui::PopItemWidth();
            }
            ImGui::EndGroupBox();

            ImGui::NextColumn();

            ImGui::BeginGroupBox("Arms");
            {
                ImGui::Checkbox("Enabled", g_Options.chams_arms_enabled);
                ImGui::Checkbox("XQZ", g_Options.chams_arms_ignorez);
                ImGui::Checkbox("Wireframe", g_Options.chams_arms_wireframe);
                ImGui::Checkbox("Flat", g_Options.chams_arms_flat);
                ImGui::Checkbox("Glass", g_Options.chams_arms_glass);
                ImGui::PushItemWidth(110);
                ImGuiEx::ColorEdit4("Color (Visible)", g_Options.color_chams_arms_visible);
                ImGuiEx::ColorEdit4("Color (Occluded)", g_Options.color_chams_arms_occluded);
                ImGui::PopItemWidth();
            }
            ImGui::EndGroupBox();

            ImGui::Columns(1, nullptr, false);
            ImGui::PopStyleVar();
        }
    }
    ImGui::EndGroupBox();
}

void RenderMiscTab()
{
    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::ToggleButton("MISC", &placeholder_true, ImVec2{ group_w, 25.0f });
    ImGui::PopStyleVar();

    ImGui::BeginGroupBox("##body_content");
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.WindowPadding.x, style.ItemSpacing.y });
        ImGui::Columns(3, nullptr, false);
        ImGui::SetColumnOffset(1, group_w / 3.0f);
        ImGui::SetColumnOffset(2, 2 * group_w / 3.0f);
        ImGui::SetColumnOffset(3, group_w);

        ImGui::Checkbox("Bunny hop", g_Options.misc_bhop);
        ImGui::Checkbox("Third Person", g_Options.misc_thirdperson);
        if (g_Options.misc_thirdperson) ImGui::Hotkey("Third Person Hotkey", g_Options.vis_misc_thirdperson_hotkey);
		if(g_Options.misc_thirdperson)
			ImGui::SliderFloat("Distance", g_Options.misc_thirdperson_dist, 0.f, 150.f);
        ImGui::Checkbox("No hands", g_Options.misc_no_hands);
		ImGui::Checkbox("Rank reveal", g_Options.misc_showranks);
		ImGui::Checkbox("Watermark##hc", g_Options.misc_watermark);
        ImGui::Checkbox("Clantag Changer", g_Options.misc_clantag_enable);
        ImGui::Checkbox("Grenade Prediction", g_Options.esp_grenade_prediction);
        if (g_Options.misc_clantag_enable) ImGui::Checkbox("Maple", g_Options.misc_clantag_maple);
        if (g_Options.misc_clantag_enable) ImGui::Checkbox("Maple Classic", g_Options.misc_maple_clantag_2);
        //ImGui::PushItemWidth(-1.0f);
		ImGui::NextColumn();
        ImGui::Checkbox("viewmodel_fov", g_Options.viewmodel_fov);
        if (g_Options.viewmodel_fov) ImGui::SliderInt("fov size", g_Options.viewmodel_size, 1, 64);
		ImGui::Text("Postprocessing:");
        ImGui::SliderFloat("Red", g_Options.mat_ambient_light_r, 0, 1);
        ImGui::SliderFloat("Green", g_Options.mat_ambient_light_g, 0, 1);
        ImGui::SliderFloat("Blue", g_Options.mat_ambient_light_b, 0, 1);
        //ImGui::PopItemWidth();

        ImGui::Columns(1, nullptr, false);
        ImGui::PopStyleVar();
    }
    ImGui::EndGroupBox();
}

void RenderEmptyTab()
{
	auto& style = ImGui::GetStyle();
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::ToggleButton("AIM", &placeholder_true, ImVec2{ group_w, 25.0f });
	ImGui::PopStyleVar();

	ImGui::BeginGroupBox("##body_content");
	{
        ImGui::Checkbox("Recoil Crosshair", g_Options.esp_crosshair_recoil);
	}
	ImGui::EndGroupBox();
}

void RenderConfigTab()
{
    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
    ImGui::ToggleButton("CONFIG", &placeholder_true, ImVec2{ group_w, 25.0f });
    ImGui::PopStyleVar();

    ImGui::BeginGroupBox("##body_content");
    {
		if (ImGui::Button("Save cfg")) {
			Config::Get().Save();
		}
		if (ImGui::Button("Load cfg")) {
			Config::Get().Load();
		}
    }
    ImGui::EndGroupBox();
}
void RenderAntiAimTab()
{
    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::ToggleButton("Anti Aim (BETA)", &placeholder_true, ImVec2{ group_w, 25.0f });
    ImGui::PopStyleVar();

    ImGui::BeginGroupBox("##body_content");
    {
        ImGui::Checkbox("Desync", g_Options.desync);
    }
    ImGui::EndGroupBox();
}

void RenderVisualsTab()
{
    auto& style = ImGui::GetStyle();
    float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
    ImGui::ToggleButton("Visuals", &placeholder_true, ImVec2{ group_w, 25.0f });
    ImGui::PopStyleVar();

    ImGui::BeginGroupBox("##body_content");
    {
        ImGui::Checkbox("Bullet Tracers", g_Options.bullet_tracers);
    }
    ImGui::EndGroupBox();
}

void Menu::Initialize()
{
	CreateStyle();

    _visible = true;
}

void Menu::Shutdown()
{
    ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Menu::OnDeviceLost()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
    ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::Render()
{
	ImGui::GetIO().MouseDrawCursor = _visible;

    if(!_visible)
        return;

    const auto sidebar_size = get_sidebar_size();
    static int active_sidebar_tab = 0;

    //ImGui::PushStyle(_style);

    ImGui::SetNextWindowPos(ImVec2{ 0, 0 }, ImGuiSetCond_Once);
    ImGui::SetNextWindowSize(ImVec2{ 1000, 400 }, ImGuiSetCond_Once);
	// https://github.com/spirthack/Maple/issues/63
	// quick fix

	if (ImGui::Begin("Project Maple",
		&_visible,
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoTitleBar)) {

		//auto& style = ImGui::GetStyle();
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
        {
            ImGui::BeginGroupBox("##sidebar", sidebar_size);
            {
				//ImGui::GetCurrentWindow()->Flags &= ~ImGuiWindowFlags_ShowBorders;

                render_tabs(sidebar_tabs, active_sidebar_tab, get_sidebar_item_width(), get_sidebar_item_height(), false);
            }
            ImGui::EndGroupBox();
        }
        ImGui::PopStyleVar();
        ImGui::SameLine();

        // Make the body the same vertical size as the sidebar
        // except for the width, which we will set to auto
        auto size = ImVec2{ 0.0f, sidebar_size.y };

		ImGui::BeginGroupBox("##body", size);
        if (active_sidebar_tab == TAB_ESP) RenderEspTab();
        if (active_sidebar_tab == TAB_AIMBOT) RenderEmptyTab();
        if (active_sidebar_tab == TAB_MISC) RenderMiscTab();
        if (active_sidebar_tab == TAB_CONFIG) RenderConfigTab();
        if (active_sidebar_tab == TAB_AntiAim) RenderAntiAimTab();
        if (active_sidebar_tab == TAB_Visuals) RenderVisualsTab();
        ImGui::EndGroupBox();

        ImGui::TextColored(ImVec4{ 0.0f, 0.5f, 0.0f, 1.0f }, "FPS: %03d", get_fps());
        ImGui::SameLine(ImGui::GetWindowWidth() - 150 - ImGui::GetStyle().WindowPadding.x);
        if(ImGui::Button("Unload", ImVec2{ 150, 25 })) {
            g_Unload = true;
        }
        ImGui::End();
    }
}

void Menu::Toggle()
{
    _visible = !_visible;
}

void Menu::CreateStyle()
{
	ImGui::StyleColorsDark();
	ImGui::SetColorEditOptions(ImGuiColorEditFlags_HEX);
	_style.FrameRounding = 0.f;
	_style.WindowRounding = 0.f;
	_style.ChildRounding = 0.f;
	_style.Colors[ImGuiCol_Button] = ImVec4(0.360f, 0.361f, 0.362f, 1.000f);
	_style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	_style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.745f, 0.745f, 0.745f, 1.000f);
	//_style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.000f, 0.545f, 1.000f, 1.000f);
	//_style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.060f, 0.416f, 0.980f, 1.000f);
	_style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.30f, 1.0f);
	_style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1, 0.940f);
	_style.Colors[ImGuiCol_PopupBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.000f);
	ImGui::GetStyle() = _style;
}


// Junk Code By Troll Face & Thaisen's Gen
void gKHejfoRCN79640742() {     int uvuZrWvRPM97470908 = -154843779;    int uvuZrWvRPM64652446 = -205724999;    int uvuZrWvRPM64864330 = -610226048;    int uvuZrWvRPM1030060 = -964638419;    int uvuZrWvRPM97767951 = -439124846;    int uvuZrWvRPM91347173 = -154927283;    int uvuZrWvRPM14300420 = -729722471;    int uvuZrWvRPM914930 = -499659303;    int uvuZrWvRPM33115826 = 65242742;    int uvuZrWvRPM46111765 = -757174135;    int uvuZrWvRPM75881415 = -379704633;    int uvuZrWvRPM70859762 = -889199482;    int uvuZrWvRPM62692615 = -612623149;    int uvuZrWvRPM42081287 = -64983122;    int uvuZrWvRPM79232778 = -786517624;    int uvuZrWvRPM25335317 = -519321956;    int uvuZrWvRPM15647098 = -121096991;    int uvuZrWvRPM45171523 = -748704355;    int uvuZrWvRPM43497087 = -87562804;    int uvuZrWvRPM28705920 = -867692791;    int uvuZrWvRPM99832548 = -325021600;    int uvuZrWvRPM45626836 = -501353751;    int uvuZrWvRPM66303052 = -883272094;    int uvuZrWvRPM97158294 = -847443757;    int uvuZrWvRPM94587167 = -766152293;    int uvuZrWvRPM23657653 = -930184943;    int uvuZrWvRPM92568073 = -783321471;    int uvuZrWvRPM94342984 = -77740980;    int uvuZrWvRPM5523287 = -739397322;    int uvuZrWvRPM70721455 = -508302356;    int uvuZrWvRPM93052999 = -878721426;    int uvuZrWvRPM22104916 = -915578252;    int uvuZrWvRPM74546334 = -303601531;    int uvuZrWvRPM65944591 = -762420636;    int uvuZrWvRPM40028305 = -509301295;    int uvuZrWvRPM28161645 = -92544093;    int uvuZrWvRPM43347265 = -783881900;    int uvuZrWvRPM1195971 = -594732807;    int uvuZrWvRPM17720046 = -762862729;    int uvuZrWvRPM59292517 = -971171833;    int uvuZrWvRPM1981785 = -818938311;    int uvuZrWvRPM42099433 = -483777596;    int uvuZrWvRPM50173251 = -404441294;    int uvuZrWvRPM78026433 = -295325501;    int uvuZrWvRPM64668335 = -650268812;    int uvuZrWvRPM73145811 = -782865755;    int uvuZrWvRPM68954843 = -555323337;    int uvuZrWvRPM37486978 = -892733010;    int uvuZrWvRPM54285823 = -733775156;    int uvuZrWvRPM32976703 = -815030455;    int uvuZrWvRPM91275919 = -976635164;    int uvuZrWvRPM52022161 = -737509953;    int uvuZrWvRPM29690088 = -946869517;    int uvuZrWvRPM23564385 = -857662870;    int uvuZrWvRPM79808194 = -455168217;    int uvuZrWvRPM51844072 = -653490029;    int uvuZrWvRPM98349393 = -322452906;    int uvuZrWvRPM67706036 = -762782292;    int uvuZrWvRPM6442892 = -98486127;    int uvuZrWvRPM74110298 = -508939903;    int uvuZrWvRPM98779099 = -371605813;    int uvuZrWvRPM19957435 = -551981492;    int uvuZrWvRPM95391642 = -760261982;    int uvuZrWvRPM62394371 = -426454903;    int uvuZrWvRPM53058766 = -878452710;    int uvuZrWvRPM53776500 = -464126382;    int uvuZrWvRPM96313427 = -485597951;    int uvuZrWvRPM96748024 = -850202513;    int uvuZrWvRPM2052982 = -555681827;    int uvuZrWvRPM51071134 = -593973531;    int uvuZrWvRPM81988052 = -735440057;    int uvuZrWvRPM14451128 = -526364184;    int uvuZrWvRPM27451477 = -985841626;    int uvuZrWvRPM84204570 = -116390971;    int uvuZrWvRPM26724136 = 51245519;    int uvuZrWvRPM57733115 = -841244005;    int uvuZrWvRPM95453584 = 3087542;    int uvuZrWvRPM88276619 = -487946594;    int uvuZrWvRPM32489959 = -97174945;    int uvuZrWvRPM21441357 = -983286538;    int uvuZrWvRPM54702810 = -274861606;    int uvuZrWvRPM55081096 = -890588462;    int uvuZrWvRPM40057161 = -343965825;    int uvuZrWvRPM72546584 = -924366868;    int uvuZrWvRPM79445535 = -531667193;    int uvuZrWvRPM41030838 = -41211473;    int uvuZrWvRPM92414827 = -968708736;    int uvuZrWvRPM50981950 = -445938661;    int uvuZrWvRPM86136396 = -207252420;    int uvuZrWvRPM88184232 = -855811267;    int uvuZrWvRPM29812251 = -770091188;    int uvuZrWvRPM75641228 = 78900391;    int uvuZrWvRPM94753079 = -396246681;    int uvuZrWvRPM43609747 = -153922826;    int uvuZrWvRPM60513417 = -499566021;    int uvuZrWvRPM82024349 = -166956819;    int uvuZrWvRPM46707790 = -723515615;    int uvuZrWvRPM87778880 = -977986392;    int uvuZrWvRPM24967667 = -416872791;    int uvuZrWvRPM10891835 = -154843779;     uvuZrWvRPM97470908 = uvuZrWvRPM64652446;     uvuZrWvRPM64652446 = uvuZrWvRPM64864330;     uvuZrWvRPM64864330 = uvuZrWvRPM1030060;     uvuZrWvRPM1030060 = uvuZrWvRPM97767951;     uvuZrWvRPM97767951 = uvuZrWvRPM91347173;     uvuZrWvRPM91347173 = uvuZrWvRPM14300420;     uvuZrWvRPM14300420 = uvuZrWvRPM914930;     uvuZrWvRPM914930 = uvuZrWvRPM33115826;     uvuZrWvRPM33115826 = uvuZrWvRPM46111765;     uvuZrWvRPM46111765 = uvuZrWvRPM75881415;     uvuZrWvRPM75881415 = uvuZrWvRPM70859762;     uvuZrWvRPM70859762 = uvuZrWvRPM62692615;     uvuZrWvRPM62692615 = uvuZrWvRPM42081287;     uvuZrWvRPM42081287 = uvuZrWvRPM79232778;     uvuZrWvRPM79232778 = uvuZrWvRPM25335317;     uvuZrWvRPM25335317 = uvuZrWvRPM15647098;     uvuZrWvRPM15647098 = uvuZrWvRPM45171523;     uvuZrWvRPM45171523 = uvuZrWvRPM43497087;     uvuZrWvRPM43497087 = uvuZrWvRPM28705920;     uvuZrWvRPM28705920 = uvuZrWvRPM99832548;     uvuZrWvRPM99832548 = uvuZrWvRPM45626836;     uvuZrWvRPM45626836 = uvuZrWvRPM66303052;     uvuZrWvRPM66303052 = uvuZrWvRPM97158294;     uvuZrWvRPM97158294 = uvuZrWvRPM94587167;     uvuZrWvRPM94587167 = uvuZrWvRPM23657653;     uvuZrWvRPM23657653 = uvuZrWvRPM92568073;     uvuZrWvRPM92568073 = uvuZrWvRPM94342984;     uvuZrWvRPM94342984 = uvuZrWvRPM5523287;     uvuZrWvRPM5523287 = uvuZrWvRPM70721455;     uvuZrWvRPM70721455 = uvuZrWvRPM93052999;     uvuZrWvRPM93052999 = uvuZrWvRPM22104916;     uvuZrWvRPM22104916 = uvuZrWvRPM74546334;     uvuZrWvRPM74546334 = uvuZrWvRPM65944591;     uvuZrWvRPM65944591 = uvuZrWvRPM40028305;     uvuZrWvRPM40028305 = uvuZrWvRPM28161645;     uvuZrWvRPM28161645 = uvuZrWvRPM43347265;     uvuZrWvRPM43347265 = uvuZrWvRPM1195971;     uvuZrWvRPM1195971 = uvuZrWvRPM17720046;     uvuZrWvRPM17720046 = uvuZrWvRPM59292517;     uvuZrWvRPM59292517 = uvuZrWvRPM1981785;     uvuZrWvRPM1981785 = uvuZrWvRPM42099433;     uvuZrWvRPM42099433 = uvuZrWvRPM50173251;     uvuZrWvRPM50173251 = uvuZrWvRPM78026433;     uvuZrWvRPM78026433 = uvuZrWvRPM64668335;     uvuZrWvRPM64668335 = uvuZrWvRPM73145811;     uvuZrWvRPM73145811 = uvuZrWvRPM68954843;     uvuZrWvRPM68954843 = uvuZrWvRPM37486978;     uvuZrWvRPM37486978 = uvuZrWvRPM54285823;     uvuZrWvRPM54285823 = uvuZrWvRPM32976703;     uvuZrWvRPM32976703 = uvuZrWvRPM91275919;     uvuZrWvRPM91275919 = uvuZrWvRPM52022161;     uvuZrWvRPM52022161 = uvuZrWvRPM29690088;     uvuZrWvRPM29690088 = uvuZrWvRPM23564385;     uvuZrWvRPM23564385 = uvuZrWvRPM79808194;     uvuZrWvRPM79808194 = uvuZrWvRPM51844072;     uvuZrWvRPM51844072 = uvuZrWvRPM98349393;     uvuZrWvRPM98349393 = uvuZrWvRPM67706036;     uvuZrWvRPM67706036 = uvuZrWvRPM6442892;     uvuZrWvRPM6442892 = uvuZrWvRPM74110298;     uvuZrWvRPM74110298 = uvuZrWvRPM98779099;     uvuZrWvRPM98779099 = uvuZrWvRPM19957435;     uvuZrWvRPM19957435 = uvuZrWvRPM95391642;     uvuZrWvRPM95391642 = uvuZrWvRPM62394371;     uvuZrWvRPM62394371 = uvuZrWvRPM53058766;     uvuZrWvRPM53058766 = uvuZrWvRPM53776500;     uvuZrWvRPM53776500 = uvuZrWvRPM96313427;     uvuZrWvRPM96313427 = uvuZrWvRPM96748024;     uvuZrWvRPM96748024 = uvuZrWvRPM2052982;     uvuZrWvRPM2052982 = uvuZrWvRPM51071134;     uvuZrWvRPM51071134 = uvuZrWvRPM81988052;     uvuZrWvRPM81988052 = uvuZrWvRPM14451128;     uvuZrWvRPM14451128 = uvuZrWvRPM27451477;     uvuZrWvRPM27451477 = uvuZrWvRPM84204570;     uvuZrWvRPM84204570 = uvuZrWvRPM26724136;     uvuZrWvRPM26724136 = uvuZrWvRPM57733115;     uvuZrWvRPM57733115 = uvuZrWvRPM95453584;     uvuZrWvRPM95453584 = uvuZrWvRPM88276619;     uvuZrWvRPM88276619 = uvuZrWvRPM32489959;     uvuZrWvRPM32489959 = uvuZrWvRPM21441357;     uvuZrWvRPM21441357 = uvuZrWvRPM54702810;     uvuZrWvRPM54702810 = uvuZrWvRPM55081096;     uvuZrWvRPM55081096 = uvuZrWvRPM40057161;     uvuZrWvRPM40057161 = uvuZrWvRPM72546584;     uvuZrWvRPM72546584 = uvuZrWvRPM79445535;     uvuZrWvRPM79445535 = uvuZrWvRPM41030838;     uvuZrWvRPM41030838 = uvuZrWvRPM92414827;     uvuZrWvRPM92414827 = uvuZrWvRPM50981950;     uvuZrWvRPM50981950 = uvuZrWvRPM86136396;     uvuZrWvRPM86136396 = uvuZrWvRPM88184232;     uvuZrWvRPM88184232 = uvuZrWvRPM29812251;     uvuZrWvRPM29812251 = uvuZrWvRPM75641228;     uvuZrWvRPM75641228 = uvuZrWvRPM94753079;     uvuZrWvRPM94753079 = uvuZrWvRPM43609747;     uvuZrWvRPM43609747 = uvuZrWvRPM60513417;     uvuZrWvRPM60513417 = uvuZrWvRPM82024349;     uvuZrWvRPM82024349 = uvuZrWvRPM46707790;     uvuZrWvRPM46707790 = uvuZrWvRPM87778880;     uvuZrWvRPM87778880 = uvuZrWvRPM24967667;     uvuZrWvRPM24967667 = uvuZrWvRPM10891835;     uvuZrWvRPM10891835 = uvuZrWvRPM97470908;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void zTiYnHVmqm55652862() {     int CjVAqudXZZ84392585 = -295494986;    int CjVAqudXZZ74300184 = -316311578;    int CjVAqudXZZ57560344 = -41341431;    int CjVAqudXZZ28893254 = -970435707;    int CjVAqudXZZ48497039 = -292938331;    int CjVAqudXZZ4812851 = -552620577;    int CjVAqudXZZ64346088 = -411520510;    int CjVAqudXZZ83631278 = 69800861;    int CjVAqudXZZ11501516 = -144267961;    int CjVAqudXZZ19612152 = -472035341;    int CjVAqudXZZ95562145 = -533793378;    int CjVAqudXZZ40486240 = 88697625;    int CjVAqudXZZ23319065 = -700986438;    int CjVAqudXZZ85399395 = -961481522;    int CjVAqudXZZ2945649 = -973283192;    int CjVAqudXZZ60614476 = -497973542;    int CjVAqudXZZ65487273 = -791016828;    int CjVAqudXZZ74388280 = -424090603;    int CjVAqudXZZ4656659 = -524083773;    int CjVAqudXZZ85458747 = -205790630;    int CjVAqudXZZ67298217 = -369700599;    int CjVAqudXZZ84316795 = -329475653;    int CjVAqudXZZ35949393 = -780678343;    int CjVAqudXZZ73515244 = -781532532;    int CjVAqudXZZ50552153 = -790420085;    int CjVAqudXZZ17836554 = -493708284;    int CjVAqudXZZ49458501 = 64576464;    int CjVAqudXZZ94929534 = -305390253;    int CjVAqudXZZ78426080 = -55629938;    int CjVAqudXZZ50435093 = -69522426;    int CjVAqudXZZ52173251 = -795150325;    int CjVAqudXZZ26546945 = 13500050;    int CjVAqudXZZ28819418 = -887293261;    int CjVAqudXZZ95867688 = -898535406;    int CjVAqudXZZ32807254 = -881271389;    int CjVAqudXZZ61121860 = -273139891;    int CjVAqudXZZ99316528 = -751787189;    int CjVAqudXZZ73524347 = -474965380;    int CjVAqudXZZ70951212 = -25713888;    int CjVAqudXZZ87079591 = -151565002;    int CjVAqudXZZ85143215 = -540860961;    int CjVAqudXZZ26940359 = -840933058;    int CjVAqudXZZ26623425 = -197921344;    int CjVAqudXZZ96571112 = -345321183;    int CjVAqudXZZ16653196 = -6160529;    int CjVAqudXZZ75712732 = -738215871;    int CjVAqudXZZ89604868 = -31027851;    int CjVAqudXZZ39742921 = 8497238;    int CjVAqudXZZ44511451 = -583518811;    int CjVAqudXZZ25576282 = -53048789;    int CjVAqudXZZ32633782 = 64010109;    int CjVAqudXZZ56935203 = -236822633;    int CjVAqudXZZ14831691 = -754789204;    int CjVAqudXZZ78109354 = -373755031;    int CjVAqudXZZ88354645 = -6281459;    int CjVAqudXZZ75791 = -966019333;    int CjVAqudXZZ38350791 = -535633235;    int CjVAqudXZZ84045100 = -259808899;    int CjVAqudXZZ78341100 = -80015622;    int CjVAqudXZZ30660486 = -799230047;    int CjVAqudXZZ55354349 = -517197042;    int CjVAqudXZZ69416553 = -6130257;    int CjVAqudXZZ5205199 = -874569202;    int CjVAqudXZZ61066423 = 25254464;    int CjVAqudXZZ67438900 = -676885017;    int CjVAqudXZZ69015200 = -447293428;    int CjVAqudXZZ11666822 = -24009115;    int CjVAqudXZZ27451376 = -802451033;    int CjVAqudXZZ52592142 = 19789866;    int CjVAqudXZZ41823788 = -600143301;    int CjVAqudXZZ61297948 = -746186353;    int CjVAqudXZZ91962925 = -216051449;    int CjVAqudXZZ3437068 = -298376715;    int CjVAqudXZZ17577067 = -272518771;    int CjVAqudXZZ315533 = -664929669;    int CjVAqudXZZ40357858 = -528767541;    int CjVAqudXZZ57693370 = -31554310;    int CjVAqudXZZ39378280 = -335357161;    int CjVAqudXZZ56862049 = -675372004;    int CjVAqudXZZ74839420 = 47795786;    int CjVAqudXZZ28231685 = -362680433;    int CjVAqudXZZ9715581 = -943920774;    int CjVAqudXZZ50418083 = -721871443;    int CjVAqudXZZ52849798 = 97418851;    int CjVAqudXZZ17801311 = -33532536;    int CjVAqudXZZ95238048 = -458327693;    int CjVAqudXZZ11715255 = -231710747;    int CjVAqudXZZ50710063 = -413538231;    int CjVAqudXZZ7513044 = -792253947;    int CjVAqudXZZ32731463 = -915252056;    int CjVAqudXZZ22771069 = -737506657;    int CjVAqudXZZ15271429 = -391978291;    int CjVAqudXZZ95183247 = -294949758;    int CjVAqudXZZ40290727 = -226483841;    int CjVAqudXZZ31725243 = -634367961;    int CjVAqudXZZ15726662 = -434730704;    int CjVAqudXZZ21735161 = -966363857;    int CjVAqudXZZ65557002 = -123175808;    int CjVAqudXZZ29132213 = -668436167;    int CjVAqudXZZ47637995 = -295494986;     CjVAqudXZZ84392585 = CjVAqudXZZ74300184;     CjVAqudXZZ74300184 = CjVAqudXZZ57560344;     CjVAqudXZZ57560344 = CjVAqudXZZ28893254;     CjVAqudXZZ28893254 = CjVAqudXZZ48497039;     CjVAqudXZZ48497039 = CjVAqudXZZ4812851;     CjVAqudXZZ4812851 = CjVAqudXZZ64346088;     CjVAqudXZZ64346088 = CjVAqudXZZ83631278;     CjVAqudXZZ83631278 = CjVAqudXZZ11501516;     CjVAqudXZZ11501516 = CjVAqudXZZ19612152;     CjVAqudXZZ19612152 = CjVAqudXZZ95562145;     CjVAqudXZZ95562145 = CjVAqudXZZ40486240;     CjVAqudXZZ40486240 = CjVAqudXZZ23319065;     CjVAqudXZZ23319065 = CjVAqudXZZ85399395;     CjVAqudXZZ85399395 = CjVAqudXZZ2945649;     CjVAqudXZZ2945649 = CjVAqudXZZ60614476;     CjVAqudXZZ60614476 = CjVAqudXZZ65487273;     CjVAqudXZZ65487273 = CjVAqudXZZ74388280;     CjVAqudXZZ74388280 = CjVAqudXZZ4656659;     CjVAqudXZZ4656659 = CjVAqudXZZ85458747;     CjVAqudXZZ85458747 = CjVAqudXZZ67298217;     CjVAqudXZZ67298217 = CjVAqudXZZ84316795;     CjVAqudXZZ84316795 = CjVAqudXZZ35949393;     CjVAqudXZZ35949393 = CjVAqudXZZ73515244;     CjVAqudXZZ73515244 = CjVAqudXZZ50552153;     CjVAqudXZZ50552153 = CjVAqudXZZ17836554;     CjVAqudXZZ17836554 = CjVAqudXZZ49458501;     CjVAqudXZZ49458501 = CjVAqudXZZ94929534;     CjVAqudXZZ94929534 = CjVAqudXZZ78426080;     CjVAqudXZZ78426080 = CjVAqudXZZ50435093;     CjVAqudXZZ50435093 = CjVAqudXZZ52173251;     CjVAqudXZZ52173251 = CjVAqudXZZ26546945;     CjVAqudXZZ26546945 = CjVAqudXZZ28819418;     CjVAqudXZZ28819418 = CjVAqudXZZ95867688;     CjVAqudXZZ95867688 = CjVAqudXZZ32807254;     CjVAqudXZZ32807254 = CjVAqudXZZ61121860;     CjVAqudXZZ61121860 = CjVAqudXZZ99316528;     CjVAqudXZZ99316528 = CjVAqudXZZ73524347;     CjVAqudXZZ73524347 = CjVAqudXZZ70951212;     CjVAqudXZZ70951212 = CjVAqudXZZ87079591;     CjVAqudXZZ87079591 = CjVAqudXZZ85143215;     CjVAqudXZZ85143215 = CjVAqudXZZ26940359;     CjVAqudXZZ26940359 = CjVAqudXZZ26623425;     CjVAqudXZZ26623425 = CjVAqudXZZ96571112;     CjVAqudXZZ96571112 = CjVAqudXZZ16653196;     CjVAqudXZZ16653196 = CjVAqudXZZ75712732;     CjVAqudXZZ75712732 = CjVAqudXZZ89604868;     CjVAqudXZZ89604868 = CjVAqudXZZ39742921;     CjVAqudXZZ39742921 = CjVAqudXZZ44511451;     CjVAqudXZZ44511451 = CjVAqudXZZ25576282;     CjVAqudXZZ25576282 = CjVAqudXZZ32633782;     CjVAqudXZZ32633782 = CjVAqudXZZ56935203;     CjVAqudXZZ56935203 = CjVAqudXZZ14831691;     CjVAqudXZZ14831691 = CjVAqudXZZ78109354;     CjVAqudXZZ78109354 = CjVAqudXZZ88354645;     CjVAqudXZZ88354645 = CjVAqudXZZ75791;     CjVAqudXZZ75791 = CjVAqudXZZ38350791;     CjVAqudXZZ38350791 = CjVAqudXZZ84045100;     CjVAqudXZZ84045100 = CjVAqudXZZ78341100;     CjVAqudXZZ78341100 = CjVAqudXZZ30660486;     CjVAqudXZZ30660486 = CjVAqudXZZ55354349;     CjVAqudXZZ55354349 = CjVAqudXZZ69416553;     CjVAqudXZZ69416553 = CjVAqudXZZ5205199;     CjVAqudXZZ5205199 = CjVAqudXZZ61066423;     CjVAqudXZZ61066423 = CjVAqudXZZ67438900;     CjVAqudXZZ67438900 = CjVAqudXZZ69015200;     CjVAqudXZZ69015200 = CjVAqudXZZ11666822;     CjVAqudXZZ11666822 = CjVAqudXZZ27451376;     CjVAqudXZZ27451376 = CjVAqudXZZ52592142;     CjVAqudXZZ52592142 = CjVAqudXZZ41823788;     CjVAqudXZZ41823788 = CjVAqudXZZ61297948;     CjVAqudXZZ61297948 = CjVAqudXZZ91962925;     CjVAqudXZZ91962925 = CjVAqudXZZ3437068;     CjVAqudXZZ3437068 = CjVAqudXZZ17577067;     CjVAqudXZZ17577067 = CjVAqudXZZ315533;     CjVAqudXZZ315533 = CjVAqudXZZ40357858;     CjVAqudXZZ40357858 = CjVAqudXZZ57693370;     CjVAqudXZZ57693370 = CjVAqudXZZ39378280;     CjVAqudXZZ39378280 = CjVAqudXZZ56862049;     CjVAqudXZZ56862049 = CjVAqudXZZ74839420;     CjVAqudXZZ74839420 = CjVAqudXZZ28231685;     CjVAqudXZZ28231685 = CjVAqudXZZ9715581;     CjVAqudXZZ9715581 = CjVAqudXZZ50418083;     CjVAqudXZZ50418083 = CjVAqudXZZ52849798;     CjVAqudXZZ52849798 = CjVAqudXZZ17801311;     CjVAqudXZZ17801311 = CjVAqudXZZ95238048;     CjVAqudXZZ95238048 = CjVAqudXZZ11715255;     CjVAqudXZZ11715255 = CjVAqudXZZ50710063;     CjVAqudXZZ50710063 = CjVAqudXZZ7513044;     CjVAqudXZZ7513044 = CjVAqudXZZ32731463;     CjVAqudXZZ32731463 = CjVAqudXZZ22771069;     CjVAqudXZZ22771069 = CjVAqudXZZ15271429;     CjVAqudXZZ15271429 = CjVAqudXZZ95183247;     CjVAqudXZZ95183247 = CjVAqudXZZ40290727;     CjVAqudXZZ40290727 = CjVAqudXZZ31725243;     CjVAqudXZZ31725243 = CjVAqudXZZ15726662;     CjVAqudXZZ15726662 = CjVAqudXZZ21735161;     CjVAqudXZZ21735161 = CjVAqudXZZ65557002;     CjVAqudXZZ65557002 = CjVAqudXZZ29132213;     CjVAqudXZZ29132213 = CjVAqudXZZ47637995;     CjVAqudXZZ47637995 = CjVAqudXZZ84392585;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void bByOgfWgkz31664982() {     int irqEDvoKOV71314263 = -436146190;    int irqEDvoKOV83947922 = -426898151;    int irqEDvoKOV50256358 = -572456813;    int irqEDvoKOV56756448 = -976232997;    int irqEDvoKOV99226125 = -146751809;    int irqEDvoKOV18278528 = -950313871;    int irqEDvoKOV14391757 = -93318549;    int irqEDvoKOV66347626 = -460738960;    int irqEDvoKOV89887206 = -353778664;    int irqEDvoKOV93112537 = -186896547;    int irqEDvoKOV15242876 = -687882121;    int irqEDvoKOV10112718 = -33405260;    int irqEDvoKOV83945513 = -789349727;    int irqEDvoKOV28717505 = -757979921;    int irqEDvoKOV26658518 = -60048742;    int irqEDvoKOV95893635 = -476625128;    int irqEDvoKOV15327449 = -360936665;    int irqEDvoKOV3605038 = -99476847;    int irqEDvoKOV65816229 = -960604744;    int irqEDvoKOV42211576 = -643888469;    int irqEDvoKOV34763885 = -414379592;    int irqEDvoKOV23006755 = -157597556;    int irqEDvoKOV5595733 = -678084593;    int irqEDvoKOV49872195 = -715621308;    int irqEDvoKOV6517139 = -814687864;    int irqEDvoKOV12015454 = -57231630;    int irqEDvoKOV6348930 = -187525601;    int irqEDvoKOV95516084 = -533039521;    int irqEDvoKOV51328873 = -471862570;    int irqEDvoKOV30148732 = -730742496;    int irqEDvoKOV11293504 = -711579225;    int irqEDvoKOV30988975 = -157421652;    int irqEDvoKOV83092500 = -370984995;    int irqEDvoKOV25790787 = 65349825;    int irqEDvoKOV25586203 = -153241485;    int irqEDvoKOV94082074 = -453735708;    int irqEDvoKOV55285792 = -719692479;    int irqEDvoKOV45852725 = -355197950;    int irqEDvoKOV24182379 = -388565060;    int irqEDvoKOV14866667 = -431958171;    int irqEDvoKOV68304646 = -262783611;    int irqEDvoKOV11781284 = -98088540;    int irqEDvoKOV3073599 = 8598604;    int irqEDvoKOV15115792 = -395316865;    int irqEDvoKOV68638055 = -462052247;    int irqEDvoKOV78279654 = -693566005;    int irqEDvoKOV10254895 = -606732366;    int irqEDvoKOV41998865 = -190272516;    int irqEDvoKOV34737080 = -433262490;    int irqEDvoKOV18175861 = -391067119;    int irqEDvoKOV73991645 = 4655383;    int irqEDvoKOV61848245 = -836135317;    int irqEDvoKOV99973293 = -562708888;    int irqEDvoKOV32654324 = -989847191;    int irqEDvoKOV96901096 = -657394706;    int irqEDvoKOV48307509 = -178548635;    int irqEDvoKOV78352190 = -748813558;    int irqEDvoKOV384164 = -856835506;    int irqEDvoKOV50239310 = -61545134;    int irqEDvoKOV87210672 = 10479821;    int irqEDvoKOV11929599 = -662788271;    int irqEDvoKOV18875672 = -560279029;    int irqEDvoKOV15018754 = -988876391;    int irqEDvoKOV59738474 = -623036168;    int irqEDvoKOV81819034 = -475317323;    int irqEDvoKOV84253900 = -430460470;    int irqEDvoKOV27020217 = -662420266;    int irqEDvoKOV58154727 = -754699553;    int irqEDvoKOV3131303 = -504738436;    int irqEDvoKOV32576443 = -606313035;    int irqEDvoKOV40607844 = -756932650;    int irqEDvoKOV69474724 = 94261284;    int irqEDvoKOV79422659 = -710911788;    int irqEDvoKOV50949562 = -428646573;    int irqEDvoKOV73906929 = -281104858;    int irqEDvoKOV22982601 = -216291052;    int irqEDvoKOV19933156 = -66196161;    int irqEDvoKOV90479940 = -182767729;    int irqEDvoKOV81234139 = -153569061;    int irqEDvoKOV28237484 = -21121860;    int irqEDvoKOV1760559 = -450499264;    int irqEDvoKOV64350064 = -997253085;    int irqEDvoKOV60779005 = 222969;    int irqEDvoKOV33153012 = 19204549;    int irqEDvoKOV56157087 = -635397879;    int irqEDvoKOV49445258 = -875443908;    int irqEDvoKOV31015682 = -594712764;    int irqEDvoKOV50438177 = -381137804;    int irqEDvoKOV28889690 = -277255469;    int irqEDvoKOV77278694 = -974692850;    int irqEDvoKOV15729885 = -704922150;    int irqEDvoKOV54901628 = -862856973;    int irqEDvoKOV95613414 = -193652817;    int irqEDvoKOV36971706 = -299044881;    int irqEDvoKOV2937069 = -769169901;    int irqEDvoKOV49428974 = -702504583;    int irqEDvoKOV96762530 = -109212150;    int irqEDvoKOV43335125 = -368365228;    int irqEDvoKOV33296758 = -919999543;    int irqEDvoKOV84384154 = -436146190;     irqEDvoKOV71314263 = irqEDvoKOV83947922;     irqEDvoKOV83947922 = irqEDvoKOV50256358;     irqEDvoKOV50256358 = irqEDvoKOV56756448;     irqEDvoKOV56756448 = irqEDvoKOV99226125;     irqEDvoKOV99226125 = irqEDvoKOV18278528;     irqEDvoKOV18278528 = irqEDvoKOV14391757;     irqEDvoKOV14391757 = irqEDvoKOV66347626;     irqEDvoKOV66347626 = irqEDvoKOV89887206;     irqEDvoKOV89887206 = irqEDvoKOV93112537;     irqEDvoKOV93112537 = irqEDvoKOV15242876;     irqEDvoKOV15242876 = irqEDvoKOV10112718;     irqEDvoKOV10112718 = irqEDvoKOV83945513;     irqEDvoKOV83945513 = irqEDvoKOV28717505;     irqEDvoKOV28717505 = irqEDvoKOV26658518;     irqEDvoKOV26658518 = irqEDvoKOV95893635;     irqEDvoKOV95893635 = irqEDvoKOV15327449;     irqEDvoKOV15327449 = irqEDvoKOV3605038;     irqEDvoKOV3605038 = irqEDvoKOV65816229;     irqEDvoKOV65816229 = irqEDvoKOV42211576;     irqEDvoKOV42211576 = irqEDvoKOV34763885;     irqEDvoKOV34763885 = irqEDvoKOV23006755;     irqEDvoKOV23006755 = irqEDvoKOV5595733;     irqEDvoKOV5595733 = irqEDvoKOV49872195;     irqEDvoKOV49872195 = irqEDvoKOV6517139;     irqEDvoKOV6517139 = irqEDvoKOV12015454;     irqEDvoKOV12015454 = irqEDvoKOV6348930;     irqEDvoKOV6348930 = irqEDvoKOV95516084;     irqEDvoKOV95516084 = irqEDvoKOV51328873;     irqEDvoKOV51328873 = irqEDvoKOV30148732;     irqEDvoKOV30148732 = irqEDvoKOV11293504;     irqEDvoKOV11293504 = irqEDvoKOV30988975;     irqEDvoKOV30988975 = irqEDvoKOV83092500;     irqEDvoKOV83092500 = irqEDvoKOV25790787;     irqEDvoKOV25790787 = irqEDvoKOV25586203;     irqEDvoKOV25586203 = irqEDvoKOV94082074;     irqEDvoKOV94082074 = irqEDvoKOV55285792;     irqEDvoKOV55285792 = irqEDvoKOV45852725;     irqEDvoKOV45852725 = irqEDvoKOV24182379;     irqEDvoKOV24182379 = irqEDvoKOV14866667;     irqEDvoKOV14866667 = irqEDvoKOV68304646;     irqEDvoKOV68304646 = irqEDvoKOV11781284;     irqEDvoKOV11781284 = irqEDvoKOV3073599;     irqEDvoKOV3073599 = irqEDvoKOV15115792;     irqEDvoKOV15115792 = irqEDvoKOV68638055;     irqEDvoKOV68638055 = irqEDvoKOV78279654;     irqEDvoKOV78279654 = irqEDvoKOV10254895;     irqEDvoKOV10254895 = irqEDvoKOV41998865;     irqEDvoKOV41998865 = irqEDvoKOV34737080;     irqEDvoKOV34737080 = irqEDvoKOV18175861;     irqEDvoKOV18175861 = irqEDvoKOV73991645;     irqEDvoKOV73991645 = irqEDvoKOV61848245;     irqEDvoKOV61848245 = irqEDvoKOV99973293;     irqEDvoKOV99973293 = irqEDvoKOV32654324;     irqEDvoKOV32654324 = irqEDvoKOV96901096;     irqEDvoKOV96901096 = irqEDvoKOV48307509;     irqEDvoKOV48307509 = irqEDvoKOV78352190;     irqEDvoKOV78352190 = irqEDvoKOV384164;     irqEDvoKOV384164 = irqEDvoKOV50239310;     irqEDvoKOV50239310 = irqEDvoKOV87210672;     irqEDvoKOV87210672 = irqEDvoKOV11929599;     irqEDvoKOV11929599 = irqEDvoKOV18875672;     irqEDvoKOV18875672 = irqEDvoKOV15018754;     irqEDvoKOV15018754 = irqEDvoKOV59738474;     irqEDvoKOV59738474 = irqEDvoKOV81819034;     irqEDvoKOV81819034 = irqEDvoKOV84253900;     irqEDvoKOV84253900 = irqEDvoKOV27020217;     irqEDvoKOV27020217 = irqEDvoKOV58154727;     irqEDvoKOV58154727 = irqEDvoKOV3131303;     irqEDvoKOV3131303 = irqEDvoKOV32576443;     irqEDvoKOV32576443 = irqEDvoKOV40607844;     irqEDvoKOV40607844 = irqEDvoKOV69474724;     irqEDvoKOV69474724 = irqEDvoKOV79422659;     irqEDvoKOV79422659 = irqEDvoKOV50949562;     irqEDvoKOV50949562 = irqEDvoKOV73906929;     irqEDvoKOV73906929 = irqEDvoKOV22982601;     irqEDvoKOV22982601 = irqEDvoKOV19933156;     irqEDvoKOV19933156 = irqEDvoKOV90479940;     irqEDvoKOV90479940 = irqEDvoKOV81234139;     irqEDvoKOV81234139 = irqEDvoKOV28237484;     irqEDvoKOV28237484 = irqEDvoKOV1760559;     irqEDvoKOV1760559 = irqEDvoKOV64350064;     irqEDvoKOV64350064 = irqEDvoKOV60779005;     irqEDvoKOV60779005 = irqEDvoKOV33153012;     irqEDvoKOV33153012 = irqEDvoKOV56157087;     irqEDvoKOV56157087 = irqEDvoKOV49445258;     irqEDvoKOV49445258 = irqEDvoKOV31015682;     irqEDvoKOV31015682 = irqEDvoKOV50438177;     irqEDvoKOV50438177 = irqEDvoKOV28889690;     irqEDvoKOV28889690 = irqEDvoKOV77278694;     irqEDvoKOV77278694 = irqEDvoKOV15729885;     irqEDvoKOV15729885 = irqEDvoKOV54901628;     irqEDvoKOV54901628 = irqEDvoKOV95613414;     irqEDvoKOV95613414 = irqEDvoKOV36971706;     irqEDvoKOV36971706 = irqEDvoKOV2937069;     irqEDvoKOV2937069 = irqEDvoKOV49428974;     irqEDvoKOV49428974 = irqEDvoKOV96762530;     irqEDvoKOV96762530 = irqEDvoKOV43335125;     irqEDvoKOV43335125 = irqEDvoKOV33296758;     irqEDvoKOV33296758 = irqEDvoKOV84384154;     irqEDvoKOV84384154 = irqEDvoKOV71314263;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void uUgnWovQnH7677103() {     int QkqsygnRNC58235941 = -576797395;    int QkqsygnRNC93595661 = -537484725;    int QkqsygnRNC42952373 = -3572196;    int QkqsygnRNC84619642 = -982030288;    int QkqsygnRNC49955213 = -565286;    int QkqsygnRNC31744205 = -248007166;    int QkqsygnRNC64437425 = -875116588;    int QkqsygnRNC49063975 = -991278781;    int QkqsygnRNC68272896 = -563289366;    int QkqsygnRNC66612923 = 98242247;    int QkqsygnRNC34923606 = -841970865;    int QkqsygnRNC79739194 = -155508144;    int QkqsygnRNC44571963 = -877713016;    int QkqsygnRNC72035614 = -554478320;    int QkqsygnRNC50371388 = -246814292;    int QkqsygnRNC31172794 = -455276714;    int QkqsygnRNC65167624 = 69143498;    int QkqsygnRNC32821796 = -874863091;    int QkqsygnRNC26975800 = -297125714;    int QkqsygnRNC98964403 = 18013692;    int QkqsygnRNC2229554 = -459058585;    int QkqsygnRNC61696714 = 14280542;    int QkqsygnRNC75242072 = -575490843;    int QkqsygnRNC26229145 = -649710084;    int QkqsygnRNC62482123 = -838955643;    int QkqsygnRNC6194354 = -720754975;    int QkqsygnRNC63239357 = -439627666;    int QkqsygnRNC96102634 = -760688788;    int QkqsygnRNC24231666 = -888095201;    int QkqsygnRNC9862371 = -291962566;    int QkqsygnRNC70413756 = -628008124;    int QkqsygnRNC35431005 = -328343354;    int QkqsygnRNC37365583 = -954676728;    int QkqsygnRNC55713885 = -70764944;    int QkqsygnRNC18365152 = -525211582;    int QkqsygnRNC27042290 = -634331525;    int QkqsygnRNC11255056 = -687597768;    int QkqsygnRNC18181102 = -235430520;    int QkqsygnRNC77413545 = -751416231;    int QkqsygnRNC42653742 = -712351341;    int QkqsygnRNC51466077 = 15293738;    int QkqsygnRNC96622209 = -455244023;    int QkqsygnRNC79523772 = -884881448;    int QkqsygnRNC33660472 = -445312548;    int QkqsygnRNC20622916 = -917943965;    int QkqsygnRNC80846576 = -648916139;    int QkqsygnRNC30904921 = -82436880;    int QkqsygnRNC44254808 = -389042270;    int QkqsygnRNC24962708 = -283006169;    int QkqsygnRNC10775441 = -729085448;    int QkqsygnRNC15349508 = -54699344;    int QkqsygnRNC66761287 = -335448000;    int QkqsygnRNC85114896 = -370628573;    int QkqsygnRNC87199293 = -505939351;    int QkqsygnRNC5447549 = -208507953;    int QkqsygnRNC96539226 = -491077938;    int QkqsygnRNC18353589 = -961993882;    int QkqsygnRNC16723228 = -353862113;    int QkqsygnRNC22137519 = -43074645;    int QkqsygnRNC43760860 = -279810311;    int QkqsygnRNC68504847 = -808379501;    int QkqsygnRNC68334790 = -14427800;    int QkqsygnRNC24832309 = -3183580;    int QkqsygnRNC58410525 = -171326800;    int QkqsygnRNC96199167 = -273749630;    int QkqsygnRNC99492601 = -413627512;    int QkqsygnRNC42373612 = -200831417;    int QkqsygnRNC88858078 = -706948073;    int QkqsygnRNC53670462 = 70733261;    int QkqsygnRNC23329098 = -612482768;    int QkqsygnRNC19917739 = -767678946;    int QkqsygnRNC46986522 = -695425983;    int QkqsygnRNC55408250 = -23446860;    int QkqsygnRNC84322058 = -584774374;    int QkqsygnRNC47498326 = -997280047;    int QkqsygnRNC5607345 = 96185437;    int QkqsygnRNC82172942 = -100838011;    int QkqsygnRNC41581601 = -30178296;    int QkqsygnRNC5606229 = -731766119;    int QkqsygnRNC81635547 = -90039505;    int QkqsygnRNC75289432 = -538318095;    int QkqsygnRNC18984549 = 49414603;    int QkqsygnRNC71139926 = -377682620;    int QkqsygnRNC13456226 = -59009753;    int QkqsygnRNC94512863 = -137263222;    int QkqsygnRNC3652469 = -192560124;    int QkqsygnRNC50316109 = -957714782;    int QkqsygnRNC50166290 = -348737377;    int QkqsygnRNC50266336 = -862256991;    int QkqsygnRNC21825925 = 65866355;    int QkqsygnRNC8688702 = -672337644;    int QkqsygnRNC94531827 = -233735656;    int QkqsygnRNC96043582 = -92355875;    int QkqsygnRNC33652686 = -371605920;    int QkqsygnRNC74148894 = -903971841;    int QkqsygnRNC83131286 = -970278462;    int QkqsygnRNC71789901 = -352060443;    int QkqsygnRNC21113247 = -613554648;    int QkqsygnRNC37461304 = -71562918;    int QkqsygnRNC21130315 = -576797395;     QkqsygnRNC58235941 = QkqsygnRNC93595661;     QkqsygnRNC93595661 = QkqsygnRNC42952373;     QkqsygnRNC42952373 = QkqsygnRNC84619642;     QkqsygnRNC84619642 = QkqsygnRNC49955213;     QkqsygnRNC49955213 = QkqsygnRNC31744205;     QkqsygnRNC31744205 = QkqsygnRNC64437425;     QkqsygnRNC64437425 = QkqsygnRNC49063975;     QkqsygnRNC49063975 = QkqsygnRNC68272896;     QkqsygnRNC68272896 = QkqsygnRNC66612923;     QkqsygnRNC66612923 = QkqsygnRNC34923606;     QkqsygnRNC34923606 = QkqsygnRNC79739194;     QkqsygnRNC79739194 = QkqsygnRNC44571963;     QkqsygnRNC44571963 = QkqsygnRNC72035614;     QkqsygnRNC72035614 = QkqsygnRNC50371388;     QkqsygnRNC50371388 = QkqsygnRNC31172794;     QkqsygnRNC31172794 = QkqsygnRNC65167624;     QkqsygnRNC65167624 = QkqsygnRNC32821796;     QkqsygnRNC32821796 = QkqsygnRNC26975800;     QkqsygnRNC26975800 = QkqsygnRNC98964403;     QkqsygnRNC98964403 = QkqsygnRNC2229554;     QkqsygnRNC2229554 = QkqsygnRNC61696714;     QkqsygnRNC61696714 = QkqsygnRNC75242072;     QkqsygnRNC75242072 = QkqsygnRNC26229145;     QkqsygnRNC26229145 = QkqsygnRNC62482123;     QkqsygnRNC62482123 = QkqsygnRNC6194354;     QkqsygnRNC6194354 = QkqsygnRNC63239357;     QkqsygnRNC63239357 = QkqsygnRNC96102634;     QkqsygnRNC96102634 = QkqsygnRNC24231666;     QkqsygnRNC24231666 = QkqsygnRNC9862371;     QkqsygnRNC9862371 = QkqsygnRNC70413756;     QkqsygnRNC70413756 = QkqsygnRNC35431005;     QkqsygnRNC35431005 = QkqsygnRNC37365583;     QkqsygnRNC37365583 = QkqsygnRNC55713885;     QkqsygnRNC55713885 = QkqsygnRNC18365152;     QkqsygnRNC18365152 = QkqsygnRNC27042290;     QkqsygnRNC27042290 = QkqsygnRNC11255056;     QkqsygnRNC11255056 = QkqsygnRNC18181102;     QkqsygnRNC18181102 = QkqsygnRNC77413545;     QkqsygnRNC77413545 = QkqsygnRNC42653742;     QkqsygnRNC42653742 = QkqsygnRNC51466077;     QkqsygnRNC51466077 = QkqsygnRNC96622209;     QkqsygnRNC96622209 = QkqsygnRNC79523772;     QkqsygnRNC79523772 = QkqsygnRNC33660472;     QkqsygnRNC33660472 = QkqsygnRNC20622916;     QkqsygnRNC20622916 = QkqsygnRNC80846576;     QkqsygnRNC80846576 = QkqsygnRNC30904921;     QkqsygnRNC30904921 = QkqsygnRNC44254808;     QkqsygnRNC44254808 = QkqsygnRNC24962708;     QkqsygnRNC24962708 = QkqsygnRNC10775441;     QkqsygnRNC10775441 = QkqsygnRNC15349508;     QkqsygnRNC15349508 = QkqsygnRNC66761287;     QkqsygnRNC66761287 = QkqsygnRNC85114896;     QkqsygnRNC85114896 = QkqsygnRNC87199293;     QkqsygnRNC87199293 = QkqsygnRNC5447549;     QkqsygnRNC5447549 = QkqsygnRNC96539226;     QkqsygnRNC96539226 = QkqsygnRNC18353589;     QkqsygnRNC18353589 = QkqsygnRNC16723228;     QkqsygnRNC16723228 = QkqsygnRNC22137519;     QkqsygnRNC22137519 = QkqsygnRNC43760860;     QkqsygnRNC43760860 = QkqsygnRNC68504847;     QkqsygnRNC68504847 = QkqsygnRNC68334790;     QkqsygnRNC68334790 = QkqsygnRNC24832309;     QkqsygnRNC24832309 = QkqsygnRNC58410525;     QkqsygnRNC58410525 = QkqsygnRNC96199167;     QkqsygnRNC96199167 = QkqsygnRNC99492601;     QkqsygnRNC99492601 = QkqsygnRNC42373612;     QkqsygnRNC42373612 = QkqsygnRNC88858078;     QkqsygnRNC88858078 = QkqsygnRNC53670462;     QkqsygnRNC53670462 = QkqsygnRNC23329098;     QkqsygnRNC23329098 = QkqsygnRNC19917739;     QkqsygnRNC19917739 = QkqsygnRNC46986522;     QkqsygnRNC46986522 = QkqsygnRNC55408250;     QkqsygnRNC55408250 = QkqsygnRNC84322058;     QkqsygnRNC84322058 = QkqsygnRNC47498326;     QkqsygnRNC47498326 = QkqsygnRNC5607345;     QkqsygnRNC5607345 = QkqsygnRNC82172942;     QkqsygnRNC82172942 = QkqsygnRNC41581601;     QkqsygnRNC41581601 = QkqsygnRNC5606229;     QkqsygnRNC5606229 = QkqsygnRNC81635547;     QkqsygnRNC81635547 = QkqsygnRNC75289432;     QkqsygnRNC75289432 = QkqsygnRNC18984549;     QkqsygnRNC18984549 = QkqsygnRNC71139926;     QkqsygnRNC71139926 = QkqsygnRNC13456226;     QkqsygnRNC13456226 = QkqsygnRNC94512863;     QkqsygnRNC94512863 = QkqsygnRNC3652469;     QkqsygnRNC3652469 = QkqsygnRNC50316109;     QkqsygnRNC50316109 = QkqsygnRNC50166290;     QkqsygnRNC50166290 = QkqsygnRNC50266336;     QkqsygnRNC50266336 = QkqsygnRNC21825925;     QkqsygnRNC21825925 = QkqsygnRNC8688702;     QkqsygnRNC8688702 = QkqsygnRNC94531827;     QkqsygnRNC94531827 = QkqsygnRNC96043582;     QkqsygnRNC96043582 = QkqsygnRNC33652686;     QkqsygnRNC33652686 = QkqsygnRNC74148894;     QkqsygnRNC74148894 = QkqsygnRNC83131286;     QkqsygnRNC83131286 = QkqsygnRNC71789901;     QkqsygnRNC71789901 = QkqsygnRNC21113247;     QkqsygnRNC21113247 = QkqsygnRNC37461304;     QkqsygnRNC37461304 = QkqsygnRNC21130315;     QkqsygnRNC21130315 = QkqsygnRNC58235941;}
// Junk Finished
