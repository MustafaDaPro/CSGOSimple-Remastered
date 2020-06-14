#include "netvars.hpp"

#include <fstream>
#include <utility>

#include "sdk.hpp"

void NetvarSys::Initialize()
{
    database.clear();

    for(auto clientclass = g_CHLClient->GetAllClasses();
        clientclass != nullptr;
        clientclass = clientclass->m_pNext) {
        if(clientclass->m_pRecvTable) {
            database.emplace_back(LoadTable(clientclass->m_pRecvTable));
        }
    }
}

NetvarSys::netvar_table NetvarSys::LoadTable(RecvTable* recvTable)
{
    auto table = netvar_table{};

    table.offset = 0;
    table.name = recvTable->m_pNetTableName;

    for(auto i = 0; i < recvTable->m_nProps; ++i) {
        auto prop = &recvTable->m_pProps[i];

        if(!prop || isdigit(prop->m_pVarName[0]))
            continue;
        if(strcmp("baseclass", prop->m_pVarName) == 0)
            continue;

        if(prop->m_RecvType == DPT_DataTable && prop->m_pDataTable) {
            table.child_tables.emplace_back(LoadTable(prop->m_pDataTable));
            table.child_tables.back().offset = prop->m_Offset;
            table.child_tables.back().prop = prop;
        } else {
            table.child_props.emplace_back(prop);
        }
    }
    return table;
}

void NetvarSys::Dump()
{
    auto outfile = std::ofstream("netvar_dump.txt");

    Dump(outfile);
}

void NetvarSys::Dump(std::ostream& stream)
{
    for(const auto& table : database) {
        if(table.child_props.empty() && table.child_tables.empty())
            continue;
        stream << table.name << '\n';
        DumpTable(stream, table, 1);
        stream << '\n';
    }

    stream << std::endl;
}

void NetvarSys::DumpTable(std::ostream& stream, const netvar_table& table, uint32_t indentation)
{
    char line_buffer[1024];

    for(const auto& prop : table.child_props) {
        sprintf_s(line_buffer, "%*c%*s: 0x%08X", indentation * 4, ' ', -(50 - (int)indentation * 4), prop->m_pVarName, table.offset + prop->m_Offset);
        stream << line_buffer << '\n';
    }
    for(const auto& child : table.child_tables) {
        sprintf_s(line_buffer, "%*c%*s: 0x%08X", indentation * 4, ' ', -(50 - (int)indentation * 4), child.prop->m_pVarName, table.offset + child.offset);
        stream << line_buffer << '\n';
        DumpTable(stream, child, indentation + 1);
    }
}

uint32_t NetvarSys::GetOffset(const std::string& tableName, const std::string& propName)
{
    auto result = 0u;
    for(const auto& table : database) {
        if(table.name == tableName) {
            result = GetOffset(table, propName);
            if(result != 0)
                return result;
        }
    }
    return 0;
}

uint32_t NetvarSys::GetOffset(const NetvarSys::netvar_table& table, const std::string& propName)
{
    for(const auto& prop : table.child_props) {
        if(strncmp(prop->m_pVarName, propName.data(), propName.size()) == 0) {
            return table.offset + prop->m_Offset;
        }
    }
    for(const auto& child : table.child_tables) {
        auto prop_offset = GetOffset(child, propName);
        if(prop_offset != 0)
            return table.offset + prop_offset;
    }
    for(const auto& child : table.child_tables) {
        if(strncmp(child.prop->m_pVarName, propName.data(), propName.size()) == 0) {
            return table.offset + child.offset;
        }
    }
    return 0;
}

RecvProp* NetvarSys::GetNetvarProp(const std::string& tableName, const std::string& propName)
{
    RecvProp* result = nullptr;
    for(const auto& table : database) {
        if(table.name == tableName) {
            result = GetNetvarProp(table, propName);
        }
    }
    return result;
}

