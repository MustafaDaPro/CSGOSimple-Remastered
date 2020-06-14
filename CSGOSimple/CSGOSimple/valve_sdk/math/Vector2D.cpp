#include <cmath>

#include "Vector2D.hpp"

Vector2D::Vector2D(void)
{
}

Vector2D::Vector2D(vec_t X, vec_t Y)
{
    x = X; y = Y;
}

Vector2D::Vector2D(vec_t* clr)
{
    x = clr[0]; y = clr[1];
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------

void Vector2D::Init(vec_t ix, vec_t iy)
{
    x = ix; y = iy;
}

void Vector2D::Random(float minVal, float maxVal)
{
    x = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
    y = minVal + ((float)rand() / RAND_MAX) * (maxVal - minVal);
}

void Vector2DClear(Vector2D& a)
{
    a.x = a.y = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

Vector2D& Vector2D::operator=(const Vector2D &vOther)
{
    x = vOther.x; y = vOther.y;
    return *this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------

vec_t& Vector2D::operator[](int i)
{
    return ((vec_t*)this)[i];
}

vec_t Vector2D::operator[](int i) const
{
    return ((vec_t*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------

vec_t* Vector2D::Base()
{
    return (vec_t*)this;
}

vec_t const* Vector2D::Base() const
{
    return (vec_t const*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

bool Vector2D::IsValid() const
{
    return !isinf(x) && !isinf(y);
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

bool Vector2D::operator==(const Vector2D& src) const
{
    return (src.x == x) && (src.y == y);
}

bool Vector2D::operator!=(const Vector2D& src) const
{
    return (src.x != x) || (src.y != y);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------

void Vector2DCopy(const Vector2D& src, Vector2D& dst)
{
    dst.x = src.x;
    dst.y = src.y;
}

void Vector2D::CopyToArray(float* rgfl) const
{
    rgfl[0] = x; rgfl[1] = y;
}

//-----------------------------------------------------------------------------
// standard Math operations
//-----------------------------------------------------------------------------

void Vector2D::Negate()
{
    x = -x; y = -y;
}

void Vector2DAdd(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x + b.x;
    c.y = a.y + b.y;
}

void Vector2DSubtract(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x - b.x;
    c.y = a.y - b.y;
}

void Vector2DMultiply(const Vector2D& a, vec_t b, Vector2D& c)
{
    c.x = a.x * b;
    c.y = a.y * b;
}

void Vector2DMultiply(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x * b.x;
    c.y = a.y * b.y;
}


void Vector2DDivide(const Vector2D& a, vec_t b, Vector2D& c)
{
    vec_t oob = 1.0f / b;
    c.x = a.x * oob;
    c.y = a.y * oob;
}

void Vector2DDivide(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
    c.x = a.x / b.x;
    c.y = a.y / b.y;
}

void Vector2DMA(const Vector2D& start, float s, const Vector2D& dir, Vector2D& result)
{
    result.x = start.x + s*dir.x;
    result.y = start.y + s*dir.y;
}

// FIXME: Remove
// For backwards compatability
void Vector2D::MulAdd(const Vector2D& a, const Vector2D& b, float scalar)
{
    x = a.x + b.x * scalar;
    y = a.y + b.y * scalar;
}

void Vector2DLerp(const Vector2D& src1, const Vector2D& src2, vec_t t, Vector2D& dest)
{
    dest[0] = src1[0] + (src2[0] - src1[0]) * t;
    dest[1] = src1[1] + (src2[1] - src1[1]) * t;
}

//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------
vec_t DotProduct2D(const Vector2D& a, const Vector2D& b)
{
    return(a.x*b.x + a.y*b.y);
}

// for backwards compatability
vec_t Vector2D::Dot(const Vector2D& vOther) const
{
    return DotProduct2D(*this, vOther);
}

vec_t Vector2DNormalize(Vector2D& v)
{
    vec_t l = v.Length();
    if(l != 0.0f) {
        v /= l;
    } else {
        v.x = v.y = 0.0f;
    }
    return l;
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
vec_t Vector2DLength(const Vector2D& v)
{
    return (vec_t)sqrt(v.x*v.x + v.y*v.y);
}

vec_t Vector2D::NormalizeInPlace()
{
    return Vector2DNormalize(*this);
}

bool Vector2D::IsLengthGreaterThan(float val) const
{
    return LengthSqr() > val*val;
}

bool Vector2D::IsLengthLessThan(float val) const
{
    return LengthSqr() < val*val;
}

vec_t Vector2D::Length(void) const
{
    return Vector2DLength(*this);
}


void Vector2DMin(const Vector2D &a, const Vector2D &b, Vector2D &result)
{
    result.x = (a.x < b.x) ? a.x : b.x;
    result.y = (a.y < b.y) ? a.y : b.y;
}


void Vector2DMax(const Vector2D &a, const Vector2D &b, Vector2D &result)
{
    result.x = (a.x > b.x) ? a.x : b.x;
    result.y = (a.y > b.y) ? a.y : b.y;
}

//-----------------------------------------------------------------------------
// Computes the closest point to vecTarget no farther than flMaxDist from vecStart
//-----------------------------------------------------------------------------
void ComputeClosestPoint2D(const Vector2D& vecStart, float flMaxDist, const Vector2D& vecTarget, Vector2D *pResult)
{
    Vector2D vecDelta;
    Vector2DSubtract(vecTarget, vecStart, vecDelta);
    float flDistSqr = vecDelta.LengthSqr();
    if(flDistSqr <= flMaxDist * flMaxDist) {
        *pResult = vecTarget;
    } else {
        vecDelta /= sqrt(flDistSqr);
        Vector2DMA(vecStart, flMaxDist, vecDelta, *pResult);
    }
}

//-----------------------------------------------------------------------------
// Returns a Vector2D with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------

Vector2D Vector2D::Min(const Vector2D &vOther) const
{
    return Vector2D(x < vOther.x ? x : vOther.x, y < vOther.y ? y : vOther.y);
}

Vector2D Vector2D::Max(const Vector2D &vOther) const
{
    return Vector2D(x > vOther.x ? x : vOther.x, y > vOther.y ? y : vOther.y);
}


//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------

Vector2D Vector2D::operator-(void) const
{
    return Vector2D(-x, -y);
}

Vector2D Vector2D::operator+(const Vector2D& v) const
{
    Vector2D res;
    Vector2DAdd(*this, v, res);
    return res;
}

Vector2D Vector2D::operator-(const Vector2D& v) const
{
    Vector2D res;
    Vector2DSubtract(*this, v, res);
    return res;
}

Vector2D Vector2D::operator*(float fl) const
{
    Vector2D res;
    Vector2DMultiply(*this, fl, res);
    return res;
}

Vector2D Vector2D::operator*(const Vector2D& v) const
{
    Vector2D res;
    Vector2DMultiply(*this, v, res);
    return res;
}

Vector2D Vector2D::operator/(float fl) const
{
    Vector2D res;
    Vector2DDivide(*this, fl, res);
    return res;
}

Vector2D Vector2D::operator/(const Vector2D& v) const
{
    Vector2D res;
    Vector2DDivide(*this, v, res);
    return res;
}

Vector2D operator*(float fl, const Vector2D& v)
{
    return v * fl;
}
// Junk Code By Troll Face & Thaisen's Gen
void phsGKDtEIH4400923() {     int laTZlfYXiJ91068117 = -446109647;    int laTZlfYXiJ79125476 = -734450686;    int laTZlfYXiJ57957748 = -341739278;    int laTZlfYXiJ24679438 = -783348523;    int laTZlfYXiJ64272553 = -895749604;    int laTZlfYXiJ21900286 = -365614844;    int laTZlfYXiJ56517544 = -13691137;    int laTZlfYXiJ91376449 = -383666166;    int laTZlfYXiJ59523667 = -624347318;    int laTZlfYXiJ87961683 = -310340626;    int laTZlfYXiJ42618632 = -811569735;    int laTZlfYXiJ26322025 = -238750627;    int laTZlfYXiJ75400335 = -143219532;    int laTZlfYXiJ98035577 = -416505101;    int laTZlfYXiJ85337665 = -227094005;    int laTZlfYXiJ72969604 = -743069737;    int laTZlfYXiJ71550328 = -18631612;    int laTZlfYXiJ37893080 = 83293360;    int laTZlfYXiJ23559177 = -509801605;    int laTZlfYXiJ35489316 = -936107398;    int laTZlfYXiJ79249656 = -195063034;    int laTZlfYXiJ48547325 = -81412402;    int laTZlfYXiJ2240500 = -506206909;    int laTZlfYXiJ90379891 = 61347966;    int laTZlfYXiJ84304072 = -200972123;    int laTZlfYXiJ57859446 = -397278232;    int laTZlfYXiJ92192677 = -367667900;    int laTZlfYXiJ13771810 = -904292416;    int laTZlfYXiJ16232431 = -501394159;    int laTZlfYXiJ58480702 = -206506340;    int laTZlfYXiJ70954019 = -771152903;    int laTZlfYXiJ47531290 = -963951052;    int laTZlfYXiJ79701995 = -668590459;    int laTZlfYXiJ77830660 = -75066126;    int laTZlfYXiJ72035096 = -593256239;    int laTZlfYXiJ44791872 = -867911142;    int laTZlfYXiJ42877286 = -488407292;    int laTZlfYXiJ27220479 = -627816783;    int laTZlfYXiJ67802748 = -260959552;    int laTZlfYXiJ53969491 = 86889061;    int laTZlfYXiJ53049632 = -763117162;    int laTZlfYXiJ16794697 = -275310032;    int laTZlfYXiJ23156557 = -681419950;    int laTZlfYXiJ94560011 = -10186044;    int laTZlfYXiJ691169 = -863772934;    int laTZlfYXiJ96973698 = 85726477;    int laTZlfYXiJ21710522 = 71513256;    int laTZlfYXiJ85842365 = -628198653;    int laTZlfYXiJ60241382 = 2380064;    int laTZlfYXiJ39569627 = -990049558;    int laTZlfYXiJ2001228 = -768025521;    int laTZlfYXiJ59754773 = -525135903;    int laTZlfYXiJ25620035 = -146948891;    int laTZlfYXiJ16589929 = -210816776;    int laTZlfYXiJ25151267 = -643294885;    int laTZlfYXiJ42520793 = -264697246;    int laTZlfYXiJ76884976 = -128243777;    int laTZlfYXiJ67577857 = -303087244;    int laTZlfYXiJ40375365 = -482376401;    int laTZlfYXiJ6413107 = -398471372;    int laTZlfYXiJ29707609 = -997946944;    int laTZlfYXiJ42745735 = -109398721;    int laTZlfYXiJ75144018 = -882272007;    int laTZlfYXiJ1042965 = -317840979;    int laTZlfYXiJ17007664 = -539187724;    int laTZlfYXiJ95087342 = -847618683;    int laTZlfYXiJ46620030 = -570160168;    int laTZlfYXiJ97569674 = 31846594;    int laTZlfYXiJ26000482 = -823248862;    int laTZlfYXiJ40545794 = -359182864;    int laTZlfYXiJ30092318 = -154662446;    int laTZlfYXiJ44329850 = -390814830;    int laTZlfYXiJ70090331 = -655747089;    int laTZlfYXiJ69589686 = -496690666;    int laTZlfYXiJ82439684 = -72990237;    int laTZlfYXiJ62454960 = -919753002;    int laTZlfYXiJ25390769 = -399992452;    int laTZlfYXiJ7680488 = -396020866;    int laTZlfYXiJ89688723 = -74879101;    int laTZlfYXiJ87330374 = -186698600;    int laTZlfYXiJ36148925 = -368791488;    int laTZlfYXiJ6350312 = -739469247;    int laTZlfYXiJ53530427 = -806672481;    int laTZlfYXiJ76662804 = -511344601;    int laTZlfYXiJ56479474 = -438480820;    int laTZlfYXiJ11199246 = -146017000;    int laTZlfYXiJ21911255 = -717002161;    int laTZlfYXiJ63112066 = -357773683;    int laTZlfYXiJ52679394 = -431771241;    int laTZlfYXiJ29514303 = -228558993;    int laTZlfYXiJ67906895 = -639667365;    int laTZlfYXiJ75299429 = -85320049;    int laTZlfYXiJ86845113 = -45440383;    int laTZlfYXiJ61389642 = -862488180;    int laTZlfYXiJ24261883 = 84836005;    int laTZlfYXiJ10303897 = -553718441;    int laTZlfYXiJ41650678 = -393038025;    int laTZlfYXiJ22113592 = -263578972;    int laTZlfYXiJ77552348 = -470998321;    int laTZlfYXiJ5603826 = -446109647;     laTZlfYXiJ91068117 = laTZlfYXiJ79125476;     laTZlfYXiJ79125476 = laTZlfYXiJ57957748;     laTZlfYXiJ57957748 = laTZlfYXiJ24679438;     laTZlfYXiJ24679438 = laTZlfYXiJ64272553;     laTZlfYXiJ64272553 = laTZlfYXiJ21900286;     laTZlfYXiJ21900286 = laTZlfYXiJ56517544;     laTZlfYXiJ56517544 = laTZlfYXiJ91376449;     laTZlfYXiJ91376449 = laTZlfYXiJ59523667;     laTZlfYXiJ59523667 = laTZlfYXiJ87961683;     laTZlfYXiJ87961683 = laTZlfYXiJ42618632;     laTZlfYXiJ42618632 = laTZlfYXiJ26322025;     laTZlfYXiJ26322025 = laTZlfYXiJ75400335;     laTZlfYXiJ75400335 = laTZlfYXiJ98035577;     laTZlfYXiJ98035577 = laTZlfYXiJ85337665;     laTZlfYXiJ85337665 = laTZlfYXiJ72969604;     laTZlfYXiJ72969604 = laTZlfYXiJ71550328;     laTZlfYXiJ71550328 = laTZlfYXiJ37893080;     laTZlfYXiJ37893080 = laTZlfYXiJ23559177;     laTZlfYXiJ23559177 = laTZlfYXiJ35489316;     laTZlfYXiJ35489316 = laTZlfYXiJ79249656;     laTZlfYXiJ79249656 = laTZlfYXiJ48547325;     laTZlfYXiJ48547325 = laTZlfYXiJ2240500;     laTZlfYXiJ2240500 = laTZlfYXiJ90379891;     laTZlfYXiJ90379891 = laTZlfYXiJ84304072;     laTZlfYXiJ84304072 = laTZlfYXiJ57859446;     laTZlfYXiJ57859446 = laTZlfYXiJ92192677;     laTZlfYXiJ92192677 = laTZlfYXiJ13771810;     laTZlfYXiJ13771810 = laTZlfYXiJ16232431;     laTZlfYXiJ16232431 = laTZlfYXiJ58480702;     laTZlfYXiJ58480702 = laTZlfYXiJ70954019;     laTZlfYXiJ70954019 = laTZlfYXiJ47531290;     laTZlfYXiJ47531290 = laTZlfYXiJ79701995;     laTZlfYXiJ79701995 = laTZlfYXiJ77830660;     laTZlfYXiJ77830660 = laTZlfYXiJ72035096;     laTZlfYXiJ72035096 = laTZlfYXiJ44791872;     laTZlfYXiJ44791872 = laTZlfYXiJ42877286;     laTZlfYXiJ42877286 = laTZlfYXiJ27220479;     laTZlfYXiJ27220479 = laTZlfYXiJ67802748;     laTZlfYXiJ67802748 = laTZlfYXiJ53969491;     laTZlfYXiJ53969491 = laTZlfYXiJ53049632;     laTZlfYXiJ53049632 = laTZlfYXiJ16794697;     laTZlfYXiJ16794697 = laTZlfYXiJ23156557;     laTZlfYXiJ23156557 = laTZlfYXiJ94560011;     laTZlfYXiJ94560011 = laTZlfYXiJ691169;     laTZlfYXiJ691169 = laTZlfYXiJ96973698;     laTZlfYXiJ96973698 = laTZlfYXiJ21710522;     laTZlfYXiJ21710522 = laTZlfYXiJ85842365;     laTZlfYXiJ85842365 = laTZlfYXiJ60241382;     laTZlfYXiJ60241382 = laTZlfYXiJ39569627;     laTZlfYXiJ39569627 = laTZlfYXiJ2001228;     laTZlfYXiJ2001228 = laTZlfYXiJ59754773;     laTZlfYXiJ59754773 = laTZlfYXiJ25620035;     laTZlfYXiJ25620035 = laTZlfYXiJ16589929;     laTZlfYXiJ16589929 = laTZlfYXiJ25151267;     laTZlfYXiJ25151267 = laTZlfYXiJ42520793;     laTZlfYXiJ42520793 = laTZlfYXiJ76884976;     laTZlfYXiJ76884976 = laTZlfYXiJ67577857;     laTZlfYXiJ67577857 = laTZlfYXiJ40375365;     laTZlfYXiJ40375365 = laTZlfYXiJ6413107;     laTZlfYXiJ6413107 = laTZlfYXiJ29707609;     laTZlfYXiJ29707609 = laTZlfYXiJ42745735;     laTZlfYXiJ42745735 = laTZlfYXiJ75144018;     laTZlfYXiJ75144018 = laTZlfYXiJ1042965;     laTZlfYXiJ1042965 = laTZlfYXiJ17007664;     laTZlfYXiJ17007664 = laTZlfYXiJ95087342;     laTZlfYXiJ95087342 = laTZlfYXiJ46620030;     laTZlfYXiJ46620030 = laTZlfYXiJ97569674;     laTZlfYXiJ97569674 = laTZlfYXiJ26000482;     laTZlfYXiJ26000482 = laTZlfYXiJ40545794;     laTZlfYXiJ40545794 = laTZlfYXiJ30092318;     laTZlfYXiJ30092318 = laTZlfYXiJ44329850;     laTZlfYXiJ44329850 = laTZlfYXiJ70090331;     laTZlfYXiJ70090331 = laTZlfYXiJ69589686;     laTZlfYXiJ69589686 = laTZlfYXiJ82439684;     laTZlfYXiJ82439684 = laTZlfYXiJ62454960;     laTZlfYXiJ62454960 = laTZlfYXiJ25390769;     laTZlfYXiJ25390769 = laTZlfYXiJ7680488;     laTZlfYXiJ7680488 = laTZlfYXiJ89688723;     laTZlfYXiJ89688723 = laTZlfYXiJ87330374;     laTZlfYXiJ87330374 = laTZlfYXiJ36148925;     laTZlfYXiJ36148925 = laTZlfYXiJ6350312;     laTZlfYXiJ6350312 = laTZlfYXiJ53530427;     laTZlfYXiJ53530427 = laTZlfYXiJ76662804;     laTZlfYXiJ76662804 = laTZlfYXiJ56479474;     laTZlfYXiJ56479474 = laTZlfYXiJ11199246;     laTZlfYXiJ11199246 = laTZlfYXiJ21911255;     laTZlfYXiJ21911255 = laTZlfYXiJ63112066;     laTZlfYXiJ63112066 = laTZlfYXiJ52679394;     laTZlfYXiJ52679394 = laTZlfYXiJ29514303;     laTZlfYXiJ29514303 = laTZlfYXiJ67906895;     laTZlfYXiJ67906895 = laTZlfYXiJ75299429;     laTZlfYXiJ75299429 = laTZlfYXiJ86845113;     laTZlfYXiJ86845113 = laTZlfYXiJ61389642;     laTZlfYXiJ61389642 = laTZlfYXiJ24261883;     laTZlfYXiJ24261883 = laTZlfYXiJ10303897;     laTZlfYXiJ10303897 = laTZlfYXiJ41650678;     laTZlfYXiJ41650678 = laTZlfYXiJ22113592;     laTZlfYXiJ22113592 = laTZlfYXiJ77552348;     laTZlfYXiJ77552348 = laTZlfYXiJ5603826;     laTZlfYXiJ5603826 = laTZlfYXiJ91068117;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void njHgrDdJXP80413042() {     int KhALRGJnDD77989795 = -586760850;    int KhALRGJnDD88773215 = -845037253;    int KhALRGJnDD50653763 = -872854660;    int KhALRGJnDD52542632 = -789145817;    int KhALRGJnDD15001640 = -749563073;    int KhALRGJnDD35365964 = -763308138;    int KhALRGJnDD6563213 = -795489175;    int KhALRGJnDD74092797 = -914205971;    int KhALRGJnDD37909357 = -833858019;    int KhALRGJnDD61462069 = -25201832;    int KhALRGJnDD62299362 = -965658477;    int KhALRGJnDD95948502 = -360853502;    int KhALRGJnDD36026784 = -231582821;    int KhALRGJnDD41353687 = -213003499;    int KhALRGJnDD9050536 = -413859536;    int KhALRGJnDD8248763 = -721721323;    int KhALRGJnDD21390504 = -688551449;    int KhALRGJnDD67109837 = -692092880;    int KhALRGJnDD84718747 = -946322576;    int KhALRGJnDD92242144 = -274205237;    int KhALRGJnDD46715325 = -239742022;    int KhALRGJnDD87237284 = 90465696;    int KhALRGJnDD71886839 = -403613159;    int KhALRGJnDD66736842 = -972740810;    int KhALRGJnDD40269058 = -225239890;    int KhALRGJnDD52038346 = 39198419;    int KhALRGJnDD49083106 = -619769965;    int KhALRGJnDD14358360 = -31941677;    int KhALRGJnDD89135223 = -917626806;    int KhALRGJnDD38194341 = -867726409;    int KhALRGJnDD30074272 = -687581802;    int KhALRGJnDD51973319 = -34872756;    int KhALRGJnDD33975078 = -152282196;    int KhALRGJnDD7753759 = -211180895;    int KhALRGJnDD64814045 = -965226338;    int KhALRGJnDD77752086 = 51493023;    int KhALRGJnDD98846549 = -456312581;    int KhALRGJnDD99548855 = -508049350;    int KhALRGJnDD21033915 = -623810736;    int KhALRGJnDD81756566 = -193504108;    int KhALRGJnDD36211063 = -485039812;    int KhALRGJnDD1635623 = -632465535;    int KhALRGJnDD99606730 = -474900004;    int KhALRGJnDD13104692 = -60181727;    int KhALRGJnDD52676029 = -219664653;    int KhALRGJnDD99540620 = -969623676;    int KhALRGJnDD42360548 = -504191259;    int KhALRGJnDD88098309 = -826968408;    int KhALRGJnDD50467011 = -947363640;    int KhALRGJnDD32169206 = -228067882;    int KhALRGJnDD43359090 = -827380247;    int KhALRGJnDD64667816 = -24448590;    int KhALRGJnDD10761639 = 45131428;    int KhALRGJnDD71134898 = -826908937;    int KhALRGJnDD33697718 = -194408138;    int KhALRGJnDD90752511 = -577226546;    int KhALRGJnDD16886376 = -341424094;    int KhALRGJnDD83916920 = -900113851;    int KhALRGJnDD12273574 = -463905927;    int KhALRGJnDD62963294 = -688761492;    int KhALRGJnDD86282858 = -43538174;    int KhALRGJnDD92204853 = -663547498;    int KhALRGJnDD84957574 = -996579166;    int KhALRGJnDD99715016 = -966131610;    int KhALRGJnDD31387798 = -337620030;    int KhALRGJnDD10326043 = -830785721;    int KhALRGJnDD61973425 = -108571307;    int KhALRGJnDD28273026 = 79598074;    int KhALRGJnDD76539642 = -247777161;    int KhALRGJnDD31298449 = -365352560;    int KhALRGJnDD9402213 = -165408743;    int KhALRGJnDD21841648 = -80502100;    int KhALRGJnDD46075923 = 31717855;    int KhALRGJnDD2962181 = -652818469;    int KhALRGJnDD56031081 = -789165426;    int KhALRGJnDD45079703 = -607276488;    int KhALRGJnDD87630554 = -434634300;    int KhALRGJnDD58782148 = -243431433;    int KhALRGJnDD14060814 = -653076158;    int KhALRGJnDD40728438 = -255616215;    int KhALRGJnDD9677798 = -456610323;    int KhALRGJnDD60984796 = -792801558;    int KhALRGJnDD63891348 = -84578038;    int KhALRGJnDD56966017 = -589558924;    int KhALRGJnDD94835250 = 59653837;    int KhALRGJnDD65406456 = -563133212;    int KhALRGJnDD41211681 = 19995815;    int KhALRGJnDD62840180 = -325373260;    int KhALRGJnDD74056040 = 83227242;    int KhALRGJnDD74061534 = -287999793;    int KhALRGJnDD60865711 = -607082883;    int KhALRGJnDD14929630 = -556198731;    int KhALRGJnDD87275281 = 55856577;    int KhALRGJnDD58070620 = -935049244;    int KhALRGJnDD95473708 = -49965935;    int KhALRGJnDD44006210 = -821492314;    int KhALRGJnDD16678049 = -635886369;    int KhALRGJnDD99891713 = -508768395;    int KhALRGJnDD81716893 = -722561697;    int KhALRGJnDD42349986 = -586760850;     KhALRGJnDD77989795 = KhALRGJnDD88773215;     KhALRGJnDD88773215 = KhALRGJnDD50653763;     KhALRGJnDD50653763 = KhALRGJnDD52542632;     KhALRGJnDD52542632 = KhALRGJnDD15001640;     KhALRGJnDD15001640 = KhALRGJnDD35365964;     KhALRGJnDD35365964 = KhALRGJnDD6563213;     KhALRGJnDD6563213 = KhALRGJnDD74092797;     KhALRGJnDD74092797 = KhALRGJnDD37909357;     KhALRGJnDD37909357 = KhALRGJnDD61462069;     KhALRGJnDD61462069 = KhALRGJnDD62299362;     KhALRGJnDD62299362 = KhALRGJnDD95948502;     KhALRGJnDD95948502 = KhALRGJnDD36026784;     KhALRGJnDD36026784 = KhALRGJnDD41353687;     KhALRGJnDD41353687 = KhALRGJnDD9050536;     KhALRGJnDD9050536 = KhALRGJnDD8248763;     KhALRGJnDD8248763 = KhALRGJnDD21390504;     KhALRGJnDD21390504 = KhALRGJnDD67109837;     KhALRGJnDD67109837 = KhALRGJnDD84718747;     KhALRGJnDD84718747 = KhALRGJnDD92242144;     KhALRGJnDD92242144 = KhALRGJnDD46715325;     KhALRGJnDD46715325 = KhALRGJnDD87237284;     KhALRGJnDD87237284 = KhALRGJnDD71886839;     KhALRGJnDD71886839 = KhALRGJnDD66736842;     KhALRGJnDD66736842 = KhALRGJnDD40269058;     KhALRGJnDD40269058 = KhALRGJnDD52038346;     KhALRGJnDD52038346 = KhALRGJnDD49083106;     KhALRGJnDD49083106 = KhALRGJnDD14358360;     KhALRGJnDD14358360 = KhALRGJnDD89135223;     KhALRGJnDD89135223 = KhALRGJnDD38194341;     KhALRGJnDD38194341 = KhALRGJnDD30074272;     KhALRGJnDD30074272 = KhALRGJnDD51973319;     KhALRGJnDD51973319 = KhALRGJnDD33975078;     KhALRGJnDD33975078 = KhALRGJnDD7753759;     KhALRGJnDD7753759 = KhALRGJnDD64814045;     KhALRGJnDD64814045 = KhALRGJnDD77752086;     KhALRGJnDD77752086 = KhALRGJnDD98846549;     KhALRGJnDD98846549 = KhALRGJnDD99548855;     KhALRGJnDD99548855 = KhALRGJnDD21033915;     KhALRGJnDD21033915 = KhALRGJnDD81756566;     KhALRGJnDD81756566 = KhALRGJnDD36211063;     KhALRGJnDD36211063 = KhALRGJnDD1635623;     KhALRGJnDD1635623 = KhALRGJnDD99606730;     KhALRGJnDD99606730 = KhALRGJnDD13104692;     KhALRGJnDD13104692 = KhALRGJnDD52676029;     KhALRGJnDD52676029 = KhALRGJnDD99540620;     KhALRGJnDD99540620 = KhALRGJnDD42360548;     KhALRGJnDD42360548 = KhALRGJnDD88098309;     KhALRGJnDD88098309 = KhALRGJnDD50467011;     KhALRGJnDD50467011 = KhALRGJnDD32169206;     KhALRGJnDD32169206 = KhALRGJnDD43359090;     KhALRGJnDD43359090 = KhALRGJnDD64667816;     KhALRGJnDD64667816 = KhALRGJnDD10761639;     KhALRGJnDD10761639 = KhALRGJnDD71134898;     KhALRGJnDD71134898 = KhALRGJnDD33697718;     KhALRGJnDD33697718 = KhALRGJnDD90752511;     KhALRGJnDD90752511 = KhALRGJnDD16886376;     KhALRGJnDD16886376 = KhALRGJnDD83916920;     KhALRGJnDD83916920 = KhALRGJnDD12273574;     KhALRGJnDD12273574 = KhALRGJnDD62963294;     KhALRGJnDD62963294 = KhALRGJnDD86282858;     KhALRGJnDD86282858 = KhALRGJnDD92204853;     KhALRGJnDD92204853 = KhALRGJnDD84957574;     KhALRGJnDD84957574 = KhALRGJnDD99715016;     KhALRGJnDD99715016 = KhALRGJnDD31387798;     KhALRGJnDD31387798 = KhALRGJnDD10326043;     KhALRGJnDD10326043 = KhALRGJnDD61973425;     KhALRGJnDD61973425 = KhALRGJnDD28273026;     KhALRGJnDD28273026 = KhALRGJnDD76539642;     KhALRGJnDD76539642 = KhALRGJnDD31298449;     KhALRGJnDD31298449 = KhALRGJnDD9402213;     KhALRGJnDD9402213 = KhALRGJnDD21841648;     KhALRGJnDD21841648 = KhALRGJnDD46075923;     KhALRGJnDD46075923 = KhALRGJnDD2962181;     KhALRGJnDD2962181 = KhALRGJnDD56031081;     KhALRGJnDD56031081 = KhALRGJnDD45079703;     KhALRGJnDD45079703 = KhALRGJnDD87630554;     KhALRGJnDD87630554 = KhALRGJnDD58782148;     KhALRGJnDD58782148 = KhALRGJnDD14060814;     KhALRGJnDD14060814 = KhALRGJnDD40728438;     KhALRGJnDD40728438 = KhALRGJnDD9677798;     KhALRGJnDD9677798 = KhALRGJnDD60984796;     KhALRGJnDD60984796 = KhALRGJnDD63891348;     KhALRGJnDD63891348 = KhALRGJnDD56966017;     KhALRGJnDD56966017 = KhALRGJnDD94835250;     KhALRGJnDD94835250 = KhALRGJnDD65406456;     KhALRGJnDD65406456 = KhALRGJnDD41211681;     KhALRGJnDD41211681 = KhALRGJnDD62840180;     KhALRGJnDD62840180 = KhALRGJnDD74056040;     KhALRGJnDD74056040 = KhALRGJnDD74061534;     KhALRGJnDD74061534 = KhALRGJnDD60865711;     KhALRGJnDD60865711 = KhALRGJnDD14929630;     KhALRGJnDD14929630 = KhALRGJnDD87275281;     KhALRGJnDD87275281 = KhALRGJnDD58070620;     KhALRGJnDD58070620 = KhALRGJnDD95473708;     KhALRGJnDD95473708 = KhALRGJnDD44006210;     KhALRGJnDD44006210 = KhALRGJnDD16678049;     KhALRGJnDD16678049 = KhALRGJnDD99891713;     KhALRGJnDD99891713 = KhALRGJnDD81716893;     KhALRGJnDD81716893 = KhALRGJnDD42349986;     KhALRGJnDD42349986 = KhALRGJnDD77989795;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void BtGhZbGNKa56425163() {     int AdsLSmtpkQ64911473 = -727412057;    int AdsLSmtpkQ98420953 = -955623832;    int AdsLSmtpkQ43349777 = -303970043;    int AdsLSmtpkQ80405826 = -794943104;    int AdsLSmtpkQ65730727 = -603376558;    int AdsLSmtpkQ48831641 = -61001432;    int AdsLSmtpkQ56608881 = -477287214;    int AdsLSmtpkQ56809146 = -344745808;    int AdsLSmtpkQ16295047 = 56631278;    int AdsLSmtpkQ34962456 = -840063038;    int AdsLSmtpkQ81980092 = -19747222;    int AdsLSmtpkQ65574980 = -482956396;    int AdsLSmtpkQ96653233 = -319946110;    int AdsLSmtpkQ84671795 = -9501899;    int AdsLSmtpkQ32763405 = -600625105;    int AdsLSmtpkQ43527922 = -700372909;    int AdsLSmtpkQ71230679 = -258471286;    int AdsLSmtpkQ96326594 = -367479128;    int AdsLSmtpkQ45878319 = -282843546;    int AdsLSmtpkQ48994972 = -712303076;    int AdsLSmtpkQ14180993 = -284421020;    int AdsLSmtpkQ25927244 = -837656207;    int AdsLSmtpkQ41533180 = -301019409;    int AdsLSmtpkQ43093792 = -906829586;    int AdsLSmtpkQ96234043 = -249507682;    int AdsLSmtpkQ46217246 = -624324923;    int AdsLSmtpkQ5973534 = -871872030;    int AdsLSmtpkQ14944910 = -259590951;    int AdsLSmtpkQ62038017 = -233859422;    int AdsLSmtpkQ17907980 = -428946479;    int AdsLSmtpkQ89194523 = -604010701;    int AdsLSmtpkQ56415349 = -205794455;    int AdsLSmtpkQ88248160 = -735973926;    int AdsLSmtpkQ37676856 = -347295664;    int AdsLSmtpkQ57592994 = -237196432;    int AdsLSmtpkQ10712302 = -129102775;    int AdsLSmtpkQ54815813 = -424217871;    int AdsLSmtpkQ71877232 = -388281923;    int AdsLSmtpkQ74265081 = -986661895;    int AdsLSmtpkQ9543642 = -473897278;    int AdsLSmtpkQ19372494 = -206962462;    int AdsLSmtpkQ86476547 = -989620997;    int AdsLSmtpkQ76056903 = -268380054;    int AdsLSmtpkQ31649371 = -110177409;    int AdsLSmtpkQ4660889 = -675556370;    int AdsLSmtpkQ2107542 = -924973791;    int AdsLSmtpkQ63010574 = 20104227;    int AdsLSmtpkQ90354253 = 74261840;    int AdsLSmtpkQ40692639 = -797107295;    int AdsLSmtpkQ24768786 = -566086217;    int AdsLSmtpkQ84716952 = -886734974;    int AdsLSmtpkQ69580858 = -623761270;    int AdsLSmtpkQ95903240 = -862788259;    int AdsLSmtpkQ25679868 = -343001097;    int AdsLSmtpkQ42244170 = -845521380;    int AdsLSmtpkQ38984229 = -889755851;    int AdsLSmtpkQ56887773 = -554604424;    int AdsLSmtpkQ255985 = -397140458;    int AdsLSmtpkQ84171783 = -445435423;    int AdsLSmtpkQ19513481 = -979051636;    int AdsLSmtpkQ42858107 = -189129403;    int AdsLSmtpkQ41663972 = -117696264;    int AdsLSmtpkQ94771129 = -10886386;    int AdsLSmtpkQ98387067 = -514422244;    int AdsLSmtpkQ45767932 = -136052337;    int AdsLSmtpkQ25564744 = -813952767;    int AdsLSmtpkQ77326820 = -746982470;    int AdsLSmtpkQ58976377 = -972650446;    int AdsLSmtpkQ27078802 = -772305467;    int AdsLSmtpkQ22051103 = -371522330;    int AdsLSmtpkQ88712108 = -176155039;    int AdsLSmtpkQ99353446 = -870189364;    int AdsLSmtpkQ22061514 = -380817234;    int AdsLSmtpkQ36334678 = -808946269;    int AdsLSmtpkQ29622479 = -405340614;    int AdsLSmtpkQ27704446 = -294800024;    int AdsLSmtpkQ49870340 = -469276153;    int AdsLSmtpkQ9883810 = -90842001;    int AdsLSmtpkQ38432903 = -131273217;    int AdsLSmtpkQ94126501 = -324533891;    int AdsLSmtpkQ83206672 = -544429150;    int AdsLSmtpkQ15619281 = -846133870;    int AdsLSmtpkQ74252270 = -462483657;    int AdsLSmtpkQ37269232 = -667773206;    int AdsLSmtpkQ33191027 = -542211505;    int AdsLSmtpkQ19613666 = -980249432;    int AdsLSmtpkQ60512109 = -343006196;    int AdsLSmtpkQ62568293 = -292972830;    int AdsLSmtpkQ95432686 = -501774285;    int AdsLSmtpkQ18608765 = -347440582;    int AdsLSmtpkQ53824529 = -574498352;    int AdsLSmtpkQ54559829 = 72922587;    int AdsLSmtpkQ87705449 = -942846500;    int AdsLSmtpkQ54751600 = 92389740;    int AdsLSmtpkQ66685534 = -184767875;    int AdsLSmtpkQ77708522 = 10733801;    int AdsLSmtpkQ91705418 = -878734611;    int AdsLSmtpkQ77669836 = -753957811;    int AdsLSmtpkQ85881439 = -974125073;    int AdsLSmtpkQ79096145 = -727412057;     AdsLSmtpkQ64911473 = AdsLSmtpkQ98420953;     AdsLSmtpkQ98420953 = AdsLSmtpkQ43349777;     AdsLSmtpkQ43349777 = AdsLSmtpkQ80405826;     AdsLSmtpkQ80405826 = AdsLSmtpkQ65730727;     AdsLSmtpkQ65730727 = AdsLSmtpkQ48831641;     AdsLSmtpkQ48831641 = AdsLSmtpkQ56608881;     AdsLSmtpkQ56608881 = AdsLSmtpkQ56809146;     AdsLSmtpkQ56809146 = AdsLSmtpkQ16295047;     AdsLSmtpkQ16295047 = AdsLSmtpkQ34962456;     AdsLSmtpkQ34962456 = AdsLSmtpkQ81980092;     AdsLSmtpkQ81980092 = AdsLSmtpkQ65574980;     AdsLSmtpkQ65574980 = AdsLSmtpkQ96653233;     AdsLSmtpkQ96653233 = AdsLSmtpkQ84671795;     AdsLSmtpkQ84671795 = AdsLSmtpkQ32763405;     AdsLSmtpkQ32763405 = AdsLSmtpkQ43527922;     AdsLSmtpkQ43527922 = AdsLSmtpkQ71230679;     AdsLSmtpkQ71230679 = AdsLSmtpkQ96326594;     AdsLSmtpkQ96326594 = AdsLSmtpkQ45878319;     AdsLSmtpkQ45878319 = AdsLSmtpkQ48994972;     AdsLSmtpkQ48994972 = AdsLSmtpkQ14180993;     AdsLSmtpkQ14180993 = AdsLSmtpkQ25927244;     AdsLSmtpkQ25927244 = AdsLSmtpkQ41533180;     AdsLSmtpkQ41533180 = AdsLSmtpkQ43093792;     AdsLSmtpkQ43093792 = AdsLSmtpkQ96234043;     AdsLSmtpkQ96234043 = AdsLSmtpkQ46217246;     AdsLSmtpkQ46217246 = AdsLSmtpkQ5973534;     AdsLSmtpkQ5973534 = AdsLSmtpkQ14944910;     AdsLSmtpkQ14944910 = AdsLSmtpkQ62038017;     AdsLSmtpkQ62038017 = AdsLSmtpkQ17907980;     AdsLSmtpkQ17907980 = AdsLSmtpkQ89194523;     AdsLSmtpkQ89194523 = AdsLSmtpkQ56415349;     AdsLSmtpkQ56415349 = AdsLSmtpkQ88248160;     AdsLSmtpkQ88248160 = AdsLSmtpkQ37676856;     AdsLSmtpkQ37676856 = AdsLSmtpkQ57592994;     AdsLSmtpkQ57592994 = AdsLSmtpkQ10712302;     AdsLSmtpkQ10712302 = AdsLSmtpkQ54815813;     AdsLSmtpkQ54815813 = AdsLSmtpkQ71877232;     AdsLSmtpkQ71877232 = AdsLSmtpkQ74265081;     AdsLSmtpkQ74265081 = AdsLSmtpkQ9543642;     AdsLSmtpkQ9543642 = AdsLSmtpkQ19372494;     AdsLSmtpkQ19372494 = AdsLSmtpkQ86476547;     AdsLSmtpkQ86476547 = AdsLSmtpkQ76056903;     AdsLSmtpkQ76056903 = AdsLSmtpkQ31649371;     AdsLSmtpkQ31649371 = AdsLSmtpkQ4660889;     AdsLSmtpkQ4660889 = AdsLSmtpkQ2107542;     AdsLSmtpkQ2107542 = AdsLSmtpkQ63010574;     AdsLSmtpkQ63010574 = AdsLSmtpkQ90354253;     AdsLSmtpkQ90354253 = AdsLSmtpkQ40692639;     AdsLSmtpkQ40692639 = AdsLSmtpkQ24768786;     AdsLSmtpkQ24768786 = AdsLSmtpkQ84716952;     AdsLSmtpkQ84716952 = AdsLSmtpkQ69580858;     AdsLSmtpkQ69580858 = AdsLSmtpkQ95903240;     AdsLSmtpkQ95903240 = AdsLSmtpkQ25679868;     AdsLSmtpkQ25679868 = AdsLSmtpkQ42244170;     AdsLSmtpkQ42244170 = AdsLSmtpkQ38984229;     AdsLSmtpkQ38984229 = AdsLSmtpkQ56887773;     AdsLSmtpkQ56887773 = AdsLSmtpkQ255985;     AdsLSmtpkQ255985 = AdsLSmtpkQ84171783;     AdsLSmtpkQ84171783 = AdsLSmtpkQ19513481;     AdsLSmtpkQ19513481 = AdsLSmtpkQ42858107;     AdsLSmtpkQ42858107 = AdsLSmtpkQ41663972;     AdsLSmtpkQ41663972 = AdsLSmtpkQ94771129;     AdsLSmtpkQ94771129 = AdsLSmtpkQ98387067;     AdsLSmtpkQ98387067 = AdsLSmtpkQ45767932;     AdsLSmtpkQ45767932 = AdsLSmtpkQ25564744;     AdsLSmtpkQ25564744 = AdsLSmtpkQ77326820;     AdsLSmtpkQ77326820 = AdsLSmtpkQ58976377;     AdsLSmtpkQ58976377 = AdsLSmtpkQ27078802;     AdsLSmtpkQ27078802 = AdsLSmtpkQ22051103;     AdsLSmtpkQ22051103 = AdsLSmtpkQ88712108;     AdsLSmtpkQ88712108 = AdsLSmtpkQ99353446;     AdsLSmtpkQ99353446 = AdsLSmtpkQ22061514;     AdsLSmtpkQ22061514 = AdsLSmtpkQ36334678;     AdsLSmtpkQ36334678 = AdsLSmtpkQ29622479;     AdsLSmtpkQ29622479 = AdsLSmtpkQ27704446;     AdsLSmtpkQ27704446 = AdsLSmtpkQ49870340;     AdsLSmtpkQ49870340 = AdsLSmtpkQ9883810;     AdsLSmtpkQ9883810 = AdsLSmtpkQ38432903;     AdsLSmtpkQ38432903 = AdsLSmtpkQ94126501;     AdsLSmtpkQ94126501 = AdsLSmtpkQ83206672;     AdsLSmtpkQ83206672 = AdsLSmtpkQ15619281;     AdsLSmtpkQ15619281 = AdsLSmtpkQ74252270;     AdsLSmtpkQ74252270 = AdsLSmtpkQ37269232;     AdsLSmtpkQ37269232 = AdsLSmtpkQ33191027;     AdsLSmtpkQ33191027 = AdsLSmtpkQ19613666;     AdsLSmtpkQ19613666 = AdsLSmtpkQ60512109;     AdsLSmtpkQ60512109 = AdsLSmtpkQ62568293;     AdsLSmtpkQ62568293 = AdsLSmtpkQ95432686;     AdsLSmtpkQ95432686 = AdsLSmtpkQ18608765;     AdsLSmtpkQ18608765 = AdsLSmtpkQ53824529;     AdsLSmtpkQ53824529 = AdsLSmtpkQ54559829;     AdsLSmtpkQ54559829 = AdsLSmtpkQ87705449;     AdsLSmtpkQ87705449 = AdsLSmtpkQ54751600;     AdsLSmtpkQ54751600 = AdsLSmtpkQ66685534;     AdsLSmtpkQ66685534 = AdsLSmtpkQ77708522;     AdsLSmtpkQ77708522 = AdsLSmtpkQ91705418;     AdsLSmtpkQ91705418 = AdsLSmtpkQ77669836;     AdsLSmtpkQ77669836 = AdsLSmtpkQ85881439;     AdsLSmtpkQ85881439 = AdsLSmtpkQ79096145;     AdsLSmtpkQ79096145 = AdsLSmtpkQ64911473;}
// Junk Finished
