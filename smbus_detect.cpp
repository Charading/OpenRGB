/* Filename		smbus_detect.cpp
 * Author		Stavros Avramidis
 * Description	Detects ports of the SMBUS via Windows queries
 * Date			31/1/2019
 */

#include "smbus_detect.h"

// Convert a wide Unicode tring to an UTF8 string
std::string utf8_encode(const std::wstring &wstr) {
  // Taken from https://stackoverflow.com/questions/215963/
  if (wstr.empty())
    return std::string();
  int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(),
                                        NULL, 0, NULL, NULL);
  std::string strTo(size_needed, 0);
  WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0],
                      size_needed, NULL, NULL);
  return strTo;
}

int detectSMBUS(DetectSMBUHandle& handle) {
  HRESULT hres;

  // Step 1: --------------------------------------------------
  // Initialize COM. ------------------------------------------

  hres = CoInitializeEx(0, COINIT_MULTITHREADED);
  if (FAILED(hres)) {
    std::cout << "Failed to initialize COM library. Error code = 0x" << std::hex
              << hres << std::endl;
    return 1; // Program has failed.
  }

  // Step 2: --------------------------------------------------
  // Set general COM security levels --------------------------

  hres =
      CoInitializeSecurity(NULL,
                           -1,                        // COM authentication
                           NULL,                      // Authentication services
                           NULL,                      // Reserved
                           RPC_C_AUTHN_LEVEL_DEFAULT, // Default authentication
                           RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation
                           NULL,                        // Authentication info
                           EOAC_NONE, // Additional capabilities
                           NULL       // Reserved
      );

  if (FAILED(hres)) {
    std::cout << "Failed to initialize security. Error code = 0x" << std::hex
              << hres << std::endl;
    CoUninitialize();
    return 1; // Program has failed.
  }

  // Step 3: ---------------------------------------------------
  // Obtain the initial locator to WMI -------------------------

  IWbemLocator *pLoc = NULL;

  hres = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER,
                          IID_IWbemLocator, (LPVOID *)&pLoc);

  if (FAILED(hres)) {
    std::cout << "Failed to create IWbemLocator object."
              << " Err code = 0x" << std::hex << hres << std::endl;
    CoUninitialize();
    return 1; // Program has failed.
  }

  // Step 4: -----------------------------------------------------
  // Connect to WMI through the IWbemLocator::ConnectServer method

  IWbemServices *pSvc = NULL;

  // Connect to the root\cimv2 namespace with
  // the current user and obtain pointer pSvc
  // to make IWbemServices calls.
  hres = pLoc->ConnectServer(
      _bstr_t(L"ROOT\\CIMV2"), // Object path of WMI namespace
      NULL,                    // User name. NULL = current user
      NULL,                    // User password. NULL = current
      0,                       // Locale. NULL indicates current
      NULL,                    // Security flags.
      0,                       // Authority (for example, Kerberos)
      0,                       // Context object
      &pSvc                    // pointer to IWbemServices proxy
  );

  if (FAILED(hres)) {
    std::cout << "Could not connect. Error code = 0x" << std::hex << hres
              << std::endl;
    pLoc->Release();
    CoUninitialize();
    return 1; // Program has failed.
  }

  // Step 5: --------------------------------------------------
  // Set security levels on the proxy -------------------------

  hres = CoSetProxyBlanket(pSvc,                        // Indicates the proxy to set
                           RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx
                           RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx
                           NULL,                        // Server principal name
                           RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx
                           RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
                           NULL,                        // client identity
                           EOAC_NONE                    // proxy capabilities
  );

  if (FAILED(hres)) {
    std::cout << "Could not set proxy blanket. Error code = 0x" << std::hex
              << hres << std::endl;
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
    return 1; // Program has failed.
  }

  // Step 6: --------------------------------------------------
  // Use the IWbemServices pointer to make requests of WMI ----

  // For example, get the name of the operating system
  IEnumWbemClassObject *pEnumerator = NULL;
  hres = pSvc->ExecQuery(bstr_t("WQL"),
                         bstr_t("SELECT * FROM Win32_PnPSignedDriver WHERE "
                                "DeviceName LIKE '\%SMBUS%'"),
                         WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
                         NULL, &pEnumerator);

  if (FAILED(hres)) {
    std::cout << "Query failed."
              << " Error code = 0x" << std::hex << hres << std::endl;
    pSvc->Release();
    pLoc->Release();
    CoUninitialize();
    return 1; // Program has failed.
  }

  // Step 7: -------------------------------------------------
  // Get the data from the query in step 6 -------------------

  IWbemClassObject *pclsObj = NULL;
  ULONG uReturn = 0;

  while (pEnumerator) {
    HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

    if (0 == uReturn) {
      break;
    }

    VARIANT vtProp;
    // Get the value of the Name property
    hr = pclsObj->Get(L"DeviceName", 0, &vtProp, 0, 0);
    auto name = utf8_encode(vtProp.bstrVal);
    hr = pclsObj->Get(L"Location", 0, &vtProp, 0, 0);

    // Get bus location parameters with RegEx
    std::regex rgx("PCI bus (\\d+), device (\\d+), function (\\d+)");
    std::smatch matches;
    std::string loc = utf8_encode(vtProp.bstrVal);

    if (std::regex_search(loc, matches, rgx)) {
      if (matches.size() == 4) {
        handle.name = new char[name.length()+1];
        strcpy(handle.name, name.c_str());
        handle.pci_bus = std::stoi(matches[1]);
        handle.deviceID = std::stoi(matches[2]);
        handle.function = std::stoi(matches[3]);
      }
    } else {
      std::cout << "Match not found for \'" << loc << "\'\n";
    }

    VariantClear(&vtProp);

    pclsObj->Release();
  }

_EXIT:
  // Cleanup
  pSvc->Release();
  pLoc->Release();
  pEnumerator->Release();
  CoUninitialize();
  return 0; // Program successfully completed.
}

int main() {
  auto bus = DetectSMBUHandle();
  if (!detectSMBUS(bus)){
    printf("%s\n", bus.name);
    printf("PCI BUS:%4d | Device ID:%4d | Function:%4d |", bus.pci_bus, bus.deviceID,bus.function);

  }else{
    printf("There was an error detecting SMBUS");
  }

  // std::cin.get();
  return 0;
}
