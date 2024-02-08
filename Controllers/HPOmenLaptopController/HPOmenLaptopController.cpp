#include "HPOmenLaptopController.h"
#include <comdef.h>
#include <Wbemidl.h>

// #pragma comment(lib, "wbemuuid.lib")

HPOmenLaptopController::HPOmenLaptopController() {}
HPOmenLaptopController::~HPOmenLaptopController() {}

int HPOmenLaptopController::execute(int command, int commandType, int inputDataSize, BYTE* inputData, int* returnDataSize, BYTE** returnData)
{
    /*---------------------------------------------------------*\
    | Talk to WMI                                               |
    \*---------------------------------------------------------*/

    // magic constant
    static const BYTE Sign[4] = { 83, 69, 67, 85 };

    // will hold the return codes from all the calls to WMI
    HRESULT hres;

    // initialize COM interface
    hres = CoInitializeEx(0, COINIT_MULTITHREADED);
    if (FAILED(hres))
    {
        return 1;
    }

    // initialize WMI security
    hres = CoInitializeSecurity(
        NULL,
        -1,   // COM negotiates service
        NULL, // Authentication services
        NULL, // Reserved
        RPC_C_AUTHN_LEVEL_DEFAULT,   // authentication
        RPC_C_IMP_LEVEL_IMPERSONATE, // Impersonation
        NULL,      // Authentication info
        EOAC_NONE, // Additional capabilities
        NULL       // Reserved
        );

    if (FAILED(hres))
    {
        CoUninitialize();
        return 1;
    }

    // obtain the initial locator to the Windows Management Instrumentation
    IWbemLocator* pLoc = nullptr;
    hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*) &pLoc );
    if (FAILED(hres))
    {
        CoUninitialize();
        return 1;
    }

    IWbemServices* pSvc = nullptr;
    hres = pLoc->ConnectServer(_bstr_t(L"ROOT\\WMI"), NULL, NULL, 0, NULL, 0, 0, &pSvc);
    if (FAILED(hres))
    {
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    hres = CoSetProxyBlanket(
        pSvc,                        // Indicates the proxy to set
        RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
        RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
        NULL,                        // Server principal name
        RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
        RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
        NULL,                        // client identity
        EOAC_NONE                    // proxy capabilities
        );

    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    /*---------------------------------------------------------*\
    | Get all the required custom hp wmi obejcts                |
    \*---------------------------------------------------------*/

    IWbemClassObject* classObject = nullptr;
    hres = pSvc->GetObject(_bstr_t(L"hpqBIntM"), 0, NULL, &classObject, NULL);
    if (FAILED(hres))
    {
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    IWbemClassObject* methodParameters = nullptr;
    hres = classObject->GetMethod(L"hpqBIOSInt128", 0, &methodParameters, NULL);
    if (FAILED(hres))
    {
        classObject->Release();
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    IWbemClassObject* dataInClass = nullptr;
    hres = pSvc->GetObject(_bstr_t(L"hpqBDataIn"), 0, NULL, &dataInClass, NULL);
    if (FAILED(hres))
    {
        methodParameters->Release();
        classObject->Release();
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    IWbemCallResult* callResult = nullptr;
    hres = pSvc->GetObject(_bstr_t(L"hpqBDataOut128"), 0, NULL, NULL, &callResult);
    if (FAILED(hres))
    {
        dataInClass->Release();
        methodParameters->Release();
        classObject->Release();
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    /*---------------------------------------------------------*\
    | Populate the input parameters                             |
    \*---------------------------------------------------------*/

    // Sign
    VARIANT signVar;
    VariantInit(&signVar);
    signVar.vt = VT_UI1 | VT_ARRAY;
    SAFEARRAYBOUND safeArrayBound = { 4, 0 };
    signVar.parray = SafeArrayCreate(VT_UI1, 1, &safeArrayBound);
    SafeArrayLock(signVar.parray);
    memcpy(signVar.parray->pvData, Sign, sizeof(Sign));
    SafeArrayUnlock(signVar.parray);
    dataInClass->Put(L"Sign", 0, &signVar, 0);
    VariantClear(&signVar);

    // Command
    VARIANT commandVar;
    VariantInit(&commandVar);
    commandVar.vt = VT_I4;
    commandVar.lVal = command;
    dataInClass->Put(L"Command", 0, &commandVar, 0);
    VariantClear(&commandVar);

    // CommandType
    VARIANT commandTypeVar;
    VariantInit(&commandTypeVar);
    commandTypeVar.vt = VT_I4;
    commandTypeVar.lVal = commandType;
    dataInClass->Put(L"CommandType", 0, &commandTypeVar, 0);
    VariantClear(&commandTypeVar);

    // Size
    VARIANT sizeVar;
    VariantInit(&sizeVar);
    sizeVar.vt = VT_I4;
    sizeVar.lVal = inputDataSize;
    dataInClass->Put(L"Size", 0, &sizeVar, 0);

    // hpqBData
    VARIANT hpqBDataVar;
    VariantInit(&hpqBDataVar);
    hpqBDataVar.vt = VT_UI1 | VT_ARRAY;
    SAFEARRAYBOUND safeArrayBoundData = { static_cast<ULONG>(inputDataSize), 0 };
    hpqBDataVar.parray = SafeArrayCreate(VT_UI1, 1, &safeArrayBoundData);
    SafeArrayLock(hpqBDataVar.parray);
    memcpy(hpqBDataVar.parray->pvData, inputData, inputDataSize);
    SafeArrayUnlock(hpqBDataVar.parray);
    dataInClass->Put(L"hpqBData", 0, &hpqBDataVar, 0);
    VariantClear(&hpqBDataVar);

    /*-----------------------------------------------------------*\
    | Fill the 'InData' parameter from the 'hpqBIOSInt128' method |
    \*-----------------------------------------------------------*/

    // InData
    VARIANT inDataVar;
    VariantInit(&inDataVar);
    inDataVar.vt = VT_UNKNOWN;
    inDataVar.punkVal = dataInClass;
    methodParameters->Put(L"InData", 0, &inDataVar, 0);
    VariantClear(&inDataVar);

    /*-----------------------------------------------------------*\
    | Call the 'hpqBIOSInt128' method from the 'hpqBIntM' class   |
    \*-----------------------------------------------------------*/

    hres = pSvc->ExecMethod(_bstr_t(L"hpqBIntM.InstanceName='ACPI\\PNP0C14\\0_0'"), _bstr_t(L"hpqBIOSInt128"), 0, NULL, methodParameters, NULL, &callResult);
    if (FAILED(hres))
    {
        if (callResult) callResult->Release();
        methodParameters->Release();
        classObject->Release();
        pSvc->Release();
        pLoc->Release();
        CoUninitialize();
        return 1;
    }

    /*-----------------------------------------------------------*\
    | Get the returned data                                       |
    \*-----------------------------------------------------------*/

    if (returnDataSize != NULL && returnData != NULL)
    {
        IWbemClassObject* ppResultObject = nullptr;
        callResult->GetResultObject(WBEM_INFINITE, &ppResultObject);

        // get OutData from object returned (is an object of type hpqBDataOut128)
        VARIANT outDataVar;
        VariantInit(&outDataVar);
        ppResultObject->Get(L"OutData", 0, &outDataVar, NULL, NULL);

        // get 'Data' property from the object returned
        IWbemClassObject* retData = (IWbemClassObject*)outDataVar.punkVal;
        retData->Get(L"Data", 0, &outDataVar, NULL, NULL);

        // extract the byte array from the result VARIANT
        long lower, upper;
        SAFEARRAY* safeArray = outDataVar.parray;
        SafeArrayGetLBound(safeArray, 1, &lower);
        SafeArrayGetUBound(safeArray, 1, &upper);
        long length = upper - lower + 1;
        *returnData = new BYTE[length];
        *returnDataSize = length;
        SafeArrayLock(safeArray);
        memcpy(*returnData, safeArray->pvData, length);
        SafeArrayUnlock(safeArray);

        // cleanup
        retData->Release();
        VariantClear(&outDataVar);
        ppResultObject->Release();
    }

    // cleanup
    callResult->Release();
    methodParameters->Release();
    classObject->Release();
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
    return 0;
}

void HPOmenLaptopController::setZoneColors(std::vector<RGBColor>& colors) {
    /*---------------------------------------------------------*\
    | Set new colors for each zone                              |
    \*---------------------------------------------------------*/

    int returnDataSize = 0;
    BYTE* returnData = nullptr;
    int num = execute(131081, 2, 0, nullptr, &returnDataSize, &returnData);

    if (num == 0 && returnData != nullptr)
    {
        // prepare the data byte array to be sent to WMI
        for (int i = 0; i < 4; i++)
        {
            returnData[25 + i * 3]     = RGBGetRValue(colors[i]);
            returnData[25 + i * 3 + 1] = RGBGetGValue(colors[i]);
            returnData[25 + i * 3 + 2] = RGBGetBValue(colors[i]);
        }

        // make the WMI call to set the colors
        execute(131081, 3, returnDataSize, returnData, NULL, NULL);
        delete[] returnData;
    }
}

bool HPOmenLaptopController::isLightingSupported() {
    /*---------------------------------------------------------*\
    | Check if the laptop supports rgb lighting                 |
    \*---------------------------------------------------------*/

    BYTE b = 0;
    int returnDataSize = 0;
    BYTE* returnData = nullptr;
    if (execute(131081, 1, 0, nullptr, &returnDataSize, &returnData) == 0)
    {
        b = (BYTE)(returnData[0] & 1u);
    }

    delete[] returnData;
    return b == 1;
}

KeyboardType HPOmenLaptopController::getKeyboardType() {
    /*---------------------------------------------------------*\
    | Get keyboard type                                         |
    \*---------------------------------------------------------*/

    int returnDataSize = 0;
    BYTE* returnData = nullptr;
    if (execute(131080, 43, 0, nullptr, &returnDataSize, &returnData) == 0)
    {
        int result = returnData[0];
        delete[] returnData;
        return (KeyboardType)(result + 1);
    }

    return KeyboardType::INVALID;
}

void HPOmenLaptopController::changeMode(KeyboardMode mode) {
    /*---------------------------------------------------------*\
    | Change keyboard rgb mode                                  |
    \*---------------------------------------------------------*/

    switch (mode)
    {
    case KeyboardMode::OFF:
    {
        BYTE array[4] = { 100, 0, 0, 0 };
        execute(131081, 5, sizeof(array), array, NULL, NULL);
        break;
    }
    case KeyboardMode::DIRECT:
    {
        BYTE array[4] = { 228, 0, 0, 0 };
        HPOmenLaptopController::execute(131081, 5, sizeof(array), array, NULL, NULL);
        break;
    }
    default:
        break;
    }
}
