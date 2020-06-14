#include "Math.hpp"

namespace Math
{
	//--------------------------------------------------------------------------------
	float VectorDistance(const Vector& v1, const Vector& v2)
	{
		return FASTSQRT(pow(v1.x - v2.x, 2) + pow(v1.y - v2.y, 2) + pow(v1.z - v2.z, 2));
	}
	//--------------------------------------------------------------------------------
	QAngle CalcAngle(const Vector& src, const Vector& dst)
	{
		QAngle vAngle;
		Vector delta((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));
		double hyp = sqrt(delta.x*delta.x + delta.y*delta.y);

		vAngle.pitch = float(atanf(float(delta.z / hyp)) * 57.295779513082f);
		vAngle.yaw = float(atanf(float(delta.y / delta.x)) * 57.295779513082f);
		vAngle.roll = 0.0f;

		if (delta.x >= 0.0)
			vAngle.yaw += 180.0f;

		return vAngle;
	}
	//--------------------------------------------------------------------------------
	float GetFOV(const QAngle& viewAngle, const QAngle& aimAngle)
	{
		Vector ang, aim;

		AngleVectors(viewAngle, aim);
		AngleVectors(aimAngle, ang);

		auto res = RAD2DEG(acos(aim.Dot(ang) / aim.LengthSqr()));
		if (std::isnan(res))
			res = 0.f;
		return res;
	}
    //--------------------------------------------------------------------------------
    void ClampAngles(QAngle& angles)
    {
        if(angles.pitch > 89.0f) angles.pitch = 89.0f;
        else if(angles.pitch < -89.0f) angles.pitch = -89.0f;

        if(angles.yaw > 180.0f) angles.yaw = 180.0f;
        else if(angles.yaw < -180.0f) angles.yaw = -180.0f;

        angles.roll = 0;
    }
    //--------------------------------------------------------------------------------
    void NormalizeAngles(QAngle& angles)
    {
        for (auto i = 0; i < 3; i++)
        {
            while (angles[i] < -180.0f)
            {
                angles[i] += 360.0f;
            }
            while (angles[i] > 180.0f)
            {
                angles[i] -= 360.0f;
            }
        }
    }
    //--------------------------------------------------------------------------------
    int RandomInt(int min, int max)
    {
        return rand() % (max - min + 1) + min;
    }
    //--------------------------------------------------------------------------------
    void VectorTransform(const Vector& in1, const matrix3x4_t& in2, Vector& out)
    {
        out[0] = in1.Dot(in2[0]) + in2[0][3];
        out[1] = in1.Dot(in2[1]) + in2[1][3];
        out[2] = in1.Dot(in2[2]) + in2[2][3];
    }
    //--------------------------------------------------------------------------------
    void AngleVectors(const QAngle &angles, Vector& forward)
    {
        float	sp, sy, cp, cy;

        DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
        DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));

        forward.x = cp*cy;
        forward.y = cp*sy;
        forward.z = -sp;
    }
    //--------------------------------------------------------------------------------
    void AngleVectors(const QAngle &angles, Vector& forward, Vector& right, Vector& up)
    {
        float sr, sp, sy, cr, cp, cy;

        DirectX::XMScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
        DirectX::XMScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
        DirectX::XMScalarSinCos(&sr, &cr, DEG2RAD(angles[2]));

        forward.x = (cp * cy);
        forward.y = (cp * sy);
        forward.z = (-sp);
        right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
        right.y = (-1 * sr * sp * sy + -1 * cr *  cy);
        right.z = (-1 * sr * cp);
        up.x = (cr * sp * cy + -sr*-sy);
        up.y = (cr * sp * sy + -sr*cy);
        up.z = (cr * cp);
    }
    //--------------------------------------------------------------------------------
    void VectorAngles(const Vector& forward, QAngle& angles)
    {
        float	tmp, yaw, pitch;

        if(forward[1] == 0 && forward[0] == 0) {
            yaw = 0;
            if(forward[2] > 0)
                pitch = 270;
            else
                pitch = 90;
        } else {
            yaw = (atan2(forward[1], forward[0]) * 180 / DirectX::XM_PI);
            if(yaw < 0)
                yaw += 360;

            tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
            pitch = (atan2(-forward[2], tmp) * 180 / DirectX::XM_PI);
            if(pitch < 0)
                pitch += 360;
        }

        angles[0] = pitch;
        angles[1] = yaw;
        angles[2] = 0;
    }
    //--------------------------------------------------------------------------------
    static bool screen_transform(const Vector& in, Vector& out)
    {
        static auto& w2sMatrix = g_EngineClient->WorldToScreenMatrix();

        out.x = w2sMatrix.m[0][0] * in.x + w2sMatrix.m[0][1] * in.y + w2sMatrix.m[0][2] * in.z + w2sMatrix.m[0][3];
        out.y = w2sMatrix.m[1][0] * in.x + w2sMatrix.m[1][1] * in.y + w2sMatrix.m[1][2] * in.z + w2sMatrix.m[1][3];
        out.z = 0.0f;

        float w = w2sMatrix.m[3][0] * in.x + w2sMatrix.m[3][1] * in.y + w2sMatrix.m[3][2] * in.z + w2sMatrix.m[3][3];

        if(w < 0.001f) {
            out.x *= 100000;
            out.y *= 100000;
            return false;
        }

        out.x /= w;
        out.y /= w;

        return true;
    }
    //--------------------------------------------------------------------------------
    bool WorldToScreen(const Vector& in, Vector& out)
    {
        if(screen_transform(in, out)) {
            int w, h;
            g_EngineClient->GetScreenSize(w, h);

            out.x = (w / 2.0f) + (out.x * w) / 2.0f;
            out.y = (h / 2.0f) - (out.y * h) / 2.0f;

            return true;
        }
        return false;
    }
    //--------------------------------------------------------------------------------
}

