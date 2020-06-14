#include "VMatrix.hpp"

//-----------------------------------------------------------------------------
// VMatrix inlines.
//-----------------------------------------------------------------------------
inline VMatrix::VMatrix()
{
}

inline VMatrix::VMatrix(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33)
{
    Init(
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23,
        m30, m31, m32, m33
    );
}


inline VMatrix::VMatrix(const matrix3x4_t& matrix3x4)
{
    Init(matrix3x4);
}


//-----------------------------------------------------------------------------
// Creates a matrix where the X axis = forward
// the Y axis = left, and the Z axis = up
//-----------------------------------------------------------------------------
inline VMatrix::VMatrix(const Vector& xAxis, const Vector& yAxis, const Vector& zAxis)
{
    Init(
        xAxis.x, yAxis.x, zAxis.x, 0.0f,
        xAxis.y, yAxis.y, zAxis.y, 0.0f,
        xAxis.z, yAxis.z, zAxis.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
}


inline void VMatrix::Init(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33
)
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[0][3] = m03;

    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[1][3] = m13;

    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
    m[2][3] = m23;

    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
    m[3][3] = m33;
}


//-----------------------------------------------------------------------------
// Initialize from a 3x4
//-----------------------------------------------------------------------------
inline void VMatrix::Init(const matrix3x4_t& matrix3x4)
{
    memcpy(m, matrix3x4.Base(), sizeof(matrix3x4_t));

    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

//-----------------------------------------------------------------------------
// Vector3DMultiplyPosition treats src2 as if it's a point (adds the translation)
//-----------------------------------------------------------------------------
// NJS: src2 is passed in as a full vector rather than a reference to prevent the need
// for 2 branches and a potential copy in the body.  (ie, handling the case when the src2
// reference is the same as the dst reference ).
inline void Vector3DMultiplyPosition(const VMatrix& src1, const Vector& src2, Vector& dst)
{
    dst[0] = src1[0][0] * src2.x + src1[0][1] * src2.y + src1[0][2] * src2.z + src1[0][3];
    dst[1] = src1[1][0] * src2.x + src1[1][1] * src2.y + src1[1][2] * src2.z + src1[1][3];
    dst[2] = src1[2][0] * src2.x + src1[2][1] * src2.y + src1[2][2] * src2.z + src1[2][3];
}

//-----------------------------------------------------------------------------
// Methods related to the basis vectors of the matrix
//-----------------------------------------------------------------------------

inline Vector VMatrix::GetForward() const
{
    return Vector(m[0][0], m[1][0], m[2][0]);
}

inline Vector VMatrix::GetLeft() const
{
    return Vector(m[0][1], m[1][1], m[2][1]);
}

inline Vector VMatrix::GetUp() const
{
    return Vector(m[0][2], m[1][2], m[2][2]);
}

inline void VMatrix::SetForward(const Vector &vForward)
{
    m[0][0] = vForward.x;
    m[1][0] = vForward.y;
    m[2][0] = vForward.z;
}

inline void VMatrix::SetLeft(const Vector &vLeft)
{
    m[0][1] = vLeft.x;
    m[1][1] = vLeft.y;
    m[2][1] = vLeft.z;
}

inline void VMatrix::SetUp(const Vector &vUp)
{
    m[0][2] = vUp.x;
    m[1][2] = vUp.y;
    m[2][2] = vUp.z;
}

inline void VMatrix::GetBasisVectors(Vector &vForward, Vector &vLeft, Vector &vUp) const
{
    vForward.Init(m[0][0], m[1][0], m[2][0]);
    vLeft.Init(m[0][1], m[1][1], m[2][1]);
    vUp.Init(m[0][2], m[1][2], m[2][2]);
}

inline void VMatrix::SetBasisVectors(const Vector &vForward, const Vector &vLeft, const Vector &vUp)
{
    SetForward(vForward);
    SetLeft(vLeft);
    SetUp(vUp);
}


//-----------------------------------------------------------------------------
// Methods related to the translation component of the matrix
//-----------------------------------------------------------------------------

inline Vector VMatrix::GetTranslation() const
{
    return Vector(m[0][3], m[1][3], m[2][3]);
}

inline Vector& VMatrix::GetTranslation(Vector &vTrans) const
{
    vTrans.x = m[0][3];
    vTrans.y = m[1][3];
    vTrans.z = m[2][3];
    return vTrans;
}

inline void VMatrix::SetTranslation(const Vector &vTrans)
{
    m[0][3] = vTrans.x;
    m[1][3] = vTrans.y;
    m[2][3] = vTrans.z;
}


//-----------------------------------------------------------------------------
// appply translation to this matrix in the input space
//-----------------------------------------------------------------------------
inline void VMatrix::PreTranslate(const Vector &vTrans)
{
    Vector tmp;
    Vector3DMultiplyPosition(*this, vTrans, tmp);
    m[0][3] = tmp.x;
    m[1][3] = tmp.y;
    m[2][3] = tmp.z;
}


//-----------------------------------------------------------------------------
// appply translation to this matrix in the output space
//-----------------------------------------------------------------------------
inline void VMatrix::PostTranslate(const Vector &vTrans)
{
    m[0][3] += vTrans.x;
    m[1][3] += vTrans.y;
    m[2][3] += vTrans.z;
}

inline const matrix3x4_t& VMatrix::As3x4() const
{
    return *((const matrix3x4_t*)this);
}

inline matrix3x4_t& VMatrix::As3x4()
{
    return *((matrix3x4_t*)this);
}

inline void VMatrix::CopyFrom3x4(const matrix3x4_t &m3x4)
{
    memcpy(m, m3x4.Base(), sizeof(matrix3x4_t));
    m[3][0] = m[3][1] = m[3][2] = 0;
    m[3][3] = 1;
}

inline void VMatrix::Set3x4(matrix3x4_t& matrix3x4) const
{
    memcpy(matrix3x4.Base(), m, sizeof(matrix3x4_t));
}


//-----------------------------------------------------------------------------
// Matrix Math operations
//-----------------------------------------------------------------------------
inline const VMatrix& VMatrix::operator+=(const VMatrix &other)
{
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            m[i][j] += other.m[i][j];
        }
    }

    return *this;
}

