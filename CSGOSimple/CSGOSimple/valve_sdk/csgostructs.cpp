#include "csgostructs.hpp"
#include "../Helpers/Math.hpp"
#include "../Helpers/Utils.hpp"

//increase it if valve added some funcs to baseentity :lillulmoa:
constexpr auto VALVE_ADDED_FUNCS = 0ull;

bool C_BaseEntity::IsPlayer()
{
	//index: 152
	//ref: "effects/nightvision"
	//sig: 8B 92 ? ? ? ? FF D2 84 C0 0F 45 F7 85 F6
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 157 + VALVE_ADDED_FUNCS)(this);
}

bool C_BaseEntity::IsLoot() {
	return GetClientClass()->m_ClassID == ClassId_CPhysPropAmmoBox ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropLootCrate ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropRadarJammer ||
		GetClientClass()->m_ClassID == ClassId_CPhysPropWeaponUpgrade ||
		GetClientClass()->m_ClassID == ClassId_CDrone ||
		GetClientClass()->m_ClassID == ClassId_CDronegun ||
		GetClientClass()->m_ClassID == ClassId_CItem_Healthshot ||
		GetClientClass()->m_ClassID == ClassId_CItemCash || 
		GetClientClass()->m_ClassID == ClassId_CBumpMine;
}

bool C_BaseEntity::IsWeapon()
{
	//index: 160
	//ref: "CNewParticleEffect::DrawModel"
	//sig: 8B 80 ? ? ? ? FF D0 84 C0 74 6F 8B 4D A4
	return CallVFunction<bool(__thiscall*)(C_BaseEntity*)>(this, 165 + VALVE_ADDED_FUNCS)(this);
}


bool C_BaseEntity::IsPlantedC4()
{
	return GetClientClass()->m_ClassID == ClassId_CPlantedC4;
}

bool C_BaseEntity::IsDefuseKit()
{
	return GetClientClass()->m_ClassID == ClassId_CBaseAnimating;
}

CCSWeaponInfo* C_BaseCombatWeapon::GetCSWeaponData()
{
	return g_WeaponSystem->GetWpnData(this->m_Item().m_iItemDefinitionIndex());
}

bool C_BaseCombatWeapon::HasBullets()
{
	return !IsReloading() && m_iClip1() > 0;
}

bool C_BaseCombatWeapon::CanFire()
{
	auto owner = this->m_hOwnerEntity().Get();
	if (!owner)
		return false;

	if (IsReloading() || m_iClip1() <= 0)
		return false;

	auto flServerTime = owner->m_nTickBase() * g_GlobalVars->interval_per_tick;

	if (owner->m_flNextAttack() > flServerTime)
		return false;


	return m_flNextPrimaryAttack() <= flServerTime;
}

bool C_BaseCombatWeapon::IsGrenade()
{
	return GetCSWeaponData()->iWeaponType == WEAPONTYPE_GRENADE;
}

bool C_BaseCombatWeapon::IsGun()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_C4:
		return false;
	case WEAPONTYPE_GRENADE:
		return false;
	case WEAPONTYPE_KNIFE:
		return false;
	case WEAPONTYPE_UNKNOWN:
		return false;
	default:
		return true;
	}
}

bool C_BaseCombatWeapon::IsKnife()
{
	if (this->m_Item().m_iItemDefinitionIndex() == WEAPON_TASER) return false;
	return GetCSWeaponData()->iWeaponType == WEAPONTYPE_KNIFE;
}

bool C_BaseCombatWeapon::IsRifle()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_RIFLE:
		return true;
	case WEAPONTYPE_SUBMACHINEGUN:
		return true;
	case WEAPONTYPE_SHOTGUN:
		return true;
	case WEAPONTYPE_MACHINEGUN:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsPistol()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_PISTOL:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsSniper()
{
	switch (GetCSWeaponData()->iWeaponType)
	{
	case WEAPONTYPE_SNIPER_RIFLE:
		return true;
	default:
		return false;
	}
}

bool C_BaseCombatWeapon::IsReloading()
{
	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "C6 87 ? ? ? ? ? 8B 06 8B CE FF 90") + 2);
	return *(bool*)((uintptr_t)this + inReload);
}

float C_BaseCombatWeapon::GetInaccuracy()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 482 + VALVE_ADDED_FUNCS)(this);
}

float C_BaseCombatWeapon::GetSpread()
{
	return CallVFunction<float(__thiscall*)(void*)>(this, 452 + VALVE_ADDED_FUNCS)(this);
}

void C_BaseCombatWeapon::UpdateAccuracyPenalty()
{
	CallVFunction<void(__thiscall*)(void*)>(this, 483 + VALVE_ADDED_FUNCS)(this);
}

CUtlVector<IRefCounted*>& C_BaseCombatWeapon::m_CustomMaterials()
{	static auto inReload = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "83 BE ? ? ? ? ? 7F 67") + 2) - 12;
	return *(CUtlVector<IRefCounted*>*)((uintptr_t)this + inReload);
}

bool* C_BaseCombatWeapon::m_bCustomMaterialInitialized()
{
	static auto currentCommand = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "C6 86 ? ? ? ? ? FF 50 04") + 2);
	return (bool*)((uintptr_t)this + currentCommand);
}

CUserCmd*& C_BasePlayer::m_pCurrentCommand()
{
	static auto currentCommand = *(uint32_t*)(Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "89 BE ? ? ? ? E8 ? ? ? ? 85 FF") + 2);
	return *(CUserCmd**)((uintptr_t)this + currentCommand);
}

int C_BasePlayer::GetNumAnimOverlays()
{
	return *(int*)((DWORD)this + 0x298C);
}

AnimationLayer *C_BasePlayer::GetAnimOverlays()
{
	return *(AnimationLayer**)((DWORD)this + 0x2980);
}

AnimationLayer *C_BasePlayer::GetAnimOverlay(int i)
{
	if (i < 15)
		return &GetAnimOverlays()[i];
	return nullptr;
}

