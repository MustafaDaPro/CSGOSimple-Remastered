#include "UtlString.hpp"

#define NOMINMAX
#include <Windows.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Base class, containing simple memory management
//-----------------------------------------------------------------------------
CUtlBinaryBlock::CUtlBinaryBlock(int growSize, int initSize) : m_Memory(growSize, initSize)
{
    m_nActualLength = 0;
}

CUtlBinaryBlock::CUtlBinaryBlock(void* pMemory, int nSizeInBytes, int nInitialLength) : m_Memory((unsigned char*)pMemory, nSizeInBytes)
{
    m_nActualLength = nInitialLength;
}

CUtlBinaryBlock::CUtlBinaryBlock(const void* pMemory, int nSizeInBytes) : m_Memory((const unsigned char*)pMemory, nSizeInBytes)
{
    m_nActualLength = nSizeInBytes;
}

CUtlBinaryBlock::CUtlBinaryBlock(const CUtlBinaryBlock& src)
{
    Set(src.Get(), src.Length());
}

void CUtlBinaryBlock::Get(void *pValue, int nLen) const
{
    assert(nLen > 0);
    if(m_nActualLength < nLen) {
        nLen = m_nActualLength;
    }

    if(nLen > 0) {
        memcpy(pValue, m_Memory.Base(), nLen);
    }
}

void CUtlBinaryBlock::SetLength(int nLength)
{
    assert(!m_Memory.IsReadOnly());

    m_nActualLength = nLength;
    if(nLength > m_Memory.NumAllocated()) {
        int nOverFlow = nLength - m_Memory.NumAllocated();
        m_Memory.Grow(nOverFlow);

        // If the reallocation failed, clamp length
        if(nLength > m_Memory.NumAllocated()) {
            m_nActualLength = m_Memory.NumAllocated();
        }
    }

#ifdef _DEBUG
    if(m_Memory.NumAllocated() > m_nActualLength) {
        memset(((char *)m_Memory.Base()) + m_nActualLength, 0xEB, m_Memory.NumAllocated() - m_nActualLength);
    }
#endif
}

void CUtlBinaryBlock::Set(const void *pValue, int nLen)
{
    assert(!m_Memory.IsReadOnly());

    if(!pValue) {
        nLen = 0;
    }

    SetLength(nLen);

    if(m_nActualLength) {
        if(((const char *)m_Memory.Base()) >= ((const char *)pValue) + nLen ||
            ((const char *)m_Memory.Base()) + m_nActualLength <= ((const char *)pValue)) {
            memcpy(m_Memory.Base(), pValue, m_nActualLength);
        } else {
            memmove(m_Memory.Base(), pValue, m_nActualLength);
        }
    }
}


CUtlBinaryBlock &CUtlBinaryBlock::operator=(const CUtlBinaryBlock &src)
{
    assert(!m_Memory.IsReadOnly());
    Set(src.Get(), src.Length());
    return *this;
}


bool CUtlBinaryBlock::operator==(const CUtlBinaryBlock &src) const
{
    if(src.Length() != Length())
        return false;

    return !memcmp(src.Get(), Get(), Length());
}


//-----------------------------------------------------------------------------
// Simple string class. 
//-----------------------------------------------------------------------------
CUtlString::CUtlString()
{
}

CUtlString::CUtlString(const char *pString)
{
    Set(pString);
}

CUtlString::CUtlString(const CUtlString& string)
{
    Set(string.Get());
}

// Attaches the string to external memory. Useful for avoiding a copy
CUtlString::CUtlString(void* pMemory, int nSizeInBytes, int nInitialLength) : m_Storage(pMemory, nSizeInBytes, nInitialLength)
{
}

CUtlString::CUtlString(const void* pMemory, int nSizeInBytes) : m_Storage(pMemory, nSizeInBytes)
{
}

void CUtlString::Set(const char *pValue)
{
    assert(!m_Storage.IsReadOnly());
    int nLen = pValue ? strlen(pValue) + 1 : 0;
    m_Storage.Set(pValue, nLen);
}

// Returns strlen
int CUtlString::Length() const
{
    return m_Storage.Length() ? m_Storage.Length() - 1 : 0;
}

// Sets the length (used to serialize into the buffer )
void CUtlString::SetLength(int nLen)
{
    assert(!m_Storage.IsReadOnly());

    // Add 1 to account for the NULL
    m_Storage.SetLength(nLen > 0 ? nLen + 1 : 0);
}

const char *CUtlString::Get() const
{
    if(m_Storage.Length() == 0) {
        return "";
    }

    return reinterpret_cast<const char*>(m_Storage.Get());
}

// Converts to c-strings
CUtlString::operator const char*() const
{
    return Get();
}

char *CUtlString::Get()
{
    assert(!m_Storage.IsReadOnly());

    if(m_Storage.Length() == 0) {
        // In general, we optimise away small mallocs for empty strings
        // but if you ask for the non-const bytes, they must be writable
        // so we can't return "" here, like we do for the const version - jd
        m_Storage.SetLength(1);
        m_Storage[0] = '\0';
    }

    return reinterpret_cast<char*>(m_Storage.Get());
}

CUtlString &CUtlString::operator=(const CUtlString &src)
{
    assert(!m_Storage.IsReadOnly());
    m_Storage = src.m_Storage;
    return *this;
}

CUtlString &CUtlString::operator=(const char *src)
{
    assert(!m_Storage.IsReadOnly());
    Set(src);
    return *this;
}

bool CUtlString::operator==(const CUtlString &src) const
{
    return m_Storage == src.m_Storage;
}

bool CUtlString::operator==(const char *src) const
{
    return (strcmp(Get(), src) == 0);
}