inline VMatrix VMatrix::operator+(const VMatrix &other) const
{
    VMatrix ret;
    for(int i = 0; i < 16; i++) {
        ((float*)ret.m)[i] = ((float*)m)[i] + ((float*)other.m)[i];
    }
    return ret;
}

inline VMatrix VMatrix::operator-(const VMatrix &other) const
{
    VMatrix ret;

    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++) {
            ret.m[i][j] = m[i][j] - other.m[i][j];
        }
    }

    return ret;
}

inline VMatrix VMatrix::operator-() const
{
    VMatrix ret;
    for(int i = 0; i < 16; i++) {
        ((float*)ret.m)[i] = -((float*)m)[i];
    }
    return ret;
}

//-----------------------------------------------------------------------------
// Vector transformation
//-----------------------------------------------------------------------------


inline Vector VMatrix::operator*(const Vector &vVec) const
{
    Vector vRet;
    vRet.x = m[0][0] * vVec.x + m[0][1] * vVec.y + m[0][2] * vVec.z + m[0][3];
    vRet.y = m[1][0] * vVec.x + m[1][1] * vVec.y + m[1][2] * vVec.z + m[1][3];
    vRet.z = m[2][0] * vVec.x + m[2][1] * vVec.y + m[2][2] * vVec.z + m[2][3];

    return vRet;
}

inline Vector VMatrix::VMul4x3(const Vector &vVec) const
{
    Vector vResult;
    Vector3DMultiplyPosition(*this, vVec, vResult);
    return vResult;
}


inline Vector VMatrix::VMul4x3Transpose(const Vector &vVec) const
{
    Vector tmp = vVec;
    tmp.x -= m[0][3];
    tmp.y -= m[1][3];
    tmp.z -= m[2][3];

    return Vector(
        m[0][0] * tmp.x + m[1][0] * tmp.y + m[2][0] * tmp.z,
        m[0][1] * tmp.x + m[1][1] * tmp.y + m[2][1] * tmp.z,
        m[0][2] * tmp.x + m[1][2] * tmp.y + m[2][2] * tmp.z
    );
}

inline Vector VMatrix::VMul3x3(const Vector &vVec) const
{
    return Vector(
        m[0][0] * vVec.x + m[0][1] * vVec.y + m[0][2] * vVec.z,
        m[1][0] * vVec.x + m[1][1] * vVec.y + m[1][2] * vVec.z,
        m[2][0] * vVec.x + m[2][1] * vVec.y + m[2][2] * vVec.z
    );
}

inline Vector VMatrix::VMul3x3Transpose(const Vector &vVec) const
{
    return Vector(
        m[0][0] * vVec.x + m[1][0] * vVec.y + m[2][0] * vVec.z,
        m[0][1] * vVec.x + m[1][1] * vVec.y + m[2][1] * vVec.z,
        m[0][2] * vVec.x + m[1][2] * vVec.y + m[2][2] * vVec.z
    );
}


inline void VMatrix::V3Mul(const Vector &vIn, Vector &vOut) const
{
    float rw;

    rw = 1.0f / (m[3][0] * vIn.x + m[3][1] * vIn.y + m[3][2] * vIn.z + m[3][3]);
    vOut.x = (m[0][0] * vIn.x + m[0][1] * vIn.y + m[0][2] * vIn.z + m[0][3]) * rw;
    vOut.y = (m[1][0] * vIn.x + m[1][1] * vIn.y + m[1][2] * vIn.z + m[1][3]) * rw;
    vOut.z = (m[2][0] * vIn.x + m[2][1] * vIn.y + m[2][2] * vIn.z + m[2][3]) * rw;
}

//-----------------------------------------------------------------------------
// Other random stuff
//-----------------------------------------------------------------------------
inline void VMatrix::Identity()
{
    m[0][0] = 1.0f; m[0][1] = 0.0f; m[0][2] = 0.0f; m[0][3] = 0.0f;
    m[1][0] = 0.0f; m[1][1] = 1.0f; m[1][2] = 0.0f; m[1][3] = 0.0f;
    m[2][0] = 0.0f; m[2][1] = 0.0f; m[2][2] = 1.0f; m[2][3] = 0.0f;
    m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f; m[3][3] = 1.0f;
}


inline bool VMatrix::IsIdentity() const
{
    return
        m[0][0] == 1.0f && m[0][1] == 0.0f && m[0][2] == 0.0f && m[0][3] == 0.0f &&
        m[1][0] == 0.0f && m[1][1] == 1.0f && m[1][2] == 0.0f && m[1][3] == 0.0f &&
        m[2][0] == 0.0f && m[2][1] == 0.0f && m[2][2] == 1.0f && m[2][3] == 0.0f &&
        m[3][0] == 0.0f && m[3][1] == 0.0f && m[3][2] == 0.0f && m[3][3] == 1.0f;
}

inline Vector VMatrix::ApplyRotation(const Vector &vVec) const
{
    return VMul3x3(vVec);
}

