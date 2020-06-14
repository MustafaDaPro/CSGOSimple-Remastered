#include "Utils.hpp"

#define NOMINMAX
#include <Windows.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "../valve_sdk/csgostructs.hpp"
#include "Math.hpp"


HANDLE _out = NULL, _old_out = NULL;
HANDLE _err = NULL, _old_err = NULL;
HANDLE _in = NULL, _old_in = NULL;

namespace Utils {
	std::vector<char> HexToBytes(const std::string& hex) {
		std::vector<char> res;

		for (auto i = 0u; i < hex.length(); i += 2) {
			std::string byteString = hex.substr(i, 2);
			char byte = (char)strtol(byteString.c_str(), NULL, 16);
			res.push_back(byte);
		}

		return res;
	}
	std::string BytesToString(unsigned char* data, int len) {
		constexpr char hexmap[] = { '0', '1', '2', '3', '4', '5', '6', '7',
                                    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		std::string res(len * 2, ' ');
		for (int i = 0; i < len; ++i) {
			res[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
			res[2 * i + 1] = hexmap[data[i] & 0x0F];
		}
		return res;
	}
	std::vector<std::string> Split(const std::string& str, const char* delim) {
		std::vector<std::string> res;
		char* pTempStr = _strdup(str.c_str());
		char* context = NULL;
		char* pWord = strtok_s(pTempStr, delim, &context);
		while (pWord != NULL) {
			res.push_back(pWord);
			pWord = strtok_s(NULL, delim, &context);
		}

		free(pTempStr);

		return res;
	}

	unsigned int FindInDataMap(datamap_t *pMap, const char *name) {
		while (pMap) {
			for (int i = 0; i<pMap->dataNumFields; i++) {
				if (pMap->dataDesc[i].fieldName == NULL)
					continue;

				if (strcmp(name, pMap->dataDesc[i].fieldName) == 0)
					return pMap->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];

				if (pMap->dataDesc[i].fieldType == FIELD_EMBEDDED) {
					if (pMap->dataDesc[i].td) {
						unsigned int offset;

						if ((offset = FindInDataMap(pMap->dataDesc[i].td, name)) != 0)
							return offset;
					}
				}
			}
			pMap = pMap->baseMap;
		}

		return 0;
	}
    /*
     * @brief Create console
     *
     * Create and attach a console window to the current process
     */
    void AttachConsole()
    {
        _old_out = GetStdHandle(STD_OUTPUT_HANDLE);
        _old_err = GetStdHandle(STD_ERROR_HANDLE);
        _old_in  = GetStdHandle(STD_INPUT_HANDLE);

        ::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

        _out     = GetStdHandle(STD_OUTPUT_HANDLE);
        _err     = GetStdHandle(STD_ERROR_HANDLE);
        _in      = GetStdHandle(STD_INPUT_HANDLE);

        SetConsoleMode(_out,
            ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

        SetConsoleMode(_in,
            ENABLE_INSERT_MODE | ENABLE_EXTENDED_FLAGS |
            ENABLE_PROCESSED_INPUT | ENABLE_QUICK_EDIT_MODE);
    }

    /*
     * @brief Detach console
     *
     * Detach and destroy the attached console
     */
    void DetachConsole()
    {
        if(_out && _err && _in) {
            FreeConsole();

            if(_old_out)
                SetStdHandle(STD_OUTPUT_HANDLE, _old_out);
            if(_old_err)
                SetStdHandle(STD_ERROR_HANDLE, _old_err);
            if(_old_in)
                SetStdHandle(STD_INPUT_HANDLE, _old_in);
        }
    }

    /*
     * @brief Print to console
     *
     * Replacement to printf that works with the newly created console
     */
    bool ConsolePrint(const char* fmt, ...)
    {
        if(!_out) 
            return false;

        char buf[1024];
        va_list va;

        va_start(va, fmt);
        _vsnprintf_s(buf, 1024, fmt, va);
        va_end(va);

        return !!WriteConsoleA(_out, buf, static_cast<DWORD>(strlen(buf)), nullptr, nullptr);
    }

    /*
     * @brief Blocks execution until a key is pressed on the console window
     *
     */
    char ConsoleReadKey()
    {
        if(!_in)
            return false;

        auto key = char{ 0 };
        auto keysread = DWORD{ 0 };

        ReadConsoleA(_in, &key, 1, &keysread, nullptr);

        return key;
    }


    /*
     * @brief Wait for all the given modules to be loaded
     *
     * @param timeout How long to wait
     * @param modules List of modules to wait for
     *
     * @returns See WaitForSingleObject return values.
     */
    int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring>& modules)
    {
        bool signaled[32] = { 0 };
        bool success = false;

        std::uint32_t totalSlept = 0;

        if(timeout == 0) {
            for(auto& mod : modules) {
                if(GetModuleHandleW(std::data(mod)) == NULL)
                    return WAIT_TIMEOUT;
            }
            return WAIT_OBJECT_0;
        }

        if(timeout < 0)
            timeout = INT32_MAX;

        while(true) {
            for(auto i = 0u; i < modules.size(); ++i) {
                auto& module = *(modules.begin() + i);
                if(!signaled[i] && GetModuleHandleW(std::data(module)) != NULL) {
                    signaled[i] = true;

                    //
                    // Checks if all modules are signaled
                    //
                    bool done = true;
                    for(auto j = 0u; j < modules.size(); ++j) {
                        if(!signaled[j]) {
                            done = false;
                            break;
                        }
                    }
                    if(done) {
                        success = true;
                        goto exit;
                    }
                }
            }
            if(totalSlept > std::uint32_t(timeout)) {
                break;
            }
            Sleep(10);
            totalSlept += 10;
        }

    exit:
        return success ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
    }

    /*
     * @brief Scan for a given byte pattern on a module
     *
     * @param module    Base of the module to search
     * @param signature IDA-style byte array pattern
     *
     * @returns Address of the first occurence
     */
    std::uint8_t* PatternScan(void* module, const char* signature)
    {
        static auto pattern_to_byte = [](const char* pattern) {
            auto bytes = std::vector<int>{};
            auto start = const_cast<char*>(pattern);
            auto end = const_cast<char*>(pattern) + strlen(pattern);

            for(auto current = start; current < end; ++current) {
                if(*current == '?') {
                    ++current;
                    if(*current == '?')
                        ++current;
                    bytes.push_back(-1);
                } else {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto dosHeader = (PIMAGE_DOS_HEADER)module;
        auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = pattern_to_byte(signature);
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        auto s = patternBytes.size();
        auto d = patternBytes.data();

        for(auto i = 0ul; i < sizeOfImage - s; ++i) {
            bool found = true;
            for(auto j = 0ul; j < s; ++j) {
                if(scanBytes[i + j] != d[j] && d[j] != -1) {
                    found = false;
                    break;
                }
            }
            if(found) {
                return &scanBytes[i];
            }
        }
        return nullptr;
    }

    /*
     * @brief Set player clantag
     *
     * @param tag New clantag
     */
    void SetClantag(const char* tag)
    {
        static auto fnClantagChanged = (int(__fastcall*)(const char*, const char*))PatternScan(GetModuleHandleW(L"engine.dll"), "53 56 57 8B DA 8B F9 FF 15");

        fnClantagChanged(tag, tag);
    }

    void Marquee(std::string& panicova_zlomena_noha)
    {
        std::string temp_string = panicova_zlomena_noha;
        panicova_zlomena_noha.erase(0, 1);
        panicova_zlomena_noha += temp_string[0];
    }

    /*
     * @brief Set player name
     *
     * @param name New name
     */
    void SetName(const char* name)
    {
        static auto nameConvar = g_CVar->FindVar("name");
        nameConvar->m_fnChangeCallbacks.m_Size = 0;

        // Fix so we can change names how many times we want
        // This code will only run once because of `static`
        static auto do_once = (nameConvar->SetValue("\n���"), true);

        nameConvar->SetValue(name);
    }
}

// Junk Code By Troll Face & Thaisen's Gen
void IZvjNMZpHU52818786() {     int mPmNJNlJXp52404518 = -61727751;    int mPmNJNlJXp92376071 = -589559482;    int mPmNJNlJXp17774587 = 29606876;    int mPmNJNlJXp6820703 = -781822920;    int mPmNJNlJXp45659635 = -702640794;    int mPmNJNlJXp18356687 = -608327135;    int mPmNJNlJXp1242369 = -213217968;    int mPmNJNlJXp59082673 = -533524108;    int mPmNJNlJXp12580065 = -337633976;    int mPmNJNlJXp42303687 = -790640308;    int mPmNJNlJXp47965809 = -655230591;    int mPmNJNlJXp60630847 = 82855395;    int mPmNJNlJXp59446006 = -467334456;    int mPmNJNlJXp49793970 = -238479206;    int mPmNJNlJXp52781647 = -525313597;    int mPmNJNlJXp527721 = -285529846;    int mPmNJNlJXp21592388 = 99768345;    int mPmNJNlJXp45993933 = -175815524;    int mPmNJNlJXp23254027 = -163348718;    int mPmNJNlJXp31080678 = -299765862;    int mPmNJNlJXp45706059 = -878042246;    int mPmNJNlJXp85734177 = -589801375;    int mPmNJNlJXp68123042 = -359521054;    int mPmNJNlJXp65022799 = -13891830;    int mPmNJNlJXp85365918 = -657743760;    int mPmNJNlJXp22549210 = -396351036;    int mPmNJNlJXp66695196 = 46043170;    int mPmNJNlJXp50459559 = -902279451;    int mPmNJNlJXp81258011 = -681332940;    int mPmNJNlJXp84871849 = -958816848;    int mPmNJNlJXp8027637 = -908934771;    int mPmNJNlJXp14783387 = -861076919;    int mPmNJNlJXp12788026 = -804461056;    int mPmNJNlJXp54166687 = -155035924;    int mPmNJNlJXp63409056 = -263790424;    int mPmNJNlJXp72960236 = -530912243;    int mPmNJNlJXp49201164 = 24199363;    int mPmNJNlJXp23976169 = -659334528;    int mPmNJNlJXp80110336 = -165472402;    int mPmNJNlJXp72972892 = -823533789;    int mPmNJNlJXp89059781 = 90020378;    int mPmNJNlJXp57626032 = -123427010;    int mPmNJNlJXp50406511 = -446293621;    int mPmNJNlJXp42311412 = -691766128;    int mPmNJNlJXp60695152 = -280643534;    int mPmNJNlJXp33140298 = -447076120;    int mPmNJNlJXp42592094 = -471722398;    int mPmNJNlJXp79985538 = -633785560;    int mPmNJNlJXp99655690 = -847687389;    int mPmNJNlJXp78359211 = -380044734;    int mPmNJNlJXp33222843 = -578721645;    int mPmNJNlJXp37409236 = -946369407;    int mPmNJNlJXp92688034 = -486970026;    int mPmNJNlJXp2235989 = -511845155;    int mPmNJNlJXp7112727 = -819317715;    int mPmNJNlJXp66670340 = -471926377;    int mPmNJNlJXp24253030 = -130038429;    int mPmNJNlJXp52751787 = -956501294;    int mPmNJNlJXp21454784 = -24079161;    int mPmNJNlJXp23110426 = -206289759;    int mPmNJNlJXp51661490 = -554370305;    int mPmNJNlJXp50782809 = -310938518;    int mPmNJNlJXp77824662 = -852191168;    int mPmNJNlJXp27708215 = -378817128;    int mPmNJNlJXp34276050 = -881705538;    int mPmNJNlJXp33182422 = -794153672;    int mPmNJNlJXp47842821 = -112683549;    int mPmNJNlJXp5279320 = -212298532;    int mPmNJNlJXp86384913 = -974688783;    int mPmNJNlJXp79821410 = -994401355;    int mPmNJNlJXp51326556 = -209729210;    int mPmNJNlJXp97616218 = -240897128;    int mPmNJNlJXp65883597 = 89656878;    int mPmNJNlJXp50281135 = -339814929;    int mPmNJNlJXp42020896 = -289786240;    int mPmNJNlJXp88080027 = -654615236;    int mPmNJNlJXp35327667 = -43507754;    int mPmNJNlJXp25811630 = -667754927;    int mPmNJNlJXp4327647 = -733248297;    int mPmNJNlJXp52225620 = -110667641;    int mPmNJNlJXp79957116 = -924628638;    int mPmNJNlJXp86709658 = -320171271;    int mPmNJNlJXp50803868 = 45407937;    int mPmNJNlJXp2898801 = -201288206;    int mPmNJNlJXp51649007 = -280095203;    int mPmNJNlJXp70618401 = -962565365;    int mPmNJNlJXp22095353 = -274106894;    int mPmNJNlJXp10552037 = -192615901;    int mPmNJNlJXp47053960 = -335718209;    int mPmNJNlJXp96738715 = -791864047;    int mPmNJNlJXp48707207 = -300873814;    int mPmNJNlJXp96449377 = -19299343;    int mPmNJNlJXp2521385 = -535255368;    int mPmNJNlJXp56999910 = -959182644;    int mPmNJNlJXp21311402 = -169163485;    int mPmNJNlJXp38276973 = -599041105;    int mPmNJNlJXp79801370 = -271235843;    int mPmNJNlJXp22698296 = 32523507;    int mPmNJNlJXp8035363 = -810060591;    int mPmNJNlJXp27512731 = -61727751;     mPmNJNlJXp52404518 = mPmNJNlJXp92376071;     mPmNJNlJXp92376071 = mPmNJNlJXp17774587;     mPmNJNlJXp17774587 = mPmNJNlJXp6820703;     mPmNJNlJXp6820703 = mPmNJNlJXp45659635;     mPmNJNlJXp45659635 = mPmNJNlJXp18356687;     mPmNJNlJXp18356687 = mPmNJNlJXp1242369;     mPmNJNlJXp1242369 = mPmNJNlJXp59082673;     mPmNJNlJXp59082673 = mPmNJNlJXp12580065;     mPmNJNlJXp12580065 = mPmNJNlJXp42303687;     mPmNJNlJXp42303687 = mPmNJNlJXp47965809;     mPmNJNlJXp47965809 = mPmNJNlJXp60630847;     mPmNJNlJXp60630847 = mPmNJNlJXp59446006;     mPmNJNlJXp59446006 = mPmNJNlJXp49793970;     mPmNJNlJXp49793970 = mPmNJNlJXp52781647;     mPmNJNlJXp52781647 = mPmNJNlJXp527721;     mPmNJNlJXp527721 = mPmNJNlJXp21592388;     mPmNJNlJXp21592388 = mPmNJNlJXp45993933;     mPmNJNlJXp45993933 = mPmNJNlJXp23254027;     mPmNJNlJXp23254027 = mPmNJNlJXp31080678;     mPmNJNlJXp31080678 = mPmNJNlJXp45706059;     mPmNJNlJXp45706059 = mPmNJNlJXp85734177;     mPmNJNlJXp85734177 = mPmNJNlJXp68123042;     mPmNJNlJXp68123042 = mPmNJNlJXp65022799;     mPmNJNlJXp65022799 = mPmNJNlJXp85365918;     mPmNJNlJXp85365918 = mPmNJNlJXp22549210;     mPmNJNlJXp22549210 = mPmNJNlJXp66695196;     mPmNJNlJXp66695196 = mPmNJNlJXp50459559;     mPmNJNlJXp50459559 = mPmNJNlJXp81258011;     mPmNJNlJXp81258011 = mPmNJNlJXp84871849;     mPmNJNlJXp84871849 = mPmNJNlJXp8027637;     mPmNJNlJXp8027637 = mPmNJNlJXp14783387;     mPmNJNlJXp14783387 = mPmNJNlJXp12788026;     mPmNJNlJXp12788026 = mPmNJNlJXp54166687;     mPmNJNlJXp54166687 = mPmNJNlJXp63409056;     mPmNJNlJXp63409056 = mPmNJNlJXp72960236;     mPmNJNlJXp72960236 = mPmNJNlJXp49201164;     mPmNJNlJXp49201164 = mPmNJNlJXp23976169;     mPmNJNlJXp23976169 = mPmNJNlJXp80110336;     mPmNJNlJXp80110336 = mPmNJNlJXp72972892;     mPmNJNlJXp72972892 = mPmNJNlJXp89059781;     mPmNJNlJXp89059781 = mPmNJNlJXp57626032;     mPmNJNlJXp57626032 = mPmNJNlJXp50406511;     mPmNJNlJXp50406511 = mPmNJNlJXp42311412;     mPmNJNlJXp42311412 = mPmNJNlJXp60695152;     mPmNJNlJXp60695152 = mPmNJNlJXp33140298;     mPmNJNlJXp33140298 = mPmNJNlJXp42592094;     mPmNJNlJXp42592094 = mPmNJNlJXp79985538;     mPmNJNlJXp79985538 = mPmNJNlJXp99655690;     mPmNJNlJXp99655690 = mPmNJNlJXp78359211;     mPmNJNlJXp78359211 = mPmNJNlJXp33222843;     mPmNJNlJXp33222843 = mPmNJNlJXp37409236;     mPmNJNlJXp37409236 = mPmNJNlJXp92688034;     mPmNJNlJXp92688034 = mPmNJNlJXp2235989;     mPmNJNlJXp2235989 = mPmNJNlJXp7112727;     mPmNJNlJXp7112727 = mPmNJNlJXp66670340;     mPmNJNlJXp66670340 = mPmNJNlJXp24253030;     mPmNJNlJXp24253030 = mPmNJNlJXp52751787;     mPmNJNlJXp52751787 = mPmNJNlJXp21454784;     mPmNJNlJXp21454784 = mPmNJNlJXp23110426;     mPmNJNlJXp23110426 = mPmNJNlJXp51661490;     mPmNJNlJXp51661490 = mPmNJNlJXp50782809;     mPmNJNlJXp50782809 = mPmNJNlJXp77824662;     mPmNJNlJXp77824662 = mPmNJNlJXp27708215;     mPmNJNlJXp27708215 = mPmNJNlJXp34276050;     mPmNJNlJXp34276050 = mPmNJNlJXp33182422;     mPmNJNlJXp33182422 = mPmNJNlJXp47842821;     mPmNJNlJXp47842821 = mPmNJNlJXp5279320;     mPmNJNlJXp5279320 = mPmNJNlJXp86384913;     mPmNJNlJXp86384913 = mPmNJNlJXp79821410;     mPmNJNlJXp79821410 = mPmNJNlJXp51326556;     mPmNJNlJXp51326556 = mPmNJNlJXp97616218;     mPmNJNlJXp97616218 = mPmNJNlJXp65883597;     mPmNJNlJXp65883597 = mPmNJNlJXp50281135;     mPmNJNlJXp50281135 = mPmNJNlJXp42020896;     mPmNJNlJXp42020896 = mPmNJNlJXp88080027;     mPmNJNlJXp88080027 = mPmNJNlJXp35327667;     mPmNJNlJXp35327667 = mPmNJNlJXp25811630;     mPmNJNlJXp25811630 = mPmNJNlJXp4327647;     mPmNJNlJXp4327647 = mPmNJNlJXp52225620;     mPmNJNlJXp52225620 = mPmNJNlJXp79957116;     mPmNJNlJXp79957116 = mPmNJNlJXp86709658;     mPmNJNlJXp86709658 = mPmNJNlJXp50803868;     mPmNJNlJXp50803868 = mPmNJNlJXp2898801;     mPmNJNlJXp2898801 = mPmNJNlJXp51649007;     mPmNJNlJXp51649007 = mPmNJNlJXp70618401;     mPmNJNlJXp70618401 = mPmNJNlJXp22095353;     mPmNJNlJXp22095353 = mPmNJNlJXp10552037;     mPmNJNlJXp10552037 = mPmNJNlJXp47053960;     mPmNJNlJXp47053960 = mPmNJNlJXp96738715;     mPmNJNlJXp96738715 = mPmNJNlJXp48707207;     mPmNJNlJXp48707207 = mPmNJNlJXp96449377;     mPmNJNlJXp96449377 = mPmNJNlJXp2521385;     mPmNJNlJXp2521385 = mPmNJNlJXp56999910;     mPmNJNlJXp56999910 = mPmNJNlJXp21311402;     mPmNJNlJXp21311402 = mPmNJNlJXp38276973;     mPmNJNlJXp38276973 = mPmNJNlJXp79801370;     mPmNJNlJXp79801370 = mPmNJNlJXp22698296;     mPmNJNlJXp22698296 = mPmNJNlJXp8035363;     mPmNJNlJXp8035363 = mPmNJNlJXp27512731;     mPmNJNlJXp27512731 = mPmNJNlJXp52404518;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void LxwAcOBYgU28830906() {     int dgwWQvVpCr39326196 = -202378956;    int dgwWQvVpCr2023810 = -700146056;    int dgwWQvVpCr10470601 = -501508507;    int dgwWQvVpCr34683897 = -787620211;    int dgwWQvVpCr96388722 = -556454271;    int dgwWQvVpCr31822364 = 93979571;    int dgwWQvVpCr51288037 = -995016007;    int dgwWQvVpCr41799022 = 35936072;    int dgwWQvVpCr90965754 = -547144678;    int dgwWQvVpCr15804073 = -505501514;    int dgwWQvVpCr67646539 = -809319335;    int dgwWQvVpCr30257324 = -39247489;    int dgwWQvVpCr20072456 = -555697745;    int dgwWQvVpCr93112079 = -34977605;    int dgwWQvVpCr76494517 = -712079147;    int dgwWQvVpCr35806879 = -264181432;    int dgwWQvVpCr71432562 = -570151492;    int dgwWQvVpCr75210690 = -951201767;    int dgwWQvVpCr84413597 = -599869688;    int dgwWQvVpCr87833505 = -737863701;    int dgwWQvVpCr13171728 = -922721239;    int dgwWQvVpCr24424137 = -417923277;    int dgwWQvVpCr37769382 = -256927304;    int dgwWQvVpCr41379750 = 52019394;    int dgwWQvVpCr41330904 = -682011539;    int dgwWQvVpCr16728110 = 40125619;    int dgwWQvVpCr23585625 = -206058895;    int dgwWQvVpCr51046109 = -29928718;    int dgwWQvVpCr54160804 = 2434428;    int dgwWQvVpCr64585488 = -520036917;    int dgwWQvVpCr67147889 = -825363671;    int dgwWQvVpCr19225417 = 68001379;    int dgwWQvVpCr67061108 = -288152789;    int dgwWQvVpCr84089785 = -291150693;    int dgwWQvVpCr56188005 = -635760521;    int dgwWQvVpCr5920452 = -711508059;    int dgwWQvVpCr5170428 = 56294074;    int dgwWQvVpCr96304545 = -539567098;    int dgwWQvVpCr33341503 = -528323573;    int dgwWQvVpCr759968 = -3926959;    int dgwWQvVpCr72221212 = -731902272;    int dgwWQvVpCr42466958 = -480582493;    int dgwWQvVpCr26856685 = -239773673;    int dgwWQvVpCr60856091 = -741761810;    int dgwWQvVpCr12680013 = -736535252;    int dgwWQvVpCr35707220 = -402426254;    int dgwWQvVpCr63242120 = 52573087;    int dgwWQvVpCr82241482 = -832555314;    int dgwWQvVpCr89881319 = -697431068;    int dgwWQvVpCr70958790 = -718063064;    int dgwWQvVpCr74580705 = -638076372;    int dgwWQvVpCr42322278 = -445682091;    int dgwWQvVpCr77829637 = -294889711;    int dgwWQvVpCr56780958 = -27937316;    int dgwWQvVpCr15659178 = -370430962;    int dgwWQvVpCr14902059 = -784455680;    int dgwWQvVpCr64254428 = -343218752;    int dgwWQvVpCr69090851 = -453527902;    int dgwWQvVpCr93352992 = -5608672;    int dgwWQvVpCr79660612 = -496579891;    int dgwWQvVpCr8236740 = -699961534;    int dgwWQvVpCr241928 = -865087289;    int dgwWQvVpCr87638217 = -966498357;    int dgwWQvVpCr26380266 = 72892239;    int dgwWQvVpCr48656184 = -680137844;    int dgwWQvVpCr48421122 = -777320714;    int dgwWQvVpCr63196216 = -751094700;    int dgwWQvVpCr35982670 = -164547053;    int dgwWQvVpCr36924074 = -399217085;    int dgwWQvVpCr70574065 = 99428912;    int dgwWQvVpCr30636451 = -220475507;    int dgwWQvVpCr75128017 = 69415605;    int dgwWQvVpCr41869188 = -322878195;    int dgwWQvVpCr83653630 = -495942730;    int dgwWQvVpCr15612293 = 94038571;    int dgwWQvVpCr70704770 = -342138747;    int dgwWQvVpCr97567452 = -78149605;    int dgwWQvVpCr76913290 = -515165494;    int dgwWQvVpCr28699737 = -211445354;    int dgwWQvVpCr5623684 = -179585286;    int dgwWQvVpCr53485990 = 87552531;    int dgwWQvVpCr41344143 = -373503582;    int dgwWQvVpCr61164790 = -332497651;    int dgwWQvVpCr83202014 = -279502508;    int dgwWQvVpCr90004783 = -881960546;    int dgwWQvVpCr24825611 = -279681580;    int dgwWQvVpCr41395780 = -637108911;    int dgwWQvVpCr10280150 = -160215474;    int dgwWQvVpCr68430607 = -920719731;    int dgwWQvVpCr41285947 = -851304842;    int dgwWQvVpCr41666024 = -268289308;    int dgwWQvVpCr36079577 = -490178025;    int dgwWQvVpCr2951553 = -433958426;    int dgwWQvVpCr53680890 = 68256317;    int dgwWQvVpCr92523227 = -303965425;    int dgwWQvVpCr71979285 = -866814984;    int dgwWQvVpCr54828740 = -514084136;    int dgwWQvVpCr476419 = -212665913;    int dgwWQvVpCr12199908 = 38376034;    int dgwWQvVpCr64258891 = -202378956;     dgwWQvVpCr39326196 = dgwWQvVpCr2023810;     dgwWQvVpCr2023810 = dgwWQvVpCr10470601;     dgwWQvVpCr10470601 = dgwWQvVpCr34683897;     dgwWQvVpCr34683897 = dgwWQvVpCr96388722;     dgwWQvVpCr96388722 = dgwWQvVpCr31822364;     dgwWQvVpCr31822364 = dgwWQvVpCr51288037;     dgwWQvVpCr51288037 = dgwWQvVpCr41799022;     dgwWQvVpCr41799022 = dgwWQvVpCr90965754;     dgwWQvVpCr90965754 = dgwWQvVpCr15804073;     dgwWQvVpCr15804073 = dgwWQvVpCr67646539;     dgwWQvVpCr67646539 = dgwWQvVpCr30257324;     dgwWQvVpCr30257324 = dgwWQvVpCr20072456;     dgwWQvVpCr20072456 = dgwWQvVpCr93112079;     dgwWQvVpCr93112079 = dgwWQvVpCr76494517;     dgwWQvVpCr76494517 = dgwWQvVpCr35806879;     dgwWQvVpCr35806879 = dgwWQvVpCr71432562;     dgwWQvVpCr71432562 = dgwWQvVpCr75210690;     dgwWQvVpCr75210690 = dgwWQvVpCr84413597;     dgwWQvVpCr84413597 = dgwWQvVpCr87833505;     dgwWQvVpCr87833505 = dgwWQvVpCr13171728;     dgwWQvVpCr13171728 = dgwWQvVpCr24424137;     dgwWQvVpCr24424137 = dgwWQvVpCr37769382;     dgwWQvVpCr37769382 = dgwWQvVpCr41379750;     dgwWQvVpCr41379750 = dgwWQvVpCr41330904;     dgwWQvVpCr41330904 = dgwWQvVpCr16728110;     dgwWQvVpCr16728110 = dgwWQvVpCr23585625;     dgwWQvVpCr23585625 = dgwWQvVpCr51046109;     dgwWQvVpCr51046109 = dgwWQvVpCr54160804;     dgwWQvVpCr54160804 = dgwWQvVpCr64585488;     dgwWQvVpCr64585488 = dgwWQvVpCr67147889;     dgwWQvVpCr67147889 = dgwWQvVpCr19225417;     dgwWQvVpCr19225417 = dgwWQvVpCr67061108;     dgwWQvVpCr67061108 = dgwWQvVpCr84089785;     dgwWQvVpCr84089785 = dgwWQvVpCr56188005;     dgwWQvVpCr56188005 = dgwWQvVpCr5920452;     dgwWQvVpCr5920452 = dgwWQvVpCr5170428;     dgwWQvVpCr5170428 = dgwWQvVpCr96304545;     dgwWQvVpCr96304545 = dgwWQvVpCr33341503;     dgwWQvVpCr33341503 = dgwWQvVpCr759968;     dgwWQvVpCr759968 = dgwWQvVpCr72221212;     dgwWQvVpCr72221212 = dgwWQvVpCr42466958;     dgwWQvVpCr42466958 = dgwWQvVpCr26856685;     dgwWQvVpCr26856685 = dgwWQvVpCr60856091;     dgwWQvVpCr60856091 = dgwWQvVpCr12680013;     dgwWQvVpCr12680013 = dgwWQvVpCr35707220;     dgwWQvVpCr35707220 = dgwWQvVpCr63242120;     dgwWQvVpCr63242120 = dgwWQvVpCr82241482;     dgwWQvVpCr82241482 = dgwWQvVpCr89881319;     dgwWQvVpCr89881319 = dgwWQvVpCr70958790;     dgwWQvVpCr70958790 = dgwWQvVpCr74580705;     dgwWQvVpCr74580705 = dgwWQvVpCr42322278;     dgwWQvVpCr42322278 = dgwWQvVpCr77829637;     dgwWQvVpCr77829637 = dgwWQvVpCr56780958;     dgwWQvVpCr56780958 = dgwWQvVpCr15659178;     dgwWQvVpCr15659178 = dgwWQvVpCr14902059;     dgwWQvVpCr14902059 = dgwWQvVpCr64254428;     dgwWQvVpCr64254428 = dgwWQvVpCr69090851;     dgwWQvVpCr69090851 = dgwWQvVpCr93352992;     dgwWQvVpCr93352992 = dgwWQvVpCr79660612;     dgwWQvVpCr79660612 = dgwWQvVpCr8236740;     dgwWQvVpCr8236740 = dgwWQvVpCr241928;     dgwWQvVpCr241928 = dgwWQvVpCr87638217;     dgwWQvVpCr87638217 = dgwWQvVpCr26380266;     dgwWQvVpCr26380266 = dgwWQvVpCr48656184;     dgwWQvVpCr48656184 = dgwWQvVpCr48421122;     dgwWQvVpCr48421122 = dgwWQvVpCr63196216;     dgwWQvVpCr63196216 = dgwWQvVpCr35982670;     dgwWQvVpCr35982670 = dgwWQvVpCr36924074;     dgwWQvVpCr36924074 = dgwWQvVpCr70574065;     dgwWQvVpCr70574065 = dgwWQvVpCr30636451;     dgwWQvVpCr30636451 = dgwWQvVpCr75128017;     dgwWQvVpCr75128017 = dgwWQvVpCr41869188;     dgwWQvVpCr41869188 = dgwWQvVpCr83653630;     dgwWQvVpCr83653630 = dgwWQvVpCr15612293;     dgwWQvVpCr15612293 = dgwWQvVpCr70704770;     dgwWQvVpCr70704770 = dgwWQvVpCr97567452;     dgwWQvVpCr97567452 = dgwWQvVpCr76913290;     dgwWQvVpCr76913290 = dgwWQvVpCr28699737;     dgwWQvVpCr28699737 = dgwWQvVpCr5623684;     dgwWQvVpCr5623684 = dgwWQvVpCr53485990;     dgwWQvVpCr53485990 = dgwWQvVpCr41344143;     dgwWQvVpCr41344143 = dgwWQvVpCr61164790;     dgwWQvVpCr61164790 = dgwWQvVpCr83202014;     dgwWQvVpCr83202014 = dgwWQvVpCr90004783;     dgwWQvVpCr90004783 = dgwWQvVpCr24825611;     dgwWQvVpCr24825611 = dgwWQvVpCr41395780;     dgwWQvVpCr41395780 = dgwWQvVpCr10280150;     dgwWQvVpCr10280150 = dgwWQvVpCr68430607;     dgwWQvVpCr68430607 = dgwWQvVpCr41285947;     dgwWQvVpCr41285947 = dgwWQvVpCr41666024;     dgwWQvVpCr41666024 = dgwWQvVpCr36079577;     dgwWQvVpCr36079577 = dgwWQvVpCr2951553;     dgwWQvVpCr2951553 = dgwWQvVpCr53680890;     dgwWQvVpCr53680890 = dgwWQvVpCr92523227;     dgwWQvVpCr92523227 = dgwWQvVpCr71979285;     dgwWQvVpCr71979285 = dgwWQvVpCr54828740;     dgwWQvVpCr54828740 = dgwWQvVpCr476419;     dgwWQvVpCr476419 = dgwWQvVpCr12199908;     dgwWQvVpCr12199908 = dgwWQvVpCr64258891;     dgwWQvVpCr64258891 = dgwWQvVpCr39326196;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void ZiDltuyRiw4843026() {     int zCaAmgfsjy26247874 = -343030161;    int zCaAmgfsjy11671549 = -810732629;    int zCaAmgfsjy3166615 = 67376110;    int zCaAmgfsjy62547091 = -793417501;    int zCaAmgfsjy47117809 = -410267748;    int zCaAmgfsjy45288041 = -303713723;    int zCaAmgfsjy1333706 = -676814046;    int zCaAmgfsjy24515370 = -494603749;    int zCaAmgfsjy69351444 = -756655380;    int zCaAmgfsjy89304459 = -220362720;    int zCaAmgfsjy87327269 = -963408079;    int zCaAmgfsjy99883801 = -161350373;    int zCaAmgfsjy80698904 = -644061034;    int zCaAmgfsjy36430188 = -931476004;    int zCaAmgfsjy207387 = -898844697;    int zCaAmgfsjy71086038 = -242833018;    int zCaAmgfsjy21272738 = -140071329;    int zCaAmgfsjy4427448 = -626588011;    int zCaAmgfsjy45573169 = 63609341;    int zCaAmgfsjy44586333 = -75961540;    int zCaAmgfsjy80637396 = -967400233;    int zCaAmgfsjy63114096 = -246045180;    int zCaAmgfsjy7415722 = -154333554;    int zCaAmgfsjy17736700 = -982069381;    int zCaAmgfsjy97295889 = -706279319;    int zCaAmgfsjy10907010 = -623397726;    int zCaAmgfsjy80476052 = -458160960;    int zCaAmgfsjy51632659 = -257577986;    int zCaAmgfsjy27063598 = -413798203;    int zCaAmgfsjy44299127 = -81256987;    int zCaAmgfsjy26268142 = -741792570;    int zCaAmgfsjy23667447 = -102920323;    int zCaAmgfsjy21334192 = -871844523;    int zCaAmgfsjy14012883 = -427265462;    int zCaAmgfsjy48966954 = 92269383;    int zCaAmgfsjy38880667 = -892103876;    int zCaAmgfsjy61139691 = 88388784;    int zCaAmgfsjy68632922 = -419799668;    int zCaAmgfsjy86572668 = -891174745;    int zCaAmgfsjy28547043 = -284320128;    int zCaAmgfsjy55382643 = -453824923;    int zCaAmgfsjy27307883 = -837737975;    int zCaAmgfsjy3306858 = -33253725;    int zCaAmgfsjy79400770 = -791757493;    int zCaAmgfsjy64664873 = -92426970;    int zCaAmgfsjy38274142 = -357776387;    int zCaAmgfsjy83892146 = -523131427;    int zCaAmgfsjy84497425 = 68674933;    int zCaAmgfsjy80106947 = -547174747;    int zCaAmgfsjy63558369 = 43918607;    int zCaAmgfsjy15938568 = -697431099;    int zCaAmgfsjy47235321 = 55005226;    int zCaAmgfsjy62971240 = -102809395;    int zCaAmgfsjy11325928 = -644029476;    int zCaAmgfsjy24205630 = 78455791;    int zCaAmgfsjy63133777 = 3015018;    int zCaAmgfsjy4255827 = -556399075;    int zCaAmgfsjy85429915 = 49445491;    int zCaAmgfsjy65251202 = 12861817;    int zCaAmgfsjy36210800 = -786870022;    int zCaAmgfsjy64811989 = -845552764;    int zCaAmgfsjy49701046 = -319236061;    int zCaAmgfsjy97451772 = 19194453;    int zCaAmgfsjy25052318 = -575398393;    int zCaAmgfsjy63036317 = -478570151;    int zCaAmgfsjy63659822 = -760487756;    int zCaAmgfsjy78549610 = -289505851;    int zCaAmgfsjy66686021 = -116795573;    int zCaAmgfsjy87463233 = -923745388;    int zCaAmgfsjy61326720 = 93259179;    int zCaAmgfsjy9946347 = -231221803;    int zCaAmgfsjy52639815 = -720271662;    int zCaAmgfsjy17854780 = -735413267;    int zCaAmgfsjy17026126 = -652070531;    int zCaAmgfsjy89203689 = -622136617;    int zCaAmgfsjy53329513 = -29662258;    int zCaAmgfsjy59807238 = -112791455;    int zCaAmgfsjy28014951 = -362576062;    int zCaAmgfsjy53071827 = -789642412;    int zCaAmgfsjy59021747 = -248502932;    int zCaAmgfsjy27014864 = -266300;    int zCaAmgfsjy95978626 = -426835893;    int zCaAmgfsjy71525712 = -710403239;    int zCaAmgfsjy63505228 = -357716810;    int zCaAmgfsjy28360559 = -383825889;    int zCaAmgfsjy79032821 = -696797796;    int zCaAmgfsjy60696207 = 99889072;    int zCaAmgfsjy10008264 = -127815047;    int zCaAmgfsjy89807253 = -405721253;    int zCaAmgfsjy85833177 = -910745636;    int zCaAmgfsjy34624840 = -235704801;    int zCaAmgfsjy75709776 = -961056707;    int zCaAmgfsjy3381721 = -332661485;    int zCaAmgfsjy50361869 = -4304723;    int zCaAmgfsjy63735053 = -438767365;    int zCaAmgfsjy5681598 = -34588863;    int zCaAmgfsjy29856111 = -756932429;    int zCaAmgfsjy78254540 = -457855332;    int zCaAmgfsjy16364454 = -213187342;    int zCaAmgfsjy1005051 = -343030161;     zCaAmgfsjy26247874 = zCaAmgfsjy11671549;     zCaAmgfsjy11671549 = zCaAmgfsjy3166615;     zCaAmgfsjy3166615 = zCaAmgfsjy62547091;     zCaAmgfsjy62547091 = zCaAmgfsjy47117809;     zCaAmgfsjy47117809 = zCaAmgfsjy45288041;     zCaAmgfsjy45288041 = zCaAmgfsjy1333706;     zCaAmgfsjy1333706 = zCaAmgfsjy24515370;     zCaAmgfsjy24515370 = zCaAmgfsjy69351444;     zCaAmgfsjy69351444 = zCaAmgfsjy89304459;     zCaAmgfsjy89304459 = zCaAmgfsjy87327269;     zCaAmgfsjy87327269 = zCaAmgfsjy99883801;     zCaAmgfsjy99883801 = zCaAmgfsjy80698904;     zCaAmgfsjy80698904 = zCaAmgfsjy36430188;     zCaAmgfsjy36430188 = zCaAmgfsjy207387;     zCaAmgfsjy207387 = zCaAmgfsjy71086038;     zCaAmgfsjy71086038 = zCaAmgfsjy21272738;     zCaAmgfsjy21272738 = zCaAmgfsjy4427448;     zCaAmgfsjy4427448 = zCaAmgfsjy45573169;     zCaAmgfsjy45573169 = zCaAmgfsjy44586333;     zCaAmgfsjy44586333 = zCaAmgfsjy80637396;     zCaAmgfsjy80637396 = zCaAmgfsjy63114096;     zCaAmgfsjy63114096 = zCaAmgfsjy7415722;     zCaAmgfsjy7415722 = zCaAmgfsjy17736700;     zCaAmgfsjy17736700 = zCaAmgfsjy97295889;     zCaAmgfsjy97295889 = zCaAmgfsjy10907010;     zCaAmgfsjy10907010 = zCaAmgfsjy80476052;     zCaAmgfsjy80476052 = zCaAmgfsjy51632659;     zCaAmgfsjy51632659 = zCaAmgfsjy27063598;     zCaAmgfsjy27063598 = zCaAmgfsjy44299127;     zCaAmgfsjy44299127 = zCaAmgfsjy26268142;     zCaAmgfsjy26268142 = zCaAmgfsjy23667447;     zCaAmgfsjy23667447 = zCaAmgfsjy21334192;     zCaAmgfsjy21334192 = zCaAmgfsjy14012883;     zCaAmgfsjy14012883 = zCaAmgfsjy48966954;     zCaAmgfsjy48966954 = zCaAmgfsjy38880667;     zCaAmgfsjy38880667 = zCaAmgfsjy61139691;     zCaAmgfsjy61139691 = zCaAmgfsjy68632922;     zCaAmgfsjy68632922 = zCaAmgfsjy86572668;     zCaAmgfsjy86572668 = zCaAmgfsjy28547043;     zCaAmgfsjy28547043 = zCaAmgfsjy55382643;     zCaAmgfsjy55382643 = zCaAmgfsjy27307883;     zCaAmgfsjy27307883 = zCaAmgfsjy3306858;     zCaAmgfsjy3306858 = zCaAmgfsjy79400770;     zCaAmgfsjy79400770 = zCaAmgfsjy64664873;     zCaAmgfsjy64664873 = zCaAmgfsjy38274142;     zCaAmgfsjy38274142 = zCaAmgfsjy83892146;     zCaAmgfsjy83892146 = zCaAmgfsjy84497425;     zCaAmgfsjy84497425 = zCaAmgfsjy80106947;     zCaAmgfsjy80106947 = zCaAmgfsjy63558369;     zCaAmgfsjy63558369 = zCaAmgfsjy15938568;     zCaAmgfsjy15938568 = zCaAmgfsjy47235321;     zCaAmgfsjy47235321 = zCaAmgfsjy62971240;     zCaAmgfsjy62971240 = zCaAmgfsjy11325928;     zCaAmgfsjy11325928 = zCaAmgfsjy24205630;     zCaAmgfsjy24205630 = zCaAmgfsjy63133777;     zCaAmgfsjy63133777 = zCaAmgfsjy4255827;     zCaAmgfsjy4255827 = zCaAmgfsjy85429915;     zCaAmgfsjy85429915 = zCaAmgfsjy65251202;     zCaAmgfsjy65251202 = zCaAmgfsjy36210800;     zCaAmgfsjy36210800 = zCaAmgfsjy64811989;     zCaAmgfsjy64811989 = zCaAmgfsjy49701046;     zCaAmgfsjy49701046 = zCaAmgfsjy97451772;     zCaAmgfsjy97451772 = zCaAmgfsjy25052318;     zCaAmgfsjy25052318 = zCaAmgfsjy63036317;     zCaAmgfsjy63036317 = zCaAmgfsjy63659822;     zCaAmgfsjy63659822 = zCaAmgfsjy78549610;     zCaAmgfsjy78549610 = zCaAmgfsjy66686021;     zCaAmgfsjy66686021 = zCaAmgfsjy87463233;     zCaAmgfsjy87463233 = zCaAmgfsjy61326720;     zCaAmgfsjy61326720 = zCaAmgfsjy9946347;     zCaAmgfsjy9946347 = zCaAmgfsjy52639815;     zCaAmgfsjy52639815 = zCaAmgfsjy17854780;     zCaAmgfsjy17854780 = zCaAmgfsjy17026126;     zCaAmgfsjy17026126 = zCaAmgfsjy89203689;     zCaAmgfsjy89203689 = zCaAmgfsjy53329513;     zCaAmgfsjy53329513 = zCaAmgfsjy59807238;     zCaAmgfsjy59807238 = zCaAmgfsjy28014951;     zCaAmgfsjy28014951 = zCaAmgfsjy53071827;     zCaAmgfsjy53071827 = zCaAmgfsjy59021747;     zCaAmgfsjy59021747 = zCaAmgfsjy27014864;     zCaAmgfsjy27014864 = zCaAmgfsjy95978626;     zCaAmgfsjy95978626 = zCaAmgfsjy71525712;     zCaAmgfsjy71525712 = zCaAmgfsjy63505228;     zCaAmgfsjy63505228 = zCaAmgfsjy28360559;     zCaAmgfsjy28360559 = zCaAmgfsjy79032821;     zCaAmgfsjy79032821 = zCaAmgfsjy60696207;     zCaAmgfsjy60696207 = zCaAmgfsjy10008264;     zCaAmgfsjy10008264 = zCaAmgfsjy89807253;     zCaAmgfsjy89807253 = zCaAmgfsjy85833177;     zCaAmgfsjy85833177 = zCaAmgfsjy34624840;     zCaAmgfsjy34624840 = zCaAmgfsjy75709776;     zCaAmgfsjy75709776 = zCaAmgfsjy3381721;     zCaAmgfsjy3381721 = zCaAmgfsjy50361869;     zCaAmgfsjy50361869 = zCaAmgfsjy63735053;     zCaAmgfsjy63735053 = zCaAmgfsjy5681598;     zCaAmgfsjy5681598 = zCaAmgfsjy29856111;     zCaAmgfsjy29856111 = zCaAmgfsjy78254540;     zCaAmgfsjy78254540 = zCaAmgfsjy16364454;     zCaAmgfsjy16364454 = zCaAmgfsjy1005051;     zCaAmgfsjy1005051 = zCaAmgfsjy26247874;}
// Junk Finished
