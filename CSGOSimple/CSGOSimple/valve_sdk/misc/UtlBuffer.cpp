//===== Copyright � 1996-2005, Valve Corporation, All rights reserved. ======//
//
// $Header: $
// $NoKeywords: $
//
// Serialization buffer
//===========================================================================//

#pragma warning (disable : 4514)

#include "UtlBuffer.hpp"
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "characterset.hpp"

const char* V_strnchr(const char* pStr, char c, int n)
{
    char const* pLetter = pStr;
    char const* pLast = pStr + n;

    // Check the entire string
    while((pLetter < pLast) && (*pLetter != 0)) {
        if(*pLetter == c)
            return pLetter;
        ++pLetter;
    }
    return NULL;
}
//-----------------------------------------------------------------------------
// Finds a string in another string with a case insensitive test w/ length validation
//-----------------------------------------------------------------------------
char const* V_strnistr(char const* pStr, char const* pSearch, int n)
{
    if(!pStr || !pSearch)
        return 0;

    char const* pLetter = pStr;

    // Check the entire string
    while(*pLetter != 0) {
        if(n <= 0)
            return 0;

        // Skip over non-matches
        if(tolower(*pLetter) == tolower(*pSearch)) {
            int n1 = n - 1;

            // Check for match
            char const* pMatch = pLetter + 1;
            char const* pTest = pSearch + 1;
            while(*pTest != 0) {
                if(n1 <= 0)
                    return 0;

                // We've run off the end; don't bother.
                if(*pMatch == 0)
                    return 0;

                if(tolower(*pMatch) != tolower(*pTest))
                    break;

                ++pMatch;
                ++pTest;
                --n1;
            }

            // Found a match!
            if(*pTest == 0)
                return pLetter;
        }

        ++pLetter;
        --n;
    }

    return 0;
}
//-----------------------------------------------------------------------------
// Character conversions for C strings
//-----------------------------------------------------------------------------
class CUtlCStringConversion : public CUtlCharConversion
{
public:
    CUtlCStringConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray);

    // Finds a conversion for the passed-in string, returns length
    virtual char FindConversion(const char *pString, int *pLength);

private:
    char m_pConversion[255];
};


//-----------------------------------------------------------------------------
// Character conversions for no-escape sequence strings
//-----------------------------------------------------------------------------
class CUtlNoEscConversion : public CUtlCharConversion
{
public:
    CUtlNoEscConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray) :
        CUtlCharConversion(nEscapeChar, pDelimiter, nCount, pArray)
    {
    }

    // Finds a conversion for the passed-in string, returns length
    virtual char FindConversion(const char *pString, int *pLength) { *pLength = 0; return 0; }
};


