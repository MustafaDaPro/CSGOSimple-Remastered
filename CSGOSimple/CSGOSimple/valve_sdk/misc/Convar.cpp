#include "Convar.hpp"

#include "../sdk.hpp"

#include "characterset.hpp"
#include "UtlBuffer.hpp"

#define ALIGN_VALUE( val, alignment ) ( ( val + alignment - 1 ) & ~( alignment - 1 ) ) 
#define stackalloc( _size )		_alloca( ALIGN_VALUE( _size, 16 ) )

ConCommandBase *ConCommandBase::s_pConCommandBases = NULL;
ConCommandBase *ConCommandBase::s_pRegisteredCommands = NULL;
IConCommandBaseAccessor	*ConCommandBase::s_pAccessor = NULL;
static int s_nDLLIdentifier = -1;
static int s_nCVarFlag = 0;
static bool s_bRegistered = false;

class CDefaultAccessor : public IConCommandBaseAccessor
{
public:
    virtual bool RegisterConCommandBase(ConCommandBase *pVar)
    {
        // Link to engine's list instead
        g_CVar->RegisterConCommand(pVar);
        return true;
    }
};

static CDefaultAccessor s_DefaultAccessor;

//-----------------------------------------------------------------------------
// Called by the framework to register ConCommandBases with the ICVar
//-----------------------------------------------------------------------------
void ConVar_Register(int nCVarFlag, IConCommandBaseAccessor *pAccessor)
{
    if(!g_CVar || s_bRegistered)
        return;

    assert(s_nDLLIdentifier < 0);
    s_bRegistered = true;
    s_nCVarFlag = nCVarFlag;
    s_nDLLIdentifier = g_CVar->AllocateDLLIdentifier();

    ConCommandBase *pCur, *pNext;

    ConCommandBase::s_pAccessor = pAccessor ? pAccessor : &s_DefaultAccessor;
    pCur = ConCommandBase::s_pConCommandBases;

    while(pCur) {
        pNext = pCur->m_pNext;
        pCur->AddFlags(s_nCVarFlag);
        pCur->Init();

        ConCommandBase::s_pRegisteredCommands = pCur;

        pCur = pNext;
    }

    ConCommandBase::s_pConCommandBases = NULL;
}

void ConVar_Unregister()
{
    if(!g_CVar || !s_bRegistered)
        return;

    assert(s_nDLLIdentifier >= 0);
    g_CVar->UnregisterConCommands(s_nDLLIdentifier);
    s_nDLLIdentifier = -1;
    s_bRegistered = false;
}

ConCommandBase::ConCommandBase(void)
{
    m_bRegistered = false;
    m_pszName = NULL;
    m_pszHelpString = NULL;

    m_nFlags = 0;
    m_pNext = NULL;
}

ConCommandBase::ConCommandBase(const char *pName, const char *pHelpString /*=0*/, int flags /*= 0*/)
{
    Create(pName, pHelpString, flags);
}

ConCommandBase::~ConCommandBase(void)
{
}

bool ConCommandBase::IsCommand(void) const
{
    //	assert( 0 ); This can't assert. . causes a recursive assert in Sys_Printf, etc.
    return true;
}

CVarDLLIdentifier_t ConCommandBase::GetDLLIdentifier() const
{
    return s_nDLLIdentifier;
}

void ConCommandBase::Create(const char *pName, const char *pHelpString /*= 0*/, int flags /*= 0*/)
{
    static const char *empty_string = "";

    m_bRegistered = false;

    // Name should be static data
    m_pszName = pName;
    m_pszHelpString = pHelpString ? pHelpString : empty_string;

    m_nFlags = flags;

    if(!(m_nFlags & FCVAR_UNREGISTERED)) {
        m_pNext = s_pConCommandBases;
        s_pConCommandBases = this;
    } else {
        m_pNext = NULL;
    }
}

void ConCommandBase::Init()
{
    if(s_pAccessor) {
        s_pAccessor->RegisterConCommandBase(this);
    }
}

void ConCommandBase::Shutdown()
{
    if(g_CVar) {
        g_CVar->UnregisterConCommand(this);
    }
}

const char *ConCommandBase::GetName(void) const
{
    return m_pszName;
}

bool ConCommandBase::IsFlagSet(int flag) const
{
    return (flag & m_nFlags) ? true : false;
}

void ConCommandBase::AddFlags(int flags)
{
    m_nFlags |= flags;
}

void ConCommandBase::RemoveFlags(int flags)
{
    m_nFlags &= ~flags;
}

int ConCommandBase::GetFlags(void) const
{
    return m_nFlags;
}

const ConCommandBase *ConCommandBase::GetNext(void) const
{
    return m_pNext;
}

ConCommandBase *ConCommandBase::GetNext(void)
{
    return m_pNext;
}

char *ConCommandBase::CopyString(const char *from)
{
    int		len;
    char	*to;

    len = strlen(from);
    if(len <= 0) {
        to = new char[1];
        to[0] = 0;
    } else {
        to = new char[len + 1];
        strncpy_s(to, len + 1, from, len + 1);
    }
    return to;
}

const char *ConCommandBase::GetHelpText(void) const
{
    return m_pszHelpString;
}

bool ConCommandBase::IsRegistered(void) const
{
    return m_bRegistered;
}

static characterset_t s_BreakSet;
static bool s_bBuiltBreakSet = false;

CCommand::CCommand()
{
    if(!s_bBuiltBreakSet) {
        s_bBuiltBreakSet = true;
        CharacterSetBuild(&s_BreakSet, "{}()':");
    }

    Reset();
}

CCommand::CCommand(int nArgC, const char **ppArgV)
{
    assert(nArgC > 0);

    if(!s_bBuiltBreakSet) {
        s_bBuiltBreakSet = true;
        CharacterSetBuild(&s_BreakSet, "{}()':");
    }

    Reset();

    char *pBuf = m_pArgvBuffer;
    char *pSBuf = m_pArgSBuffer;
    m_nArgc = nArgC;
    for(int i = 0; i < nArgC; ++i) {
        m_ppArgv[i] = pBuf;
        int nLen = strlen(ppArgV[i]);
        memcpy(pBuf, ppArgV[i], nLen + 1);
        if(i == 0) {
            m_nArgv0Size = nLen;
        }
        pBuf += nLen + 1;

        bool bContainsSpace = strchr(ppArgV[i], ' ') != NULL;
        if(bContainsSpace) {
            *pSBuf++ = '\"';
        }
        memcpy(pSBuf, ppArgV[i], nLen);
        pSBuf += nLen;
        if(bContainsSpace) {
            *pSBuf++ = '\"';
        }

        if(i != nArgC - 1) {
            *pSBuf++ = ' ';
        }
    }
}

void CCommand::Reset()
{
    m_nArgc = 0;
    m_nArgv0Size = 0;
    m_pArgSBuffer[0] = 0;
}

characterset_t* CCommand::DefaultBreakSet()
{
    return &s_BreakSet;
}