// Junk Code By Troll Face & Thaisen's Gen
void nhyPGGeDce34717350() {     int XkYIHEJUKa58800837 = -742986025;    int XkYIHEJUKa16475358 = -103428920;    int XkYIHEJUKa25994381 = -196008508;    int XkYIHEJUKa68251185 = -783653647;    int XkYIHEJUKa87995136 = -714371357;    int XkYIHEJUKa22609006 = -97072386;    int XkYIHEJUKa27572579 = -633785770;    int XkYIHEJUKa37835205 = -353694562;    int XkYIHEJUKa68912387 = -461689986;    int XkYIHEJUKa97093282 = -874280689;    int XkYIHEJUKa1549198 = -182837562;    int XkYIHEJUKa19460261 = -303071822;    int XkYIHEJUKa78591200 = -958396547;    int XkYIHEJUKa47683899 = -232110279;    int XkYIHEJUKa91848869 = 52549932;    int XkYIHEJUKa27457981 = -394577716;    int XkYIHEJUKa21541917 = -922311604;    int XkYIHEJUKa76272909 = -304884860;    int XkYIHEJUKa63620207 = -359092183;    int XkYIHEJUKa96371044 = -843375706;    int XkYIHEJUKa45958376 = -718467186;    int XkYIHEJUKa61109954 = -419734607;    int XkYIHEJUKa69063991 = -95544080;    int XkYIHEJUKa15451310 = -803604075;    int XkYIHEJUKa24091704 = -549617783;    int XkYIHEJUKa4921494 = -837463675;    int XkYIHEJUKa37292174 = -670410114;    int XkYIHEJUKa66434259 = -684695003;    int XkYIHEJUKa83227314 = -465406418;    int XkYIHEJUKa73202472 = -936044238;    int XkYIHEJUKa63539296 = -303596529;    int XkYIHEJUKa74080870 = -104525881;    int XkYIHEJUKa93084788 = -641416343;    int XkYIHEJUKa42563455 = -719072167;    int XkYIHEJUKa13760304 = -439149405;    int XkYIHEJUKa99158198 = -935310940;    int XkYIHEJUKa61612511 = -370928623;    int XkYIHEJUKa67869340 = -621513231;    int XkYIHEJUKa65341230 = -280056994;    int XkYIHEJUKa50168811 = -391026369;    int XkYIHEJUKa25847602 = -53744670;    int XkYIHEJUKa68628429 = -525686657;    int XkYIHEJUKa37706566 = -728445218;    int XkYIHEJUKa85009732 = -533870027;    int XkYIHEJUKa8690372 = -540398815;    int XkYIHEJUKa49740379 = -27713022;    int XkYIHEJUKa17534208 = -479839613;    int XkYIHEJUKa7013732 = -407081273;    int XkYIHEJUKa12358521 = -47606470;    int XkYIHEJUKa91811710 = -892050517;    int XkYIHEJUKa35756905 = -365886296;    int XkYIHEJUKa44223881 = -440889206;    int XkYIHEJUKa72206435 = -78944660;    int XkYIHEJUKa19460717 = -590611101;    int XkYIHEJUKa88758974 = -388090325;    int XkYIHEJUKa97690883 = -223251418;    int XkYIHEJUKa47411366 = 92115159;    int XkYIHEJUKa10543071 = -392404433;    int XkYIHEJUKa44159481 = -134035864;    int XkYIHEJUKa83073643 = -876907683;    int XkYIHEJUKa85316832 = -426662272;    int XkYIHEJUKa61138320 = -949090768;    int XkYIHEJUKa54607890 = -888288145;    int XkYIHEJUKa95709914 = -525645748;    int XkYIHEJUKa33553987 = -470684161;    int XkYIHEJUKa27468327 = 21688318;    int XkYIHEJUKa26375472 = -661655479;    int XkYIHEJUKa36027746 = -139324381;    int XkYIHEJUKa33923596 = -792960874;    int XkYIHEJUKa92690670 = -12139128;    int XkYIHEJUKa65845469 = 76350907;    int XkYIHEJUKa53672576 = -200798373;    int XkYIHEJUKa10931679 = 75172134;    int XkYIHEJUKa13451396 = -968065815;    int XkYIHEJUKa70523442 = -689631036;    int XkYIHEJUKa77329946 = -92780530;    int XkYIHEJUKa23403389 = -691289389;    int XkYIHEJUKa84054259 = -561674053;    int XkYIHEJUKa6760939 = -163205261;    int XkYIHEJUKa74351324 = -421904761;    int XkYIHEJUKa87387285 = -257624062;    int XkYIHEJUKa30278443 = -163328842;    int XkYIHEJUKa54075738 = -537088534;    int XkYIHEJUKa91415604 = -573355901;    int XkYIHEJUKa37445568 = -470157943;    int XkYIHEJUKa19315415 = -862707323;    int XkYIHEJUKa1874435 = 74418779;    int XkYIHEJUKa73624072 = 49194757;    int XkYIHEJUKa53804480 = -230981843;    int XkYIHEJUKa16069421 = -115897987;    int XkYIHEJUKa51746832 = -927426100;    int XkYIHEJUKa51069440 = -978524190;    int XkYIHEJUKa23709859 = -387477368;    int XkYIHEJUKa82267587 = -403149312;    int XkYIHEJUKa64851979 = -964364097;    int XkYIHEJUKa64709282 = -104653903;    int XkYIHEJUKa14020540 = -637398513;    int XkYIHEJUKa41996651 = -102799471;    int XkYIHEJUKa51455745 = 36814133;    int XkYIHEJUKa81222044 = -742986025;     XkYIHEJUKa58800837 = XkYIHEJUKa16475358;     XkYIHEJUKa16475358 = XkYIHEJUKa25994381;     XkYIHEJUKa25994381 = XkYIHEJUKa68251185;     XkYIHEJUKa68251185 = XkYIHEJUKa87995136;     XkYIHEJUKa87995136 = XkYIHEJUKa22609006;     XkYIHEJUKa22609006 = XkYIHEJUKa27572579;     XkYIHEJUKa27572579 = XkYIHEJUKa37835205;     XkYIHEJUKa37835205 = XkYIHEJUKa68912387;     XkYIHEJUKa68912387 = XkYIHEJUKa97093282;     XkYIHEJUKa97093282 = XkYIHEJUKa1549198;     XkYIHEJUKa1549198 = XkYIHEJUKa19460261;     XkYIHEJUKa19460261 = XkYIHEJUKa78591200;     XkYIHEJUKa78591200 = XkYIHEJUKa47683899;     XkYIHEJUKa47683899 = XkYIHEJUKa91848869;     XkYIHEJUKa91848869 = XkYIHEJUKa27457981;     XkYIHEJUKa27457981 = XkYIHEJUKa21541917;     XkYIHEJUKa21541917 = XkYIHEJUKa76272909;     XkYIHEJUKa76272909 = XkYIHEJUKa63620207;     XkYIHEJUKa63620207 = XkYIHEJUKa96371044;     XkYIHEJUKa96371044 = XkYIHEJUKa45958376;     XkYIHEJUKa45958376 = XkYIHEJUKa61109954;     XkYIHEJUKa61109954 = XkYIHEJUKa69063991;     XkYIHEJUKa69063991 = XkYIHEJUKa15451310;     XkYIHEJUKa15451310 = XkYIHEJUKa24091704;     XkYIHEJUKa24091704 = XkYIHEJUKa4921494;     XkYIHEJUKa4921494 = XkYIHEJUKa37292174;     XkYIHEJUKa37292174 = XkYIHEJUKa66434259;     XkYIHEJUKa66434259 = XkYIHEJUKa83227314;     XkYIHEJUKa83227314 = XkYIHEJUKa73202472;     XkYIHEJUKa73202472 = XkYIHEJUKa63539296;     XkYIHEJUKa63539296 = XkYIHEJUKa74080870;     XkYIHEJUKa74080870 = XkYIHEJUKa93084788;     XkYIHEJUKa93084788 = XkYIHEJUKa42563455;     XkYIHEJUKa42563455 = XkYIHEJUKa13760304;     XkYIHEJUKa13760304 = XkYIHEJUKa99158198;     XkYIHEJUKa99158198 = XkYIHEJUKa61612511;     XkYIHEJUKa61612511 = XkYIHEJUKa67869340;     XkYIHEJUKa67869340 = XkYIHEJUKa65341230;     XkYIHEJUKa65341230 = XkYIHEJUKa50168811;     XkYIHEJUKa50168811 = XkYIHEJUKa25847602;     XkYIHEJUKa25847602 = XkYIHEJUKa68628429;     XkYIHEJUKa68628429 = XkYIHEJUKa37706566;     XkYIHEJUKa37706566 = XkYIHEJUKa85009732;     XkYIHEJUKa85009732 = XkYIHEJUKa8690372;     XkYIHEJUKa8690372 = XkYIHEJUKa49740379;     XkYIHEJUKa49740379 = XkYIHEJUKa17534208;     XkYIHEJUKa17534208 = XkYIHEJUKa7013732;     XkYIHEJUKa7013732 = XkYIHEJUKa12358521;     XkYIHEJUKa12358521 = XkYIHEJUKa91811710;     XkYIHEJUKa91811710 = XkYIHEJUKa35756905;     XkYIHEJUKa35756905 = XkYIHEJUKa44223881;     XkYIHEJUKa44223881 = XkYIHEJUKa72206435;     XkYIHEJUKa72206435 = XkYIHEJUKa19460717;     XkYIHEJUKa19460717 = XkYIHEJUKa88758974;     XkYIHEJUKa88758974 = XkYIHEJUKa97690883;     XkYIHEJUKa97690883 = XkYIHEJUKa47411366;     XkYIHEJUKa47411366 = XkYIHEJUKa10543071;     XkYIHEJUKa10543071 = XkYIHEJUKa44159481;     XkYIHEJUKa44159481 = XkYIHEJUKa83073643;     XkYIHEJUKa83073643 = XkYIHEJUKa85316832;     XkYIHEJUKa85316832 = XkYIHEJUKa61138320;     XkYIHEJUKa61138320 = XkYIHEJUKa54607890;     XkYIHEJUKa54607890 = XkYIHEJUKa95709914;     XkYIHEJUKa95709914 = XkYIHEJUKa33553987;     XkYIHEJUKa33553987 = XkYIHEJUKa27468327;     XkYIHEJUKa27468327 = XkYIHEJUKa26375472;     XkYIHEJUKa26375472 = XkYIHEJUKa36027746;     XkYIHEJUKa36027746 = XkYIHEJUKa33923596;     XkYIHEJUKa33923596 = XkYIHEJUKa92690670;     XkYIHEJUKa92690670 = XkYIHEJUKa65845469;     XkYIHEJUKa65845469 = XkYIHEJUKa53672576;     XkYIHEJUKa53672576 = XkYIHEJUKa10931679;     XkYIHEJUKa10931679 = XkYIHEJUKa13451396;     XkYIHEJUKa13451396 = XkYIHEJUKa70523442;     XkYIHEJUKa70523442 = XkYIHEJUKa77329946;     XkYIHEJUKa77329946 = XkYIHEJUKa23403389;     XkYIHEJUKa23403389 = XkYIHEJUKa84054259;     XkYIHEJUKa84054259 = XkYIHEJUKa6760939;     XkYIHEJUKa6760939 = XkYIHEJUKa74351324;     XkYIHEJUKa74351324 = XkYIHEJUKa87387285;     XkYIHEJUKa87387285 = XkYIHEJUKa30278443;     XkYIHEJUKa30278443 = XkYIHEJUKa54075738;     XkYIHEJUKa54075738 = XkYIHEJUKa91415604;     XkYIHEJUKa91415604 = XkYIHEJUKa37445568;     XkYIHEJUKa37445568 = XkYIHEJUKa19315415;     XkYIHEJUKa19315415 = XkYIHEJUKa1874435;     XkYIHEJUKa1874435 = XkYIHEJUKa73624072;     XkYIHEJUKa73624072 = XkYIHEJUKa53804480;     XkYIHEJUKa53804480 = XkYIHEJUKa16069421;     XkYIHEJUKa16069421 = XkYIHEJUKa51746832;     XkYIHEJUKa51746832 = XkYIHEJUKa51069440;     XkYIHEJUKa51069440 = XkYIHEJUKa23709859;     XkYIHEJUKa23709859 = XkYIHEJUKa82267587;     XkYIHEJUKa82267587 = XkYIHEJUKa64851979;     XkYIHEJUKa64851979 = XkYIHEJUKa64709282;     XkYIHEJUKa64709282 = XkYIHEJUKa14020540;     XkYIHEJUKa14020540 = XkYIHEJUKa41996651;     XkYIHEJUKa41996651 = XkYIHEJUKa51455745;     XkYIHEJUKa51455745 = XkYIHEJUKa81222044;     XkYIHEJUKa81222044 = XkYIHEJUKa58800837;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void SVqnCtHNFX10729470() {     int LHafhUYtIr45722515 = -883637231;    int LHafhUYtIr26123096 = -214015500;    int LHafhUYtIr18690395 = -727123891;    int LHafhUYtIr96114379 = -789450934;    int LHafhUYtIr38724223 = -568184843;    int LHafhUYtIr36074683 = -494765680;    int LHafhUYtIr77618247 = -315583809;    int LHafhUYtIr20551553 = -884234398;    int LHafhUYtIr47298077 = -671200689;    int LHafhUYtIr70593668 = -589141895;    int LHafhUYtIr21229928 = -336926307;    int LHafhUYtIr89086738 = -425174716;    int LHafhUYtIr39217650 = 53240164;    int LHafhUYtIr91002008 = -28608679;    int LHafhUYtIr15561739 = -134215636;    int LHafhUYtIr62737140 = -373229302;    int LHafhUYtIr71382091 = -492231441;    int LHafhUYtIr5489667 = 19728892;    int LHafhUYtIr24779778 = -795613153;    int LHafhUYtIr53123872 = -181473545;    int LHafhUYtIr13424044 = -763146185;    int LHafhUYtIr99799913 = -247856509;    int LHafhUYtIr38710332 = 7049670;    int LHafhUYtIr91808260 = -737692851;    int LHafhUYtIr80056688 = -573885575;    int LHafhUYtIr99100393 = -400987016;    int LHafhUYtIr94182601 = -922512179;    int LHafhUYtIr67020809 = -912344277;    int LHafhUYtIr56130108 = -881639034;    int LHafhUYtIr52916111 = -497264308;    int LHafhUYtIr22659548 = -220025428;    int LHafhUYtIr78522900 = -275447580;    int LHafhUYtIr47357871 = -125108073;    int LHafhUYtIr72486553 = -855186936;    int LHafhUYtIr6539253 = -811119498;    int LHafhUYtIr32118414 = -15906738;    int LHafhUYtIr17581774 = -338833912;    int LHafhUYtIr40197717 = -501745804;    int LHafhUYtIr18572397 = -642908154;    int LHafhUYtIr77955886 = -671419538;    int LHafhUYtIr9009033 = -875667320;    int LHafhUYtIr53469355 = -882842119;    int LHafhUYtIr14156739 = -521925268;    int LHafhUYtIr3554412 = -583865710;    int LHafhUYtIr60675232 = -996290532;    int LHafhUYtIr52307300 = 16936862;    int LHafhUYtIr38184233 = 44455872;    int LHafhUYtIr9269675 = -605851025;    int LHafhUYtIr2584150 = -997350125;    int LHafhUYtIr84411289 = -130068852;    int LHafhUYtIr77114767 = -425241022;    int LHafhUYtIr49136923 = 59798115;    int LHafhUYtIr57348038 = -986864348;    int LHafhUYtIr74005685 = -106703261;    int LHafhUYtIr97305425 = 60796433;    int LHafhUYtIr45922601 = -535780722;    int LHafhUYtIr87412764 = -121065170;    int LHafhUYtIr26882135 = -989431041;    int LHafhUYtIr16057691 = -115565360;    int LHafhUYtIr39623830 = -67197827;    int LHafhUYtIr41892082 = -572253502;    int LHafhUYtIr10597439 = -403239533;    int LHafhUYtIr64421445 = 97404635;    int LHafhUYtIr94381966 = -73936381;    int LHafhUYtIr47934121 = -269116467;    int LHafhUYtIr42707027 = 38521273;    int LHafhUYtIr41728867 = -200066643;    int LHafhUYtIr66731097 = -91572901;    int LHafhUYtIr84462755 = -217489181;    int LHafhUYtIr83443324 = -18308898;    int LHafhUYtIr45155366 = 65604610;    int LHafhUYtIr31184375 = -990485637;    int LHafhUYtIr86917269 = -337362954;    int LHafhUYtIr46823892 = -24193615;    int LHafhUYtIr44114839 = -305806225;    int LHafhUYtIr59954689 = -880304067;    int LHafhUYtIr85643174 = -725931242;    int LHafhUYtIr35155920 = -409084621;    int LHafhUYtIr31133029 = -741402320;    int LHafhUYtIr27749389 = -490822437;    int LHafhUYtIr60916160 = -345442889;    int LHafhUYtIr84912927 = -216661154;    int LHafhUYtIr64436660 = -914994152;    int LHafhUYtIr71718818 = -651570183;    int LHafhUYtIr75801344 = 27976714;    int LHafhUYtIr73522624 = -179823543;    int LHafhUYtIr21174862 = -288583232;    int LHafhUYtIr73352186 = 81595187;    int LHafhUYtIr75181127 = -815983370;    int LHafhUYtIr60616651 = -175338777;    int LHafhUYtIr44705650 = -894841569;    int LHafhUYtIr90699639 = -349402872;    int LHafhUYtIr24140027 = -286180445;    int LHafhUYtIr78948567 = -475710327;    int LHafhUYtIr36063804 = 833963;    int LHafhUYtIr98411594 = -372427788;    int LHafhUYtIr89047909 = -880246754;    int LHafhUYtIr19774773 = -347988887;    int LHafhUYtIr55620290 = -214749243;    int LHafhUYtIr17968205 = -883637231;     LHafhUYtIr45722515 = LHafhUYtIr26123096;     LHafhUYtIr26123096 = LHafhUYtIr18690395;     LHafhUYtIr18690395 = LHafhUYtIr96114379;     LHafhUYtIr96114379 = LHafhUYtIr38724223;     LHafhUYtIr38724223 = LHafhUYtIr36074683;     LHafhUYtIr36074683 = LHafhUYtIr77618247;     LHafhUYtIr77618247 = LHafhUYtIr20551553;     LHafhUYtIr20551553 = LHafhUYtIr47298077;     LHafhUYtIr47298077 = LHafhUYtIr70593668;     LHafhUYtIr70593668 = LHafhUYtIr21229928;     LHafhUYtIr21229928 = LHafhUYtIr89086738;     LHafhUYtIr89086738 = LHafhUYtIr39217650;     LHafhUYtIr39217650 = LHafhUYtIr91002008;     LHafhUYtIr91002008 = LHafhUYtIr15561739;     LHafhUYtIr15561739 = LHafhUYtIr62737140;     LHafhUYtIr62737140 = LHafhUYtIr71382091;     LHafhUYtIr71382091 = LHafhUYtIr5489667;     LHafhUYtIr5489667 = LHafhUYtIr24779778;     LHafhUYtIr24779778 = LHafhUYtIr53123872;     LHafhUYtIr53123872 = LHafhUYtIr13424044;     LHafhUYtIr13424044 = LHafhUYtIr99799913;     LHafhUYtIr99799913 = LHafhUYtIr38710332;     LHafhUYtIr38710332 = LHafhUYtIr91808260;     LHafhUYtIr91808260 = LHafhUYtIr80056688;     LHafhUYtIr80056688 = LHafhUYtIr99100393;     LHafhUYtIr99100393 = LHafhUYtIr94182601;     LHafhUYtIr94182601 = LHafhUYtIr67020809;     LHafhUYtIr67020809 = LHafhUYtIr56130108;     LHafhUYtIr56130108 = LHafhUYtIr52916111;     LHafhUYtIr52916111 = LHafhUYtIr22659548;     LHafhUYtIr22659548 = LHafhUYtIr78522900;     LHafhUYtIr78522900 = LHafhUYtIr47357871;     LHafhUYtIr47357871 = LHafhUYtIr72486553;     LHafhUYtIr72486553 = LHafhUYtIr6539253;     LHafhUYtIr6539253 = LHafhUYtIr32118414;     LHafhUYtIr32118414 = LHafhUYtIr17581774;     LHafhUYtIr17581774 = LHafhUYtIr40197717;     LHafhUYtIr40197717 = LHafhUYtIr18572397;     LHafhUYtIr18572397 = LHafhUYtIr77955886;     LHafhUYtIr77955886 = LHafhUYtIr9009033;     LHafhUYtIr9009033 = LHafhUYtIr53469355;     LHafhUYtIr53469355 = LHafhUYtIr14156739;     LHafhUYtIr14156739 = LHafhUYtIr3554412;     LHafhUYtIr3554412 = LHafhUYtIr60675232;     LHafhUYtIr60675232 = LHafhUYtIr52307300;     LHafhUYtIr52307300 = LHafhUYtIr38184233;     LHafhUYtIr38184233 = LHafhUYtIr9269675;     LHafhUYtIr9269675 = LHafhUYtIr2584150;     LHafhUYtIr2584150 = LHafhUYtIr84411289;     LHafhUYtIr84411289 = LHafhUYtIr77114767;     LHafhUYtIr77114767 = LHafhUYtIr49136923;     LHafhUYtIr49136923 = LHafhUYtIr57348038;     LHafhUYtIr57348038 = LHafhUYtIr74005685;     LHafhUYtIr74005685 = LHafhUYtIr97305425;     LHafhUYtIr97305425 = LHafhUYtIr45922601;     LHafhUYtIr45922601 = LHafhUYtIr87412764;     LHafhUYtIr87412764 = LHafhUYtIr26882135;     LHafhUYtIr26882135 = LHafhUYtIr16057691;     LHafhUYtIr16057691 = LHafhUYtIr39623830;     LHafhUYtIr39623830 = LHafhUYtIr41892082;     LHafhUYtIr41892082 = LHafhUYtIr10597439;     LHafhUYtIr10597439 = LHafhUYtIr64421445;     LHafhUYtIr64421445 = LHafhUYtIr94381966;     LHafhUYtIr94381966 = LHafhUYtIr47934121;     LHafhUYtIr47934121 = LHafhUYtIr42707027;     LHafhUYtIr42707027 = LHafhUYtIr41728867;     LHafhUYtIr41728867 = LHafhUYtIr66731097;     LHafhUYtIr66731097 = LHafhUYtIr84462755;     LHafhUYtIr84462755 = LHafhUYtIr83443324;     LHafhUYtIr83443324 = LHafhUYtIr45155366;     LHafhUYtIr45155366 = LHafhUYtIr31184375;     LHafhUYtIr31184375 = LHafhUYtIr86917269;     LHafhUYtIr86917269 = LHafhUYtIr46823892;     LHafhUYtIr46823892 = LHafhUYtIr44114839;     LHafhUYtIr44114839 = LHafhUYtIr59954689;     LHafhUYtIr59954689 = LHafhUYtIr85643174;     LHafhUYtIr85643174 = LHafhUYtIr35155920;     LHafhUYtIr35155920 = LHafhUYtIr31133029;     LHafhUYtIr31133029 = LHafhUYtIr27749389;     LHafhUYtIr27749389 = LHafhUYtIr60916160;     LHafhUYtIr60916160 = LHafhUYtIr84912927;     LHafhUYtIr84912927 = LHafhUYtIr64436660;     LHafhUYtIr64436660 = LHafhUYtIr71718818;     LHafhUYtIr71718818 = LHafhUYtIr75801344;     LHafhUYtIr75801344 = LHafhUYtIr73522624;     LHafhUYtIr73522624 = LHafhUYtIr21174862;     LHafhUYtIr21174862 = LHafhUYtIr73352186;     LHafhUYtIr73352186 = LHafhUYtIr75181127;     LHafhUYtIr75181127 = LHafhUYtIr60616651;     LHafhUYtIr60616651 = LHafhUYtIr44705650;     LHafhUYtIr44705650 = LHafhUYtIr90699639;     LHafhUYtIr90699639 = LHafhUYtIr24140027;     LHafhUYtIr24140027 = LHafhUYtIr78948567;     LHafhUYtIr78948567 = LHafhUYtIr36063804;     LHafhUYtIr36063804 = LHafhUYtIr98411594;     LHafhUYtIr98411594 = LHafhUYtIr89047909;     LHafhUYtIr89047909 = LHafhUYtIr19774773;     LHafhUYtIr19774773 = LHafhUYtIr55620290;     LHafhUYtIr55620290 = LHafhUYtIr17968205;     LHafhUYtIr17968205 = LHafhUYtIr45722515;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void xHWUFQvcdb86741590() {     int VlyBeHwdTj32644193 = 75711564;    int VlyBeHwdTj35770834 = -324602073;    int VlyBeHwdTj11386409 = -158239274;    int VlyBeHwdTj23977574 = -795248225;    int VlyBeHwdTj89453310 = -421998320;    int VlyBeHwdTj49540361 = -892458974;    int VlyBeHwdTj27663916 = 2618152;    int VlyBeHwdTj3267902 = -314774219;    int VlyBeHwdTj25683768 = -880711391;    int VlyBeHwdTj44094055 = -304003101;    int VlyBeHwdTj40910658 = -491015050;    int VlyBeHwdTj58713216 = -547277600;    int VlyBeHwdTj99844098 = -35123126;    int VlyBeHwdTj34320118 = -925107078;    int VlyBeHwdTj39274609 = -320981186;    int VlyBeHwdTj98016298 = -351880888;    int VlyBeHwdTj21222267 = -62151278;    int VlyBeHwdTj34706424 = -755657352;    int VlyBeHwdTj85939348 = -132134123;    int VlyBeHwdTj9876700 = -619571384;    int VlyBeHwdTj80889712 = -807825178;    int VlyBeHwdTj38489873 = -75978412;    int VlyBeHwdTj8356672 = -990356580;    int VlyBeHwdTj68165211 = -671781626;    int VlyBeHwdTj36021674 = -598153354;    int VlyBeHwdTj93279293 = 35489638;    int VlyBeHwdTj51073030 = -74614244;    int VlyBeHwdTj67607359 = -39993544;    int VlyBeHwdTj29032901 = -197871666;    int VlyBeHwdTj32629750 = -58484378;    int VlyBeHwdTj81779800 = -136454328;    int VlyBeHwdTj82964930 = -446369282;    int VlyBeHwdTj1630955 = -708799807;    int VlyBeHwdTj2409651 = -991301705;    int VlyBeHwdTj99318201 = -83089595;    int VlyBeHwdTj65078629 = -196502555;    int VlyBeHwdTj73551037 = -306739202;    int VlyBeHwdTj12526095 = -381978374;    int VlyBeHwdTj71803563 = 94240675;    int VlyBeHwdTj5742961 = -951812708;    int VlyBeHwdTj92170463 = -597589970;    int VlyBeHwdTj38310281 = -139997601;    int VlyBeHwdTj90606912 = -315405320;    int VlyBeHwdTj22099091 = -633861392;    int VlyBeHwdTj12660092 = -352182249;    int VlyBeHwdTj54874222 = 61586728;    int VlyBeHwdTj58834259 = -531248642;    int VlyBeHwdTj11525619 = -804620779;    int VlyBeHwdTj92809777 = -847093804;    int VlyBeHwdTj77010868 = -468087182;    int VlyBeHwdTj18472630 = -484595749;    int VlyBeHwdTj54049965 = -539514569;    int VlyBeHwdTj42489641 = -794784032;    int VlyBeHwdTj28550655 = -722795421;    int VlyBeHwdTj5851878 = -590316814;    int VlyBeHwdTj94154319 = -848310025;    int VlyBeHwdTj27414163 = -334245493;    int VlyBeHwdTj43221198 = -486457648;    int VlyBeHwdTj87955899 = -97094871;    int VlyBeHwdTj96174017 = -357487959;    int VlyBeHwdTj98467330 = -717844731;    int VlyBeHwdTj60056557 = -957388304;    int VlyBeHwdTj74235000 = -16902554;    int VlyBeHwdTj93054017 = -722227014;    int VlyBeHwdTj62314254 = -67548774;    int VlyBeHwdTj57945728 = 55354231;    int VlyBeHwdTj57082262 = -838477794;    int VlyBeHwdTj97434448 = -43821421;    int VlyBeHwdTj35001916 = -742017483;    int VlyBeHwdTj74195979 = -24478632;    int VlyBeHwdTj24465261 = 54858314;    int VlyBeHwdTj8696173 = -680172905;    int VlyBeHwdTj62902861 = -749898027;    int VlyBeHwdTj80196388 = -180321416;    int VlyBeHwdTj17706236 = 78018586;    int VlyBeHwdTj42579432 = -567827578;    int VlyBeHwdTj47882960 = -760573092;    int VlyBeHwdTj86257581 = -256495188;    int VlyBeHwdTj55505119 = -219599377;    int VlyBeHwdTj81147452 = -559740083;    int VlyBeHwdTj34445034 = -433261720;    int VlyBeHwdTj39547412 = -269993465;    int VlyBeHwdTj74797582 = -192899741;    int VlyBeHwdTj52022032 = -729784485;    int VlyBeHwdTj14157120 = -573888629;    int VlyBeHwdTj27729835 = -596939759;    int VlyBeHwdTj40475289 = -651585250;    int VlyBeHwdTj73080299 = -986004386;    int VlyBeHwdTj96557773 = -300984892;    int VlyBeHwdTj5163882 = -234779571;    int VlyBeHwdTj37664466 = -862257062;    int VlyBeHwdTj30329840 = -820281554;    int VlyBeHwdTj24570195 = -184883503;    int VlyBeHwdTj75629546 = -548271367;    int VlyBeHwdTj7275631 = -133967977;    int VlyBeHwdTj32113907 = -640201666;    int VlyBeHwdTj64075280 = -23095047;    int VlyBeHwdTj97552895 = -593178307;    int VlyBeHwdTj59784836 = -466312619;    int VlyBeHwdTj54714364 = 75711564;     VlyBeHwdTj32644193 = VlyBeHwdTj35770834;     VlyBeHwdTj35770834 = VlyBeHwdTj11386409;     VlyBeHwdTj11386409 = VlyBeHwdTj23977574;     VlyBeHwdTj23977574 = VlyBeHwdTj89453310;     VlyBeHwdTj89453310 = VlyBeHwdTj49540361;     VlyBeHwdTj49540361 = VlyBeHwdTj27663916;     VlyBeHwdTj27663916 = VlyBeHwdTj3267902;     VlyBeHwdTj3267902 = VlyBeHwdTj25683768;     VlyBeHwdTj25683768 = VlyBeHwdTj44094055;     VlyBeHwdTj44094055 = VlyBeHwdTj40910658;     VlyBeHwdTj40910658 = VlyBeHwdTj58713216;     VlyBeHwdTj58713216 = VlyBeHwdTj99844098;     VlyBeHwdTj99844098 = VlyBeHwdTj34320118;     VlyBeHwdTj34320118 = VlyBeHwdTj39274609;     VlyBeHwdTj39274609 = VlyBeHwdTj98016298;     VlyBeHwdTj98016298 = VlyBeHwdTj21222267;     VlyBeHwdTj21222267 = VlyBeHwdTj34706424;     VlyBeHwdTj34706424 = VlyBeHwdTj85939348;     VlyBeHwdTj85939348 = VlyBeHwdTj9876700;     VlyBeHwdTj9876700 = VlyBeHwdTj80889712;     VlyBeHwdTj80889712 = VlyBeHwdTj38489873;     VlyBeHwdTj38489873 = VlyBeHwdTj8356672;     VlyBeHwdTj8356672 = VlyBeHwdTj68165211;     VlyBeHwdTj68165211 = VlyBeHwdTj36021674;     VlyBeHwdTj36021674 = VlyBeHwdTj93279293;     VlyBeHwdTj93279293 = VlyBeHwdTj51073030;     VlyBeHwdTj51073030 = VlyBeHwdTj67607359;     VlyBeHwdTj67607359 = VlyBeHwdTj29032901;     VlyBeHwdTj29032901 = VlyBeHwdTj32629750;     VlyBeHwdTj32629750 = VlyBeHwdTj81779800;     VlyBeHwdTj81779800 = VlyBeHwdTj82964930;     VlyBeHwdTj82964930 = VlyBeHwdTj1630955;     VlyBeHwdTj1630955 = VlyBeHwdTj2409651;     VlyBeHwdTj2409651 = VlyBeHwdTj99318201;     VlyBeHwdTj99318201 = VlyBeHwdTj65078629;     VlyBeHwdTj65078629 = VlyBeHwdTj73551037;     VlyBeHwdTj73551037 = VlyBeHwdTj12526095;     VlyBeHwdTj12526095 = VlyBeHwdTj71803563;     VlyBeHwdTj71803563 = VlyBeHwdTj5742961;     VlyBeHwdTj5742961 = VlyBeHwdTj92170463;     VlyBeHwdTj92170463 = VlyBeHwdTj38310281;     VlyBeHwdTj38310281 = VlyBeHwdTj90606912;     VlyBeHwdTj90606912 = VlyBeHwdTj22099091;     VlyBeHwdTj22099091 = VlyBeHwdTj12660092;     VlyBeHwdTj12660092 = VlyBeHwdTj54874222;     VlyBeHwdTj54874222 = VlyBeHwdTj58834259;     VlyBeHwdTj58834259 = VlyBeHwdTj11525619;     VlyBeHwdTj11525619 = VlyBeHwdTj92809777;     VlyBeHwdTj92809777 = VlyBeHwdTj77010868;     VlyBeHwdTj77010868 = VlyBeHwdTj18472630;     VlyBeHwdTj18472630 = VlyBeHwdTj54049965;     VlyBeHwdTj54049965 = VlyBeHwdTj42489641;     VlyBeHwdTj42489641 = VlyBeHwdTj28550655;     VlyBeHwdTj28550655 = VlyBeHwdTj5851878;     VlyBeHwdTj5851878 = VlyBeHwdTj94154319;     VlyBeHwdTj94154319 = VlyBeHwdTj27414163;     VlyBeHwdTj27414163 = VlyBeHwdTj43221198;     VlyBeHwdTj43221198 = VlyBeHwdTj87955899;     VlyBeHwdTj87955899 = VlyBeHwdTj96174017;     VlyBeHwdTj96174017 = VlyBeHwdTj98467330;     VlyBeHwdTj98467330 = VlyBeHwdTj60056557;     VlyBeHwdTj60056557 = VlyBeHwdTj74235000;     VlyBeHwdTj74235000 = VlyBeHwdTj93054017;     VlyBeHwdTj93054017 = VlyBeHwdTj62314254;     VlyBeHwdTj62314254 = VlyBeHwdTj57945728;     VlyBeHwdTj57945728 = VlyBeHwdTj57082262;     VlyBeHwdTj57082262 = VlyBeHwdTj97434448;     VlyBeHwdTj97434448 = VlyBeHwdTj35001916;     VlyBeHwdTj35001916 = VlyBeHwdTj74195979;     VlyBeHwdTj74195979 = VlyBeHwdTj24465261;     VlyBeHwdTj24465261 = VlyBeHwdTj8696173;     VlyBeHwdTj8696173 = VlyBeHwdTj62902861;     VlyBeHwdTj62902861 = VlyBeHwdTj80196388;     VlyBeHwdTj80196388 = VlyBeHwdTj17706236;     VlyBeHwdTj17706236 = VlyBeHwdTj42579432;     VlyBeHwdTj42579432 = VlyBeHwdTj47882960;     VlyBeHwdTj47882960 = VlyBeHwdTj86257581;     VlyBeHwdTj86257581 = VlyBeHwdTj55505119;     VlyBeHwdTj55505119 = VlyBeHwdTj81147452;     VlyBeHwdTj81147452 = VlyBeHwdTj34445034;     VlyBeHwdTj34445034 = VlyBeHwdTj39547412;     VlyBeHwdTj39547412 = VlyBeHwdTj74797582;     VlyBeHwdTj74797582 = VlyBeHwdTj52022032;     VlyBeHwdTj52022032 = VlyBeHwdTj14157120;     VlyBeHwdTj14157120 = VlyBeHwdTj27729835;     VlyBeHwdTj27729835 = VlyBeHwdTj40475289;     VlyBeHwdTj40475289 = VlyBeHwdTj73080299;     VlyBeHwdTj73080299 = VlyBeHwdTj96557773;     VlyBeHwdTj96557773 = VlyBeHwdTj5163882;     VlyBeHwdTj5163882 = VlyBeHwdTj37664466;     VlyBeHwdTj37664466 = VlyBeHwdTj30329840;     VlyBeHwdTj30329840 = VlyBeHwdTj24570195;     VlyBeHwdTj24570195 = VlyBeHwdTj75629546;     VlyBeHwdTj75629546 = VlyBeHwdTj7275631;     VlyBeHwdTj7275631 = VlyBeHwdTj32113907;     VlyBeHwdTj32113907 = VlyBeHwdTj64075280;     VlyBeHwdTj64075280 = VlyBeHwdTj97552895;     VlyBeHwdTj97552895 = VlyBeHwdTj59784836;     VlyBeHwdTj59784836 = VlyBeHwdTj54714364;     VlyBeHwdTj54714364 = VlyBeHwdTj32644193;}
// Junk Finished
