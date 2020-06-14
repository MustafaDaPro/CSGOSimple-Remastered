#include "checksum_md5.hpp"

#include <windows.h>
#include <stdio.h>

// The four core functions - F1 is optimized somewhat
// #define F1(x, y, z) (x & y | ~x & z)
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

// This is the central step in the MD5 algorithm.
#define MD5STEP(f, w, x, y, z, data, s) \
        ( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )

//-----------------------------------------------------------------------------
// Purpose: The core of the MD5 algorithm, this alters an existing MD5 hash to
//  reflect the addition of 16 longwords of new data.  MD5Update blocks
//  the data and converts bytes into longwords for this routine.
// Input  : buf[4] - 
//			in[16] - 
// Output : static void
//-----------------------------------------------------------------------------
static void MD5Transform(unsigned int buf[4], unsigned int const in[16])
{
    register unsigned int a, b, c, d;

    a = buf[0];
    b = buf[1];
    c = buf[2];
    d = buf[3];

    MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
    MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
    MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
    MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
    MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
    MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
    MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
    MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
    MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
    MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
    MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
    MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
    MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
    MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
    MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
    MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

    MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
    MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
    MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
    MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
    MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
    MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
    MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
    MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
    MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
    MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
    MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
    MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
    MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
    MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
    MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
    MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

    MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
    MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
    MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
    MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
    MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
    MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
    MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
    MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
    MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
    MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
    MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
    MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
    MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
    MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
    MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
    MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

    MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
    MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
    MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
    MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
    MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
    MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
    MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
    MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
    MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
    MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
    MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
    MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
    MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
    MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
    MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
    MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

    buf[0] += a;
    buf[1] += b;
    buf[2] += c;
    buf[3] += d;
}

//-----------------------------------------------------------------------------
// Purpose: Start MD5 accumulation.  Set bit count to 0 and buffer to mysterious initialization constants.

