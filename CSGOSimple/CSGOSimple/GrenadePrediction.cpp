GrenadePrediction::GrenadePrediction() :
	tickcount(0),
	curtime(0.f),
	m_nNextThinkTick(0),
	m_nWaterType(0),
	m_CollisionGroup(COLLISION_GROUP_PROJECTILE),
	m_flDetonateTime(0),
	m_nBounces(0),
	m_CollisionEntity(nullptr),
	m_Detonated(false),
	m_WeaponID(0),
	m_LastUpdateTick(0)
{ }

void GrenadePrediction::Init()
{
	g_CheatManager.GetPostCreateMoveEvent() += [this](CreateMoveEventArgs& e)
	{
		m_CurrentPath.Old = true;

		bool IsObserver;
		auto Player = C_BasePlayer::GetLocalPlayerOrObserverPlayer(&IsObserver);
		if (!Player)
			return;

		auto Weapon = Player->GetActiveWeapon();
		if (!Weapon)
			return;

		auto Origin = Player->EyePosition();
		//if ( enginetrace->GetPointContents( Origin ) & CONTENTS_SOLID )	// Let's not bother if we're noclipping inside something
			//return;														// There's a little bit more to it... Doing this inside ThrowGrenade won't help much either

		if (Weapon->IsGrenade())
		{
			auto Grenade = reinterpret_cast<C_BaseCSGrenade*>(Weapon);
			if (Grenade->m_bPinPulled() || Grenade->m_fThrowTime())
			{
				m_WeaponID = Weapon->m_AttributeManager().m_Item().m_iItemDefinitionIndex();
				Predict(Grenade, Origin, IsObserver ? Player->m_angEyeAngles() : e.GetCommand()->viewangles);
			}
		}

		if (m_CurrentPath.Old)
		{
			if (m_OldPaths.empty() && !m_CurrentPath.Points.empty() || !m_OldPaths.empty() && m_OldPaths.back().EraseTime != m_CurrentPath.EraseTime)
			{
				m_OldPaths.emplace_back(m_CurrentPath);
				m_CurrentPath.Points.clear();
			}
		}
	};

	g_CheatManager.GetNewMapEvent() += bind(&GrenadePrediction::OnNewMap, this, std::placeholders::_1);
}

void GrenadePrediction::PopulateGeometry()
{
	using namespace Drawing;

	Graphics G(*m_Geometry);

	static auto DrawPath = [](const Graphics& G, Path& Path)
	{
		if (Path.Points.empty())
			return;

		auto Curtime = Prediction::Curtime();
		if (Curtime - Path.EraseTime > 0.f) // I feel like this is pretty gay
		{
			const auto EraseTime = 1.f / 500.f;

			if (Curtime - Path.LastEraseTime > EraseTime)
			{
				Path.Points.pop_front(); // Maybe just save the iterator inside Path and use a vector?

				Path.LastEraseTime = Curtime + EraseTime;
			}
		}

		if (Path.Points.empty())
			return;

		G.SetAntialias(true);

		G.DrawSphere(Path.Points.back().Location, 100.f, Color::Green(100)); // TO DO: Endpoint visualization based on m_WeaponID

		auto LastOnScreen = false;
		Point LastLocation;
		for (const auto& Point : Path.Points)
		{
			Drawing::Point Location;
			auto OnScreen = WorldToScreen(Point.Location, Location);

			if (LastOnScreen && OnScreen)
				G.DrawLine(Color::Red(), LastLocation, Location);

			if (OnScreen && Point.Bounced)
				G.Draw3DBox(Point.Location, Math::QAngle::Clear(), Math::Vector(-2.f, -2.f, -2.f), Math::Vector(2.f, 2.f, 2.f), Color::Blue(100)); // TO DO: Maybe draw these last? I think a line above looks weird

			LastOnScreen = OnScreen;
			LastLocation = Location;
		}

		G.SetAntialias(false);
	};

	for (auto it = m_OldPaths.begin(); it != m_OldPaths.end(); )
	{
		DrawPath(G, *it);

		if (it->Points.empty())
			it = m_OldPaths.erase(it);
		else
			++it;
	}

	DrawPath(G, m_CurrentPath);
}

void GrenadePrediction::OnNewMap(const char* Map) const // TO DO: Maybe handle more events
{
	struct EventData
	{
		EventData(const std::string& Targetname, const std::vector<std::string>& OnBreak) :
			Targetname(Targetname),
			OnBreak(OnBreak)
		{ }

		std::string Targetname;
		std::vector<std::string> OnBreak;
	};
	std::vector<EventData> Events;
	Breakables.clear();

	static auto FindEntitiesByTargetname = [](const std::vector<EventData>& Events, const std::string& Targetname) // See https://github.com/LestaD/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/server/entitylist.cpp#L620 as well
	{
		std::vector<Breakable*> Entities;

		for (size_t i = 0; i < Events.size(); ++i)
		{
			auto it1 = Events[i].Targetname.begin();
			auto it2 = Targetname.begin();

			while (it1 != Events[i].Targetname.end() && it2 != Targetname.end())
			{
				if (*it1 != *it2 && tolower(*it1) != tolower(*it2))
					break;

				++it1;
				++it2;
			}

			if (it2 == Targetname.end())
			{
				if (it1 == Events[i].Targetname.end())
					Entities.emplace_back(&Breakables[i]);
			}
			else if (*it2 == '*')
				Entities.emplace_back(&Breakables[i]);
		}

		return Entities;
	};

	auto Entities = GetMapEntities(Map); // It's up to you to parse the BSP file
	for (const auto& Entity : Entities)
	{
		auto Classname = Entity.GetValue(XorStringC("classname"));

		BreakableType Type;
		if (Classname == XorStringC("func_breakable"))
			Type = BreakableType::func_breakable;
		else if (Classname == XorStringC("prop_dynamic") || Classname == XorStringC("prop_dynamic_override"))
			Type = BreakableType::prop_dynamic;
		else if (Classname == XorStringC("func_breakable_surf"))
			Type = BreakableType::func_breakable_surf;
		/*else if ( Classname == XorStringC( "prop_physics_multiplayer" ) ) // This never seems to break like I want it to so let's just ignore it, works fine in all official maps 10/4/17
			Type = BreakableType::prop_physics_multiplayer;*/
		else
			continue;

		if (Type == BreakableType::prop_dynamic && atoi(Entity.GetValue(XorStringC("solid")).c_str()) == SOLID_NONE) // TO DO: Figure out how to filter out bs better to save some iterations inside IsBreakableEntity
			continue;

		Breakable Breakable;
		Breakable.Model = Entity.GetValue(XorStringC("model"));
		Breakable.Health = atoi(Entity.GetValue(XorStringC("health")).c_str());
		Breakable.Type = Type;
		Breakables.emplace_back(Breakable);

		Events.emplace_back(Entity.GetValue(XorStringC("targetname")), Entity.GetValues(XorStringC("OnBreak")));
	}

	for (size_t i = 0; i < Events.size(); ++i)
	{
		for (const auto& OnBreak : Events[i].OnBreak)
		{
			auto Event = ParseEvent(OnBreak); // Just split the string with '\x1B'
			if (Event.TargetInput != XorStringC("Break"))
				continue;

			auto Found = FindEntitiesByTargetname(Events, Event.Target); // TO DO: Maybe use the delay, keeping in mind that it is completely useless in the official maps
			if (Found.empty())
				continue; // Maybe error?

			Breakables[i].Breakables.insert(Breakables[i].Breakables.end(), Found.begin(), Found.end());
		}
	}
}

void GrenadePrediction::Predict(C_BaseCSGrenade* Grenade, const Math::Vector& Origin, const Math::QAngle& Angle) // Running this w/ low sv_gravity w/ smoke/decoy can kill your fps
{
	const auto SamplesPerSecond = 30.f;

	ThrowGrenade(Grenade, Origin, Angle);

	const auto SampleTick = static_cast<int>(1.f / SamplesPerSecond / gpGlobals->interval_per_tick);
	m_LastUpdateTick = -SampleTick;

	m_CurrentPath.Old = false;
	m_CurrentPath.Points.clear();
	for (tickcount = 0; ; ++tickcount)
	{
		if (curtime > 60.f) // This is here because of (see the comment above)
		{
			Warning(XorStringC("The grenade lives more than a minute. Too much time spent predicting!\n"));

			break;
		}

		if (m_LastUpdateTick + SampleTick <= tickcount)
			UpdatePath(false);

		PhysicsSimulate();

		if (m_Detonated)
			break;

		curtime += gpGlobals->interval_per_tick;
	}

	// TO DO: Check if we actually threw the nade and we didn't just switch weapons
	m_CurrentPath.LastEraseTime = 0.f;
	m_CurrentPath.EraseTime = Prediction::Curtime() + curtime + 2.f; // Start erasing the path 2 seconds after the nade popped
}

void GrenadePrediction::ThrowGrenade(C_BaseCSGrenade* Grenade, Math::Vector vecSrc, Math::QAngle angThrow)
{
	auto pPlayer = C_BasePlayer::GetLocalPlayerOrObserverPlayer();

	if (angThrow.x() < -90)
		angThrow.x() += 360.f;
	else if (angThrow.x() > 90.f)
		angThrow.x() -= 360.f;

	angThrow.x() -= (90.f - abs(angThrow.x())) * 10.f / 90.f;

	auto ThrowStrength = std::clamp(Grenade->m_flThrowStrength(), 0.f, 1.f);

	auto flVel = std::clamp(Grenade->GetWeaponData()->ThrowVelocity * 0.9f, 15.f, 750.f);
	flVel *= ThrowStrength * 0.7f + 0.3f;

	Math::Vector vForward;
	AngleVectors(angThrow, &vForward);

	vecSrc.z() += ThrowStrength * 12.f - 12.f;

	trace_t trace;
	Math::Vector mins(-2.f, -2.f, -2.f);
	Math::Vector maxs(2.f, 2.f, 2.f);
	UTIL_TraceHull(vecSrc, vecSrc + vForward * 22.f, mins, maxs, MASK_SOLID | CONTENTS_CSGO_CUSTOM, pPlayer, COLLISION_GROUP_NONE, &trace);
	vecSrc = trace.endpos - vForward * 6.f;

	auto vecThrow = vForward * flVel + pPlayer->GetAbsVelocity() * 1.25f;

	EmitGrenade(vecSrc, vecThrow);
}