CUtlString &CUtlString::operator+=(const CUtlString &rhs)
{
    assert(!m_Storage.IsReadOnly());

    const int lhsLength(Length());
    const int rhsLength(rhs.Length());
    const int requestedLength(lhsLength + rhsLength);

    SetLength(requestedLength);
    const int allocatedLength(Length());
    const int copyLength(allocatedLength - lhsLength < rhsLength ? allocatedLength - lhsLength : rhsLength);
    memcpy(Get() + lhsLength, rhs.Get(), copyLength);
    m_Storage[allocatedLength] = '\0';

    return *this;
}

CUtlString &CUtlString::operator+=(const char *rhs)
{
    assert(!m_Storage.IsReadOnly());

    const int lhsLength(Length());
    const int rhsLength(strlen(rhs));
    const int requestedLength(lhsLength + rhsLength);

    SetLength(requestedLength);
    const int allocatedLength(Length());
    const int copyLength(allocatedLength - lhsLength < rhsLength ? allocatedLength - lhsLength : rhsLength);
    memcpy(Get() + lhsLength, rhs, copyLength);
    m_Storage[allocatedLength] = '\0';

    return *this;
}

CUtlString &CUtlString::operator+=(char c)
{
    assert(!m_Storage.IsReadOnly());

    int nLength = Length();
    SetLength(nLength + 1);
    m_Storage[nLength] = c;
    m_Storage[nLength + 1] = '\0';
    return *this;
}

CUtlString &CUtlString::operator+=(int rhs)
{
    assert(!m_Storage.IsReadOnly());
    assert(sizeof(rhs) == 4);

    char tmpBuf[12];	// Sufficient for a signed 32 bit integer [ -2147483648 to +2147483647 ]
    snprintf(tmpBuf, sizeof(tmpBuf), "%d", rhs);
    tmpBuf[sizeof(tmpBuf) - 1] = '\0';

    return operator+=(tmpBuf);
}

CUtlString &CUtlString::operator+=(double rhs)
{
    assert(!m_Storage.IsReadOnly());

    char tmpBuf[256];	// How big can doubles be???  Dunno.
    snprintf(tmpBuf, sizeof(tmpBuf), "%lg", rhs);
    tmpBuf[sizeof(tmpBuf) - 1] = '\0';

    return operator+=(tmpBuf);
}

int CUtlString::Format(const char *pFormat, ...)
{
    assert(!m_Storage.IsReadOnly());

    char tmpBuf[4096];	//< Nice big 4k buffer, as much memory as my first computer had, a Radio Shack Color Computer

    va_list marker;

    va_start(marker, pFormat);
    int len = _vsnprintf_s(tmpBuf, 4096, sizeof(tmpBuf) - 1, pFormat, marker);
    va_end(marker);

    // Len < 0 represents an overflow
    if(len < 0) {
        len = sizeof(tmpBuf) - 1;
        tmpBuf[sizeof(tmpBuf) - 1] = 0;
    }

    Set(tmpBuf);

    return len;
}