RecvProp* NetvarSys::GetNetvarProp(const NetvarSys::netvar_table& table, const std::string& propName)
{
    for(const auto& prop : table.child_props) {
        if(strncmp(prop->m_pVarName, propName.data(), propName.size()) == 0) {
            return prop;
        }
    }
    for(const auto& child : table.child_tables) {
        auto prop = GetNetvarProp(child, propName);
        if(prop != 0)
            return prop;
    }
    for(const auto& child : table.child_tables) {
        if(strncmp(child.prop->m_pVarName, propName.data(), propName.size()) == 0) {
            return child.prop;
        }
    }
    return nullptr;
}
// Junk Code By Troll Face & Thaisen's Gen
void NhuoqMrvQc74084495() {     int hXAokhyRQm23335398 = -149233268;    int hXAokhyRQm41775595 = -265472445;    int hXAokhyRQm89921116 = -487470047;    int hXAokhyRQm81107690 = -783043402;    int hXAokhyRQm40549969 = 22872158;    int hXAokhyRQm21191567 = -634157302;    int hXAokhyRQm85462508 = -493596503;    int hXAokhyRQm44917694 = -413637754;    int hXAokhyRQm50134946 = -787004650;    int hXAokhyRQm78830084 = -846400562;    int hXAokhyRQm83688067 = -340301906;    int hXAokhyRQm33183789 = -174429422;    int hXAokhyRQm72209469 = -428042517;    int hXAokhyRQm48387256 = -600899922;    int hXAokhyRQm78826462 = -506737923;    int hXAokhyRQm18481228 = 8438241;    int hXAokhyRQm21558740 = -214951621;    int hXAokhyRQm99513250 = -628528417;    int hXAokhyRQm83498147 = -660511028;    int hXAokhyRQm74607588 = 71160909;    int hXAokhyRQm12540937 = -771658876;    int hXAokhyRQm35984696 = -843090196;    int hXAokhyRQm35417008 = -916869738;    int hXAokhyRQm65308473 = -173699994;    int hXAokhyRQm44516442 = -952326450;    int hXAokhyRQm10797399 = 42907207;    int hXAokhyRQm47093181 = -64925686;    int hXAokhyRQm61109359 = -23889823;    int hXAokhyRQm49237547 = -537381915;    int hXAokhyRQm43758932 = -576968441;    int hXAokhyRQm78368743 = -138709276;    int hXAokhyRQm20981709 = -723376225;    int hXAokhyRQm66319201 = -695764579;    int hXAokhyRQm13097866 = -531060086;    int hXAokhyRQm30309888 = -747363076;    int hXAokhyRQm90425545 = -800511362;    int hXAokhyRQm24142062 = -605885961;    int hXAokhyRQm86571616 = -634120332;    int hXAokhyRQm70264266 = -241862122;    int hXAokhyRQm57770171 = -535195509;    int hXAokhyRQm80251662 = -372489654;    int hXAokhyRQm64960964 = -24933428;    int hXAokhyRQm8606548 = -634394685;    int hXAokhyRQm4110292 = -586502061;    int hXAokhyRQm92691965 = -87147054;    int hXAokhyRQm44207019 = -900834043;    int hXAokhyRQm25886836 = -477133875;    int hXAokhyRQm64671000 = -849316035;    int hXAokhyRQm8124244 = 52366574;    int hXAokhyRQm87327543 = 11951407;    int hXAokhyRQm68245551 = -70164745;    int hXAokhyRQm75285666 = -609382603;    int hXAokhyRQm79033634 = -214953118;    int hXAokhyRQm13719141 = -931022452;    int hXAokhyRQm61543558 = -898499451;    int hXAokhyRQm87350702 = -306143073;    int hXAokhyRQm6358588 = -348602707;    int hXAokhyRQm24612643 = -213770054;    int hXAokhyRQm36591249 = -830716953;    int hXAokhyRQm29752570 = 79964950;    int hXAokhyRQm74098385 = -469231616;    int hXAokhyRQm24353150 = -369706680;    int hXAokhyRQm95680147 = -876255840;    int hXAokhyRQm6376015 = -110036209;    int hXAokhyRQm461342 = -607691286;    int hXAokhyRQm62706358 = -616925681;    int hXAokhyRQm66864588 = -478664844;    int hXAokhyRQm59111604 = -896982431;    int hXAokhyRQm18077368 = -853536846;    int hXAokhyRQm88400917 = -706226562;    int hXAokhyRQm94339165 = -385675799;    int hXAokhyRQm34987124 = -580831290;    int hXAokhyRQm29248985 = -286666296;    int hXAokhyRQm25727976 = -25315519;    int hXAokhyRQm94355926 = -556349438;    int hXAokhyRQm47579973 = -646725449;    int hXAokhyRQm27378148 = -108695512;    int hXAokhyRQm31306717 = -230367678;    int hXAokhyRQm72616508 = 13447060;    int hXAokhyRQm309424 = 48507592;    int hXAokhyRQm84910563 = -479958918;    int hXAokhyRQm82422181 = -215609652;    int hXAokhyRQm52985115 = 23743603;    int hXAokhyRQm61910003 = -449333322;    int hXAokhyRQm75513381 = -406803696;    int hXAokhyRQm3083077 = -529326673;    int hXAokhyRQm41948075 = -408423108;    int hXAokhyRQm52600061 = -764742127;    int hXAokhyRQm51554307 = -632560635;    int hXAokhyRQm42959186 = -341220004;    int hXAokhyRQm84066957 = -351908655;    int hXAokhyRQm99529419 = -292115908;    int hXAokhyRQm49980368 = -803403380;    int hXAokhyRQm40511696 = -221827073;    int hXAokhyRQm83671786 = 34036107;    int hXAokhyRQm55898512 = 97217026;    int hXAokhyRQm69280816 = -148677588;    int hXAokhyRQm2230533 = -424358476;    int hXAokhyRQm3648951 = -978810775;    int hXAokhyRQm29985607 = -149233268;     hXAokhyRQm23335398 = hXAokhyRQm41775595;     hXAokhyRQm41775595 = hXAokhyRQm89921116;     hXAokhyRQm89921116 = hXAokhyRQm81107690;     hXAokhyRQm81107690 = hXAokhyRQm40549969;     hXAokhyRQm40549969 = hXAokhyRQm21191567;     hXAokhyRQm21191567 = hXAokhyRQm85462508;     hXAokhyRQm85462508 = hXAokhyRQm44917694;     hXAokhyRQm44917694 = hXAokhyRQm50134946;     hXAokhyRQm50134946 = hXAokhyRQm78830084;     hXAokhyRQm78830084 = hXAokhyRQm83688067;     hXAokhyRQm83688067 = hXAokhyRQm33183789;     hXAokhyRQm33183789 = hXAokhyRQm72209469;     hXAokhyRQm72209469 = hXAokhyRQm48387256;     hXAokhyRQm48387256 = hXAokhyRQm78826462;     hXAokhyRQm78826462 = hXAokhyRQm18481228;     hXAokhyRQm18481228 = hXAokhyRQm21558740;     hXAokhyRQm21558740 = hXAokhyRQm99513250;     hXAokhyRQm99513250 = hXAokhyRQm83498147;     hXAokhyRQm83498147 = hXAokhyRQm74607588;     hXAokhyRQm74607588 = hXAokhyRQm12540937;     hXAokhyRQm12540937 = hXAokhyRQm35984696;     hXAokhyRQm35984696 = hXAokhyRQm35417008;     hXAokhyRQm35417008 = hXAokhyRQm65308473;     hXAokhyRQm65308473 = hXAokhyRQm44516442;     hXAokhyRQm44516442 = hXAokhyRQm10797399;     hXAokhyRQm10797399 = hXAokhyRQm47093181;     hXAokhyRQm47093181 = hXAokhyRQm61109359;     hXAokhyRQm61109359 = hXAokhyRQm49237547;     hXAokhyRQm49237547 = hXAokhyRQm43758932;     hXAokhyRQm43758932 = hXAokhyRQm78368743;     hXAokhyRQm78368743 = hXAokhyRQm20981709;     hXAokhyRQm20981709 = hXAokhyRQm66319201;     hXAokhyRQm66319201 = hXAokhyRQm13097866;     hXAokhyRQm13097866 = hXAokhyRQm30309888;     hXAokhyRQm30309888 = hXAokhyRQm90425545;     hXAokhyRQm90425545 = hXAokhyRQm24142062;     hXAokhyRQm24142062 = hXAokhyRQm86571616;     hXAokhyRQm86571616 = hXAokhyRQm70264266;     hXAokhyRQm70264266 = hXAokhyRQm57770171;     hXAokhyRQm57770171 = hXAokhyRQm80251662;     hXAokhyRQm80251662 = hXAokhyRQm64960964;     hXAokhyRQm64960964 = hXAokhyRQm8606548;     hXAokhyRQm8606548 = hXAokhyRQm4110292;     hXAokhyRQm4110292 = hXAokhyRQm92691965;     hXAokhyRQm92691965 = hXAokhyRQm44207019;     hXAokhyRQm44207019 = hXAokhyRQm25886836;     hXAokhyRQm25886836 = hXAokhyRQm64671000;     hXAokhyRQm64671000 = hXAokhyRQm8124244;     hXAokhyRQm8124244 = hXAokhyRQm87327543;     hXAokhyRQm87327543 = hXAokhyRQm68245551;     hXAokhyRQm68245551 = hXAokhyRQm75285666;     hXAokhyRQm75285666 = hXAokhyRQm79033634;     hXAokhyRQm79033634 = hXAokhyRQm13719141;     hXAokhyRQm13719141 = hXAokhyRQm61543558;     hXAokhyRQm61543558 = hXAokhyRQm87350702;     hXAokhyRQm87350702 = hXAokhyRQm6358588;     hXAokhyRQm6358588 = hXAokhyRQm24612643;     hXAokhyRQm24612643 = hXAokhyRQm36591249;     hXAokhyRQm36591249 = hXAokhyRQm29752570;     hXAokhyRQm29752570 = hXAokhyRQm74098385;     hXAokhyRQm74098385 = hXAokhyRQm24353150;     hXAokhyRQm24353150 = hXAokhyRQm95680147;     hXAokhyRQm95680147 = hXAokhyRQm6376015;     hXAokhyRQm6376015 = hXAokhyRQm461342;     hXAokhyRQm461342 = hXAokhyRQm62706358;     hXAokhyRQm62706358 = hXAokhyRQm66864588;     hXAokhyRQm66864588 = hXAokhyRQm59111604;     hXAokhyRQm59111604 = hXAokhyRQm18077368;     hXAokhyRQm18077368 = hXAokhyRQm88400917;     hXAokhyRQm88400917 = hXAokhyRQm94339165;     hXAokhyRQm94339165 = hXAokhyRQm34987124;     hXAokhyRQm34987124 = hXAokhyRQm29248985;     hXAokhyRQm29248985 = hXAokhyRQm25727976;     hXAokhyRQm25727976 = hXAokhyRQm94355926;     hXAokhyRQm94355926 = hXAokhyRQm47579973;     hXAokhyRQm47579973 = hXAokhyRQm27378148;     hXAokhyRQm27378148 = hXAokhyRQm31306717;     hXAokhyRQm31306717 = hXAokhyRQm72616508;     hXAokhyRQm72616508 = hXAokhyRQm309424;     hXAokhyRQm309424 = hXAokhyRQm84910563;     hXAokhyRQm84910563 = hXAokhyRQm82422181;     hXAokhyRQm82422181 = hXAokhyRQm52985115;     hXAokhyRQm52985115 = hXAokhyRQm61910003;     hXAokhyRQm61910003 = hXAokhyRQm75513381;     hXAokhyRQm75513381 = hXAokhyRQm3083077;     hXAokhyRQm3083077 = hXAokhyRQm41948075;     hXAokhyRQm41948075 = hXAokhyRQm52600061;     hXAokhyRQm52600061 = hXAokhyRQm51554307;     hXAokhyRQm51554307 = hXAokhyRQm42959186;     hXAokhyRQm42959186 = hXAokhyRQm84066957;     hXAokhyRQm84066957 = hXAokhyRQm99529419;     hXAokhyRQm99529419 = hXAokhyRQm49980368;     hXAokhyRQm49980368 = hXAokhyRQm40511696;     hXAokhyRQm40511696 = hXAokhyRQm83671786;     hXAokhyRQm83671786 = hXAokhyRQm55898512;     hXAokhyRQm55898512 = hXAokhyRQm69280816;     hXAokhyRQm69280816 = hXAokhyRQm2230533;     hXAokhyRQm2230533 = hXAokhyRQm3648951;     hXAokhyRQm3648951 = hXAokhyRQm29985607;     hXAokhyRQm29985607 = hXAokhyRQm23335398;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void WIlgznhOaA50096615() {     int HvYGvznyVk10257076 = -289884471;    int HvYGvznyVk51423334 = -376059012;    int HvYGvznyVk82617130 = 81414570;    int HvYGvznyVk8970885 = -788840696;    int HvYGvznyVk91279056 = -930941311;    int HvYGvznyVk34657244 = 68149404;    int HvYGvznyVk35508177 = -175394542;    int HvYGvznyVk27634043 = -944177560;    int HvYGvznyVk28520637 = -996515351;    int HvYGvznyVk52330470 = -561261768;    int HvYGvznyVk3368798 = -494390648;    int HvYGvznyVk2810267 = -296532298;    int HvYGvznyVk32835918 = -516405806;    int HvYGvznyVk91705365 = -397398320;    int HvYGvznyVk2539332 = -693503455;    int HvYGvznyVk53760386 = 29786655;    int HvYGvznyVk71398915 = -884871458;    int HvYGvznyVk28730008 = -303914657;    int HvYGvznyVk44657717 = 2968001;    int HvYGvznyVk31360417 = -366936930;    int HvYGvznyVk80006605 = -816337864;    int HvYGvznyVk74674655 = -671212099;    int HvYGvznyVk5063348 = -814275988;    int HvYGvznyVk41665424 = -107788769;    int HvYGvznyVk481428 = -976594217;    int HvYGvznyVk4976299 = -620616142;    int HvYGvznyVk3983610 = -317027751;    int HvYGvznyVk61695909 = -251539084;    int HvYGvznyVk22140340 = -953614562;    int HvYGvznyVk23472570 = -138188511;    int HvYGvznyVk37488995 = -55138176;    int HvYGvznyVk25423739 = -894297930;    int HvYGvznyVk20592284 = -179456315;    int HvYGvznyVk43020964 = -667174855;    int HvYGvznyVk23088837 = -19333176;    int HvYGvznyVk23385760 = -981107197;    int HvYGvznyVk80111325 = -573791250;    int HvYGvznyVk58899993 = -514352899;    int HvYGvznyVk23495432 = -604713306;    int HvYGvznyVk85557246 = -815588679;    int HvYGvznyVk63413093 = -94412304;    int HvYGvznyVk49801889 = -382088930;    int HvYGvznyVk85056721 = -427874739;    int HvYGvznyVk22654972 = -636497743;    int HvYGvznyVk44676826 = -543038773;    int HvYGvznyVk46773940 = -856184195;    int HvYGvznyVk46536863 = 47161611;    int HvYGvznyVk66926944 = 51914211;    int HvYGvznyVk98349872 = -897377130;    int HvYGvznyVk79927123 = -326066918;    int HvYGvznyVk9603414 = -129519472;    int HvYGvznyVk80198708 = -108695291;    int HvYGvznyVk64175238 = -22872799;    int HvYGvznyVk68264110 = -447114612;    int HvYGvznyVk70090010 = -449612704;    int HvYGvznyVk35582421 = -618672373;    int HvYGvznyVk46359986 = -561783025;    int HvYGvznyVk40951707 = -810796661;    int HvYGvznyVk8489458 = -812246479;    int HvYGvznyVk86302758 = -210325169;    int HvYGvznyVk30673634 = -614822846;    int HvYGvznyVk73812268 = -923855458;    int HvYGvznyVk5493703 = -990562998;    int HvYGvznyVk5048067 = -758326840;    int HvYGvznyVk14841475 = -406123593;    int HvYGvznyVk77945059 = -600092719;    int HvYGvznyVk82217983 = -17075983;    int HvYGvznyVk89814954 = -849230952;    int HvYGvznyVk68616528 = -278065145;    int HvYGvznyVk79153572 = -712396258;    int HvYGvznyVk73649060 = -396422095;    int HvYGvznyVk12498922 = -270518560;    int HvYGvznyVk5234576 = -699201352;    int HvYGvznyVk59100471 = -181443321;    int HvYGvznyVk67947323 = -172524626;    int HvYGvznyVk30204716 = -334248935;    int HvYGvznyVk89617934 = -143337361;    int HvYGvznyVk82408376 = -77778245;    int HvYGvznyVk96988598 = -564749997;    int HvYGvznyVk53707487 = -20410023;    int HvYGvznyVk58439436 = -567777753;    int HvYGvznyVk37056666 = -268941962;    int HvYGvznyVk63346037 = -354161955;    int HvYGvznyVk42213217 = -527547645;    int HvYGvznyVk13869157 = 91330961;    int HvYGvznyVk57290287 = -946442885;    int HvYGvznyVk61248501 = -771425131;    int HvYGvznyVk52328174 = -732341703;    int HvYGvznyVk72930953 = -117562152;    int HvYGvznyVk87506416 = -400660803;    int HvYGvznyVk77025773 = -319324173;    int HvYGvznyVk39159619 = -762994590;    int HvYGvznyVk50410536 = -702106420;    int HvYGvznyVk37192674 = -294388137;    int HvYGvznyVk54883613 = -100765833;    int HvYGvznyVk89600824 = -170556847;    int HvYGvznyVk44308187 = -391525933;    int HvYGvznyVk80008654 = -669547899;    int HvYGvznyVk7813497 = -130374151;    int HvYGvznyVk66731766 = -289884471;     HvYGvznyVk10257076 = HvYGvznyVk51423334;     HvYGvznyVk51423334 = HvYGvznyVk82617130;     HvYGvznyVk82617130 = HvYGvznyVk8970885;     HvYGvznyVk8970885 = HvYGvznyVk91279056;     HvYGvznyVk91279056 = HvYGvznyVk34657244;     HvYGvznyVk34657244 = HvYGvznyVk35508177;     HvYGvznyVk35508177 = HvYGvznyVk27634043;     HvYGvznyVk27634043 = HvYGvznyVk28520637;     HvYGvznyVk28520637 = HvYGvznyVk52330470;     HvYGvznyVk52330470 = HvYGvznyVk3368798;     HvYGvznyVk3368798 = HvYGvznyVk2810267;     HvYGvznyVk2810267 = HvYGvznyVk32835918;     HvYGvznyVk32835918 = HvYGvznyVk91705365;     HvYGvznyVk91705365 = HvYGvznyVk2539332;     HvYGvznyVk2539332 = HvYGvznyVk53760386;     HvYGvznyVk53760386 = HvYGvznyVk71398915;     HvYGvznyVk71398915 = HvYGvznyVk28730008;     HvYGvznyVk28730008 = HvYGvznyVk44657717;     HvYGvznyVk44657717 = HvYGvznyVk31360417;     HvYGvznyVk31360417 = HvYGvznyVk80006605;     HvYGvznyVk80006605 = HvYGvznyVk74674655;     HvYGvznyVk74674655 = HvYGvznyVk5063348;     HvYGvznyVk5063348 = HvYGvznyVk41665424;     HvYGvznyVk41665424 = HvYGvznyVk481428;     HvYGvznyVk481428 = HvYGvznyVk4976299;     HvYGvznyVk4976299 = HvYGvznyVk3983610;     HvYGvznyVk3983610 = HvYGvznyVk61695909;     HvYGvznyVk61695909 = HvYGvznyVk22140340;     HvYGvznyVk22140340 = HvYGvznyVk23472570;     HvYGvznyVk23472570 = HvYGvznyVk37488995;     HvYGvznyVk37488995 = HvYGvznyVk25423739;     HvYGvznyVk25423739 = HvYGvznyVk20592284;     HvYGvznyVk20592284 = HvYGvznyVk43020964;     HvYGvznyVk43020964 = HvYGvznyVk23088837;     HvYGvznyVk23088837 = HvYGvznyVk23385760;     HvYGvznyVk23385760 = HvYGvznyVk80111325;     HvYGvznyVk80111325 = HvYGvznyVk58899993;     HvYGvznyVk58899993 = HvYGvznyVk23495432;     HvYGvznyVk23495432 = HvYGvznyVk85557246;     HvYGvznyVk85557246 = HvYGvznyVk63413093;     HvYGvznyVk63413093 = HvYGvznyVk49801889;     HvYGvznyVk49801889 = HvYGvznyVk85056721;     HvYGvznyVk85056721 = HvYGvznyVk22654972;     HvYGvznyVk22654972 = HvYGvznyVk44676826;     HvYGvznyVk44676826 = HvYGvznyVk46773940;     HvYGvznyVk46773940 = HvYGvznyVk46536863;     HvYGvznyVk46536863 = HvYGvznyVk66926944;     HvYGvznyVk66926944 = HvYGvznyVk98349872;     HvYGvznyVk98349872 = HvYGvznyVk79927123;     HvYGvznyVk79927123 = HvYGvznyVk9603414;     HvYGvznyVk9603414 = HvYGvznyVk80198708;     HvYGvznyVk80198708 = HvYGvznyVk64175238;     HvYGvznyVk64175238 = HvYGvznyVk68264110;     HvYGvznyVk68264110 = HvYGvznyVk70090010;     HvYGvznyVk70090010 = HvYGvznyVk35582421;     HvYGvznyVk35582421 = HvYGvznyVk46359986;     HvYGvznyVk46359986 = HvYGvznyVk40951707;     HvYGvznyVk40951707 = HvYGvznyVk8489458;     HvYGvznyVk8489458 = HvYGvznyVk86302758;     HvYGvznyVk86302758 = HvYGvznyVk30673634;     HvYGvznyVk30673634 = HvYGvznyVk73812268;     HvYGvznyVk73812268 = HvYGvznyVk5493703;     HvYGvznyVk5493703 = HvYGvznyVk5048067;     HvYGvznyVk5048067 = HvYGvznyVk14841475;     HvYGvznyVk14841475 = HvYGvznyVk77945059;     HvYGvznyVk77945059 = HvYGvznyVk82217983;     HvYGvznyVk82217983 = HvYGvznyVk89814954;     HvYGvznyVk89814954 = HvYGvznyVk68616528;     HvYGvznyVk68616528 = HvYGvznyVk79153572;     HvYGvznyVk79153572 = HvYGvznyVk73649060;     HvYGvznyVk73649060 = HvYGvznyVk12498922;     HvYGvznyVk12498922 = HvYGvznyVk5234576;     HvYGvznyVk5234576 = HvYGvznyVk59100471;     HvYGvznyVk59100471 = HvYGvznyVk67947323;     HvYGvznyVk67947323 = HvYGvznyVk30204716;     HvYGvznyVk30204716 = HvYGvznyVk89617934;     HvYGvznyVk89617934 = HvYGvznyVk82408376;     HvYGvznyVk82408376 = HvYGvznyVk96988598;     HvYGvznyVk96988598 = HvYGvznyVk53707487;     HvYGvznyVk53707487 = HvYGvznyVk58439436;     HvYGvznyVk58439436 = HvYGvznyVk37056666;     HvYGvznyVk37056666 = HvYGvznyVk63346037;     HvYGvznyVk63346037 = HvYGvznyVk42213217;     HvYGvznyVk42213217 = HvYGvznyVk13869157;     HvYGvznyVk13869157 = HvYGvznyVk57290287;     HvYGvznyVk57290287 = HvYGvznyVk61248501;     HvYGvznyVk61248501 = HvYGvznyVk52328174;     HvYGvznyVk52328174 = HvYGvznyVk72930953;     HvYGvznyVk72930953 = HvYGvznyVk87506416;     HvYGvznyVk87506416 = HvYGvznyVk77025773;     HvYGvznyVk77025773 = HvYGvznyVk39159619;     HvYGvznyVk39159619 = HvYGvznyVk50410536;     HvYGvznyVk50410536 = HvYGvznyVk37192674;     HvYGvznyVk37192674 = HvYGvznyVk54883613;     HvYGvznyVk54883613 = HvYGvznyVk89600824;     HvYGvznyVk89600824 = HvYGvznyVk44308187;     HvYGvznyVk44308187 = HvYGvznyVk80008654;     HvYGvznyVk80008654 = HvYGvznyVk7813497;     HvYGvznyVk7813497 = HvYGvznyVk66731766;     HvYGvznyVk66731766 = HvYGvznyVk10257076;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void gsEeUbbsjI26108735() {     int etGCCZmGhW97178753 = -430535678;    int etGCCZmGhW61071072 = -486645592;    int etGCCZmGhW75313144 = -449700812;    int etGCCZmGhW36834079 = -794637984;    int etGCCZmGhW42008143 = -784754796;    int etGCCZmGhW48122921 = -329543891;    int etGCCZmGhW85553846 = -957192581;    int etGCCZmGhW10350391 = -374717396;    int etGCCZmGhW6906327 = -106026054;    int etGCCZmGhW25830856 = -276122974;    int etGCCZmGhW23049528 = -648479393;    int etGCCZmGhW72436744 = -418635191;    int etGCCZmGhW93462367 = -604769095;    int etGCCZmGhW35023474 = -193896720;    int etGCCZmGhW26252201 = -880269023;    int etGCCZmGhW89039545 = 51135069;    int etGCCZmGhW21239091 = -454791295;    int etGCCZmGhW57946765 = 20699095;    int etGCCZmGhW5817289 = -433552969;    int etGCCZmGhW88113244 = -805034769;    int etGCCZmGhW47472274 = -861016863;    int etGCCZmGhW13364615 = -499334001;    int etGCCZmGhW74709688 = -711682238;    int etGCCZmGhW18022374 = -41877545;    int etGCCZmGhW56446412 = 99137991;    int etGCCZmGhW99155198 = -184139483;    int etGCCZmGhW60874037 = -569129816;    int etGCCZmGhW62282459 = -479188358;    int etGCCZmGhW95043133 = -269847178;    int etGCCZmGhW3186209 = -799408581;    int etGCCZmGhW96609247 = 28432925;    int etGCCZmGhW29865769 = 34780371;    int etGCCZmGhW74865366 = -763148046;    int etGCCZmGhW72944061 = -803289624;    int etGCCZmGhW15867786 = -391303269;    int etGCCZmGhW56345975 = -61702995;    int etGCCZmGhW36080589 = -541696540;    int etGCCZmGhW31228371 = -394585472;    int etGCCZmGhW76726598 = -967564465;    int etGCCZmGhW13344322 = 4018152;    int etGCCZmGhW46574523 = -916334954;    int etGCCZmGhW34642815 = -739244392;    int etGCCZmGhW61506894 = -221354788;    int etGCCZmGhW41199651 = -686493426;    int etGCCZmGhW96661685 = -998930490;    int etGCCZmGhW49340862 = -811534310;    int etGCCZmGhW67186888 = -528542904;    int etGCCZmGhW69182887 = -146855542;    int etGCCZmGhW88575500 = -747120785;    int etGCCZmGhW72526702 = -664085252;    int etGCCZmGhW50961276 = -188874199;    int etGCCZmGhW85111750 = -708007971;    int etGCCZmGhW49316840 = -930792487;    int etGCCZmGhW22809080 = 36793227;    int etGCCZmGhW78636461 = -725946;    int etGCCZmGhW83814138 = -931201677;    int etGCCZmGhW86361384 = -774963354;    int etGCCZmGhW57290770 = -307823268;    int etGCCZmGhW80387667 = -793775975;    int etGCCZmGhW42852945 = -500615313;    int etGCCZmGhW87248883 = -760414075;    int etGCCZmGhW23271387 = -378004223;    int etGCCZmGhW15307258 = -4870218;    int etGCCZmGhW3720118 = -306617473;    int etGCCZmGhW29221609 = -204555900;    int etGCCZmGhW93183759 = -583259765;    int etGCCZmGhW97571378 = -655487146;    int etGCCZmGhW20518306 = -801479472;    int etGCCZmGhW19155689 = -802593452;    int etGCCZmGhW69906226 = -718566028;    int etGCCZmGhW52958956 = -407168392;    int etGCCZmGhW90010720 = 39794176;    int etGCCZmGhW81220167 = -11736440;    int etGCCZmGhW92472967 = -337571121;    int etGCCZmGhW41538721 = -888699815;    int etGCCZmGhW12829460 = -21772471;    int etGCCZmGhW51857720 = -177979213;    int etGCCZmGhW33510038 = 74811187;    int etGCCZmGhW21360688 = -42947056;    int etGCCZmGhW7105551 = -89327699;    int etGCCZmGhW31968311 = -655596580;    int etGCCZmGhW91691149 = -322274275;    int etGCCZmGhW73706958 = -732067573;    int etGCCZmGhW22516431 = -605761927;    int etGCCZmGhW52224933 = -510534382;    int etGCCZmGhW11497497 = -263559105;    int etGCCZmGhW80548928 = -34427143;    int etGCCZmGhW52056287 = -699941273;    int etGCCZmGhW94307600 = -702563679;    int etGCCZmGhW32053647 = -460101593;    int etGCCZmGhW69984591 = -286739642;    int etGCCZmGhW78789818 = -133873272;    int etGCCZmGhW50840704 = -600809497;    int etGCCZmGhW33873654 = -366949152;    int etGCCZmGhW26095438 = -235567773;    int etGCCZmGhW23303137 = -438330732;    int etGCCZmGhW19335557 = -634374174;    int etGCCZmGhW57786777 = -914737316;    int etGCCZmGhW11978042 = -381937527;    int etGCCZmGhW3477927 = -430535678;     etGCCZmGhW97178753 = etGCCZmGhW61071072;     etGCCZmGhW61071072 = etGCCZmGhW75313144;     etGCCZmGhW75313144 = etGCCZmGhW36834079;     etGCCZmGhW36834079 = etGCCZmGhW42008143;     etGCCZmGhW42008143 = etGCCZmGhW48122921;     etGCCZmGhW48122921 = etGCCZmGhW85553846;     etGCCZmGhW85553846 = etGCCZmGhW10350391;     etGCCZmGhW10350391 = etGCCZmGhW6906327;     etGCCZmGhW6906327 = etGCCZmGhW25830856;     etGCCZmGhW25830856 = etGCCZmGhW23049528;     etGCCZmGhW23049528 = etGCCZmGhW72436744;     etGCCZmGhW72436744 = etGCCZmGhW93462367;     etGCCZmGhW93462367 = etGCCZmGhW35023474;     etGCCZmGhW35023474 = etGCCZmGhW26252201;     etGCCZmGhW26252201 = etGCCZmGhW89039545;     etGCCZmGhW89039545 = etGCCZmGhW21239091;     etGCCZmGhW21239091 = etGCCZmGhW57946765;     etGCCZmGhW57946765 = etGCCZmGhW5817289;     etGCCZmGhW5817289 = etGCCZmGhW88113244;     etGCCZmGhW88113244 = etGCCZmGhW47472274;     etGCCZmGhW47472274 = etGCCZmGhW13364615;     etGCCZmGhW13364615 = etGCCZmGhW74709688;     etGCCZmGhW74709688 = etGCCZmGhW18022374;     etGCCZmGhW18022374 = etGCCZmGhW56446412;     etGCCZmGhW56446412 = etGCCZmGhW99155198;     etGCCZmGhW99155198 = etGCCZmGhW60874037;     etGCCZmGhW60874037 = etGCCZmGhW62282459;     etGCCZmGhW62282459 = etGCCZmGhW95043133;     etGCCZmGhW95043133 = etGCCZmGhW3186209;     etGCCZmGhW3186209 = etGCCZmGhW96609247;     etGCCZmGhW96609247 = etGCCZmGhW29865769;     etGCCZmGhW29865769 = etGCCZmGhW74865366;     etGCCZmGhW74865366 = etGCCZmGhW72944061;     etGCCZmGhW72944061 = etGCCZmGhW15867786;     etGCCZmGhW15867786 = etGCCZmGhW56345975;     etGCCZmGhW56345975 = etGCCZmGhW36080589;     etGCCZmGhW36080589 = etGCCZmGhW31228371;     etGCCZmGhW31228371 = etGCCZmGhW76726598;     etGCCZmGhW76726598 = etGCCZmGhW13344322;     etGCCZmGhW13344322 = etGCCZmGhW46574523;     etGCCZmGhW46574523 = etGCCZmGhW34642815;     etGCCZmGhW34642815 = etGCCZmGhW61506894;     etGCCZmGhW61506894 = etGCCZmGhW41199651;     etGCCZmGhW41199651 = etGCCZmGhW96661685;     etGCCZmGhW96661685 = etGCCZmGhW49340862;     etGCCZmGhW49340862 = etGCCZmGhW67186888;     etGCCZmGhW67186888 = etGCCZmGhW69182887;     etGCCZmGhW69182887 = etGCCZmGhW88575500;     etGCCZmGhW88575500 = etGCCZmGhW72526702;     etGCCZmGhW72526702 = etGCCZmGhW50961276;     etGCCZmGhW50961276 = etGCCZmGhW85111750;     etGCCZmGhW85111750 = etGCCZmGhW49316840;     etGCCZmGhW49316840 = etGCCZmGhW22809080;     etGCCZmGhW22809080 = etGCCZmGhW78636461;     etGCCZmGhW78636461 = etGCCZmGhW83814138;     etGCCZmGhW83814138 = etGCCZmGhW86361384;     etGCCZmGhW86361384 = etGCCZmGhW57290770;     etGCCZmGhW57290770 = etGCCZmGhW80387667;     etGCCZmGhW80387667 = etGCCZmGhW42852945;     etGCCZmGhW42852945 = etGCCZmGhW87248883;     etGCCZmGhW87248883 = etGCCZmGhW23271387;     etGCCZmGhW23271387 = etGCCZmGhW15307258;     etGCCZmGhW15307258 = etGCCZmGhW3720118;     etGCCZmGhW3720118 = etGCCZmGhW29221609;     etGCCZmGhW29221609 = etGCCZmGhW93183759;     etGCCZmGhW93183759 = etGCCZmGhW97571378;     etGCCZmGhW97571378 = etGCCZmGhW20518306;     etGCCZmGhW20518306 = etGCCZmGhW19155689;     etGCCZmGhW19155689 = etGCCZmGhW69906226;     etGCCZmGhW69906226 = etGCCZmGhW52958956;     etGCCZmGhW52958956 = etGCCZmGhW90010720;     etGCCZmGhW90010720 = etGCCZmGhW81220167;     etGCCZmGhW81220167 = etGCCZmGhW92472967;     etGCCZmGhW92472967 = etGCCZmGhW41538721;     etGCCZmGhW41538721 = etGCCZmGhW12829460;     etGCCZmGhW12829460 = etGCCZmGhW51857720;     etGCCZmGhW51857720 = etGCCZmGhW33510038;     etGCCZmGhW33510038 = etGCCZmGhW21360688;     etGCCZmGhW21360688 = etGCCZmGhW7105551;     etGCCZmGhW7105551 = etGCCZmGhW31968311;     etGCCZmGhW31968311 = etGCCZmGhW91691149;     etGCCZmGhW91691149 = etGCCZmGhW73706958;     etGCCZmGhW73706958 = etGCCZmGhW22516431;     etGCCZmGhW22516431 = etGCCZmGhW52224933;     etGCCZmGhW52224933 = etGCCZmGhW11497497;     etGCCZmGhW11497497 = etGCCZmGhW80548928;     etGCCZmGhW80548928 = etGCCZmGhW52056287;     etGCCZmGhW52056287 = etGCCZmGhW94307600;     etGCCZmGhW94307600 = etGCCZmGhW32053647;     etGCCZmGhW32053647 = etGCCZmGhW69984591;     etGCCZmGhW69984591 = etGCCZmGhW78789818;     etGCCZmGhW78789818 = etGCCZmGhW50840704;     etGCCZmGhW50840704 = etGCCZmGhW33873654;     etGCCZmGhW33873654 = etGCCZmGhW26095438;     etGCCZmGhW26095438 = etGCCZmGhW23303137;     etGCCZmGhW23303137 = etGCCZmGhW19335557;     etGCCZmGhW19335557 = etGCCZmGhW57786777;     etGCCZmGhW57786777 = etGCCZmGhW11978042;     etGCCZmGhW11978042 = etGCCZmGhW3477927;     etGCCZmGhW3477927 = etGCCZmGhW97178753;}
// Junk Finished