bool CCommand::Tokenize(const char *pCommand, characterset_t *pBreakSet)
{
    Reset();
    if(!pCommand)
        return false;

    // Use default break Set
    if(!pBreakSet) {
        pBreakSet = &s_BreakSet;
    }

    // Copy the current command into a temp buffer
    // NOTE: This is here to avoid the pointers returned by DequeueNextCommand
    // to become invalid by calling AddText. Is there a way we can avoid the memcpy?
    int nLen = strlen(pCommand);
    if(nLen >= COMMAND_MAX_LENGTH - 1) {
        //Warning("CCommand::Tokenize: Encountered command which overflows the tokenizer buffer.. Skipping!\n");
        return false;
    }

    memcpy(m_pArgSBuffer, pCommand, nLen + 1);

    // Parse the current command into the current command buffer
    CUtlBuffer bufParse(m_pArgSBuffer, nLen, CUtlBuffer::TEXT_BUFFER | CUtlBuffer::READ_ONLY);
    int nArgvBufferSize = 0;
    while(bufParse.IsValid() && (m_nArgc < COMMAND_MAX_ARGC)) {
        char *pArgvBuf = &m_pArgvBuffer[nArgvBufferSize];
        int nMaxLen = COMMAND_MAX_LENGTH - nArgvBufferSize;
        int nStartGet = bufParse.TellGet();
        int	nSize = bufParse.ParseToken(pBreakSet, pArgvBuf, nMaxLen);
        if(nSize < 0)
            break;

        // Check for overflow condition
        if(nMaxLen == nSize) {
            Reset();
            return false;
        }

        if(m_nArgc == 1) {
            // Deal with the case where the arguments were quoted
            m_nArgv0Size = bufParse.TellGet();
            bool bFoundEndQuote = m_pArgSBuffer[m_nArgv0Size - 1] == '\"';
            if(bFoundEndQuote) {
                --m_nArgv0Size;
            }
            m_nArgv0Size -= nSize;
            assert(m_nArgv0Size != 0);

            // The StartGet check is to handle this case: "foo"bar
            // which will parse into 2 different args. ArgS should point to bar.
            bool bFoundStartQuote = (m_nArgv0Size > nStartGet) && (m_pArgSBuffer[m_nArgv0Size - 1] == '\"');
            assert(bFoundEndQuote == bFoundStartQuote);
            if(bFoundStartQuote) {
                --m_nArgv0Size;
            }
        }

        m_ppArgv[m_nArgc++] = pArgvBuf;
        if(m_nArgc >= COMMAND_MAX_ARGC) {
            //Warning("CCommand::Tokenize: Encountered command which overflows the argument buffer.. Clamped!\n");
        }

        nArgvBufferSize += nSize + 1;
        assert(nArgvBufferSize <= COMMAND_MAX_LENGTH);
    }

    return true;
}

const char* CCommand::FindArg(const char *pName) const
{
    int nArgC = ArgC();
    for(int i = 1; i < nArgC; i++) {
        if(!_stricmp(Arg(i), pName))
            return (i + 1) < nArgC ? Arg(i + 1) : "";
    }
    return 0;
}

int CCommand::FindArgInt(const char *pName, int nDefaultVal) const
{
    const char *pVal = FindArg(pName);
    if(pVal)
        return atoi(pVal);
    else
        return nDefaultVal;
}

int DefaultCompletionFunc(const char *partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH])
{
    return 0;
}

ConCommand::ConCommand(const char *pName, FnCommandCallbackV1_t callback, const char *pHelpString /*= 0*/, int flags /*= 0*/, FnCommandCompletionCallback completionFunc /*= 0*/)
{
    // Set the callback
    m_fnCommandCallbackV1 = callback;
    m_bUsingNewCommandCallback = false;
    m_bUsingCommandCallbackInterface = false;
    m_fnCompletionCallback = completionFunc ? completionFunc : DefaultCompletionFunc;
    m_bHasCompletionCallback = completionFunc != 0 ? true : false;

    // Setup the rest
    BaseClass::Create(pName, pHelpString, flags);
}

ConCommand::ConCommand(const char *pName, FnCommandCallback_t callback, const char *pHelpString /*= 0*/, int flags /*= 0*/, FnCommandCompletionCallback completionFunc /*= 0*/)
{
    // Set the callback
    m_fnCommandCallback = callback;
    m_bUsingNewCommandCallback = true;
    m_fnCompletionCallback = completionFunc ? completionFunc : DefaultCompletionFunc;
    m_bHasCompletionCallback = completionFunc != 0 ? true : false;
    m_bUsingCommandCallbackInterface = false;

    // Setup the rest
    BaseClass::Create(pName, pHelpString, flags);
}

ConCommand::ConCommand(const char *pName, ICommandCallback *pCallback, const char *pHelpString /*= 0*/, int flags /*= 0*/, ICommandCompletionCallback *pCompletionCallback /*= 0*/)
{
    // Set the callback
    m_pCommandCallback = pCallback;
    m_bUsingNewCommandCallback = false;
    m_pCommandCompletionCallback = pCompletionCallback;
    m_bHasCompletionCallback = (pCompletionCallback != 0);
    m_bUsingCommandCallbackInterface = true;

    // Setup the rest
    BaseClass::Create(pName, pHelpString, flags);
}

ConCommand::~ConCommand(void)
{
}

bool ConCommand::IsCommand(void) const
{
    return true;
}

void ConCommand::Dispatch(const CCommand &command)
{
    if(m_bUsingNewCommandCallback) {
        if(m_fnCommandCallback) {
            (*m_fnCommandCallback)(command);
            return;
        }
    } else if(m_bUsingCommandCallbackInterface) {
        if(m_pCommandCallback) {
            m_pCommandCallback->CommandCallback(command);
            return;
        }
    } else {
        if(m_fnCommandCallbackV1) {
            (*m_fnCommandCallbackV1)();
            return;
        }
    }

    // Command without callback!!!
    //AssertMsg(0, ("Encountered ConCommand without a callback!\n"));
}

int	ConCommand::AutoCompleteSuggest(const char *partial, CUtlVector< CUtlString > &commands)
{
    if(m_bUsingCommandCallbackInterface) {
        if(!m_pCommandCompletionCallback)
            return 0;
        return m_pCommandCompletionCallback->CommandCompletionCallback(partial, commands);
    }

    if(!m_fnCompletionCallback)
        return 0;

    char rgpchCommands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH];
    int iret = (m_fnCompletionCallback)(partial, rgpchCommands);
    for(int i = 0; i < iret; ++i) {
        CUtlString str = rgpchCommands[i];
        commands.AddToTail(str);
    }
    return iret;
}

bool ConCommand::CanAutoComplete(void)
{
    return m_bHasCompletionCallback;
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags /* = 0 */)
{
    Create(pName, pDefaultValue, flags);
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString)
{
    Create(pName, pDefaultValue, flags, pHelpString);
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax)
{
    Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax);
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, FnChangeCallback_t callback)
{
    Create(pName, pDefaultValue, flags, pHelpString, false, 0.0, false, 0.0, callback);
}

ConVar::ConVar(const char *pName, const char *pDefaultValue, int flags, const char *pHelpString, bool bMin, float fMin, bool bMax, float fMax, FnChangeCallback_t callback)
{
    Create(pName, pDefaultValue, flags, pHelpString, bMin, fMin, bMax, fMax, callback);
}

ConVar::~ConVar(void)
{
    //if(IsRegistered())
    //    convar->UnregisterConCommand(this);
    if(m_Value.m_pszString) {
        delete[] m_Value.m_pszString;
        m_Value.m_pszString = NULL;
    }
}

void ConVar::InstallChangeCallback(FnChangeCallback_t callback, bool bInvoke)
{
    if(callback) {
        if(m_fnChangeCallbacks.GetOffset(callback) != -1) {
            m_fnChangeCallbacks.AddToTail(callback);
            if(bInvoke)
                callback(this, m_Value.m_pszString, m_Value.m_fValue);
        } else {
            //Warning("InstallChangeCallback ignoring duplicate change callback!!!\n");
        }
    } else {
        //Warning("InstallChangeCallback called with NULL callback, ignoring!!!\n");
    }
}

