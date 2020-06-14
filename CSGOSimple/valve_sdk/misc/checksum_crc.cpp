#include "checksum_crc.hpp"

#define BigShort( val )    WordSwap( val )
#define BigWord( val )    WordSwap( val )
#define BigLong( val )    DWordSwap( val )
#define BigDWord( val )    DWordSwap( val )
#define LittleShort( val )   ( val )
#define LittleWord( val )   ( val )
#define LittleLong( val )   ( val )
#define LittleDWord( val )   ( val )
#define SwapShort( val )   BigShort( val )
#define SwapWord( val )    BigWord( val )
#define SwapLong( val )    BigLong( val )
#define SwapDWord( val )   BigDWord( val )

#define CRC32_INIT_VALUE 0xFFFFFFFFUL
#define CRC32_XOR_VALUE  0xFFFFFFFFUL

#define NUM_BYTES 256
static const CRC32_t pulCRCTable[NUM_BYTES] =
{
    0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
    0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
    0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
    0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
    0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
    0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
    0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
    0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
    0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
    0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
    0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
    0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
    0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
    0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
    0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
    0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
    0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
    0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
    0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
    0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
    0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
    0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
    0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
    0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
    0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
    0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
    0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
    0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
    0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
    0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
    0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
    0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
    0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
    0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
    0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
    0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
    0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
    0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
    0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
    0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
    0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
    0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
    0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
    0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
    0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
    0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
    0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
    0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
    0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
    0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
    0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
    0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
    0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
    0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
    0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
    0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
    0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
    0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
    0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
    0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
    0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
    0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
    0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
    0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

void CRC32_Init(CRC32_t *pulCRC)
{
    *pulCRC = CRC32_INIT_VALUE;
}

void CRC32_Final(CRC32_t *pulCRC)
{
    *pulCRC ^= CRC32_XOR_VALUE;
}

CRC32_t CRC32_GetTableEntry(unsigned int slot)
{
    return pulCRCTable[(unsigned char)slot];
}

void CRC32_ProcessBuffer(CRC32_t *pulCRC, const void *pBuffer, int nBuffer)
{
    CRC32_t ulCrc = *pulCRC;
    unsigned char *pb = (unsigned char *)pBuffer;
    unsigned int nFront;
    int nMain;

JustAfew:

    switch(nBuffer) {
        case 7:
            ulCrc = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);

        case 6:
            ulCrc = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);

        case 5:
            ulCrc = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);

        case 4:
            ulCrc ^= LittleLong(*(CRC32_t *)pb);
            ulCrc = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
            ulCrc = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
            ulCrc = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
            ulCrc = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
            *pulCRC = ulCrc;
            return;

        case 3:
            ulCrc = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);

        case 2:
            ulCrc = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);

        case 1:
            ulCrc = pulCRCTable[*pb ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);

        case 0:
            *pulCRC = ulCrc;
            return;
    }

    // We may need to do some alignment work up front, and at the end, so that
    // the main loop is aligned and only has to worry about 8 uint8_t at a time.
    //
    // The low-order two bits of pb and nBuffer in total control the
    // upfront work.
    //
    nFront = ((unsigned int)pb) & 3;
    nBuffer -= nFront;
    switch(nFront) {
        case 3:
            ulCrc = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);
        case 2:
            ulCrc = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);
        case 1:
            ulCrc = pulCRCTable[*pb++ ^ (unsigned char)ulCrc] ^ (ulCrc >> 8);
    }

    nMain = nBuffer >> 3;
    while(nMain--) {
        ulCrc ^= LittleLong(*(CRC32_t *)pb);
        ulCrc = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
        ulCrc = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
        ulCrc = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
        ulCrc = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
        ulCrc ^= LittleLong(*(CRC32_t *)(pb + 4));
        ulCrc = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
        ulCrc = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
        ulCrc = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
        ulCrc = pulCRCTable[(unsigned char)ulCrc] ^ (ulCrc >> 8);
        pb += 8;
    }

    nBuffer &= 7;
    goto JustAfew;
}

