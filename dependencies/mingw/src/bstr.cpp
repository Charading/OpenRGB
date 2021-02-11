#include <windows.h>
#include <strsafe.h>
#include <comdef.h>

#ifdef __MINGW32__
BSTR _com_util::ConvertStringToBSTR(const char* pSrc)
{
    if(!pSrc) return NULL;
    int size_needed;
    BSTR strTo(NULL);
    if((size_needed = MultiByteToWideChar(CP_ACP, 0, pSrc, -1, NULL, 0)))
    {
        size_needed--; // Size sans NULL terminator
        strTo = SysAllocStringLen(NULL, size_needed);
        if(strTo)
        {
            if(!MultiByteToWideChar(CP_ACP, 0, pSrc, -1, strTo, size_needed))
            {
                if(ERROR_INSUFFICIENT_BUFFER == GetLastError())
                    return strTo;
                SysFreeString(strTo);
                strTo = NULL;
            }
        }
    };
    return strTo;
};

#endif