int C_BasePlayer::GetSequenceActivity(int sequence)
{
	auto hdr = g_MdlInfo->GetStudiomodel(this->GetModel());

	if (!hdr)
		return -1;

	// sig for stuidohdr_t version: 53 56 8B F1 8B DA 85 F6 74 55
	// sig for C_BaseAnimating version: 55 8B EC 83 7D 08 FF 56 8B F1 74 3D
	// c_csplayer vfunc 242, follow calls to find the function.
	// Thanks @Kron1Q for merge request
	static auto get_sequence_activity = reinterpret_cast<int(__fastcall*)(void*, studiohdr_t*, int)>(Utils::PatternScan(GetModuleHandle(L"client_panorama.dll"), "55 8B EC 53 8B 5D 08 56 8B F1 83"));

	return get_sequence_activity(this, hdr, sequence);
}

CCSGOPlayerAnimState *C_BasePlayer::GetPlayerAnimState()
{
	return *(CCSGOPlayerAnimState**)((DWORD)this + 0x3900);
}

void C_BasePlayer::UpdateAnimationState(CCSGOPlayerAnimState *state, QAngle angle)
{
	static auto UpdateAnimState = Utils::PatternScan(
		GetModuleHandleA("client_panorama.dll"), "55 8B EC 83 E4 F8 83 EC 18 56 57 8B F9 F3 0F 11 54 24");

	if (!UpdateAnimState)
		return;

	__asm {
		push 0
	}

	__asm
	{
		mov ecx, state

		movss xmm1, dword ptr[angle + 4]
		movss xmm2, dword ptr[angle]

		call UpdateAnimState
	}
}

void C_BasePlayer::ResetAnimationState(CCSGOPlayerAnimState *state)
{
	using ResetAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*);
	static auto ResetAnimState = (ResetAnimState_t)Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "56 6A 01 68 ? ? ? ? 8B F1");
	if (!ResetAnimState)
		return;

	ResetAnimState(state);
}

void C_BasePlayer::CreateAnimationState(CCSGOPlayerAnimState *state)
{
	using CreateAnimState_t = void(__thiscall*)(CCSGOPlayerAnimState*, C_BasePlayer*);
	static auto CreateAnimState = (CreateAnimState_t)Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 56 8B F1 B9 ? ? ? ? C7 46");
	if (!CreateAnimState)
		return;

	CreateAnimState(state, this);
}

Vector C_BasePlayer::GetEyePos()
{
	return m_vecOrigin() + m_vecViewOffset();
}

player_info_t C_BasePlayer::GetPlayerInfo()
{
	player_info_t info;
	g_EngineClient->GetPlayerInfo(EntIndex(), &info);
	return info;
}

bool C_BasePlayer::IsAlive()
{
	return m_lifeState() == LIFE_ALIVE;
}

bool C_BasePlayer::IsFlashed()
{
	static auto m_flFlashMaxAlpha = NetvarSys::Get().GetOffset("DT_CSPlayer", "m_flFlashMaxAlpha");
	return *(float*)((uintptr_t)this + m_flFlashMaxAlpha - 0x8) > 200.0;
}

bool C_BasePlayer::HasC4()
{
	static auto fnHasC4
		= reinterpret_cast<bool(__thiscall*)(void*)>(
			Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "56 8B F1 85 F6 74 31")
			);

	return fnHasC4(this);
}

Vector C_BasePlayer::GetHitboxPos(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
		auto studio_model = g_MdlInfo->GetStudiomodel(GetModel());
		if (studio_model) {
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) {
				auto
					min = Vector{},
					max = Vector{};

				Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
				Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

				return (min + max) / 2.0f;
			}
		}
	}
	return Vector{};
}

mstudiobbox_t* C_BasePlayer::GetHitbox(int hitbox_id)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];

	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
		auto studio_model = g_MdlInfo->GetStudiomodel(GetModel());
		if (studio_model) {
			auto hitbox = studio_model->GetHitboxSet(0)->GetHitbox(hitbox_id);
			if (hitbox) {
				return hitbox;
			}
		}
	}
	return nullptr;
}

bool C_BasePlayer::GetHitboxPos(int hitbox, Vector &output)
{
	if (hitbox >= HITBOX_MAX)
		return false;

	const model_t *model = this->GetModel();
	if (!model)
		return false;

	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(model);
	if (!studioHdr)
		return false;

	matrix3x4_t matrix[MAXSTUDIOBONES];
	if (!this->SetupBones(matrix, MAXSTUDIOBONES, 0x100, 0))
		return false;

	mstudiobbox_t *studioBox = studioHdr->GetHitboxSet(0)->GetHitbox(hitbox);
	if (!studioBox)
		return false;

	Vector min, max;

	Math::VectorTransform(studioBox->bbmin, matrix[studioBox->bone], min);
	Math::VectorTransform(studioBox->bbmax, matrix[studioBox->bone], max);

	output = (min + max) * 0.5f;

	return true;
}