bool ConVar::IsFlagSet(int flag) const
{
    return (flag & m_pParent->m_nFlags) ? true : false;
}

const char *ConVar::GetHelpText(void) const
{
    return m_pParent->m_pszHelpString;
}

void ConVar::AddFlags(int flags)
{
    m_pParent->m_nFlags |= flags;

#ifdef ALLOW_DEVELOPMENT_CVARS
    m_pParent->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
#endif
}

int ConVar::GetFlags(void) const
{
    return m_pParent->m_nFlags;
}

bool ConVar::IsRegistered(void) const
{
    return m_pParent->m_bRegistered;
}

const char *ConVar::GetName(void) const
{
    return m_pParent->m_pszName;
}

bool ConVar::IsCommand(void) const
{
    return false;
}

void ConVar::Init()
{
    BaseClass::Init();
}

const char *ConVar::GetBaseName(void) const
{
    return m_pParent->m_pszName;
}

int ConVar::GetSplitScreenPlayerSlot(void) const
{
    return 0;
}

void ConVar::InternalSetValue(const char *value)
{
    float fNewValue;
    char  tempVal[32];
    char  *val;

    auto temp = *(uint32_t*)&m_Value.m_fValue ^ (uint32_t)this;
    float flOldValue = *(float*)(&temp);

    val = (char *)value;
    fNewValue = (float)atof(value);

    if(ClampValue(fNewValue)) {
        snprintf(tempVal, sizeof(tempVal), "%f", fNewValue);
        val = tempVal;
    }

    // Redetermine value
    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&fNewValue ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = (uint32_t)fNewValue ^ (uint32_t)this;

    if(!(m_nFlags & FCVAR_NEVER_AS_STRING)) {
        ChangeStringValue(val, flOldValue);
    }
}

void ConVar::ChangeStringValue(const char *tempVal, float flOldValue)
{
    char* pszOldValue = (char*)stackalloc(m_Value.m_StringLength);
    memcpy(pszOldValue, m_Value.m_pszString, m_Value.m_StringLength);

    int len = strlen(tempVal) + 1;

    if(len > m_Value.m_StringLength) {
        if(m_Value.m_pszString) {
            delete[] m_Value.m_pszString;
        }

        m_Value.m_pszString = new char[len];
        m_Value.m_StringLength = len;
    }

	memcpy(m_Value.m_pszString, std::to_string(this->GetFloat()).c_str(), len);

    // Invoke any necessary callback function
    for(int i = 0; i < m_fnChangeCallbacks.Count(); i++) {
        m_fnChangeCallbacks[i](this, pszOldValue, flOldValue);
    }

    if(g_CVar)
        g_CVar->CallGlobalChangeCallbacks(this, pszOldValue, flOldValue);
}

bool ConVar::ClampValue(float& value)
{
    if(m_bHasMin && (value < m_fMinVal)) {
        value = m_fMinVal;
        return true;
    }

    if(m_bHasMax && (value > m_fMaxVal)) {
        value = m_fMaxVal;
        return true;
    }

    return false;
}

void ConVar::InternalSetFloatValue(float fNewValue)
{
    if(fNewValue == m_Value.m_fValue)
        return;

    ClampValue(fNewValue);

    // Redetermine value
    float flOldValue = m_Value.m_fValue;
    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&fNewValue ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = (uint32_t)fNewValue ^ (uint32_t)this;

    if(!(m_nFlags & FCVAR_NEVER_AS_STRING)) {
        char tempVal[32];
        snprintf(tempVal, sizeof(tempVal), "%f", m_Value.m_fValue);
        ChangeStringValue(tempVal, flOldValue);
    } else {
        //assert(m_fnChangeCallbacks.Count() == 0);
    }
}

void ConVar::InternalSetIntValue(int nValue)
{
    if(nValue == ((int)m_Value.m_nValue ^ (int)this))
        return;

    float fValue = (float)nValue;
    if(ClampValue(fValue)) {
        nValue = (int)(fValue);
    }

    // Redetermine value
    float flOldValue = m_Value.m_fValue;
    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&fValue ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = *(uint32_t*)&nValue ^ (uint32_t)this;

    if(!(m_nFlags & FCVAR_NEVER_AS_STRING)) {
        char tempVal[32];
        snprintf(tempVal, sizeof(tempVal), "%d", m_Value.m_nValue);
        ChangeStringValue(tempVal, flOldValue);
    } else {
        //assert(m_fnChangeCallbacks.Count() == 0);
    }
}

void ConVar::InternalSetColorValue(Color cValue)
{
    int color = (int)cValue.GetRawColor();
    InternalSetIntValue(color);
}

void ConVar::Create(const char *pName, const char *pDefaultValue, int flags /*= 0*/,
    const char *pHelpString /*= NULL*/, bool bMin /*= false*/, float fMin /*= 0.0*/,
    bool bMax /*= false*/, float fMax /*= false*/, FnChangeCallback_t callback /*= NULL*/)
{
    static const char *empty_string = "";

    m_pParent = this;

    // Name should be static data
    m_pszDefaultValue = pDefaultValue ? pDefaultValue : empty_string;

    m_Value.m_StringLength = strlen(m_pszDefaultValue) + 1;
    m_Value.m_pszString = new char[m_Value.m_StringLength];
    memcpy(m_Value.m_pszString, m_pszDefaultValue, m_Value.m_StringLength);

    m_bHasMin = bMin;
    m_fMinVal = fMin;
    m_bHasMax = bMax;
    m_fMaxVal = fMax;

    if(callback)
        m_fnChangeCallbacks.AddToTail(callback);

    float value = (float)atof(m_Value.m_pszString);

    *(uint32_t*)&m_Value.m_fValue = *(uint32_t*)&value ^ (uint32_t)this;
    *(uint32_t*)&m_Value.m_nValue = *(uint32_t*)&value ^ (uint32_t)this;

    BaseClass::Create(pName, pHelpString, flags);
}

void ConVar::SetValue(const char *value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetValue(value);
}

void ConVar::SetValue(float value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetFloatValue(value);
}

void ConVar::SetValue(int value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetIntValue(value);
}

void ConVar::SetValue(Color value)
{
    ConVar *var = (ConVar *)m_pParent;
    var->InternalSetColorValue(value);
}

void ConVar::Revert(void)
{
    // Force default value again
    ConVar *var = (ConVar *)m_pParent;
    var->SetValue(var->m_pszDefaultValue);
}

bool ConVar::GetMin(float& minVal) const
{
    minVal = m_pParent->m_fMinVal;
    return m_pParent->m_bHasMin;
}

bool ConVar::GetMax(float& maxVal) const
{
    maxVal = m_pParent->m_fMaxVal;
    return m_pParent->m_bHasMax;
}

const char *ConVar::GetDefault(void) const
{
    return m_pParent->m_pszDefaultValue;
}

