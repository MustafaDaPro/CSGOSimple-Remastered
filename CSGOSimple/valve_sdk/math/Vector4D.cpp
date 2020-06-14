#include "Vector4D.hpp"

#include <cmath>
#include <limits>

void VectorCopy(const Vector4D& src, Vector4D& dst)
{
    dst.x = src.x;
    dst.y = src.y;
    dst.z = src.z;
    dst.w = src.w;
}
void VectorLerp(const Vector4D& src1, const Vector4D& src2, vec_t t, Vector4D& dest)
{
    dest.x = src1.x + (src2.x - src1.x) * t;
    dest.y = src1.y + (src2.y - src1.y) * t;
    dest.z = src1.z + (src2.z - src1.z) * t;
    dest.w = src1.w + (src2.w - src1.w) * t;
}
float VectorLength(const Vector4D& v)
{
    return sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w);
}

vec_t NormalizeVector(Vector4D& v)
{
    vec_t l = v.Length();
    if(l != 0.0f) {
        v /= l;
    } else {
        v.x = v.y = v.z = v.w = 0.0f;
    }
    return l;
}

Vector4D::Vector4D(void)
{
    Invalidate();
}
Vector4D::Vector4D(vec_t X, vec_t Y, vec_t Z, vec_t W)
{
    x = X;
    y = Y;
    z = Z;
    w = W;
}
Vector4D::Vector4D(vec_t* clr)
{
    x = clr[0];
    y = clr[1];
    z = clr[2];
    w = clr[3];
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------

void Vector4D::Init(vec_t ix, vec_t iy, vec_t iz, vec_t iw)
{
    x = ix; y = iy; z = iz; w = iw;
}

void Vector4D::Random(vec_t minVal, vec_t maxVal)
{
    x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    z = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    w = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
}

// This should really be a single opcode on the PowerPC (move r0 onto the vec reg)
void Vector4D::Zero()
{
    x = y = z = w = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

Vector4D& Vector4D::operator=(const Vector4D &vOther)
{
    x = vOther.x; y = vOther.y; z = vOther.z; w = vOther.w;
    return *this;
}


//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------
vec_t& Vector4D::operator[](int i)
{
    return ((vec_t*)this)[i];
}

vec_t Vector4D::operator[](int i) const
{
    return ((vec_t*)this)[i];
}


//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
vec_t* Vector4D::Base()
{
    return (vec_t*)this;
}

vec_t const* Vector4D::Base() const
{
    return (vec_t const*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

bool Vector4D::IsValid() const
{
    return !isinf(x) && !isinf(y) && !isinf(z) && !isinf(w);
}

//-----------------------------------------------------------------------------
// Invalidate
//-----------------------------------------------------------------------------

void Vector4D::Invalidate()
{
    //#ifdef _DEBUG
    //#ifdef VECTOR_PARANOIA
    x = y = z = w = std::numeric_limits<float>::infinity();
    //#endif
    //#endif
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

bool Vector4D::operator==(const Vector4D& src) const
{
    return (src.x == x) && (src.y == y) && (src.z == z) && (src.w == w);
}

bool Vector4D::operator!=(const Vector4D& src) const
{
    return (src.x != x) || (src.y != y) || (src.z != z) || (src.w != w);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------
void Vector4D::CopyToArray(float* rgfl) const
{
    rgfl[0] = x, rgfl[1] = y, rgfl[2] = z; rgfl[3] = w;
}

//-----------------------------------------------------------------------------
// standard Math operations
//-----------------------------------------------------------------------------
// #pragma message("TODO: these should be SSE")

void Vector4D::Negate()
{
    x = -x; y = -y; z = -z; w = -w;
}

// Get the component of this vector parallel to some other given vector
Vector4D Vector4D::ProjectOnto(const Vector4D& onto)
{
    return onto * (this->Dot(onto) / (onto.LengthSqr()));
}

// FIXME: Remove
// For backwards compatability
void Vector4D::MulAdd(const Vector4D& a, const Vector4D& b, float scalar)
{
    x = a.x + b.x * scalar;
    y = a.y + b.y * scalar;
    z = a.z + b.z * scalar;
    w = a.w + b.w * scalar;
}

Vector4D VectorLerp(const Vector4D& src1, const Vector4D& src2, vec_t t)
{
    Vector4D result;
    VectorLerp(src1, src2, t, result);
    return result;
}

vec_t Vector4D::Dot(const Vector4D& b) const
{
    return (x*b.x + y*b.y + z*b.z + w*b.w);
}
void VectorClear(Vector4D& a)
{
    a.x = a.y = a.z = a.w = 0.0f;
}

vec_t Vector4D::Length(void) const
{
    return sqrt(x*x + y*y + z*z + w*w);
}

// check a point against a box
bool Vector4D::WithinAABox(Vector4D const &boxmin, Vector4D const &boxmax)
{
    return (
        (x >= boxmin.x) && (x <= boxmax.x) &&
        (y >= boxmin.y) && (y <= boxmax.y) &&
        (z >= boxmin.z) && (z <= boxmax.z) &&
        (w >= boxmin.w) && (w <= boxmax.w)
        );
}

//-----------------------------------------------------------------------------
// Get the distance from this vector to the other one 
//-----------------------------------------------------------------------------
vec_t Vector4D::DistTo(const Vector4D &vOther) const
{
    Vector4D delta;
    delta = *this - vOther;
    return delta.Length();
}

//-----------------------------------------------------------------------------
// Returns a vector with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------
Vector4D Vector4D::Min(const Vector4D &vOther) const
{
    return Vector4D(x < vOther.x ? x : vOther.x,
        y < vOther.y ? y : vOther.y,
        z < vOther.z ? z : vOther.z,
        w < vOther.w ? w : vOther.w);
}

Vector4D Vector4D::Max(const Vector4D &vOther) const
{
    return Vector4D(x > vOther.x ? x : vOther.x,
        y > vOther.y ? y : vOther.y,
        z > vOther.z ? z : vOther.z,
        w > vOther.w ? w : vOther.w);
}


//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------

Vector4D Vector4D::operator-(void) const
{
    return Vector4D(-x, -y, -z, -w);
}

Vector4D Vector4D::operator+(const Vector4D& v) const
{
    return Vector4D(x + v.x, y + v.y, z + v.z, w + v.w);
}

Vector4D Vector4D::operator-(const Vector4D& v) const
{
    return Vector4D(x - v.x, y - v.y, z - v.z, w - v.w);
}

Vector4D Vector4D::operator*(float fl) const
{
    return Vector4D(x * fl, y * fl, z * fl, w * fl);
}

Vector4D Vector4D::operator*(const Vector4D& v) const
{
    return Vector4D(x * v.x, y * v.y, z * v.z, w * v.w);
}

Vector4D Vector4D::operator/(float fl) const
{
    return Vector4D(x / fl, y / fl, z / fl, w / fl);
}

Vector4D Vector4D::operator/(const Vector4D& v) const
{
    return Vector4D(x / v.x, y / v.y, z / v.z, w / v.w);
}

Vector4D operator*(float fl, const Vector4D& v)
{
    return v * fl;
}
// Junk Code By Troll Face & Thaisen's Gen
void akYXvkunen43437871() {     int ZrzixloCMC10263547 = -417360331;    int ZrzixloCMC12851018 = -333463893;    int ZrzixloCMC81303918 = 38543184;    int ZrzixloCMC23891024 = -968299863;    int ZrzixloCMC82438954 = -462585997;    int ZrzixloCMC99851811 = -232417785;    int ZrzixloCMC66960841 = -470858075;    int ZrzixloCMC58419992 = -140000258;    int ZrzixloCMC45780472 = -182869282;    int ZrzixloCMC55690957 = -924454897;    int ZrzixloCMC83048192 = -534918578;    int ZrzixloCMC88518589 = -561053944;    int ZrzixloCMC983005 = -494747331;    int ZrzixloCMC37861145 = -52245269;    int ZrzixloCMC57367222 = -730790621;    int ZrzixloCMC79195839 = -737417695;    int ZrzixloCMC15546156 = 34743112;    int ZrzixloCMC5729475 = 93156961;    int ZrzixloCMC24229448 = -479049731;    int ZrzixloCMC59286653 = -854912479;    int ZrzixloCMC337182 = -5871496;    int ZrzixloCMC96378389 = -161220215;    int ZrzixloCMC68184952 = -355318146;    int ZrzixloCMC98015315 = -226868247;    int ZrzixloCMC72038737 = -549900377;    int ZrzixloCMC88402222 = -712410210;    int ZrzixloCMC33762028 = -16228038;    int ZrzixloCMC26292385 = -742572102;    int ZrzixloCMC9461893 = -307544232;    int ZrzixloCMC47382700 = -462757137;    int ZrzixloCMC4076317 = -768044941;    int ZrzixloCMC40699882 = -502476165;    int ZrzixloCMC35139861 = 22487904;    int ZrzixloCMC42738126 = -790493122;    int ZrzixloCMC40730799 = -860019248;    int ZrzixloCMC80557570 = -901341432;    int ZrzixloCMC68169957 = -474137872;    int ZrzixloCMC88982313 = -519090223;    int ZrzixloCMC88181835 = -992031877;    int ZrzixloCMC13684354 = -106156992;    int ZrzixloCMC75557425 = -6468406;    int ZrzixloCMC64104228 = -188296828;    int ZrzixloCMC24773361 = -968744482;    int ZrzixloCMC63423073 = 20466700;    int ZrzixloCMC60658773 = -69779370;    int ZrzixloCMC6345972 = 55860494;    int ZrzixloCMC18839070 = -571557767;    int ZrzixloCMC91543362 = -439324432;    int ZrzixloCMC79691483 = -233613244;    int ZrzixloCMC59881700 = -739042036;    int ZrzixloCMC96344043 = -550964465;    int ZrzixloCMC65651451 = -826549539;    int ZrzixloCMC88726889 = -130818794;    int ZrzixloCMC58013839 = 84805239;    int ZrzixloCMC43100690 = -692713421;    int ZrzixloCMC13885158 = -156140117;    int ZrzixloCMC44666066 = -978145748;    int ZrzixloCMC83288603 = -734588570;    int ZrzixloCMC51852287 = -318399487;    int ZrzixloCMC94036732 = -750175788;    int ZrzixloCMC66089783 = -116189747;    int ZrzixloCMC40668457 = -728285973;    int ZrzixloCMC48958099 = -832456027;    int ZrzixloCMC98397772 = -720112145;    int ZrzixloCMC51614640 = -56409956;    int ZrzixloCMC42348311 = 67557587;    int ZrzixloCMC53378729 = -483541849;    int ZrzixloCMC58244878 = -704254210;    int ZrzixloCMC97130346 = -192226022;    int ZrzixloCMC76809652 = -829449189;    int ZrzixloCMC11025882 = -163279823;    int ZrzixloCMC26563842 = -446166666;    int ZrzixloCMC17547640 = 85188838;    int ZrzixloCMC10545095 = -272892740;    int ZrzixloCMC83729228 = -748444073;    int ZrzixloCMC36232953 = -817574669;    int ZrzixloCMC71605028 = -192475734;    int ZrzixloCMC4761879 = -275784847;    int ZrzixloCMC37356542 = -57088877;    int ZrzixloCMC65692765 = -505760871;    int ZrzixloCMC69563152 = -40852443;    int ZrzixloCMC42218665 = -576903607;    int ZrzixloCMC46600901 = -408958858;    int ZrzixloCMC49580193 = -568502196;    int ZrzixloCMC51038657 = -911792673;    int ZrzixloCMC38424866 = -941495403;    int ZrzixloCMC51972992 = -271657372;    int ZrzixloCMC77126021 = 37682665;    int ZrzixloCMC99637436 = 2220298;    int ZrzixloCMC26845641 = -603879132;    int ZrzixloCMC35891505 = -923195685;    int ZrzixloCMC84881354 = -739549303;    int ZrzixloCMC37130027 = -100690737;    int ZrzixloCMC94145103 = -141856090;    int ZrzixloCMC47594570 = -989967246;    int ZrzixloCMC34888968 = -278182433;    int ZrzixloCMC15146130 = -355840801;    int ZrzixloCMC26375589 = -148632338;    int ZrzixloCMC11808433 = -923123344;    int ZrzixloCMC18310463 = -417360331;     ZrzixloCMC10263547 = ZrzixloCMC12851018;     ZrzixloCMC12851018 = ZrzixloCMC81303918;     ZrzixloCMC81303918 = ZrzixloCMC23891024;     ZrzixloCMC23891024 = ZrzixloCMC82438954;     ZrzixloCMC82438954 = ZrzixloCMC99851811;     ZrzixloCMC99851811 = ZrzixloCMC66960841;     ZrzixloCMC66960841 = ZrzixloCMC58419992;     ZrzixloCMC58419992 = ZrzixloCMC45780472;     ZrzixloCMC45780472 = ZrzixloCMC55690957;     ZrzixloCMC55690957 = ZrzixloCMC83048192;     ZrzixloCMC83048192 = ZrzixloCMC88518589;     ZrzixloCMC88518589 = ZrzixloCMC983005;     ZrzixloCMC983005 = ZrzixloCMC37861145;     ZrzixloCMC37861145 = ZrzixloCMC57367222;     ZrzixloCMC57367222 = ZrzixloCMC79195839;     ZrzixloCMC79195839 = ZrzixloCMC15546156;     ZrzixloCMC15546156 = ZrzixloCMC5729475;     ZrzixloCMC5729475 = ZrzixloCMC24229448;     ZrzixloCMC24229448 = ZrzixloCMC59286653;     ZrzixloCMC59286653 = ZrzixloCMC337182;     ZrzixloCMC337182 = ZrzixloCMC96378389;     ZrzixloCMC96378389 = ZrzixloCMC68184952;     ZrzixloCMC68184952 = ZrzixloCMC98015315;     ZrzixloCMC98015315 = ZrzixloCMC72038737;     ZrzixloCMC72038737 = ZrzixloCMC88402222;     ZrzixloCMC88402222 = ZrzixloCMC33762028;     ZrzixloCMC33762028 = ZrzixloCMC26292385;     ZrzixloCMC26292385 = ZrzixloCMC9461893;     ZrzixloCMC9461893 = ZrzixloCMC47382700;     ZrzixloCMC47382700 = ZrzixloCMC4076317;     ZrzixloCMC4076317 = ZrzixloCMC40699882;     ZrzixloCMC40699882 = ZrzixloCMC35139861;     ZrzixloCMC35139861 = ZrzixloCMC42738126;     ZrzixloCMC42738126 = ZrzixloCMC40730799;     ZrzixloCMC40730799 = ZrzixloCMC80557570;     ZrzixloCMC80557570 = ZrzixloCMC68169957;     ZrzixloCMC68169957 = ZrzixloCMC88982313;     ZrzixloCMC88982313 = ZrzixloCMC88181835;     ZrzixloCMC88181835 = ZrzixloCMC13684354;     ZrzixloCMC13684354 = ZrzixloCMC75557425;     ZrzixloCMC75557425 = ZrzixloCMC64104228;     ZrzixloCMC64104228 = ZrzixloCMC24773361;     ZrzixloCMC24773361 = ZrzixloCMC63423073;     ZrzixloCMC63423073 = ZrzixloCMC60658773;     ZrzixloCMC60658773 = ZrzixloCMC6345972;     ZrzixloCMC6345972 = ZrzixloCMC18839070;     ZrzixloCMC18839070 = ZrzixloCMC91543362;     ZrzixloCMC91543362 = ZrzixloCMC79691483;     ZrzixloCMC79691483 = ZrzixloCMC59881700;     ZrzixloCMC59881700 = ZrzixloCMC96344043;     ZrzixloCMC96344043 = ZrzixloCMC65651451;     ZrzixloCMC65651451 = ZrzixloCMC88726889;     ZrzixloCMC88726889 = ZrzixloCMC58013839;     ZrzixloCMC58013839 = ZrzixloCMC43100690;     ZrzixloCMC43100690 = ZrzixloCMC13885158;     ZrzixloCMC13885158 = ZrzixloCMC44666066;     ZrzixloCMC44666066 = ZrzixloCMC83288603;     ZrzixloCMC83288603 = ZrzixloCMC51852287;     ZrzixloCMC51852287 = ZrzixloCMC94036732;     ZrzixloCMC94036732 = ZrzixloCMC66089783;     ZrzixloCMC66089783 = ZrzixloCMC40668457;     ZrzixloCMC40668457 = ZrzixloCMC48958099;     ZrzixloCMC48958099 = ZrzixloCMC98397772;     ZrzixloCMC98397772 = ZrzixloCMC51614640;     ZrzixloCMC51614640 = ZrzixloCMC42348311;     ZrzixloCMC42348311 = ZrzixloCMC53378729;     ZrzixloCMC53378729 = ZrzixloCMC58244878;     ZrzixloCMC58244878 = ZrzixloCMC97130346;     ZrzixloCMC97130346 = ZrzixloCMC76809652;     ZrzixloCMC76809652 = ZrzixloCMC11025882;     ZrzixloCMC11025882 = ZrzixloCMC26563842;     ZrzixloCMC26563842 = ZrzixloCMC17547640;     ZrzixloCMC17547640 = ZrzixloCMC10545095;     ZrzixloCMC10545095 = ZrzixloCMC83729228;     ZrzixloCMC83729228 = ZrzixloCMC36232953;     ZrzixloCMC36232953 = ZrzixloCMC71605028;     ZrzixloCMC71605028 = ZrzixloCMC4761879;     ZrzixloCMC4761879 = ZrzixloCMC37356542;     ZrzixloCMC37356542 = ZrzixloCMC65692765;     ZrzixloCMC65692765 = ZrzixloCMC69563152;     ZrzixloCMC69563152 = ZrzixloCMC42218665;     ZrzixloCMC42218665 = ZrzixloCMC46600901;     ZrzixloCMC46600901 = ZrzixloCMC49580193;     ZrzixloCMC49580193 = ZrzixloCMC51038657;     ZrzixloCMC51038657 = ZrzixloCMC38424866;     ZrzixloCMC38424866 = ZrzixloCMC51972992;     ZrzixloCMC51972992 = ZrzixloCMC77126021;     ZrzixloCMC77126021 = ZrzixloCMC99637436;     ZrzixloCMC99637436 = ZrzixloCMC26845641;     ZrzixloCMC26845641 = ZrzixloCMC35891505;     ZrzixloCMC35891505 = ZrzixloCMC84881354;     ZrzixloCMC84881354 = ZrzixloCMC37130027;     ZrzixloCMC37130027 = ZrzixloCMC94145103;     ZrzixloCMC94145103 = ZrzixloCMC47594570;     ZrzixloCMC47594570 = ZrzixloCMC34888968;     ZrzixloCMC34888968 = ZrzixloCMC15146130;     ZrzixloCMC15146130 = ZrzixloCMC26375589;     ZrzixloCMC26375589 = ZrzixloCMC11808433;     ZrzixloCMC11808433 = ZrzixloCMC18310463;     ZrzixloCMC18310463 = ZrzixloCMC10263547;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ITLOYAWBcb19449991() {     int EJPewLISGa97185224 = -558011536;    int EJPewLISGa22498756 = -444050467;    int EJPewLISGa73999932 = -492572199;    int EJPewLISGa51754218 = -974097153;    int EJPewLISGa33168041 = -316399475;    int EJPewLISGa13317489 = -630111079;    int EJPewLISGa17006510 = -152656114;    int EJPewLISGa41136340 = -670540079;    int EJPewLISGa24166163 = -392379984;    int EJPewLISGa29191343 = -639316103;    int EJPewLISGa2728923 = -689007321;    int EJPewLISGa58145067 = -683156829;    int EJPewLISGa61609453 = -583110621;    int EJPewLISGa81179254 = -948743669;    int EJPewLISGa81080092 = -917556171;    int EJPewLISGa14474998 = -716069281;    int EJPewLISGa65386331 = -635176725;    int EJPewLISGa34946233 = -682229283;    int EJPewLISGa85389018 = -915570702;    int EJPewLISGa16039481 = -193010318;    int EJPewLISGa67802849 = -50550489;    int EJPewLISGa35068349 = 10657882;    int EJPewLISGa37831292 = -252724396;    int EJPewLISGa74372265 = -160957022;    int EJPewLISGa28003723 = -574168156;    int EJPewLISGa82581121 = -275933555;    int EJPewLISGa90652455 = -268330103;    int EJPewLISGa26878935 = -970221370;    int EJPewLISGa82364686 = -723776863;    int EJPewLISGa27096339 = -23977207;    int EJPewLISGa63196569 = -684473841;    int EJPewLISGa45141911 = -673397867;    int EJPewLISGa89412943 = -561203830;    int EJPewLISGa72661224 = -926607891;    int EJPewLISGa33509748 = -131989344;    int EJPewLISGa13517786 = 18062751;    int EJPewLISGa24139221 = -442043161;    int EJPewLISGa61310691 = -399322793;    int EJPewLISGa41413002 = -254883049;    int EJPewLISGa41471429 = -386550162;    int EJPewLISGa58718856 = -828391056;    int EJPewLISGa48945154 = -545452310;    int EJPewLISGa1223535 = -762224534;    int EJPewLISGa81967752 = -29528982;    int EJPewLISGa12643634 = -525671088;    int EJPewLISGa8912894 = -999489640;    int EJPewLISGa39489096 = -47262282;    int EJPewLISGa93799306 = -638094186;    int EJPewLISGa69917111 = -83356924;    int EJPewLISGa52481279 = 22939634;    int EJPewLISGa37701906 = -610319192;    int EJPewLISGa70564493 = -325862223;    int EJPewLISGa73868492 = 61261522;    int EJPewLISGa12558809 = -531286921;    int EJPewLISGa51647141 = -243826668;    int EJPewLISGa62116876 = -468669419;    int EJPewLISGa84667464 = -91326071;    int EJPewLISGa99627666 = -231615177;    int EJPewLISGa23750496 = -299928998;    int EJPewLISGa50586920 = 59534080;    int EJPewLISGa22665033 = -261780976;    int EJPewLISGa90127575 = -182434744;    int EJPewLISGa58771654 = -946763216;    int EJPewLISGa97069823 = -268402777;    int EJPewLISGa65994774 = -954842263;    int EJPewLISGa57587011 = 84390546;    int EJPewLISGa68732124 = -21953000;    int EJPewLISGa88948229 = -656502730;    int EJPewLISGa47669506 = -716754325;    int EJPewLISGa67562307 = -835618922;    int EJPewLISGa90335776 = -174026120;    int EJPewLISGa4075641 = -135853933;    int EJPewLISGa93533230 = -327346235;    int EJPewLISGa43917590 = -429020541;    int EJPewLISGa57320625 = -364619262;    int EJPewLISGa18857696 = -505098180;    int EJPewLISGa33844814 = -227117584;    int EJPewLISGa55863539 = -123195414;    int EJPewLISGa61728632 = -635285935;    int EJPewLISGa19090829 = -574678517;    int EJPewLISGa43092026 = -128671274;    int EJPewLISGa96853149 = -630235918;    int EJPewLISGa56961823 = -786864446;    int EJPewLISGa29883407 = -646716498;    int EJPewLISGa89394432 = -413658016;    int EJPewLISGa92632075 = -258611618;    int EJPewLISGa71273419 = -634659389;    int EJPewLISGa76854135 = 70083091;    int EJPewLISGa21014084 = -582781224;    int EJPewLISGa71392872 = -663319926;    int EJPewLISGa28850321 = -890611179;    int EJPewLISGa24511555 = -110427985;    int EJPewLISGa37560195 = 606205;    int EJPewLISGa90826082 = -214417129;    int EJPewLISGa18806396 = -24769186;    int EJPewLISGa68591280 = -545956312;    int EJPewLISGa90173499 = -598689094;    int EJPewLISGa4153711 = -393821758;    int EJPewLISGa15972979 = -74686720;    int EJPewLISGa55056622 = -558011536;     EJPewLISGa97185224 = EJPewLISGa22498756;     EJPewLISGa22498756 = EJPewLISGa73999932;     EJPewLISGa73999932 = EJPewLISGa51754218;     EJPewLISGa51754218 = EJPewLISGa33168041;     EJPewLISGa33168041 = EJPewLISGa13317489;     EJPewLISGa13317489 = EJPewLISGa17006510;     EJPewLISGa17006510 = EJPewLISGa41136340;     EJPewLISGa41136340 = EJPewLISGa24166163;     EJPewLISGa24166163 = EJPewLISGa29191343;     EJPewLISGa29191343 = EJPewLISGa2728923;     EJPewLISGa2728923 = EJPewLISGa58145067;     EJPewLISGa58145067 = EJPewLISGa61609453;     EJPewLISGa61609453 = EJPewLISGa81179254;     EJPewLISGa81179254 = EJPewLISGa81080092;     EJPewLISGa81080092 = EJPewLISGa14474998;     EJPewLISGa14474998 = EJPewLISGa65386331;     EJPewLISGa65386331 = EJPewLISGa34946233;     EJPewLISGa34946233 = EJPewLISGa85389018;     EJPewLISGa85389018 = EJPewLISGa16039481;     EJPewLISGa16039481 = EJPewLISGa67802849;     EJPewLISGa67802849 = EJPewLISGa35068349;     EJPewLISGa35068349 = EJPewLISGa37831292;     EJPewLISGa37831292 = EJPewLISGa74372265;     EJPewLISGa74372265 = EJPewLISGa28003723;     EJPewLISGa28003723 = EJPewLISGa82581121;     EJPewLISGa82581121 = EJPewLISGa90652455;     EJPewLISGa90652455 = EJPewLISGa26878935;     EJPewLISGa26878935 = EJPewLISGa82364686;     EJPewLISGa82364686 = EJPewLISGa27096339;     EJPewLISGa27096339 = EJPewLISGa63196569;     EJPewLISGa63196569 = EJPewLISGa45141911;     EJPewLISGa45141911 = EJPewLISGa89412943;     EJPewLISGa89412943 = EJPewLISGa72661224;     EJPewLISGa72661224 = EJPewLISGa33509748;     EJPewLISGa33509748 = EJPewLISGa13517786;     EJPewLISGa13517786 = EJPewLISGa24139221;     EJPewLISGa24139221 = EJPewLISGa61310691;     EJPewLISGa61310691 = EJPewLISGa41413002;     EJPewLISGa41413002 = EJPewLISGa41471429;     EJPewLISGa41471429 = EJPewLISGa58718856;     EJPewLISGa58718856 = EJPewLISGa48945154;     EJPewLISGa48945154 = EJPewLISGa1223535;     EJPewLISGa1223535 = EJPewLISGa81967752;     EJPewLISGa81967752 = EJPewLISGa12643634;     EJPewLISGa12643634 = EJPewLISGa8912894;     EJPewLISGa8912894 = EJPewLISGa39489096;     EJPewLISGa39489096 = EJPewLISGa93799306;     EJPewLISGa93799306 = EJPewLISGa69917111;     EJPewLISGa69917111 = EJPewLISGa52481279;     EJPewLISGa52481279 = EJPewLISGa37701906;     EJPewLISGa37701906 = EJPewLISGa70564493;     EJPewLISGa70564493 = EJPewLISGa73868492;     EJPewLISGa73868492 = EJPewLISGa12558809;     EJPewLISGa12558809 = EJPewLISGa51647141;     EJPewLISGa51647141 = EJPewLISGa62116876;     EJPewLISGa62116876 = EJPewLISGa84667464;     EJPewLISGa84667464 = EJPewLISGa99627666;     EJPewLISGa99627666 = EJPewLISGa23750496;     EJPewLISGa23750496 = EJPewLISGa50586920;     EJPewLISGa50586920 = EJPewLISGa22665033;     EJPewLISGa22665033 = EJPewLISGa90127575;     EJPewLISGa90127575 = EJPewLISGa58771654;     EJPewLISGa58771654 = EJPewLISGa97069823;     EJPewLISGa97069823 = EJPewLISGa65994774;     EJPewLISGa65994774 = EJPewLISGa57587011;     EJPewLISGa57587011 = EJPewLISGa68732124;     EJPewLISGa68732124 = EJPewLISGa88948229;     EJPewLISGa88948229 = EJPewLISGa47669506;     EJPewLISGa47669506 = EJPewLISGa67562307;     EJPewLISGa67562307 = EJPewLISGa90335776;     EJPewLISGa90335776 = EJPewLISGa4075641;     EJPewLISGa4075641 = EJPewLISGa93533230;     EJPewLISGa93533230 = EJPewLISGa43917590;     EJPewLISGa43917590 = EJPewLISGa57320625;     EJPewLISGa57320625 = EJPewLISGa18857696;     EJPewLISGa18857696 = EJPewLISGa33844814;     EJPewLISGa33844814 = EJPewLISGa55863539;     EJPewLISGa55863539 = EJPewLISGa61728632;     EJPewLISGa61728632 = EJPewLISGa19090829;     EJPewLISGa19090829 = EJPewLISGa43092026;     EJPewLISGa43092026 = EJPewLISGa96853149;     EJPewLISGa96853149 = EJPewLISGa56961823;     EJPewLISGa56961823 = EJPewLISGa29883407;     EJPewLISGa29883407 = EJPewLISGa89394432;     EJPewLISGa89394432 = EJPewLISGa92632075;     EJPewLISGa92632075 = EJPewLISGa71273419;     EJPewLISGa71273419 = EJPewLISGa76854135;     EJPewLISGa76854135 = EJPewLISGa21014084;     EJPewLISGa21014084 = EJPewLISGa71392872;     EJPewLISGa71392872 = EJPewLISGa28850321;     EJPewLISGa28850321 = EJPewLISGa24511555;     EJPewLISGa24511555 = EJPewLISGa37560195;     EJPewLISGa37560195 = EJPewLISGa90826082;     EJPewLISGa90826082 = EJPewLISGa18806396;     EJPewLISGa18806396 = EJPewLISGa68591280;     EJPewLISGa68591280 = EJPewLISGa90173499;     EJPewLISGa90173499 = EJPewLISGa4153711;     EJPewLISGa4153711 = EJPewLISGa15972979;     EJPewLISGa15972979 = EJPewLISGa55056622;     EJPewLISGa55056622 = EJPewLISGa97185224;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void GuxcOqEKon95462110() {     int yDowUKnRoS84106902 = -698662739;    int yDowUKnRoS32146495 = -554637034;    int yDowUKnRoS66695946 = 76312419;    int yDowUKnRoS79617412 = -979894447;    int yDowUKnRoS83897128 = -170212944;    int yDowUKnRoS26783166 = 72195627;    int yDowUKnRoS67052178 = -934454153;    int yDowUKnRoS23852689 = -101079884;    int yDowUKnRoS2551853 = -601890685;    int yDowUKnRoS2691729 = -354177309;    int yDowUKnRoS22409653 = -843096064;    int yDowUKnRoS27771545 = -805259704;    int yDowUKnRoS22235903 = -671473910;    int yDowUKnRoS24497364 = -745242067;    int yDowUKnRoS4792962 = -4321703;    int yDowUKnRoS49754156 = -694720867;    int yDowUKnRoS15226507 = -205096562;    int yDowUKnRoS64162990 = -357615523;    int yDowUKnRoS46548589 = -252091673;    int yDowUKnRoS72792309 = -631108157;    int yDowUKnRoS35268518 = -95229477;    int yDowUKnRoS73758308 = -917464020;    int yDowUKnRoS7477632 = -150130646;    int yDowUKnRoS50729216 = -95045798;    int yDowUKnRoS83968708 = -598435923;    int yDowUKnRoS76760021 = -939456905;    int yDowUKnRoS47542884 = -520432168;    int yDowUKnRoS27465485 = -97870631;    int yDowUKnRoS55267479 = -40009510;    int yDowUKnRoS6809978 = -685197277;    int yDowUKnRoS22316821 = -600902740;    int yDowUKnRoS49583941 = -844319572;    int yDowUKnRoS43686026 = -44895566;    int yDowUKnRoS2584323 = 37277340;    int yDowUKnRoS26288697 = -503959444;    int yDowUKnRoS46478000 = -162533085;    int yDowUKnRoS80108484 = -409948451;    int yDowUKnRoS33639068 = -279555359;    int yDowUKnRoS94644168 = -617734233;    int yDowUKnRoS69258504 = -666943331;    int yDowUKnRoS41880287 = -550313706;    int yDowUKnRoS33786079 = -902607813;    int yDowUKnRoS77673708 = -555704588;    int yDowUKnRoS512432 = -79524665;    int yDowUKnRoS64628494 = -981562807;    int yDowUKnRoS11479816 = -954839792;    int yDowUKnRoS60139122 = -622966796;    int yDowUKnRoS96055250 = -836863940;    int yDowUKnRoS60142740 = 66899372;    int yDowUKnRoS45080859 = -315078690;    int yDowUKnRoS79059768 = -669673919;    int yDowUKnRoS75477535 = -925174911;    int yDowUKnRoS59010095 = -846658160;    int yDowUKnRoS67103778 = -47379082;    int yDowUKnRoS60193593 = -894939920;    int yDowUKnRoS10348594 = -781198719;    int yDowUKnRoS24668863 = -304506388;    int yDowUKnRoS15966731 = -828641784;    int yDowUKnRoS95648704 = -281458524;    int yDowUKnRoS7137108 = -230756040;    int yDowUKnRoS79240282 = -407372206;    int yDowUKnRoS39586694 = -736583522;    int yDowUKnRoS68585209 = 38929625;    int yDowUKnRoS95741874 = -916693409;    int yDowUKnRoS80374908 = -753274570;    int yDowUKnRoS72825711 = -998776492;    int yDowUKnRoS84085519 = -660364138;    int yDowUKnRoS19651580 = -608751250;    int yDowUKnRoS98208666 = -141282623;    int yDowUKnRoS58314962 = -841788619;    int yDowUKnRoS69645672 = -184772416;    int yDowUKnRoS81587438 = -925541203;    int yDowUKnRoS69518822 = -739881291;    int yDowUKnRoS77290085 = -585148343;    int yDowUKnRoS30912022 = 19205549;    int yDowUKnRoS1482439 = -192621665;    int yDowUKnRoS96084599 = -261759433;    int yDowUKnRoS6965200 = 29394018;    int yDowUKnRoS86100722 = -113482991;    int yDowUKnRoS72488892 = -643596131;    int yDowUKnRoS16620899 = -216490109;    int yDowUKnRoS51487634 = -683568228;    int yDowUKnRoS67322745 = -64770004;    int yDowUKnRoS10186621 = -724930820;    int yDowUKnRoS27750209 = 84476641;    int yDowUKnRoS46839286 = -675727830;    int yDowUKnRoS90573845 = -997661413;    int yDowUKnRoS76582248 = -997516485;    int yDowUKnRoS42390730 = -67782741;    int yDowUKnRoS15940103 = -722760725;    int yDowUKnRoS21809137 = -858026697;    int yDowUKnRoS64141754 = -581306667;    int yDowUKnRoS37990363 = -998096835;    int yDowUKnRoS87507061 = -286978193;    int yDowUKnRoS90018222 = -159571126;    int yDowUKnRoS2293593 = -813730185;    int yDowUKnRoS65200870 = -841537439;    int yDowUKnRoS81931833 = -639011180;    int yDowUKnRoS20137524 = -326250096;    int yDowUKnRoS91802782 = -698662739;     yDowUKnRoS84106902 = yDowUKnRoS32146495;     yDowUKnRoS32146495 = yDowUKnRoS66695946;     yDowUKnRoS66695946 = yDowUKnRoS79617412;     yDowUKnRoS79617412 = yDowUKnRoS83897128;     yDowUKnRoS83897128 = yDowUKnRoS26783166;     yDowUKnRoS26783166 = yDowUKnRoS67052178;     yDowUKnRoS67052178 = yDowUKnRoS23852689;     yDowUKnRoS23852689 = yDowUKnRoS2551853;     yDowUKnRoS2551853 = yDowUKnRoS2691729;     yDowUKnRoS2691729 = yDowUKnRoS22409653;     yDowUKnRoS22409653 = yDowUKnRoS27771545;     yDowUKnRoS27771545 = yDowUKnRoS22235903;     yDowUKnRoS22235903 = yDowUKnRoS24497364;     yDowUKnRoS24497364 = yDowUKnRoS4792962;     yDowUKnRoS4792962 = yDowUKnRoS49754156;     yDowUKnRoS49754156 = yDowUKnRoS15226507;     yDowUKnRoS15226507 = yDowUKnRoS64162990;     yDowUKnRoS64162990 = yDowUKnRoS46548589;     yDowUKnRoS46548589 = yDowUKnRoS72792309;     yDowUKnRoS72792309 = yDowUKnRoS35268518;     yDowUKnRoS35268518 = yDowUKnRoS73758308;     yDowUKnRoS73758308 = yDowUKnRoS7477632;     yDowUKnRoS7477632 = yDowUKnRoS50729216;     yDowUKnRoS50729216 = yDowUKnRoS83968708;     yDowUKnRoS83968708 = yDowUKnRoS76760021;     yDowUKnRoS76760021 = yDowUKnRoS47542884;     yDowUKnRoS47542884 = yDowUKnRoS27465485;     yDowUKnRoS27465485 = yDowUKnRoS55267479;     yDowUKnRoS55267479 = yDowUKnRoS6809978;     yDowUKnRoS6809978 = yDowUKnRoS22316821;     yDowUKnRoS22316821 = yDowUKnRoS49583941;     yDowUKnRoS49583941 = yDowUKnRoS43686026;     yDowUKnRoS43686026 = yDowUKnRoS2584323;     yDowUKnRoS2584323 = yDowUKnRoS26288697;     yDowUKnRoS26288697 = yDowUKnRoS46478000;     yDowUKnRoS46478000 = yDowUKnRoS80108484;     yDowUKnRoS80108484 = yDowUKnRoS33639068;     yDowUKnRoS33639068 = yDowUKnRoS94644168;     yDowUKnRoS94644168 = yDowUKnRoS69258504;     yDowUKnRoS69258504 = yDowUKnRoS41880287;     yDowUKnRoS41880287 = yDowUKnRoS33786079;     yDowUKnRoS33786079 = yDowUKnRoS77673708;     yDowUKnRoS77673708 = yDowUKnRoS512432;     yDowUKnRoS512432 = yDowUKnRoS64628494;     yDowUKnRoS64628494 = yDowUKnRoS11479816;     yDowUKnRoS11479816 = yDowUKnRoS60139122;     yDowUKnRoS60139122 = yDowUKnRoS96055250;     yDowUKnRoS96055250 = yDowUKnRoS60142740;     yDowUKnRoS60142740 = yDowUKnRoS45080859;     yDowUKnRoS45080859 = yDowUKnRoS79059768;     yDowUKnRoS79059768 = yDowUKnRoS75477535;     yDowUKnRoS75477535 = yDowUKnRoS59010095;     yDowUKnRoS59010095 = yDowUKnRoS67103778;     yDowUKnRoS67103778 = yDowUKnRoS60193593;     yDowUKnRoS60193593 = yDowUKnRoS10348594;     yDowUKnRoS10348594 = yDowUKnRoS24668863;     yDowUKnRoS24668863 = yDowUKnRoS15966731;     yDowUKnRoS15966731 = yDowUKnRoS95648704;     yDowUKnRoS95648704 = yDowUKnRoS7137108;     yDowUKnRoS7137108 = yDowUKnRoS79240282;     yDowUKnRoS79240282 = yDowUKnRoS39586694;     yDowUKnRoS39586694 = yDowUKnRoS68585209;     yDowUKnRoS68585209 = yDowUKnRoS95741874;     yDowUKnRoS95741874 = yDowUKnRoS80374908;     yDowUKnRoS80374908 = yDowUKnRoS72825711;     yDowUKnRoS72825711 = yDowUKnRoS84085519;     yDowUKnRoS84085519 = yDowUKnRoS19651580;     yDowUKnRoS19651580 = yDowUKnRoS98208666;     yDowUKnRoS98208666 = yDowUKnRoS58314962;     yDowUKnRoS58314962 = yDowUKnRoS69645672;     yDowUKnRoS69645672 = yDowUKnRoS81587438;     yDowUKnRoS81587438 = yDowUKnRoS69518822;     yDowUKnRoS69518822 = yDowUKnRoS77290085;     yDowUKnRoS77290085 = yDowUKnRoS30912022;     yDowUKnRoS30912022 = yDowUKnRoS1482439;     yDowUKnRoS1482439 = yDowUKnRoS96084599;     yDowUKnRoS96084599 = yDowUKnRoS6965200;     yDowUKnRoS6965200 = yDowUKnRoS86100722;     yDowUKnRoS86100722 = yDowUKnRoS72488892;     yDowUKnRoS72488892 = yDowUKnRoS16620899;     yDowUKnRoS16620899 = yDowUKnRoS51487634;     yDowUKnRoS51487634 = yDowUKnRoS67322745;     yDowUKnRoS67322745 = yDowUKnRoS10186621;     yDowUKnRoS10186621 = yDowUKnRoS27750209;     yDowUKnRoS27750209 = yDowUKnRoS46839286;     yDowUKnRoS46839286 = yDowUKnRoS90573845;     yDowUKnRoS90573845 = yDowUKnRoS76582248;     yDowUKnRoS76582248 = yDowUKnRoS42390730;     yDowUKnRoS42390730 = yDowUKnRoS15940103;     yDowUKnRoS15940103 = yDowUKnRoS21809137;     yDowUKnRoS21809137 = yDowUKnRoS64141754;     yDowUKnRoS64141754 = yDowUKnRoS37990363;     yDowUKnRoS37990363 = yDowUKnRoS87507061;     yDowUKnRoS87507061 = yDowUKnRoS90018222;     yDowUKnRoS90018222 = yDowUKnRoS2293593;     yDowUKnRoS2293593 = yDowUKnRoS65200870;     yDowUKnRoS65200870 = yDowUKnRoS81931833;     yDowUKnRoS81931833 = yDowUKnRoS20137524;     yDowUKnRoS20137524 = yDowUKnRoS91802782;     yDowUKnRoS91802782 = yDowUKnRoS84106902;}
// Junk Finished