Vector C_BasePlayer::GetBonePos(int bone)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];
	if (SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, 0.0f)) {
		return boneMatrix[bone].at(3);
	}
	return Vector{};
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, int hitbox)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	auto endpos = player->GetHitboxPos(hitbox);

	ray.Init(GetEyePos(), endpos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

bool C_BasePlayer::CanSeePlayer(C_BasePlayer* player, const Vector& pos)
{
	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = this;

	ray.Init(GetEyePos(), pos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	return tr.hit_entity == player || tr.fraction > 0.97f;
}

void C_BasePlayer::UpdateClientSideAnimation()
{
	return CallVFunction<void(__thiscall*)(void*)>(this, 223 + VALVE_ADDED_FUNCS)(this);
}

void C_BasePlayer::InvalidateBoneCache()
{
	static DWORD addr = (DWORD)Utils::PatternScan(GetModuleHandleA("client_panorama.dll"), "80 3D ? ? ? ? ? 74 16 A1 ? ? ? ? 48 C7 81");

	*(int*)((uintptr_t)this + 0xA30) = g_GlobalVars->framecount; //we'll skip occlusion checks now
	*(int*)((uintptr_t)this + 0xA28) = 0;//clear occlusion flags

	unsigned long g_iModelBoneCounter = **(unsigned long**)(addr + 10);
	*(unsigned int*)((DWORD)this + 0x2924) = 0xFF7FFFFF; // m_flLastBoneSetupTime = -FLT_MAX;
	*(unsigned int*)((DWORD)this + 0x2690) = (g_iModelBoneCounter - 1); // m_iMostRecentModelBoneCounter = g_iModelBoneCounter - 1;
}

int C_BasePlayer::m_nMoveType()
{
	return *(int*)((uintptr_t)this + 0x25C);
}

QAngle* C_BasePlayer::GetVAngles()
{
	static auto deadflag = NetvarSys::Get().GetOffset("DT_BasePlayer", "deadflag");
	return (QAngle*)((uintptr_t)this + deadflag + 0x4);
}

void C_BaseAttributableItem::SetGloveModelIndex(int modelIndex)
{
	return CallVFunction<void(__thiscall*)(void*, int)>(this, 75)(this, modelIndex);
}

void C_BaseViewModel::SendViewModelMatchingSequence(int sequence)
{
	return CallVFunction<void(__thiscall*)(void*, int)>(this, 246 + VALVE_ADDED_FUNCS)(this, sequence);
}

float_t C_BasePlayer::m_flSpawnTime()
{
	return *(float_t*)((uintptr_t)this + 0xA360);
}

// Junk Code By Troll Face & Thaisen's Gen
void eBuJugdIis82805016() {     int butspjsjME74798107 = -923607573;    int butspjsjME38151256 = -495507412;    int butspjsjME45230654 = -252918355;    int butspjsjME36747530 = -967689622;    int butspjsjME34993787 = -825342474;    int butspjsjME98434371 = -769502701;    int butspjsjME24850771 = -330668808;    int butspjsjME65502481 = -199943435;    int butspjsjME27003031 = -508183945;    int butspjsjME37427758 = -896574770;    int butspjsjME65187063 = -692382920;    int butspjsjME2242119 = -432411535;    int butspjsjME94601272 = 35606699;    int butspjsjME38564502 = -421034912;    int butspjsjME44344815 = -190078458;    int butspjsjME70219085 = -334401738;    int butspjsjME15562980 = -357896905;    int butspjsjME28969816 = -230486592;    int butspjsjME44107388 = -780468577;    int butspjsjME37523198 = 59624136;    int butspjsjME66919742 = -59063181;    int butspjsjME71253130 = -584575805;    int butspjsjME34537969 = -76643804;    int butspjsjME47872478 = -696964165;    int butspjsjME92463476 = -952609031;    int butspjsjME94278127 = -932039331;    int butspjsjME43563035 = -510743610;    int butspjsjME20967485 = -81766916;    int butspjsjME75472125 = -379519744;    int butspjsjME17939160 = -103681340;    int butspjsjME18905764 = -603157689;    int butspjsjME87600720 = -21326513;    int butspjsjME8374273 = -31860335;    int butspjsjME13272537 = -602481041;    int butspjsjME57280383 = -68232922;    int butspjsjME71824916 = -766541873;    int butspjsjME30699509 = -709095210;    int butspjsjME7684590 = -531697321;    int butspjsjME93104870 = -953837017;    int butspjsjME21285714 = -250326132;    int butspjsjME29961485 = -325213390;    int butspjsjME60436762 = -787543619;    int butspjsjME95673342 = -874693951;    int butspjsjME82523633 = -32165333;    int butspjsjME44660367 = -716527610;    int butspjsjME812612 = -817260544;    int butspjsjME27191699 = -568852029;    int butspjsjME49200632 = -881559195;    int butspjsjME75457206 = -133640226;    int butspjsjME55397533 = -935040107;    int butspjsjME28832690 = -255242915;    int butspjsjME96713235 = -995042941;    int butspjsjME95554088 = -266827248;    int butspjsjME52272263 = -255606113;    int butspjsjME15885274 = -103122553;    int butspjsjME3544977 = -239031769;    int butspjsjME3613287 = -318863608;    int butspjsjME97358175 = -555954190;    int butspjsjME44284054 = 84919409;    int butspjsjME40715660 = -893303143;    int butspjsjME54871336 = -158759091;    int butspjsjME3883287 = -148901892;    int butspjsjME90030356 = -820423691;    int butspjsjME9063872 = -304502605;    int butspjsjME18521995 = -193417082;    int butspjsjME77586342 = -571056408;    int butspjsjME93867845 = -300551201;    int butspjsjME81328736 = -361912260;    int butspjsjME81284118 = -252801990;    int butspjsjME72519899 = -423536586;    int butspjsjME39519577 = -625306529;    int butspjsjME7878390 = -826199585;    int butspjsjME35864946 = -276649576;    int butspjsjME22821674 = -430142445;    int butspjsjME7561713 = -615162475;    int butspjsjME6482980 = -271519562;    int butspjsjME75579787 = -709881855;    int butspjsjME52014335 = 55521529;    int butspjsjME3212112 = -980436555;    int butspjsjME91650864 = -35348488;    int butspjsjME67086429 = -263187303;    int butspjsjME94362403 = -629184416;    int butspjsjME45510278 = -948126691;    int butspjsjME20074592 = -444479638;    int butspjsjME89106469 = -848438426;    int butspjsjME22192528 = -608114748;    int butspjsjME92046631 = -754499265;    int butspjsjME56102010 = -776254222;    int butspjsjME97387263 = -399358489;    int butspjsjME53735406 = -829201153;    int butspjsjME68211629 = -347678265;    int butspjsjME33341334 = -53141020;    int butspjsjME63400536 = -516616730;    int butspjsjME52389211 = 39466125;    int butspjsjME66414378 = 8432958;    int butspjsjME26078198 = -76311499;    int butspjsjME70406406 = -967119929;    int butspjsjME86609470 = -470191346;    int butspjsjME64001638 = -838748252;    int butspjsjME67074024 = -923607573;     butspjsjME74798107 = butspjsjME38151256;     butspjsjME38151256 = butspjsjME45230654;     butspjsjME45230654 = butspjsjME36747530;     butspjsjME36747530 = butspjsjME34993787;     butspjsjME34993787 = butspjsjME98434371;     butspjsjME98434371 = butspjsjME24850771;     butspjsjME24850771 = butspjsjME65502481;     butspjsjME65502481 = butspjsjME27003031;     butspjsjME27003031 = butspjsjME37427758;     butspjsjME37427758 = butspjsjME65187063;     butspjsjME65187063 = butspjsjME2242119;     butspjsjME2242119 = butspjsjME94601272;     butspjsjME94601272 = butspjsjME38564502;     butspjsjME38564502 = butspjsjME44344815;     butspjsjME44344815 = butspjsjME70219085;     butspjsjME70219085 = butspjsjME15562980;     butspjsjME15562980 = butspjsjME28969816;     butspjsjME28969816 = butspjsjME44107388;     butspjsjME44107388 = butspjsjME37523198;     butspjsjME37523198 = butspjsjME66919742;     butspjsjME66919742 = butspjsjME71253130;     butspjsjME71253130 = butspjsjME34537969;     butspjsjME34537969 = butspjsjME47872478;     butspjsjME47872478 = butspjsjME92463476;     butspjsjME92463476 = butspjsjME94278127;     butspjsjME94278127 = butspjsjME43563035;     butspjsjME43563035 = butspjsjME20967485;     butspjsjME20967485 = butspjsjME75472125;     butspjsjME75472125 = butspjsjME17939160;     butspjsjME17939160 = butspjsjME18905764;     butspjsjME18905764 = butspjsjME87600720;     butspjsjME87600720 = butspjsjME8374273;     butspjsjME8374273 = butspjsjME13272537;     butspjsjME13272537 = butspjsjME57280383;     butspjsjME57280383 = butspjsjME71824916;     butspjsjME71824916 = butspjsjME30699509;     butspjsjME30699509 = butspjsjME7684590;     butspjsjME7684590 = butspjsjME93104870;     butspjsjME93104870 = butspjsjME21285714;     butspjsjME21285714 = butspjsjME29961485;     butspjsjME29961485 = butspjsjME60436762;     butspjsjME60436762 = butspjsjME95673342;     butspjsjME95673342 = butspjsjME82523633;     butspjsjME82523633 = butspjsjME44660367;     butspjsjME44660367 = butspjsjME812612;     butspjsjME812612 = butspjsjME27191699;     butspjsjME27191699 = butspjsjME49200632;     butspjsjME49200632 = butspjsjME75457206;     butspjsjME75457206 = butspjsjME55397533;     butspjsjME55397533 = butspjsjME28832690;     butspjsjME28832690 = butspjsjME96713235;     butspjsjME96713235 = butspjsjME95554088;     butspjsjME95554088 = butspjsjME52272263;     butspjsjME52272263 = butspjsjME15885274;     butspjsjME15885274 = butspjsjME3544977;     butspjsjME3544977 = butspjsjME3613287;     butspjsjME3613287 = butspjsjME97358175;     butspjsjME97358175 = butspjsjME44284054;     butspjsjME44284054 = butspjsjME40715660;     butspjsjME40715660 = butspjsjME54871336;     butspjsjME54871336 = butspjsjME3883287;     butspjsjME3883287 = butspjsjME90030356;     butspjsjME90030356 = butspjsjME9063872;     butspjsjME9063872 = butspjsjME18521995;     butspjsjME18521995 = butspjsjME77586342;     butspjsjME77586342 = butspjsjME93867845;     butspjsjME93867845 = butspjsjME81328736;     butspjsjME81328736 = butspjsjME81284118;     butspjsjME81284118 = butspjsjME72519899;     butspjsjME72519899 = butspjsjME39519577;     butspjsjME39519577 = butspjsjME7878390;     butspjsjME7878390 = butspjsjME35864946;     butspjsjME35864946 = butspjsjME22821674;     butspjsjME22821674 = butspjsjME7561713;     butspjsjME7561713 = butspjsjME6482980;     butspjsjME6482980 = butspjsjME75579787;     butspjsjME75579787 = butspjsjME52014335;     butspjsjME52014335 = butspjsjME3212112;     butspjsjME3212112 = butspjsjME91650864;     butspjsjME91650864 = butspjsjME67086429;     butspjsjME67086429 = butspjsjME94362403;     butspjsjME94362403 = butspjsjME45510278;     butspjsjME45510278 = butspjsjME20074592;     butspjsjME20074592 = butspjsjME89106469;     butspjsjME89106469 = butspjsjME22192528;     butspjsjME22192528 = butspjsjME92046631;     butspjsjME92046631 = butspjsjME56102010;     butspjsjME56102010 = butspjsjME97387263;     butspjsjME97387263 = butspjsjME53735406;     butspjsjME53735406 = butspjsjME68211629;     butspjsjME68211629 = butspjsjME33341334;     butspjsjME33341334 = butspjsjME63400536;     butspjsjME63400536 = butspjsjME52389211;     butspjsjME52389211 = butspjsjME66414378;     butspjsjME66414378 = butspjsjME26078198;     butspjsjME26078198 = butspjsjME70406406;     butspjsjME70406406 = butspjsjME86609470;     butspjsjME86609470 = butspjsjME64001638;     butspjsjME64001638 = butspjsjME67074024;     butspjsjME67074024 = butspjsjME74798107;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void dgJjeVyigz58817136() {     int yBPTOxgVvN61719784 = 35741222;    int yBPTOxgVvN47798994 = -606093985;    int yBPTOxgVvN37926668 = -784033737;    int yBPTOxgVvN64610724 = -973486912;    int yBPTOxgVvN85722874 = -679155951;    int yBPTOxgVvN11900049 = -67195995;    int yBPTOxgVvN74896439 = -12466846;    int yBPTOxgVvN48218830 = -730483256;    int yBPTOxgVvN5388722 = -717694647;    int yBPTOxgVvN10928145 = -611435976;    int yBPTOxgVvN84867793 = -846471664;    int yBPTOxgVvN71868596 = -554514420;    int yBPTOxgVvN55227722 = -52756590;    int yBPTOxgVvN81882611 = -217533311;    int yBPTOxgVvN68057685 = -376844008;    int yBPTOxgVvN5498245 = -313053324;    int yBPTOxgVvN65403155 = 72183258;    int yBPTOxgVvN58186574 = 94127164;    int yBPTOxgVvN5266959 = -116989547;    int yBPTOxgVvN94276025 = -378473703;    int yBPTOxgVvN34385411 = -103742174;    int yBPTOxgVvN9943090 = -412697707;    int yBPTOxgVvN4184309 = 25949946;    int yBPTOxgVvN24229429 = -631052941;    int yBPTOxgVvN48428461 = -976876811;    int yBPTOxgVvN88457027 = -495562677;    int yBPTOxgVvN453464 = -762845675;    int yBPTOxgVvN21554035 = -309416184;    int yBPTOxgVvN48374918 = -795752376;    int yBPTOxgVvN97652797 = -764901410;    int yBPTOxgVvN78026015 = -519586588;    int yBPTOxgVvN92042750 = -192248214;    int yBPTOxgVvN62647356 = -615552068;    int yBPTOxgVvN43195635 = -738595810;    int yBPTOxgVvN50059332 = -440203018;    int yBPTOxgVvN4785131 = -947137690;    int yBPTOxgVvN86668772 = -677000499;    int yBPTOxgVvN80012967 = -411929891;    int yBPTOxgVvN46336037 = -216688189;    int yBPTOxgVvN49072789 = -530719302;    int yBPTOxgVvN13122916 = -47136040;    int yBPTOxgVvN45277688 = -44699101;    int yBPTOxgVvN72123516 = -668174003;    int yBPTOxgVvN1068313 = -82161016;    int yBPTOxgVvN96645227 = -72419328;    int yBPTOxgVvN3379534 = -772610678;    int yBPTOxgVvN47841725 = -44556543;    int yBPTOxgVvN51456576 = 19671052;    int yBPTOxgVvN65682834 = 16616095;    int yBPTOxgVvN47997113 = -173058436;    int yBPTOxgVvN70190552 = -314597642;    int yBPTOxgVvN1626279 = -494355624;    int yBPTOxgVvN80695692 = -74746933;    int yBPTOxgVvN6817233 = -871698273;    int yBPTOxgVvN24431725 = -754235800;    int yBPTOxgVvN51776695 = -551561071;    int yBPTOxgVvN43614685 = -532043932;    int yBPTOxgVvN13697239 = -52980797;    int yBPTOxgVvN16182263 = -996610102;    int yBPTOxgVvN97265847 = -83593275;    int yBPTOxgVvN11446586 = -304350321;    int yBPTOxgVvN53342405 = -703050663;    int yBPTOxgVvN99843911 = -934730880;    int yBPTOxgVvN7735924 = -952793237;    int yBPTOxgVvN32902129 = 8150611;    int yBPTOxgVvN92825042 = -554223450;    int yBPTOxgVvN9221241 = -938962352;    int yBPTOxgVvN12032087 = -314160780;    int yBPTOxgVvN31823279 = -777330293;    int yBPTOxgVvN63272554 = -429706319;    int yBPTOxgVvN18829472 = -636052825;    int yBPTOxgVvN85390188 = -515886852;    int yBPTOxgVvN11850537 = -689184648;    int yBPTOxgVvN56194170 = -586270246;    int yBPTOxgVvN81153109 = -231337663;    int yBPTOxgVvN89107723 = 40956927;    int yBPTOxgVvN37819574 = -744523705;    int yBPTOxgVvN3115997 = -891889039;    int yBPTOxgVvN27584202 = -458633613;    int yBPTOxgVvN45048928 = -104266133;    int yBPTOxgVvN40615302 = -351006134;    int yBPTOxgVvN48996888 = -682516727;    int yBPTOxgVvN55871200 = -226032279;    int yBPTOxgVvN377806 = -522693940;    int yBPTOxgVvN27462246 = -350303769;    int yBPTOxgVvN76399737 = 74769036;    int yBPTOxgVvN11347059 = -17501282;    int yBPTOxgVvN55830123 = -743853796;    int yBPTOxgVvN18763910 = -984360011;    int yBPTOxgVvN98282637 = -888641948;    int yBPTOxgVvN61170446 = -315093758;    int yBPTOxgVvN72971533 = -524019703;    int yBPTOxgVvN63830704 = -415319789;    int yBPTOxgVvN49070190 = -33094914;    int yBPTOxgVvN37626204 = -126368982;    int yBPTOxgVvN59780511 = -344085377;    int yBPTOxgVvN45433776 = -109968222;    int yBPTOxgVvN64387593 = -715380766;    int yBPTOxgVvN68166184 = 9688372;    int yBPTOxgVvN3820185 = 35741222;     yBPTOxgVvN61719784 = yBPTOxgVvN47798994;     yBPTOxgVvN47798994 = yBPTOxgVvN37926668;     yBPTOxgVvN37926668 = yBPTOxgVvN64610724;     yBPTOxgVvN64610724 = yBPTOxgVvN85722874;     yBPTOxgVvN85722874 = yBPTOxgVvN11900049;     yBPTOxgVvN11900049 = yBPTOxgVvN74896439;     yBPTOxgVvN74896439 = yBPTOxgVvN48218830;     yBPTOxgVvN48218830 = yBPTOxgVvN5388722;     yBPTOxgVvN5388722 = yBPTOxgVvN10928145;     yBPTOxgVvN10928145 = yBPTOxgVvN84867793;     yBPTOxgVvN84867793 = yBPTOxgVvN71868596;     yBPTOxgVvN71868596 = yBPTOxgVvN55227722;     yBPTOxgVvN55227722 = yBPTOxgVvN81882611;     yBPTOxgVvN81882611 = yBPTOxgVvN68057685;     yBPTOxgVvN68057685 = yBPTOxgVvN5498245;     yBPTOxgVvN5498245 = yBPTOxgVvN65403155;     yBPTOxgVvN65403155 = yBPTOxgVvN58186574;     yBPTOxgVvN58186574 = yBPTOxgVvN5266959;     yBPTOxgVvN5266959 = yBPTOxgVvN94276025;     yBPTOxgVvN94276025 = yBPTOxgVvN34385411;     yBPTOxgVvN34385411 = yBPTOxgVvN9943090;     yBPTOxgVvN9943090 = yBPTOxgVvN4184309;     yBPTOxgVvN4184309 = yBPTOxgVvN24229429;     yBPTOxgVvN24229429 = yBPTOxgVvN48428461;     yBPTOxgVvN48428461 = yBPTOxgVvN88457027;     yBPTOxgVvN88457027 = yBPTOxgVvN453464;     yBPTOxgVvN453464 = yBPTOxgVvN21554035;     yBPTOxgVvN21554035 = yBPTOxgVvN48374918;     yBPTOxgVvN48374918 = yBPTOxgVvN97652797;     yBPTOxgVvN97652797 = yBPTOxgVvN78026015;     yBPTOxgVvN78026015 = yBPTOxgVvN92042750;     yBPTOxgVvN92042750 = yBPTOxgVvN62647356;     yBPTOxgVvN62647356 = yBPTOxgVvN43195635;     yBPTOxgVvN43195635 = yBPTOxgVvN50059332;     yBPTOxgVvN50059332 = yBPTOxgVvN4785131;     yBPTOxgVvN4785131 = yBPTOxgVvN86668772;     yBPTOxgVvN86668772 = yBPTOxgVvN80012967;     yBPTOxgVvN80012967 = yBPTOxgVvN46336037;     yBPTOxgVvN46336037 = yBPTOxgVvN49072789;     yBPTOxgVvN49072789 = yBPTOxgVvN13122916;     yBPTOxgVvN13122916 = yBPTOxgVvN45277688;     yBPTOxgVvN45277688 = yBPTOxgVvN72123516;     yBPTOxgVvN72123516 = yBPTOxgVvN1068313;     yBPTOxgVvN1068313 = yBPTOxgVvN96645227;     yBPTOxgVvN96645227 = yBPTOxgVvN3379534;     yBPTOxgVvN3379534 = yBPTOxgVvN47841725;     yBPTOxgVvN47841725 = yBPTOxgVvN51456576;     yBPTOxgVvN51456576 = yBPTOxgVvN65682834;     yBPTOxgVvN65682834 = yBPTOxgVvN47997113;     yBPTOxgVvN47997113 = yBPTOxgVvN70190552;     yBPTOxgVvN70190552 = yBPTOxgVvN1626279;     yBPTOxgVvN1626279 = yBPTOxgVvN80695692;     yBPTOxgVvN80695692 = yBPTOxgVvN6817233;     yBPTOxgVvN6817233 = yBPTOxgVvN24431725;     yBPTOxgVvN24431725 = yBPTOxgVvN51776695;     yBPTOxgVvN51776695 = yBPTOxgVvN43614685;     yBPTOxgVvN43614685 = yBPTOxgVvN13697239;     yBPTOxgVvN13697239 = yBPTOxgVvN16182263;     yBPTOxgVvN16182263 = yBPTOxgVvN97265847;     yBPTOxgVvN97265847 = yBPTOxgVvN11446586;     yBPTOxgVvN11446586 = yBPTOxgVvN53342405;     yBPTOxgVvN53342405 = yBPTOxgVvN99843911;     yBPTOxgVvN99843911 = yBPTOxgVvN7735924;     yBPTOxgVvN7735924 = yBPTOxgVvN32902129;     yBPTOxgVvN32902129 = yBPTOxgVvN92825042;     yBPTOxgVvN92825042 = yBPTOxgVvN9221241;     yBPTOxgVvN9221241 = yBPTOxgVvN12032087;     yBPTOxgVvN12032087 = yBPTOxgVvN31823279;     yBPTOxgVvN31823279 = yBPTOxgVvN63272554;     yBPTOxgVvN63272554 = yBPTOxgVvN18829472;     yBPTOxgVvN18829472 = yBPTOxgVvN85390188;     yBPTOxgVvN85390188 = yBPTOxgVvN11850537;     yBPTOxgVvN11850537 = yBPTOxgVvN56194170;     yBPTOxgVvN56194170 = yBPTOxgVvN81153109;     yBPTOxgVvN81153109 = yBPTOxgVvN89107723;     yBPTOxgVvN89107723 = yBPTOxgVvN37819574;     yBPTOxgVvN37819574 = yBPTOxgVvN3115997;     yBPTOxgVvN3115997 = yBPTOxgVvN27584202;     yBPTOxgVvN27584202 = yBPTOxgVvN45048928;     yBPTOxgVvN45048928 = yBPTOxgVvN40615302;     yBPTOxgVvN40615302 = yBPTOxgVvN48996888;     yBPTOxgVvN48996888 = yBPTOxgVvN55871200;     yBPTOxgVvN55871200 = yBPTOxgVvN377806;     yBPTOxgVvN377806 = yBPTOxgVvN27462246;     yBPTOxgVvN27462246 = yBPTOxgVvN76399737;     yBPTOxgVvN76399737 = yBPTOxgVvN11347059;     yBPTOxgVvN11347059 = yBPTOxgVvN55830123;     yBPTOxgVvN55830123 = yBPTOxgVvN18763910;     yBPTOxgVvN18763910 = yBPTOxgVvN98282637;     yBPTOxgVvN98282637 = yBPTOxgVvN61170446;     yBPTOxgVvN61170446 = yBPTOxgVvN72971533;     yBPTOxgVvN72971533 = yBPTOxgVvN63830704;     yBPTOxgVvN63830704 = yBPTOxgVvN49070190;     yBPTOxgVvN49070190 = yBPTOxgVvN37626204;     yBPTOxgVvN37626204 = yBPTOxgVvN59780511;     yBPTOxgVvN59780511 = yBPTOxgVvN45433776;     yBPTOxgVvN45433776 = yBPTOxgVvN64387593;     yBPTOxgVvN64387593 = yBPTOxgVvN68166184;     yBPTOxgVvN68166184 = yBPTOxgVvN3820185;     yBPTOxgVvN3820185 = yBPTOxgVvN61719784;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void IfNmMdmpqE34829256() {     int UMPNznkZhW48641462 = -104909982;    int UMPNznkZhW57446732 = -716680558;    int UMPNznkZhW30622682 = -215149120;    int UMPNznkZhW92473918 = -979284203;    int UMPNznkZhW36451961 = -532969428;    int UMPNznkZhW25365726 = -464889290;    int UMPNznkZhW24942108 = -794264885;    int UMPNznkZhW30935178 = -161023077;    int UMPNznkZhW83774411 = -927205349;    int UMPNznkZhW84428530 = -326297182;    int UMPNznkZhW4548524 = 99439592;    int UMPNznkZhW41495074 = -676617304;    int UMPNznkZhW15854171 = -141119879;    int UMPNznkZhW25200720 = -14031710;    int UMPNznkZhW91770554 = -563609558;    int UMPNznkZhW40777403 = -291704910;    int UMPNznkZhW15243331 = -597736579;    int UMPNznkZhW87403331 = -681259080;    int UMPNznkZhW66426529 = -553510517;    int UMPNznkZhW51028853 = -816571542;    int UMPNznkZhW1851080 = -148421167;    int UMPNznkZhW48633049 = -240819610;    int UMPNznkZhW73830648 = -971456304;    int UMPNznkZhW586379 = -565141716;    int UMPNznkZhW4393447 = 98855410;    int UMPNznkZhW82635927 = -59086022;    int UMPNznkZhW57343891 = 85052260;    int UMPNznkZhW22140585 = -537065451;    int UMPNznkZhW21277712 = -111985007;    int UMPNznkZhW77366436 = -326121480;    int UMPNznkZhW37146268 = -436015487;    int UMPNznkZhW96484780 = -363169916;    int UMPNznkZhW16920439 = -99243802;    int UMPNznkZhW73118733 = -874710580;    int UMPNznkZhW42838281 = -812173115;    int UMPNznkZhW37745346 = -27733506;    int UMPNznkZhW42638036 = -644905789;    int UMPNznkZhW52341344 = -292162460;    int UMPNznkZhW99567203 = -579539360;    int UMPNznkZhW76859864 = -811112471;    int UMPNznkZhW96284346 = -869058690;    int UMPNznkZhW30118613 = -401854584;    int UMPNznkZhW48573690 = -461654054;    int UMPNznkZhW19612992 = -132156698;    int UMPNznkZhW48630087 = -528311046;    int UMPNznkZhW5946456 = -727960812;    int UMPNznkZhW68491751 = -620261058;    int UMPNznkZhW53712519 = -179098702;    int UMPNznkZhW55908463 = -933127584;    int UMPNznkZhW40596692 = -511076766;    int UMPNznkZhW11548415 = -373952368;    int UMPNznkZhW6539321 = 6331692;    int UMPNznkZhW65837295 = -982666617;    int UMPNznkZhW61362202 = -387790433;    int UMPNznkZhW32978177 = -305349047;    int UMPNznkZhW8414 = -864090373;    int UMPNznkZhW83616083 = -745224255;    int UMPNznkZhW30036303 = -650007404;    int UMPNznkZhW88080472 = -978139613;    int UMPNznkZhW53816034 = -373883407;    int UMPNznkZhW68021835 = -449941550;    int UMPNznkZhW2801524 = -157199435;    int UMPNznkZhW9657467 = 50961930;    int UMPNznkZhW6407975 = -501083869;    int UMPNznkZhW47282262 = -890281695;    int UMPNznkZhW8063744 = -537390492;    int UMPNznkZhW24574636 = -477373503;    int UMPNznkZhW42735438 = -266409300;    int UMPNznkZhW82362439 = -201858596;    int UMPNznkZhW54025209 = -435876052;    int UMPNznkZhW98139367 = -646799122;    int UMPNznkZhW62901986 = -205574120;    int UMPNznkZhW87836128 = -1719721;    int UMPNznkZhW89566665 = -742398047;    int UMPNznkZhW54744507 = -947512852;    int UMPNznkZhW71732466 = -746566584;    int UMPNznkZhW59360 = -779165556;    int UMPNznkZhW54217657 = -739299606;    int UMPNznkZhW51956292 = 63169330;    int UMPNznkZhW98446991 = -173183778;    int UMPNznkZhW14144176 = -438824965;    int UMPNznkZhW3631372 = -735849039;    int UMPNznkZhW66232121 = -603937867;    int UMPNznkZhW80681019 = -600908242;    int UMPNznkZhW65818022 = -952169112;    int UMPNznkZhW30606948 = -342347180;    int UMPNznkZhW30647486 = -380503300;    int UMPNznkZhW55558236 = -711453369;    int UMPNznkZhW40140557 = -469361533;    int UMPNznkZhW42829868 = -948082742;    int UMPNznkZhW54129262 = -282509252;    int UMPNznkZhW12601733 = -994898385;    int UMPNznkZhW64260872 = -314022848;    int UMPNznkZhW45751169 = -105655954;    int UMPNznkZhW8838030 = -261170922;    int UMPNznkZhW93482823 = -611859256;    int UMPNznkZhW20461147 = -352816514;    int UMPNznkZhW42165715 = -960570186;    int UMPNznkZhW72330729 = -241875003;    int UMPNznkZhW40566344 = -104909982;     UMPNznkZhW48641462 = UMPNznkZhW57446732;     UMPNznkZhW57446732 = UMPNznkZhW30622682;     UMPNznkZhW30622682 = UMPNznkZhW92473918;     UMPNznkZhW92473918 = UMPNznkZhW36451961;     UMPNznkZhW36451961 = UMPNznkZhW25365726;     UMPNznkZhW25365726 = UMPNznkZhW24942108;     UMPNznkZhW24942108 = UMPNznkZhW30935178;     UMPNznkZhW30935178 = UMPNznkZhW83774411;     UMPNznkZhW83774411 = UMPNznkZhW84428530;     UMPNznkZhW84428530 = UMPNznkZhW4548524;     UMPNznkZhW4548524 = UMPNznkZhW41495074;     UMPNznkZhW41495074 = UMPNznkZhW15854171;     UMPNznkZhW15854171 = UMPNznkZhW25200720;     UMPNznkZhW25200720 = UMPNznkZhW91770554;     UMPNznkZhW91770554 = UMPNznkZhW40777403;     UMPNznkZhW40777403 = UMPNznkZhW15243331;     UMPNznkZhW15243331 = UMPNznkZhW87403331;     UMPNznkZhW87403331 = UMPNznkZhW66426529;     UMPNznkZhW66426529 = UMPNznkZhW51028853;     UMPNznkZhW51028853 = UMPNznkZhW1851080;     UMPNznkZhW1851080 = UMPNznkZhW48633049;     UMPNznkZhW48633049 = UMPNznkZhW73830648;     UMPNznkZhW73830648 = UMPNznkZhW586379;     UMPNznkZhW586379 = UMPNznkZhW4393447;     UMPNznkZhW4393447 = UMPNznkZhW82635927;     UMPNznkZhW82635927 = UMPNznkZhW57343891;     UMPNznkZhW57343891 = UMPNznkZhW22140585;     UMPNznkZhW22140585 = UMPNznkZhW21277712;     UMPNznkZhW21277712 = UMPNznkZhW77366436;     UMPNznkZhW77366436 = UMPNznkZhW37146268;     UMPNznkZhW37146268 = UMPNznkZhW96484780;     UMPNznkZhW96484780 = UMPNznkZhW16920439;     UMPNznkZhW16920439 = UMPNznkZhW73118733;     UMPNznkZhW73118733 = UMPNznkZhW42838281;     UMPNznkZhW42838281 = UMPNznkZhW37745346;     UMPNznkZhW37745346 = UMPNznkZhW42638036;     UMPNznkZhW42638036 = UMPNznkZhW52341344;     UMPNznkZhW52341344 = UMPNznkZhW99567203;     UMPNznkZhW99567203 = UMPNznkZhW76859864;     UMPNznkZhW76859864 = UMPNznkZhW96284346;     UMPNznkZhW96284346 = UMPNznkZhW30118613;     UMPNznkZhW30118613 = UMPNznkZhW48573690;     UMPNznkZhW48573690 = UMPNznkZhW19612992;     UMPNznkZhW19612992 = UMPNznkZhW48630087;     UMPNznkZhW48630087 = UMPNznkZhW5946456;     UMPNznkZhW5946456 = UMPNznkZhW68491751;     UMPNznkZhW68491751 = UMPNznkZhW53712519;     UMPNznkZhW53712519 = UMPNznkZhW55908463;     UMPNznkZhW55908463 = UMPNznkZhW40596692;     UMPNznkZhW40596692 = UMPNznkZhW11548415;     UMPNznkZhW11548415 = UMPNznkZhW6539321;     UMPNznkZhW6539321 = UMPNznkZhW65837295;     UMPNznkZhW65837295 = UMPNznkZhW61362202;     UMPNznkZhW61362202 = UMPNznkZhW32978177;     UMPNznkZhW32978177 = UMPNznkZhW8414;     UMPNznkZhW8414 = UMPNznkZhW83616083;     UMPNznkZhW83616083 = UMPNznkZhW30036303;     UMPNznkZhW30036303 = UMPNznkZhW88080472;     UMPNznkZhW88080472 = UMPNznkZhW53816034;     UMPNznkZhW53816034 = UMPNznkZhW68021835;     UMPNznkZhW68021835 = UMPNznkZhW2801524;     UMPNznkZhW2801524 = UMPNznkZhW9657467;     UMPNznkZhW9657467 = UMPNznkZhW6407975;     UMPNznkZhW6407975 = UMPNznkZhW47282262;     UMPNznkZhW47282262 = UMPNznkZhW8063744;     UMPNznkZhW8063744 = UMPNznkZhW24574636;     UMPNznkZhW24574636 = UMPNznkZhW42735438;     UMPNznkZhW42735438 = UMPNznkZhW82362439;     UMPNznkZhW82362439 = UMPNznkZhW54025209;     UMPNznkZhW54025209 = UMPNznkZhW98139367;     UMPNznkZhW98139367 = UMPNznkZhW62901986;     UMPNznkZhW62901986 = UMPNznkZhW87836128;     UMPNznkZhW87836128 = UMPNznkZhW89566665;     UMPNznkZhW89566665 = UMPNznkZhW54744507;     UMPNznkZhW54744507 = UMPNznkZhW71732466;     UMPNznkZhW71732466 = UMPNznkZhW59360;     UMPNznkZhW59360 = UMPNznkZhW54217657;     UMPNznkZhW54217657 = UMPNznkZhW51956292;     UMPNznkZhW51956292 = UMPNznkZhW98446991;     UMPNznkZhW98446991 = UMPNznkZhW14144176;     UMPNznkZhW14144176 = UMPNznkZhW3631372;     UMPNznkZhW3631372 = UMPNznkZhW66232121;     UMPNznkZhW66232121 = UMPNznkZhW80681019;     UMPNznkZhW80681019 = UMPNznkZhW65818022;     UMPNznkZhW65818022 = UMPNznkZhW30606948;     UMPNznkZhW30606948 = UMPNznkZhW30647486;     UMPNznkZhW30647486 = UMPNznkZhW55558236;     UMPNznkZhW55558236 = UMPNznkZhW40140557;     UMPNznkZhW40140557 = UMPNznkZhW42829868;     UMPNznkZhW42829868 = UMPNznkZhW54129262;     UMPNznkZhW54129262 = UMPNznkZhW12601733;     UMPNznkZhW12601733 = UMPNznkZhW64260872;     UMPNznkZhW64260872 = UMPNznkZhW45751169;     UMPNznkZhW45751169 = UMPNznkZhW8838030;     UMPNznkZhW8838030 = UMPNznkZhW93482823;     UMPNznkZhW93482823 = UMPNznkZhW20461147;     UMPNznkZhW20461147 = UMPNznkZhW42165715;     UMPNznkZhW42165715 = UMPNznkZhW72330729;     UMPNznkZhW72330729 = UMPNznkZhW40566344;     UMPNznkZhW40566344 = UMPNznkZhW48641462;}
// Junk Finished