void GrenadePrediction::EmitGrenade(const Math::Vector& vecSrc, const Math::Vector& vecVel)
{
	curtime = 0.f;
	m_vecAbsVelocity = vecVel;
	m_vecAbsOrigin = vecSrc;
	m_CollisionGroup = COLLISION_GROUP_PROJECTILE;
	m_nBounces = 0;
	m_CollisionEntity = nullptr; // This is a handle in game code. I don't know how Valve named it...
	m_Detonated = false;
	UpdateWaterState();
	ClearBrokenEntities();

	switch (m_WeaponID)
	{
	case WEAPON_SMOKEGRENADE:
		SetNextThink(1.5f);

		break;
	case WEAPON_DECOY:
		SetNextThink(2.f);

		break;
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
		m_flDetonateTime = 1.5f;

		SetNextThink(0.02f); // I really don't fucking know... I pulled this out of my ass because of some 3 tick diff

		break;
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
		m_flDetonateTime = molotov_throw_detonate_time->GetFloat();

		SetNextThink(0.02f); // I really don't fucking know... I pulled this out of my ass because of some 3 tick diff

		break;
	default:
		break;
	}
}

void GrenadePrediction::PhysicsSimulate() // PhysicsToss
{
	trace_t trace;
	Math::Vector move;

	PhysicsRunThink();

	if (m_Detonated)
		return;

	PhysicsAddGravityMove(move);
	PhysicsPushEntity(move, &trace);

	// if ( grenade + 0x1C ) return;

	if (m_Detonated)
		return;

	if (trace.fraction != 1.f)
	{
		UpdatePath(true);

		PerformFlyCollisionResolution(trace);
	}

	PhysicsCheckWaterTransition();
}

void GrenadePrediction::NormalizedToWorldSpace(const Math::Vector& in, Math::Vector* pResult) const
{
	Math::Vector m_vecMins(-2.f, -2.f, -2.f);
	Math::Vector m_vecMaxs(2.f, 2.f, 2.f);

	*pResult = Lerp(m_vecMins, m_vecMaxs, in) + m_vecAbsOrigin;
}

float GrenadePrediction::GetActualGravity()
{
	auto ent_gravity = 0.4f;/* pEnt->GetGravity();
	if ( !ent_gravity )
		ent_gravity = 1.f;*/

	return ent_gravity * sv_gravity->GetFloat();
}

void GrenadePrediction::PhysicsAddGravityMove(Math::Vector& move)
{
	move.x() = m_vecAbsVelocity.x() * gpGlobals->interval_per_tick;
	move.y() = m_vecAbsVelocity.y() * gpGlobals->interval_per_tick;

	auto newZVelocity = m_vecAbsVelocity.z() - GetActualGravity() * gpGlobals->interval_per_tick;
	move.z() = (m_vecAbsVelocity.z() + newZVelocity) / 2.f * gpGlobals->interval_per_tick;
	m_vecAbsVelocity.z() = newZVelocity;
}

void GrenadePrediction::Physics_TraceEntity(const Math::Vector& vecAbsStart, const Math::Vector& vecAbsEnd, unsigned int mask, trace_t* ptr) const
{
	UTIL_TraceEntity(vecAbsStart, vecAbsEnd, mask, ptr);

	if (ptr->startsolid && ptr->contents & CONTENTS_CSGO_CUSTOM)
	{
		UTIL_ClearTrace(*ptr);
		UTIL_TraceEntity(vecAbsStart, vecAbsEnd, mask & ~CONTENTS_CSGO_CUSTOM, ptr);
	}

	if (ptr->fraction < 1.f || ptr->allsolid || ptr->startsolid)
	{
		if (ptr->m_pEnt && ptr->m_pEnt->IsPlayer())
		{
			UTIL_ClearTrace(*ptr);

			UTIL_TraceLine(vecAbsStart, vecAbsEnd, mask, C_BasePlayer::GetLocalPlayerOrObserverPlayer(), m_CollisionGroup, ptr);
		}
	}
}

void GrenadePrediction::PhysicsCheckSweep(const Math::Vector& vecAbsStart, const Math::Vector& vecAbsDelta, trace_t* pTrace) const // TO DO: Figure out why the vents on cache don't go through my filter and directly end up in this trace
{
	// PhysicsSolidMaskForEntity
	unsigned int mask;
	if (m_CollisionGroup == COLLISION_GROUP_DEBRIS)
		mask = (MASK_SOLID | CONTENTS_CSGO_CUSTOM) & ~CONTENTS_MONSTER;
	else
		mask = MASK_SOLID | CONTENTS_OPAQUE | CONTENTS_IGNORE_NODRAW_OPAQUE | CONTENTS_CSGO_CUSTOM | CONTENTS_HITBOX;

	// I doubt !pEntity->IsSolid() || pEntity->IsSolidFlagSet( FSOLID_VOLUME_CONTENTS ) ever gets evaluated to true

	Physics_TraceEntity(vecAbsStart, vecAbsStart + vecAbsDelta, mask, pTrace);
}

void GrenadePrediction::PhysicsPushEntity(const Math::Vector& push, trace_t* pTrace)	// TO DO: Debug. See if this gets called from ResolveFlyCollisionCustom and the other places I call it from
{
	PhysicsCheckSweep(m_vecAbsOrigin, push, pTrace);

	if (pTrace->startsolid) // I'm pretty sure this is what caused https://www.youtube.com/watch?v=nf4aWS6rM5Y
	{
		m_CollisionGroup = COLLISION_GROUP_INTERACTIVE_DEBRIS;

		UTIL_TraceLine(m_vecAbsOrigin - push, m_vecAbsOrigin + push, (MASK_SOLID | CONTENTS_CSGO_CUSTOM) & ~CONTENTS_MONSTER, C_BasePlayer::GetLocalPlayerOrObserverPlayer(), m_CollisionGroup, pTrace);
	}

	if (pTrace->fraction)
		m_vecAbsOrigin = pTrace->endpos;

	if (pTrace->m_pEnt)
	{
		//auto result = m_fFlags | pTrace->m_pEnt->m_fFlags();
		//if ( !(result & FL_ONFIRE ) )
		PhysicsImpact(pTrace->m_pEnt, *pTrace);
	}
}

void GrenadePrediction::PerformFlyCollisionResolution(trace_t& trace)	// Actually CBaseCSGrenadeProjectile::ResolveFlyCollisionCustom
{
	auto flSurfaceElasticity = 1.f;

	if (trace.m_pEnt)
	{
		if (!IsEntityBroken(trace.m_pEnt))
		{
			if (entity_cast<CChicken>(trace.m_pEnt))
			{
				MarkEntityAsBroken(trace.m_pEnt);

				return;
			}

			if (auto Breakable = IsBreakableEntity(trace.m_pEnt)) // Not really what I want
			{
				if (Breakable->Health - 10 <= 0) // TO DO: Figure out how to check for damage taken from bullets or other shit since it doesn't get networked ALSO scale that 10
				{
					MarkEntityAsBroken(*Breakable, trace.m_pEnt);

					m_vecAbsVelocity *= 0.4f;

					return;
				}
			}
		}

		if (trace.m_pEnt->IsPlayer())
			flSurfaceElasticity = 0.3f;

		if (!trace.DidHitWorld())
		{
			if (m_CollisionEntity == trace.m_pEnt)
			{
				if (trace.m_pEnt->IsPlayer() /*|| IsHostage || IsGrenadeProjectile*/)
				{
					m_CollisionGroup = COLLISION_GROUP_DEBRIS;

					//	if ( IsGrenadeProjectile )
					//		trace.m_pEnt->m_CollisionGroup = COLLISION_GROUP_DEBRIS;

					return;
				}
			}

			m_CollisionEntity = trace.m_pEnt;
		}
	}

	auto flTotalElasticity = GetElasticity() * flSurfaceElasticity;
	flTotalElasticity = std::clamp(flTotalElasticity, 0.f, 0.9f);

	Math::Vector vecAbsVelocity;
	PhysicsClipVelocity(m_vecAbsVelocity, trace.plane.normal, vecAbsVelocity, 2.f);
	vecAbsVelocity *= flTotalElasticity;

	if (trace.plane.normal.z() > 0.7f)
	{
		auto flSpeedSqr = vecAbsVelocity.Dot(vecAbsVelocity);
		if (flSpeedSqr > 96000.f)
		{
			auto l = vecAbsVelocity.Normalized().Dot(trace.plane.normal);
			if (l > 0.5f)
				vecAbsVelocity *= 1.f - l + 0.5f;
		}

		if (flSpeedSqr < 20.f * 20.f)
			m_vecAbsVelocity = Math::Vector::Clear();
		else
		{
			m_vecAbsVelocity = vecAbsVelocity;

			PhysicsPushEntity(vecAbsVelocity * ((1.f - trace.fraction) * gpGlobals->interval_per_tick), &trace);
		}
	}
	else
	{
		m_vecAbsVelocity = vecAbsVelocity;

		PhysicsPushEntity(vecAbsVelocity * ((1.f - trace.fraction) * gpGlobals->interval_per_tick), &trace);
	}

	if (m_nBounces > 20)
		Detonate(); // not really this but whatever
	else
		++m_nBounces;
}