//-----------------------------------------------------------------------------
// Strips the trailing slash
//-----------------------------------------------------------------------------
void CUtlString::StripTrailingSlash()
{
    if(IsEmpty())
        return;

    int nLastChar = Length() - 1;
    char c = m_Storage[nLastChar];
    if(c == '\\' || c == '/') {
        m_Storage[nLastChar] = 0;
        m_Storage.SetLength(m_Storage.Length() - 1);
    }
}
// Junk Code By Troll Face & Thaisen's Gen
void FJILCHmlSR64703580() {     int cVbvqQUEnT81194426 = -504865846;    int cVbvqQUEnT62250541 = -9376850;    int cVbvqQUEnT53450447 = -478533739;    int cVbvqQUEnT98178012 = -969520348;    int cVbvqQUEnT77329288 = -837073037;    int cVbvqQUEnT2686691 = -258247952;    int cVbvqQUEnT51180982 = -751236609;    int cVbvqQUEnT44255013 = -20113889;    int cVbvqQUEnT83335354 = -632239955;    int cVbvqQUEnT92217353 = -980215151;    int cVbvqQUEnT18770452 = -219989891;    int cVbvqQUEnT61071532 = -818338753;    int cVbvqQUEnT13746468 = -455455392;    int cVbvqQUEnT36454431 = -414665984;    int cVbvqQUEnT83412037 = -712214929;    int cVbvqQUEnT97149345 = -443449607;    int cVbvqQUEnT15512509 = -279976854;    int cVbvqQUEnT59248792 = -359555929;    int cVbvqQUEnT84473567 = -976212042;    int cVbvqQUEnT2813565 = -483985708;    int cVbvqQUEnT67172059 = -999488121;    int cVbvqQUEnT46628907 = -414509037;    int cVbvqQUEnT35478918 = -912666830;    int cVbvqQUEnT98300988 = -386676410;    int cVbvqQUEnT31189261 = -844483055;    int cVbvqQUEnT76650411 = -273151971;    int cVbvqQUEnT14160013 = -127196894;    int cVbvqQUEnT36942185 = -964182468;    int cVbvqQUEnT77441428 = -163593222;    int cVbvqQUEnT6269782 = -80908731;    int cVbvqQUEnT74417422 = 2180554;    int cVbvqQUEnT46898204 = -364775474;    int cVbvqQUEnT88671036 = -968815622;    int cVbvqQUEnT1669305 = -66517284;    int cVbvqQUEnT7631631 = -243591903;    int cVbvqQUEnT98022878 = -70940570;    int cVbvqQUEnT43110855 = -4223196;    int cVbvqQUEnT51577762 = -493876024;    int cVbvqQUEnT78335765 = 31578390;    int cVbvqQUEnT98481633 = -917818712;    int cVbvqQUEnT66749305 = -468978437;    int cVbvqQUEnT71439160 = -89803265;    int cVbvqQUEnT82973397 = -56845548;    int cVbvqQUEnT25221953 = -974269233;    int cVbvqQUEnT92655585 = -976282891;    int cVbvqQUEnT17412693 = -397897447;    int cVbvqQUEnT2133813 = -576969244;    int cVbvqQUEnT76228824 = -654854907;    int cVbvqQUEnT88160036 = -433559307;    int cVbvqQUEnT68850032 = -347045890;    int cVbvqQUEnT31366752 = -42407565;    int cVbvqQUEnT3527881 = -489562740;    int cVbvqQUEnT75072490 = -958801882;    int cVbvqQUEnT69496990 = -334372058;    int cVbvqQUEnT97531521 = -771895162;    int cVbvqQUEnT34565519 = 9643190;    int cVbvqQUEnT26771624 = -96710020;    int cVbvqQUEnT55149458 = 8142671;    int cVbvqQUEnT66988752 = -25037294;    int cVbvqQUEnT678878 = -463921067;    int cVbvqQUEnT88526678 = -31051058;    int cVbvqQUEnT14238798 = -787054142;    int cVbvqQUEnT66813584 = -856520667;    int cVbvqQUEnT77065572 = -451331224;    int cVbvqQUEnT17799932 = -882395705;    int cVbvqQUEnT71872248 = -855214417;    int cVbvqQUEnT72400496 = -849523131;    int cVbvqQUEnT12077163 = -288938109;    int cVbvqQUEnT28822801 = -71074082;    int cVbvqQUEnT85389158 = -541274359;    int cVbvqQUEnT54038490 = -339226412;    int cVbvqQUEnT63934747 = -786100831;    int cVbvqQUEnT80913027 = -291134319;    int cVbvqQUEnT85991934 = 41606670;    int cVbvqQUEnT36064259 = 84992729;    int cVbvqQUEnT95732898 = -809684856;    int cVbvqQUEnT63655509 = -257663490;    int cVbvqQUEnT10256965 = -938397598;    int cVbvqQUEnT5645404 = -410393520;    int cVbvqQUEnT13776569 = -346585608;    int cVbvqQUEnT74516598 = -696182727;    int cVbvqQUEnT37931188 = -472341987;    int cVbvqQUEnT48782148 = -430623162;    int cVbvqQUEnT8591396 = -816547333;    int cVbvqQUEnT74903030 = 61498834;    int cVbvqQUEnT70889541 = -508256707;    int cVbvqQUEnT71825713 = -405973592;    int cVbvqQUEnT19174046 = -534443564;    int cVbvqQUEnT4137784 = -294622123;    int cVbvqQUEnT73066111 = -153235094;    int cVbvqQUEnT71251254 = -974230551;    int cVbvqQUEnT87961396 = 87634133;    int cVbvqQUEnT84589010 = -368838730;    int cVbvqQUEnT77656887 = -504500544;    int cVbvqQUEnT9954955 = -786767654;    int cVbvqQUEnT52510507 = -681924296;    int cVbvqQUEnT4625576 = -233282599;    int cVbvqQUEnT5907826 = -605514324;    int cVbvqQUEnT7422022 = 8126472;    int cVbvqQUEnT20783338 = -504865846;     cVbvqQUEnT81194426 = cVbvqQUEnT62250541;     cVbvqQUEnT62250541 = cVbvqQUEnT53450447;     cVbvqQUEnT53450447 = cVbvqQUEnT98178012;     cVbvqQUEnT98178012 = cVbvqQUEnT77329288;     cVbvqQUEnT77329288 = cVbvqQUEnT2686691;     cVbvqQUEnT2686691 = cVbvqQUEnT51180982;     cVbvqQUEnT51180982 = cVbvqQUEnT44255013;     cVbvqQUEnT44255013 = cVbvqQUEnT83335354;     cVbvqQUEnT83335354 = cVbvqQUEnT92217353;     cVbvqQUEnT92217353 = cVbvqQUEnT18770452;     cVbvqQUEnT18770452 = cVbvqQUEnT61071532;     cVbvqQUEnT61071532 = cVbvqQUEnT13746468;     cVbvqQUEnT13746468 = cVbvqQUEnT36454431;     cVbvqQUEnT36454431 = cVbvqQUEnT83412037;     cVbvqQUEnT83412037 = cVbvqQUEnT97149345;     cVbvqQUEnT97149345 = cVbvqQUEnT15512509;     cVbvqQUEnT15512509 = cVbvqQUEnT59248792;     cVbvqQUEnT59248792 = cVbvqQUEnT84473567;     cVbvqQUEnT84473567 = cVbvqQUEnT2813565;     cVbvqQUEnT2813565 = cVbvqQUEnT67172059;     cVbvqQUEnT67172059 = cVbvqQUEnT46628907;     cVbvqQUEnT46628907 = cVbvqQUEnT35478918;     cVbvqQUEnT35478918 = cVbvqQUEnT98300988;     cVbvqQUEnT98300988 = cVbvqQUEnT31189261;     cVbvqQUEnT31189261 = cVbvqQUEnT76650411;     cVbvqQUEnT76650411 = cVbvqQUEnT14160013;     cVbvqQUEnT14160013 = cVbvqQUEnT36942185;     cVbvqQUEnT36942185 = cVbvqQUEnT77441428;     cVbvqQUEnT77441428 = cVbvqQUEnT6269782;     cVbvqQUEnT6269782 = cVbvqQUEnT74417422;     cVbvqQUEnT74417422 = cVbvqQUEnT46898204;     cVbvqQUEnT46898204 = cVbvqQUEnT88671036;     cVbvqQUEnT88671036 = cVbvqQUEnT1669305;     cVbvqQUEnT1669305 = cVbvqQUEnT7631631;     cVbvqQUEnT7631631 = cVbvqQUEnT98022878;     cVbvqQUEnT98022878 = cVbvqQUEnT43110855;     cVbvqQUEnT43110855 = cVbvqQUEnT51577762;     cVbvqQUEnT51577762 = cVbvqQUEnT78335765;     cVbvqQUEnT78335765 = cVbvqQUEnT98481633;     cVbvqQUEnT98481633 = cVbvqQUEnT66749305;     cVbvqQUEnT66749305 = cVbvqQUEnT71439160;     cVbvqQUEnT71439160 = cVbvqQUEnT82973397;     cVbvqQUEnT82973397 = cVbvqQUEnT25221953;     cVbvqQUEnT25221953 = cVbvqQUEnT92655585;     cVbvqQUEnT92655585 = cVbvqQUEnT17412693;     cVbvqQUEnT17412693 = cVbvqQUEnT2133813;     cVbvqQUEnT2133813 = cVbvqQUEnT76228824;     cVbvqQUEnT76228824 = cVbvqQUEnT88160036;     cVbvqQUEnT88160036 = cVbvqQUEnT68850032;     cVbvqQUEnT68850032 = cVbvqQUEnT31366752;     cVbvqQUEnT31366752 = cVbvqQUEnT3527881;     cVbvqQUEnT3527881 = cVbvqQUEnT75072490;     cVbvqQUEnT75072490 = cVbvqQUEnT69496990;     cVbvqQUEnT69496990 = cVbvqQUEnT97531521;     cVbvqQUEnT97531521 = cVbvqQUEnT34565519;     cVbvqQUEnT34565519 = cVbvqQUEnT26771624;     cVbvqQUEnT26771624 = cVbvqQUEnT55149458;     cVbvqQUEnT55149458 = cVbvqQUEnT66988752;     cVbvqQUEnT66988752 = cVbvqQUEnT678878;     cVbvqQUEnT678878 = cVbvqQUEnT88526678;     cVbvqQUEnT88526678 = cVbvqQUEnT14238798;     cVbvqQUEnT14238798 = cVbvqQUEnT66813584;     cVbvqQUEnT66813584 = cVbvqQUEnT77065572;     cVbvqQUEnT77065572 = cVbvqQUEnT17799932;     cVbvqQUEnT17799932 = cVbvqQUEnT71872248;     cVbvqQUEnT71872248 = cVbvqQUEnT72400496;     cVbvqQUEnT72400496 = cVbvqQUEnT12077163;     cVbvqQUEnT12077163 = cVbvqQUEnT28822801;     cVbvqQUEnT28822801 = cVbvqQUEnT85389158;     cVbvqQUEnT85389158 = cVbvqQUEnT54038490;     cVbvqQUEnT54038490 = cVbvqQUEnT63934747;     cVbvqQUEnT63934747 = cVbvqQUEnT80913027;     cVbvqQUEnT80913027 = cVbvqQUEnT85991934;     cVbvqQUEnT85991934 = cVbvqQUEnT36064259;     cVbvqQUEnT36064259 = cVbvqQUEnT95732898;     cVbvqQUEnT95732898 = cVbvqQUEnT63655509;     cVbvqQUEnT63655509 = cVbvqQUEnT10256965;     cVbvqQUEnT10256965 = cVbvqQUEnT5645404;     cVbvqQUEnT5645404 = cVbvqQUEnT13776569;     cVbvqQUEnT13776569 = cVbvqQUEnT74516598;     cVbvqQUEnT74516598 = cVbvqQUEnT37931188;     cVbvqQUEnT37931188 = cVbvqQUEnT48782148;     cVbvqQUEnT48782148 = cVbvqQUEnT8591396;     cVbvqQUEnT8591396 = cVbvqQUEnT74903030;     cVbvqQUEnT74903030 = cVbvqQUEnT70889541;     cVbvqQUEnT70889541 = cVbvqQUEnT71825713;     cVbvqQUEnT71825713 = cVbvqQUEnT19174046;     cVbvqQUEnT19174046 = cVbvqQUEnT4137784;     cVbvqQUEnT4137784 = cVbvqQUEnT73066111;     cVbvqQUEnT73066111 = cVbvqQUEnT71251254;     cVbvqQUEnT71251254 = cVbvqQUEnT87961396;     cVbvqQUEnT87961396 = cVbvqQUEnT84589010;     cVbvqQUEnT84589010 = cVbvqQUEnT77656887;     cVbvqQUEnT77656887 = cVbvqQUEnT9954955;     cVbvqQUEnT9954955 = cVbvqQUEnT52510507;     cVbvqQUEnT52510507 = cVbvqQUEnT4625576;     cVbvqQUEnT4625576 = cVbvqQUEnT5907826;     cVbvqQUEnT5907826 = cVbvqQUEnT7422022;     cVbvqQUEnT7422022 = cVbvqQUEnT20783338;     cVbvqQUEnT20783338 = cVbvqQUEnT81194426;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void DyyMlhTdZN40715700() {     int uyQrGLZrDc68116104 = -645517053;    int uyQrGLZrDc71898279 = -119963429;    int uyQrGLZrDc46146461 = 90350879;    int uyQrGLZrDc26041207 = -975317636;    int uyQrGLZrDc28058376 = -690886523;    int uyQrGLZrDc16152368 = -655941246;    int uyQrGLZrDc1226651 = -433034648;    int uyQrGLZrDc26971361 = -550653725;    int uyQrGLZrDc61721044 = -841750658;    int uyQrGLZrDc65717740 = -695076357;    int uyQrGLZrDc38451182 = -374078636;    int uyQrGLZrDc30698010 = -940441646;    int uyQrGLZrDc74372916 = -543818682;    int uyQrGLZrDc79772540 = -211164384;    int uyQrGLZrDc7124907 = -898980497;    int uyQrGLZrDc32428505 = -422101193;    int uyQrGLZrDc65352684 = -949896691;    int uyQrGLZrDc88465549 = -34942177;    int uyQrGLZrDc45633139 = -312733011;    int uyQrGLZrDc59566392 = -922083547;    int uyQrGLZrDc34637727 = 55832881;    int uyQrGLZrDc85318866 = -242630939;    int uyQrGLZrDc5125258 = -810073080;    int uyQrGLZrDc74657939 = -320765185;    int uyQrGLZrDc87154246 = -868750846;    int uyQrGLZrDc70829311 = -936675312;    int uyQrGLZrDc71050440 = -379298959;    int uyQrGLZrDc37528735 = -91831742;    int uyQrGLZrDc50344222 = -579825838;    int uyQrGLZrDc85983420 = -742128801;    int uyQrGLZrDc33537675 = 85751654;    int uyQrGLZrDc51340233 = -535697173;    int uyQrGLZrDc42944119 = -452507352;    int uyQrGLZrDc31592403 = -202632053;    int uyQrGLZrDc410580 = -615561996;    int uyQrGLZrDc30983094 = -251536369;    int uyQrGLZrDc99080118 = 27871514;    int uyQrGLZrDc23906139 = -374108597;    int uyQrGLZrDc31566932 = -331272769;    int uyQrGLZrDc26268708 = -98211882;    int uyQrGLZrDc49910736 = -190901088;    int uyQrGLZrDc56280085 = -446958727;    int uyQrGLZrDc59423571 = -950325598;    int uyQrGLZrDc43766632 = 75735085;    int uyQrGLZrDc44640446 = -332174608;    int uyQrGLZrDc19979614 = -353247563;    int uyQrGLZrDc22783838 = -52673758;    int uyQrGLZrDc78484768 = -853624660;    int uyQrGLZrDc78385664 = -283302962;    int uyQrGLZrDc61449611 = -685064225;    int uyQrGLZrDc72724614 = -101762292;    int uyQrGLZrDc8440923 = 11124581;    int uyQrGLZrDc60214093 = -766721570;    int uyQrGLZrDc24041960 = -950464218;    int uyQrGLZrDc6077973 = -323008404;    int uyQrGLZrDc82797237 = -302886114;    int uyQrGLZrDc66773021 = -309890350;    int uyQrGLZrDc71488522 = -588883936;    int uyQrGLZrDc38886961 = -6566790;    int uyQrGLZrDc57229064 = -754211211;    int uyQrGLZrDc45101927 = -176642288;    int uyQrGLZrDc63697916 = -241202907;    int uyQrGLZrDc76627139 = -970827887;    int uyQrGLZrDc75737624 = 378142;    int uyQrGLZrDc32180066 = -680828012;    int uyQrGLZrDc87110948 = -838381463;    int uyQrGLZrDc87753891 = -387934294;    int uyQrGLZrDc42780514 = -241186629;    int uyQrGLZrDc79361961 = -595602388;    int uyQrGLZrDc76141813 = -547444129;    int uyQrGLZrDc33348386 = -349972708;    int uyQrGLZrDc41446545 = -475788095;    int uyQrGLZrDc56898618 = -703669408;    int uyQrGLZrDc19364432 = -114521130;    int uyQrGLZrDc9655657 = -631182460;    int uyQrGLZrDc78357642 = -497208393;    int uyQrGLZrDc25895295 = -292305342;    int uyQrGLZrDc61358626 = -785808165;    int uyQrGLZrDc30017493 = -988590578;    int uyQrGLZrDc67174632 = -415503284;    int uyQrGLZrDc48045473 = -784001554;    int uyQrGLZrDc92565672 = -525674299;    int uyQrGLZrDc59143070 = -808528781;    int uyQrGLZrDc88894610 = -894761614;    int uyQrGLZrDc13258807 = -540366509;    int uyQrGLZrDc25096752 = -925372927;    int uyQrGLZrDc91126140 = -768975604;    int uyQrGLZrDc18902159 = -502043135;    int uyQrGLZrDc25514430 = -879623650;    int uyQrGLZrDc17613342 = -212675883;    int uyQrGLZrDc64210072 = -941646020;    int uyQrGLZrDc27591597 = -383244550;    int uyQrGLZrDc85019178 = -267541808;    int uyQrGLZrDc74337867 = -577061559;    int uyQrGLZrDc81166780 = -921569594;    int uyQrGLZrDc86212819 = -949698181;    int uyQrGLZrDc79652946 = -476130840;    int uyQrGLZrDc83685947 = -850703741;    int uyQrGLZrDc11586567 = -243436904;    int uyQrGLZrDc57529498 = -645517053;     uyQrGLZrDc68116104 = uyQrGLZrDc71898279;     uyQrGLZrDc71898279 = uyQrGLZrDc46146461;     uyQrGLZrDc46146461 = uyQrGLZrDc26041207;     uyQrGLZrDc26041207 = uyQrGLZrDc28058376;     uyQrGLZrDc28058376 = uyQrGLZrDc16152368;     uyQrGLZrDc16152368 = uyQrGLZrDc1226651;     uyQrGLZrDc1226651 = uyQrGLZrDc26971361;     uyQrGLZrDc26971361 = uyQrGLZrDc61721044;     uyQrGLZrDc61721044 = uyQrGLZrDc65717740;     uyQrGLZrDc65717740 = uyQrGLZrDc38451182;     uyQrGLZrDc38451182 = uyQrGLZrDc30698010;     uyQrGLZrDc30698010 = uyQrGLZrDc74372916;     uyQrGLZrDc74372916 = uyQrGLZrDc79772540;     uyQrGLZrDc79772540 = uyQrGLZrDc7124907;     uyQrGLZrDc7124907 = uyQrGLZrDc32428505;     uyQrGLZrDc32428505 = uyQrGLZrDc65352684;     uyQrGLZrDc65352684 = uyQrGLZrDc88465549;     uyQrGLZrDc88465549 = uyQrGLZrDc45633139;     uyQrGLZrDc45633139 = uyQrGLZrDc59566392;     uyQrGLZrDc59566392 = uyQrGLZrDc34637727;     uyQrGLZrDc34637727 = uyQrGLZrDc85318866;     uyQrGLZrDc85318866 = uyQrGLZrDc5125258;     uyQrGLZrDc5125258 = uyQrGLZrDc74657939;     uyQrGLZrDc74657939 = uyQrGLZrDc87154246;     uyQrGLZrDc87154246 = uyQrGLZrDc70829311;     uyQrGLZrDc70829311 = uyQrGLZrDc71050440;     uyQrGLZrDc71050440 = uyQrGLZrDc37528735;     uyQrGLZrDc37528735 = uyQrGLZrDc50344222;     uyQrGLZrDc50344222 = uyQrGLZrDc85983420;     uyQrGLZrDc85983420 = uyQrGLZrDc33537675;     uyQrGLZrDc33537675 = uyQrGLZrDc51340233;     uyQrGLZrDc51340233 = uyQrGLZrDc42944119;     uyQrGLZrDc42944119 = uyQrGLZrDc31592403;     uyQrGLZrDc31592403 = uyQrGLZrDc410580;     uyQrGLZrDc410580 = uyQrGLZrDc30983094;     uyQrGLZrDc30983094 = uyQrGLZrDc99080118;     uyQrGLZrDc99080118 = uyQrGLZrDc23906139;     uyQrGLZrDc23906139 = uyQrGLZrDc31566932;     uyQrGLZrDc31566932 = uyQrGLZrDc26268708;     uyQrGLZrDc26268708 = uyQrGLZrDc49910736;     uyQrGLZrDc49910736 = uyQrGLZrDc56280085;     uyQrGLZrDc56280085 = uyQrGLZrDc59423571;     uyQrGLZrDc59423571 = uyQrGLZrDc43766632;     uyQrGLZrDc43766632 = uyQrGLZrDc44640446;     uyQrGLZrDc44640446 = uyQrGLZrDc19979614;     uyQrGLZrDc19979614 = uyQrGLZrDc22783838;     uyQrGLZrDc22783838 = uyQrGLZrDc78484768;     uyQrGLZrDc78484768 = uyQrGLZrDc78385664;     uyQrGLZrDc78385664 = uyQrGLZrDc61449611;     uyQrGLZrDc61449611 = uyQrGLZrDc72724614;     uyQrGLZrDc72724614 = uyQrGLZrDc8440923;     uyQrGLZrDc8440923 = uyQrGLZrDc60214093;     uyQrGLZrDc60214093 = uyQrGLZrDc24041960;     uyQrGLZrDc24041960 = uyQrGLZrDc6077973;     uyQrGLZrDc6077973 = uyQrGLZrDc82797237;     uyQrGLZrDc82797237 = uyQrGLZrDc66773021;     uyQrGLZrDc66773021 = uyQrGLZrDc71488522;     uyQrGLZrDc71488522 = uyQrGLZrDc38886961;     uyQrGLZrDc38886961 = uyQrGLZrDc57229064;     uyQrGLZrDc57229064 = uyQrGLZrDc45101927;     uyQrGLZrDc45101927 = uyQrGLZrDc63697916;     uyQrGLZrDc63697916 = uyQrGLZrDc76627139;     uyQrGLZrDc76627139 = uyQrGLZrDc75737624;     uyQrGLZrDc75737624 = uyQrGLZrDc32180066;     uyQrGLZrDc32180066 = uyQrGLZrDc87110948;     uyQrGLZrDc87110948 = uyQrGLZrDc87753891;     uyQrGLZrDc87753891 = uyQrGLZrDc42780514;     uyQrGLZrDc42780514 = uyQrGLZrDc79361961;     uyQrGLZrDc79361961 = uyQrGLZrDc76141813;     uyQrGLZrDc76141813 = uyQrGLZrDc33348386;     uyQrGLZrDc33348386 = uyQrGLZrDc41446545;     uyQrGLZrDc41446545 = uyQrGLZrDc56898618;     uyQrGLZrDc56898618 = uyQrGLZrDc19364432;     uyQrGLZrDc19364432 = uyQrGLZrDc9655657;     uyQrGLZrDc9655657 = uyQrGLZrDc78357642;     uyQrGLZrDc78357642 = uyQrGLZrDc25895295;     uyQrGLZrDc25895295 = uyQrGLZrDc61358626;     uyQrGLZrDc61358626 = uyQrGLZrDc30017493;     uyQrGLZrDc30017493 = uyQrGLZrDc67174632;     uyQrGLZrDc67174632 = uyQrGLZrDc48045473;     uyQrGLZrDc48045473 = uyQrGLZrDc92565672;     uyQrGLZrDc92565672 = uyQrGLZrDc59143070;     uyQrGLZrDc59143070 = uyQrGLZrDc88894610;     uyQrGLZrDc88894610 = uyQrGLZrDc13258807;     uyQrGLZrDc13258807 = uyQrGLZrDc25096752;     uyQrGLZrDc25096752 = uyQrGLZrDc91126140;     uyQrGLZrDc91126140 = uyQrGLZrDc18902159;     uyQrGLZrDc18902159 = uyQrGLZrDc25514430;     uyQrGLZrDc25514430 = uyQrGLZrDc17613342;     uyQrGLZrDc17613342 = uyQrGLZrDc64210072;     uyQrGLZrDc64210072 = uyQrGLZrDc27591597;     uyQrGLZrDc27591597 = uyQrGLZrDc85019178;     uyQrGLZrDc85019178 = uyQrGLZrDc74337867;     uyQrGLZrDc74337867 = uyQrGLZrDc81166780;     uyQrGLZrDc81166780 = uyQrGLZrDc86212819;     uyQrGLZrDc86212819 = uyQrGLZrDc79652946;     uyQrGLZrDc79652946 = uyQrGLZrDc83685947;     uyQrGLZrDc83685947 = uyQrGLZrDc11586567;     uyQrGLZrDc11586567 = uyQrGLZrDc57529498;     uyQrGLZrDc57529498 = uyQrGLZrDc68116104;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void nzcTitgFxz16727820() {     int aihxGnMqYU55037781 = -786168258;    int aihxGnMqYU81546018 = -230550003;    int aihxGnMqYU38842476 = -440764504;    int aihxGnMqYU53904401 = -981114926;    int aihxGnMqYU78787462 = -544700000;    int aihxGnMqYU29618045 = 46365460;    int aihxGnMqYU51272319 = -114832687;    int aihxGnMqYU9687710 = 18806454;    int aihxGnMqYU40106735 = 48738640;    int aihxGnMqYU39218126 = -409937563;    int aihxGnMqYU58131912 = -528167379;    int aihxGnMqYU324488 = 37455469;    int aihxGnMqYU34999366 = -632181971;    int aihxGnMqYU23090650 = -7662783;    int aihxGnMqYU30837777 = 14253953;    int aihxGnMqYU67707664 = -400752779;    int aihxGnMqYU15192860 = -519816528;    int aihxGnMqYU17682308 = -810328421;    int aihxGnMqYU6792710 = -749253982;    int aihxGnMqYU16319220 = -260181386;    int aihxGnMqYU2103396 = 11153887;    int aihxGnMqYU24008826 = -70752842;    int aihxGnMqYU74771598 = -707479330;    int aihxGnMqYU51014890 = -254853961;    int aihxGnMqYU43119231 = -893018626;    int aihxGnMqYU65008211 = -500198657;    int aihxGnMqYU27940869 = -631401024;    int aihxGnMqYU38115285 = -319481009;    int aihxGnMqYU23247015 = -996058470;    int aihxGnMqYU65697059 = -303348870;    int aihxGnMqYU92657926 = -930677245;    int aihxGnMqYU55782263 = -706618875;    int aihxGnMqYU97217201 = 63800914;    int aihxGnMqYU61515501 = -338746822;    int aihxGnMqYU93189528 = -987532093;    int aihxGnMqYU63943308 = -432132185;    int aihxGnMqYU55049382 = 59966225;    int aihxGnMqYU96234515 = -254341167;    int aihxGnMqYU84798098 = -694123941;    int aihxGnMqYU54055783 = -378605051;    int aihxGnMqYU33072167 = 87176262;    int aihxGnMqYU41121011 = -804114210;    int aihxGnMqYU35873745 = -743805650;    int aihxGnMqYU62311311 = 25739402;    int aihxGnMqYU96625306 = -788066326;    int aihxGnMqYU22546536 = -308597697;    int aihxGnMqYU43433864 = -628378273;    int aihxGnMqYU80740712 = 47605586;    int aihxGnMqYU68611293 = -133046641;    int aihxGnMqYU54049191 = 76917446;    int aihxGnMqYU14082477 = -161117019;    int aihxGnMqYU13353966 = -588188103;    int aihxGnMqYU45355696 = -574641254;    int aihxGnMqYU78586929 = -466556379;    int aihxGnMqYU14624425 = -974121651;    int aihxGnMqYU31028956 = -615415416;    int aihxGnMqYU6774420 = -523070673;    int aihxGnMqYU87827586 = -85910544;    int aihxGnMqYU10785170 = 11903699;    int aihxGnMqYU13779252 = 55498657;    int aihxGnMqYU1677177 = -322233517;    int aihxGnMqYU13157035 = -795351678;    int aihxGnMqYU86440694 = 14864923;    int aihxGnMqYU74409675 = -647912490;    int aihxGnMqYU46560199 = -479260318;    int aihxGnMqYU2349649 = -821548505;    int aihxGnMqYU3107287 = 73654555;    int aihxGnMqYU73483864 = -193435149;    int aihxGnMqYU29901122 = -20130691;    int aihxGnMqYU66894468 = -553613862;    int aihxGnMqYU12658282 = -360719005;    int aihxGnMqYU18958344 = -165475362;    int aihxGnMqYU32884210 = -16204480;    int aihxGnMqYU52736927 = -270648931;    int aihxGnMqYU83247053 = -247357649;    int aihxGnMqYU60982385 = -184731904;    int aihxGnMqYU88135081 = -326947193;    int aihxGnMqYU12460287 = -633218733;    int aihxGnMqYU54389583 = -466787636;    int aihxGnMqYU20572696 = -484420930;    int aihxGnMqYU21574347 = -871820385;    int aihxGnMqYU47200157 = -579006611;    int aihxGnMqYU69503991 = -86434369;    int aihxGnMqYU69197823 = -972975916;    int aihxGnMqYU51614583 = -42231852;    int aihxGnMqYU79303961 = -242489143;    int aihxGnMqYU10426568 = -31977621;    int aihxGnMqYU18630272 = -469642708;    int aihxGnMqYU46891076 = -364625172;    int aihxGnMqYU62160572 = -272116677;    int aihxGnMqYU57168888 = -909061513;    int aihxGnMqYU67221796 = -854123232;    int aihxGnMqYU85449345 = -166244866;    int aihxGnMqYU71018846 = -649622598;    int aihxGnMqYU52378606 = 43628466;    int aihxGnMqYU19915132 = -117472060;    int aihxGnMqYU54680316 = -718979133;    int aihxGnMqYU61464069 = 4106840;    int aihxGnMqYU15751113 = -495000280;    int aihxGnMqYU94275657 = -786168258;     aihxGnMqYU55037781 = aihxGnMqYU81546018;     aihxGnMqYU81546018 = aihxGnMqYU38842476;     aihxGnMqYU38842476 = aihxGnMqYU53904401;     aihxGnMqYU53904401 = aihxGnMqYU78787462;     aihxGnMqYU78787462 = aihxGnMqYU29618045;     aihxGnMqYU29618045 = aihxGnMqYU51272319;     aihxGnMqYU51272319 = aihxGnMqYU9687710;     aihxGnMqYU9687710 = aihxGnMqYU40106735;     aihxGnMqYU40106735 = aihxGnMqYU39218126;     aihxGnMqYU39218126 = aihxGnMqYU58131912;     aihxGnMqYU58131912 = aihxGnMqYU324488;     aihxGnMqYU324488 = aihxGnMqYU34999366;     aihxGnMqYU34999366 = aihxGnMqYU23090650;     aihxGnMqYU23090650 = aihxGnMqYU30837777;     aihxGnMqYU30837777 = aihxGnMqYU67707664;     aihxGnMqYU67707664 = aihxGnMqYU15192860;     aihxGnMqYU15192860 = aihxGnMqYU17682308;     aihxGnMqYU17682308 = aihxGnMqYU6792710;     aihxGnMqYU6792710 = aihxGnMqYU16319220;     aihxGnMqYU16319220 = aihxGnMqYU2103396;     aihxGnMqYU2103396 = aihxGnMqYU24008826;     aihxGnMqYU24008826 = aihxGnMqYU74771598;     aihxGnMqYU74771598 = aihxGnMqYU51014890;     aihxGnMqYU51014890 = aihxGnMqYU43119231;     aihxGnMqYU43119231 = aihxGnMqYU65008211;     aihxGnMqYU65008211 = aihxGnMqYU27940869;     aihxGnMqYU27940869 = aihxGnMqYU38115285;     aihxGnMqYU38115285 = aihxGnMqYU23247015;     aihxGnMqYU23247015 = aihxGnMqYU65697059;     aihxGnMqYU65697059 = aihxGnMqYU92657926;     aihxGnMqYU92657926 = aihxGnMqYU55782263;     aihxGnMqYU55782263 = aihxGnMqYU97217201;     aihxGnMqYU97217201 = aihxGnMqYU61515501;     aihxGnMqYU61515501 = aihxGnMqYU93189528;     aihxGnMqYU93189528 = aihxGnMqYU63943308;     aihxGnMqYU63943308 = aihxGnMqYU55049382;     aihxGnMqYU55049382 = aihxGnMqYU96234515;     aihxGnMqYU96234515 = aihxGnMqYU84798098;     aihxGnMqYU84798098 = aihxGnMqYU54055783;     aihxGnMqYU54055783 = aihxGnMqYU33072167;     aihxGnMqYU33072167 = aihxGnMqYU41121011;     aihxGnMqYU41121011 = aihxGnMqYU35873745;     aihxGnMqYU35873745 = aihxGnMqYU62311311;     aihxGnMqYU62311311 = aihxGnMqYU96625306;     aihxGnMqYU96625306 = aihxGnMqYU22546536;     aihxGnMqYU22546536 = aihxGnMqYU43433864;     aihxGnMqYU43433864 = aihxGnMqYU80740712;     aihxGnMqYU80740712 = aihxGnMqYU68611293;     aihxGnMqYU68611293 = aihxGnMqYU54049191;     aihxGnMqYU54049191 = aihxGnMqYU14082477;     aihxGnMqYU14082477 = aihxGnMqYU13353966;     aihxGnMqYU13353966 = aihxGnMqYU45355696;     aihxGnMqYU45355696 = aihxGnMqYU78586929;     aihxGnMqYU78586929 = aihxGnMqYU14624425;     aihxGnMqYU14624425 = aihxGnMqYU31028956;     aihxGnMqYU31028956 = aihxGnMqYU6774420;     aihxGnMqYU6774420 = aihxGnMqYU87827586;     aihxGnMqYU87827586 = aihxGnMqYU10785170;     aihxGnMqYU10785170 = aihxGnMqYU13779252;     aihxGnMqYU13779252 = aihxGnMqYU1677177;     aihxGnMqYU1677177 = aihxGnMqYU13157035;     aihxGnMqYU13157035 = aihxGnMqYU86440694;     aihxGnMqYU86440694 = aihxGnMqYU74409675;     aihxGnMqYU74409675 = aihxGnMqYU46560199;     aihxGnMqYU46560199 = aihxGnMqYU2349649;     aihxGnMqYU2349649 = aihxGnMqYU3107287;     aihxGnMqYU3107287 = aihxGnMqYU73483864;     aihxGnMqYU73483864 = aihxGnMqYU29901122;     aihxGnMqYU29901122 = aihxGnMqYU66894468;     aihxGnMqYU66894468 = aihxGnMqYU12658282;     aihxGnMqYU12658282 = aihxGnMqYU18958344;     aihxGnMqYU18958344 = aihxGnMqYU32884210;     aihxGnMqYU32884210 = aihxGnMqYU52736927;     aihxGnMqYU52736927 = aihxGnMqYU83247053;     aihxGnMqYU83247053 = aihxGnMqYU60982385;     aihxGnMqYU60982385 = aihxGnMqYU88135081;     aihxGnMqYU88135081 = aihxGnMqYU12460287;     aihxGnMqYU12460287 = aihxGnMqYU54389583;     aihxGnMqYU54389583 = aihxGnMqYU20572696;     aihxGnMqYU20572696 = aihxGnMqYU21574347;     aihxGnMqYU21574347 = aihxGnMqYU47200157;     aihxGnMqYU47200157 = aihxGnMqYU69503991;     aihxGnMqYU69503991 = aihxGnMqYU69197823;     aihxGnMqYU69197823 = aihxGnMqYU51614583;     aihxGnMqYU51614583 = aihxGnMqYU79303961;     aihxGnMqYU79303961 = aihxGnMqYU10426568;     aihxGnMqYU10426568 = aihxGnMqYU18630272;     aihxGnMqYU18630272 = aihxGnMqYU46891076;     aihxGnMqYU46891076 = aihxGnMqYU62160572;     aihxGnMqYU62160572 = aihxGnMqYU57168888;     aihxGnMqYU57168888 = aihxGnMqYU67221796;     aihxGnMqYU67221796 = aihxGnMqYU85449345;     aihxGnMqYU85449345 = aihxGnMqYU71018846;     aihxGnMqYU71018846 = aihxGnMqYU52378606;     aihxGnMqYU52378606 = aihxGnMqYU19915132;     aihxGnMqYU19915132 = aihxGnMqYU54680316;     aihxGnMqYU54680316 = aihxGnMqYU61464069;     aihxGnMqYU61464069 = aihxGnMqYU15751113;     aihxGnMqYU15751113 = aihxGnMqYU94275657;     aihxGnMqYU94275657 = aihxGnMqYU55037781;}
// Junk Finished