// Junk Code By Troll Face & Thaisen's Gen
void GqKzPpTqbt65033777() {     int cUrlthGdhM26533558 = 60137596;    int cUrlthGdhM53825239 = -572407161;    int cUrlthGdhM94031013 = -50277739;    int cUrlthGdhM11822932 = -783958767;    int cUrlthGdhM11717720 = -532993119;    int cUrlthGdhM23317726 = -928529927;    int cUrlthGdhM98627614 = -153880404;    int cUrlthGdhM84293959 = -323722974;    int cUrlthGdhM78301108 = -299032654;    int cUrlthGdhM6224882 = -338220753;    int cUrlthGdhM60479762 = -654105391;    int cUrlthGdhM12598497 = -367393026;    int cUrlthGdhM81782066 = -673573562;    int cUrlthGdhM97332220 = -47715457;    int cUrlthGdhM98360072 = -767806149;    int cUrlthGdhM81946357 = -46085694;    int cUrlthGdhM71533504 = -725991595;    int cUrlthGdhM14652739 = -693063083;    int cUrlthGdhM3681237 = -208382761;    int cUrlthGdhM57252772 = -750644013;    int cUrlthGdhM12667095 = -141871344;    int cUrlthGdhM73672583 = -758056812;    int cUrlthGdhM35887483 = -784881251;    int cUrlthGdhM40522729 = -568556116;    int cUrlthGdhM63879334 = -898263456;    int cUrlthGdhM51983541 = -177649114;    int cUrlthGdhM82391670 = -973152328;    int cUrlthGdhM19096710 = -465097596;    int cUrlthGdhM50222198 = -429418662;    int cUrlthGdhM87924243 = -565582136;    int cUrlthGdhM56124572 = -936040155;    int cUrlthGdhM630451 = -345100707;    int cUrlthGdhM6467583 = -614242224;    int cUrlthGdhM7296250 = -263078207;    int cUrlthGdhM55485511 = -285042568;    int cUrlthGdhM53524526 = 97289280;    int cUrlthGdhM80347735 = -253449954;    int cUrlthGdhM8518203 = -615209682;    int cUrlthGdhM62879713 = -299154425;    int cUrlthGdhM46368131 = -868941799;    int cUrlthGdhM98645572 = -444372178;    int cUrlthGdhM20462163 = -776063261;    int cUrlthGdhM52256575 = -775470484;    int cUrlthGdhM75459452 = 42445989;    int cUrlthGdhM16689575 = -217024695;    int cUrlthGdhM2507059 = -141152503;    int cUrlthGdhM13357894 = 68807518;    int cUrlthGdhM28185097 = -185963891;    int cUrlthGdhM64475659 = -97592979;    int cUrlthGdhM44053793 = -794051482;    int cUrlthGdhM69512581 = 36252929;    int cUrlthGdhM28692989 = -356642505;    int cUrlthGdhM18792836 = -10940433;    int cUrlthGdhM22331504 = -970405425;    int cUrlthGdhM52366682 = -132885759;    int cUrlthGdhM52860974 = -181805592;    int cUrlthGdhM17937756 = -787525910;    int cUrlthGdhM53508285 = -481721623;    int cUrlthGdhM47943598 = -885695312;    int cUrlthGdhM59734179 = -255344005;    int cUrlthGdhM40926056 = -955377600;    int cUrlthGdhM79530905 = -688782808;    int cUrlthGdhM34071761 = -894304312;    int cUrlthGdhM90376865 = -733450518;    int cUrlthGdhM50100310 = -402180598;    int cUrlthGdhM59849311 = -209004684;    int cUrlthGdhM6130914 = -753150803;    int cUrlthGdhM74485817 = -310495356;    int cUrlthGdhM41846709 = -762672890;    int cUrlthGdhM44835547 = -765095430;    int cUrlthGdhM1598623 = -792635740;    int cUrlthGdhM63015302 = -10781914;    int cUrlthGdhM51773025 = -293908659;    int cUrlthGdhM57313106 = -339440962;    int cUrlthGdhM58607200 = -206271836;    int cUrlthGdhM92204932 = -365808083;    int cUrlthGdhM21416009 = -982586329;    int cUrlthGdhM60428031 = -727327241;    int cUrlthGdhM23833154 = -251531422;    int cUrlthGdhM61372275 = -657110953;    int cUrlthGdhM38625647 = -146456632;    int cUrlthGdhM54206573 = -687188437;    int cUrlthGdhM54621050 = -267504617;    int cUrlthGdhM6168406 = -635367180;    int cUrlthGdhM18411662 = -501835066;    int cUrlthGdhM27431584 = -479397651;    int cUrlthGdhM81837615 = -234160275;    int cUrlthGdhM84136078 = -643836799;    int cUrlthGdhM54929567 = -30192449;    int cUrlthGdhM2624538 = -3236977;    int cUrlthGdhM35586770 = -115184810;    int cUrlthGdhM26839451 = -771728331;    int cUrlthGdhM60574605 = -729514371;    int cUrlthGdhM3145534 = 56189580;    int cUrlthGdhM5442075 = -913564199;    int cUrlthGdhM19114667 = -755589370;    int cUrlthGdhM86390401 = -881758949;    int cUrlthGdhM61879710 = 57980033;    int cUrlthGdhM25359142 = -555373413;    int cUrlthGdhM56840263 = 60137596;     cUrlthGdhM26533558 = cUrlthGdhM53825239;     cUrlthGdhM53825239 = cUrlthGdhM94031013;     cUrlthGdhM94031013 = cUrlthGdhM11822932;     cUrlthGdhM11822932 = cUrlthGdhM11717720;     cUrlthGdhM11717720 = cUrlthGdhM23317726;     cUrlthGdhM23317726 = cUrlthGdhM98627614;     cUrlthGdhM98627614 = cUrlthGdhM84293959;     cUrlthGdhM84293959 = cUrlthGdhM78301108;     cUrlthGdhM78301108 = cUrlthGdhM6224882;     cUrlthGdhM6224882 = cUrlthGdhM60479762;     cUrlthGdhM60479762 = cUrlthGdhM12598497;     cUrlthGdhM12598497 = cUrlthGdhM81782066;     cUrlthGdhM81782066 = cUrlthGdhM97332220;     cUrlthGdhM97332220 = cUrlthGdhM98360072;     cUrlthGdhM98360072 = cUrlthGdhM81946357;     cUrlthGdhM81946357 = cUrlthGdhM71533504;     cUrlthGdhM71533504 = cUrlthGdhM14652739;     cUrlthGdhM14652739 = cUrlthGdhM3681237;     cUrlthGdhM3681237 = cUrlthGdhM57252772;     cUrlthGdhM57252772 = cUrlthGdhM12667095;     cUrlthGdhM12667095 = cUrlthGdhM73672583;     cUrlthGdhM73672583 = cUrlthGdhM35887483;     cUrlthGdhM35887483 = cUrlthGdhM40522729;     cUrlthGdhM40522729 = cUrlthGdhM63879334;     cUrlthGdhM63879334 = cUrlthGdhM51983541;     cUrlthGdhM51983541 = cUrlthGdhM82391670;     cUrlthGdhM82391670 = cUrlthGdhM19096710;     cUrlthGdhM19096710 = cUrlthGdhM50222198;     cUrlthGdhM50222198 = cUrlthGdhM87924243;     cUrlthGdhM87924243 = cUrlthGdhM56124572;     cUrlthGdhM56124572 = cUrlthGdhM630451;     cUrlthGdhM630451 = cUrlthGdhM6467583;     cUrlthGdhM6467583 = cUrlthGdhM7296250;     cUrlthGdhM7296250 = cUrlthGdhM55485511;     cUrlthGdhM55485511 = cUrlthGdhM53524526;     cUrlthGdhM53524526 = cUrlthGdhM80347735;     cUrlthGdhM80347735 = cUrlthGdhM8518203;     cUrlthGdhM8518203 = cUrlthGdhM62879713;     cUrlthGdhM62879713 = cUrlthGdhM46368131;     cUrlthGdhM46368131 = cUrlthGdhM98645572;     cUrlthGdhM98645572 = cUrlthGdhM20462163;     cUrlthGdhM20462163 = cUrlthGdhM52256575;     cUrlthGdhM52256575 = cUrlthGdhM75459452;     cUrlthGdhM75459452 = cUrlthGdhM16689575;     cUrlthGdhM16689575 = cUrlthGdhM2507059;     cUrlthGdhM2507059 = cUrlthGdhM13357894;     cUrlthGdhM13357894 = cUrlthGdhM28185097;     cUrlthGdhM28185097 = cUrlthGdhM64475659;     cUrlthGdhM64475659 = cUrlthGdhM44053793;     cUrlthGdhM44053793 = cUrlthGdhM69512581;     cUrlthGdhM69512581 = cUrlthGdhM28692989;     cUrlthGdhM28692989 = cUrlthGdhM18792836;     cUrlthGdhM18792836 = cUrlthGdhM22331504;     cUrlthGdhM22331504 = cUrlthGdhM52366682;     cUrlthGdhM52366682 = cUrlthGdhM52860974;     cUrlthGdhM52860974 = cUrlthGdhM17937756;     cUrlthGdhM17937756 = cUrlthGdhM53508285;     cUrlthGdhM53508285 = cUrlthGdhM47943598;     cUrlthGdhM47943598 = cUrlthGdhM59734179;     cUrlthGdhM59734179 = cUrlthGdhM40926056;     cUrlthGdhM40926056 = cUrlthGdhM79530905;     cUrlthGdhM79530905 = cUrlthGdhM34071761;     cUrlthGdhM34071761 = cUrlthGdhM90376865;     cUrlthGdhM90376865 = cUrlthGdhM50100310;     cUrlthGdhM50100310 = cUrlthGdhM59849311;     cUrlthGdhM59849311 = cUrlthGdhM6130914;     cUrlthGdhM6130914 = cUrlthGdhM74485817;     cUrlthGdhM74485817 = cUrlthGdhM41846709;     cUrlthGdhM41846709 = cUrlthGdhM44835547;     cUrlthGdhM44835547 = cUrlthGdhM1598623;     cUrlthGdhM1598623 = cUrlthGdhM63015302;     cUrlthGdhM63015302 = cUrlthGdhM51773025;     cUrlthGdhM51773025 = cUrlthGdhM57313106;     cUrlthGdhM57313106 = cUrlthGdhM58607200;     cUrlthGdhM58607200 = cUrlthGdhM92204932;     cUrlthGdhM92204932 = cUrlthGdhM21416009;     cUrlthGdhM21416009 = cUrlthGdhM60428031;     cUrlthGdhM60428031 = cUrlthGdhM23833154;     cUrlthGdhM23833154 = cUrlthGdhM61372275;     cUrlthGdhM61372275 = cUrlthGdhM38625647;     cUrlthGdhM38625647 = cUrlthGdhM54206573;     cUrlthGdhM54206573 = cUrlthGdhM54621050;     cUrlthGdhM54621050 = cUrlthGdhM6168406;     cUrlthGdhM6168406 = cUrlthGdhM18411662;     cUrlthGdhM18411662 = cUrlthGdhM27431584;     cUrlthGdhM27431584 = cUrlthGdhM81837615;     cUrlthGdhM81837615 = cUrlthGdhM84136078;     cUrlthGdhM84136078 = cUrlthGdhM54929567;     cUrlthGdhM54929567 = cUrlthGdhM2624538;     cUrlthGdhM2624538 = cUrlthGdhM35586770;     cUrlthGdhM35586770 = cUrlthGdhM26839451;     cUrlthGdhM26839451 = cUrlthGdhM60574605;     cUrlthGdhM60574605 = cUrlthGdhM3145534;     cUrlthGdhM3145534 = cUrlthGdhM5442075;     cUrlthGdhM5442075 = cUrlthGdhM19114667;     cUrlthGdhM19114667 = cUrlthGdhM86390401;     cUrlthGdhM86390401 = cUrlthGdhM61879710;     cUrlthGdhM61879710 = cUrlthGdhM25359142;     cUrlthGdhM25359142 = cUrlthGdhM56840263;     cUrlthGdhM56840263 = cUrlthGdhM26533558;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void tJvFQxjjxc41045898() {     int nOKPtqpudB13455235 = -80513610;    int nOKPtqpudB63472977 = -682993741;    int nOKPtqpudB86727027 = -581393122;    int nOKPtqpudB39686126 = -789756055;    int nOKPtqpudB62446807 = -386806605;    int nOKPtqpudB36783403 = -226223222;    int nOKPtqpudB48673283 = -935678443;    int nOKPtqpudB67010308 = -854262810;    int nOKPtqpudB56686798 = -508543357;    int nOKPtqpudB79725268 = -53081959;    int nOKPtqpudB80160492 = -808194135;    int nOKPtqpudB82224974 = -489495920;    int nOKPtqpudB42408516 = -761936852;    int nOKPtqpudB40650330 = -944213858;    int nOKPtqpudB22072943 = -954571718;    int nOKPtqpudB17225517 = -24737280;    int nOKPtqpudB21373680 = -295911432;    int nOKPtqpudB43869496 = -368449331;    int nOKPtqpudB64840808 = -644903730;    int nOKPtqpudB14005600 = -88741852;    int nOKPtqpudB80132763 = -186550342;    int nOKPtqpudB12362544 = -586178715;    int nOKPtqpudB5533823 = -682287501;    int nOKPtqpudB16879679 = -502644892;    int nOKPtqpudB19844320 = -922531248;    int nOKPtqpudB46162441 = -841172456;    int nOKPtqpudB39282098 = -125254393;    int nOKPtqpudB19683260 = -692746870;    int nOKPtqpudB23124992 = -845651278;    int nOKPtqpudB67637881 = -126802206;    int nOKPtqpudB15244825 = -852469055;    int nOKPtqpudB5072481 = -516022406;    int nOKPtqpudB60740665 = -97933954;    int nOKPtqpudB37219348 = -399192976;    int nOKPtqpudB48264460 = -657012661;    int nOKPtqpudB86484741 = -83306518;    int nOKPtqpudB36316999 = -221355243;    int nOKPtqpudB80846579 = -495442255;    int nOKPtqpudB16110880 = -662005584;    int nOKPtqpudB74155205 = -49334968;    int nOKPtqpudB81807002 = -166294828;    int nOKPtqpudB5303088 = -33218723;    int nOKPtqpudB28706748 = -568950534;    int nOKPtqpudB94004131 = -7549693;    int nOKPtqpudB68674435 = -672916411;    int nOKPtqpudB5073981 = -96502619;    int nOKPtqpudB34007919 = -506896997;    int nOKPtqpudB30441040 = -384733644;    int nOKPtqpudB54701287 = 52663366;    int nOKPtqpudB36653372 = -32069817;    int nOKPtqpudB10870445 = -23101798;    int nOKPtqpudB33606031 = -955955185;    int nOKPtqpudB3934438 = -918860121;    int nOKPtqpudB76876473 = -486497585;    int nOKPtqpudB60913134 = -783999001;    int nOKPtqpudB1092692 = -494334896;    int nOKPtqpudB57939154 = 99293760;    int nOKPtqpudB69847348 = 21251770;    int nOKPtqpudB19841807 = -867224808;    int nOKPtqpudB16284366 = -545634150;    int nOKPtqpudB97501305 = -968829;    int nOKPtqpudB28990024 = -142931573;    int nOKPtqpudB43885317 = 91388468;    int nOKPtqpudB89048916 = -281741152;    int nOKPtqpudB64480443 = -200612905;    int nOKPtqpudB75088011 = -192171730;    int nOKPtqpudB21484309 = -291561966;    int nOKPtqpudB5189168 = -262743876;    int nOKPtqpudB92385869 = -187201197;    int nOKPtqpudB35588202 = -771265200;    int nOKPtqpudB80908518 = -803382037;    int nOKPtqpudB40527101 = -800469178;    int nOKPtqpudB27758617 = -706443748;    int nOKPtqpudB90685602 = -495568762;    int nOKPtqpudB32198597 = -922447025;    int nOKPtqpudB74829675 = -53331620;    int nOKPtqpudB83655795 = 82771819;    int nOKPtqpudB11529692 = -574737809;    int nOKPtqpudB48205244 = -829728481;    int nOKPtqpudB14770339 = -726028629;    int nOKPtqpudB12154522 = -234275459;    int nOKPtqpudB8841058 = -740520749;    int nOKPtqpudB64981972 = -645410236;    int nOKPtqpudB86471619 = -713581462;    int nOKPtqpudB56767437 = -3700409;    int nOKPtqpudB81638793 = -896513870;    int nOKPtqpudB1138043 = -597162286;    int nOKPtqpudB83864191 = -611436370;    int nOKPtqpudB76306213 = -615193976;    int nOKPtqpudB47171769 = -62677766;    int nOKPtqpudB28545588 = -82600279;    int nOKPtqpudB66469650 = -142607013;    int nOKPtqpudB61004773 = -628217448;    int nOKPtqpudB99826513 = -16371435;    int nOKPtqpudB76653900 = 51633861;    int nOKPtqpudB52816979 = 76636745;    int nOKPtqpudB61417771 = -24607191;    int nOKPtqpudB39657832 = -187209383;    int nOKPtqpudB29523688 = -806936789;    int nOKPtqpudB93586423 = -80513610;     nOKPtqpudB13455235 = nOKPtqpudB63472977;     nOKPtqpudB63472977 = nOKPtqpudB86727027;     nOKPtqpudB86727027 = nOKPtqpudB39686126;     nOKPtqpudB39686126 = nOKPtqpudB62446807;     nOKPtqpudB62446807 = nOKPtqpudB36783403;     nOKPtqpudB36783403 = nOKPtqpudB48673283;     nOKPtqpudB48673283 = nOKPtqpudB67010308;     nOKPtqpudB67010308 = nOKPtqpudB56686798;     nOKPtqpudB56686798 = nOKPtqpudB79725268;     nOKPtqpudB79725268 = nOKPtqpudB80160492;     nOKPtqpudB80160492 = nOKPtqpudB82224974;     nOKPtqpudB82224974 = nOKPtqpudB42408516;     nOKPtqpudB42408516 = nOKPtqpudB40650330;     nOKPtqpudB40650330 = nOKPtqpudB22072943;     nOKPtqpudB22072943 = nOKPtqpudB17225517;     nOKPtqpudB17225517 = nOKPtqpudB21373680;     nOKPtqpudB21373680 = nOKPtqpudB43869496;     nOKPtqpudB43869496 = nOKPtqpudB64840808;     nOKPtqpudB64840808 = nOKPtqpudB14005600;     nOKPtqpudB14005600 = nOKPtqpudB80132763;     nOKPtqpudB80132763 = nOKPtqpudB12362544;     nOKPtqpudB12362544 = nOKPtqpudB5533823;     nOKPtqpudB5533823 = nOKPtqpudB16879679;     nOKPtqpudB16879679 = nOKPtqpudB19844320;     nOKPtqpudB19844320 = nOKPtqpudB46162441;     nOKPtqpudB46162441 = nOKPtqpudB39282098;     nOKPtqpudB39282098 = nOKPtqpudB19683260;     nOKPtqpudB19683260 = nOKPtqpudB23124992;     nOKPtqpudB23124992 = nOKPtqpudB67637881;     nOKPtqpudB67637881 = nOKPtqpudB15244825;     nOKPtqpudB15244825 = nOKPtqpudB5072481;     nOKPtqpudB5072481 = nOKPtqpudB60740665;     nOKPtqpudB60740665 = nOKPtqpudB37219348;     nOKPtqpudB37219348 = nOKPtqpudB48264460;     nOKPtqpudB48264460 = nOKPtqpudB86484741;     nOKPtqpudB86484741 = nOKPtqpudB36316999;     nOKPtqpudB36316999 = nOKPtqpudB80846579;     nOKPtqpudB80846579 = nOKPtqpudB16110880;     nOKPtqpudB16110880 = nOKPtqpudB74155205;     nOKPtqpudB74155205 = nOKPtqpudB81807002;     nOKPtqpudB81807002 = nOKPtqpudB5303088;     nOKPtqpudB5303088 = nOKPtqpudB28706748;     nOKPtqpudB28706748 = nOKPtqpudB94004131;     nOKPtqpudB94004131 = nOKPtqpudB68674435;     nOKPtqpudB68674435 = nOKPtqpudB5073981;     nOKPtqpudB5073981 = nOKPtqpudB34007919;     nOKPtqpudB34007919 = nOKPtqpudB30441040;     nOKPtqpudB30441040 = nOKPtqpudB54701287;     nOKPtqpudB54701287 = nOKPtqpudB36653372;     nOKPtqpudB36653372 = nOKPtqpudB10870445;     nOKPtqpudB10870445 = nOKPtqpudB33606031;     nOKPtqpudB33606031 = nOKPtqpudB3934438;     nOKPtqpudB3934438 = nOKPtqpudB76876473;     nOKPtqpudB76876473 = nOKPtqpudB60913134;     nOKPtqpudB60913134 = nOKPtqpudB1092692;     nOKPtqpudB1092692 = nOKPtqpudB57939154;     nOKPtqpudB57939154 = nOKPtqpudB69847348;     nOKPtqpudB69847348 = nOKPtqpudB19841807;     nOKPtqpudB19841807 = nOKPtqpudB16284366;     nOKPtqpudB16284366 = nOKPtqpudB97501305;     nOKPtqpudB97501305 = nOKPtqpudB28990024;     nOKPtqpudB28990024 = nOKPtqpudB43885317;     nOKPtqpudB43885317 = nOKPtqpudB89048916;     nOKPtqpudB89048916 = nOKPtqpudB64480443;     nOKPtqpudB64480443 = nOKPtqpudB75088011;     nOKPtqpudB75088011 = nOKPtqpudB21484309;     nOKPtqpudB21484309 = nOKPtqpudB5189168;     nOKPtqpudB5189168 = nOKPtqpudB92385869;     nOKPtqpudB92385869 = nOKPtqpudB35588202;     nOKPtqpudB35588202 = nOKPtqpudB80908518;     nOKPtqpudB80908518 = nOKPtqpudB40527101;     nOKPtqpudB40527101 = nOKPtqpudB27758617;     nOKPtqpudB27758617 = nOKPtqpudB90685602;     nOKPtqpudB90685602 = nOKPtqpudB32198597;     nOKPtqpudB32198597 = nOKPtqpudB74829675;     nOKPtqpudB74829675 = nOKPtqpudB83655795;     nOKPtqpudB83655795 = nOKPtqpudB11529692;     nOKPtqpudB11529692 = nOKPtqpudB48205244;     nOKPtqpudB48205244 = nOKPtqpudB14770339;     nOKPtqpudB14770339 = nOKPtqpudB12154522;     nOKPtqpudB12154522 = nOKPtqpudB8841058;     nOKPtqpudB8841058 = nOKPtqpudB64981972;     nOKPtqpudB64981972 = nOKPtqpudB86471619;     nOKPtqpudB86471619 = nOKPtqpudB56767437;     nOKPtqpudB56767437 = nOKPtqpudB81638793;     nOKPtqpudB81638793 = nOKPtqpudB1138043;     nOKPtqpudB1138043 = nOKPtqpudB83864191;     nOKPtqpudB83864191 = nOKPtqpudB76306213;     nOKPtqpudB76306213 = nOKPtqpudB47171769;     nOKPtqpudB47171769 = nOKPtqpudB28545588;     nOKPtqpudB28545588 = nOKPtqpudB66469650;     nOKPtqpudB66469650 = nOKPtqpudB61004773;     nOKPtqpudB61004773 = nOKPtqpudB99826513;     nOKPtqpudB99826513 = nOKPtqpudB76653900;     nOKPtqpudB76653900 = nOKPtqpudB52816979;     nOKPtqpudB52816979 = nOKPtqpudB61417771;     nOKPtqpudB61417771 = nOKPtqpudB39657832;     nOKPtqpudB39657832 = nOKPtqpudB29523688;     nOKPtqpudB29523688 = nOKPtqpudB93586423;     nOKPtqpudB93586423 = nOKPtqpudB13455235;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void vcgMRNEnRM17058018() {     int OJyFwAiysJ376913 = -221164815;    int OJyFwAiysJ73120715 = -793580314;    int OJyFwAiysJ79423041 = -12508504;    int OJyFwAiysJ67549320 = -795553345;    int OJyFwAiysJ13175894 = -240620082;    int OJyFwAiysJ50249080 = -623916516;    int OJyFwAiysJ98718951 = -617476481;    int OJyFwAiysJ49726656 = -284802631;    int OJyFwAiysJ35072488 = -718054059;    int OJyFwAiysJ53225654 = -867943165;    int OJyFwAiysJ99841222 = -962282879;    int OJyFwAiysJ51851452 = -611598805;    int OJyFwAiysJ3034965 = -850300141;    int OJyFwAiysJ83968439 = -740712257;    int OJyFwAiysJ45785812 = -41337268;    int OJyFwAiysJ52504675 = -3388866;    int OJyFwAiysJ71213855 = -965831269;    int OJyFwAiysJ73086253 = -43835575;    int OJyFwAiysJ26000379 = 18575299;    int OJyFwAiysJ70758427 = -526839691;    int OJyFwAiysJ47598432 = -231229335;    int OJyFwAiysJ51052503 = -414300617;    int OJyFwAiysJ75180163 = -579693751;    int OJyFwAiysJ93236629 = -436733667;    int OJyFwAiysJ75809304 = -946799027;    int OJyFwAiysJ40341341 = -404695801;    int OJyFwAiysJ96172526 = -377356458;    int OJyFwAiysJ20269810 = -920396137;    int OJyFwAiysJ96027784 = -161883910;    int OJyFwAiysJ47351520 = -788022276;    int OJyFwAiysJ74365076 = -768897954;    int OJyFwAiysJ9514511 = -686944108;    int OJyFwAiysJ15013748 = -681625688;    int OJyFwAiysJ67142445 = -535307745;    int OJyFwAiysJ41043409 = 71017242;    int OJyFwAiysJ19444956 = -263902335;    int OJyFwAiysJ92286262 = -189260533;    int OJyFwAiysJ53174956 = -375674825;    int OJyFwAiysJ69342046 = 75143245;    int OJyFwAiysJ1942281 = -329728138;    int OJyFwAiysJ64968433 = -988217478;    int OJyFwAiysJ90144013 = -390374205;    int OJyFwAiysJ5156922 = -362430586;    int OJyFwAiysJ12548811 = -57545376;    int OJyFwAiysJ20659296 = -28808129;    int OJyFwAiysJ7640903 = -51852753;    int OJyFwAiysJ54657945 = 17398489;    int OJyFwAiysJ32696984 = -583503398;    int OJyFwAiysJ44926916 = -897080313;    int OJyFwAiysJ29252952 = -370088146;    int OJyFwAiysJ52228307 = -82456524;    int OJyFwAiysJ38519073 = -455267868;    int OJyFwAiysJ89076040 = -726779805;    int OJyFwAiysJ31421443 = -2589745;    int OJyFwAiysJ69459585 = -335112248;    int OJyFwAiysJ49324410 = -806864198;    int OJyFwAiysJ97940552 = -113886563;    int OJyFwAiysJ86186412 = -575774838;    int OJyFwAiysJ91740015 = -848754319;    int OJyFwAiysJ72834553 = -835924282;    int OJyFwAiysJ54076554 = -146560059;    int OJyFwAiysJ78449142 = -697080345;    int OJyFwAiysJ53698872 = -22918722;    int OJyFwAiysJ87720968 = -930031784;    int OJyFwAiysJ78860577 = 954789;    int OJyFwAiysJ90326711 = -175338771;    int OJyFwAiysJ36837704 = -929973117;    int OJyFwAiysJ35892519 = -214992396;    int OJyFwAiysJ42925030 = -711729499;    int OJyFwAiysJ26340856 = -777434933;    int OJyFwAiysJ60218413 = -814128334;    int OJyFwAiysJ18038899 = -490156445;    int OJyFwAiysJ3744208 = -18978820;    int OJyFwAiysJ24058098 = -651696563;    int OJyFwAiysJ5789994 = -538622213;    int OJyFwAiysJ57454418 = -840855131;    int OJyFwAiysJ45895581 = 48129968;    int OJyFwAiysJ62631352 = -422148376;    int OJyFwAiysJ72577334 = -307925538;    int OJyFwAiysJ68168402 = -794946275;    int OJyFwAiysJ85683395 = -322094290;    int OJyFwAiysJ63475542 = -793853061;    int OJyFwAiysJ75342893 = 76684176;    int OJyFwAiysJ66774833 = -791795764;    int OJyFwAiysJ95123213 = -605565752;    int OJyFwAiysJ35846004 = -213630086;    int OJyFwAiysJ20438470 = -960164303;    int OJyFwAiysJ83592305 = -579035943;    int OJyFwAiysJ97682860 = -100195498;    int OJyFwAiysJ91718999 = -122118560;    int OJyFwAiysJ21504404 = -50015772;    int OJyFwAiysJ6099850 = -613485696;    int OJyFwAiysJ61434940 = -526920506;    int OJyFwAiysJ96507492 = -88932474;    int OJyFwAiysJ47865726 = -83168079;    int OJyFwAiysJ86519291 = -191137134;    int OJyFwAiysJ36445142 = -267455484;    int OJyFwAiysJ17435954 = -432398803;    int OJyFwAiysJ33688233 = 41499835;    int OJyFwAiysJ30332584 = -221164815;     OJyFwAiysJ376913 = OJyFwAiysJ73120715;     OJyFwAiysJ73120715 = OJyFwAiysJ79423041;     OJyFwAiysJ79423041 = OJyFwAiysJ67549320;     OJyFwAiysJ67549320 = OJyFwAiysJ13175894;     OJyFwAiysJ13175894 = OJyFwAiysJ50249080;     OJyFwAiysJ50249080 = OJyFwAiysJ98718951;     OJyFwAiysJ98718951 = OJyFwAiysJ49726656;     OJyFwAiysJ49726656 = OJyFwAiysJ35072488;     OJyFwAiysJ35072488 = OJyFwAiysJ53225654;     OJyFwAiysJ53225654 = OJyFwAiysJ99841222;     OJyFwAiysJ99841222 = OJyFwAiysJ51851452;     OJyFwAiysJ51851452 = OJyFwAiysJ3034965;     OJyFwAiysJ3034965 = OJyFwAiysJ83968439;     OJyFwAiysJ83968439 = OJyFwAiysJ45785812;     OJyFwAiysJ45785812 = OJyFwAiysJ52504675;     OJyFwAiysJ52504675 = OJyFwAiysJ71213855;     OJyFwAiysJ71213855 = OJyFwAiysJ73086253;     OJyFwAiysJ73086253 = OJyFwAiysJ26000379;     OJyFwAiysJ26000379 = OJyFwAiysJ70758427;     OJyFwAiysJ70758427 = OJyFwAiysJ47598432;     OJyFwAiysJ47598432 = OJyFwAiysJ51052503;     OJyFwAiysJ51052503 = OJyFwAiysJ75180163;     OJyFwAiysJ75180163 = OJyFwAiysJ93236629;     OJyFwAiysJ93236629 = OJyFwAiysJ75809304;     OJyFwAiysJ75809304 = OJyFwAiysJ40341341;     OJyFwAiysJ40341341 = OJyFwAiysJ96172526;     OJyFwAiysJ96172526 = OJyFwAiysJ20269810;     OJyFwAiysJ20269810 = OJyFwAiysJ96027784;     OJyFwAiysJ96027784 = OJyFwAiysJ47351520;     OJyFwAiysJ47351520 = OJyFwAiysJ74365076;     OJyFwAiysJ74365076 = OJyFwAiysJ9514511;     OJyFwAiysJ9514511 = OJyFwAiysJ15013748;     OJyFwAiysJ15013748 = OJyFwAiysJ67142445;     OJyFwAiysJ67142445 = OJyFwAiysJ41043409;     OJyFwAiysJ41043409 = OJyFwAiysJ19444956;     OJyFwAiysJ19444956 = OJyFwAiysJ92286262;     OJyFwAiysJ92286262 = OJyFwAiysJ53174956;     OJyFwAiysJ53174956 = OJyFwAiysJ69342046;     OJyFwAiysJ69342046 = OJyFwAiysJ1942281;     OJyFwAiysJ1942281 = OJyFwAiysJ64968433;     OJyFwAiysJ64968433 = OJyFwAiysJ90144013;     OJyFwAiysJ90144013 = OJyFwAiysJ5156922;     OJyFwAiysJ5156922 = OJyFwAiysJ12548811;     OJyFwAiysJ12548811 = OJyFwAiysJ20659296;     OJyFwAiysJ20659296 = OJyFwAiysJ7640903;     OJyFwAiysJ7640903 = OJyFwAiysJ54657945;     OJyFwAiysJ54657945 = OJyFwAiysJ32696984;     OJyFwAiysJ32696984 = OJyFwAiysJ44926916;     OJyFwAiysJ44926916 = OJyFwAiysJ29252952;     OJyFwAiysJ29252952 = OJyFwAiysJ52228307;     OJyFwAiysJ52228307 = OJyFwAiysJ38519073;     OJyFwAiysJ38519073 = OJyFwAiysJ89076040;     OJyFwAiysJ89076040 = OJyFwAiysJ31421443;     OJyFwAiysJ31421443 = OJyFwAiysJ69459585;     OJyFwAiysJ69459585 = OJyFwAiysJ49324410;     OJyFwAiysJ49324410 = OJyFwAiysJ97940552;     OJyFwAiysJ97940552 = OJyFwAiysJ86186412;     OJyFwAiysJ86186412 = OJyFwAiysJ91740015;     OJyFwAiysJ91740015 = OJyFwAiysJ72834553;     OJyFwAiysJ72834553 = OJyFwAiysJ54076554;     OJyFwAiysJ54076554 = OJyFwAiysJ78449142;     OJyFwAiysJ78449142 = OJyFwAiysJ53698872;     OJyFwAiysJ53698872 = OJyFwAiysJ87720968;     OJyFwAiysJ87720968 = OJyFwAiysJ78860577;     OJyFwAiysJ78860577 = OJyFwAiysJ90326711;     OJyFwAiysJ90326711 = OJyFwAiysJ36837704;     OJyFwAiysJ36837704 = OJyFwAiysJ35892519;     OJyFwAiysJ35892519 = OJyFwAiysJ42925030;     OJyFwAiysJ42925030 = OJyFwAiysJ26340856;     OJyFwAiysJ26340856 = OJyFwAiysJ60218413;     OJyFwAiysJ60218413 = OJyFwAiysJ18038899;     OJyFwAiysJ18038899 = OJyFwAiysJ3744208;     OJyFwAiysJ3744208 = OJyFwAiysJ24058098;     OJyFwAiysJ24058098 = OJyFwAiysJ5789994;     OJyFwAiysJ5789994 = OJyFwAiysJ57454418;     OJyFwAiysJ57454418 = OJyFwAiysJ45895581;     OJyFwAiysJ45895581 = OJyFwAiysJ62631352;     OJyFwAiysJ62631352 = OJyFwAiysJ72577334;     OJyFwAiysJ72577334 = OJyFwAiysJ68168402;     OJyFwAiysJ68168402 = OJyFwAiysJ85683395;     OJyFwAiysJ85683395 = OJyFwAiysJ63475542;     OJyFwAiysJ63475542 = OJyFwAiysJ75342893;     OJyFwAiysJ75342893 = OJyFwAiysJ66774833;     OJyFwAiysJ66774833 = OJyFwAiysJ95123213;     OJyFwAiysJ95123213 = OJyFwAiysJ35846004;     OJyFwAiysJ35846004 = OJyFwAiysJ20438470;     OJyFwAiysJ20438470 = OJyFwAiysJ83592305;     OJyFwAiysJ83592305 = OJyFwAiysJ97682860;     OJyFwAiysJ97682860 = OJyFwAiysJ91718999;     OJyFwAiysJ91718999 = OJyFwAiysJ21504404;     OJyFwAiysJ21504404 = OJyFwAiysJ6099850;     OJyFwAiysJ6099850 = OJyFwAiysJ61434940;     OJyFwAiysJ61434940 = OJyFwAiysJ96507492;     OJyFwAiysJ96507492 = OJyFwAiysJ47865726;     OJyFwAiysJ47865726 = OJyFwAiysJ86519291;     OJyFwAiysJ86519291 = OJyFwAiysJ36445142;     OJyFwAiysJ36445142 = OJyFwAiysJ17435954;     OJyFwAiysJ17435954 = OJyFwAiysJ33688233;     OJyFwAiysJ33688233 = OJyFwAiysJ30332584;     OJyFwAiysJ30332584 = OJyFwAiysJ376913;}
// Junk Finished