// Input  : *ctx - 
//-----------------------------------------------------------------------------
void MD5Init(MD5Context_t *ctx)
{
    ctx->buf[0] = 0x67452301;
    ctx->buf[1] = 0xefcdab89;
    ctx->buf[2] = 0x98badcfe;
    ctx->buf[3] = 0x10325476;

    ctx->bits[0] = 0;
    ctx->bits[1] = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Update context to reflect the concatenation of another buffer full of bytes.
// Input  : *ctx - 
//			*buf - 
//			len - 
//-----------------------------------------------------------------------------
void MD5Update(MD5Context_t *ctx, unsigned char const *buf, unsigned int len)
{
    unsigned int t;

    /* Update bitcount */

    t = ctx->bits[0];
    if((ctx->bits[0] = t + ((unsigned int)len << 3)) < t)
        ctx->bits[1]++;         /* Carry from low to high */
    ctx->bits[1] += len >> 29;

    t = (t >> 3) & 0x3f;        /* Bytes already in shsInfo->data */

                                /* Handle any leading odd-sized chunks */

    if(t) {
        unsigned char *p = (unsigned char *)ctx->in + t;

        t = 64 - t;
        if(len < t) {
            memcpy(p, buf, len);
            return;
        }
        memcpy(p, buf, t);
        //byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (unsigned int *)ctx->in);
        buf += t;
        len -= t;
    }
    /* Process data in 64-uint8_t chunks */

    while(len >= 64) {
        memcpy(ctx->in, buf, 64);
        //byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (unsigned int *)ctx->in);
        buf += 64;
        len -= 64;
    }

    /* Handle any remaining bytes of data. */
    memcpy(ctx->in, buf, len);
}

//-----------------------------------------------------------------------------
// Purpose: Final wrapup - pad to 64-uint8_t boundary with the bit pattern 
// 1 0* (64-bit count of bits processed, MSB-first)
// Input  : digest[MD5_DIGEST_LENGTH] - 
//			*ctx - 
//-----------------------------------------------------------------------------
void MD5Final(unsigned char digest[MD5_DIGEST_LENGTH], MD5Context_t *ctx)
{
    unsigned count;
    unsigned char *p;

    /* Compute number of bytes mod 64 */
    count = (ctx->bits[0] >> 3) & 0x3F;

    /* Set the first char of padding to 0x80.  This is safe since there is
    always at least one uint8_t free */
    p = ctx->in + count;
    *p++ = 0x80;

    /* Bytes of padding needed to make 64 bytes */
    count = 64 - 1 - count;

    /* Pad out to 56 mod 64 */
    if(count < 8) {
        /* Two lots of padding:  Pad the first block to 64 bytes */
        memset(p, 0, count);
        //byteReverse(ctx->in, 16);
        MD5Transform(ctx->buf, (unsigned int *)ctx->in);

        /* Now fill the next block with 56 bytes */
        memset(ctx->in, 0, 56);
    } else {
        /* Pad block to 56 bytes */
        memset(p, 0, count - 8);
    }
    //byteReverse(ctx->in, 14);

    /* Append length in bits and transform */
    ((unsigned int *)ctx->in)[14] = ctx->bits[0];
    ((unsigned int *)ctx->in)[15] = ctx->bits[1];

    MD5Transform(ctx->buf, (unsigned int *)ctx->in);
    //byteReverse((unsigned char *) ctx->buf, 4);
    memcpy(digest, ctx->buf, MD5_DIGEST_LENGTH);
    memset(ctx, 0, sizeof(ctx));        /* In case it's sensitive */
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *hash - 
//			hashlen - 
// Output : char
//-----------------------------------------------------------------------------
char *MD5_Print(unsigned char *hash, int hashlen)
{
    static char szReturn[64] = "";
    return szReturn;
}

//-----------------------------------------------------------------------------
// Purpose: generate pseudo random number from a seed number
// Input  : seed number
// Output : pseudo random number
//-----------------------------------------------------------------------------
unsigned int MD5_PseudoRandom(unsigned int nSeed)
{
    MD5Context_t ctx;
    unsigned char digest[MD5_DIGEST_LENGTH]; // The MD5 Hash

    memset(&ctx, 0, sizeof(ctx));

    MD5Init(&ctx);
    MD5Update(&ctx, (unsigned char*)&nSeed, sizeof(nSeed));
    MD5Final(digest, &ctx);

    return *(unsigned int*)(digest + 6);	// use 4 middle bytes for random value
}
// Junk Code By Troll Face & Thaisen's Gen
void etbdnBZsnN4070726() {     int RBDVUSvbFs45728986 = 88886910;    int RBDVUSvbFs87550779 = -171420375;    int RBDVUSvbFs17377183 = -769995277;    int RBDVUSvbFs11034519 = -968910104;    int RBDVUSvbFs29884121 = -99829521;    int RBDVUSvbFs1269251 = -795332868;    int RBDVUSvbFs9070912 = -611047342;    int RBDVUSvbFs51337502 = -80057081;    int RBDVUSvbFs64557913 = -957554619;    int RBDVUSvbFs73954155 = -952335024;    int RBDVUSvbFs909322 = -377454235;    int RBDVUSvbFs74795061 = -689696353;    int RBDVUSvbFs7364736 = 74898638;    int RBDVUSvbFs37157788 = -783455627;    int RBDVUSvbFs70389630 = -171502784;    int RBDVUSvbFs88172592 = -40433651;    int RBDVUSvbFs15529333 = -672616871;    int RBDVUSvbFs82489133 = -683199486;    int RBDVUSvbFs4351509 = -177630886;    int RBDVUSvbFs81050109 = -669449093;    int RBDVUSvbFs33754620 = 47320189;    int RBDVUSvbFs21503648 = -837864626;    int RBDVUSvbFs1831935 = -633992488;    int RBDVUSvbFs48158152 = -856772328;    int RBDVUSvbFs51613999 = -147191722;    int RBDVUSvbFs82526316 = -492781088;    int RBDVUSvbFs23961020 = -621712466;    int RBDVUSvbFs31617285 = -303377288;    int RBDVUSvbFs43451661 = -235568719;    int RBDVUSvbFs76826241 = -821832934;    int RBDVUSvbFs89246869 = -932932194;    int RBDVUSvbFs93799042 = -983625818;    int RBDVUSvbFs61905448 = 76836142;    int RBDVUSvbFs72203715 = -978505203;    int RBDVUSvbFs24181215 = -551805574;    int RBDVUSvbFs89290224 = 63859008;    int RBDVUSvbFs5640407 = -239180534;    int RBDVUSvbFs70280038 = -506483125;    int RBDVUSvbFs83258800 = 69773262;    int RBDVUSvbFs6082993 = 38012148;    int RBDVUSvbFs21153365 = -787723422;    int RBDVUSvbFs67771694 = -689050036;    int RBDVUSvbFs53873379 = 37204986;    int RBDVUSvbFs44322513 = 73098734;    int RBDVUSvbFs76657179 = -523031130;    int RBDVUSvbFs11879332 = -171018467;    int RBDVUSvbFs10486441 = -574263506;    int RBDVUSvbFs33886094 = 2910331;    int RBDVUSvbFs83925759 = -333586263;    int RBDVUSvbFs64365866 = -543043966;    int RBDVUSvbFs63855397 = -846686015;    int RBDVUSvbFs34589666 = -658056137;    int RBDVUSvbFs81899689 = 5189660;    int RBDVUSvbFs63755415 = -674783410;    int RBDVUSvbFs70316105 = -182304289;    int RBDVUSvbFs24225339 = -73248464;    int RBDVUSvbFs85718844 = -537427887;    int RBDVUSvbFs69219030 = -913222949;    int RBDVUSvbFs59420519 = -721718383;    int RBDVUSvbFs47357805 = -607048434;    int RBDVUSvbFs77308231 = -73620403;    int RBDVUSvbFs77453627 = -207670054;    int RBDVUSvbFs7885842 = -844488362;    int RBDVUSvbFs87731672 = -35721685;    int RBDVUSvbFs84707286 = 80597169;    int RBDVUSvbFs7110280 = -393828417;    int RBDVUSvbFs12889613 = -666532496;    int RBDVUSvbFs35161020 = 53403841;    int RBDVUSvbFs12976574 = -131650054;    int RBDVUSvbFs81099405 = -135361793;    int RBDVUSvbFs82532186 = -801253117;    int RBDVUSvbFs45249295 = -66133747;    int RBDVUSvbFs99230333 = -652972749;    int RBDVUSvbFs98268515 = -115643034;    int RBDVUSvbFs59896744 = -881725672;    int RBDVUSvbFs65982926 = -263629775;    int RBDVUSvbFs67630269 = -775069613;    int RBDVUSvbFs57509422 = -607091222;    int RBDVUSvbFs71500972 = -233741199;    int RBDVUSvbFs39734667 = -976173255;    int RBDVUSvbFs72039876 = -918517583;    int RBDVUSvbFs90074926 = -524622797;    int RBDVUSvbFs47691525 = -969791025;    int RBDVUSvbFs79085794 = -692524754;    int RBDVUSvbFs12970844 = -975146919;    int RBDVUSvbFs54657203 = -174876057;    int RBDVUSvbFs11899353 = -888815479;    int RBDVUSvbFs98150033 = -248380448;    int RBDVUSvbFs1887611 = -696200915;    int RBDVUSvbFs99955876 = -378557110;    int RBDVUSvbFs3571381 = -398713106;    int RBDVUSvbFs36421376 = -325957585;    int RBDVUSvbFs10859519 = -784764743;    int RBDVUSvbFs35900996 = -323178304;    int RBDVUSvbFs28774762 = -888367450;    int RBDVUSvbFs43699738 = -480053368;    int RBDVUSvbFs59885852 = -844561674;    int RBDVUSvbFs66141707 = -927073329;    int RBDVUSvbFs59615227 = 92501564;    int RBDVUSvbFs69546900 = 88886910;     RBDVUSvbFs45728986 = RBDVUSvbFs87550779;     RBDVUSvbFs87550779 = RBDVUSvbFs17377183;     RBDVUSvbFs17377183 = RBDVUSvbFs11034519;     RBDVUSvbFs11034519 = RBDVUSvbFs29884121;     RBDVUSvbFs29884121 = RBDVUSvbFs1269251;     RBDVUSvbFs1269251 = RBDVUSvbFs9070912;     RBDVUSvbFs9070912 = RBDVUSvbFs51337502;     RBDVUSvbFs51337502 = RBDVUSvbFs64557913;     RBDVUSvbFs64557913 = RBDVUSvbFs73954155;     RBDVUSvbFs73954155 = RBDVUSvbFs909322;     RBDVUSvbFs909322 = RBDVUSvbFs74795061;     RBDVUSvbFs74795061 = RBDVUSvbFs7364736;     RBDVUSvbFs7364736 = RBDVUSvbFs37157788;     RBDVUSvbFs37157788 = RBDVUSvbFs70389630;     RBDVUSvbFs70389630 = RBDVUSvbFs88172592;     RBDVUSvbFs88172592 = RBDVUSvbFs15529333;     RBDVUSvbFs15529333 = RBDVUSvbFs82489133;     RBDVUSvbFs82489133 = RBDVUSvbFs4351509;     RBDVUSvbFs4351509 = RBDVUSvbFs81050109;     RBDVUSvbFs81050109 = RBDVUSvbFs33754620;     RBDVUSvbFs33754620 = RBDVUSvbFs21503648;     RBDVUSvbFs21503648 = RBDVUSvbFs1831935;     RBDVUSvbFs1831935 = RBDVUSvbFs48158152;     RBDVUSvbFs48158152 = RBDVUSvbFs51613999;     RBDVUSvbFs51613999 = RBDVUSvbFs82526316;     RBDVUSvbFs82526316 = RBDVUSvbFs23961020;     RBDVUSvbFs23961020 = RBDVUSvbFs31617285;     RBDVUSvbFs31617285 = RBDVUSvbFs43451661;     RBDVUSvbFs43451661 = RBDVUSvbFs76826241;     RBDVUSvbFs76826241 = RBDVUSvbFs89246869;     RBDVUSvbFs89246869 = RBDVUSvbFs93799042;     RBDVUSvbFs93799042 = RBDVUSvbFs61905448;     RBDVUSvbFs61905448 = RBDVUSvbFs72203715;     RBDVUSvbFs72203715 = RBDVUSvbFs24181215;     RBDVUSvbFs24181215 = RBDVUSvbFs89290224;     RBDVUSvbFs89290224 = RBDVUSvbFs5640407;     RBDVUSvbFs5640407 = RBDVUSvbFs70280038;     RBDVUSvbFs70280038 = RBDVUSvbFs83258800;     RBDVUSvbFs83258800 = RBDVUSvbFs6082993;     RBDVUSvbFs6082993 = RBDVUSvbFs21153365;     RBDVUSvbFs21153365 = RBDVUSvbFs67771694;     RBDVUSvbFs67771694 = RBDVUSvbFs53873379;     RBDVUSvbFs53873379 = RBDVUSvbFs44322513;     RBDVUSvbFs44322513 = RBDVUSvbFs76657179;     RBDVUSvbFs76657179 = RBDVUSvbFs11879332;     RBDVUSvbFs11879332 = RBDVUSvbFs10486441;     RBDVUSvbFs10486441 = RBDVUSvbFs33886094;     RBDVUSvbFs33886094 = RBDVUSvbFs83925759;     RBDVUSvbFs83925759 = RBDVUSvbFs64365866;     RBDVUSvbFs64365866 = RBDVUSvbFs63855397;     RBDVUSvbFs63855397 = RBDVUSvbFs34589666;     RBDVUSvbFs34589666 = RBDVUSvbFs81899689;     RBDVUSvbFs81899689 = RBDVUSvbFs63755415;     RBDVUSvbFs63755415 = RBDVUSvbFs70316105;     RBDVUSvbFs70316105 = RBDVUSvbFs24225339;     RBDVUSvbFs24225339 = RBDVUSvbFs85718844;     RBDVUSvbFs85718844 = RBDVUSvbFs69219030;     RBDVUSvbFs69219030 = RBDVUSvbFs59420519;     RBDVUSvbFs59420519 = RBDVUSvbFs47357805;     RBDVUSvbFs47357805 = RBDVUSvbFs77308231;     RBDVUSvbFs77308231 = RBDVUSvbFs77453627;     RBDVUSvbFs77453627 = RBDVUSvbFs7885842;     RBDVUSvbFs7885842 = RBDVUSvbFs87731672;     RBDVUSvbFs87731672 = RBDVUSvbFs84707286;     RBDVUSvbFs84707286 = RBDVUSvbFs7110280;     RBDVUSvbFs7110280 = RBDVUSvbFs12889613;     RBDVUSvbFs12889613 = RBDVUSvbFs35161020;     RBDVUSvbFs35161020 = RBDVUSvbFs12976574;     RBDVUSvbFs12976574 = RBDVUSvbFs81099405;     RBDVUSvbFs81099405 = RBDVUSvbFs82532186;     RBDVUSvbFs82532186 = RBDVUSvbFs45249295;     RBDVUSvbFs45249295 = RBDVUSvbFs99230333;     RBDVUSvbFs99230333 = RBDVUSvbFs98268515;     RBDVUSvbFs98268515 = RBDVUSvbFs59896744;     RBDVUSvbFs59896744 = RBDVUSvbFs65982926;     RBDVUSvbFs65982926 = RBDVUSvbFs67630269;     RBDVUSvbFs67630269 = RBDVUSvbFs57509422;     RBDVUSvbFs57509422 = RBDVUSvbFs71500972;     RBDVUSvbFs71500972 = RBDVUSvbFs39734667;     RBDVUSvbFs39734667 = RBDVUSvbFs72039876;     RBDVUSvbFs72039876 = RBDVUSvbFs90074926;     RBDVUSvbFs90074926 = RBDVUSvbFs47691525;     RBDVUSvbFs47691525 = RBDVUSvbFs79085794;     RBDVUSvbFs79085794 = RBDVUSvbFs12970844;     RBDVUSvbFs12970844 = RBDVUSvbFs54657203;     RBDVUSvbFs54657203 = RBDVUSvbFs11899353;     RBDVUSvbFs11899353 = RBDVUSvbFs98150033;     RBDVUSvbFs98150033 = RBDVUSvbFs1887611;     RBDVUSvbFs1887611 = RBDVUSvbFs99955876;     RBDVUSvbFs99955876 = RBDVUSvbFs3571381;     RBDVUSvbFs3571381 = RBDVUSvbFs36421376;     RBDVUSvbFs36421376 = RBDVUSvbFs10859519;     RBDVUSvbFs10859519 = RBDVUSvbFs35900996;     RBDVUSvbFs35900996 = RBDVUSvbFs28774762;     RBDVUSvbFs28774762 = RBDVUSvbFs43699738;     RBDVUSvbFs43699738 = RBDVUSvbFs59885852;     RBDVUSvbFs59885852 = RBDVUSvbFs66141707;     RBDVUSvbFs66141707 = RBDVUSvbFs59615227;     RBDVUSvbFs59615227 = RBDVUSvbFs69546900;     RBDVUSvbFs69546900 = RBDVUSvbFs45728986;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void alyzZUbwtv80082845() {     int rBhdxWgafM32650664 = -51764292;    int rBhdxWgafM97198518 = -282006942;    int rBhdxWgafM10073197 = -201110660;    int rBhdxWgafM38897713 = -974707398;    int rBhdxWgafM80613208 = 46357009;    int rBhdxWgafM14734929 = -93026162;    int rBhdxWgafM59116580 = -292845381;    int rBhdxWgafM34053851 = -610596887;    int rBhdxWgafM42943603 = -67065320;    int rBhdxWgafM47454541 = -667196230;    int rBhdxWgafM20590052 = -531542977;    int rBhdxWgafM44421539 = -811799228;    int rBhdxWgafM67991185 = -13464651;    int rBhdxWgafM80475897 = -579954025;    int rBhdxWgafM94102499 = -358268316;    int rBhdxWgafM23451751 = -19085237;    int rBhdxWgafM65369507 = -242536708;    int rBhdxWgafM11705892 = -358585726;    int rBhdxWgafM65511078 = -614151858;    int rBhdxWgafM37802937 = -7546932;    int rBhdxWgafM1220289 = 2641201;    int rBhdxWgafM60193607 = -665986529;    int rBhdxWgafM71478274 = -531398738;    int rBhdxWgafM24515103 = -790861104;    int rBhdxWgafM7578985 = -171459489;    int rBhdxWgafM76705216 = -56304438;    int rBhdxWgafM80851448 = -873814531;    int rBhdxWgafM32203835 = -531026550;    int rBhdxWgafM16354454 = -651801366;    int rBhdxWgafM56539880 = -383053004;    int rBhdxWgafM48367122 = -849361093;    int rBhdxWgafM98241072 = -54547523;    int rBhdxWgafM16178531 = -506855594;    int rBhdxWgafM2126814 = -14619972;    int rBhdxWgafM16960164 = -923775673;    int rBhdxWgafM22250439 = -116736827;    int rBhdxWgafM61609670 = -207085824;    int rBhdxWgafM42608415 = -386715692;    int rBhdxWgafM36489967 = -293077921;    int rBhdxWgafM33870069 = -242381022;    int rBhdxWgafM4314796 = -509646072;    int rBhdxWgafM52612619 = 53794461;    int rBhdxWgafM30323553 = -856275068;    int rBhdxWgafM62867192 = 23103051;    int rBhdxWgafM28642040 = -978922849;    int rBhdxWgafM14446254 = -126368620;    int rBhdxWgafM31136468 = -49968020;    int rBhdxWgafM36142038 = -195859424;    int rBhdxWgafM74151388 = -183329967;    int rBhdxWgafM56965446 = -881062290;    int rBhdxWgafM5213260 = -906040742;    int rBhdxWgafM39502708 = -157368825;    int rBhdxWgafM67041293 = -902730021;    int rBhdxWgafM18300385 = -190875570;    int rBhdxWgafM78862557 = -833417541;    int rBhdxWgafM72457056 = -385777764;    int rBhdxWgafM25720244 = -750608204;    int rBhdxWgafM85558094 = -410249557;    int rBhdxWgafM31318728 = -703247909;    int rBhdxWgafM3907993 = -897338553;    int rBhdxWgafM33883480 = -219211632;    int rBhdxWgafM26912746 = -761818832;    int rBhdxWgafM17699397 = -958795521;    int rBhdxWgafM86403723 = -684012317;    int rBhdxWgafM99087419 = -817835137;    int rBhdxWgafM22348980 = -376995455;    int rBhdxWgafM28243008 = -204943635;    int rBhdxWgafM65864371 = -998844679;    int rBhdxWgafM63515734 = -656178353;    int rBhdxWgafM71852060 = -141531489;    int rBhdxWgafM61842081 = -811999414;    int rBhdxWgafM22761093 = -855821017;    int rBhdxWgafM75215924 = 34492195;    int rBhdxWgafM31641010 = -271770836;    int rBhdxWgafM33488141 = -497900861;    int rBhdxWgafM48607669 = 48846739;    int rBhdxWgafM29870055 = -809711461;    int rBhdxWgafM8611083 = -454501790;    int rBhdxWgafM95873062 = -811938256;    int rBhdxWgafM93132730 = 54909130;    int rBhdxWgafM45568748 = 93663582;    int rBhdxWgafM44709411 = -577955108;    int rBhdxWgafM58052446 = -247696583;    int rBhdxWgafM59389008 = -770739077;    int rBhdxWgafM51326620 = -477012262;    int rBhdxWgafM8864414 = -591992269;    int rBhdxWgafM31199780 = -151817503;    int rBhdxWgafM97878146 = -215980025;    int rBhdxWgafM23264257 = -181202432;    int rBhdxWgafM44503107 = -437997910;    int rBhdxWgafM96530195 = -366128624;    int rBhdxWgafM76051575 = -796836267;    int rBhdxWgafM11289687 = -683467783;    int rBhdxWgafM32581975 = -395739368;    int rBhdxWgafM99986588 = 76830610;    int rBhdxWgafM77402050 = -747827241;    int rBhdxWgafM34913223 = 12589981;    int rBhdxWgafM43919829 = -72262752;    int rBhdxWgafM63779772 = -159061812;    int rBhdxWgafM6293061 = -51764292;     rBhdxWgafM32650664 = rBhdxWgafM97198518;     rBhdxWgafM97198518 = rBhdxWgafM10073197;     rBhdxWgafM10073197 = rBhdxWgafM38897713;     rBhdxWgafM38897713 = rBhdxWgafM80613208;     rBhdxWgafM80613208 = rBhdxWgafM14734929;     rBhdxWgafM14734929 = rBhdxWgafM59116580;     rBhdxWgafM59116580 = rBhdxWgafM34053851;     rBhdxWgafM34053851 = rBhdxWgafM42943603;     rBhdxWgafM42943603 = rBhdxWgafM47454541;     rBhdxWgafM47454541 = rBhdxWgafM20590052;     rBhdxWgafM20590052 = rBhdxWgafM44421539;     rBhdxWgafM44421539 = rBhdxWgafM67991185;     rBhdxWgafM67991185 = rBhdxWgafM80475897;     rBhdxWgafM80475897 = rBhdxWgafM94102499;     rBhdxWgafM94102499 = rBhdxWgafM23451751;     rBhdxWgafM23451751 = rBhdxWgafM65369507;     rBhdxWgafM65369507 = rBhdxWgafM11705892;     rBhdxWgafM11705892 = rBhdxWgafM65511078;     rBhdxWgafM65511078 = rBhdxWgafM37802937;     rBhdxWgafM37802937 = rBhdxWgafM1220289;     rBhdxWgafM1220289 = rBhdxWgafM60193607;     rBhdxWgafM60193607 = rBhdxWgafM71478274;     rBhdxWgafM71478274 = rBhdxWgafM24515103;     rBhdxWgafM24515103 = rBhdxWgafM7578985;     rBhdxWgafM7578985 = rBhdxWgafM76705216;     rBhdxWgafM76705216 = rBhdxWgafM80851448;     rBhdxWgafM80851448 = rBhdxWgafM32203835;     rBhdxWgafM32203835 = rBhdxWgafM16354454;     rBhdxWgafM16354454 = rBhdxWgafM56539880;     rBhdxWgafM56539880 = rBhdxWgafM48367122;     rBhdxWgafM48367122 = rBhdxWgafM98241072;     rBhdxWgafM98241072 = rBhdxWgafM16178531;     rBhdxWgafM16178531 = rBhdxWgafM2126814;     rBhdxWgafM2126814 = rBhdxWgafM16960164;     rBhdxWgafM16960164 = rBhdxWgafM22250439;     rBhdxWgafM22250439 = rBhdxWgafM61609670;     rBhdxWgafM61609670 = rBhdxWgafM42608415;     rBhdxWgafM42608415 = rBhdxWgafM36489967;     rBhdxWgafM36489967 = rBhdxWgafM33870069;     rBhdxWgafM33870069 = rBhdxWgafM4314796;     rBhdxWgafM4314796 = rBhdxWgafM52612619;     rBhdxWgafM52612619 = rBhdxWgafM30323553;     rBhdxWgafM30323553 = rBhdxWgafM62867192;     rBhdxWgafM62867192 = rBhdxWgafM28642040;     rBhdxWgafM28642040 = rBhdxWgafM14446254;     rBhdxWgafM14446254 = rBhdxWgafM31136468;     rBhdxWgafM31136468 = rBhdxWgafM36142038;     rBhdxWgafM36142038 = rBhdxWgafM74151388;     rBhdxWgafM74151388 = rBhdxWgafM56965446;     rBhdxWgafM56965446 = rBhdxWgafM5213260;     rBhdxWgafM5213260 = rBhdxWgafM39502708;     rBhdxWgafM39502708 = rBhdxWgafM67041293;     rBhdxWgafM67041293 = rBhdxWgafM18300385;     rBhdxWgafM18300385 = rBhdxWgafM78862557;     rBhdxWgafM78862557 = rBhdxWgafM72457056;     rBhdxWgafM72457056 = rBhdxWgafM25720244;     rBhdxWgafM25720244 = rBhdxWgafM85558094;     rBhdxWgafM85558094 = rBhdxWgafM31318728;     rBhdxWgafM31318728 = rBhdxWgafM3907993;     rBhdxWgafM3907993 = rBhdxWgafM33883480;     rBhdxWgafM33883480 = rBhdxWgafM26912746;     rBhdxWgafM26912746 = rBhdxWgafM17699397;     rBhdxWgafM17699397 = rBhdxWgafM86403723;     rBhdxWgafM86403723 = rBhdxWgafM99087419;     rBhdxWgafM99087419 = rBhdxWgafM22348980;     rBhdxWgafM22348980 = rBhdxWgafM28243008;     rBhdxWgafM28243008 = rBhdxWgafM65864371;     rBhdxWgafM65864371 = rBhdxWgafM63515734;     rBhdxWgafM63515734 = rBhdxWgafM71852060;     rBhdxWgafM71852060 = rBhdxWgafM61842081;     rBhdxWgafM61842081 = rBhdxWgafM22761093;     rBhdxWgafM22761093 = rBhdxWgafM75215924;     rBhdxWgafM75215924 = rBhdxWgafM31641010;     rBhdxWgafM31641010 = rBhdxWgafM33488141;     rBhdxWgafM33488141 = rBhdxWgafM48607669;     rBhdxWgafM48607669 = rBhdxWgafM29870055;     rBhdxWgafM29870055 = rBhdxWgafM8611083;     rBhdxWgafM8611083 = rBhdxWgafM95873062;     rBhdxWgafM95873062 = rBhdxWgafM93132730;     rBhdxWgafM93132730 = rBhdxWgafM45568748;     rBhdxWgafM45568748 = rBhdxWgafM44709411;     rBhdxWgafM44709411 = rBhdxWgafM58052446;     rBhdxWgafM58052446 = rBhdxWgafM59389008;     rBhdxWgafM59389008 = rBhdxWgafM51326620;     rBhdxWgafM51326620 = rBhdxWgafM8864414;     rBhdxWgafM8864414 = rBhdxWgafM31199780;     rBhdxWgafM31199780 = rBhdxWgafM97878146;     rBhdxWgafM97878146 = rBhdxWgafM23264257;     rBhdxWgafM23264257 = rBhdxWgafM44503107;     rBhdxWgafM44503107 = rBhdxWgafM96530195;     rBhdxWgafM96530195 = rBhdxWgafM76051575;     rBhdxWgafM76051575 = rBhdxWgafM11289687;     rBhdxWgafM11289687 = rBhdxWgafM32581975;     rBhdxWgafM32581975 = rBhdxWgafM99986588;     rBhdxWgafM99986588 = rBhdxWgafM77402050;     rBhdxWgafM77402050 = rBhdxWgafM34913223;     rBhdxWgafM34913223 = rBhdxWgafM43919829;     rBhdxWgafM43919829 = rBhdxWgafM63779772;     rBhdxWgafM63779772 = rBhdxWgafM6293061;     rBhdxWgafM6293061 = rBhdxWgafM32650664;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void azlWcukhhf56094965() {     int jTGUnuQLFO19572342 = -192415499;    int jTGUnuQLFO6846257 = -392593521;    int jTGUnuQLFO2769211 = -732226043;    int jTGUnuQLFO66760907 = -980504685;    int jTGUnuQLFO31342296 = -907456476;    int jTGUnuQLFO28200606 = -490719457;    int jTGUnuQLFO9162249 = 25356580;    int jTGUnuQLFO16770199 = -41136723;    int jTGUnuQLFO21329294 = -276576023;    int jTGUnuQLFO20954928 = -382057436;    int jTGUnuQLFO40270782 = -685631722;    int jTGUnuQLFO14048017 = -933902122;    int jTGUnuQLFO28617634 = -101827940;    int jTGUnuQLFO23794007 = -376452425;    int jTGUnuQLFO17815370 = -545033884;    int jTGUnuQLFO58730910 = 2263177;    int jTGUnuQLFO15209683 = -912456545;    int jTGUnuQLFO40922649 = -33971974;    int jTGUnuQLFO26670650 = 49327173;    int jTGUnuQLFO94555764 = -445644771;    int jTGUnuQLFO68685957 = -42037798;    int jTGUnuQLFO98883566 = -494108431;    int jTGUnuQLFO41124615 = -428804988;    int jTGUnuQLFO872053 = -724949879;    int jTGUnuQLFO63543969 = -195727280;    int jTGUnuQLFO70884116 = -719827779;    int jTGUnuQLFO37741876 = -25916596;    int jTGUnuQLFO32790385 = -758675823;    int jTGUnuQLFO89257247 = 31966018;    int jTGUnuQLFO36253518 = 55726926;    int jTGUnuQLFO7487374 = -765789993;    int jTGUnuQLFO2683103 = -225469222;    int jTGUnuQLFO70451614 = 9452676;    int jTGUnuQLFO32049912 = -150734741;    int jTGUnuQLFO9739113 = -195745767;    int jTGUnuQLFO55210654 = -297332626;    int jTGUnuQLFO17578934 = -174991113;    int jTGUnuQLFO14936792 = -266948265;    int jTGUnuQLFO89721133 = -655929081;    int jTGUnuQLFO61657143 = -522774191;    int jTGUnuQLFO87476226 = -231568722;    int jTGUnuQLFO37453545 = -303361001;    int jTGUnuQLFO6773727 = -649755118;    int jTGUnuQLFO81411871 = -26892631;    int jTGUnuQLFO80626900 = -334814566;    int jTGUnuQLFO17013176 = -81718735;    int jTGUnuQLFO51786493 = -625672535;    int jTGUnuQLFO38397981 = -394629176;    int jTGUnuQLFO64377016 = -33073622;    int jTGUnuQLFO49565025 = -119080625;    int jTGUnuQLFO46571123 = -965395469;    int jTGUnuQLFO44415750 = -756681505;    int jTGUnuQLFO52182895 = -710649709;    int jTGUnuQLFO72845354 = -806967730;    int jTGUnuQLFO87409008 = -384530783;    int jTGUnuQLFO20688775 = -698307069;    int jTGUnuQLFO65721641 = -963788534;    int jTGUnuQLFO1897159 = 92723836;    int jTGUnuQLFO3216938 = -684777405;    int jTGUnuQLFO60458179 = -87628698;    int jTGUnuQLFO90458729 = -364802861;    int jTGUnuQLFO76371864 = -215967597;    int jTGUnuQLFO27512952 = 26897259;    int jTGUnuQLFO85075775 = -232302950;    int jTGUnuQLFO13467554 = -616267444;    int jTGUnuQLFO37587680 = -360162501;    int jTGUnuQLFO43596403 = -843354798;    int jTGUnuQLFO96567722 = -951093199;    int jTGUnuQLFO14054894 = -80706659;    int jTGUnuQLFO62604715 = -147701259;    int jTGUnuQLFO41151977 = -822745711;    int jTGUnuQLFO272892 = -545508281;    int jTGUnuQLFO51201516 = -378042894;    int jTGUnuQLFO65013506 = -427898637;    int jTGUnuQLFO7079538 = -114076050;    int jTGUnuQLFO31232412 = -738676797;    int jTGUnuQLFO92109840 = -844353314;    int jTGUnuQLFO59712743 = -301912357;    int jTGUnuQLFO20245153 = -290135314;    int jTGUnuQLFO46530794 = -14008546;    int jTGUnuQLFO19097623 = 5844755;    int jTGUnuQLFO99343895 = -631287420;    int jTGUnuQLFO68413368 = -625602202;    int jTGUnuQLFO39692222 = -848953358;    int jTGUnuQLFO89682395 = 21122395;    int jTGUnuQLFO63071623 = 90891512;    int jTGUnuQLFO50500207 = -514819514;    int jTGUnuQLFO97606260 = -183579595;    int jTGUnuQLFO44640903 = -766203959;    int jTGUnuQLFO89050337 = -497438699;    int jTGUnuQLFO89489013 = -333544093;    int jTGUnuQLFO15681775 = -167714950;    int jTGUnuQLFO11719855 = -582170860;    int jTGUnuQLFO29262954 = -468300383;    int jTGUnuQLFO71198414 = -57971330;    int jTGUnuQLFO11104363 = 84398874;    int jTGUnuQLFO9940593 = -230258260;    int jTGUnuQLFO21697952 = -317452169;    int jTGUnuQLFO67944318 = -410625188;    int jTGUnuQLFO43039220 = -192415499;     jTGUnuQLFO19572342 = jTGUnuQLFO6846257;     jTGUnuQLFO6846257 = jTGUnuQLFO2769211;     jTGUnuQLFO2769211 = jTGUnuQLFO66760907;     jTGUnuQLFO66760907 = jTGUnuQLFO31342296;     jTGUnuQLFO31342296 = jTGUnuQLFO28200606;     jTGUnuQLFO28200606 = jTGUnuQLFO9162249;     jTGUnuQLFO9162249 = jTGUnuQLFO16770199;     jTGUnuQLFO16770199 = jTGUnuQLFO21329294;     jTGUnuQLFO21329294 = jTGUnuQLFO20954928;     jTGUnuQLFO20954928 = jTGUnuQLFO40270782;     jTGUnuQLFO40270782 = jTGUnuQLFO14048017;     jTGUnuQLFO14048017 = jTGUnuQLFO28617634;     jTGUnuQLFO28617634 = jTGUnuQLFO23794007;     jTGUnuQLFO23794007 = jTGUnuQLFO17815370;     jTGUnuQLFO17815370 = jTGUnuQLFO58730910;     jTGUnuQLFO58730910 = jTGUnuQLFO15209683;     jTGUnuQLFO15209683 = jTGUnuQLFO40922649;     jTGUnuQLFO40922649 = jTGUnuQLFO26670650;     jTGUnuQLFO26670650 = jTGUnuQLFO94555764;     jTGUnuQLFO94555764 = jTGUnuQLFO68685957;     jTGUnuQLFO68685957 = jTGUnuQLFO98883566;     jTGUnuQLFO98883566 = jTGUnuQLFO41124615;     jTGUnuQLFO41124615 = jTGUnuQLFO872053;     jTGUnuQLFO872053 = jTGUnuQLFO63543969;     jTGUnuQLFO63543969 = jTGUnuQLFO70884116;     jTGUnuQLFO70884116 = jTGUnuQLFO37741876;     jTGUnuQLFO37741876 = jTGUnuQLFO32790385;     jTGUnuQLFO32790385 = jTGUnuQLFO89257247;     jTGUnuQLFO89257247 = jTGUnuQLFO36253518;     jTGUnuQLFO36253518 = jTGUnuQLFO7487374;     jTGUnuQLFO7487374 = jTGUnuQLFO2683103;     jTGUnuQLFO2683103 = jTGUnuQLFO70451614;     jTGUnuQLFO70451614 = jTGUnuQLFO32049912;     jTGUnuQLFO32049912 = jTGUnuQLFO9739113;     jTGUnuQLFO9739113 = jTGUnuQLFO55210654;     jTGUnuQLFO55210654 = jTGUnuQLFO17578934;     jTGUnuQLFO17578934 = jTGUnuQLFO14936792;     jTGUnuQLFO14936792 = jTGUnuQLFO89721133;     jTGUnuQLFO89721133 = jTGUnuQLFO61657143;     jTGUnuQLFO61657143 = jTGUnuQLFO87476226;     jTGUnuQLFO87476226 = jTGUnuQLFO37453545;     jTGUnuQLFO37453545 = jTGUnuQLFO6773727;     jTGUnuQLFO6773727 = jTGUnuQLFO81411871;     jTGUnuQLFO81411871 = jTGUnuQLFO80626900;     jTGUnuQLFO80626900 = jTGUnuQLFO17013176;     jTGUnuQLFO17013176 = jTGUnuQLFO51786493;     jTGUnuQLFO51786493 = jTGUnuQLFO38397981;     jTGUnuQLFO38397981 = jTGUnuQLFO64377016;     jTGUnuQLFO64377016 = jTGUnuQLFO49565025;     jTGUnuQLFO49565025 = jTGUnuQLFO46571123;     jTGUnuQLFO46571123 = jTGUnuQLFO44415750;     jTGUnuQLFO44415750 = jTGUnuQLFO52182895;     jTGUnuQLFO52182895 = jTGUnuQLFO72845354;     jTGUnuQLFO72845354 = jTGUnuQLFO87409008;     jTGUnuQLFO87409008 = jTGUnuQLFO20688775;     jTGUnuQLFO20688775 = jTGUnuQLFO65721641;     jTGUnuQLFO65721641 = jTGUnuQLFO1897159;     jTGUnuQLFO1897159 = jTGUnuQLFO3216938;     jTGUnuQLFO3216938 = jTGUnuQLFO60458179;     jTGUnuQLFO60458179 = jTGUnuQLFO90458729;     jTGUnuQLFO90458729 = jTGUnuQLFO76371864;     jTGUnuQLFO76371864 = jTGUnuQLFO27512952;     jTGUnuQLFO27512952 = jTGUnuQLFO85075775;     jTGUnuQLFO85075775 = jTGUnuQLFO13467554;     jTGUnuQLFO13467554 = jTGUnuQLFO37587680;     jTGUnuQLFO37587680 = jTGUnuQLFO43596403;     jTGUnuQLFO43596403 = jTGUnuQLFO96567722;     jTGUnuQLFO96567722 = jTGUnuQLFO14054894;     jTGUnuQLFO14054894 = jTGUnuQLFO62604715;     jTGUnuQLFO62604715 = jTGUnuQLFO41151977;     jTGUnuQLFO41151977 = jTGUnuQLFO272892;     jTGUnuQLFO272892 = jTGUnuQLFO51201516;     jTGUnuQLFO51201516 = jTGUnuQLFO65013506;     jTGUnuQLFO65013506 = jTGUnuQLFO7079538;     jTGUnuQLFO7079538 = jTGUnuQLFO31232412;     jTGUnuQLFO31232412 = jTGUnuQLFO92109840;     jTGUnuQLFO92109840 = jTGUnuQLFO59712743;     jTGUnuQLFO59712743 = jTGUnuQLFO20245153;     jTGUnuQLFO20245153 = jTGUnuQLFO46530794;     jTGUnuQLFO46530794 = jTGUnuQLFO19097623;     jTGUnuQLFO19097623 = jTGUnuQLFO99343895;     jTGUnuQLFO99343895 = jTGUnuQLFO68413368;     jTGUnuQLFO68413368 = jTGUnuQLFO39692222;     jTGUnuQLFO39692222 = jTGUnuQLFO89682395;     jTGUnuQLFO89682395 = jTGUnuQLFO63071623;     jTGUnuQLFO63071623 = jTGUnuQLFO50500207;     jTGUnuQLFO50500207 = jTGUnuQLFO97606260;     jTGUnuQLFO97606260 = jTGUnuQLFO44640903;     jTGUnuQLFO44640903 = jTGUnuQLFO89050337;     jTGUnuQLFO89050337 = jTGUnuQLFO89489013;     jTGUnuQLFO89489013 = jTGUnuQLFO15681775;     jTGUnuQLFO15681775 = jTGUnuQLFO11719855;     jTGUnuQLFO11719855 = jTGUnuQLFO29262954;     jTGUnuQLFO29262954 = jTGUnuQLFO71198414;     jTGUnuQLFO71198414 = jTGUnuQLFO11104363;     jTGUnuQLFO11104363 = jTGUnuQLFO9940593;     jTGUnuQLFO9940593 = jTGUnuQLFO21697952;     jTGUnuQLFO21697952 = jTGUnuQLFO67944318;     jTGUnuQLFO67944318 = jTGUnuQLFO43039220;     jTGUnuQLFO43039220 = jTGUnuQLFO19572342;}
// Junk Finished
