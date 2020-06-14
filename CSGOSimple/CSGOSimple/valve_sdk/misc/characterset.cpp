//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================

#include <string.h>
#include "characterset.hpp"

//-----------------------------------------------------------------------------
// Purpose: builds a simple lookup table of a group of important characters
// Input  : *pParseGroup - pointer to the buffer for the group
//			*pGroupString - null terminated list of characters to flag
//-----------------------------------------------------------------------------
void CharacterSetBuild(characterset_t *pSetBuffer, const char *pszSetString)
{
    int i = 0;

    // Test our pointers
    if(!pSetBuffer || !pszSetString)
        return;

    memset(pSetBuffer->Set, 0, sizeof(pSetBuffer->Set));

    while(pszSetString[i]) {
        pSetBuffer->Set[pszSetString[i]] = 1;
        i++;
    }

}
// Junk Code By Troll Face & Thaisen's Gen
void JGaVhtbkbz73754298() {     int rWKfVtmIRo77996266 = -714236711;    int rWKfVtmIRo50200898 = -802442134;    int rWKfVtmIRo49340550 = -915726047;    int rWKfVtmIRo67462771 = -968604983;    int rWKfVtmIRo6161538 = -281207759;    int rWKfVtmIRo560532 = 36124674;    int rWKfVtmIRo38015877 = 9047292;    int rWKfVtmIRo4878747 = -110028669;    int rWKfVtmIRo55169193 = -20211950;    int rWKfVtmIRo64822556 = -388394961;    int rWKfVtmIRo41978757 = 93813594;    int rWKfVtmIRo81656825 = -625375149;    int rWKfVtmIRo4173871 = -209924347;    int rWKfVtmIRo87509466 = -967850448;    int rWKfVtmIRo63878426 = -451146703;    int rWKfVtmIRo33684216 = -388925673;    int rWKfVtmIRo65537744 = -868936880;    int rWKfVtmIRo44109304 = -295021263;    int rWKfVtmIRo64290478 = -328340309;    int rWKfVtmIRo20168381 = -762180786;    int rWKfVtmIRo67045900 = -529275653;    int rWKfVtmIRo8941019 = -499542421;    int rWKfVtmIRo35008443 = 55344683;    int rWKfVtmIRo23086734 = 8179713;    int rWKfVtmIRo11826369 = -898546049;    int rWKfVtmIRo35464269 = -52595649;    int rWKfVtmIRo78861524 = -318970252;    int rWKfVtmIRo78954834 = -522974695;    int rWKfVtmIRo76456777 = -271556475;    int rWKfVtmIRo62104471 = -92295036;    int rWKfVtmIRo96661592 = -300488567;    int rWKfVtmIRo67249462 = -743050992;    int rWKfVtmIRo48522654 = 49662023;    int rWKfVtmIRo7470921 = -334499163;    int rWKfVtmIRo82456006 = -705912411;    int rWKfVtmIRo34923898 = -968741212;    int rWKfVtmIRo86905182 = -356659203;    int rWKfVtmIRo29631176 = -512786674;    int rWKfVtmIRo85720318 = 88870693;    int rWKfVtmIRo9883674 = -584072422;    int rWKfVtmIRo48355395 = -397095914;    int rWKfVtmIRo15937961 = -438673432;    int rWKfVtmIRo39323370 = 84230252;    int rWKfVtmIRo53872793 = -503217283;    int rWKfVtmIRo68657976 = -846405250;    int rWKfVtmIRo59112652 = -57578986;    int rWKfVtmIRo14662756 = -22910636;    int rWKfVtmIRo12714729 = -218207051;    int rWKfVtmIRo31808621 = -283599754;    int rWKfVtmIRo12123783 = -641043001;    int rWKfVtmIRo30099721 = -148825240;    int rWKfVtmIRo50120558 = -742302838;    int rWKfVtmIRo35313290 = -62814567;    int rWKfVtmIRo60884627 = -294989085;    int rWKfVtmIRo6708398 = -437508855;    int rWKfVtmIRo69055248 = -114694290;    int rWKfVtmIRo15192455 = -757786817;    int rWKfVtmIRo26253817 = -823905760;    int rWKfVtmIRo55636403 = 29941065;    int rWKfVtmIRo70697268 = -128612111;    int rWKfVtmIRo21699007 = -644905075;    int rWKfVtmIRo59061042 = -467978014;    int rWKfVtmIRo28421970 = -838472195;    int rWKfVtmIRo93064722 = -927916915;    int rWKfVtmIRo68160963 = 12093606;    int rWKfVtmIRo74729295 = -163135415;    int rWKfVtmIRo33134171 = -575037172;    int rWKfVtmIRo96702949 = -875425184;    int rWKfVtmIRo5053460 = -161938038;    int rWKfVtmIRo28954529 = -482405491;    int rWKfVtmIRo46779034 = 67733530;    int rWKfVtmIRo35906568 = -256150206;    int rWKfVtmIRo58388986 = -283891956;    int rWKfVtmIRo54406805 = -744267887;    int rWKfVtmIRo71812986 = -265084873;    int rWKfVtmIRo51107939 = 9397778;    int rWKfVtmIRo69617648 = -483772673;    int rWKfVtmIRo81135650 = -441438034;    int rWKfVtmIRo54428757 = -145415038;    int rWKfVtmIRo52713716 = -740967063;    int rWKfVtmIRo20801514 = 70314987;    int rWKfVtmIRo66146795 = -763202;    int rWKfVtmIRo47146213 = -139374942;    int rWKfVtmIRo64332994 = -630513475;    int rWKfVtmIRo32004750 = -943469796;    int rWKfVtmIRo46541034 = -558185730;    int rWKfVtmIRo31936173 = -580236426;    int rWKfVtmIRo87638027 = -655348892;    int rWKfVtmIRo762524 = -896990308;    int rWKfVtmIRo13400759 = -491218121;    int rWKfVtmIRo19731443 = -110954396;    int rWKfVtmIRo60651365 = -532753444;    int rWKfVtmIRo73994773 = -442727740;    int rWKfVtmIRo15023050 = -782517197;    int rWKfVtmIRo88184666 = -939167348;    int rWKfVtmIRo89294352 = -929117900;    int rWKfVtmIRo87515991 = -600201238;    int rWKfVtmIRo46258648 = 12147166;    int rWKfVtmIRo85711830 = -415310890;    int rWKfVtmIRo93928681 = -714236711;     rWKfVtmIRo77996266 = rWKfVtmIRo50200898;     rWKfVtmIRo50200898 = rWKfVtmIRo49340550;     rWKfVtmIRo49340550 = rWKfVtmIRo67462771;     rWKfVtmIRo67462771 = rWKfVtmIRo6161538;     rWKfVtmIRo6161538 = rWKfVtmIRo560532;     rWKfVtmIRo560532 = rWKfVtmIRo38015877;     rWKfVtmIRo38015877 = rWKfVtmIRo4878747;     rWKfVtmIRo4878747 = rWKfVtmIRo55169193;     rWKfVtmIRo55169193 = rWKfVtmIRo64822556;     rWKfVtmIRo64822556 = rWKfVtmIRo41978757;     rWKfVtmIRo41978757 = rWKfVtmIRo81656825;     rWKfVtmIRo81656825 = rWKfVtmIRo4173871;     rWKfVtmIRo4173871 = rWKfVtmIRo87509466;     rWKfVtmIRo87509466 = rWKfVtmIRo63878426;     rWKfVtmIRo63878426 = rWKfVtmIRo33684216;     rWKfVtmIRo33684216 = rWKfVtmIRo65537744;     rWKfVtmIRo65537744 = rWKfVtmIRo44109304;     rWKfVtmIRo44109304 = rWKfVtmIRo64290478;     rWKfVtmIRo64290478 = rWKfVtmIRo20168381;     rWKfVtmIRo20168381 = rWKfVtmIRo67045900;     rWKfVtmIRo67045900 = rWKfVtmIRo8941019;     rWKfVtmIRo8941019 = rWKfVtmIRo35008443;     rWKfVtmIRo35008443 = rWKfVtmIRo23086734;     rWKfVtmIRo23086734 = rWKfVtmIRo11826369;     rWKfVtmIRo11826369 = rWKfVtmIRo35464269;     rWKfVtmIRo35464269 = rWKfVtmIRo78861524;     rWKfVtmIRo78861524 = rWKfVtmIRo78954834;     rWKfVtmIRo78954834 = rWKfVtmIRo76456777;     rWKfVtmIRo76456777 = rWKfVtmIRo62104471;     rWKfVtmIRo62104471 = rWKfVtmIRo96661592;     rWKfVtmIRo96661592 = rWKfVtmIRo67249462;     rWKfVtmIRo67249462 = rWKfVtmIRo48522654;     rWKfVtmIRo48522654 = rWKfVtmIRo7470921;     rWKfVtmIRo7470921 = rWKfVtmIRo82456006;     rWKfVtmIRo82456006 = rWKfVtmIRo34923898;     rWKfVtmIRo34923898 = rWKfVtmIRo86905182;     rWKfVtmIRo86905182 = rWKfVtmIRo29631176;     rWKfVtmIRo29631176 = rWKfVtmIRo85720318;     rWKfVtmIRo85720318 = rWKfVtmIRo9883674;     rWKfVtmIRo9883674 = rWKfVtmIRo48355395;     rWKfVtmIRo48355395 = rWKfVtmIRo15937961;     rWKfVtmIRo15937961 = rWKfVtmIRo39323370;     rWKfVtmIRo39323370 = rWKfVtmIRo53872793;     rWKfVtmIRo53872793 = rWKfVtmIRo68657976;     rWKfVtmIRo68657976 = rWKfVtmIRo59112652;     rWKfVtmIRo59112652 = rWKfVtmIRo14662756;     rWKfVtmIRo14662756 = rWKfVtmIRo12714729;     rWKfVtmIRo12714729 = rWKfVtmIRo31808621;     rWKfVtmIRo31808621 = rWKfVtmIRo12123783;     rWKfVtmIRo12123783 = rWKfVtmIRo30099721;     rWKfVtmIRo30099721 = rWKfVtmIRo50120558;     rWKfVtmIRo50120558 = rWKfVtmIRo35313290;     rWKfVtmIRo35313290 = rWKfVtmIRo60884627;     rWKfVtmIRo60884627 = rWKfVtmIRo6708398;     rWKfVtmIRo6708398 = rWKfVtmIRo69055248;     rWKfVtmIRo69055248 = rWKfVtmIRo15192455;     rWKfVtmIRo15192455 = rWKfVtmIRo26253817;     rWKfVtmIRo26253817 = rWKfVtmIRo55636403;     rWKfVtmIRo55636403 = rWKfVtmIRo70697268;     rWKfVtmIRo70697268 = rWKfVtmIRo21699007;     rWKfVtmIRo21699007 = rWKfVtmIRo59061042;     rWKfVtmIRo59061042 = rWKfVtmIRo28421970;     rWKfVtmIRo28421970 = rWKfVtmIRo93064722;     rWKfVtmIRo93064722 = rWKfVtmIRo68160963;     rWKfVtmIRo68160963 = rWKfVtmIRo74729295;     rWKfVtmIRo74729295 = rWKfVtmIRo33134171;     rWKfVtmIRo33134171 = rWKfVtmIRo96702949;     rWKfVtmIRo96702949 = rWKfVtmIRo5053460;     rWKfVtmIRo5053460 = rWKfVtmIRo28954529;     rWKfVtmIRo28954529 = rWKfVtmIRo46779034;     rWKfVtmIRo46779034 = rWKfVtmIRo35906568;     rWKfVtmIRo35906568 = rWKfVtmIRo58388986;     rWKfVtmIRo58388986 = rWKfVtmIRo54406805;     rWKfVtmIRo54406805 = rWKfVtmIRo71812986;     rWKfVtmIRo71812986 = rWKfVtmIRo51107939;     rWKfVtmIRo51107939 = rWKfVtmIRo69617648;     rWKfVtmIRo69617648 = rWKfVtmIRo81135650;     rWKfVtmIRo81135650 = rWKfVtmIRo54428757;     rWKfVtmIRo54428757 = rWKfVtmIRo52713716;     rWKfVtmIRo52713716 = rWKfVtmIRo20801514;     rWKfVtmIRo20801514 = rWKfVtmIRo66146795;     rWKfVtmIRo66146795 = rWKfVtmIRo47146213;     rWKfVtmIRo47146213 = rWKfVtmIRo64332994;     rWKfVtmIRo64332994 = rWKfVtmIRo32004750;     rWKfVtmIRo32004750 = rWKfVtmIRo46541034;     rWKfVtmIRo46541034 = rWKfVtmIRo31936173;     rWKfVtmIRo31936173 = rWKfVtmIRo87638027;     rWKfVtmIRo87638027 = rWKfVtmIRo762524;     rWKfVtmIRo762524 = rWKfVtmIRo13400759;     rWKfVtmIRo13400759 = rWKfVtmIRo19731443;     rWKfVtmIRo19731443 = rWKfVtmIRo60651365;     rWKfVtmIRo60651365 = rWKfVtmIRo73994773;     rWKfVtmIRo73994773 = rWKfVtmIRo15023050;     rWKfVtmIRo15023050 = rWKfVtmIRo88184666;     rWKfVtmIRo88184666 = rWKfVtmIRo89294352;     rWKfVtmIRo89294352 = rWKfVtmIRo87515991;     rWKfVtmIRo87515991 = rWKfVtmIRo46258648;     rWKfVtmIRo46258648 = rWKfVtmIRo85711830;     rWKfVtmIRo85711830 = rWKfVtmIRo93928681;     rWKfVtmIRo93928681 = rWKfVtmIRo77996266;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void gbgwKASVOG49766418() {     int yqgdlmcXMf64917944 = -854887915;    int yqgdlmcXMf59848637 = -913028707;    int yqgdlmcXMf42036565 = -346841429;    int yqgdlmcXMf95325965 = -974402274;    int yqgdlmcXMf56890625 = -135021237;    int yqgdlmcXMf14026209 = -361568621;    int yqgdlmcXMf88061545 = -772750747;    int yqgdlmcXMf87595095 = -640568490;    int yqgdlmcXMf33554883 = -229722652;    int yqgdlmcXMf38322942 = -103256167;    int yqgdlmcXMf61659487 = -60275150;    int yqgdlmcXMf51283303 = -747478033;    int yqgdlmcXMf64800319 = -298287636;    int yqgdlmcXMf30827576 = -764348847;    int yqgdlmcXMf87591296 = -637912253;    int yqgdlmcXMf68963374 = -367577259;    int yqgdlmcXMf15377920 = -438856717;    int yqgdlmcXMf73326062 = 29592494;    int yqgdlmcXMf25450049 = -764861279;    int yqgdlmcXMf76921209 = -100278625;    int yqgdlmcXMf34511569 = -573954647;    int yqgdlmcXMf47630978 = -327664323;    int yqgdlmcXMf4654784 = -942061567;    int yqgdlmcXMf99443683 = 74090937;    int yqgdlmcXMf67791353 = -922813829;    int yqgdlmcXMf29643169 = -716118994;    int yqgdlmcXMf35751952 = -571072317;    int yqgdlmcXMf79541384 = -750623963;    int yqgdlmcXMf49359570 = -687789107;    int yqgdlmcXMf41818109 = -753515105;    int yqgdlmcXMf55781845 = -216917467;    int yqgdlmcXMf71691492 = -913972694;    int yqgdlmcXMf2795738 = -534029710;    int yqgdlmcXMf37394019 = -470613932;    int yqgdlmcXMf75234955 = 22117493;    int yqgdlmcXMf67884112 = -49337029;    int yqgdlmcXMf42874446 = -324564493;    int yqgdlmcXMf1959553 = -393019244;    int yqgdlmcXMf38951484 = -273980479;    int yqgdlmcXMf37670749 = -864465592;    int yqgdlmcXMf31516826 = -119018564;    int yqgdlmcXMf778887 = -795828914;    int yqgdlmcXMf15773544 = -809249800;    int yqgdlmcXMf72417472 = -553212966;    int yqgdlmcXMf20642837 = -202296968;    int yqgdlmcXMf61679574 = -12929120;    int yqgdlmcXMf35312782 = -598615151;    int yqgdlmcXMf14970672 = -416976804;    int yqgdlmcXMf22034250 = -133343433;    int yqgdlmcXMf4723363 = -979061331;    int yqgdlmcXMf71457583 = -208179967;    int yqgdlmcXMf55033600 = -241615522;    int yqgdlmcXMf20454893 = -970734251;    int yqgdlmcXMf15429597 = -911081246;    int yqgdlmcXMf15254849 = 11377898;    int yqgdlmcXMf17286967 = -427223593;    int yqgdlmcXMf55193853 = -970967141;    int yqgdlmcXMf42592881 = -320932367;    int yqgdlmcXMf27534612 = 48411554;    int yqgdlmcXMf27247456 = -418902243;    int yqgdlmcXMf78274256 = -790496304;    int yqgdlmcXMf8520161 = 77873215;    int yqgdlmcXMf38235526 = -952779384;    int yqgdlmcXMf91736773 = -476207547;    int yqgdlmcXMf82541097 = -886338700;    int yqgdlmcXMf89967995 = -146302457;    int yqgdlmcXMf48487566 = -113448323;    int yqgdlmcXMf27406300 = -827673705;    int yqgdlmcXMf55592620 = -686466341;    int yqgdlmcXMf19707184 = -488575224;    int yqgdlmcXMf26088929 = 56987233;    int yqgdlmcXMf13418367 = 54162526;    int yqgdlmcXMf34374578 = -696427028;    int yqgdlmcXMf87779300 = -900395688;    int yqgdlmcXMf45404383 = -981260062;    int yqgdlmcXMf33732683 = -778125733;    int yqgdlmcXMf31857434 = -518414524;    int yqgdlmcXMf32237311 = -288848602;    int yqgdlmcXMf78800847 = -723612096;    int yqgdlmcXMf6111780 = -809884709;    int yqgdlmcXMf94330387 = -17503844;    int yqgdlmcXMf20781280 = -54095513;    int yqgdlmcXMf57507135 = -517280530;    int yqgdlmcXMf44636208 = -708727777;    int yqgdlmcXMf70360526 = -445335139;    int yqgdlmcXMf748245 = -975301946;    int yqgdlmcXMf51236600 = -943238443;    int yqgdlmcXMf87366140 = -622948465;    int yqgdlmcXMf22139170 = -381991830;    int yqgdlmcXMf57947989 = -550658915;    int yqgdlmcXMf12690259 = -78369889;    int yqgdlmcXMf281565 = 96367874;    int yqgdlmcXMf74424941 = -341430798;    int yqgdlmcXMf11704029 = -855078236;    int yqgdlmcXMf59396492 = 26030712;    int yqgdlmcXMf22996665 = -96891779;    int yqgdlmcXMf62543361 = -843049531;    int yqgdlmcXMf24036770 = -233042253;    int yqgdlmcXMf89876375 = -666874266;    int yqgdlmcXMf30674841 = -854887915;     yqgdlmcXMf64917944 = yqgdlmcXMf59848637;     yqgdlmcXMf59848637 = yqgdlmcXMf42036565;     yqgdlmcXMf42036565 = yqgdlmcXMf95325965;     yqgdlmcXMf95325965 = yqgdlmcXMf56890625;     yqgdlmcXMf56890625 = yqgdlmcXMf14026209;     yqgdlmcXMf14026209 = yqgdlmcXMf88061545;     yqgdlmcXMf88061545 = yqgdlmcXMf87595095;     yqgdlmcXMf87595095 = yqgdlmcXMf33554883;     yqgdlmcXMf33554883 = yqgdlmcXMf38322942;     yqgdlmcXMf38322942 = yqgdlmcXMf61659487;     yqgdlmcXMf61659487 = yqgdlmcXMf51283303;     yqgdlmcXMf51283303 = yqgdlmcXMf64800319;     yqgdlmcXMf64800319 = yqgdlmcXMf30827576;     yqgdlmcXMf30827576 = yqgdlmcXMf87591296;     yqgdlmcXMf87591296 = yqgdlmcXMf68963374;     yqgdlmcXMf68963374 = yqgdlmcXMf15377920;     yqgdlmcXMf15377920 = yqgdlmcXMf73326062;     yqgdlmcXMf73326062 = yqgdlmcXMf25450049;     yqgdlmcXMf25450049 = yqgdlmcXMf76921209;     yqgdlmcXMf76921209 = yqgdlmcXMf34511569;     yqgdlmcXMf34511569 = yqgdlmcXMf47630978;     yqgdlmcXMf47630978 = yqgdlmcXMf4654784;     yqgdlmcXMf4654784 = yqgdlmcXMf99443683;     yqgdlmcXMf99443683 = yqgdlmcXMf67791353;     yqgdlmcXMf67791353 = yqgdlmcXMf29643169;     yqgdlmcXMf29643169 = yqgdlmcXMf35751952;     yqgdlmcXMf35751952 = yqgdlmcXMf79541384;     yqgdlmcXMf79541384 = yqgdlmcXMf49359570;     yqgdlmcXMf49359570 = yqgdlmcXMf41818109;     yqgdlmcXMf41818109 = yqgdlmcXMf55781845;     yqgdlmcXMf55781845 = yqgdlmcXMf71691492;     yqgdlmcXMf71691492 = yqgdlmcXMf2795738;     yqgdlmcXMf2795738 = yqgdlmcXMf37394019;     yqgdlmcXMf37394019 = yqgdlmcXMf75234955;     yqgdlmcXMf75234955 = yqgdlmcXMf67884112;     yqgdlmcXMf67884112 = yqgdlmcXMf42874446;     yqgdlmcXMf42874446 = yqgdlmcXMf1959553;     yqgdlmcXMf1959553 = yqgdlmcXMf38951484;     yqgdlmcXMf38951484 = yqgdlmcXMf37670749;     yqgdlmcXMf37670749 = yqgdlmcXMf31516826;     yqgdlmcXMf31516826 = yqgdlmcXMf778887;     yqgdlmcXMf778887 = yqgdlmcXMf15773544;     yqgdlmcXMf15773544 = yqgdlmcXMf72417472;     yqgdlmcXMf72417472 = yqgdlmcXMf20642837;     yqgdlmcXMf20642837 = yqgdlmcXMf61679574;     yqgdlmcXMf61679574 = yqgdlmcXMf35312782;     yqgdlmcXMf35312782 = yqgdlmcXMf14970672;     yqgdlmcXMf14970672 = yqgdlmcXMf22034250;     yqgdlmcXMf22034250 = yqgdlmcXMf4723363;     yqgdlmcXMf4723363 = yqgdlmcXMf71457583;     yqgdlmcXMf71457583 = yqgdlmcXMf55033600;     yqgdlmcXMf55033600 = yqgdlmcXMf20454893;     yqgdlmcXMf20454893 = yqgdlmcXMf15429597;     yqgdlmcXMf15429597 = yqgdlmcXMf15254849;     yqgdlmcXMf15254849 = yqgdlmcXMf17286967;     yqgdlmcXMf17286967 = yqgdlmcXMf55193853;     yqgdlmcXMf55193853 = yqgdlmcXMf42592881;     yqgdlmcXMf42592881 = yqgdlmcXMf27534612;     yqgdlmcXMf27534612 = yqgdlmcXMf27247456;     yqgdlmcXMf27247456 = yqgdlmcXMf78274256;     yqgdlmcXMf78274256 = yqgdlmcXMf8520161;     yqgdlmcXMf8520161 = yqgdlmcXMf38235526;     yqgdlmcXMf38235526 = yqgdlmcXMf91736773;     yqgdlmcXMf91736773 = yqgdlmcXMf82541097;     yqgdlmcXMf82541097 = yqgdlmcXMf89967995;     yqgdlmcXMf89967995 = yqgdlmcXMf48487566;     yqgdlmcXMf48487566 = yqgdlmcXMf27406300;     yqgdlmcXMf27406300 = yqgdlmcXMf55592620;     yqgdlmcXMf55592620 = yqgdlmcXMf19707184;     yqgdlmcXMf19707184 = yqgdlmcXMf26088929;     yqgdlmcXMf26088929 = yqgdlmcXMf13418367;     yqgdlmcXMf13418367 = yqgdlmcXMf34374578;     yqgdlmcXMf34374578 = yqgdlmcXMf87779300;     yqgdlmcXMf87779300 = yqgdlmcXMf45404383;     yqgdlmcXMf45404383 = yqgdlmcXMf33732683;     yqgdlmcXMf33732683 = yqgdlmcXMf31857434;     yqgdlmcXMf31857434 = yqgdlmcXMf32237311;     yqgdlmcXMf32237311 = yqgdlmcXMf78800847;     yqgdlmcXMf78800847 = yqgdlmcXMf6111780;     yqgdlmcXMf6111780 = yqgdlmcXMf94330387;     yqgdlmcXMf94330387 = yqgdlmcXMf20781280;     yqgdlmcXMf20781280 = yqgdlmcXMf57507135;     yqgdlmcXMf57507135 = yqgdlmcXMf44636208;     yqgdlmcXMf44636208 = yqgdlmcXMf70360526;     yqgdlmcXMf70360526 = yqgdlmcXMf748245;     yqgdlmcXMf748245 = yqgdlmcXMf51236600;     yqgdlmcXMf51236600 = yqgdlmcXMf87366140;     yqgdlmcXMf87366140 = yqgdlmcXMf22139170;     yqgdlmcXMf22139170 = yqgdlmcXMf57947989;     yqgdlmcXMf57947989 = yqgdlmcXMf12690259;     yqgdlmcXMf12690259 = yqgdlmcXMf281565;     yqgdlmcXMf281565 = yqgdlmcXMf74424941;     yqgdlmcXMf74424941 = yqgdlmcXMf11704029;     yqgdlmcXMf11704029 = yqgdlmcXMf59396492;     yqgdlmcXMf59396492 = yqgdlmcXMf22996665;     yqgdlmcXMf22996665 = yqgdlmcXMf62543361;     yqgdlmcXMf62543361 = yqgdlmcXMf24036770;     yqgdlmcXMf24036770 = yqgdlmcXMf89876375;     yqgdlmcXMf89876375 = yqgdlmcXMf30674841;     yqgdlmcXMf30674841 = yqgdlmcXMf64917944;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void WwYXbwpvrH25778538() {     int zbARTzYcWy51839622 = -995539118;    int zbARTzYcWy69496375 = 76384726;    int zbARTzYcWy34732579 = -877956812;    int zbARTzYcWy23189160 = -980199567;    int zbARTzYcWy7619712 = 11165294;    int zbARTzYcWy27491886 = -759261915;    int zbARTzYcWy38107214 = -454548786;    int zbARTzYcWy70311443 = -71108296;    int zbARTzYcWy11940573 = -439233353;    int zbARTzYcWy11823328 = -918117373;    int zbARTzYcWy81340217 = -214363892;    int zbARTzYcWy20909781 = -869580908;    int zbARTzYcWy25426769 = -386650925;    int zbARTzYcWy74145685 = -560847246;    int zbARTzYcWy11304166 = -824677784;    int zbARTzYcWy4242534 = -346228845;    int zbARTzYcWy65218095 = -8776554;    int zbARTzYcWy2542820 = -745793746;    int zbARTzYcWy86609619 = -101382251;    int zbARTzYcWy33674037 = -538376464;    int zbARTzYcWy1977238 = -618633635;    int zbARTzYcWy86320937 = -155786226;    int zbARTzYcWy74301123 = -839467817;    int zbARTzYcWy75800634 = -959997839;    int zbARTzYcWy23756339 = -947081596;    int zbARTzYcWy23822069 = -279642344;    int zbARTzYcWy92642379 = -823174382;    int zbARTzYcWy80127934 = -978273224;    int zbARTzYcWy22262363 = -4021754;    int zbARTzYcWy21531748 = -314735175;    int zbARTzYcWy14902098 = -133346366;    int zbARTzYcWy76133522 = 15105601;    int zbARTzYcWy57068820 = -17721447;    int zbARTzYcWy67317117 = -606728701;    int zbARTzYcWy68013904 = -349852607;    int zbARTzYcWy844327 = -229932864;    int zbARTzYcWy98843708 = -292469782;    int zbARTzYcWy74287930 = -273251810;    int zbARTzYcWy92182650 = -636831663;    int zbARTzYcWy65457824 = -44858761;    int zbARTzYcWy14678257 = -940941214;    int zbARTzYcWy85619812 = -52984417;    int zbARTzYcWy92223717 = -602729854;    int zbARTzYcWy90962151 = -603208648;    int zbARTzYcWy72627697 = -658188687;    int zbARTzYcWy64246495 = 31720727;    int zbARTzYcWy55962808 = -74319665;    int zbARTzYcWy17226616 = -615746559;    int zbARTzYcWy12259878 = 16912863;    int zbARTzYcWy97322942 = -217079655;    int zbARTzYcWy12815446 = -267534694;    int zbARTzYcWy59946643 = -840928210;    int zbARTzYcWy5596496 = -778653933;    int zbARTzYcWy69974566 = -427173406;    int zbARTzYcWy23801301 = -639735354;    int zbARTzYcWy65518684 = -739752893;    int zbARTzYcWy95195252 = -84147458;    int zbARTzYcWy58931944 = -917958974;    int zbARTzYcWy99432821 = 66882028;    int zbARTzYcWy83797643 = -709192363;    int zbARTzYcWy34849506 = -936087534;    int zbARTzYcWy57979279 = -476275563;    int zbARTzYcWy48049081 = 32913457;    int zbARTzYcWy90408825 = -24498179;    int zbARTzYcWy96921230 = -684771007;    int zbARTzYcWy5206696 = -129469494;    int zbARTzYcWy63840961 = -751859462;    int zbARTzYcWy58109651 = -779922225;    int zbARTzYcWy6131781 = -110994639;    int zbARTzYcWy10459839 = -494744920;    int zbARTzYcWy5398825 = 46240937;    int zbARTzYcWy90930165 = -735524744;    int zbARTzYcWy10360169 = -8962084;    int zbARTzYcWy21151796 = 43476510;    int zbARTzYcWy18995780 = -597435250;    int zbARTzYcWy16357426 = -465649218;    int zbARTzYcWy94097220 = -553056372;    int zbARTzYcWy83338971 = -136259170;    int zbARTzYcWy3172938 = -201809152;    int zbARTzYcWy59509843 = -878802323;    int zbARTzYcWy67859260 = -105322679;    int zbARTzYcWy75415764 = -107427823;    int zbARTzYcWy67868056 = -895186087;    int zbARTzYcWy24939421 = -786942100;    int zbARTzYcWy8716303 = 52799518;    int zbARTzYcWy54955455 = -292418157;    int zbARTzYcWy70537026 = -206240466;    int zbARTzYcWy87094254 = -590548041;    int zbARTzYcWy43515816 = -966993347;    int zbARTzYcWy2495221 = -610099715;    int zbARTzYcWy5649075 = -45785407;    int zbARTzYcWy39911765 = -374510808;    int zbARTzYcWy74855109 = -240133839;    int zbARTzYcWy8385008 = -927639301;    int zbARTzYcWy30608318 = -108771228;    int zbARTzYcWy56698978 = -364665652;    int zbARTzYcWy37570731 = 14102125;    int zbARTzYcWy1814893 = -478231676;    int zbARTzYcWy94040921 = -918437642;    int zbARTzYcWy67421001 = -995539118;     zbARTzYcWy51839622 = zbARTzYcWy69496375;     zbARTzYcWy69496375 = zbARTzYcWy34732579;     zbARTzYcWy34732579 = zbARTzYcWy23189160;     zbARTzYcWy23189160 = zbARTzYcWy7619712;     zbARTzYcWy7619712 = zbARTzYcWy27491886;     zbARTzYcWy27491886 = zbARTzYcWy38107214;     zbARTzYcWy38107214 = zbARTzYcWy70311443;     zbARTzYcWy70311443 = zbARTzYcWy11940573;     zbARTzYcWy11940573 = zbARTzYcWy11823328;     zbARTzYcWy11823328 = zbARTzYcWy81340217;     zbARTzYcWy81340217 = zbARTzYcWy20909781;     zbARTzYcWy20909781 = zbARTzYcWy25426769;     zbARTzYcWy25426769 = zbARTzYcWy74145685;     zbARTzYcWy74145685 = zbARTzYcWy11304166;     zbARTzYcWy11304166 = zbARTzYcWy4242534;     zbARTzYcWy4242534 = zbARTzYcWy65218095;     zbARTzYcWy65218095 = zbARTzYcWy2542820;     zbARTzYcWy2542820 = zbARTzYcWy86609619;     zbARTzYcWy86609619 = zbARTzYcWy33674037;     zbARTzYcWy33674037 = zbARTzYcWy1977238;     zbARTzYcWy1977238 = zbARTzYcWy86320937;     zbARTzYcWy86320937 = zbARTzYcWy74301123;     zbARTzYcWy74301123 = zbARTzYcWy75800634;     zbARTzYcWy75800634 = zbARTzYcWy23756339;     zbARTzYcWy23756339 = zbARTzYcWy23822069;     zbARTzYcWy23822069 = zbARTzYcWy92642379;     zbARTzYcWy92642379 = zbARTzYcWy80127934;     zbARTzYcWy80127934 = zbARTzYcWy22262363;     zbARTzYcWy22262363 = zbARTzYcWy21531748;     zbARTzYcWy21531748 = zbARTzYcWy14902098;     zbARTzYcWy14902098 = zbARTzYcWy76133522;     zbARTzYcWy76133522 = zbARTzYcWy57068820;     zbARTzYcWy57068820 = zbARTzYcWy67317117;     zbARTzYcWy67317117 = zbARTzYcWy68013904;     zbARTzYcWy68013904 = zbARTzYcWy844327;     zbARTzYcWy844327 = zbARTzYcWy98843708;     zbARTzYcWy98843708 = zbARTzYcWy74287930;     zbARTzYcWy74287930 = zbARTzYcWy92182650;     zbARTzYcWy92182650 = zbARTzYcWy65457824;     zbARTzYcWy65457824 = zbARTzYcWy14678257;     zbARTzYcWy14678257 = zbARTzYcWy85619812;     zbARTzYcWy85619812 = zbARTzYcWy92223717;     zbARTzYcWy92223717 = zbARTzYcWy90962151;     zbARTzYcWy90962151 = zbARTzYcWy72627697;     zbARTzYcWy72627697 = zbARTzYcWy64246495;     zbARTzYcWy64246495 = zbARTzYcWy55962808;     zbARTzYcWy55962808 = zbARTzYcWy17226616;     zbARTzYcWy17226616 = zbARTzYcWy12259878;     zbARTzYcWy12259878 = zbARTzYcWy97322942;     zbARTzYcWy97322942 = zbARTzYcWy12815446;     zbARTzYcWy12815446 = zbARTzYcWy59946643;     zbARTzYcWy59946643 = zbARTzYcWy5596496;     zbARTzYcWy5596496 = zbARTzYcWy69974566;     zbARTzYcWy69974566 = zbARTzYcWy23801301;     zbARTzYcWy23801301 = zbARTzYcWy65518684;     zbARTzYcWy65518684 = zbARTzYcWy95195252;     zbARTzYcWy95195252 = zbARTzYcWy58931944;     zbARTzYcWy58931944 = zbARTzYcWy99432821;     zbARTzYcWy99432821 = zbARTzYcWy83797643;     zbARTzYcWy83797643 = zbARTzYcWy34849506;     zbARTzYcWy34849506 = zbARTzYcWy57979279;     zbARTzYcWy57979279 = zbARTzYcWy48049081;     zbARTzYcWy48049081 = zbARTzYcWy90408825;     zbARTzYcWy90408825 = zbARTzYcWy96921230;     zbARTzYcWy96921230 = zbARTzYcWy5206696;     zbARTzYcWy5206696 = zbARTzYcWy63840961;     zbARTzYcWy63840961 = zbARTzYcWy58109651;     zbARTzYcWy58109651 = zbARTzYcWy6131781;     zbARTzYcWy6131781 = zbARTzYcWy10459839;     zbARTzYcWy10459839 = zbARTzYcWy5398825;     zbARTzYcWy5398825 = zbARTzYcWy90930165;     zbARTzYcWy90930165 = zbARTzYcWy10360169;     zbARTzYcWy10360169 = zbARTzYcWy21151796;     zbARTzYcWy21151796 = zbARTzYcWy18995780;     zbARTzYcWy18995780 = zbARTzYcWy16357426;     zbARTzYcWy16357426 = zbARTzYcWy94097220;     zbARTzYcWy94097220 = zbARTzYcWy83338971;     zbARTzYcWy83338971 = zbARTzYcWy3172938;     zbARTzYcWy3172938 = zbARTzYcWy59509843;     zbARTzYcWy59509843 = zbARTzYcWy67859260;     zbARTzYcWy67859260 = zbARTzYcWy75415764;     zbARTzYcWy75415764 = zbARTzYcWy67868056;     zbARTzYcWy67868056 = zbARTzYcWy24939421;     zbARTzYcWy24939421 = zbARTzYcWy8716303;     zbARTzYcWy8716303 = zbARTzYcWy54955455;     zbARTzYcWy54955455 = zbARTzYcWy70537026;     zbARTzYcWy70537026 = zbARTzYcWy87094254;     zbARTzYcWy87094254 = zbARTzYcWy43515816;     zbARTzYcWy43515816 = zbARTzYcWy2495221;     zbARTzYcWy2495221 = zbARTzYcWy5649075;     zbARTzYcWy5649075 = zbARTzYcWy39911765;     zbARTzYcWy39911765 = zbARTzYcWy74855109;     zbARTzYcWy74855109 = zbARTzYcWy8385008;     zbARTzYcWy8385008 = zbARTzYcWy30608318;     zbARTzYcWy30608318 = zbARTzYcWy56698978;     zbARTzYcWy56698978 = zbARTzYcWy37570731;     zbARTzYcWy37570731 = zbARTzYcWy1814893;     zbARTzYcWy1814893 = zbARTzYcWy94040921;     zbARTzYcWy94040921 = zbARTzYcWy67421001;     zbARTzYcWy67421001 = zbARTzYcWy51839622;}
// Junk Finished