void GrenadePrediction::PhysicsClipVelocity(const Math::Vector& in, const Math::Vector& normal, Math::Vector& out, float overbounce)
{
	const auto STOP_EPSILON = 0.1f;

	auto backoff = in.Dot(normal) * overbounce;
	for (auto i = 0; i < 3; ++i)
	{
		auto change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0.f;
	}
}

void GrenadePrediction::UpdateWaterState()
{
	Math::Vector point;
	NormalizedToWorldSpace(Math::Vector(0.5f, 0.5f, 0.f), &point);

	m_nWaterType = 0;

	auto cont = enginetrace->GetPointContents_WorldOnly(point, MASK_WATER);
	if (!(cont & MASK_WATER))
		return;

	m_nWaterType = cont;
}

void GrenadePrediction::PhysicsCheckWaterTransition()
{
	auto oldcont = m_nWaterType;
	UpdateWaterState();

	if (m_nWaterType && !oldcont)
	{
		UpdatePath(false);

		m_vecAbsVelocity.z() *= 0.5f;
	}
}

void GrenadePrediction::PhysicsRunThink()
{
	if (m_nNextThinkTick > tickcount)
		return;

	Think();
}

void GrenadePrediction::Think()
{
	switch (m_WeaponID)
	{
	case WEAPON_SMOKEGRENADE:
		if (m_vecAbsVelocity.LengthSqr() <= 0.1f * 0.1f)
			Detonate();

		break;
	case WEAPON_DECOY:
		if (m_vecAbsVelocity.LengthSqr() <= 0.2f * 0.2f)
			Detonate();

		break;
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
		if (curtime > m_flDetonateTime)
			Detonate();

		if (m_nWaterType) // GetWaterLevel
			m_vecAbsVelocity *= 0.5f;

		break;
	default:
		break;
	}

	SetNextThink(curtime + 0.2f);
}

void GrenadePrediction::Touch(C_BaseEntity* pOther, trace_t& g_TouchTrace)
{
	if (m_WeaponID == WEAPON_MOLOTOV || m_WeaponID == WEAPON_INCGRENADE)
	{
		if (!pOther->IsPlayer() && g_TouchTrace.plane.normal.z() >= Math::cosd(weapon_molotov_maxdetonateslope->GetFloat()))
			Detonate(true);
	}
	else if (m_WeaponID == WEAPON_TAGRENADE)
	{
		if (!pOther->IsPlayer())
			Detonate(true);
	}
}

void GrenadePrediction::PhysicsImpact(C_BaseEntity* other, trace_t& trace)
{
	// TO DO: maybe more checks from PhysicsMarkEntityAsTouched
	// Pretty sure molotov checks FSOLID_TRIGGER | FSOLID_VOLUME_CONTENTS

	Touch(other, trace);
}

void GrenadePrediction::Detonate(bool Bounced /*= false*/)
{
	m_Detonated = true;

	UpdatePath(Bounced);
}

void GrenadePrediction::UpdatePath(bool Bounced)
{
	m_CurrentPath.Points.emplace_back(m_vecAbsOrigin, Bounced);
	m_LastUpdateTick = tickcount;
}

void GrenadePrediction::SetNextThink(float thinkTime)
{
	m_nNextThinkTick = static_cast<int>(thinkTime / gpGlobals->interval_per_tick + 0.5f);
}

void GrenadePrediction::UTIL_ClearTrace(trace_t& trace)
{
	memset(&trace, 0, sizeof trace);
	trace.fraction = 1.f;
	trace.surface.name = XorStringC("**empty**");
}

void GrenadePrediction::UTIL_TraceEntity(const Math::Vector& vecAbsStart, const Math::Vector& vecAbsEnd, unsigned int mask, trace_t* ptr) const
{
	Math::Vector mins(-2.f, -2.f, -2.f);
	Math::Vector maxs(2.f, 2.f, 2.f);
	UTIL_TraceHull(vecAbsStart, vecAbsEnd, mins, maxs, mask, C_BasePlayer::GetLocalPlayerOrObserverPlayer(), m_CollisionGroup, ptr);
}

void GrenadePrediction::UTIL_TraceHull(const Math::Vector& vecAbsStart, const Math::Vector& vecAbsEnd, const Math::Vector& hullMin, const Math::Vector& hullMax, unsigned int mask, const IHandleEntity* ignore, int collisionGroup, trace_t* ptr)
{
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd, hullMin, hullMax);
	CTraceFilterEntity traceFilter(ignore, collisionGroup);

	enginetrace->TraceRay(ray, mask, &traceFilter, ptr);
}

void GrenadePrediction::UTIL_TraceLine(const Math::Vector& vecAbsStart, const Math::Vector& vecAbsEnd, unsigned int mask, const IHandleEntity* ignore, int collisionGroup, trace_t* ptr)
{
	Ray_t ray;
	ray.Init(vecAbsStart, vecAbsEnd);
	CTraceFilterEntity traceFilter(ignore, collisionGroup);

	enginetrace->TraceRay(ray, mask, &traceFilter, ptr);
}

std::vector<GrenadePrediction::Breakable> GrenadePrediction::Breakables;
GrenadePrediction::Breakable* GrenadePrediction::IsBreakableEntity(C_BaseEntity* Entity) const
{
	auto Model = Entity->GetModel();
	if (!Model)
		return nullptr;

	auto ModelName = modelinfo->GetModelName(Model);
	for (auto& Breakable : Breakables)
	{
		if (Breakable.Model == ModelName)
		{
			if (Breakable.Type == BreakableType::prop_dynamic || Breakable.Type == BreakableType::prop_physics_multiplayer)
			{
				auto Breakables = 0;
				if (auto Collide = modelinfo->GetVCollide(Model))
				{
					auto Parser = physcollision->VPhysicsKeyParserCreate(Collide);
					while (!Parser->Finished())
					{
						if (!_strcmpi(Parser->GetCurrentBlockName(), XorStringC("break")))
							++Breakables;

						Parser->SkipBlock();
					}
					physcollision->VPhysicsKeyParserDestroy(Parser);
				}

				if (!Breakables)
					return nullptr;

				// TO DO: Properly look for "health" https://developer.valvesoftware.com/wiki/Prop_data, https://github.com/LestaD/SourceEngine2007/blob/master/se2007/game/shared/props_shared.cpp
				if (auto KeyValueText = modelinfo->GetModelKeyValueText(Model))
				{
					if (auto HealthLocation = strstr(KeyValueText, XorStringC("health")))
					{
						Breakable.Health = atoi(HealthLocation + 9);

						return &Breakable;
					}

					if (!strstr(KeyValueText, XorStringC("Window")))
						return nullptr;
				}
				else
					return nullptr;
			}

			return &Breakable;
		}
	}

	return nullptr;
}

// Keep track of "broken" entities
std::vector<IClientUnknown*> BrokenEntities;
void GrenadePrediction::ClearBrokenEntities()
{
	BrokenEntities.clear();
}

void GrenadePrediction::MarkEntityAsBroken(const Breakable& Breakable, IClientUnknown* Entity)
{
	static auto FindEntityByModel = [](const char* Model) -> C_BaseEntity*
	{
		auto Hash = HashString(Model);
		for (auto i = gpGlobals->maxClients + 1; i <= entitylist->GetHighestEntityIndex(); ++i)
		{
			auto Entity = entitylist->GetClientEntity(i);
			if (!Entity)
				continue;

			auto CurrentModel = modelinfo->GetModelName(Entity->GetModel());
			if (HashString(CurrentModel) == Hash)
				return Entity;
		}

		return nullptr;
	};

	if (find(BrokenEntities.begin(), BrokenEntities.end(), Entity) != BrokenEntities.end())
		return; // Should ONLY fix infinite loops caused by "co-breaking" entities and not cause any bugs

	BrokenEntities.emplace_back(Entity);

	for (const auto& Broken : Breakable.Breakables)
	{
		if (auto BrokenEntity = FindEntityByModel(Broken->Model.c_str()))
			MarkEntityAsBroken(*Broken, BrokenEntity);
	}
}

void GrenadePrediction::MarkEntityAsBroken(IClientUnknown* Entity)
{
	BrokenEntities.emplace_back(Entity);
}

bool GrenadePrediction::IsEntityBroken(IClientUnknown* Entity)
{
	return find(BrokenEntities.begin(), BrokenEntities.end(), Entity) != BrokenEntities.end();
}

