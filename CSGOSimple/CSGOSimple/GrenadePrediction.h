class C_BaseCSGrenade;

class GrenadePrediction : public DrawingCheat
{
public:
	GrenadePrediction();

	void Init() override;

	void PopulateGeometry() override;

private:
	/**
	 * \brief Called when the map changes.
	 * \param Map Map name
	 */
	void OnNewMap(const char* Map) const;

	/**
	 * \brief Predicts the grenade and fills the path.
	 * \param Grenade Grenade entity
	 * \param Origin Thrower's eye position
	 * \param Angle Thrower's view angle
	 */
	void Predict(C_BaseCSGrenade* Grenade, const Math::Vector& Origin, const Math::QAngle& Angle);

	/**
	* \brief Adds the current position to the path.
	* \param Bounced Did the grenade collide with something in this tick?
	*/
	void UpdatePath(bool Bounced);

	void Detonate(bool Bounced = false);
	void EmitGrenade(const Math::Vector& vecSrc, const Math::Vector& vecVel);
	static float GetActualGravity();
	static float GetElasticity() { return 0.45f; }
	void NormalizedToWorldSpace(const Math::Vector& in, Math::Vector* pResult) const;
	void PerformFlyCollisionResolution(trace_t& trace);
	void PhysicsAddGravityMove(Math::Vector& move);
	void PhysicsCheckSweep(const Math::Vector& vecAbsStart, const Math::Vector& vecAbsDelta, trace_t* pTrace) const;
	void PhysicsCheckWaterTransition();
	static void PhysicsClipVelocity(const Math::Vector& in, const Math::Vector& normal, Math::Vector& out, float overbounce);
	void PhysicsImpact(C_BaseEntity* other, trace_t& trace);
	void PhysicsPushEntity(const Math::Vector& push, trace_t* pTrace);
	void PhysicsRunThink();
	void PhysicsSimulate();
	void Physics_TraceEntity(const Math::Vector& vecAbsStart, const Math::Vector& vecAbsEnd, unsigned int mask, trace_t* ptr) const;
	void SetNextThink(float thinkTime);
	void Think();
	void ThrowGrenade(C_BaseCSGrenade* Grenade, Math::Vector vecSrc, Math::QAngle angThrow);
	void Touch(C_BaseEntity* pOther, trace_t& g_TouchTrace);
	void UpdateWaterState();
	// UTIL_ functions that use our filter
	static void UTIL_ClearTrace(trace_t& trace); // Not really
	void UTIL_TraceEntity(const Math::Vector& vecAbsStart, const Math::Vector& vecAbsEnd, unsigned int mask, trace_t* ptr) const;
	static void UTIL_TraceHull(const Math::Vector& vecAbsStart, const Math::Vector& vecAbsEnd, const Math::Vector& hullMin, const Math::Vector& hullMax, unsigned int mask, const IHandleEntity* ignore, int collisionGroup, trace_t* ptr);
	static void UTIL_TraceLine(const Math::Vector& vecAbsStart, const Math::Vector& vecAbsEnd, unsigned int mask, const IHandleEntity* ignore, int collisionGroup, trace_t* ptr);

	int tickcount;
	float curtime;
	Math::Vector m_vecAbsVelocity;
	int m_nNextThinkTick;
	int m_nWaterType;
	Math::Vector m_vecAbsOrigin;
	int m_CollisionGroup;
	float m_flDetonateTime;
	int m_nBounces;
	C_BaseEntity* m_CollisionEntity;

	bool m_Enabled;
	bool m_Detonated;
	int m_WeaponID;

	struct Path
	{
		/**
		* \brief Used to store information about every point in the path.
		*/
		struct PathPoint
		{
			PathPoint(const Math::Vector& Location, bool Bounced) :
				Location(Location),
				Bounced(Bounced)
			{ }

			Math::Vector Location;
			bool Bounced;
		};
		std::deque<PathPoint> Points;

		bool Old;

		float EraseTime;
		float LastEraseTime;
	};

	Path m_CurrentPath;
	std::vector<Path> m_OldPaths;
	int m_LastUpdateTick;

	enum class BreakableType
	{
		func_breakable,
		func_breakable_surf,
		prop_physics_multiplayer,
		prop_dynamic,

		Child
	};

	struct Breakable
	{
		BreakableType Type;
		std::string Model; // Maybe Hash?
		int Health;

		std::vector<Breakable*> Breakables;
	};
	static std::vector<Breakable> Breakables;
	Breakable* IsBreakableEntity(C_BaseEntity* Entity) const;

	static void ClearBrokenEntities();
	static void MarkEntityAsBroken(const Breakable& Breakable, IClientUnknown* Entity);
	static void MarkEntityAsBroken(IClientUnknown* Entity);
	static bool IsEntityBroken(IClientUnknown* Entity);

	class CTraceFilterEntity : public CTraceFilterSimple
	{
		using BaseClass = CTraceFilterSimple;

	public:
		CTraceFilterEntity(const IClientUnknown* passentity, int collisionGroup) : CTraceFilterSimple(passentity, collisionGroup)
		{ }

		bool ShouldHitEntity(IClientUnknown* pHandleEntity, int contentsMask) override;
	};
};