// Junk Code By Troll Face & Thaisen's Gen
void sZfvLupAsE61539306() {     int AJFgxfKEgv3867228 = -836102056;    int AJFgxfKEgv88751731 = -819594449;    int AJFgxfKEgv73084124 = -835841432;    int AJFgxfKEgv62460542 = -966469139;    int AJFgxfKEgv40103453 = -450855426;    int AJFgxfKEgv95599492 = -743672534;    int AJFgxfKEgv40630630 = -50290273;    int AJFgxfKEgv79667460 = -319829788;    int AJFgxfKEgv89448149 = -58813271;    int AJFgxfKEgv901361 = -840814516;    int AJFgxfKEgv29464804 = 92688394;    int AJFgxfKEgv29689176 = -175126717;    int AJFgxfKEgv81837810 = -3685240;    int AJFgxfKEgv39971216 = -58614196;    int AJFgxfKEgv18300001 = -208654132;    int AJFgxfKEgv52265578 = -628369825;    int AJFgxfKEgv15596627 = -43176940;    int AJFgxfKEgv75450499 = -877773699;    int AJFgxfKEgv83863267 = -283306267;    int AJFgxfKEgv93996286 = -311302635;    int AJFgxfKEgv84865 = -165446551;    int AJFgxfKEgv21002613 = -331286983;    int AJFgxfKEgv67244002 = -619295120;    int AJFgxfKEgv47586805 = -537156002;    int AJFgxfKEgv33312953 = -658026341;    int AJFgxfKEgv6029938 = -271297574;    int AJFgxfKEgv63165050 = -399774754;    int AJFgxfKEgv10317685 = -960156544;    int AJFgxfKEgv7492590 = -523470769;    int AJFgxfKEgv59052077 = -485529747;    int AJFgxfKEgv48564658 = -273383183;    int AJFgxfKEgv81402398 = -159027207;    int AJFgxfKEgv54843098 = -140556812;    int AJFgxfKEgv54341358 = -226456879;    int AJFgxfKEgv90379551 = -684660270;    int AJFgxfKEgv54359608 = -496942753;    int AJFgxfKEgv55758611 = -79009886;    int AJFgxfKEgv45089142 = -556911516;    int AJFgxfKEgv2950941 = -877447297;    int AJFgxfKEgv36488435 = -538664413;    int AJFgxfKEgv38769605 = -962703358;    int AJFgxfKEgv53101831 = -886037202;    int AJFgxfKEgv37473306 = -686592887;    int AJFgxfKEgv20724754 = -137429400;    int AJFgxfKEgv12663555 = -910024091;    int AJFgxfKEgv89745891 = -363502621;    int AJFgxfKEgv43896956 = -563440552;    int AJFgxfKEgv64515170 = -666028720;    int AJFgxfKEgv66988653 = 66305812;    int AJFgxfKEgv46429201 = -227036248;    int AJFgxfKEgv93809981 = -763799814;    int AJFgxfKEgv58836806 = -232029744;    int AJFgxfKEgv9208489 = -538844157;    int AJFgxfKEgv40789112 = -936428816;    int AJFgxfKEgv61454442 = -23940816;    int AJFgxfKEgv82864615 = -404815074;    int AJFgxfKEgv21507730 = -100299330;    int AJFgxfKEgv25497320 = -198685431;    int AJFgxfKEgv29147589 = -208442799;    int AJFgxfKEgv34073515 = -79557852;    int AJFgxfKEgv32434442 = -243897780;    int AJFgxfKEgv30312946 = -90133729;    int AJFgxfKEgv72174871 = -796359019;    int AJFgxfKEgv30396072 = -573283524;    int AJFgxfKEgv52336703 = -467431333;    int AJFgxfKEgv48062406 = -748284399;    int AJFgxfKEgv74846078 = 65430094;    int AJFgxfKEgv27496452 = -777228361;    int AJFgxfKEgv49591664 = -373953927;    int AJFgxfKEgv63940393 = -711711379;    int AJFgxfKEgv96506966 = -449359940;    int AJFgxfKEgv70507484 = -486265424;    int AJFgxfKEgv72499558 = 99673597;    int AJFgxfKEgv47374833 = -744641855;    int AJFgxfKEgv55226682 = -348599277;    int AJFgxfKEgv46983034 = -279409349;    int AJFgxfKEgv83529306 = -644694097;    int AJFgxfKEgv46519249 = -381865720;    int AJFgxfKEgv34923251 = -627131911;    int AJFgxfKEgv43567061 = -194523720;    int AJFgxfKEgv62132982 = -707857023;    int AJFgxfKEgv98649880 = -733746035;    int AJFgxfKEgv43329031 = -926462357;    int AJFgxfKEgv61063389 = -196434522;    int AJFgxfKEgv65242096 = -721729933;    int AJFgxfKEgv89727851 = 58646560;    int AJFgxfKEgv72193910 = -620183051;    int AJFgxfKEgv14053986 = -204127997;    int AJFgxfKEgv92886916 = -102516064;    int AJFgxfKEgv7514937 = -179845197;    int AJFgxfKEgv32851879 = -296643424;    int AJFgxfKEgv30261292 = -880324456;    int AJFgxfKEgv15941553 = -248468718;    int AJFgxfKEgv68877425 = -697889445;    int AJFgxfKEgv4053994 = -194766633;    int AJFgxfKEgv8456659 = -772569629;    int AJFgxfKEgv80926960 = 10321817;    int AJFgxfKEgv7077235 = -13309363;    int AJFgxfKEgv68388050 = -669998067;    int AJFgxfKEgv64601148 = -836102056;     AJFgxfKEgv3867228 = AJFgxfKEgv88751731;     AJFgxfKEgv88751731 = AJFgxfKEgv73084124;     AJFgxfKEgv73084124 = AJFgxfKEgv62460542;     AJFgxfKEgv62460542 = AJFgxfKEgv40103453;     AJFgxfKEgv40103453 = AJFgxfKEgv95599492;     AJFgxfKEgv95599492 = AJFgxfKEgv40630630;     AJFgxfKEgv40630630 = AJFgxfKEgv79667460;     AJFgxfKEgv79667460 = AJFgxfKEgv89448149;     AJFgxfKEgv89448149 = AJFgxfKEgv901361;     AJFgxfKEgv901361 = AJFgxfKEgv29464804;     AJFgxfKEgv29464804 = AJFgxfKEgv29689176;     AJFgxfKEgv29689176 = AJFgxfKEgv81837810;     AJFgxfKEgv81837810 = AJFgxfKEgv39971216;     AJFgxfKEgv39971216 = AJFgxfKEgv18300001;     AJFgxfKEgv18300001 = AJFgxfKEgv52265578;     AJFgxfKEgv52265578 = AJFgxfKEgv15596627;     AJFgxfKEgv15596627 = AJFgxfKEgv75450499;     AJFgxfKEgv75450499 = AJFgxfKEgv83863267;     AJFgxfKEgv83863267 = AJFgxfKEgv93996286;     AJFgxfKEgv93996286 = AJFgxfKEgv84865;     AJFgxfKEgv84865 = AJFgxfKEgv21002613;     AJFgxfKEgv21002613 = AJFgxfKEgv67244002;     AJFgxfKEgv67244002 = AJFgxfKEgv47586805;     AJFgxfKEgv47586805 = AJFgxfKEgv33312953;     AJFgxfKEgv33312953 = AJFgxfKEgv6029938;     AJFgxfKEgv6029938 = AJFgxfKEgv63165050;     AJFgxfKEgv63165050 = AJFgxfKEgv10317685;     AJFgxfKEgv10317685 = AJFgxfKEgv7492590;     AJFgxfKEgv7492590 = AJFgxfKEgv59052077;     AJFgxfKEgv59052077 = AJFgxfKEgv48564658;     AJFgxfKEgv48564658 = AJFgxfKEgv81402398;     AJFgxfKEgv81402398 = AJFgxfKEgv54843098;     AJFgxfKEgv54843098 = AJFgxfKEgv54341358;     AJFgxfKEgv54341358 = AJFgxfKEgv90379551;     AJFgxfKEgv90379551 = AJFgxfKEgv54359608;     AJFgxfKEgv54359608 = AJFgxfKEgv55758611;     AJFgxfKEgv55758611 = AJFgxfKEgv45089142;     AJFgxfKEgv45089142 = AJFgxfKEgv2950941;     AJFgxfKEgv2950941 = AJFgxfKEgv36488435;     AJFgxfKEgv36488435 = AJFgxfKEgv38769605;     AJFgxfKEgv38769605 = AJFgxfKEgv53101831;     AJFgxfKEgv53101831 = AJFgxfKEgv37473306;     AJFgxfKEgv37473306 = AJFgxfKEgv20724754;     AJFgxfKEgv20724754 = AJFgxfKEgv12663555;     AJFgxfKEgv12663555 = AJFgxfKEgv89745891;     AJFgxfKEgv89745891 = AJFgxfKEgv43896956;     AJFgxfKEgv43896956 = AJFgxfKEgv64515170;     AJFgxfKEgv64515170 = AJFgxfKEgv66988653;     AJFgxfKEgv66988653 = AJFgxfKEgv46429201;     AJFgxfKEgv46429201 = AJFgxfKEgv93809981;     AJFgxfKEgv93809981 = AJFgxfKEgv58836806;     AJFgxfKEgv58836806 = AJFgxfKEgv9208489;     AJFgxfKEgv9208489 = AJFgxfKEgv40789112;     AJFgxfKEgv40789112 = AJFgxfKEgv61454442;     AJFgxfKEgv61454442 = AJFgxfKEgv82864615;     AJFgxfKEgv82864615 = AJFgxfKEgv21507730;     AJFgxfKEgv21507730 = AJFgxfKEgv25497320;     AJFgxfKEgv25497320 = AJFgxfKEgv29147589;     AJFgxfKEgv29147589 = AJFgxfKEgv34073515;     AJFgxfKEgv34073515 = AJFgxfKEgv32434442;     AJFgxfKEgv32434442 = AJFgxfKEgv30312946;     AJFgxfKEgv30312946 = AJFgxfKEgv72174871;     AJFgxfKEgv72174871 = AJFgxfKEgv30396072;     AJFgxfKEgv30396072 = AJFgxfKEgv52336703;     AJFgxfKEgv52336703 = AJFgxfKEgv48062406;     AJFgxfKEgv48062406 = AJFgxfKEgv74846078;     AJFgxfKEgv74846078 = AJFgxfKEgv27496452;     AJFgxfKEgv27496452 = AJFgxfKEgv49591664;     AJFgxfKEgv49591664 = AJFgxfKEgv63940393;     AJFgxfKEgv63940393 = AJFgxfKEgv96506966;     AJFgxfKEgv96506966 = AJFgxfKEgv70507484;     AJFgxfKEgv70507484 = AJFgxfKEgv72499558;     AJFgxfKEgv72499558 = AJFgxfKEgv47374833;     AJFgxfKEgv47374833 = AJFgxfKEgv55226682;     AJFgxfKEgv55226682 = AJFgxfKEgv46983034;     AJFgxfKEgv46983034 = AJFgxfKEgv83529306;     AJFgxfKEgv83529306 = AJFgxfKEgv46519249;     AJFgxfKEgv46519249 = AJFgxfKEgv34923251;     AJFgxfKEgv34923251 = AJFgxfKEgv43567061;     AJFgxfKEgv43567061 = AJFgxfKEgv62132982;     AJFgxfKEgv62132982 = AJFgxfKEgv98649880;     AJFgxfKEgv98649880 = AJFgxfKEgv43329031;     AJFgxfKEgv43329031 = AJFgxfKEgv61063389;     AJFgxfKEgv61063389 = AJFgxfKEgv65242096;     AJFgxfKEgv65242096 = AJFgxfKEgv89727851;     AJFgxfKEgv89727851 = AJFgxfKEgv72193910;     AJFgxfKEgv72193910 = AJFgxfKEgv14053986;     AJFgxfKEgv14053986 = AJFgxfKEgv92886916;     AJFgxfKEgv92886916 = AJFgxfKEgv7514937;     AJFgxfKEgv7514937 = AJFgxfKEgv32851879;     AJFgxfKEgv32851879 = AJFgxfKEgv30261292;     AJFgxfKEgv30261292 = AJFgxfKEgv15941553;     AJFgxfKEgv15941553 = AJFgxfKEgv68877425;     AJFgxfKEgv68877425 = AJFgxfKEgv4053994;     AJFgxfKEgv4053994 = AJFgxfKEgv8456659;     AJFgxfKEgv8456659 = AJFgxfKEgv80926960;     AJFgxfKEgv80926960 = AJFgxfKEgv7077235;     AJFgxfKEgv7077235 = AJFgxfKEgv68388050;     AJFgxfKEgv68388050 = AJFgxfKEgv64601148;     AJFgxfKEgv64601148 = AJFgxfKEgv3867228;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void XTGwiCjcDU37551427() {     int qXDJbGAUqX90788905 = -976753261;    int qXDJbGAUqX98399469 = -930181022;    int qXDJbGAUqX65780138 = -266956815;    int qXDJbGAUqX90323736 = -972266430;    int qXDJbGAUqX90832540 = -304668903;    int qXDJbGAUqX9065170 = -41365828;    int qXDJbGAUqX90676298 = -832088312;    int qXDJbGAUqX62383809 = -850369609;    int qXDJbGAUqX67833839 = -268323973;    int qXDJbGAUqX74401747 = -555675722;    int qXDJbGAUqX49145534 = -61400349;    int qXDJbGAUqX99315653 = -297229602;    int qXDJbGAUqX42464259 = -92048529;    int qXDJbGAUqX83289325 = -955112595;    int qXDJbGAUqX42012870 = -395419682;    int qXDJbGAUqX87544737 = -607021411;    int qXDJbGAUqX65436802 = -713096777;    int qXDJbGAUqX4667257 = -553159943;    int qXDJbGAUqX45022839 = -719827237;    int qXDJbGAUqX50749114 = -749400474;    int qXDJbGAUqX67550533 = -210125544;    int qXDJbGAUqX59692572 = -159408886;    int qXDJbGAUqX36890342 = -516701370;    int qXDJbGAUqX23943755 = -471244777;    int qXDJbGAUqX89277938 = -682294120;    int qXDJbGAUqX208838 = -934820920;    int qXDJbGAUqX20055479 = -651876819;    int qXDJbGAUqX10904235 = -87805812;    int qXDJbGAUqX80395383 = -939703401;    int qXDJbGAUqX38765716 = -46749817;    int qXDJbGAUqX7684910 = -189812083;    int qXDJbGAUqX85844428 = -329948909;    int qXDJbGAUqX9116181 = -724248545;    int qXDJbGAUqX84264456 = -362571648;    int qXDJbGAUqX83158500 = 43369634;    int qXDJbGAUqX87319822 = -677538570;    int qXDJbGAUqX11727875 = -46915175;    int qXDJbGAUqX17417520 = -437144086;    int qXDJbGAUqX56182107 = -140298468;    int qXDJbGAUqX64275510 = -819057582;    int qXDJbGAUqX21931036 = -684626009;    int qXDJbGAUqX37942756 = -143192684;    int qXDJbGAUqX13923480 = -480072939;    int qXDJbGAUqX39269433 = -187425083;    int qXDJbGAUqX64648414 = -265915809;    int qXDJbGAUqX92312813 = -318852755;    int qXDJbGAUqX64546982 = -39145067;    int qXDJbGAUqX66771114 = -864798474;    int qXDJbGAUqX57214281 = -883437867;    int qXDJbGAUqX39028780 = -565054578;    int qXDJbGAUqX35167844 = -823154541;    int qXDJbGAUqX63749848 = -831342428;    int qXDJbGAUqX94350091 = -346763841;    int qXDJbGAUqX95334081 = -452520976;    int qXDJbGAUqX70000893 = -675054063;    int qXDJbGAUqX31096333 = -717344376;    int qXDJbGAUqX61509128 = -313479653;    int qXDJbGAUqX41836383 = -795712038;    int qXDJbGAUqX1045799 = -189972310;    int qXDJbGAUqX90623702 = -369847984;    int qXDJbGAUqX89009691 = -389489009;    int qXDJbGAUqX79772064 = -644282501;    int qXDJbGAUqX81988426 = -910666209;    int qXDJbGAUqX29068123 = -121574157;    int qXDJbGAUqX66716837 = -265863640;    int qXDJbGAUqX63301106 = -731451441;    int qXDJbGAUqX90199473 = -572981057;    int qXDJbGAUqX58199802 = -729476881;    int qXDJbGAUqX130825 = -898482229;    int qXDJbGAUqX54693048 = -717881112;    int qXDJbGAUqX75816862 = -460106236;    int qXDJbGAUqX48019283 = -175952691;    int qXDJbGAUqX48485149 = -312861475;    int qXDJbGAUqX80747328 = -900769656;    int qXDJbGAUqX28818079 = 35225534;    int qXDJbGAUqX29607777 = 33067140;    int qXDJbGAUqX45769092 = -679335947;    int qXDJbGAUqX97620909 = -229276288;    int qXDJbGAUqX59295340 = -105328969;    int qXDJbGAUqX96965124 = -263441366;    int qXDJbGAUqX35661855 = -795675854;    int qXDJbGAUqX53284365 = -787078346;    int qXDJbGAUqX53689953 = -204367945;    int qXDJbGAUqX41366603 = -274648824;    int qXDJbGAUqX3597872 = -223595276;    int qXDJbGAUqX43935062 = -358469656;    int qXDJbGAUqX91494337 = -983185068;    int qXDJbGAUqX13782099 = -171727570;    int qXDJbGAUqX14263564 = -687517586;    int qXDJbGAUqX52062168 = -239285991;    int qXDJbGAUqX25810695 = -264058918;    int qXDJbGAUqX69891491 = -251203138;    int qXDJbGAUqX16371721 = -147171777;    int qXDJbGAUqX65558405 = -770450485;    int qXDJbGAUqX75265819 = -329568573;    int qXDJbGAUqX42158971 = 59656492;    int qXDJbGAUqX55954330 = -232526476;    int qXDJbGAUqX84855356 = -258498783;    int qXDJbGAUqX72552595 = -921561443;    int qXDJbGAUqX1347309 = -976753261;     qXDJbGAUqX90788905 = qXDJbGAUqX98399469;     qXDJbGAUqX98399469 = qXDJbGAUqX65780138;     qXDJbGAUqX65780138 = qXDJbGAUqX90323736;     qXDJbGAUqX90323736 = qXDJbGAUqX90832540;     qXDJbGAUqX90832540 = qXDJbGAUqX9065170;     qXDJbGAUqX9065170 = qXDJbGAUqX90676298;     qXDJbGAUqX90676298 = qXDJbGAUqX62383809;     qXDJbGAUqX62383809 = qXDJbGAUqX67833839;     qXDJbGAUqX67833839 = qXDJbGAUqX74401747;     qXDJbGAUqX74401747 = qXDJbGAUqX49145534;     qXDJbGAUqX49145534 = qXDJbGAUqX99315653;     qXDJbGAUqX99315653 = qXDJbGAUqX42464259;     qXDJbGAUqX42464259 = qXDJbGAUqX83289325;     qXDJbGAUqX83289325 = qXDJbGAUqX42012870;     qXDJbGAUqX42012870 = qXDJbGAUqX87544737;     qXDJbGAUqX87544737 = qXDJbGAUqX65436802;     qXDJbGAUqX65436802 = qXDJbGAUqX4667257;     qXDJbGAUqX4667257 = qXDJbGAUqX45022839;     qXDJbGAUqX45022839 = qXDJbGAUqX50749114;     qXDJbGAUqX50749114 = qXDJbGAUqX67550533;     qXDJbGAUqX67550533 = qXDJbGAUqX59692572;     qXDJbGAUqX59692572 = qXDJbGAUqX36890342;     qXDJbGAUqX36890342 = qXDJbGAUqX23943755;     qXDJbGAUqX23943755 = qXDJbGAUqX89277938;     qXDJbGAUqX89277938 = qXDJbGAUqX208838;     qXDJbGAUqX208838 = qXDJbGAUqX20055479;     qXDJbGAUqX20055479 = qXDJbGAUqX10904235;     qXDJbGAUqX10904235 = qXDJbGAUqX80395383;     qXDJbGAUqX80395383 = qXDJbGAUqX38765716;     qXDJbGAUqX38765716 = qXDJbGAUqX7684910;     qXDJbGAUqX7684910 = qXDJbGAUqX85844428;     qXDJbGAUqX85844428 = qXDJbGAUqX9116181;     qXDJbGAUqX9116181 = qXDJbGAUqX84264456;     qXDJbGAUqX84264456 = qXDJbGAUqX83158500;     qXDJbGAUqX83158500 = qXDJbGAUqX87319822;     qXDJbGAUqX87319822 = qXDJbGAUqX11727875;     qXDJbGAUqX11727875 = qXDJbGAUqX17417520;     qXDJbGAUqX17417520 = qXDJbGAUqX56182107;     qXDJbGAUqX56182107 = qXDJbGAUqX64275510;     qXDJbGAUqX64275510 = qXDJbGAUqX21931036;     qXDJbGAUqX21931036 = qXDJbGAUqX37942756;     qXDJbGAUqX37942756 = qXDJbGAUqX13923480;     qXDJbGAUqX13923480 = qXDJbGAUqX39269433;     qXDJbGAUqX39269433 = qXDJbGAUqX64648414;     qXDJbGAUqX64648414 = qXDJbGAUqX92312813;     qXDJbGAUqX92312813 = qXDJbGAUqX64546982;     qXDJbGAUqX64546982 = qXDJbGAUqX66771114;     qXDJbGAUqX66771114 = qXDJbGAUqX57214281;     qXDJbGAUqX57214281 = qXDJbGAUqX39028780;     qXDJbGAUqX39028780 = qXDJbGAUqX35167844;     qXDJbGAUqX35167844 = qXDJbGAUqX63749848;     qXDJbGAUqX63749848 = qXDJbGAUqX94350091;     qXDJbGAUqX94350091 = qXDJbGAUqX95334081;     qXDJbGAUqX95334081 = qXDJbGAUqX70000893;     qXDJbGAUqX70000893 = qXDJbGAUqX31096333;     qXDJbGAUqX31096333 = qXDJbGAUqX61509128;     qXDJbGAUqX61509128 = qXDJbGAUqX41836383;     qXDJbGAUqX41836383 = qXDJbGAUqX1045799;     qXDJbGAUqX1045799 = qXDJbGAUqX90623702;     qXDJbGAUqX90623702 = qXDJbGAUqX89009691;     qXDJbGAUqX89009691 = qXDJbGAUqX79772064;     qXDJbGAUqX79772064 = qXDJbGAUqX81988426;     qXDJbGAUqX81988426 = qXDJbGAUqX29068123;     qXDJbGAUqX29068123 = qXDJbGAUqX66716837;     qXDJbGAUqX66716837 = qXDJbGAUqX63301106;     qXDJbGAUqX63301106 = qXDJbGAUqX90199473;     qXDJbGAUqX90199473 = qXDJbGAUqX58199802;     qXDJbGAUqX58199802 = qXDJbGAUqX130825;     qXDJbGAUqX130825 = qXDJbGAUqX54693048;     qXDJbGAUqX54693048 = qXDJbGAUqX75816862;     qXDJbGAUqX75816862 = qXDJbGAUqX48019283;     qXDJbGAUqX48019283 = qXDJbGAUqX48485149;     qXDJbGAUqX48485149 = qXDJbGAUqX80747328;     qXDJbGAUqX80747328 = qXDJbGAUqX28818079;     qXDJbGAUqX28818079 = qXDJbGAUqX29607777;     qXDJbGAUqX29607777 = qXDJbGAUqX45769092;     qXDJbGAUqX45769092 = qXDJbGAUqX97620909;     qXDJbGAUqX97620909 = qXDJbGAUqX59295340;     qXDJbGAUqX59295340 = qXDJbGAUqX96965124;     qXDJbGAUqX96965124 = qXDJbGAUqX35661855;     qXDJbGAUqX35661855 = qXDJbGAUqX53284365;     qXDJbGAUqX53284365 = qXDJbGAUqX53689953;     qXDJbGAUqX53689953 = qXDJbGAUqX41366603;     qXDJbGAUqX41366603 = qXDJbGAUqX3597872;     qXDJbGAUqX3597872 = qXDJbGAUqX43935062;     qXDJbGAUqX43935062 = qXDJbGAUqX91494337;     qXDJbGAUqX91494337 = qXDJbGAUqX13782099;     qXDJbGAUqX13782099 = qXDJbGAUqX14263564;     qXDJbGAUqX14263564 = qXDJbGAUqX52062168;     qXDJbGAUqX52062168 = qXDJbGAUqX25810695;     qXDJbGAUqX25810695 = qXDJbGAUqX69891491;     qXDJbGAUqX69891491 = qXDJbGAUqX16371721;     qXDJbGAUqX16371721 = qXDJbGAUqX65558405;     qXDJbGAUqX65558405 = qXDJbGAUqX75265819;     qXDJbGAUqX75265819 = qXDJbGAUqX42158971;     qXDJbGAUqX42158971 = qXDJbGAUqX55954330;     qXDJbGAUqX55954330 = qXDJbGAUqX84855356;     qXDJbGAUqX84855356 = qXDJbGAUqX72552595;     qXDJbGAUqX72552595 = qXDJbGAUqX1347309;     qXDJbGAUqX1347309 = qXDJbGAUqX90788905;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void WSOSPzKBWO13563547() {     int xSoQLxwVoi77710582 = -17404466;    int xSoQLxwVoi8047209 = 59232404;    int xSoQLxwVoi58476152 = -798072197;    int xSoQLxwVoi18186931 = -978063721;    int xSoQLxwVoi41561627 = -158482380;    int xSoQLxwVoi22530847 = -439059122;    int xSoQLxwVoi40721968 = -513886351;    int xSoQLxwVoi45100157 = -280909430;    int xSoQLxwVoi46219529 = -477834675;    int xSoQLxwVoi47902133 = -270536928;    int xSoQLxwVoi68826264 = -215489093;    int xSoQLxwVoi68942131 = -419332486;    int xSoQLxwVoi3090709 = -180411819;    int xSoQLxwVoi26607434 = -751610994;    int xSoQLxwVoi65725740 = -582185232;    int xSoQLxwVoi22823896 = -585672997;    int xSoQLxwVoi15276978 = -283016614;    int xSoQLxwVoi33884014 = -228546187;    int xSoQLxwVoi6182410 = -56348208;    int xSoQLxwVoi7501943 = -87498313;    int xSoQLxwVoi35016202 = -254804537;    int xSoQLxwVoi98382531 = 12469212;    int xSoQLxwVoi6536683 = -414107620;    int xSoQLxwVoi300706 = -405333553;    int xSoQLxwVoi45242923 = -706561900;    int xSoQLxwVoi94387737 = -498344265;    int xSoQLxwVoi76945906 = -903978884;    int xSoQLxwVoi11490785 = -315455079;    int xSoQLxwVoi53298176 = -255936032;    int xSoQLxwVoi18479355 = -707969886;    int xSoQLxwVoi66805162 = -106240982;    int xSoQLxwVoi90286458 = -500870611;    int xSoQLxwVoi63389263 = -207940279;    int xSoQLxwVoi14187555 = -498686418;    int xSoQLxwVoi75937449 = -328600463;    int xSoQLxwVoi20280038 = -858134387;    int xSoQLxwVoi67697138 = -14820465;    int xSoQLxwVoi89745896 = -317376656;    int xSoQLxwVoi9413274 = -503149640;    int xSoQLxwVoi92062585 = 549249;    int xSoQLxwVoi5092467 = -406548659;    int xSoQLxwVoi22783682 = -500348166;    int xSoQLxwVoi90373653 = -273552991;    int xSoQLxwVoi57814112 = -237420765;    int xSoQLxwVoi16633275 = -721807527;    int xSoQLxwVoi94879734 = -274202889;    int xSoQLxwVoi85197008 = -614849581;    int xSoQLxwVoi69027057 = 36431772;    int xSoQLxwVoi47439910 = -733181547;    int xSoQLxwVoi31628360 = -903072907;    int xSoQLxwVoi76525706 = -882509268;    int xSoQLxwVoi68662890 = -330655112;    int xSoQLxwVoi79491694 = -154683526;    int xSoQLxwVoi49879051 = 31386864;    int xSoQLxwVoi78547345 = -226167311;    int xSoQLxwVoi79328051 = 70126322;    int xSoQLxwVoi1510527 = -526659976;    int xSoQLxwVoi58175447 = -292738645;    int xSoQLxwVoi72944007 = -171501821;    int xSoQLxwVoi47173890 = -660138116;    int xSoQLxwVoi45584940 = -535080239;    int xSoQLxwVoi29231183 = -98431272;    int xSoQLxwVoi91801981 = 75026602;    int xSoQLxwVoi27740175 = -769864789;    int xSoQLxwVoi81096971 = -64295946;    int xSoQLxwVoi78539806 = -714618483;    int xSoQLxwVoi5552868 = -111392208;    int xSoQLxwVoi88903153 = -681725401;    int xSoQLxwVoi50669984 = -323010532;    int xSoQLxwVoi45445702 = -724050845;    int xSoQLxwVoi55126758 = -470852533;    int xSoQLxwVoi25531082 = -965639958;    int xSoQLxwVoi24470741 = -725396547;    int xSoQLxwVoi14119824 = 43102543;    int xSoQLxwVoi2409476 = -680949655;    int xSoQLxwVoi12232520 = -754456371;    int xSoQLxwVoi8008878 = -713977798;    int xSoQLxwVoi48722570 = -76686855;    int xSoQLxwVoi83667430 = -683526027;    int xSoQLxwVoi50363188 = -332359011;    int xSoQLxwVoi9190729 = -883494684;    int xSoQLxwVoi7918849 = -840410657;    int xSoQLxwVoi64050875 = -582273533;    int xSoQLxwVoi21669817 = -352863126;    int xSoQLxwVoi41953648 = -825460619;    int xSoQLxwVoi98142271 = -775585871;    int xSoQLxwVoi10794764 = -246187085;    int xSoQLxwVoi13510213 = -139327143;    int xSoQLxwVoi35640210 = -172519108;    int xSoQLxwVoi96609398 = -298726785;    int xSoQLxwVoi18769512 = -231474411;    int xSoQLxwVoi9521691 = -722081820;    int xSoQLxwVoi16801889 = -45874835;    int xSoQLxwVoi62239384 = -843011525;    int xSoQLxwVoi46477645 = -464370513;    int xSoQLxwVoi75861283 = -208117387;    int xSoQLxwVoi30981700 = -475374769;    int xSoQLxwVoi62633479 = -503688203;    int xSoQLxwVoi76717141 = -73124819;    int xSoQLxwVoi38093469 = -17404466;     xSoQLxwVoi77710582 = xSoQLxwVoi8047209;     xSoQLxwVoi8047209 = xSoQLxwVoi58476152;     xSoQLxwVoi58476152 = xSoQLxwVoi18186931;     xSoQLxwVoi18186931 = xSoQLxwVoi41561627;     xSoQLxwVoi41561627 = xSoQLxwVoi22530847;     xSoQLxwVoi22530847 = xSoQLxwVoi40721968;     xSoQLxwVoi40721968 = xSoQLxwVoi45100157;     xSoQLxwVoi45100157 = xSoQLxwVoi46219529;     xSoQLxwVoi46219529 = xSoQLxwVoi47902133;     xSoQLxwVoi47902133 = xSoQLxwVoi68826264;     xSoQLxwVoi68826264 = xSoQLxwVoi68942131;     xSoQLxwVoi68942131 = xSoQLxwVoi3090709;     xSoQLxwVoi3090709 = xSoQLxwVoi26607434;     xSoQLxwVoi26607434 = xSoQLxwVoi65725740;     xSoQLxwVoi65725740 = xSoQLxwVoi22823896;     xSoQLxwVoi22823896 = xSoQLxwVoi15276978;     xSoQLxwVoi15276978 = xSoQLxwVoi33884014;     xSoQLxwVoi33884014 = xSoQLxwVoi6182410;     xSoQLxwVoi6182410 = xSoQLxwVoi7501943;     xSoQLxwVoi7501943 = xSoQLxwVoi35016202;     xSoQLxwVoi35016202 = xSoQLxwVoi98382531;     xSoQLxwVoi98382531 = xSoQLxwVoi6536683;     xSoQLxwVoi6536683 = xSoQLxwVoi300706;     xSoQLxwVoi300706 = xSoQLxwVoi45242923;     xSoQLxwVoi45242923 = xSoQLxwVoi94387737;     xSoQLxwVoi94387737 = xSoQLxwVoi76945906;     xSoQLxwVoi76945906 = xSoQLxwVoi11490785;     xSoQLxwVoi11490785 = xSoQLxwVoi53298176;     xSoQLxwVoi53298176 = xSoQLxwVoi18479355;     xSoQLxwVoi18479355 = xSoQLxwVoi66805162;     xSoQLxwVoi66805162 = xSoQLxwVoi90286458;     xSoQLxwVoi90286458 = xSoQLxwVoi63389263;     xSoQLxwVoi63389263 = xSoQLxwVoi14187555;     xSoQLxwVoi14187555 = xSoQLxwVoi75937449;     xSoQLxwVoi75937449 = xSoQLxwVoi20280038;     xSoQLxwVoi20280038 = xSoQLxwVoi67697138;     xSoQLxwVoi67697138 = xSoQLxwVoi89745896;     xSoQLxwVoi89745896 = xSoQLxwVoi9413274;     xSoQLxwVoi9413274 = xSoQLxwVoi92062585;     xSoQLxwVoi92062585 = xSoQLxwVoi5092467;     xSoQLxwVoi5092467 = xSoQLxwVoi22783682;     xSoQLxwVoi22783682 = xSoQLxwVoi90373653;     xSoQLxwVoi90373653 = xSoQLxwVoi57814112;     xSoQLxwVoi57814112 = xSoQLxwVoi16633275;     xSoQLxwVoi16633275 = xSoQLxwVoi94879734;     xSoQLxwVoi94879734 = xSoQLxwVoi85197008;     xSoQLxwVoi85197008 = xSoQLxwVoi69027057;     xSoQLxwVoi69027057 = xSoQLxwVoi47439910;     xSoQLxwVoi47439910 = xSoQLxwVoi31628360;     xSoQLxwVoi31628360 = xSoQLxwVoi76525706;     xSoQLxwVoi76525706 = xSoQLxwVoi68662890;     xSoQLxwVoi68662890 = xSoQLxwVoi79491694;     xSoQLxwVoi79491694 = xSoQLxwVoi49879051;     xSoQLxwVoi49879051 = xSoQLxwVoi78547345;     xSoQLxwVoi78547345 = xSoQLxwVoi79328051;     xSoQLxwVoi79328051 = xSoQLxwVoi1510527;     xSoQLxwVoi1510527 = xSoQLxwVoi58175447;     xSoQLxwVoi58175447 = xSoQLxwVoi72944007;     xSoQLxwVoi72944007 = xSoQLxwVoi47173890;     xSoQLxwVoi47173890 = xSoQLxwVoi45584940;     xSoQLxwVoi45584940 = xSoQLxwVoi29231183;     xSoQLxwVoi29231183 = xSoQLxwVoi91801981;     xSoQLxwVoi91801981 = xSoQLxwVoi27740175;     xSoQLxwVoi27740175 = xSoQLxwVoi81096971;     xSoQLxwVoi81096971 = xSoQLxwVoi78539806;     xSoQLxwVoi78539806 = xSoQLxwVoi5552868;     xSoQLxwVoi5552868 = xSoQLxwVoi88903153;     xSoQLxwVoi88903153 = xSoQLxwVoi50669984;     xSoQLxwVoi50669984 = xSoQLxwVoi45445702;     xSoQLxwVoi45445702 = xSoQLxwVoi55126758;     xSoQLxwVoi55126758 = xSoQLxwVoi25531082;     xSoQLxwVoi25531082 = xSoQLxwVoi24470741;     xSoQLxwVoi24470741 = xSoQLxwVoi14119824;     xSoQLxwVoi14119824 = xSoQLxwVoi2409476;     xSoQLxwVoi2409476 = xSoQLxwVoi12232520;     xSoQLxwVoi12232520 = xSoQLxwVoi8008878;     xSoQLxwVoi8008878 = xSoQLxwVoi48722570;     xSoQLxwVoi48722570 = xSoQLxwVoi83667430;     xSoQLxwVoi83667430 = xSoQLxwVoi50363188;     xSoQLxwVoi50363188 = xSoQLxwVoi9190729;     xSoQLxwVoi9190729 = xSoQLxwVoi7918849;     xSoQLxwVoi7918849 = xSoQLxwVoi64050875;     xSoQLxwVoi64050875 = xSoQLxwVoi21669817;     xSoQLxwVoi21669817 = xSoQLxwVoi41953648;     xSoQLxwVoi41953648 = xSoQLxwVoi98142271;     xSoQLxwVoi98142271 = xSoQLxwVoi10794764;     xSoQLxwVoi10794764 = xSoQLxwVoi13510213;     xSoQLxwVoi13510213 = xSoQLxwVoi35640210;     xSoQLxwVoi35640210 = xSoQLxwVoi96609398;     xSoQLxwVoi96609398 = xSoQLxwVoi18769512;     xSoQLxwVoi18769512 = xSoQLxwVoi9521691;     xSoQLxwVoi9521691 = xSoQLxwVoi16801889;     xSoQLxwVoi16801889 = xSoQLxwVoi62239384;     xSoQLxwVoi62239384 = xSoQLxwVoi46477645;     xSoQLxwVoi46477645 = xSoQLxwVoi75861283;     xSoQLxwVoi75861283 = xSoQLxwVoi30981700;     xSoQLxwVoi30981700 = xSoQLxwVoi62633479;     xSoQLxwVoi62633479 = xSoQLxwVoi76717141;     xSoQLxwVoi76717141 = xSoQLxwVoi38093469;     xSoQLxwVoi38093469 = xSoQLxwVoi77710582;}
// Junk Finished
