#include "hooks.hpp"
#include <intrin.h>  
#include "render.hpp"
#include "menu.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "features/bhop.hpp"
#include "features/chams.hpp"
#include "features/visuals.hpp"
#include "features/glow.hpp"
#include <dos.h>
#include <conio.h>
#include "RuntimeSaver.h"

namespace Hooks {

	void Initialize()
	{
		hlclient_hook.setup(g_CHLClient);
		direct3d_hook.setup(g_D3DDevice9);
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		sound_hook.setup(g_EngineSound);
		mdlrender_hook.setup(g_MdlRender);
		clientmode_hook.setup(g_ClientMode);
		ConVar* sv_cheats_con = g_CVar->FindVar("sv_cheats");
		sv_cheats.setup(sv_cheats_con);

		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);
		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);
		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);
		sound_hook.hook_index(index::EmitSound1, hkEmitSound1);
		vguisurf_hook.hook_index(index::LockCursor, hkLockCursor);
		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);
		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(index::OverrideView, hkOverrideView);
		sv_cheats.hook_index(index::SvCheatsGetBool, hkSvCheatsGetBool);
		clientmode_hook.hook_index(index::GetViewmodelFOV, hkGetViewmodelFOV);
	}
	//--------------------------------------------------------------------------------
	void Shutdown()
	{
		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();
		sound_hook.unhook_all();
		sv_cheats.unhook_all();

		Glow::Get().Shutdown();
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkEndScene(IDirect3DDevice9* pDevice)
	{
		static auto oEndScene = direct3d_hook.get_original<decltype(&hkEndScene)>(index::EndScene);

		static auto mat_ambient_light_r = g_CVar->FindVar("mat_ambient_light_r");
		static auto mat_ambient_light_g = g_CVar->FindVar("mat_ambient_light_g");
		static auto mat_ambient_light_b = g_CVar->FindVar("mat_ambient_light_b");
		static auto cl_crosshair_recoil = g_CVar->FindVar("cl_crosshair_recoil");

		mat_ambient_light_r->SetValue(g_Options.mat_ambient_light_r);
		mat_ambient_light_g->SetValue(g_Options.mat_ambient_light_g);
		mat_ambient_light_b->SetValue(g_Options.mat_ambient_light_b);
		
		cl_crosshair_recoil->SetValue(g_Options.esp_crosshair_recoil);

		DWORD colorwrite, srgbwrite;
		IDirect3DVertexDeclaration9* vert_dec = nullptr;
		IDirect3DVertexShader9* vert_shader = nullptr;
		DWORD dwOld_D3DRS_COLORWRITEENABLE = NULL;
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorwrite);
		pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbwrite);

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		//removes the source engine color correction
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);

		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->GetVertexDeclaration(&vert_dec);
		pDevice->GetVertexShader(&vert_shader);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);

		
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();





		auto esp_drawlist = Render::Get().RenderScene();

		Menu::Get().Render();
	

		ImGui::Render(esp_drawlist);

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorwrite);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbwrite);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
		pDevice->SetVertexDeclaration(vert_dec);
		pDevice->SetVertexShader(vert_shader);

		return oEndScene(pDevice);
	}
	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		static auto oReset = direct3d_hook.get_original<decltype(&hkReset)>(index::Reset);

		Menu::Get().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);
		if (hr >= 0) Menu::Get().OnDeviceReset();

		return hr;
	}
	//--------------------------------------------------------------------------------
	float MaxRotation(CCSGOPlayerAnimState* AnimState)
	{
		auto animstate = uintptr_t(AnimState);
		float duckammount = *(float*)(animstate + 0XA4);
		float speedfraction = std::max(0.f, std::min(*reinterpret_cast<float*>(animstate + 0xF8), 1.f));

		float unk1 = ((*reinterpret_cast<float*>(animstate + 0x11C) * -0.3f) - 0.2f) * speedfraction;
		float unk2 = unk1 + 1.f;

		if (duckammount > 0.f)
		{
			float speedfactor = std::max(0.f, std::min(1.f, *reinterpret_cast<float*>(animstate + 0xFC)));
			unk2 += ((duckammount * speedfactor) * (0.5f - unk2));
		}
		return *(float*)(animstate + 0x334) * unk2;
	}

	void __stdcall hkCreateMove(int sequence_number, float input_sample_frametime, bool active, bool& bSendPacket)
	{
		static auto oCreateMove = hlclient_hook.get_original<decltype(&hkCreateMove_Proxy)>(index::CreateMove);

		oCreateMove(g_CHLClient, 0, sequence_number, input_sample_frametime, active);

		auto cmd = g_Input->GetUserCmd(sequence_number);
		auto verified = g_Input->GetVerifiedCmd(sequence_number);

		if (!cmd || !cmd->command_number) return;
		
		if (Menu::Get().IsVisible()) cmd->buttons &= ~IN_ATTACK;
		if (g_Options.misc_bhop) BunnyHop::OnCreateMove(cmd);

		if ( g_Options.desync )
		{
			if ( !bSendPacket )
			{
				cmd->viewangles.yaw += 180.f;
			}
			else 
			{
				//Desync is here.
				cmd->viewangles.yaw -= MaxRotation( g_LocalPlayer->GetPlayerAnimState(  ) );

			}

		}
		Math::NormalizeAngles(cmd->viewangles);
		Math::ClampAngles(cmd->viewangles);

		if (g_Options.misc_clantag_maple)
		{
			static int i = 0;
			switch (i) {
			case 0:
				Utils::SetClantag("");
				break;
			case 10:
				Utils::SetClantag("M");
				break;
			case 20:
				Utils::SetClantag("M");
				break;
			case 30:
				Utils::SetClantag("M@");
				break;
			case 40:
				Utils::SetClantag("M@");
				break;
			case 50:
				Utils::SetClantag("Ma");
				break;
			case 60:
				Utils::SetClantag("Ma");
				break;
			case 70:
				Utils::SetClantag("Map");
				break;
			case 80:
				Utils::SetClantag("Map");
				break;
			case 90:
				Utils::SetClantag("Map|");
				break;
			case 100:
				Utils::SetClantag("Map|");
				break;
			case 110:
				Utils::SetClantag("Mapl");
				break;
			case 120:
				Utils::SetClantag("Mapl");
				break;
			case 130:
				Utils::SetClantag("Mapl3");
				break;
			case 140:
				Utils::SetClantag("Mapl3");
				break;
			case 150:
				Utils::SetClantag("M@p|3");
				break;
			case 160:
				Utils::SetClantag("M@p|3");
				break;
			case 170:
				Utils::SetClantag("Maple");
			case 180:
				Utils::SetClantag("Maple");
			case 190:
				Utils::SetClantag("Map|");
			case 200:
				Utils::SetClantag("Map|");
			case 210:
				Utils::SetClantag("Mapl");
			case 220:
				Utils::SetClantag("Mapl");
			case 230:
				Utils::SetClantag("Map");
			case 240:
				Utils::SetClantag("Map");
			case 250:
				Utils::SetClantag("M@");
			case 260:
				Utils::SetClantag("M@");
			case 270:
				Utils::SetClantag("Ma");
			case 280:
				Utils::SetClantag("Ma");
			case 290:
				Utils::SetClantag("M");
				i = 0;
				return;
				break;
			}
			i++;
		}
		if (!g_Options.misc_clantag_enable)
		{
				Utils::SetClantag("");
		}
		if (g_Options.misc_maple_clantag_2)
		{
			static int i = 0;
			switch (i) {
			case 0:
				Utils::SetClantag("");
				break;
			case 10:
				Utils::SetClantag("M");
				break;
			case 20:
				Utils::SetClantag("M");
				break;
			case 30:
				Utils::SetClantag("Ma");
				break;
			case 40:
				Utils::SetClantag("Ma");
				break;
			case 50:
				Utils::SetClantag("Map");
				break;
			case 60:
				Utils::SetClantag("Map");
				break;
			case 70:
				Utils::SetClantag("Mapl");
				break;
			case 80:
				Utils::SetClantag("Mapl");
				break;
			case 90:
				Utils::SetClantag("Maple");
				break;
			case 100:
				Utils::SetClantag("Maple");
				break;
			case 110:
				Utils::SetClantag("Maple.");
				break;
			case 120:
				Utils::SetClantag("Maple.");
				break;
			case 130:
				Utils::SetClantag("Maple.p");
				break;
			case 140:
				Utils::SetClantag("Maple.p");
				break;
			case 150:
				Utils::SetClantag("Maple.pw");
				break;
			case 160:
				Utils::SetClantag("Maple.pw");
				break;
			case 170:
				Utils::SetClantag("Maple.pw");
				
			case 180:
				Utils::SetClantag("Maple.pw");
				
			case 190:
				Utils::SetClantag("Maple.pw");
				
			case 200:
				Utils::SetClantag("Maple.pw");
				
			case 210:
				Utils::SetClantag("Maple.p");
				
			case 220:
				Utils::SetClantag("Maple.p");
				
			case 230:
				Utils::SetClantag("Maple.");
				
			case 240:
				Utils::SetClantag("Maple.");
				
			case 250:
				Utils::SetClantag("Maple");
				
			case 260:
				Utils::SetClantag("Maple");
				
			case 270:
				Utils::SetClantag("Mapl");
				
			case 280:
				Utils::SetClantag("Mapl");
				
			case 290:
				Utils::SetClantag("Map");
				
			case 300:
				Utils::SetClantag("Map");
				
			case 310:
				Utils::SetClantag("Ma");
				
			case 320:
				Utils::SetClantag("Ma");
				
			case 330:
				Utils::SetClantag("M");
			case 340:
				Utils::SetClantag("M");
				
			case 350:
				Utils::SetClantag("");
			case 360:
				Utils::SetClantag("");
			case 370:
				Utils::SetClantag("");
			case 380:
				Utils::SetClantag("");
			case 390:
				Utils::SetClantag("");
			case 400:
				Utils::SetClantag("");
				i = 0;
				return;
				break;
			}
			i++;
		}

		if (InputSys::Get().WasKeyPressed(g_Options.vis_misc_thirdperson_hotkey))
		{
			g_Options.misc_thirdperson != !g_Options.misc_thirdperson;
		}


	

		// https://github.com/spirthack/Maple/issues/69
		if (g_Options.misc_showranks && cmd->buttons & IN_SCORE) // rank revealer will work even after unhooking, idk how to "hide" ranks  again
			g_CHLClient->DispatchUserMessage(CS_UM_ServerRankRevealAll, 0, 0, nullptr);


		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum();
	}
	//--------------------------------------------------------------------------------
	__declspec(naked) void __fastcall hkCreateMove_Proxy(void* _this, int, int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx; not sure if we need this
			push esp
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}
	}

	// below is our no scope lmfao

	//--------------------------------------------------------------------------------
	void __fastcall hkPaintTraverse(void* _this, int edx, vgui::VPANEL panel, bool forceRepaint, bool allowForce)
	{
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<decltype(&hkPaintTraverse)>(index::PaintTraverse);

		oPaintTraverse(g_VGuiPanel, edx, panel, forceRepaint, allowForce);

		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (panelId == panel) 
		{
			//Ignore 50% cuz it called very often
			static bool bSkip = false;
			bSkip = !bSkip;

			if (bSkip)
				return;

			Render::Get().BeginScene();

		}
	}


	//--------------------------------------------------------------------------------

	//------------------------------------------------------------------------------- ignore this i was going to paste but im bad coder

	//------------------------------------------------------------------------------- Usage
	void __fastcall hkEmitSound1(void* _this, int edx, IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSoundEntry, unsigned int nSoundEntryHash, const char *pSample, float flVolume, int nSeed, float flAttenuation, int iFlags, int iPitch, const Vector* pOrigin, const Vector* pDirection, void* pUtlVecOrigins, bool bUpdatePositions, float soundtime, int speakerentity, int unk) {
		static auto ofunc = sound_hook.get_original<decltype(&hkEmitSound1)>(index::EmitSound1);


		if (!strcmp(pSoundEntry, "UIPanorama.popup_accept_match_beep")) {
			static auto fnAccept = reinterpret_cast<bool(__stdcall*)(const char*)>(Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12"));

			if (fnAccept) {

				fnAccept("");

				//This will flash the CSGO window on the taskbar
				//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
				FLASHWINFO fi;
				fi.cbSize = sizeof(FLASHWINFO);
				fi.hwnd = InputSys::Get().GetMainWindow();
				fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
				fi.uCount = 0;
				fi.dwTimeout = 0;
				FlashWindowEx(&fi);
			}
		}

		ofunc(g_EngineSound, edx, filter, iEntIndex, iChannel, pSoundEntry, nSoundEntryHash, pSample, flVolume, nSeed, flAttenuation, iFlags, iPitch, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity, unk);

	}
	//--------------------------------------------------------------------------------
	int __fastcall hkDoPostScreenEffects(void* _this, int edx, int a1)
	{
		static auto oDoPostScreenEffects = clientmode_hook.get_original<decltype(&hkDoPostScreenEffects)>(index::DoPostScreenSpaceEffects);

		if (g_LocalPlayer && g_Options.glow_enabled)
			Glow::Get().Run();

		return oDoPostScreenEffects(g_ClientMode, edx, a1);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkFrameStageNotify(void* _this, int edx, ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<decltype(&hkFrameStageNotify)>(index::FrameStageNotify);
		// may be u will use it lol
		ofunc(g_CHLClient, edx, stage);
	}
	//--------------------------------------------------------------------------------
	void __fastcall hkOverrideView(void* _this, int edx, CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<decltype(&hkOverrideView)>(index::OverrideView);

		if (g_EngineClient->IsInGame() && vsView)
			Visuals::Get().ThirdPerson();

		ofunc(g_ClientMode, edx, vsView);

	}
	//--------------------------------------------------------------------------------
	void __fastcall hkLockCursor(void* _this)
	{
		static auto ofunc = vguisurf_hook.get_original<decltype(&hkLockCursor)>(index::LockCursor);

		if (Menu::Get().IsVisible()) {
			g_VGuiSurface->UnlockCursor();
			g_InputSystem->ResetInputState();
			return;
		}
		ofunc(g_VGuiSurface);

	}
	//--------------------------------------------------------------------------------
	void __fastcall hkDrawModelExecute(void* _this, int edx, IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<decltype(&hkDrawModelExecute)>(index::DrawModelExecute);

		if (g_MdlRender->IsForcedMaterialOverride() &&
			!strstr(pInfo.pModel->szName, "arms") &&
			!strstr(pInfo.pModel->szName, "weapons/v_")) {
			return ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);
		}

		Chams::Get().OnDrawModelExecute(ctx, state, pInfo, pCustomBoneToWorld);

		ofunc(_this, edx, ctx, state, pInfo, pCustomBoneToWorld);

		g_MdlRender->ForcedMaterialOverride(nullptr);
	}
	
	bool __fastcall hkSvCheatsGetBool(PVOID pConVar, void* edx)
	{
		static auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "85 C0 75 30 38 86");
		static auto ofunc = sv_cheats.get_original<bool(__thiscall *)(PVOID)>(index::SvCheatsGetBool);
		if (!ofunc)
			return false;

		if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think))
			return true;
		return ofunc(pConVar);
	}

	float __stdcall hkGetViewmodelFOV()
	{
		static auto ofunc = clientmode_hook.get_original<decltype(&hkGetViewmodelFOV)>(index::GetViewmodelFOV);
		while (!g_EngineClient->IsTakingScreenshot() && g_EngineClient->IsInGame() && !g_LocalPlayer->m_bIsScoped())
		{
			if (g_Options.viewmodel_fov) return ofunc() + g_Options.viewmodel_size;
			else return ofunc();

		} return ofunc();
	}


	bool __fastcall hkgrenadepreviewGetBool(PVOID pConVar, void* edx)
	{
		static auto ofunc = sv_cheats.get_original<bool(__thiscall*)(PVOID)>(13);
		if (!ofunc)
			return false;

		if (g_Options.esp_grenade_prediction)
			return true;
		return ofunc(pConVar);
	}

}