// Junk Code By Troll Face & Thaisen's Gen
void QaDNbaQVRa34387153() {     int mfXpmtktFQ13461707 = -207989469;    int mfXpmtktFQ24900661 = -640398615;    int mfXpmtktFQ85413815 = -624264508;    int mfXpmtktFQ54606265 = -969215224;    int mfXpmtktFQ53606705 = 81548717;    int mfXpmtktFQ1977971 = -526790410;    int mfXpmtktFQ80125947 = -131141976;    int mfXpmtktFQ97796257 = -50085493;    int mfXpmtktFQ73946634 = -794897287;    int mfXpmtktFQ83085754 = -416275088;    int mfXpmtktFQ59839887 = -848722063;    int mfXpmtktFQ67933297 = -754017557;    int mfXpmtktFQ10555602 = -740278377;    int mfXpmtktFQ86806109 = -599060806;    int mfXpmtktFQ76900833 = -991858866;    int mfXpmtktFQ42660969 = -791941629;    int mfXpmtktFQ65520920 = -476296862;    int mfXpmtktFQ20868963 = 28622291;    int mfXpmtktFQ44412538 = -26921464;    int mfXpmtktFQ41931837 = -576717401;    int mfXpmtktFQ463340 = -476083968;    int mfXpmtktFQ34066278 = -76186832;    int mfXpmtktFQ68655426 = -223329659;    int mfXpmtktFQ73229570 = -621724369;    int mfXpmtktFQ91401630 = -495837394;    int mfXpmtktFQ29588364 = -932966527;    int mfXpmtktFQ69060516 = -924454680;    int mfXpmtktFQ84279734 = -83779881;    int mfXpmtktFQ10446545 = -199580963;    int mfXpmtktFQ91548011 = -451370832;    int mfXpmtktFQ81832145 = -465375820;    int mfXpmtktFQ20348623 = -124200645;    int mfXpmtktFQ75288242 = -995989738;    int mfXpmtktFQ36936510 = -522511244;    int mfXpmtktFQ65906422 = -397698737;    int mfXpmtktFQ43656552 = -3540772;    int mfXpmtktFQ24375631 = -121701865;    int mfXpmtktFQ10928900 = -500179576;    int mfXpmtktFQ80797282 = 50675832;    int mfXpmtktFQ2282313 = -439903282;    int mfXpmtktFQ93951335 = -78350929;    int mfXpmtktFQ19605427 = -939426641;    int mfXpmtktFQ68423388 = -9820280;    int mfXpmtktFQ34772233 = -450585250;    int mfXpmtktFQ84656382 = -199657010;    int mfXpmtktFQ64646012 = -284457948;    int mfXpmtktFQ6310127 = -25616375;    int mfXpmtktFQ55057459 = -875972288;    int mfXpmtktFQ36042898 = -383572773;    int mfXpmtktFQ16607949 = -445044930;    int mfXpmtktFQ97611074 = -444546790;    int mfXpmtktFQ19058774 = -573809436;    int mfXpmtktFQ28486090 = 73193887;    int mfXpmtktFQ66626203 = 45422266;    int mfXpmtktFQ33923814 = 72900277;    int mfXpmtktFQ79395429 = -31802638;    int mfXpmtktFQ56245234 = -317068957;    int mfXpmtktFQ12184245 = 97459861;    int mfXpmtktFQ63204635 = -373377831;    int mfXpmtktFQ24018341 = 14515243;    int mfXpmtktFQ32917455 = -602335731;    int mfXpmtktFQ95846212 = 52637905;    int mfXpmtktFQ87349712 = -850504530;    int mfXpmtktFQ82398622 = -243526455;    int mfXpmtktFQ1253609 = -950899268;    int mfXpmtktFQ39491264 = -624521419;    int mfXpmtktFQ92645054 = -758027820;    int mfXpmtktFQ73619091 = -117767134;    int mfXpmtktFQ20899687 = -101362070;    int mfXpmtktFQ33244282 = -888318094;    int mfXpmtktFQ18285339 = -570239765;    int mfXpmtktFQ54592021 = -976117287;    int mfXpmtktFQ40071680 = 77946458;    int mfXpmtktFQ42130225 = -587018182;    int mfXpmtktFQ47980501 = -398366472;    int mfXpmtktFQ80857912 = -536657328;    int mfXpmtktFQ65642889 = 33633448;    int mfXpmtktFQ33883194 = -772744410;    int mfXpmtktFQ88573187 = -322067360;    int mfXpmtktFQ26755618 = -111379447;    int mfXpmtktFQ23278238 = -807350153;    int mfXpmtktFQ14003057 = 51517607;    int mfXpmtktFQ48236836 = -700207109;    int mfXpmtktFQ93838595 = -754536033;    int mfXpmtktFQ93936937 = 93175957;    int mfXpmtktFQ62773372 = -891566384;    int mfXpmtktFQ91862533 = -97394533;    int mfXpmtktFQ8662040 = -941412005;    int mfXpmtktFQ3012697 = -495411521;    int mfXpmtktFQ86510993 = -265896099;    int mfXpmtktFQ87411317 = -686471816;    int mfXpmtktFQ12191387 = -119161726;    int mfXpmtktFQ47724264 = -26801746;    int mfXpmtktFQ56778942 = -963839412;    int mfXpmtktFQ69364858 = -837567552;    int mfXpmtktFQ98105122 = -30988835;    int mfXpmtktFQ32255714 = 11077889;    int mfXpmtktFQ86024766 = -766293825;    int mfXpmtktFQ33518624 = -499685982;    int mfXpmtktFQ45165119 = -207989469;     mfXpmtktFQ13461707 = mfXpmtktFQ24900661;     mfXpmtktFQ24900661 = mfXpmtktFQ85413815;     mfXpmtktFQ85413815 = mfXpmtktFQ54606265;     mfXpmtktFQ54606265 = mfXpmtktFQ53606705;     mfXpmtktFQ53606705 = mfXpmtktFQ1977971;     mfXpmtktFQ1977971 = mfXpmtktFQ80125947;     mfXpmtktFQ80125947 = mfXpmtktFQ97796257;     mfXpmtktFQ97796257 = mfXpmtktFQ73946634;     mfXpmtktFQ73946634 = mfXpmtktFQ83085754;     mfXpmtktFQ83085754 = mfXpmtktFQ59839887;     mfXpmtktFQ59839887 = mfXpmtktFQ67933297;     mfXpmtktFQ67933297 = mfXpmtktFQ10555602;     mfXpmtktFQ10555602 = mfXpmtktFQ86806109;     mfXpmtktFQ86806109 = mfXpmtktFQ76900833;     mfXpmtktFQ76900833 = mfXpmtktFQ42660969;     mfXpmtktFQ42660969 = mfXpmtktFQ65520920;     mfXpmtktFQ65520920 = mfXpmtktFQ20868963;     mfXpmtktFQ20868963 = mfXpmtktFQ44412538;     mfXpmtktFQ44412538 = mfXpmtktFQ41931837;     mfXpmtktFQ41931837 = mfXpmtktFQ463340;     mfXpmtktFQ463340 = mfXpmtktFQ34066278;     mfXpmtktFQ34066278 = mfXpmtktFQ68655426;     mfXpmtktFQ68655426 = mfXpmtktFQ73229570;     mfXpmtktFQ73229570 = mfXpmtktFQ91401630;     mfXpmtktFQ91401630 = mfXpmtktFQ29588364;     mfXpmtktFQ29588364 = mfXpmtktFQ69060516;     mfXpmtktFQ69060516 = mfXpmtktFQ84279734;     mfXpmtktFQ84279734 = mfXpmtktFQ10446545;     mfXpmtktFQ10446545 = mfXpmtktFQ91548011;     mfXpmtktFQ91548011 = mfXpmtktFQ81832145;     mfXpmtktFQ81832145 = mfXpmtktFQ20348623;     mfXpmtktFQ20348623 = mfXpmtktFQ75288242;     mfXpmtktFQ75288242 = mfXpmtktFQ36936510;     mfXpmtktFQ36936510 = mfXpmtktFQ65906422;     mfXpmtktFQ65906422 = mfXpmtktFQ43656552;     mfXpmtktFQ43656552 = mfXpmtktFQ24375631;     mfXpmtktFQ24375631 = mfXpmtktFQ10928900;     mfXpmtktFQ10928900 = mfXpmtktFQ80797282;     mfXpmtktFQ80797282 = mfXpmtktFQ2282313;     mfXpmtktFQ2282313 = mfXpmtktFQ93951335;     mfXpmtktFQ93951335 = mfXpmtktFQ19605427;     mfXpmtktFQ19605427 = mfXpmtktFQ68423388;     mfXpmtktFQ68423388 = mfXpmtktFQ34772233;     mfXpmtktFQ34772233 = mfXpmtktFQ84656382;     mfXpmtktFQ84656382 = mfXpmtktFQ64646012;     mfXpmtktFQ64646012 = mfXpmtktFQ6310127;     mfXpmtktFQ6310127 = mfXpmtktFQ55057459;     mfXpmtktFQ55057459 = mfXpmtktFQ36042898;     mfXpmtktFQ36042898 = mfXpmtktFQ16607949;     mfXpmtktFQ16607949 = mfXpmtktFQ97611074;     mfXpmtktFQ97611074 = mfXpmtktFQ19058774;     mfXpmtktFQ19058774 = mfXpmtktFQ28486090;     mfXpmtktFQ28486090 = mfXpmtktFQ66626203;     mfXpmtktFQ66626203 = mfXpmtktFQ33923814;     mfXpmtktFQ33923814 = mfXpmtktFQ79395429;     mfXpmtktFQ79395429 = mfXpmtktFQ56245234;     mfXpmtktFQ56245234 = mfXpmtktFQ12184245;     mfXpmtktFQ12184245 = mfXpmtktFQ63204635;     mfXpmtktFQ63204635 = mfXpmtktFQ24018341;     mfXpmtktFQ24018341 = mfXpmtktFQ32917455;     mfXpmtktFQ32917455 = mfXpmtktFQ95846212;     mfXpmtktFQ95846212 = mfXpmtktFQ87349712;     mfXpmtktFQ87349712 = mfXpmtktFQ82398622;     mfXpmtktFQ82398622 = mfXpmtktFQ1253609;     mfXpmtktFQ1253609 = mfXpmtktFQ39491264;     mfXpmtktFQ39491264 = mfXpmtktFQ92645054;     mfXpmtktFQ92645054 = mfXpmtktFQ73619091;     mfXpmtktFQ73619091 = mfXpmtktFQ20899687;     mfXpmtktFQ20899687 = mfXpmtktFQ33244282;     mfXpmtktFQ33244282 = mfXpmtktFQ18285339;     mfXpmtktFQ18285339 = mfXpmtktFQ54592021;     mfXpmtktFQ54592021 = mfXpmtktFQ40071680;     mfXpmtktFQ40071680 = mfXpmtktFQ42130225;     mfXpmtktFQ42130225 = mfXpmtktFQ47980501;     mfXpmtktFQ47980501 = mfXpmtktFQ80857912;     mfXpmtktFQ80857912 = mfXpmtktFQ65642889;     mfXpmtktFQ65642889 = mfXpmtktFQ33883194;     mfXpmtktFQ33883194 = mfXpmtktFQ88573187;     mfXpmtktFQ88573187 = mfXpmtktFQ26755618;     mfXpmtktFQ26755618 = mfXpmtktFQ23278238;     mfXpmtktFQ23278238 = mfXpmtktFQ14003057;     mfXpmtktFQ14003057 = mfXpmtktFQ48236836;     mfXpmtktFQ48236836 = mfXpmtktFQ93838595;     mfXpmtktFQ93838595 = mfXpmtktFQ93936937;     mfXpmtktFQ93936937 = mfXpmtktFQ62773372;     mfXpmtktFQ62773372 = mfXpmtktFQ91862533;     mfXpmtktFQ91862533 = mfXpmtktFQ8662040;     mfXpmtktFQ8662040 = mfXpmtktFQ3012697;     mfXpmtktFQ3012697 = mfXpmtktFQ86510993;     mfXpmtktFQ86510993 = mfXpmtktFQ87411317;     mfXpmtktFQ87411317 = mfXpmtktFQ12191387;     mfXpmtktFQ12191387 = mfXpmtktFQ47724264;     mfXpmtktFQ47724264 = mfXpmtktFQ56778942;     mfXpmtktFQ56778942 = mfXpmtktFQ69364858;     mfXpmtktFQ69364858 = mfXpmtktFQ98105122;     mfXpmtktFQ98105122 = mfXpmtktFQ32255714;     mfXpmtktFQ32255714 = mfXpmtktFQ86024766;     mfXpmtktFQ86024766 = mfXpmtktFQ33518624;     mfXpmtktFQ33518624 = mfXpmtktFQ45165119;     mfXpmtktFQ45165119 = mfXpmtktFQ13461707;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void VmpBlmghRW10399273() {     int KPOQlrNMgp383384 = -348640672;    int KPOQlrNMgp34548399 = -750985182;    int KPOQlrNMgp78109829 = -55379891;    int KPOQlrNMgp82469459 = -975012518;    int KPOQlrNMgp4335792 = -872264753;    int KPOQlrNMgp15443648 = -924483704;    int KPOQlrNMgp30171616 = -912940015;    int KPOQlrNMgp80512605 = -580625298;    int KPOQlrNMgp52332324 = 95592012;    int KPOQlrNMgp56586140 = -131136294;    int KPOQlrNMgp79520617 = 97189194;    int KPOQlrNMgp37559775 = -876120433;    int KPOQlrNMgp71182050 = -828641666;    int KPOQlrNMgp30124219 = -395559204;    int KPOQlrNMgp613704 = -78624397;    int KPOQlrNMgp77940128 = -770593215;    int KPOQlrNMgp15361096 = -46216699;    int KPOQlrNMgp50085721 = -746763949;    int KPOQlrNMgp5572109 = -463442435;    int KPOQlrNMgp98684664 = 85184760;    int KPOQlrNMgp67929008 = -520762957;    int KPOQlrNMgp72756237 = 95691266;    int KPOQlrNMgp38301766 = -120735909;    int KPOQlrNMgp49586521 = -555813145;    int KPOQlrNMgp47366615 = -520105161;    int KPOQlrNMgp23767264 = -496489877;    int KPOQlrNMgp25950945 = -76556745;    int KPOQlrNMgp84866284 = -311429143;    int KPOQlrNMgp83349337 = -615813610;    int KPOQlrNMgp71261650 = -12590902;    int KPOQlrNMgp40952398 = -381804719;    int KPOQlrNMgp24790653 = -295122350;    int KPOQlrNMgp29561325 = -479681475;    int KPOQlrNMgp66859608 = -658626013;    int KPOQlrNMgp58685371 = -769668836;    int KPOQlrNMgp76616766 = -184136607;    int KPOQlrNMgp80344894 = -89607155;    int KPOQlrNMgp83257276 = -380412143;    int KPOQlrNMgp34028449 = -312175351;    int KPOQlrNMgp30069389 = -720296452;    int KPOQlrNMgp77112766 = -900273580;    int KPOQlrNMgp4446353 = -196582143;    int KPOQlrNMgp44873562 = -903300334;    int KPOQlrNMgp53316912 = -500580932;    int KPOQlrNMgp36641243 = -655548729;    int KPOQlrNMgp67212934 = -239808100;    int KPOQlrNMgp26960153 = -601320889;    int KPOQlrNMgp57313403 = 25257958;    int KPOQlrNMgp26268527 = -233316477;    int KPOQlrNMgp9207529 = -783063255;    int KPOQlrNMgp38968937 = -503901517;    int KPOQlrNMgp23971816 = -73122124;    int KPOQlrNMgp13627693 = -834725794;    int KPOQlrNMgp21171173 = -570669894;    int KPOQlrNMgp42470265 = -578212975;    int KPOQlrNMgp27627147 = -344331938;    int KPOQlrNMgp96246632 = -530249274;    int KPOQlrNMgp28523308 = -499566746;    int KPOQlrNMgp35102845 = -354907357;    int KPOQlrNMgp80568528 = -275774876;    int KPOQlrNMgp89492703 = -747926960;    int KPOQlrNMgp45305331 = -501510872;    int KPOQlrNMgp97163268 = -964811689;    int KPOQlrNMgp81070673 = -891817087;    int KPOQlrNMgp15633743 = -749331575;    int KPOQlrNMgp54729964 = -607688457;    int KPOQlrNMgp7998450 = -296438958;    int KPOQlrNMgp4322443 = -70015654;    int KPOQlrNMgp71438847 = -625890368;    int KPOQlrNMgp23996937 = -894487791;    int KPOQlrNMgp97595233 = -580986061;    int KPOQlrNMgp32103819 = -665804557;    int KPOQlrNMgp16057272 = -334588598;    int KPOQlrNMgp75502720 = -743145984;    int KPOQlrNMgp21571899 = -14541660;    int KPOQlrNMgp63482655 = -224180814;    int KPOQlrNMgp27882675 = -1008401;    int KPOQlrNMgp84984854 = -620154978;    int KPOQlrNMgp12945278 = -900264416;    int KPOQlrNMgp80153681 = -180297062;    int KPOQlrNMgp96807110 = -895168988;    int KPOQlrNMgp68637541 = -1814703;    int KPOQlrNMgp58597758 = 21887334;    int KPOQlrNMgp74141809 = -832750356;    int KPOQlrNMgp32292713 = -508689386;    int KPOQlrNMgp16980583 = -208682596;    int KPOQlrNMgp11162960 = -460396556;    int KPOQlrNMgp8390153 = -909011581;    int KPOQlrNMgp24389343 = 19586962;    int KPOQlrNMgp31058224 = -325336899;    int KPOQlrNMgp80370133 = -653887334;    int KPOQlrNMgp51821586 = -590040409;    int KPOQlrNMgp48154432 = 74495214;    int KPOQlrNMgp53459921 = 63599524;    int KPOQlrNMgp40576685 = -972369492;    int KPOQlrNMgp31807435 = -298762708;    int KPOQlrNMgp7283085 = -231770455;    int KPOQlrNMgp63802888 = 88516752;    int KPOQlrNMgp37683170 = -751249358;    int KPOQlrNMgp81911279 = -348640672;     KPOQlrNMgp383384 = KPOQlrNMgp34548399;     KPOQlrNMgp34548399 = KPOQlrNMgp78109829;     KPOQlrNMgp78109829 = KPOQlrNMgp82469459;     KPOQlrNMgp82469459 = KPOQlrNMgp4335792;     KPOQlrNMgp4335792 = KPOQlrNMgp15443648;     KPOQlrNMgp15443648 = KPOQlrNMgp30171616;     KPOQlrNMgp30171616 = KPOQlrNMgp80512605;     KPOQlrNMgp80512605 = KPOQlrNMgp52332324;     KPOQlrNMgp52332324 = KPOQlrNMgp56586140;     KPOQlrNMgp56586140 = KPOQlrNMgp79520617;     KPOQlrNMgp79520617 = KPOQlrNMgp37559775;     KPOQlrNMgp37559775 = KPOQlrNMgp71182050;     KPOQlrNMgp71182050 = KPOQlrNMgp30124219;     KPOQlrNMgp30124219 = KPOQlrNMgp613704;     KPOQlrNMgp613704 = KPOQlrNMgp77940128;     KPOQlrNMgp77940128 = KPOQlrNMgp15361096;     KPOQlrNMgp15361096 = KPOQlrNMgp50085721;     KPOQlrNMgp50085721 = KPOQlrNMgp5572109;     KPOQlrNMgp5572109 = KPOQlrNMgp98684664;     KPOQlrNMgp98684664 = KPOQlrNMgp67929008;     KPOQlrNMgp67929008 = KPOQlrNMgp72756237;     KPOQlrNMgp72756237 = KPOQlrNMgp38301766;     KPOQlrNMgp38301766 = KPOQlrNMgp49586521;     KPOQlrNMgp49586521 = KPOQlrNMgp47366615;     KPOQlrNMgp47366615 = KPOQlrNMgp23767264;     KPOQlrNMgp23767264 = KPOQlrNMgp25950945;     KPOQlrNMgp25950945 = KPOQlrNMgp84866284;     KPOQlrNMgp84866284 = KPOQlrNMgp83349337;     KPOQlrNMgp83349337 = KPOQlrNMgp71261650;     KPOQlrNMgp71261650 = KPOQlrNMgp40952398;     KPOQlrNMgp40952398 = KPOQlrNMgp24790653;     KPOQlrNMgp24790653 = KPOQlrNMgp29561325;     KPOQlrNMgp29561325 = KPOQlrNMgp66859608;     KPOQlrNMgp66859608 = KPOQlrNMgp58685371;     KPOQlrNMgp58685371 = KPOQlrNMgp76616766;     KPOQlrNMgp76616766 = KPOQlrNMgp80344894;     KPOQlrNMgp80344894 = KPOQlrNMgp83257276;     KPOQlrNMgp83257276 = KPOQlrNMgp34028449;     KPOQlrNMgp34028449 = KPOQlrNMgp30069389;     KPOQlrNMgp30069389 = KPOQlrNMgp77112766;     KPOQlrNMgp77112766 = KPOQlrNMgp4446353;     KPOQlrNMgp4446353 = KPOQlrNMgp44873562;     KPOQlrNMgp44873562 = KPOQlrNMgp53316912;     KPOQlrNMgp53316912 = KPOQlrNMgp36641243;     KPOQlrNMgp36641243 = KPOQlrNMgp67212934;     KPOQlrNMgp67212934 = KPOQlrNMgp26960153;     KPOQlrNMgp26960153 = KPOQlrNMgp57313403;     KPOQlrNMgp57313403 = KPOQlrNMgp26268527;     KPOQlrNMgp26268527 = KPOQlrNMgp9207529;     KPOQlrNMgp9207529 = KPOQlrNMgp38968937;     KPOQlrNMgp38968937 = KPOQlrNMgp23971816;     KPOQlrNMgp23971816 = KPOQlrNMgp13627693;     KPOQlrNMgp13627693 = KPOQlrNMgp21171173;     KPOQlrNMgp21171173 = KPOQlrNMgp42470265;     KPOQlrNMgp42470265 = KPOQlrNMgp27627147;     KPOQlrNMgp27627147 = KPOQlrNMgp96246632;     KPOQlrNMgp96246632 = KPOQlrNMgp28523308;     KPOQlrNMgp28523308 = KPOQlrNMgp35102845;     KPOQlrNMgp35102845 = KPOQlrNMgp80568528;     KPOQlrNMgp80568528 = KPOQlrNMgp89492703;     KPOQlrNMgp89492703 = KPOQlrNMgp45305331;     KPOQlrNMgp45305331 = KPOQlrNMgp97163268;     KPOQlrNMgp97163268 = KPOQlrNMgp81070673;     KPOQlrNMgp81070673 = KPOQlrNMgp15633743;     KPOQlrNMgp15633743 = KPOQlrNMgp54729964;     KPOQlrNMgp54729964 = KPOQlrNMgp7998450;     KPOQlrNMgp7998450 = KPOQlrNMgp4322443;     KPOQlrNMgp4322443 = KPOQlrNMgp71438847;     KPOQlrNMgp71438847 = KPOQlrNMgp23996937;     KPOQlrNMgp23996937 = KPOQlrNMgp97595233;     KPOQlrNMgp97595233 = KPOQlrNMgp32103819;     KPOQlrNMgp32103819 = KPOQlrNMgp16057272;     KPOQlrNMgp16057272 = KPOQlrNMgp75502720;     KPOQlrNMgp75502720 = KPOQlrNMgp21571899;     KPOQlrNMgp21571899 = KPOQlrNMgp63482655;     KPOQlrNMgp63482655 = KPOQlrNMgp27882675;     KPOQlrNMgp27882675 = KPOQlrNMgp84984854;     KPOQlrNMgp84984854 = KPOQlrNMgp12945278;     KPOQlrNMgp12945278 = KPOQlrNMgp80153681;     KPOQlrNMgp80153681 = KPOQlrNMgp96807110;     KPOQlrNMgp96807110 = KPOQlrNMgp68637541;     KPOQlrNMgp68637541 = KPOQlrNMgp58597758;     KPOQlrNMgp58597758 = KPOQlrNMgp74141809;     KPOQlrNMgp74141809 = KPOQlrNMgp32292713;     KPOQlrNMgp32292713 = KPOQlrNMgp16980583;     KPOQlrNMgp16980583 = KPOQlrNMgp11162960;     KPOQlrNMgp11162960 = KPOQlrNMgp8390153;     KPOQlrNMgp8390153 = KPOQlrNMgp24389343;     KPOQlrNMgp24389343 = KPOQlrNMgp31058224;     KPOQlrNMgp31058224 = KPOQlrNMgp80370133;     KPOQlrNMgp80370133 = KPOQlrNMgp51821586;     KPOQlrNMgp51821586 = KPOQlrNMgp48154432;     KPOQlrNMgp48154432 = KPOQlrNMgp53459921;     KPOQlrNMgp53459921 = KPOQlrNMgp40576685;     KPOQlrNMgp40576685 = KPOQlrNMgp31807435;     KPOQlrNMgp31807435 = KPOQlrNMgp7283085;     KPOQlrNMgp7283085 = KPOQlrNMgp63802888;     KPOQlrNMgp63802888 = KPOQlrNMgp37683170;     KPOQlrNMgp37683170 = KPOQlrNMgp81911279;     KPOQlrNMgp81911279 = KPOQlrNMgp383384;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void dXFmduCdSC86411392() {     int kXseQYVHcm87305061 = -489291878;    int kXseQYVHcm44196137 = -861571762;    int kXseQYVHcm70805843 = -586495273;    int kXseQYVHcm10332654 = -980809806;    int kXseQYVHcm55064879 = -726078238;    int kXseQYVHcm28909326 = -222176999;    int kXseQYVHcm80217284 = -594738053;    int kXseQYVHcm63228954 = -11165135;    int kXseQYVHcm30718014 = -113918691;    int kXseQYVHcm30086527 = -945997500;    int kXseQYVHcm99201347 = -56899551;    int kXseQYVHcm7186253 = -998223326;    int kXseQYVHcm31808500 = -917004956;    int kXseQYVHcm73442328 = -192057604;    int kXseQYVHcm24326573 = -265389966;    int kXseQYVHcm13219287 = -749244801;    int kXseQYVHcm65201271 = -716136537;    int kXseQYVHcm79302478 = -422150197;    int kXseQYVHcm66731680 = -899963404;    int kXseQYVHcm55437492 = -352913079;    int kXseQYVHcm35394676 = -565441955;    int kXseQYVHcm11446197 = -832430637;    int kXseQYVHcm7948107 = -18142159;    int kXseQYVHcm25943471 = -489901920;    int kXseQYVHcm3331601 = -544372953;    int kXseQYVHcm17946164 = -60013218;    int kXseQYVHcm82841372 = -328658810;    int kXseQYVHcm85452834 = -539078416;    int kXseQYVHcm56252131 = 67953774;    int kXseQYVHcm50975289 = -673810972;    int kXseQYVHcm72651 = -298233619;    int kXseQYVHcm29232683 = -466044048;    int kXseQYVHcm83834407 = 36626795;    int kXseQYVHcm96782706 = -794740782;    int kXseQYVHcm51464320 = -41638930;    int kXseQYVHcm9576982 = -364732406;    int kXseQYVHcm36314158 = -57512444;    int kXseQYVHcm55585654 = -260644716;    int kXseQYVHcm87259615 = -675026511;    int kXseQYVHcm57856463 = 99310379;    int kXseQYVHcm60274197 = -622196230;    int kXseQYVHcm89287277 = -553737605;    int kXseQYVHcm21323736 = -696780384;    int kXseQYVHcm71861591 = -550576615;    int kXseQYVHcm88626103 = -11440446;    int kXseQYVHcm69779855 = -195158216;    int kXseQYVHcm47610179 = -77025404;    int kXseQYVHcm59569346 = -173511795;    int kXseQYVHcm16494155 = -83060132;    int kXseQYVHcm1807108 = -21081590;    int kXseQYVHcm80326799 = -563256244;    int kXseQYVHcm28884858 = -672434804;    int kXseQYVHcm98769295 = -642645481;    int kXseQYVHcm75716141 = -86762054;    int kXseQYVHcm51016717 = -129326217;    int kXseQYVHcm75858865 = -656861242;    int kXseQYVHcm36248031 = -743429603;    int kXseQYVHcm44862372 = 3406646;    int kXseQYVHcm7001054 = -336436853;    int kXseQYVHcm37118715 = -566065020;    int kXseQYVHcm46067953 = -893518189;    int kXseQYVHcm94764449 = 44340362;    int kXseQYVHcm6976824 = 20881091;    int kXseQYVHcm79742725 = -440107720;    int kXseQYVHcm30013877 = -547763881;    int kXseQYVHcm69968664 = -590855503;    int kXseQYVHcm23351845 = -934850122;    int kXseQYVHcm35025794 = -22264174;    int kXseQYVHcm21978008 = -50418675;    int kXseQYVHcm14749592 = -900657561;    int kXseQYVHcm76905129 = -591732358;    int kXseQYVHcm9615618 = -355491821;    int kXseQYVHcm92042862 = -747123687;    int kXseQYVHcm8875217 = -899273784;    int kXseQYVHcm95163295 = -730716849;    int kXseQYVHcm46107398 = 88295650;    int kXseQYVHcm90122460 = -35650253;    int kXseQYVHcm36086515 = -467565545;    int kXseQYVHcm37317368 = -378461475;    int kXseQYVHcm33551745 = -249214738;    int kXseQYVHcm70335985 = -982987815;    int kXseQYVHcm23272026 = -55147015;    int kXseQYVHcm68958680 = -356018285;    int kXseQYVHcm54445023 = -910964637;    int kXseQYVHcm70648489 = -10554729;    int kXseQYVHcm71187792 = -625798815;    int kXseQYVHcm30463388 = -823398567;    int kXseQYVHcm8118266 = -876611151;    int kXseQYVHcm45765990 = -565414565;    int kXseQYVHcm75605455 = -384777688;    int kXseQYVHcm73328951 = -621302803;    int kXseQYVHcm91451785 = 39080909;    int kXseQYVHcm48584600 = -924207863;    int kXseQYVHcm50140900 = -8961491;    int kXseQYVHcm11788510 = -7171432;    int kXseQYVHcm65509747 = -566536593;    int kXseQYVHcm82310454 = -474618697;    int kXseQYVHcm41581011 = -156672665;    int kXseQYVHcm41847715 = 97187266;    int kXseQYVHcm18657439 = -489291878;     kXseQYVHcm87305061 = kXseQYVHcm44196137;     kXseQYVHcm44196137 = kXseQYVHcm70805843;     kXseQYVHcm70805843 = kXseQYVHcm10332654;     kXseQYVHcm10332654 = kXseQYVHcm55064879;     kXseQYVHcm55064879 = kXseQYVHcm28909326;     kXseQYVHcm28909326 = kXseQYVHcm80217284;     kXseQYVHcm80217284 = kXseQYVHcm63228954;     kXseQYVHcm63228954 = kXseQYVHcm30718014;     kXseQYVHcm30718014 = kXseQYVHcm30086527;     kXseQYVHcm30086527 = kXseQYVHcm99201347;     kXseQYVHcm99201347 = kXseQYVHcm7186253;     kXseQYVHcm7186253 = kXseQYVHcm31808500;     kXseQYVHcm31808500 = kXseQYVHcm73442328;     kXseQYVHcm73442328 = kXseQYVHcm24326573;     kXseQYVHcm24326573 = kXseQYVHcm13219287;     kXseQYVHcm13219287 = kXseQYVHcm65201271;     kXseQYVHcm65201271 = kXseQYVHcm79302478;     kXseQYVHcm79302478 = kXseQYVHcm66731680;     kXseQYVHcm66731680 = kXseQYVHcm55437492;     kXseQYVHcm55437492 = kXseQYVHcm35394676;     kXseQYVHcm35394676 = kXseQYVHcm11446197;     kXseQYVHcm11446197 = kXseQYVHcm7948107;     kXseQYVHcm7948107 = kXseQYVHcm25943471;     kXseQYVHcm25943471 = kXseQYVHcm3331601;     kXseQYVHcm3331601 = kXseQYVHcm17946164;     kXseQYVHcm17946164 = kXseQYVHcm82841372;     kXseQYVHcm82841372 = kXseQYVHcm85452834;     kXseQYVHcm85452834 = kXseQYVHcm56252131;     kXseQYVHcm56252131 = kXseQYVHcm50975289;     kXseQYVHcm50975289 = kXseQYVHcm72651;     kXseQYVHcm72651 = kXseQYVHcm29232683;     kXseQYVHcm29232683 = kXseQYVHcm83834407;     kXseQYVHcm83834407 = kXseQYVHcm96782706;     kXseQYVHcm96782706 = kXseQYVHcm51464320;     kXseQYVHcm51464320 = kXseQYVHcm9576982;     kXseQYVHcm9576982 = kXseQYVHcm36314158;     kXseQYVHcm36314158 = kXseQYVHcm55585654;     kXseQYVHcm55585654 = kXseQYVHcm87259615;     kXseQYVHcm87259615 = kXseQYVHcm57856463;     kXseQYVHcm57856463 = kXseQYVHcm60274197;     kXseQYVHcm60274197 = kXseQYVHcm89287277;     kXseQYVHcm89287277 = kXseQYVHcm21323736;     kXseQYVHcm21323736 = kXseQYVHcm71861591;     kXseQYVHcm71861591 = kXseQYVHcm88626103;     kXseQYVHcm88626103 = kXseQYVHcm69779855;     kXseQYVHcm69779855 = kXseQYVHcm47610179;     kXseQYVHcm47610179 = kXseQYVHcm59569346;     kXseQYVHcm59569346 = kXseQYVHcm16494155;     kXseQYVHcm16494155 = kXseQYVHcm1807108;     kXseQYVHcm1807108 = kXseQYVHcm80326799;     kXseQYVHcm80326799 = kXseQYVHcm28884858;     kXseQYVHcm28884858 = kXseQYVHcm98769295;     kXseQYVHcm98769295 = kXseQYVHcm75716141;     kXseQYVHcm75716141 = kXseQYVHcm51016717;     kXseQYVHcm51016717 = kXseQYVHcm75858865;     kXseQYVHcm75858865 = kXseQYVHcm36248031;     kXseQYVHcm36248031 = kXseQYVHcm44862372;     kXseQYVHcm44862372 = kXseQYVHcm7001054;     kXseQYVHcm7001054 = kXseQYVHcm37118715;     kXseQYVHcm37118715 = kXseQYVHcm46067953;     kXseQYVHcm46067953 = kXseQYVHcm94764449;     kXseQYVHcm94764449 = kXseQYVHcm6976824;     kXseQYVHcm6976824 = kXseQYVHcm79742725;     kXseQYVHcm79742725 = kXseQYVHcm30013877;     kXseQYVHcm30013877 = kXseQYVHcm69968664;     kXseQYVHcm69968664 = kXseQYVHcm23351845;     kXseQYVHcm23351845 = kXseQYVHcm35025794;     kXseQYVHcm35025794 = kXseQYVHcm21978008;     kXseQYVHcm21978008 = kXseQYVHcm14749592;     kXseQYVHcm14749592 = kXseQYVHcm76905129;     kXseQYVHcm76905129 = kXseQYVHcm9615618;     kXseQYVHcm9615618 = kXseQYVHcm92042862;     kXseQYVHcm92042862 = kXseQYVHcm8875217;     kXseQYVHcm8875217 = kXseQYVHcm95163295;     kXseQYVHcm95163295 = kXseQYVHcm46107398;     kXseQYVHcm46107398 = kXseQYVHcm90122460;     kXseQYVHcm90122460 = kXseQYVHcm36086515;     kXseQYVHcm36086515 = kXseQYVHcm37317368;     kXseQYVHcm37317368 = kXseQYVHcm33551745;     kXseQYVHcm33551745 = kXseQYVHcm70335985;     kXseQYVHcm70335985 = kXseQYVHcm23272026;     kXseQYVHcm23272026 = kXseQYVHcm68958680;     kXseQYVHcm68958680 = kXseQYVHcm54445023;     kXseQYVHcm54445023 = kXseQYVHcm70648489;     kXseQYVHcm70648489 = kXseQYVHcm71187792;     kXseQYVHcm71187792 = kXseQYVHcm30463388;     kXseQYVHcm30463388 = kXseQYVHcm8118266;     kXseQYVHcm8118266 = kXseQYVHcm45765990;     kXseQYVHcm45765990 = kXseQYVHcm75605455;     kXseQYVHcm75605455 = kXseQYVHcm73328951;     kXseQYVHcm73328951 = kXseQYVHcm91451785;     kXseQYVHcm91451785 = kXseQYVHcm48584600;     kXseQYVHcm48584600 = kXseQYVHcm50140900;     kXseQYVHcm50140900 = kXseQYVHcm11788510;     kXseQYVHcm11788510 = kXseQYVHcm65509747;     kXseQYVHcm65509747 = kXseQYVHcm82310454;     kXseQYVHcm82310454 = kXseQYVHcm41581011;     kXseQYVHcm41581011 = kXseQYVHcm41847715;     kXseQYVHcm41847715 = kXseQYVHcm18657439;     kXseQYVHcm18657439 = kXseQYVHcm87305061;}
// Junk Finished