//-----------------------------------------------------------------------------
// List of character conversions
//-----------------------------------------------------------------------------
BEGIN_CUSTOM_CHAR_CONVERSION(CUtlCStringConversion, s_StringCharConversion, "\"", '\\')
{
    '\n', "n"
},
{ '\t', "t" },
{ '\v', "v" },
{ '\b', "b" },
{ '\r', "r" },
{ '\f', "f" },
{ '\a', "a" },
{ '\\', "\\" },
{ '\?', "\?" },
{ '\'', "\'" },
{ '\"', "\"" },
END_CUSTOM_CHAR_CONVERSION(CUtlCStringConversion, s_StringCharConversion, "\"", '\\');

    CUtlCharConversion *GetCStringCharConversion()
    {
        return &s_StringCharConversion;
    }

    BEGIN_CUSTOM_CHAR_CONVERSION(CUtlNoEscConversion, s_NoEscConversion, "\"", 0x7F)
    {
        0x7F, ""
    },
        END_CUSTOM_CHAR_CONVERSION(CUtlNoEscConversion, s_NoEscConversion, "\"", 0x7F);

        CUtlCharConversion *GetNoEscCharConversion()
        {
            return &s_NoEscConversion;
        }


        //-----------------------------------------------------------------------------
        // Constructor
        //-----------------------------------------------------------------------------
        CUtlCStringConversion::CUtlCStringConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray) :
            CUtlCharConversion(nEscapeChar, pDelimiter, nCount, pArray)
        {
            memset(m_pConversion, 0x0, sizeof(m_pConversion));
            for(int i = 0; i < nCount; ++i) {
                m_pConversion[pArray[i].m_pReplacementString[0]] = pArray[i].m_nActualChar;
            }
        }

        // Finds a conversion for the passed-in string, returns length
        char CUtlCStringConversion::FindConversion(const char *pString, int *pLength)
        {
            char c = m_pConversion[pString[0]];
            *pLength = (c != '\0') ? 1 : 0;
            return c;
        }



        //-----------------------------------------------------------------------------
        // Constructor
        //-----------------------------------------------------------------------------
        CUtlCharConversion::CUtlCharConversion(char nEscapeChar, const char *pDelimiter, int nCount, ConversionArray_t *pArray)
        {
            m_nEscapeChar = nEscapeChar;
            m_pDelimiter = pDelimiter;
            m_nCount = nCount;
            m_nDelimiterLength = strlen(pDelimiter);
            m_nMaxConversionLength = 0;

            memset(m_pReplacements, 0, sizeof(m_pReplacements));

            for(int i = 0; i < nCount; ++i) {
                m_pList[i] = pArray[i].m_nActualChar;
                ConversionInfo_t &info = m_pReplacements[m_pList[i]];
                assert(info.m_pReplacementString == 0);
                info.m_pReplacementString = pArray[i].m_pReplacementString;
                info.m_nLength = strlen(info.m_pReplacementString);
                if(info.m_nLength > m_nMaxConversionLength) {
                    m_nMaxConversionLength = info.m_nLength;
                }
            }
        }


        //-----------------------------------------------------------------------------
        // Escape character + delimiter
        //-----------------------------------------------------------------------------
        char CUtlCharConversion::GetEscapeChar() const
        {
            return m_nEscapeChar;
        }

        const char *CUtlCharConversion::GetDelimiter() const
        {
            return m_pDelimiter;
        }

        int CUtlCharConversion::GetDelimiterLength() const
        {
            return m_nDelimiterLength;
        }


        //-----------------------------------------------------------------------------
        // Constructor
        //-----------------------------------------------------------------------------
        const char *CUtlCharConversion::GetConversionString(char c) const
        {
            return m_pReplacements[c].m_pReplacementString;
        }

        int CUtlCharConversion::GetConversionLength(char c) const
        {
            return m_pReplacements[c].m_nLength;
        }

        int CUtlCharConversion::MaxConversionLength() const
        {
            return m_nMaxConversionLength;
        }


        //-----------------------------------------------------------------------------
        // Finds a conversion for the passed-in string, returns length
        //-----------------------------------------------------------------------------
        char CUtlCharConversion::FindConversion(const char *pString, int *pLength)
        {
            for(int i = 0; i < m_nCount; ++i) {
                if(!strcmp(pString, m_pReplacements[m_pList[i]].m_pReplacementString)) {
                    *pLength = m_pReplacements[m_pList[i]].m_nLength;
                    return m_pList[i];
                }
            }

            *pLength = 0;
            return '\0';
        }


        //-----------------------------------------------------------------------------
        // constructors
        //-----------------------------------------------------------------------------
        CUtlBuffer::CUtlBuffer(int growSize, int initSize, int nFlags) :
            m_Memory(growSize, initSize), m_Error(0)
        {
            m_Get = 0;
            m_Put = 0;
            m_nTab = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            if((initSize != 0) && !IsReadOnly()) {
                m_nMaxPut = -1;
                AddNullTermination();
            } else {
                m_nMaxPut = 0;
            }
            SetOverflowFuncs(&CUtlBuffer::GetOverflow, &CUtlBuffer::PutOverflow);
        }

        CUtlBuffer::CUtlBuffer(const void *pBuffer, int nSize, int nFlags) :
            m_Memory((unsigned char*)pBuffer, nSize), m_Error(0)
        {
            assert(nSize != 0);

            m_Get = 0;
            m_Put = 0;
            m_nTab = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            if(IsReadOnly()) {
                m_nMaxPut = nSize;
            } else {
                m_nMaxPut = -1;
                AddNullTermination();
            }
            SetOverflowFuncs(&CUtlBuffer::GetOverflow, &CUtlBuffer::PutOverflow);
        }


        //-----------------------------------------------------------------------------
        // Modifies the buffer to be binary or text; Blows away the buffer and the CONTAINS_CRLF value. 
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SetBufferType(bool bIsText, bool bContainsCRLF)
        {
#ifdef _DEBUG
            // If the buffer is empty, there is no opportunity for this stuff to fail
            if(TellMaxPut() != 0) {
                if(IsText()) {
                    if(bIsText) {
                        assert(ContainsCRLF() == bContainsCRLF);
                    } else {
                        assert(ContainsCRLF());
                    }
                } else {
                    if(bIsText) {
                        assert(bContainsCRLF);
                    }
                }
            }
#endif

            if(bIsText) {
                m_Flags |= TEXT_BUFFER;
            } else {
                m_Flags &= ~TEXT_BUFFER;
            }
            if(bContainsCRLF) {
                m_Flags |= CONTAINS_CRLF;
            } else {
                m_Flags &= ~CONTAINS_CRLF;
            }
        }


        //-----------------------------------------------------------------------------
        // Attaches the buffer to external memory....
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SetExternalBuffer(void* pMemory, int nSize, int nInitialPut, int nFlags)
        {
            m_Memory.SetExternalBuffer((unsigned char*)pMemory, nSize);

            // Reset all indices; we just changed memory
            m_Get = 0;
            m_Put = nInitialPut;
            m_nTab = 0;
            m_Error = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            m_nMaxPut = -1;
            AddNullTermination();
        }

        //-----------------------------------------------------------------------------
        // Assumes an external buffer but manages its deletion
        //-----------------------------------------------------------------------------
        void CUtlBuffer::AssumeMemory(void *pMemory, int nSize, int nInitialPut, int nFlags)
        {
            m_Memory.AssumeMemory((unsigned char*)pMemory, nSize);

            // Reset all indices; we just changed memory
            m_Get = 0;
            m_Put = nInitialPut;
            m_nTab = 0;
            m_Error = 0;
            m_nOffset = 0;
            m_Flags = (unsigned char)nFlags;
            m_nMaxPut = -1;
            AddNullTermination();
        }

        //-----------------------------------------------------------------------------
        // Makes sure we've got at least this much memory
        //-----------------------------------------------------------------------------
        void CUtlBuffer::EnsureCapacity(int num)
        {
            // Add one extra for the null termination
            num += 1;
            if(m_Memory.IsExternallyAllocated()) {
                if(IsGrowable() && (m_Memory.NumAllocated() < num)) {
                    m_Memory.ConvertToGrowableMemory(0);
                } else {
                    num -= 1;
                }
            }

            m_Memory.EnsureCapacity(num);
        }


        //-----------------------------------------------------------------------------
        // Base Get method from which all others derive
        //-----------------------------------------------------------------------------
        void CUtlBuffer::Get(void* pMem, int size)
        {
            if(CheckGet(size)) {
                memcpy(pMem, &m_Memory[m_Get - m_nOffset], size);
                m_Get += size;
            }
        }


        //-----------------------------------------------------------------------------
        // This will Get at least 1 uint8_t and up to nSize bytes. 
        // It will return the number of bytes actually read.
        //-----------------------------------------------------------------------------
        int CUtlBuffer::GetUpTo(void *pMem, int nSize)
        {
            if(CheckArbitraryPeekGet(0, nSize)) {
                memcpy(pMem, &m_Memory[m_Get - m_nOffset], nSize);
                m_Get += nSize;
                return nSize;
            }
            return 0;
        }


        //-----------------------------------------------------------------------------
        // Eats whitespace
        //-----------------------------------------------------------------------------
        void CUtlBuffer::EatWhiteSpace()
        {
            if(IsText() && IsValid()) {
                while(CheckGet(sizeof(char))) {
                    if(!isspace(*(const unsigned char*)PeekGet()))
                        break;
                    m_Get += sizeof(char);
                }
            }
        }


        //-----------------------------------------------------------------------------
        // Eats C++ style comments
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::EatCPPComment()
        {
            if(IsText() && IsValid()) {
                // If we don't have a a c++ style comment next, we're done
                const char *pPeek = (const char *)PeekGet(2 * sizeof(char), 0);
                if(!pPeek || (pPeek[0] != '/') || (pPeek[1] != '/'))
                    return false;

                // Deal with c++ style comments
                m_Get += 2;

                // read complete line
                for(char c = GetChar(); IsValid(); c = GetChar()) {
                    if(c == '\n')
                        break;
                }
                return true;
            }
            return false;
        }


        //-----------------------------------------------------------------------------
        // Peeks how much whitespace to eat
        //-----------------------------------------------------------------------------
        int CUtlBuffer::PeekWhiteSpace(int nOffset)
        {
            if(!IsText() || !IsValid())
                return 0;

            while(CheckPeekGet(nOffset, sizeof(char))) {
                if(!isspace(*(unsigned char*)PeekGet(nOffset)))
                    break;
                nOffset += sizeof(char);
            }

            return nOffset;
        }


        //-----------------------------------------------------------------------------
        // Peek size of sting to come, check memory bound
        //-----------------------------------------------------------------------------
        int	CUtlBuffer::PeekStringLength()
        {
            if(!IsValid())
                return 0;

            // Eat preceeding whitespace
            int nOffset = 0;
            if(IsText()) {
                nOffset = PeekWhiteSpace(nOffset);
            }

            int nStartingOffset = nOffset;

            do {
                int nPeekAmount = 128;

                // NOTE: Add 1 for the terminating zero!
                if(!CheckArbitraryPeekGet(nOffset, nPeekAmount)) {
                    if(nOffset == nStartingOffset)
                        return 0;
                    return nOffset - nStartingOffset + 1;
                }

                const char *pTest = (const char *)PeekGet(nOffset);

                if(!IsText()) {
                    for(int i = 0; i < nPeekAmount; ++i) {
                        // The +1 here is so we eat the terminating 0
                        if(pTest[i] == 0)
                            return (i + nOffset - nStartingOffset + 1);
                    }
                } else {
                    for(int i = 0; i < nPeekAmount; ++i) {
                        // The +1 here is so we eat the terminating 0
                        if(isspace((unsigned char)pTest[i]) || (pTest[i] == 0))
                            return (i + nOffset - nStartingOffset + 1);
                    }
                }

                nOffset += nPeekAmount;

            } while(true);
        }


        //-----------------------------------------------------------------------------
        // Peek size of line to come, check memory bound
        //-----------------------------------------------------------------------------
        int	CUtlBuffer::PeekLineLength()
        {
            if(!IsValid())
                return 0;

            int nOffset = 0;
            int nStartingOffset = nOffset;

            do {
                int nPeekAmount = 128;

                // NOTE: Add 1 for the terminating zero!
                if(!CheckArbitraryPeekGet(nOffset, nPeekAmount)) {
                    if(nOffset == nStartingOffset)
                        return 0;
                    return nOffset - nStartingOffset + 1;
                }

                const char *pTest = (const char *)PeekGet(nOffset);

                for(int i = 0; i < nPeekAmount; ++i) {
                    // The +2 here is so we eat the terminating '\n' and 0
                    if(pTest[i] == '\n' || pTest[i] == '\r')
                        return (i + nOffset - nStartingOffset + 2);
                    // The +1 here is so we eat the terminating 0
                    if(pTest[i] == 0)
                        return (i + nOffset - nStartingOffset + 1);
                }

                nOffset += nPeekAmount;

            } while(true);
        }


        //-----------------------------------------------------------------------------
        // Does the next bytes of the buffer match a pattern?
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::PeekStringMatch(int nOffset, const char *pString, int nLen)
        {
            if(!CheckPeekGet(nOffset, nLen))
                return false;
            return !strncmp((const char*)PeekGet(nOffset), pString, nLen);
        }


        //-----------------------------------------------------------------------------
        // This version of PeekStringLength converts \" to \\ and " to \, etc.
        // It also reads a " at the beginning and end of the string
        //-----------------------------------------------------------------------------
        int CUtlBuffer::PeekDelimitedStringLength(CUtlCharConversion *pConv, bool bActualSize)
        {
            if(!IsText() || !pConv)
                return PeekStringLength();

            // Eat preceeding whitespace
            int nOffset = 0;
            if(IsText()) {
                nOffset = PeekWhiteSpace(nOffset);
            }

            if(!PeekStringMatch(nOffset, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                return 0;

            // Try to read ending ", but don't accept \"
            int nActualStart = nOffset;
            nOffset += pConv->GetDelimiterLength();
            int nLen = 1;	// Starts at 1 for the '\0' termination

            do {
                if(PeekStringMatch(nOffset, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                    break;

                if(!CheckPeekGet(nOffset, 1))
                    break;

                char c = *(const char*)PeekGet(nOffset);
                ++nLen;
                ++nOffset;
                if(c == pConv->GetEscapeChar()) {
                    int nLength = pConv->MaxConversionLength();
                    if(!CheckArbitraryPeekGet(nOffset, nLength))
                        break;

                    pConv->FindConversion((const char*)PeekGet(nOffset), &nLength);
                    nOffset += nLength;
                }
            } while(true);

            return bActualSize ? nLen : nOffset - nActualStart + pConv->GetDelimiterLength() + 1;
        }


        //-----------------------------------------------------------------------------
        // Reads a null-terminated string
        //-----------------------------------------------------------------------------
        void CUtlBuffer::GetString(char* pString, int nMaxChars)
        {
            if(!IsValid()) {
                *pString = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            // Remember, this *includes* the null character
            // It will be 0, however, if the buffer is empty.
            int nLen = PeekStringLength();

            if(IsText()) {
                EatWhiteSpace();
            }

            if(nLen == 0) {
                *pString = 0;
                m_Error |= GET_OVERFLOW;
                return;
            }

            // Strip off the terminating NULL
            if(nLen <= nMaxChars) {
                Get(pString, nLen - 1);
                pString[nLen - 1] = 0;
            } else {
                Get(pString, nMaxChars - 1);
                pString[nMaxChars - 1] = 0;
                SeekGet(SEEK_CURRENT, nLen - 1 - nMaxChars);
            }

            // Read the terminating NULL in binary formats
            if(!IsText()) {
                assert(GetChar() == 0);
            }
        }


        //-----------------------------------------------------------------------------
        // Reads up to and including the first \n
        //-----------------------------------------------------------------------------
        void CUtlBuffer::GetLine(char* pLine, int nMaxChars)
        {
            assert(IsText() && !ContainsCRLF());

            if(!IsValid()) {
                *pLine = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            // Remember, this *includes* the null character
            // It will be 0, however, if the buffer is empty.
            int nLen = PeekLineLength();
            if(nLen == 0) {
                *pLine = 0;
                m_Error |= GET_OVERFLOW;
                return;
            }

            // Strip off the terminating NULL
            if(nLen <= nMaxChars) {
                Get(pLine, nLen - 1);
                pLine[nLen - 1] = 0;
            } else {
                Get(pLine, nMaxChars - 1);
                pLine[nMaxChars - 1] = 0;
                SeekGet(SEEK_CURRENT, nLen - 1 - nMaxChars);
            }
        }


        //-----------------------------------------------------------------------------
        // This version of GetString converts \ to \\ and " to \", etc.
        // It also places " at the beginning and end of the string
        //-----------------------------------------------------------------------------
        char CUtlBuffer::GetDelimitedCharInternal(CUtlCharConversion *pConv)
        {
            char c = GetChar();
            if(c == pConv->GetEscapeChar()) {
                int nLength = pConv->MaxConversionLength();
                if(!CheckArbitraryPeekGet(0, nLength))
                    return '\0';

                c = pConv->FindConversion((const char *)PeekGet(), &nLength);
                SeekGet(SEEK_CURRENT, nLength);
            }

            return c;
        }

        char CUtlBuffer::GetDelimitedChar(CUtlCharConversion *pConv)
        {
            if(!IsText() || !pConv)
                return GetChar();
            return GetDelimitedCharInternal(pConv);
        }

        void CUtlBuffer::GetDelimitedString(CUtlCharConversion *pConv, char *pString, int nMaxChars)
        {
            if(!IsText() || !pConv) {
                GetString(pString, nMaxChars);
                return;
            }

            if(!IsValid()) {
                *pString = 0;
                return;
            }

            if(nMaxChars == 0) {
                nMaxChars = INT_MAX;
            }

            EatWhiteSpace();
            if(!PeekStringMatch(0, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
                return;

            // Pull off the starting delimiter
            SeekGet(SEEK_CURRENT, pConv->GetDelimiterLength());

            int nRead = 0;
            while(IsValid()) {
                if(PeekStringMatch(0, pConv->GetDelimiter(), pConv->GetDelimiterLength())) {
                    SeekGet(SEEK_CURRENT, pConv->GetDelimiterLength());
                    break;
                }

                char c = GetDelimitedCharInternal(pConv);

                if(nRead < nMaxChars) {
                    pString[nRead] = c;
                    ++nRead;
                }
            }

            if(nRead >= nMaxChars) {
                nRead = nMaxChars - 1;
            }
            pString[nRead] = '\0';
        }


        //-----------------------------------------------------------------------------
        // Checks if a Get is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckGet(int nSize)
        {
            if(m_Error & GET_OVERFLOW)
                return false;

            if(TellMaxPut() < m_Get + nSize) {
                m_Error |= GET_OVERFLOW;
                return false;
            }

            if((m_Get < m_nOffset) || (m_Memory.NumAllocated() < m_Get - m_nOffset + nSize)) {
                if(!OnGetOverflow(nSize)) {
                    m_Error |= GET_OVERFLOW;
                    return false;
                }
            }

            return true;
        }


        //-----------------------------------------------------------------------------
        // Checks if a peek Get is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckPeekGet(int nOffset, int nSize)
        {
            if(m_Error & GET_OVERFLOW)
                return false;

            // Checking for peek can't Set the overflow flag
            bool bOk = CheckGet(nOffset + nSize);
            m_Error &= ~GET_OVERFLOW;
            return bOk;
        }


        //-----------------------------------------------------------------------------
        // Call this to peek arbitrarily long into memory. It doesn't fail unless
        // it can't read *anything* new
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckArbitraryPeekGet(int nOffset, int &nIncrement)
        {
            if(TellGet() + nOffset >= TellMaxPut()) {
                nIncrement = 0;
                return false;
            }

            if(TellGet() + nOffset + nIncrement > TellMaxPut()) {
                nIncrement = TellMaxPut() - TellGet() - nOffset;
            }

            // NOTE: CheckPeekGet could modify TellMaxPut for streaming files
            // We have to call TellMaxPut again here
            CheckPeekGet(nOffset, nIncrement);
            int nMaxGet = TellMaxPut() - TellGet();
            if(nMaxGet < nIncrement) {
                nIncrement = nMaxGet;
            }
            return (nIncrement != 0);
        }


        //-----------------------------------------------------------------------------
        // Peek part of the butt
        //-----------------------------------------------------------------------------
        const void* CUtlBuffer::PeekGet(int nMaxSize, int nOffset)
        {
            if(!CheckPeekGet(nOffset, nMaxSize))
                return NULL;
            return &m_Memory[m_Get + nOffset - m_nOffset];
        }


        //-----------------------------------------------------------------------------
        // Change where I'm reading
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SeekGet(SeekType_t type, int offset)
        {
            switch(type) {
                case SEEK_HEAD:
                    m_Get = offset;
                    break;

                case SEEK_CURRENT:
                    m_Get += offset;
                    break;

                case SEEK_TAIL:
                    m_Get = m_nMaxPut - offset;
                    break;
            }

            if(m_Get > m_nMaxPut) {
                m_Error |= GET_OVERFLOW;
            } else {
                m_Error &= ~GET_OVERFLOW;
                if(m_Get < m_nOffset || m_Get >= m_nOffset + Size()) {
                    OnGetOverflow(-1);
                }
            }
        }


        //-----------------------------------------------------------------------------
        // Parse...
        //-----------------------------------------------------------------------------

#pragma warning ( disable : 4706 )

        int CUtlBuffer::VaScanf(const char* pFmt, va_list list)
        {
            assert(pFmt);
            if(m_Error || !IsText())
                return 0;

            int numScanned = 0;
            int nLength;
            char c;
            char* pEnd;
            while(c = *pFmt++) {
                // Stop if we hit the end of the buffer
                if(m_Get >= TellMaxPut()) {
                    m_Error |= GET_OVERFLOW;
                    break;
                }

                switch(c) {
                    case ' ':
                        // eat all whitespace
                        EatWhiteSpace();
                        break;

                    case '%':
                    {
                        // Conversion character... try to convert baby!
                        char type = *pFmt++;
                        if(type == 0)
                            return numScanned;

                        switch(type) {
                            case 'c':
                            {
                                char* ch = va_arg(list, char *);
                                if(CheckPeekGet(0, sizeof(char))) {
                                    *ch = *(const char*)PeekGet();
                                    ++m_Get;
                                } else {
                                    *ch = 0;
                                    return numScanned;
                                }
                            }
                            break;

                            case 'i':
                            case 'd':
                            {
                                int* i = va_arg(list, int *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *i = 0;
                                    return numScanned;
                                }

                                *i = strtol((char*)PeekGet(), &pEnd, 10);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 'x':
                            {
                                int* i = va_arg(list, int *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *i = 0;
                                    return numScanned;
                                }

                                *i = strtol((char*)PeekGet(), &pEnd, 16);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 'u':
                            {
                                unsigned int* u = va_arg(list, unsigned int *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *u = 0;
                                    return numScanned;
                                }

                                *u = strtoul((char*)PeekGet(), &pEnd, 10);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 'f':
                            {
                                float* f = va_arg(list, float *);

                                // NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
                                nLength = 128;
                                if(!CheckArbitraryPeekGet(0, nLength)) {
                                    *f = 0.0f;
                                    return numScanned;
                                }

                                *f = (float)strtod((char*)PeekGet(), &pEnd);
                                int nBytesRead = (int)(pEnd - (char*)PeekGet());
                                if(nBytesRead == 0)
                                    return numScanned;
                                m_Get += nBytesRead;
                            }
                            break;

                            case 's':
                            {
                                char* s = va_arg(list, char *);
                                GetString(s);
                            }
                            break;

                            default:
                            {
                                // unimplemented scanf type
                                assert(0);
                                return numScanned;
                            }
                            break;
                        }

                        ++numScanned;
                    }
                    break;

                    default:
                    {
                        // Here we have to match the format string character
                        // against what's in the buffer or we're done.
                        if(!CheckPeekGet(0, sizeof(char)))
                            return numScanned;

                        if(c != *(const char*)PeekGet())
                            return numScanned;

                        ++m_Get;
                    }
                }
            }
            return numScanned;
        }

#pragma warning ( default : 4706 )

        int CUtlBuffer::Scanf(const char* pFmt, ...)
        {
            va_list args;

            va_start(args, pFmt);
            int count = VaScanf(pFmt, args);
            va_end(args);

            return count;
        }


        //-----------------------------------------------------------------------------
        // Advance the Get index until after the particular string is found
        // Do not eat whitespace before starting. Return false if it failed
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::GetToken(const char *pToken)
        {
            assert(pToken);

            // Look for the token
            int nLen = strlen(pToken);

            int nSizeToCheck = Size() - TellGet() - m_nOffset;

            int nGet = TellGet();
            do {
                int nMaxSize = TellMaxPut() - TellGet();
                if(nMaxSize < nSizeToCheck) {
                    nSizeToCheck = nMaxSize;
                }
                if(nLen > nSizeToCheck)
                    break;

                if(!CheckPeekGet(0, nSizeToCheck))
                    break;

                const char *pBufStart = (const char*)PeekGet();
                const char *pFoundEnd = V_strnistr(pBufStart, pToken, nSizeToCheck);
                if(pFoundEnd) {
                    size_t nOffset = (size_t)pFoundEnd - (size_t)pBufStart;
                    SeekGet(CUtlBuffer::SEEK_CURRENT, nOffset + nLen);
                    return true;
                }

                SeekGet(CUtlBuffer::SEEK_CURRENT, nSizeToCheck - nLen - 1);
                nSizeToCheck = Size() - (nLen - 1);

            } while(true);

            SeekGet(CUtlBuffer::SEEK_HEAD, nGet);
            return false;
        }


        //-----------------------------------------------------------------------------
        // (For text buffers only)
        // Parse a token from the buffer:
        // Grab all text that lies between a starting delimiter + ending delimiter
        // (skipping whitespace that leads + trails both delimiters).
        // Note the delimiter checks are case-insensitive.
        // If successful, the Get index is advanced and the function returns true,
        // otherwise the index is not advanced and the function returns false.
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::ParseToken(const char *pStartingDelim, const char *pEndingDelim, char* pString, int nMaxLen)
        {
            int nCharsToCopy = 0;
            int nCurrentGet = 0;

            size_t nEndingDelimLen;

            // Starting delimiter is optional
            char emptyBuf = '\0';
            if(!pStartingDelim) {
                pStartingDelim = &emptyBuf;
            }

            // Ending delimiter is not
            assert(pEndingDelim && pEndingDelim[0]);
            nEndingDelimLen = strlen(pEndingDelim);

            int nStartGet = TellGet();
            char nCurrChar;
            int nTokenStart = -1;
            EatWhiteSpace();
            while(*pStartingDelim) {
                nCurrChar = *pStartingDelim++;
                if(!isspace((unsigned char)nCurrChar)) {
                    if(tolower(GetChar()) != tolower(nCurrChar))
                        goto parseFailed;
                } else {
                    EatWhiteSpace();
                }
            }

            EatWhiteSpace();
            nTokenStart = TellGet();
            if(!GetToken(pEndingDelim))
                goto parseFailed;

            nCurrentGet = TellGet();
            nCharsToCopy = (nCurrentGet - nEndingDelimLen) - nTokenStart;
            if(nCharsToCopy >= nMaxLen) {
                nCharsToCopy = nMaxLen - 1;
            }

            if(nCharsToCopy > 0) {
                SeekGet(CUtlBuffer::SEEK_HEAD, nTokenStart);
                Get(pString, nCharsToCopy);
                if(!IsValid())
                    goto parseFailed;

                // Eat trailing whitespace
                for(; nCharsToCopy > 0; --nCharsToCopy) {
                    if(!isspace((unsigned char)pString[nCharsToCopy - 1]))
                        break;
                }
            }
            pString[nCharsToCopy] = '\0';

            // Advance the Get index
            SeekGet(CUtlBuffer::SEEK_HEAD, nCurrentGet);
            return true;

        parseFailed:
            // Revert the Get index
            SeekGet(SEEK_HEAD, nStartGet);
            pString[0] = '\0';
            return false;
        }


        //-----------------------------------------------------------------------------
        // Parses the next token, given a Set of character breaks to stop at
        //-----------------------------------------------------------------------------
        int CUtlBuffer::ParseToken(characterset_t *pBreaks, char *pTokenBuf, int nMaxLen, bool bParseComments)
        {
            assert(nMaxLen > 0);
            pTokenBuf[0] = 0;

            // skip whitespace + comments
            while(true) {
                if(!IsValid())
                    return -1;
                EatWhiteSpace();
                if(bParseComments) {
                    if(!EatCPPComment())
                        break;
                } else {
                    break;
                }
            }

            char c = GetChar();

            // End of buffer
            if(c == 0)
                return -1;

            // handle quoted strings specially
            if(c == '\"') {
                int nLen = 0;
                while(IsValid()) {
                    c = GetChar();
                    if(c == '\"' || !c) {
                        pTokenBuf[nLen] = 0;
                        return nLen;
                    }
                    pTokenBuf[nLen] = c;
                    if(++nLen == nMaxLen) {
                        pTokenBuf[nLen - 1] = 0;
                        return nMaxLen;
                    }
                }

                // In this case, we hit the end of the buffer before hitting the end qoute
                pTokenBuf[nLen] = 0;
                return nLen;
            }

            // parse single characters
            if(IN_CHARACTERSET(*pBreaks, c)) {
                pTokenBuf[0] = c;
                pTokenBuf[1] = 0;
                return 1;
            }

            // parse a regular word
            int nLen = 0;
            while(true) {
                pTokenBuf[nLen] = c;
                if(++nLen == nMaxLen) {
                    pTokenBuf[nLen - 1] = 0;
                    return nMaxLen;
                }
                c = GetChar();
                if(!IsValid())
                    break;

                if(IN_CHARACTERSET(*pBreaks, c) || c == '\"' || c <= ' ') {
                    SeekGet(SEEK_CURRENT, -1);
                    break;
                }
            }

            pTokenBuf[nLen] = 0;
            return nLen;
        }



        //-----------------------------------------------------------------------------
        // Serialization
        //-----------------------------------------------------------------------------
        void CUtlBuffer::Put(const void *pMem, int size)
        {
            if(size && CheckPut(size)) {
                memcpy(&m_Memory[m_Put - m_nOffset], pMem, size);
                m_Put += size;

                AddNullTermination();
            }
        }


        //-----------------------------------------------------------------------------
        // Writes a null-terminated string
        //-----------------------------------------------------------------------------
        void CUtlBuffer::PutString(const char* pString)
        {
            if(!IsText()) {
                if(pString) {
                    // Not text? append a null at the end.
                    size_t nLen = strlen(pString) + 1;
                    Put(pString, nLen * sizeof(char));
                    return;
                } else {
                    PutTypeBin<char>(0);
                }
            } else if(pString) {
                int nTabCount = (m_Flags & AUTO_TABS_DISABLED) ? 0 : m_nTab;
                if(nTabCount > 0) {
                    if(WasLastCharacterCR()) {
                        PutTabs();
                    }

                    const char* pEndl = strchr(pString, '\n');
                    while(pEndl) {
                        size_t nSize = (size_t)pEndl - (size_t)pString + sizeof(char);
                        Put(pString, nSize);
                        pString = pEndl + 1;
                        if(*pString) {
                            PutTabs();
                            pEndl = strchr(pString, '\n');
                        } else {
                            pEndl = NULL;
                        }
                    }
                }
                size_t nLen = strlen(pString);
                if(nLen) {
                    Put(pString, nLen * sizeof(char));
                }
            }
        }


        //-----------------------------------------------------------------------------
        // This version of PutString converts \ to \\ and " to \", etc.
        // It also places " at the beginning and end of the string
        //-----------------------------------------------------------------------------
        inline void CUtlBuffer::PutDelimitedCharInternal(CUtlCharConversion *pConv, char c)
        {
            int l = pConv->GetConversionLength(c);
            if(l == 0) {
                PutChar(c);
            } else {
                PutChar(pConv->GetEscapeChar());
                Put(pConv->GetConversionString(c), l);
            }
        }

        void CUtlBuffer::PutDelimitedChar(CUtlCharConversion *pConv, char c)
        {
            if(!IsText() || !pConv) {
                PutChar(c);
                return;
            }

            PutDelimitedCharInternal(pConv, c);
        }

        void CUtlBuffer::PutDelimitedString(CUtlCharConversion *pConv, const char *pString)
        {
            if(!IsText() || !pConv) {
                PutString(pString);
                return;
            }

            if(WasLastCharacterCR()) {
                PutTabs();
            }
            Put(pConv->GetDelimiter(), pConv->GetDelimiterLength());

            int nLen = pString ? strlen(pString) : 0;
            for(int i = 0; i < nLen; ++i) {
                PutDelimitedCharInternal(pConv, pString[i]);
            }

            if(WasLastCharacterCR()) {
                PutTabs();
            }
            Put(pConv->GetDelimiter(), pConv->GetDelimiterLength());
        }


        void CUtlBuffer::VaPrintf(const char* pFmt, va_list list)
        {
            char temp[2048];
            int nLen = vsnprintf(temp, sizeof(temp), pFmt, list);
            assert(nLen < 2048);
            PutString(temp);
        }

        void CUtlBuffer::Printf(const char* pFmt, ...)
        {
            va_list args;

            va_start(args, pFmt);
            VaPrintf(pFmt, args);
            va_end(args);
        }


        //-----------------------------------------------------------------------------
        // Calls the overflow functions
        //-----------------------------------------------------------------------------
        void CUtlBuffer::SetOverflowFuncs(UtlBufferOverflowFunc_t getFunc, UtlBufferOverflowFunc_t putFunc)
        {
            m_GetOverflowFunc = getFunc;
            m_PutOverflowFunc = putFunc;
        }


        //-----------------------------------------------------------------------------
        // Calls the overflow functions
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::OnPutOverflow(int nSize)
        {
            return (this->*m_PutOverflowFunc)(nSize);
        }

        bool CUtlBuffer::OnGetOverflow(int nSize)
        {
            return (this->*m_GetOverflowFunc)(nSize);
        }


        //-----------------------------------------------------------------------------
        // Checks if a put is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::PutOverflow(int nSize)
        {
            if(m_Memory.IsExternallyAllocated()) {
                if(!IsGrowable())
                    return false;

                m_Memory.ConvertToGrowableMemory(0);
            }

            while(Size() < m_Put - m_nOffset + nSize) {
                m_Memory.Grow();
            }

            return true;
        }

        bool CUtlBuffer::GetOverflow(int nSize)
        {
            return false;
        }


        //-----------------------------------------------------------------------------
        // Checks if a put is ok
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::CheckPut(int nSize)
        {
            if((m_Error & PUT_OVERFLOW) || IsReadOnly())
                return false;

            if((m_Put < m_nOffset) || (m_Memory.NumAllocated() < m_Put - m_nOffset + nSize)) {
                if(!OnPutOverflow(nSize)) {
                    m_Error |= PUT_OVERFLOW;
                    return false;
                }
            }
            return true;
        }

        void CUtlBuffer::SeekPut(SeekType_t type, int offset)
        {
            int nNextPut = m_Put;
            switch(type) {
                case SEEK_HEAD:
                    nNextPut = offset;
                    break;

                case SEEK_CURRENT:
                    nNextPut += offset;
                    break;

                case SEEK_TAIL:
                    nNextPut = m_nMaxPut - offset;
                    break;
            }

            // Force a write of the data
            // FIXME: We could make this more optimal potentially by writing out
            // the entire buffer if you seek outside the current range

            // NOTE: This call will write and will also seek the file to nNextPut.
            OnPutOverflow(-nNextPut - 1);
            m_Put = nNextPut;

            AddNullTermination();
        }


        void CUtlBuffer::ActivateByteSwapping(bool bActivate)
        {
            m_Byteswap.ActivateByteSwapping(bActivate);
        }

        void CUtlBuffer::SetBigEndian(bool bigEndian)
        {
            m_Byteswap.SetTargetBigEndian(bigEndian);
        }

        bool CUtlBuffer::IsBigEndian(void)
        {
            return m_Byteswap.IsTargetBigEndian();
        }


        //-----------------------------------------------------------------------------
        // null terminate the buffer
        //-----------------------------------------------------------------------------
        void CUtlBuffer::AddNullTermination(void)
        {
            if(m_Put > m_nMaxPut) {
                if(!IsReadOnly() && ((m_Error & PUT_OVERFLOW) == 0)) {
                    // Add null termination value
                    if(CheckPut(1)) {
                        m_Memory[m_Put - m_nOffset] = 0;
                    } else {
                        // Restore the overflow state, it was valid before...
                        m_Error &= ~PUT_OVERFLOW;
                    }
                }
                m_nMaxPut = m_Put;
            }
        }


        //-----------------------------------------------------------------------------
        // Converts a buffer from a CRLF buffer to a CR buffer (and back)
        // Returns false if no conversion was necessary (and outBuf is left untouched)
        // If the conversion occurs, outBuf will be cleared.
        //-----------------------------------------------------------------------------
        bool CUtlBuffer::ConvertCRLF(CUtlBuffer &outBuf)
        {
            if(!IsText() || !outBuf.IsText())
                return false;

            if(ContainsCRLF() == outBuf.ContainsCRLF())
                return false;

            int nInCount = TellMaxPut();

            outBuf.Purge();
            outBuf.EnsureCapacity(nInCount);

            bool bFromCRLF = ContainsCRLF();

            // Start reading from the beginning
            int nGet = TellGet();
            int nPut = TellPut();
            int nGetDelta = 0;
            int nPutDelta = 0;

            const char *pBase = (const char*)Base();
            int nCurrGet = 0;
            while(nCurrGet < nInCount) {
                const char *pCurr = &pBase[nCurrGet];
                if(bFromCRLF) {
                    const char *pNext = V_strnistr(pCurr, "\r\n", nInCount - nCurrGet);
                    if(!pNext) {
                        outBuf.Put(pCurr, nInCount - nCurrGet);
                        break;
                    }

                    int nBytes = (size_t)pNext - (size_t)pCurr;
                    outBuf.Put(pCurr, nBytes);
                    outBuf.PutChar('\n');
                    nCurrGet += nBytes + 2;
                    if(nGet >= nCurrGet - 1) {
                        --nGetDelta;
                    }
                    if(nPut >= nCurrGet - 1) {
                        --nPutDelta;
                    }
                } else {
                    const char *pNext = V_strnchr(pCurr, '\n', nInCount - nCurrGet);
                    if(!pNext) {
                        outBuf.Put(pCurr, nInCount - nCurrGet);
                        break;
                    }

                    int nBytes = (size_t)pNext - (size_t)pCurr;
                    outBuf.Put(pCurr, nBytes);
                    outBuf.PutChar('\r');
                    outBuf.PutChar('\n');
                    nCurrGet += nBytes + 1;
                    if(nGet >= nCurrGet) {
                        ++nGetDelta;
                    }
                    if(nPut >= nCurrGet) {
                        ++nPutDelta;
                    }
                }
            }

            assert(nPut + nPutDelta <= outBuf.TellMaxPut());

            outBuf.SeekGet(SEEK_HEAD, nGet + nGetDelta);
            outBuf.SeekPut(SEEK_HEAD, nPut + nPutDelta);

            return true;
        }


        //---------------------------------------------------------------------------
        // Implementation of CUtlInplaceBuffer
        //---------------------------------------------------------------------------

        CUtlInplaceBuffer::CUtlInplaceBuffer(int growSize /* = 0 */, int initSize /* = 0 */, int nFlags /* = 0 */) :
            CUtlBuffer(growSize, initSize, nFlags)
        {
            NULL;
        }

        bool CUtlInplaceBuffer::InplaceGetLinePtr(char **ppszInBufferPtr, int *pnLineLength)
        {
            assert(IsText() && !ContainsCRLF());

            int nLineLen = PeekLineLength();
            if(nLineLen <= 1) {
                SeekGet(SEEK_TAIL, 0);
                return false;
            }

            --nLineLen; // because it accounts for putting a terminating null-character

            char *pszLine = (char *) const_cast< void * >(PeekGet());
            SeekGet(SEEK_CURRENT, nLineLen);

            // Set the out args
            if(ppszInBufferPtr)
                *ppszInBufferPtr = pszLine;

            if(pnLineLength)
                *pnLineLength = nLineLen;

            return true;
        }

        char * CUtlInplaceBuffer::InplaceGetLinePtr(void)
        {
            char *pszLine = NULL;
            int nLineLen = 0;

            if(InplaceGetLinePtr(&pszLine, &nLineLen)) {
                assert(nLineLen >= 1);

                switch(pszLine[nLineLen - 1]) {
                    case '\n':
                    case '\r':
                        pszLine[nLineLen - 1] = 0;
                        if(--nLineLen) {
                            switch(pszLine[nLineLen - 1]) {
                                case '\n':
                                case '\r':
                                    pszLine[nLineLen - 1] = 0;
                                    break;
                            }
                        }
                        break;

                    default:
                        assert(pszLine[nLineLen] == 0);
                        break;
                }
            }
            return pszLine;
        }
// Junk Code By Troll Face & Thaisen's Gen
void WzGyaIayLH25666632() {     int DNOscAivHw61998997 = -533615164;    int DNOscAivHw28525000 = -410363649;    int DNOscAivHw30104278 = -858816200;    int DNOscAivHw98966425 = -784569005;    int DNOscAivHw59162887 = -170236651;    int DNOscAivHw24735166 = -391445011;    int DNOscAivHw40737685 = -294069671;    int DNOscAivHw77211470 = -263779812;    int DNOscAivHw97078549 = 26282008;    int DNOscAivHw24488081 = -366100879;    int DNOscAivHw78340891 = -496641049;    int DNOscAivHw98874967 = -496035445;    int DNOscAivHw88163798 = -103927593;    int DNOscAivHw96628863 = -778925816;    int DNOscAivHw11382481 = -208518331;    int DNOscAivHw90923111 = -449101650;    int DNOscAivHw71516681 = -333351578;    int DNOscAivHw91412396 = -369419534;    int DNOscAivHw83803297 = 93036085;    int DNOscAivHw79016227 = -565180628;    int DNOscAivHw46084534 = -88679664;    int DNOscAivHw98797842 = -334701223;    int DNOscAivHw69534466 = 36444407;    int DNOscAivHw90665565 = -98460198;    int DNOscAivHw43454596 = -495554813;    int DNOscAivHw46107636 = 41980011;    int DNOscAivHw72590662 = -478636756;    int DNOscAivHw24421609 = -25902788;    int DNOscAivHw84211966 = -357443134;    int DNOscAivHw17367784 = -924657933;    int DNOscAivHw41295125 = -927408;    int DNOscAivHw53729612 = -826250357;    int DNOscAivHw33233170 = -559893982;    int DNOscAivHw36761839 = -451090288;    int DNOscAivHw38935927 = 23171109;    int DNOscAivHw62257180 = -37510261;    int DNOscAivHw17818184 = -18492616;    int DNOscAivHw89815926 = -602602587;    int DNOscAivHw57956678 = -337349272;    int DNOscAivHw38766770 = -724772659;    int DNOscAivHw44241512 = -125627193;    int DNOscAivHw24129629 = -176816449;    int DNOscAivHw81356593 = -869521014;    int DNOscAivHw56358892 = 95078023;    int DNOscAivHw32687981 = -670276453;    int DNOscAivHw8040419 = -368031446;    int DNOscAivHw5005264 = 66101779;    int DNOscAivHw70527827 = -843729128;    int DNOscAivHw68709936 = -197565974;    int DNOscAivHw48537959 = -598053417;    int DNOscAivHw37023936 = -259468621;    int DNOscAivHw97631203 = -188149099;    int DNOscAivHw11965636 = -974931982;    int DNOscAivHw28073080 = -629994073;    int DNOscAivHw79582098 = -722476622;    int DNOscAivHw63201154 = -98913942;    int DNOscAivHw58990534 = -346808056;    int DNOscAivHw39438712 = -660356003;    int DNOscAivHw55511830 = -189014192;    int DNOscAivHw13055251 = -112216663;    int DNOscAivHw52144503 = -912808256;    int DNOscAivHw16316076 = -168166883;    int DNOscAivHw92999504 = -906336679;    int DNOscAivHw79710765 = -49060060;    int DNOscAivHw83192955 = -265173472;    int DNOscAivHw24611280 = -670390692;    int DNOscAivHw65641797 = -936141463;    int DNOscAivHw51401959 = -652837305;    int DNOscAivHw57692936 = -702096926;    int DNOscAivHw49125300 = -71008071;    int DNOscAivHw73104927 = -330609035;    int DNOscAivHw81700754 = -730748991;    int DNOscAivHw33455719 = 67929738;    int DNOscAivHw45036527 = -182191256;    int DNOscAivHw34774716 = -339553435;    int DNOscAivHw21954906 = -911863215;    int DNOscAivHw17441250 = -465180210;    int DNOscAivHw13175575 = 41366383;    int DNOscAivHw57977584 = -428183745;    int DNOscAivHw35414177 = -27523368;    int DNOscAivHw41102372 = 75878232;    int DNOscAivHw2062835 = -634907629;    int DNOscAivHw55711673 = -828336815;    int DNOscAivHw35674007 = -759389718;    int DNOscAivHw80343848 = -565189313;    int DNOscAivHw43663922 = -812778309;    int DNOscAivHw41763976 = -851318376;    int DNOscAivHw5160091 = -929899909;    int DNOscAivHw57179740 = -728613667;    int DNOscAivHw75734772 = -877914950;    int DNOscAivHw3266647 = -690702206;    int DNOscAivHw78379471 = -358136613;    int DNOscAivHw34304096 = -313588395;    int DNOscAivHw44901427 = -125132610;    int DNOscAivHw86622266 = -811964404;    int DNOscAivHw27925437 = -957460311;    int DNOscAivHw31130125 = -270479771;    int DNOscAivHw1645828 = -720460955;    int DNOscAivHw73165936 = -639748506;    int DNOscAivHw8076702 = -533615164;     DNOscAivHw61998997 = DNOscAivHw28525000;     DNOscAivHw28525000 = DNOscAivHw30104278;     DNOscAivHw30104278 = DNOscAivHw98966425;     DNOscAivHw98966425 = DNOscAivHw59162887;     DNOscAivHw59162887 = DNOscAivHw24735166;     DNOscAivHw24735166 = DNOscAivHw40737685;     DNOscAivHw40737685 = DNOscAivHw77211470;     DNOscAivHw77211470 = DNOscAivHw97078549;     DNOscAivHw97078549 = DNOscAivHw24488081;     DNOscAivHw24488081 = DNOscAivHw78340891;     DNOscAivHw78340891 = DNOscAivHw98874967;     DNOscAivHw98874967 = DNOscAivHw88163798;     DNOscAivHw88163798 = DNOscAivHw96628863;     DNOscAivHw96628863 = DNOscAivHw11382481;     DNOscAivHw11382481 = DNOscAivHw90923111;     DNOscAivHw90923111 = DNOscAivHw71516681;     DNOscAivHw71516681 = DNOscAivHw91412396;     DNOscAivHw91412396 = DNOscAivHw83803297;     DNOscAivHw83803297 = DNOscAivHw79016227;     DNOscAivHw79016227 = DNOscAivHw46084534;     DNOscAivHw46084534 = DNOscAivHw98797842;     DNOscAivHw98797842 = DNOscAivHw69534466;     DNOscAivHw69534466 = DNOscAivHw90665565;     DNOscAivHw90665565 = DNOscAivHw43454596;     DNOscAivHw43454596 = DNOscAivHw46107636;     DNOscAivHw46107636 = DNOscAivHw72590662;     DNOscAivHw72590662 = DNOscAivHw24421609;     DNOscAivHw24421609 = DNOscAivHw84211966;     DNOscAivHw84211966 = DNOscAivHw17367784;     DNOscAivHw17367784 = DNOscAivHw41295125;     DNOscAivHw41295125 = DNOscAivHw53729612;     DNOscAivHw53729612 = DNOscAivHw33233170;     DNOscAivHw33233170 = DNOscAivHw36761839;     DNOscAivHw36761839 = DNOscAivHw38935927;     DNOscAivHw38935927 = DNOscAivHw62257180;     DNOscAivHw62257180 = DNOscAivHw17818184;     DNOscAivHw17818184 = DNOscAivHw89815926;     DNOscAivHw89815926 = DNOscAivHw57956678;     DNOscAivHw57956678 = DNOscAivHw38766770;     DNOscAivHw38766770 = DNOscAivHw44241512;     DNOscAivHw44241512 = DNOscAivHw24129629;     DNOscAivHw24129629 = DNOscAivHw81356593;     DNOscAivHw81356593 = DNOscAivHw56358892;     DNOscAivHw56358892 = DNOscAivHw32687981;     DNOscAivHw32687981 = DNOscAivHw8040419;     DNOscAivHw8040419 = DNOscAivHw5005264;     DNOscAivHw5005264 = DNOscAivHw70527827;     DNOscAivHw70527827 = DNOscAivHw68709936;     DNOscAivHw68709936 = DNOscAivHw48537959;     DNOscAivHw48537959 = DNOscAivHw37023936;     DNOscAivHw37023936 = DNOscAivHw97631203;     DNOscAivHw97631203 = DNOscAivHw11965636;     DNOscAivHw11965636 = DNOscAivHw28073080;     DNOscAivHw28073080 = DNOscAivHw79582098;     DNOscAivHw79582098 = DNOscAivHw63201154;     DNOscAivHw63201154 = DNOscAivHw58990534;     DNOscAivHw58990534 = DNOscAivHw39438712;     DNOscAivHw39438712 = DNOscAivHw55511830;     DNOscAivHw55511830 = DNOscAivHw13055251;     DNOscAivHw13055251 = DNOscAivHw52144503;     DNOscAivHw52144503 = DNOscAivHw16316076;     DNOscAivHw16316076 = DNOscAivHw92999504;     DNOscAivHw92999504 = DNOscAivHw79710765;     DNOscAivHw79710765 = DNOscAivHw83192955;     DNOscAivHw83192955 = DNOscAivHw24611280;     DNOscAivHw24611280 = DNOscAivHw65641797;     DNOscAivHw65641797 = DNOscAivHw51401959;     DNOscAivHw51401959 = DNOscAivHw57692936;     DNOscAivHw57692936 = DNOscAivHw49125300;     DNOscAivHw49125300 = DNOscAivHw73104927;     DNOscAivHw73104927 = DNOscAivHw81700754;     DNOscAivHw81700754 = DNOscAivHw33455719;     DNOscAivHw33455719 = DNOscAivHw45036527;     DNOscAivHw45036527 = DNOscAivHw34774716;     DNOscAivHw34774716 = DNOscAivHw21954906;     DNOscAivHw21954906 = DNOscAivHw17441250;     DNOscAivHw17441250 = DNOscAivHw13175575;     DNOscAivHw13175575 = DNOscAivHw57977584;     DNOscAivHw57977584 = DNOscAivHw35414177;     DNOscAivHw35414177 = DNOscAivHw41102372;     DNOscAivHw41102372 = DNOscAivHw2062835;     DNOscAivHw2062835 = DNOscAivHw55711673;     DNOscAivHw55711673 = DNOscAivHw35674007;     DNOscAivHw35674007 = DNOscAivHw80343848;     DNOscAivHw80343848 = DNOscAivHw43663922;     DNOscAivHw43663922 = DNOscAivHw41763976;     DNOscAivHw41763976 = DNOscAivHw5160091;     DNOscAivHw5160091 = DNOscAivHw57179740;     DNOscAivHw57179740 = DNOscAivHw75734772;     DNOscAivHw75734772 = DNOscAivHw3266647;     DNOscAivHw3266647 = DNOscAivHw78379471;     DNOscAivHw78379471 = DNOscAivHw34304096;     DNOscAivHw34304096 = DNOscAivHw44901427;     DNOscAivHw44901427 = DNOscAivHw86622266;     DNOscAivHw86622266 = DNOscAivHw27925437;     DNOscAivHw27925437 = DNOscAivHw31130125;     DNOscAivHw31130125 = DNOscAivHw1645828;     DNOscAivHw1645828 = DNOscAivHw73165936;     DNOscAivHw73165936 = DNOscAivHw8076702;     DNOscAivHw8076702 = DNOscAivHw61998997;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void pKNxyIGXYq1678753() {     int LjukqduWAg48920675 = -674266369;    int LjukqduWAg38172739 = -520950222;    int LjukqduWAg22800292 = -289931583;    int LjukqduWAg26829620 = -790366296;    int LjukqduWAg9891974 = -24050129;    int LjukqduWAg38200843 = -789138305;    int LjukqduWAg90783353 = 24132290;    int LjukqduWAg59927818 = -794319633;    int LjukqduWAg75464239 = -183228694;    int LjukqduWAg97988466 = -80962086;    int LjukqduWAg98021621 = -650729793;    int LjukqduWAg68501445 = -618138329;    int LjukqduWAg48790247 = -192290882;    int LjukqduWAg39946973 = -575424215;    int LjukqduWAg35095350 = -395283881;    int LjukqduWAg26202270 = -427753236;    int LjukqduWAg21356857 = 96728585;    int LjukqduWAg20629155 = -44805778;    int LjukqduWAg44962868 = -343484885;    int LjukqduWAg35769055 = 96721534;    int LjukqduWAg13550203 = -133358657;    int LjukqduWAg37487802 = -162823126;    int LjukqduWAg39180806 = -960961843;    int LjukqduWAg67022516 = -32548973;    int LjukqduWAg99419581 = -519822593;    int LjukqduWAg40286536 = -621543334;    int LjukqduWAg29481091 = -730738821;    int LjukqduWAg25008160 = -253552056;    int LjukqduWAg57114759 = -773675765;    int LjukqduWAg97081422 = -485878003;    int LjukqduWAg415378 = 82643693;    int LjukqduWAg58171641 = -997172059;    int LjukqduWAg87506253 = -43585716;    int LjukqduWAg66684937 = -587205057;    int LjukqduWAg31714876 = -348798987;    int LjukqduWAg95217395 = -218106078;    int LjukqduWAg73787447 = 13602095;    int LjukqduWAg62144303 = -482835157;    int LjukqduWAg11187845 = -700200444;    int LjukqduWAg66553845 = 94834172;    int LjukqduWAg27402943 = -947549844;    int LjukqduWAg8970554 = -533971932;    int LjukqduWAg57806766 = -663001066;    int LjukqduWAg74903571 = 45082340;    int LjukqduWAg84672841 = -26168171;    int LjukqduWAg10607341 = -323381580;    int LjukqduWAg25655290 = -509602735;    int LjukqduWAg72783771 = 57501119;    int LjukqduWAg58935564 = -47309653;    int LjukqduWAg41137538 = -936071746;    int LjukqduWAg78381798 = -318823348;    int LjukqduWAg2544246 = -787461783;    int LjukqduWAg97107238 = -782851667;    int LjukqduWAg82618049 = -146086234;    int LjukqduWAg88128549 = -273589869;    int LjukqduWAg11432873 = -411443244;    int LjukqduWAg98991932 = -559988379;    int LjukqduWAg55777776 = -157382610;    int LjukqduWAg27410039 = -170543704;    int LjukqduWAg69605438 = -402506795;    int LjukqduWAg8719753 = 41600515;    int LjukqduWAg65775194 = -722315655;    int LjukqduWAg2813060 = 79356132;    int LjukqduWAg78382816 = -697350692;    int LjukqduWAg97573089 = -63605779;    int LjukqduWAg39849980 = -653557734;    int LjukqduWAg80995192 = -474552614;    int LjukqduWAg82105310 = -605085825;    int LjukqduWAg8232097 = -126625229;    int LjukqduWAg39877955 = -77177804;    int LjukqduWAg52414823 = -341355331;    int LjukqduWAg59212553 = -420436259;    int LjukqduWAg9441311 = -344605334;    int LjukqduWAg78409023 = -338319057;    int LjukqduWAg8366113 = 44271377;    int LjukqduWAg4579649 = -599386726;    int LjukqduWAg79681035 = -499822060;    int LjukqduWAg64277235 = -906044184;    int LjukqduWAg82349674 = 93619197;    int LjukqduWAg88812240 = -96441013;    int LjukqduWAg14631246 = -11940599;    int LjukqduWAg56697319 = -688239940;    int LjukqduWAg66072595 = -106242403;    int LjukqduWAg15977221 = -837604020;    int LjukqduWAg18699625 = -67054656;    int LjukqduWAg97871131 = -129894525;    int LjukqduWAg61064403 = -114320393;    int LjukqduWAg4888204 = -897499483;    int LjukqduWAg78556387 = -213615189;    int LjukqduWAg20282004 = -937355744;    int LjukqduWAg96225462 = -658117699;    int LjukqduWAg18009672 = -829015296;    int LjukqduWAg34734264 = -212291454;    int LjukqduWAg41582406 = -197693649;    int LjukqduWAg57834093 = -946766344;    int LjukqduWAg61627749 = -125234189;    int LjukqduWAg6157495 = -513328064;    int LjukqduWAg79423950 = -965650375;    int LjukqduWAg77330482 = -891311881;    int LjukqduWAg44822861 = -674266369;     LjukqduWAg48920675 = LjukqduWAg38172739;     LjukqduWAg38172739 = LjukqduWAg22800292;     LjukqduWAg22800292 = LjukqduWAg26829620;     LjukqduWAg26829620 = LjukqduWAg9891974;     LjukqduWAg9891974 = LjukqduWAg38200843;     LjukqduWAg38200843 = LjukqduWAg90783353;     LjukqduWAg90783353 = LjukqduWAg59927818;     LjukqduWAg59927818 = LjukqduWAg75464239;     LjukqduWAg75464239 = LjukqduWAg97988466;     LjukqduWAg97988466 = LjukqduWAg98021621;     LjukqduWAg98021621 = LjukqduWAg68501445;     LjukqduWAg68501445 = LjukqduWAg48790247;     LjukqduWAg48790247 = LjukqduWAg39946973;     LjukqduWAg39946973 = LjukqduWAg35095350;     LjukqduWAg35095350 = LjukqduWAg26202270;     LjukqduWAg26202270 = LjukqduWAg21356857;     LjukqduWAg21356857 = LjukqduWAg20629155;     LjukqduWAg20629155 = LjukqduWAg44962868;     LjukqduWAg44962868 = LjukqduWAg35769055;     LjukqduWAg35769055 = LjukqduWAg13550203;     LjukqduWAg13550203 = LjukqduWAg37487802;     LjukqduWAg37487802 = LjukqduWAg39180806;     LjukqduWAg39180806 = LjukqduWAg67022516;     LjukqduWAg67022516 = LjukqduWAg99419581;     LjukqduWAg99419581 = LjukqduWAg40286536;     LjukqduWAg40286536 = LjukqduWAg29481091;     LjukqduWAg29481091 = LjukqduWAg25008160;     LjukqduWAg25008160 = LjukqduWAg57114759;     LjukqduWAg57114759 = LjukqduWAg97081422;     LjukqduWAg97081422 = LjukqduWAg415378;     LjukqduWAg415378 = LjukqduWAg58171641;     LjukqduWAg58171641 = LjukqduWAg87506253;     LjukqduWAg87506253 = LjukqduWAg66684937;     LjukqduWAg66684937 = LjukqduWAg31714876;     LjukqduWAg31714876 = LjukqduWAg95217395;     LjukqduWAg95217395 = LjukqduWAg73787447;     LjukqduWAg73787447 = LjukqduWAg62144303;     LjukqduWAg62144303 = LjukqduWAg11187845;     LjukqduWAg11187845 = LjukqduWAg66553845;     LjukqduWAg66553845 = LjukqduWAg27402943;     LjukqduWAg27402943 = LjukqduWAg8970554;     LjukqduWAg8970554 = LjukqduWAg57806766;     LjukqduWAg57806766 = LjukqduWAg74903571;     LjukqduWAg74903571 = LjukqduWAg84672841;     LjukqduWAg84672841 = LjukqduWAg10607341;     LjukqduWAg10607341 = LjukqduWAg25655290;     LjukqduWAg25655290 = LjukqduWAg72783771;     LjukqduWAg72783771 = LjukqduWAg58935564;     LjukqduWAg58935564 = LjukqduWAg41137538;     LjukqduWAg41137538 = LjukqduWAg78381798;     LjukqduWAg78381798 = LjukqduWAg2544246;     LjukqduWAg2544246 = LjukqduWAg97107238;     LjukqduWAg97107238 = LjukqduWAg82618049;     LjukqduWAg82618049 = LjukqduWAg88128549;     LjukqduWAg88128549 = LjukqduWAg11432873;     LjukqduWAg11432873 = LjukqduWAg98991932;     LjukqduWAg98991932 = LjukqduWAg55777776;     LjukqduWAg55777776 = LjukqduWAg27410039;     LjukqduWAg27410039 = LjukqduWAg69605438;     LjukqduWAg69605438 = LjukqduWAg8719753;     LjukqduWAg8719753 = LjukqduWAg65775194;     LjukqduWAg65775194 = LjukqduWAg2813060;     LjukqduWAg2813060 = LjukqduWAg78382816;     LjukqduWAg78382816 = LjukqduWAg97573089;     LjukqduWAg97573089 = LjukqduWAg39849980;     LjukqduWAg39849980 = LjukqduWAg80995192;     LjukqduWAg80995192 = LjukqduWAg82105310;     LjukqduWAg82105310 = LjukqduWAg8232097;     LjukqduWAg8232097 = LjukqduWAg39877955;     LjukqduWAg39877955 = LjukqduWAg52414823;     LjukqduWAg52414823 = LjukqduWAg59212553;     LjukqduWAg59212553 = LjukqduWAg9441311;     LjukqduWAg9441311 = LjukqduWAg78409023;     LjukqduWAg78409023 = LjukqduWAg8366113;     LjukqduWAg8366113 = LjukqduWAg4579649;     LjukqduWAg4579649 = LjukqduWAg79681035;     LjukqduWAg79681035 = LjukqduWAg64277235;     LjukqduWAg64277235 = LjukqduWAg82349674;     LjukqduWAg82349674 = LjukqduWAg88812240;     LjukqduWAg88812240 = LjukqduWAg14631246;     LjukqduWAg14631246 = LjukqduWAg56697319;     LjukqduWAg56697319 = LjukqduWAg66072595;     LjukqduWAg66072595 = LjukqduWAg15977221;     LjukqduWAg15977221 = LjukqduWAg18699625;     LjukqduWAg18699625 = LjukqduWAg97871131;     LjukqduWAg97871131 = LjukqduWAg61064403;     LjukqduWAg61064403 = LjukqduWAg4888204;     LjukqduWAg4888204 = LjukqduWAg78556387;     LjukqduWAg78556387 = LjukqduWAg20282004;     LjukqduWAg20282004 = LjukqduWAg96225462;     LjukqduWAg96225462 = LjukqduWAg18009672;     LjukqduWAg18009672 = LjukqduWAg34734264;     LjukqduWAg34734264 = LjukqduWAg41582406;     LjukqduWAg41582406 = LjukqduWAg57834093;     LjukqduWAg57834093 = LjukqduWAg61627749;     LjukqduWAg61627749 = LjukqduWAg6157495;     LjukqduWAg6157495 = LjukqduWAg79423950;     LjukqduWAg79423950 = LjukqduWAg77330482;     LjukqduWAg77330482 = LjukqduWAg44822861;     LjukqduWAg44822861 = LjukqduWAg48920675;}
// Junk Finished

// Junk Code By Troll Face & Thaisen's Gen
void RpLOOJjsxr77690872() {     int DGbilUxrAy35842353 = -814917574;    int DGbilUxrAy47820477 = -631536795;    int DGbilUxrAy15496306 = -821046966;    int DGbilUxrAy54692814 = -796163586;    int DGbilUxrAy60621061 = -977863606;    int DGbilUxrAy51666520 = -86831600;    int DGbilUxrAy40829022 = -757665749;    int DGbilUxrAy42644167 = -224859454;    int DGbilUxrAy53849929 = -392739396;    int DGbilUxrAy71488852 = -895823292;    int DGbilUxrAy17702352 = -804818536;    int DGbilUxrAy38127923 = -740241213;    int DGbilUxrAy9416697 = -280654171;    int DGbilUxrAy83265082 = -371922615;    int DGbilUxrAy58808220 = -582049431;    int DGbilUxrAy61481429 = -406404822;    int DGbilUxrAy71197031 = -573191252;    int DGbilUxrAy49845912 = -820192022;    int DGbilUxrAy6122439 = -780005855;    int DGbilUxrAy92521883 = -341376305;    int DGbilUxrAy81015870 = -178037651;    int DGbilUxrAy76177761 = 9054972;    int DGbilUxrAy8827147 = -858368093;    int DGbilUxrAy43379466 = 33362251;    int DGbilUxrAy55384566 = -544090372;    int DGbilUxrAy34465436 = -185066680;    int DGbilUxrAy86371518 = -982840886;    int DGbilUxrAy25594710 = -481201323;    int DGbilUxrAy30017552 = -89908397;    int DGbilUxrAy76795061 = -47098073;    int DGbilUxrAy59535629 = -933785207;    int DGbilUxrAy62613671 = -68093761;    int DGbilUxrAy41779336 = -627277449;    int DGbilUxrAy96608034 = -723319826;    int DGbilUxrAy24493825 = -720769084;    int DGbilUxrAy28177611 = -398701895;    int DGbilUxrAy29756711 = 45696805;    int DGbilUxrAy34472680 = -363067727;    int DGbilUxrAy64419011 = 36948385;    int DGbilUxrAy94340920 = -185558998;    int DGbilUxrAy10564374 = -669472494;    int DGbilUxrAy93811479 = -891127414;    int DGbilUxrAy34256940 = -456481118;    int DGbilUxrAy93448250 = -4913342;    int DGbilUxrAy36657702 = -482059889;    int DGbilUxrAy13174263 = -278731714;    int DGbilUxrAy46305316 = 14692750;    int DGbilUxrAy75039714 = -141268635;    int DGbilUxrAy49161192 = -997053332;    int DGbilUxrAy33737118 = -174090076;    int DGbilUxrAy19739661 = -378178075;    int DGbilUxrAy7457288 = -286774467;    int DGbilUxrAy82248841 = -590771351;    int DGbilUxrAy37163019 = -762178394;    int DGbilUxrAy96675001 = -924703116;    int DGbilUxrAy59664591 = -723972546;    int DGbilUxrAy38993331 = -773168702;    int DGbilUxrAy72116840 = -754409217;    int DGbilUxrAy99308248 = -152073215;    int DGbilUxrAy26155626 = -692796927;    int DGbilUxrAy65295002 = -103990715;    int DGbilUxrAy15234313 = -176464426;    int DGbilUxrAy12626615 = -34951058;    int DGbilUxrAy77054868 = -245641324;    int DGbilUxrAy11953223 = -962038085;    int DGbilUxrAy55088680 = -636724776;    int DGbilUxrAy96348587 = -12963765;    int DGbilUxrAy12808662 = -557334345;    int DGbilUxrAy58771257 = -651153531;    int DGbilUxrAy30630609 = -83347537;    int DGbilUxrAy31724718 = -352101628;    int DGbilUxrAy36724352 = -110123526;    int DGbilUxrAy85426901 = -757140407;    int DGbilUxrAy11781519 = -494446858;    int DGbilUxrAy81957509 = -671903812;    int DGbilUxrAy87204391 = -286910237;    int DGbilUxrAy41920821 = -534463911;    int DGbilUxrAy15378896 = -753454752;    int DGbilUxrAy6721765 = -484577860;    int DGbilUxrAy42210304 = -165358658;    int DGbilUxrAy88160119 = -99759430;    int DGbilUxrAy11331804 = -741572251;    int DGbilUxrAy76433517 = -484147991;    int DGbilUxrAy96280434 = -915818322;    int DGbilUxrAy57055400 = -668919999;    int DGbilUxrAy52078342 = -547010740;    int DGbilUxrAy80364830 = -477322410;    int DGbilUxrAy4616317 = -865099056;    int DGbilUxrAy99933033 = -798616711;    int DGbilUxrAy64829234 = -996796538;    int DGbilUxrAy89184279 = -625533193;    int DGbilUxrAy57639871 = -199893978;    int DGbilUxrAy35164432 = -110994513;    int DGbilUxrAy38263385 = -270254689;    int DGbilUxrAy29045919 = 18431716;    int DGbilUxrAy95330061 = -393008068;    int DGbilUxrAy81184864 = -756176357;    int DGbilUxrAy57202072 = -110839794;    int DGbilUxrAy81495027 = -42875257;    int DGbilUxrAy81569021 = -814917574;     DGbilUxrAy35842353 = DGbilUxrAy47820477;     DGbilUxrAy47820477 = DGbilUxrAy15496306;     DGbilUxrAy15496306 = DGbilUxrAy54692814;     DGbilUxrAy54692814 = DGbilUxrAy60621061;     DGbilUxrAy60621061 = DGbilUxrAy51666520;     DGbilUxrAy51666520 = DGbilUxrAy40829022;     DGbilUxrAy40829022 = DGbilUxrAy42644167;     DGbilUxrAy42644167 = DGbilUxrAy53849929;     DGbilUxrAy53849929 = DGbilUxrAy71488852;     DGbilUxrAy71488852 = DGbilUxrAy17702352;     DGbilUxrAy17702352 = DGbilUxrAy38127923;     DGbilUxrAy38127923 = DGbilUxrAy9416697;     DGbilUxrAy9416697 = DGbilUxrAy83265082;     DGbilUxrAy83265082 = DGbilUxrAy58808220;     DGbilUxrAy58808220 = DGbilUxrAy61481429;     DGbilUxrAy61481429 = DGbilUxrAy71197031;     DGbilUxrAy71197031 = DGbilUxrAy49845912;     DGbilUxrAy49845912 = DGbilUxrAy6122439;     DGbilUxrAy6122439 = DGbilUxrAy92521883;     DGbilUxrAy92521883 = DGbilUxrAy81015870;     DGbilUxrAy81015870 = DGbilUxrAy76177761;     DGbilUxrAy76177761 = DGbilUxrAy8827147;     DGbilUxrAy8827147 = DGbilUxrAy43379466;     DGbilUxrAy43379466 = DGbilUxrAy55384566;     DGbilUxrAy55384566 = DGbilUxrAy34465436;     DGbilUxrAy34465436 = DGbilUxrAy86371518;     DGbilUxrAy86371518 = DGbilUxrAy25594710;     DGbilUxrAy25594710 = DGbilUxrAy30017552;     DGbilUxrAy30017552 = DGbilUxrAy76795061;     DGbilUxrAy76795061 = DGbilUxrAy59535629;     DGbilUxrAy59535629 = DGbilUxrAy62613671;     DGbilUxrAy62613671 = DGbilUxrAy41779336;     DGbilUxrAy41779336 = DGbilUxrAy96608034;     DGbilUxrAy96608034 = DGbilUxrAy24493825;     DGbilUxrAy24493825 = DGbilUxrAy28177611;     DGbilUxrAy28177611 = DGbilUxrAy29756711;     DGbilUxrAy29756711 = DGbilUxrAy34472680;     DGbilUxrAy34472680 = DGbilUxrAy64419011;     DGbilUxrAy64419011 = DGbilUxrAy94340920;     DGbilUxrAy94340920 = DGbilUxrAy10564374;     DGbilUxrAy10564374 = DGbilUxrAy93811479;     DGbilUxrAy93811479 = DGbilUxrAy34256940;     DGbilUxrAy34256940 = DGbilUxrAy93448250;     DGbilUxrAy93448250 = DGbilUxrAy36657702;     DGbilUxrAy36657702 = DGbilUxrAy13174263;     DGbilUxrAy13174263 = DGbilUxrAy46305316;     DGbilUxrAy46305316 = DGbilUxrAy75039714;     DGbilUxrAy75039714 = DGbilUxrAy49161192;     DGbilUxrAy49161192 = DGbilUxrAy33737118;     DGbilUxrAy33737118 = DGbilUxrAy19739661;     DGbilUxrAy19739661 = DGbilUxrAy7457288;     DGbilUxrAy7457288 = DGbilUxrAy82248841;     DGbilUxrAy82248841 = DGbilUxrAy37163019;     DGbilUxrAy37163019 = DGbilUxrAy96675001;     DGbilUxrAy96675001 = DGbilUxrAy59664591;     DGbilUxrAy59664591 = DGbilUxrAy38993331;     DGbilUxrAy38993331 = DGbilUxrAy72116840;     DGbilUxrAy72116840 = DGbilUxrAy99308248;     DGbilUxrAy99308248 = DGbilUxrAy26155626;     DGbilUxrAy26155626 = DGbilUxrAy65295002;     DGbilUxrAy65295002 = DGbilUxrAy15234313;     DGbilUxrAy15234313 = DGbilUxrAy12626615;     DGbilUxrAy12626615 = DGbilUxrAy77054868;     DGbilUxrAy77054868 = DGbilUxrAy11953223;     DGbilUxrAy11953223 = DGbilUxrAy55088680;     DGbilUxrAy55088680 = DGbilUxrAy96348587;     DGbilUxrAy96348587 = DGbilUxrAy12808662;     DGbilUxrAy12808662 = DGbilUxrAy58771257;     DGbilUxrAy58771257 = DGbilUxrAy30630609;     DGbilUxrAy30630609 = DGbilUxrAy31724718;     DGbilUxrAy31724718 = DGbilUxrAy36724352;     DGbilUxrAy36724352 = DGbilUxrAy85426901;     DGbilUxrAy85426901 = DGbilUxrAy11781519;     DGbilUxrAy11781519 = DGbilUxrAy81957509;     DGbilUxrAy81957509 = DGbilUxrAy87204391;     DGbilUxrAy87204391 = DGbilUxrAy41920821;     DGbilUxrAy41920821 = DGbilUxrAy15378896;     DGbilUxrAy15378896 = DGbilUxrAy6721765;     DGbilUxrAy6721765 = DGbilUxrAy42210304;     DGbilUxrAy42210304 = DGbilUxrAy88160119;     DGbilUxrAy88160119 = DGbilUxrAy11331804;     DGbilUxrAy11331804 = DGbilUxrAy76433517;     DGbilUxrAy76433517 = DGbilUxrAy96280434;     DGbilUxrAy96280434 = DGbilUxrAy57055400;     DGbilUxrAy57055400 = DGbilUxrAy52078342;     DGbilUxrAy52078342 = DGbilUxrAy80364830;     DGbilUxrAy80364830 = DGbilUxrAy4616317;     DGbilUxrAy4616317 = DGbilUxrAy99933033;     DGbilUxrAy99933033 = DGbilUxrAy64829234;     DGbilUxrAy64829234 = DGbilUxrAy89184279;     DGbilUxrAy89184279 = DGbilUxrAy57639871;     DGbilUxrAy57639871 = DGbilUxrAy35164432;     DGbilUxrAy35164432 = DGbilUxrAy38263385;     DGbilUxrAy38263385 = DGbilUxrAy29045919;     DGbilUxrAy29045919 = DGbilUxrAy95330061;     DGbilUxrAy95330061 = DGbilUxrAy81184864;     DGbilUxrAy81184864 = DGbilUxrAy57202072;     DGbilUxrAy57202072 = DGbilUxrAy81495027;     DGbilUxrAy81495027 = DGbilUxrAy81569021;     DGbilUxrAy81569021 = DGbilUxrAy35842353;}
// Junk Finished