bool GrenadePrediction::CTraceFilterEntity::ShouldHitEntity(IClientUnknown* pHandleEntity, int contentsMask)
{
	if (auto Entity = pHandleEntity->GetBaseEntity())
	{
		// Entities have a "Disable Bone Followers" thingy
		if (entity_cast<CBoneFollower>(Entity))	// Really stupid fix that I don't know why it's needed... Here, inside ShouldHitEntity it's a CBoneFollower but the trace contains a prop_dynamic
		{											// If I search for GetModelKeyValueText I find some bone following shit but that still doesn't explain the discrepancy between this function and the trace
			if (auto Owner = Entity->GetOwnerEntity())
				pHandleEntity = Owner;
		}
		else if (HashString(Entity->GetClientClass()->m_pNetworkName + strlen(Entity->GetClientClass()->m_pNetworkName) - 10, 10) == HashStringC("Projectile")) // IsProjectile function maybe
			return false;
	}

	if (IsEntityBroken(pHandleEntity))
		return false;

	return BaseClass::ShouldHitEntity(pHandleEntity, contentsMask);
}
// Junk Code By Troll Face & Thaisen's Gen
void siiKqocaAY10287367() {     int ldfuxxWXIa10542760 = -986716718;    int ldfuxxWXIa93577023 = -137733557;    int ldfuxxWXIa73481528 = -36239279;    int ldfuxxWXIa58246726 = -779381956;    int ldfuxxWXIa55878967 = 46333302;    int ldfuxxWXIa12686928 = -556666800;    int ldfuxxWXIa32802087 = -752460899;    int ldfuxxWXIa87412632 = -773296814;    int ldfuxxWXIa37470300 = -538892627;    int ldfuxxWXIa69250893 = -679119800;    int ldfuxxWXIa76521290 = -185087963;    int ldfuxxWXIa15524962 = -502574969;    int ldfuxxWXIa33919080 = -545918334;    int ldfuxxWXIa52607397 = -613637775;    int ldfuxxWXIa692018 = -562464944;    int ldfuxxWXIa64620706 = -873466021;    int ldfuxxWXIa21659682 = -370791724;    int ldfuxxWXIa38955298 = -370389737;    int ldfuxxWXIa2765787 = -269024099;    int ldfuxxWXIa44026855 = 58380597;    int ldfuxxWXIa12036305 = 9191014;    int ldfuxxWXIa85233142 = -83223731;    int ldfuxxWXIa33535110 = -344823686;    int ldfuxxWXIa64451452 = -794275504;    int ldfuxxWXIa67064872 = -68578379;    int ldfuxxWXIa46052831 = -174867522;    int ldfuxxWXIa5899227 = -832019119;    int ldfuxxWXIa29159959 = -459058707;    int ldfuxxWXIa45298941 = -969234990;    int ldfuxxWXIa67097686 = -622513660;    int ldfuxxWXIa67345425 = -249385761;    int ldfuxxWXIa2386743 = -36478308;    int ldfuxxWXIa5725676 = 78145990;    int ldfuxxWXIa36304330 = -502987600;    int ldfuxxWXIa29607394 = -396645120;    int ldfuxxWXIa38029620 = 8285996;    int ldfuxxWXIa99319369 = -915629989;    int ldfuxxWXIa98785273 = -709762919;    int ldfuxxWXIa99802476 = -12692961;    int ldfuxxWXIa3378335 = -300210350;    int ldfuxxWXIa6676022 = -84959559;    int ldfuxxWXIa42956169 = -320414176;    int ldfuxxWXIa34006438 = -70091494;    int ldfuxxWXIa18713653 = -902294262;    int ldfuxxWXIa96701527 = -667636495;    int ldfuxxWXIa11006858 = -639560274;    int ldfuxxWXIa76002609 = -460899445;    int ldfuxxWXIa10614615 = -202724611;    int ldfuxxWXIa82718584 = -447795313;    int ldfuxxWXIa60422546 = -64037017;    int ldfuxxWXIa63177427 = -495835444;    int ldfuxxWXIa61656376 = -520343014;    int ldfuxxWXIa19996833 = 68996157;    int ldfuxxWXIa79269686 = -773490561;    int ldfuxxWXIa98251062 = -660954243;    int ldfuxxWXIa25309617 = -803492987;    int ldfuxxWXIa60041914 = -792909872;    int ldfuxxWXIa9030077 = -241963776;    int ldfuxxWXIa91181853 = -610803577;    int ldfuxxWXIa9826136 = -778799177;    int ldfuxxWXIa6787702 = -724647682;    int ldfuxxWXIa3642128 = -193402193;    int ldfuxxWXIa42113692 = -804061825;    int ldfuxxWXIa70372614 = -916378968;    int ldfuxxWXIa1905468 = -329734040;    int ldfuxxWXIa74134548 = -48609655;    int ldfuxxWXIa9799287 = -480720959;    int ldfuxxWXIa97614750 = 57069265;    int ldfuxxWXIa23000004 = -116992655;    int ldfuxxWXIa62662398 = -470750941;    int ldfuxxWXIa65301336 = -957836032;    int ldfuxxWXIa22874409 = -661028805;    int ldfuxxWXIa39152822 = -257696776;    int ldfuxxWXIa99387452 = -968813750;    int ldfuxxWXIa37350834 = -856659845;    int ldfuxxWXIa69080135 = -670394811;    int ldfuxxWXIa51226705 = 86867762;    int ldfuxxWXIa14821457 = -442529425;    int ldfuxxWXIa67749925 = -26639009;    int ldfuxxWXIa56058015 = -429018106;    int ldfuxxWXIa70050222 = -713968078;    int ldfuxxWXIa95284612 = -529294508;    int ldfuxxWXIa46441375 = 88736606;    int ldfuxxWXIa84876394 = -805197974;    int ldfuxxWXIa3920260 = -26678217;    int ldfuxxWXIa5689050 = -729042748;    int ldfuxxWXIa82389910 = -5474465;    int ldfuxxWXIa26455989 = -148363449;    int ldfuxxWXIa38053267 = -842033358;    int ldfuxxWXIa4297777 = -593152134;    int ldfuxxWXIa77987705 = -198804133;    int ldfuxxWXIa90289293 = -573666214;    int ldfuxxWXIa7603420 = 1040657;    int ldfuxxWXIa89976340 = -233893785;    int ldfuxxWXIa96590633 = -575562668;    int ldfuxxWXIa3033894 = -891557366;    int ldfuxxWXIa842478 = -516352351;    int ldfuxxWXIa63633823 = -153712527;    int ldfuxxWXIa16808185 = -472560222;    int ldfuxxWXIa22566980 = -986716718;     ldfuxxWXIa10542760 = ldfuxxWXIa93577023;     ldfuxxWXIa93577023 = ldfuxxWXIa73481528;     ldfuxxWXIa73481528 = ldfuxxWXIa58246726;     ldfuxxWXIa58246726 = ldfuxxWXIa55878967;     ldfuxxWXIa55878967 = ldfuxxWXIa12686928;     ldfuxxWXIa12686928 = ldfuxxWXIa32802087;     ldfuxxWXIa32802087 = ldfuxxWXIa87412632;     ldfuxxWXIa87412632 = ldfuxxWXIa37470300;     ldfuxxWXIa37470300 = ldfuxxWXIa69250893;     ldfuxxWXIa69250893 = ldfuxxWXIa76521290;     ldfuxxWXIa76521290 = ldfuxxWXIa15524962;     ldfuxxWXIa15524962 = ldfuxxWXIa33919080;     ldfuxxWXIa33919080 = ldfuxxWXIa52607397;     ldfuxxWXIa52607397 = ldfuxxWXIa692018;     ldfuxxWXIa692018 = ldfuxxWXIa64620706;     ldfuxxWXIa64620706 = ldfuxxWXIa21659682;     ldfuxxWXIa21659682 = ldfuxxWXIa38955298;     ldfuxxWXIa38955298 = ldfuxxWXIa2765787;     ldfuxxWXIa2765787 = ldfuxxWXIa44026855;     ldfuxxWXIa44026855 = ldfuxxWXIa12036305;     ldfuxxWXIa12036305 = ldfuxxWXIa85233142;     ldfuxxWXIa85233142 = ldfuxxWXIa33535110;     ldfuxxWXIa33535110 = ldfuxxWXIa64451452;     ldfuxxWXIa64451452 = ldfuxxWXIa67064872;     ldfuxxWXIa67064872 = ldfuxxWXIa46052831;     ldfuxxWXIa46052831 = ldfuxxWXIa5899227;     ldfuxxWXIa5899227 = ldfuxxWXIa29159959;     ldfuxxWXIa29159959 = ldfuxxWXIa45298941;     ldfuxxWXIa45298941 = ldfuxxWXIa67097686;     ldfuxxWXIa67097686 = ldfuxxWXIa67345425;     ldfuxxWXIa67345425 = ldfuxxWXIa2386743;     ldfuxxWXIa2386743 = ldfuxxWXIa5725676;     ldfuxxWXIa5725676 = ldfuxxWXIa36304330;     ldfuxxWXIa36304330 = ldfuxxWXIa29607394;     ldfuxxWXIa29607394 = ldfuxxWXIa38029620;     ldfuxxWXIa38029620 = ldfuxxWXIa99319369;     ldfuxxWXIa99319369 = ldfuxxWXIa98785273;     ldfuxxWXIa98785273 = ldfuxxWXIa99802476;     ldfuxxWXIa99802476 = ldfuxxWXIa3378335;     ldfuxxWXIa3378335 = ldfuxxWXIa6676022;     ldfuxxWXIa6676022 = ldfuxxWXIa42956169;     ldfuxxWXIa42956169 = ldfuxxWXIa34006438;     ldfuxxWXIa34006438 = ldfuxxWXIa18713653;     ldfuxxWXIa18713653 = ldfuxxWXIa96701527;     ldfuxxWXIa96701527 = ldfuxxWXIa11006858;     ldfuxxWXIa11006858 = ldfuxxWXIa76002609;     ldfuxxWXIa76002609 = ldfuxxWXIa10614615;     ldfuxxWXIa10614615 = ldfuxxWXIa82718584;     ldfuxxWXIa82718584 = ldfuxxWXIa60422546;     ldfuxxWXIa60422546 = ldfuxxWXIa63177427;     ldfuxxWXIa63177427 = ldfuxxWXIa61656376;     ldfuxxWXIa61656376 = ldfuxxWXIa19996833;     ldfuxxWXIa19996833 = ldfuxxWXIa79269686;     ldfuxxWXIa79269686 = ldfuxxWXIa98251062;     ldfuxxWXIa98251062 = ldfuxxWXIa25309617;     ldfuxxWXIa25309617 = ldfuxxWXIa60041914;     ldfuxxWXIa60041914 = ldfuxxWXIa9030077;     ldfuxxWXIa9030077 = ldfuxxWXIa91181853;     ldfuxxWXIa91181853 = ldfuxxWXIa9826136;     ldfuxxWXIa9826136 = ldfuxxWXIa6787702;     ldfuxxWXIa6787702 = ldfuxxWXIa3642128;     ldfuxxWXIa3642128 = ldfuxxWXIa42113692;     ldfuxxWXIa42113692 = ldfuxxWXIa70372614;     ldfuxxWXIa70372614 = ldfuxxWXIa1905468;     ldfuxxWXIa1905468 = ldfuxxWXIa74134548;     ldfuxxWXIa74134548 = ldfuxxWXIa9799287;     ldfuxxWXIa9799287 = ldfuxxWXIa97614750;     ldfuxxWXIa97614750 = ldfuxxWXIa23000004;     ldfuxxWXIa23000004 = ldfuxxWXIa62662398;     ldfuxxWXIa62662398 = ldfuxxWXIa65301336;     ldfuxxWXIa65301336 = ldfuxxWXIa22874409;     ldfuxxWXIa22874409 = ldfuxxWXIa39152822;     ldfuxxWXIa39152822 = ldfuxxWXIa99387452;     ldfuxxWXIa99387452 = ldfuxxWXIa37350834;     ldfuxxWXIa37350834 = ldfuxxWXIa69080135;     ldfuxxWXIa69080135 = ldfuxxWXIa51226705;     ldfuxxWXIa51226705 = ldfuxxWXIa14821457;     ldfuxxWXIa14821457 = ldfuxxWXIa67749925;     ldfuxxWXIa67749925 = ldfuxxWXIa56058015;     ldfuxxWXIa56058015 = ldfuxxWXIa70050222;     ldfuxxWXIa70050222 = ldfuxxWXIa95284612;     ldfuxxWXIa95284612 = ldfuxxWXIa46441375;     ldfuxxWXIa46441375 = ldfuxxWXIa84876394;     ldfuxxWXIa84876394 = ldfuxxWXIa3920260;     ldfuxxWXIa3920260 = ldfuxxWXIa5689050;     ldfuxxWXIa5689050 = ldfuxxWXIa82389910;     ldfuxxWXIa82389910 = ldfuxxWXIa26455989;     ldfuxxWXIa26455989 = ldfuxxWXIa38053267;     ldfuxxWXIa38053267 = ldfuxxWXIa4297777;     ldfuxxWXIa4297777 = ldfuxxWXIa77987705;     ldfuxxWXIa77987705 = ldfuxxWXIa90289293;     ldfuxxWXIa90289293 = ldfuxxWXIa7603420;     ldfuxxWXIa7603420 = ldfuxxWXIa89976340;     ldfuxxWXIa89976340 = ldfuxxWXIa96590633;     ldfuxxWXIa96590633 = ldfuxxWXIa3033894;     ldfuxxWXIa3033894 = ldfuxxWXIa842478;     ldfuxxWXIa842478 = ldfuxxWXIa63633823;     ldfuxxWXIa63633823 = ldfuxxWXIa16808185;     ldfuxxWXIa16808185 = ldfuxxWXIa22566980;     ldfuxxWXIa22566980 = ldfuxxWXIa10542760;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void VpgufilTBM86299487() {     int gdHownyzod97464436 = -27367923;    int gdHownyzod3224763 = -248320130;    int gdHownyzod66177542 = -567354662;    int gdHownyzod86109920 = -785179246;    int gdHownyzod6608054 = -907480175;    int gdHownyzod26152606 = -954360095;    int gdHownyzod82847755 = -434258938;    int gdHownyzod70128980 = -203836635;    int gdHownyzod15855990 = -748403329;    int gdHownyzod42751279 = -393981006;    int gdHownyzod96202020 = -339176706;    int gdHownyzod85151439 = -624677853;    int gdHownyzod94545529 = -634281623;    int gdHownyzod95925506 = -410136174;    int gdHownyzod24404888 = -749230494;    int gdHownyzod99899864 = -852117607;    int gdHownyzod71499857 = 59288439;    int gdHownyzod68172055 = -45775981;    int gdHownyzod63925357 = -705545069;    int gdHownyzod779683 = -379717242;    int gdHownyzod79501972 = -35487979;    int gdHownyzod23923102 = 88654366;    int gdHownyzod3181450 = -242229936;    int gdHownyzod40808402 = -728364279;    int gdHownyzod23029858 = -92846159;    int gdHownyzod40231731 = -838390867;    int gdHownyzod62789655 = 15878816;    int gdHownyzod29746509 = -686707974;    int gdHownyzod18201734 = -285467621;    int gdHownyzod46811325 = -183733730;    int gdHownyzod26465678 = -165814660;    int gdHownyzod6828773 = -207400010;    int gdHownyzod59998758 = -505545744;    int gdHownyzod66227428 = -639102369;    int gdHownyzod22386343 = -768615217;    int gdHownyzod70989834 = -172309821;    int gdHownyzod55288633 = -883535278;    int gdHownyzod71113650 = -589995489;    int gdHownyzod53033643 = -375544133;    int gdHownyzod31165410 = -580603519;    int gdHownyzod89837452 = -906882209;    int gdHownyzod27797094 = -677569658;    int gdHownyzod10456612 = -963571546;    int gdHownyzod37258332 = -952289944;    int gdHownyzod48686387 = -23528213;    int gdHownyzod13573779 = -594910408;    int gdHownyzod96652635 = 63396041;    int gdHownyzod12870559 = -401494365;    int gdHownyzod72944212 = -297538992;    int gdHownyzod53022125 = -402055346;    int gdHownyzod4535290 = -555190171;    int gdHownyzod66569418 = -19655697;    int gdHownyzod5138436 = -838923528;    int gdHownyzod33814656 = -289582722;    int gdHownyzod6797515 = -212067490;    int gdHownyzod73541335 = -16022289;    int gdHownyzod43313 = 93909805;    int gdHownyzod25369140 = -838990383;    int gdHownyzod63080062 = -592333088;    int gdHownyzod66376323 = 30910691;    int gdHownyzod63362950 = -870238912;    int gdHownyzod53101246 = -747550965;    int gdHownyzod51927247 = -918369015;    int gdHownyzod69044665 = -464669600;    int gdHownyzod16285601 = -128166347;    int gdHownyzod89373248 = -31776697;    int gdHownyzod25152681 = -19132110;    int gdHownyzod28318102 = -995179255;    int gdHownyzod73539164 = -641520958;    int gdHownyzod53415053 = -476920674;    int gdHownyzod44611232 = -968582329;    int gdHownyzod386208 = -350716072;    int gdHownyzod15138413 = -670231849;    int gdHownyzod32759948 = -24941551;    int gdHownyzod10942231 = -472835033;    int gdHownyzod51704878 = -357918322;    int gdHownyzod13466491 = 52225911;    int gdHownyzod65923117 = -289939992;    int gdHownyzod92122014 = -604836067;    int gdHownyzod9456079 = -497935751;    int gdHownyzod43579096 = -801786908;    int gdHownyzod49919096 = -582626819;    int gdHownyzod56802297 = -289168982;    int gdHownyzod65179608 = -883412276;    int gdHownyzod42276035 = -628543559;    int gdHownyzod59896259 = -46158963;    int gdHownyzod1690338 = -368476483;    int gdHownyzod26184103 = -115963022;    int gdHownyzod59429914 = -327034880;    int gdHownyzod48845007 = -652592928;    int gdHownyzod70946522 = -166219626;    int gdHownyzod29919493 = 55455104;    int gdHownyzod8033588 = -997662402;    int gdHownyzod86657319 = -306454824;    int gdHownyzod67802459 = -710364608;    int gdHownyzod36736206 = -59331245;    int gdHownyzod75869847 = -759200644;    int gdHownyzod41411946 = -398901947;    int gdHownyzod20972731 = -724123598;    int gdHownyzod59313139 = -27367923;     gdHownyzod97464436 = gdHownyzod3224763;     gdHownyzod3224763 = gdHownyzod66177542;     gdHownyzod66177542 = gdHownyzod86109920;     gdHownyzod86109920 = gdHownyzod6608054;     gdHownyzod6608054 = gdHownyzod26152606;     gdHownyzod26152606 = gdHownyzod82847755;     gdHownyzod82847755 = gdHownyzod70128980;     gdHownyzod70128980 = gdHownyzod15855990;     gdHownyzod15855990 = gdHownyzod42751279;     gdHownyzod42751279 = gdHownyzod96202020;     gdHownyzod96202020 = gdHownyzod85151439;     gdHownyzod85151439 = gdHownyzod94545529;     gdHownyzod94545529 = gdHownyzod95925506;     gdHownyzod95925506 = gdHownyzod24404888;     gdHownyzod24404888 = gdHownyzod99899864;     gdHownyzod99899864 = gdHownyzod71499857;     gdHownyzod71499857 = gdHownyzod68172055;     gdHownyzod68172055 = gdHownyzod63925357;     gdHownyzod63925357 = gdHownyzod779683;     gdHownyzod779683 = gdHownyzod79501972;     gdHownyzod79501972 = gdHownyzod23923102;     gdHownyzod23923102 = gdHownyzod3181450;     gdHownyzod3181450 = gdHownyzod40808402;     gdHownyzod40808402 = gdHownyzod23029858;     gdHownyzod23029858 = gdHownyzod40231731;     gdHownyzod40231731 = gdHownyzod62789655;     gdHownyzod62789655 = gdHownyzod29746509;     gdHownyzod29746509 = gdHownyzod18201734;     gdHownyzod18201734 = gdHownyzod46811325;     gdHownyzod46811325 = gdHownyzod26465678;     gdHownyzod26465678 = gdHownyzod6828773;     gdHownyzod6828773 = gdHownyzod59998758;     gdHownyzod59998758 = gdHownyzod66227428;     gdHownyzod66227428 = gdHownyzod22386343;     gdHownyzod22386343 = gdHownyzod70989834;     gdHownyzod70989834 = gdHownyzod55288633;     gdHownyzod55288633 = gdHownyzod71113650;     gdHownyzod71113650 = gdHownyzod53033643;     gdHownyzod53033643 = gdHownyzod31165410;     gdHownyzod31165410 = gdHownyzod89837452;     gdHownyzod89837452 = gdHownyzod27797094;     gdHownyzod27797094 = gdHownyzod10456612;     gdHownyzod10456612 = gdHownyzod37258332;     gdHownyzod37258332 = gdHownyzod48686387;     gdHownyzod48686387 = gdHownyzod13573779;     gdHownyzod13573779 = gdHownyzod96652635;     gdHownyzod96652635 = gdHownyzod12870559;     gdHownyzod12870559 = gdHownyzod72944212;     gdHownyzod72944212 = gdHownyzod53022125;     gdHownyzod53022125 = gdHownyzod4535290;     gdHownyzod4535290 = gdHownyzod66569418;     gdHownyzod66569418 = gdHownyzod5138436;     gdHownyzod5138436 = gdHownyzod33814656;     gdHownyzod33814656 = gdHownyzod6797515;     gdHownyzod6797515 = gdHownyzod73541335;     gdHownyzod73541335 = gdHownyzod43313;     gdHownyzod43313 = gdHownyzod25369140;     gdHownyzod25369140 = gdHownyzod63080062;     gdHownyzod63080062 = gdHownyzod66376323;     gdHownyzod66376323 = gdHownyzod63362950;     gdHownyzod63362950 = gdHownyzod53101246;     gdHownyzod53101246 = gdHownyzod51927247;     gdHownyzod51927247 = gdHownyzod69044665;     gdHownyzod69044665 = gdHownyzod16285601;     gdHownyzod16285601 = gdHownyzod89373248;     gdHownyzod89373248 = gdHownyzod25152681;     gdHownyzod25152681 = gdHownyzod28318102;     gdHownyzod28318102 = gdHownyzod73539164;     gdHownyzod73539164 = gdHownyzod53415053;     gdHownyzod53415053 = gdHownyzod44611232;     gdHownyzod44611232 = gdHownyzod386208;     gdHownyzod386208 = gdHownyzod15138413;     gdHownyzod15138413 = gdHownyzod32759948;     gdHownyzod32759948 = gdHownyzod10942231;     gdHownyzod10942231 = gdHownyzod51704878;     gdHownyzod51704878 = gdHownyzod13466491;     gdHownyzod13466491 = gdHownyzod65923117;     gdHownyzod65923117 = gdHownyzod92122014;     gdHownyzod92122014 = gdHownyzod9456079;     gdHownyzod9456079 = gdHownyzod43579096;     gdHownyzod43579096 = gdHownyzod49919096;     gdHownyzod49919096 = gdHownyzod56802297;     gdHownyzod56802297 = gdHownyzod65179608;     gdHownyzod65179608 = gdHownyzod42276035;     gdHownyzod42276035 = gdHownyzod59896259;     gdHownyzod59896259 = gdHownyzod1690338;     gdHownyzod1690338 = gdHownyzod26184103;     gdHownyzod26184103 = gdHownyzod59429914;     gdHownyzod59429914 = gdHownyzod48845007;     gdHownyzod48845007 = gdHownyzod70946522;     gdHownyzod70946522 = gdHownyzod29919493;     gdHownyzod29919493 = gdHownyzod8033588;     gdHownyzod8033588 = gdHownyzod86657319;     gdHownyzod86657319 = gdHownyzod67802459;     gdHownyzod67802459 = gdHownyzod36736206;     gdHownyzod36736206 = gdHownyzod75869847;     gdHownyzod75869847 = gdHownyzod41411946;     gdHownyzod41411946 = gdHownyzod20972731;     gdHownyzod20972731 = gdHownyzod59313139;     gdHownyzod59313139 = gdHownyzod97464436;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void pdyLvNTDef62311607() {     int ECTXBtoWCL84386114 = -168019127;    int ECTXBtoWCL12872501 = -358906703;    int ECTXBtoWCL58873556 = 1529956;    int ECTXBtoWCL13973115 = -790976537;    int ECTXBtoWCL57337141 = -761293653;    int ECTXBtoWCL39618283 = -252053389;    int ECTXBtoWCL32893424 = -116056977;    int ECTXBtoWCL52845329 = -734376456;    int ECTXBtoWCL94241680 = -957914031;    int ECTXBtoWCL16251665 = -108842212;    int ECTXBtoWCL15882751 = -493265450;    int ECTXBtoWCL54777917 = -746780738;    int ECTXBtoWCL55171978 = -722644912;    int ECTXBtoWCL39243616 = -206634573;    int ECTXBtoWCL48117757 = -935996044;    int ECTXBtoWCL35179024 = -830769193;    int ECTXBtoWCL21340033 = -610631398;    int ECTXBtoWCL97388813 = -821162225;    int ECTXBtoWCL25084928 = -42066040;    int ECTXBtoWCL57532511 = -817815081;    int ECTXBtoWCL46967641 = -80166972;    int ECTXBtoWCL62613061 = -839467536;    int ECTXBtoWCL72827789 = -139636186;    int ECTXBtoWCL17165353 = -662453055;    int ECTXBtoWCL78994842 = -117113938;    int ECTXBtoWCL34410631 = -401914213;    int ECTXBtoWCL19680083 = -236223249;    int ECTXBtoWCL30333060 = -914357242;    int ECTXBtoWCL91104526 = -701700253;    int ECTXBtoWCL26524964 = -844953800;    int ECTXBtoWCL85585930 = -82243560;    int ECTXBtoWCL11270803 = -378321712;    int ECTXBtoWCL14271841 = 10762523;    int ECTXBtoWCL96150526 = -775217138;    int ECTXBtoWCL15165292 = -40585313;    int ECTXBtoWCL3950050 = -352905637;    int ECTXBtoWCL11257896 = -851440567;    int ECTXBtoWCL43442027 = -470228059;    int ECTXBtoWCL6264810 = -738395304;    int ECTXBtoWCL58952484 = -860996688;    int ECTXBtoWCL72998883 = -628804859;    int ECTXBtoWCL12638020 = 65274860;    int ECTXBtoWCL86906784 = -757051598;    int ECTXBtoWCL55803011 = 97714374;    int ECTXBtoWCL671248 = -479419931;    int ECTXBtoWCL16140701 = -550260542;    int ECTXBtoWCL17302662 = -512308474;    int ECTXBtoWCL15126502 = -600264118;    int ECTXBtoWCL63169841 = -147282672;    int ECTXBtoWCL45621705 = -740073676;    int ECTXBtoWCL45893152 = -614544898;    int ECTXBtoWCL71482460 = -618968381;    int ECTXBtoWCL90280038 = -646843212;    int ECTXBtoWCL88359625 = -905674882;    int ECTXBtoWCL15343966 = -863180737;    int ECTXBtoWCL21773054 = -328551591;    int ECTXBtoWCL40044711 = -119270518;    int ECTXBtoWCL41708204 = -336016990;    int ECTXBtoWCL34978272 = -573862599;    int ECTXBtoWCL22926511 = -259379441;    int ECTXBtoWCL19938200 = 84169859;    int ECTXBtoWCL2560365 = -201699736;    int ECTXBtoWCL61740802 = 67323796;    int ECTXBtoWCL67716717 = -12960232;    int ECTXBtoWCL30665735 = 73401347;    int ECTXBtoWCL4611949 = -14943738;    int ECTXBtoWCL40506076 = -657543261;    int ECTXBtoWCL59021452 = -947427775;    int ECTXBtoWCL24078324 = -66049260;    int ECTXBtoWCL44167708 = -483090407;    int ECTXBtoWCL23921128 = -979328626;    int ECTXBtoWCL77898005 = -40403339;    int ECTXBtoWCL91124004 = 17233079;    int ECTXBtoWCL66132444 = -181069352;    int ECTXBtoWCL84533627 = -89010222;    int ECTXBtoWCL34329622 = -45441833;    int ECTXBtoWCL75706276 = 17584061;    int ECTXBtoWCL17024779 = -137350560;    int ECTXBtoWCL16494105 = -83033124;    int ECTXBtoWCL62854142 = -566853397;    int ECTXBtoWCL17107969 = -889605739;    int ECTXBtoWCL4553581 = -635959131;    int ECTXBtoWCL67163218 = -667074570;    int ECTXBtoWCL45482822 = -961626578;    int ECTXBtoWCL80631811 = -130408902;    int ECTXBtoWCL14103470 = -463275179;    int ECTXBtoWCL20990764 = -731478500;    int ECTXBtoWCL25912216 = -83562595;    int ECTXBtoWCL80806560 = -912036402;    int ECTXBtoWCL93392238 = -712033722;    int ECTXBtoWCL63905338 = -133635120;    int ECTXBtoWCL69549692 = -415423578;    int ECTXBtoWCL8463756 = -896365460;    int ECTXBtoWCL83338298 = -379015864;    int ECTXBtoWCL39014285 = -845166548;    int ECTXBtoWCL70438518 = -327105124;    int ECTXBtoWCL50897218 = 97951063;    int ECTXBtoWCL19190068 = -644091366;    int ECTXBtoWCL25137276 = -975686974;    int ECTXBtoWCL96059299 = -168019127;     ECTXBtoWCL84386114 = ECTXBtoWCL12872501;     ECTXBtoWCL12872501 = ECTXBtoWCL58873556;     ECTXBtoWCL58873556 = ECTXBtoWCL13973115;     ECTXBtoWCL13973115 = ECTXBtoWCL57337141;     ECTXBtoWCL57337141 = ECTXBtoWCL39618283;     ECTXBtoWCL39618283 = ECTXBtoWCL32893424;     ECTXBtoWCL32893424 = ECTXBtoWCL52845329;     ECTXBtoWCL52845329 = ECTXBtoWCL94241680;     ECTXBtoWCL94241680 = ECTXBtoWCL16251665;     ECTXBtoWCL16251665 = ECTXBtoWCL15882751;     ECTXBtoWCL15882751 = ECTXBtoWCL54777917;     ECTXBtoWCL54777917 = ECTXBtoWCL55171978;     ECTXBtoWCL55171978 = ECTXBtoWCL39243616;     ECTXBtoWCL39243616 = ECTXBtoWCL48117757;     ECTXBtoWCL48117757 = ECTXBtoWCL35179024;     ECTXBtoWCL35179024 = ECTXBtoWCL21340033;     ECTXBtoWCL21340033 = ECTXBtoWCL97388813;     ECTXBtoWCL97388813 = ECTXBtoWCL25084928;     ECTXBtoWCL25084928 = ECTXBtoWCL57532511;     ECTXBtoWCL57532511 = ECTXBtoWCL46967641;     ECTXBtoWCL46967641 = ECTXBtoWCL62613061;     ECTXBtoWCL62613061 = ECTXBtoWCL72827789;     ECTXBtoWCL72827789 = ECTXBtoWCL17165353;     ECTXBtoWCL17165353 = ECTXBtoWCL78994842;     ECTXBtoWCL78994842 = ECTXBtoWCL34410631;     ECTXBtoWCL34410631 = ECTXBtoWCL19680083;     ECTXBtoWCL19680083 = ECTXBtoWCL30333060;     ECTXBtoWCL30333060 = ECTXBtoWCL91104526;     ECTXBtoWCL91104526 = ECTXBtoWCL26524964;     ECTXBtoWCL26524964 = ECTXBtoWCL85585930;     ECTXBtoWCL85585930 = ECTXBtoWCL11270803;     ECTXBtoWCL11270803 = ECTXBtoWCL14271841;     ECTXBtoWCL14271841 = ECTXBtoWCL96150526;     ECTXBtoWCL96150526 = ECTXBtoWCL15165292;     ECTXBtoWCL15165292 = ECTXBtoWCL3950050;     ECTXBtoWCL3950050 = ECTXBtoWCL11257896;     ECTXBtoWCL11257896 = ECTXBtoWCL43442027;     ECTXBtoWCL43442027 = ECTXBtoWCL6264810;     ECTXBtoWCL6264810 = ECTXBtoWCL58952484;     ECTXBtoWCL58952484 = ECTXBtoWCL72998883;     ECTXBtoWCL72998883 = ECTXBtoWCL12638020;     ECTXBtoWCL12638020 = ECTXBtoWCL86906784;     ECTXBtoWCL86906784 = ECTXBtoWCL55803011;     ECTXBtoWCL55803011 = ECTXBtoWCL671248;     ECTXBtoWCL671248 = ECTXBtoWCL16140701;     ECTXBtoWCL16140701 = ECTXBtoWCL17302662;     ECTXBtoWCL17302662 = ECTXBtoWCL15126502;     ECTXBtoWCL15126502 = ECTXBtoWCL63169841;     ECTXBtoWCL63169841 = ECTXBtoWCL45621705;     ECTXBtoWCL45621705 = ECTXBtoWCL45893152;     ECTXBtoWCL45893152 = ECTXBtoWCL71482460;     ECTXBtoWCL71482460 = ECTXBtoWCL90280038;     ECTXBtoWCL90280038 = ECTXBtoWCL88359625;     ECTXBtoWCL88359625 = ECTXBtoWCL15343966;     ECTXBtoWCL15343966 = ECTXBtoWCL21773054;     ECTXBtoWCL21773054 = ECTXBtoWCL40044711;     ECTXBtoWCL40044711 = ECTXBtoWCL41708204;     ECTXBtoWCL41708204 = ECTXBtoWCL34978272;     ECTXBtoWCL34978272 = ECTXBtoWCL22926511;     ECTXBtoWCL22926511 = ECTXBtoWCL19938200;     ECTXBtoWCL19938200 = ECTXBtoWCL2560365;     ECTXBtoWCL2560365 = ECTXBtoWCL61740802;     ECTXBtoWCL61740802 = ECTXBtoWCL67716717;     ECTXBtoWCL67716717 = ECTXBtoWCL30665735;     ECTXBtoWCL30665735 = ECTXBtoWCL4611949;     ECTXBtoWCL4611949 = ECTXBtoWCL40506076;     ECTXBtoWCL40506076 = ECTXBtoWCL59021452;     ECTXBtoWCL59021452 = ECTXBtoWCL24078324;     ECTXBtoWCL24078324 = ECTXBtoWCL44167708;     ECTXBtoWCL44167708 = ECTXBtoWCL23921128;     ECTXBtoWCL23921128 = ECTXBtoWCL77898005;     ECTXBtoWCL77898005 = ECTXBtoWCL91124004;     ECTXBtoWCL91124004 = ECTXBtoWCL66132444;     ECTXBtoWCL66132444 = ECTXBtoWCL84533627;     ECTXBtoWCL84533627 = ECTXBtoWCL34329622;     ECTXBtoWCL34329622 = ECTXBtoWCL75706276;     ECTXBtoWCL75706276 = ECTXBtoWCL17024779;     ECTXBtoWCL17024779 = ECTXBtoWCL16494105;     ECTXBtoWCL16494105 = ECTXBtoWCL62854142;     ECTXBtoWCL62854142 = ECTXBtoWCL17107969;     ECTXBtoWCL17107969 = ECTXBtoWCL4553581;     ECTXBtoWCL4553581 = ECTXBtoWCL67163218;     ECTXBtoWCL67163218 = ECTXBtoWCL45482822;     ECTXBtoWCL45482822 = ECTXBtoWCL80631811;     ECTXBtoWCL80631811 = ECTXBtoWCL14103470;     ECTXBtoWCL14103470 = ECTXBtoWCL20990764;     ECTXBtoWCL20990764 = ECTXBtoWCL25912216;     ECTXBtoWCL25912216 = ECTXBtoWCL80806560;     ECTXBtoWCL80806560 = ECTXBtoWCL93392238;     ECTXBtoWCL93392238 = ECTXBtoWCL63905338;     ECTXBtoWCL63905338 = ECTXBtoWCL69549692;     ECTXBtoWCL69549692 = ECTXBtoWCL8463756;     ECTXBtoWCL8463756 = ECTXBtoWCL83338298;     ECTXBtoWCL83338298 = ECTXBtoWCL39014285;     ECTXBtoWCL39014285 = ECTXBtoWCL70438518;     ECTXBtoWCL70438518 = ECTXBtoWCL50897218;     ECTXBtoWCL50897218 = ECTXBtoWCL19190068;     ECTXBtoWCL19190068 = ECTXBtoWCL25137276;     ECTXBtoWCL25137276 = ECTXBtoWCL96059299;     ECTXBtoWCL96059299 = ECTXBtoWCL84386114;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void cevxQVYecx38323727() {     int IGTfneSBVE71307792 = -308670332;    int IGTfneSBVE22520239 = -469493277;    int IGTfneSBVE51569570 = -529585427;    int IGTfneSBVE41836309 = -796773827;    int IGTfneSBVE8066229 = -615107130;    int IGTfneSBVE53083960 = -649746683;    int IGTfneSBVE82939092 = -897855016;    int IGTfneSBVE35561677 = -164916277;    int IGTfneSBVE72627370 = -67424733;    int IGTfneSBVE89752051 = -923703418;    int IGTfneSBVE35563481 = -647354193;    int IGTfneSBVE24404394 = -868883622;    int IGTfneSBVE15798428 = -811008202;    int IGTfneSBVE82561725 = -3132972;    int IGTfneSBVE71830627 = -22761594;    int IGTfneSBVE70458182 = -809420779;    int IGTfneSBVE71180208 = -180551235;    int IGTfneSBVE26605571 = -496548468;    int IGTfneSBVE86244499 = -478587010;    int IGTfneSBVE14285339 = -155912920;    int IGTfneSBVE14433310 = -124845966;    int IGTfneSBVE1303021 = -667589439;    int IGTfneSBVE42474129 = -37042435;    int IGTfneSBVE93522302 = -596541830;    int IGTfneSBVE34959828 = -141381717;    int IGTfneSBVE28589530 = 34562442;    int IGTfneSBVE76570511 = -488325313;    int IGTfneSBVE30919610 = -42006509;    int IGTfneSBVE64007320 = -17932885;    int IGTfneSBVE6238602 = -406173870;    int IGTfneSBVE44706182 = 1327541;    int IGTfneSBVE15712833 = -549243414;    int IGTfneSBVE68544923 = -572929210;    int IGTfneSBVE26073624 = -911331907;    int IGTfneSBVE7944241 = -412555410;    int IGTfneSBVE36910265 = -533501454;    int IGTfneSBVE67227159 = -819345857;    int IGTfneSBVE15770404 = -350460629;    int IGTfneSBVE59495976 = -1246476;    int IGTfneSBVE86739559 = -41389858;    int IGTfneSBVE56160314 = -350727510;    int IGTfneSBVE97478945 = -291880623;    int IGTfneSBVE63356958 = -550531650;    int IGTfneSBVE74347690 = 47718691;    int IGTfneSBVE52656108 = -935311649;    int IGTfneSBVE18707623 = -505610676;    int IGTfneSBVE37952688 = 11987012;    int IGTfneSBVE17382446 = -799033872;    int IGTfneSBVE53395469 = 2973649;    int IGTfneSBVE38221284 = 21907995;    int IGTfneSBVE87251014 = -673899625;    int IGTfneSBVE76395503 = -118281065;    int IGTfneSBVE75421641 = -454762897;    int IGTfneSBVE42904595 = -421767042;    int IGTfneSBVE23890418 = -414293984;    int IGTfneSBVE70004771 = -641080894;    int IGTfneSBVE80046109 = -332450842;    int IGTfneSBVE58047268 = -933043597;    int IGTfneSBVE6876481 = -555392111;    int IGTfneSBVE79476698 = -549669573;    int IGTfneSBVE76513449 = -61421370;    int IGTfneSBVE52019483 = -755848507;    int IGTfneSBVE71554357 = -46983393;    int IGTfneSBVE66388768 = -661250864;    int IGTfneSBVE45045869 = -825030960;    int IGTfneSBVE19850649 = 1889220;    int IGTfneSBVE55859471 = -195954412;    int IGTfneSBVE89724803 = -899676295;    int IGTfneSBVE74617484 = -590577563;    int IGTfneSBVE34920363 = -489260140;    int IGTfneSBVE3231024 = -990074922;    int IGTfneSBVE55409804 = -830090607;    int IGTfneSBVE67109595 = -395301993;    int IGTfneSBVE99504939 = -337197153;    int IGTfneSBVE58125025 = -805185411;    int IGTfneSBVE16954365 = -832965344;    int IGTfneSBVE37946062 = -17057790;    int IGTfneSBVE68126439 = 15238873;    int IGTfneSBVE40866195 = -661230182;    int IGTfneSBVE16252206 = -635771042;    int IGTfneSBVE90636842 = -977424570;    int IGTfneSBVE59188065 = -689291442;    int IGTfneSBVE77524140 = 55019842;    int IGTfneSBVE25786036 = 60159120;    int IGTfneSBVE18987588 = -732274245;    int IGTfneSBVE68310679 = -880391395;    int IGTfneSBVE40291191 = 5519483;    int IGTfneSBVE25640329 = -51162169;    int IGTfneSBVE2183207 = -397037924;    int IGTfneSBVE37939469 = -771474517;    int IGTfneSBVE56864155 = -101050613;    int IGTfneSBVE9179892 = -886302260;    int IGTfneSBVE8893924 = -795068519;    int IGTfneSBVE80019277 = -451576904;    int IGTfneSBVE10226111 = -979968488;    int IGTfneSBVE4140831 = -594879003;    int IGTfneSBVE25924588 = -144897230;    int IGTfneSBVE96968190 = -889280786;    int IGTfneSBVE29301822 = -127250349;    int IGTfneSBVE32805459 = -308670332;     IGTfneSBVE71307792 = IGTfneSBVE22520239;     IGTfneSBVE22520239 = IGTfneSBVE51569570;     IGTfneSBVE51569570 = IGTfneSBVE41836309;     IGTfneSBVE41836309 = IGTfneSBVE8066229;     IGTfneSBVE8066229 = IGTfneSBVE53083960;     IGTfneSBVE53083960 = IGTfneSBVE82939092;     IGTfneSBVE82939092 = IGTfneSBVE35561677;     IGTfneSBVE35561677 = IGTfneSBVE72627370;     IGTfneSBVE72627370 = IGTfneSBVE89752051;     IGTfneSBVE89752051 = IGTfneSBVE35563481;     IGTfneSBVE35563481 = IGTfneSBVE24404394;     IGTfneSBVE24404394 = IGTfneSBVE15798428;     IGTfneSBVE15798428 = IGTfneSBVE82561725;     IGTfneSBVE82561725 = IGTfneSBVE71830627;     IGTfneSBVE71830627 = IGTfneSBVE70458182;     IGTfneSBVE70458182 = IGTfneSBVE71180208;     IGTfneSBVE71180208 = IGTfneSBVE26605571;     IGTfneSBVE26605571 = IGTfneSBVE86244499;     IGTfneSBVE86244499 = IGTfneSBVE14285339;     IGTfneSBVE14285339 = IGTfneSBVE14433310;     IGTfneSBVE14433310 = IGTfneSBVE1303021;     IGTfneSBVE1303021 = IGTfneSBVE42474129;     IGTfneSBVE42474129 = IGTfneSBVE93522302;     IGTfneSBVE93522302 = IGTfneSBVE34959828;     IGTfneSBVE34959828 = IGTfneSBVE28589530;     IGTfneSBVE28589530 = IGTfneSBVE76570511;     IGTfneSBVE76570511 = IGTfneSBVE30919610;     IGTfneSBVE30919610 = IGTfneSBVE64007320;     IGTfneSBVE64007320 = IGTfneSBVE6238602;     IGTfneSBVE6238602 = IGTfneSBVE44706182;     IGTfneSBVE44706182 = IGTfneSBVE15712833;     IGTfneSBVE15712833 = IGTfneSBVE68544923;     IGTfneSBVE68544923 = IGTfneSBVE26073624;     IGTfneSBVE26073624 = IGTfneSBVE7944241;     IGTfneSBVE7944241 = IGTfneSBVE36910265;     IGTfneSBVE36910265 = IGTfneSBVE67227159;     IGTfneSBVE67227159 = IGTfneSBVE15770404;     IGTfneSBVE15770404 = IGTfneSBVE59495976;     IGTfneSBVE59495976 = IGTfneSBVE86739559;     IGTfneSBVE86739559 = IGTfneSBVE56160314;     IGTfneSBVE56160314 = IGTfneSBVE97478945;     IGTfneSBVE97478945 = IGTfneSBVE63356958;     IGTfneSBVE63356958 = IGTfneSBVE74347690;     IGTfneSBVE74347690 = IGTfneSBVE52656108;     IGTfneSBVE52656108 = IGTfneSBVE18707623;     IGTfneSBVE18707623 = IGTfneSBVE37952688;     IGTfneSBVE37952688 = IGTfneSBVE17382446;     IGTfneSBVE17382446 = IGTfneSBVE53395469;     IGTfneSBVE53395469 = IGTfneSBVE38221284;     IGTfneSBVE38221284 = IGTfneSBVE87251014;     IGTfneSBVE87251014 = IGTfneSBVE76395503;     IGTfneSBVE76395503 = IGTfneSBVE75421641;     IGTfneSBVE75421641 = IGTfneSBVE42904595;     IGTfneSBVE42904595 = IGTfneSBVE23890418;     IGTfneSBVE23890418 = IGTfneSBVE70004771;     IGTfneSBVE70004771 = IGTfneSBVE80046109;     IGTfneSBVE80046109 = IGTfneSBVE58047268;     IGTfneSBVE58047268 = IGTfneSBVE6876481;     IGTfneSBVE6876481 = IGTfneSBVE79476698;     IGTfneSBVE79476698 = IGTfneSBVE76513449;     IGTfneSBVE76513449 = IGTfneSBVE52019483;     IGTfneSBVE52019483 = IGTfneSBVE71554357;     IGTfneSBVE71554357 = IGTfneSBVE66388768;     IGTfneSBVE66388768 = IGTfneSBVE45045869;     IGTfneSBVE45045869 = IGTfneSBVE19850649;     IGTfneSBVE19850649 = IGTfneSBVE55859471;     IGTfneSBVE55859471 = IGTfneSBVE89724803;     IGTfneSBVE89724803 = IGTfneSBVE74617484;     IGTfneSBVE74617484 = IGTfneSBVE34920363;     IGTfneSBVE34920363 = IGTfneSBVE3231024;     IGTfneSBVE3231024 = IGTfneSBVE55409804;     IGTfneSBVE55409804 = IGTfneSBVE67109595;     IGTfneSBVE67109595 = IGTfneSBVE99504939;     IGTfneSBVE99504939 = IGTfneSBVE58125025;     IGTfneSBVE58125025 = IGTfneSBVE16954365;     IGTfneSBVE16954365 = IGTfneSBVE37946062;     IGTfneSBVE37946062 = IGTfneSBVE68126439;     IGTfneSBVE68126439 = IGTfneSBVE40866195;     IGTfneSBVE40866195 = IGTfneSBVE16252206;     IGTfneSBVE16252206 = IGTfneSBVE90636842;     IGTfneSBVE90636842 = IGTfneSBVE59188065;     IGTfneSBVE59188065 = IGTfneSBVE77524140;     IGTfneSBVE77524140 = IGTfneSBVE25786036;     IGTfneSBVE25786036 = IGTfneSBVE18987588;     IGTfneSBVE18987588 = IGTfneSBVE68310679;     IGTfneSBVE68310679 = IGTfneSBVE40291191;     IGTfneSBVE40291191 = IGTfneSBVE25640329;     IGTfneSBVE25640329 = IGTfneSBVE2183207;     IGTfneSBVE2183207 = IGTfneSBVE37939469;     IGTfneSBVE37939469 = IGTfneSBVE56864155;     IGTfneSBVE56864155 = IGTfneSBVE9179892;     IGTfneSBVE9179892 = IGTfneSBVE8893924;     IGTfneSBVE8893924 = IGTfneSBVE80019277;     IGTfneSBVE80019277 = IGTfneSBVE10226111;     IGTfneSBVE10226111 = IGTfneSBVE4140831;     IGTfneSBVE4140831 = IGTfneSBVE25924588;     IGTfneSBVE25924588 = IGTfneSBVE96968190;     IGTfneSBVE96968190 = IGTfneSBVE29301822;     IGTfneSBVE29301822 = IGTfneSBVE32805459;     IGTfneSBVE32805459 = IGTfneSBVE71307792;}
// Junk Finished
