#include "researcher.h"

std::string makeExceptionText(std::string text) {
    std::ostringstream ss;
    ss << text << "\n" << "error code: " << GetLastError();
    return ss.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "usage: <function name>" << std::endl;
        return 0;
    }

    std::string functionName = argv[1];

    try {
        if (!SymInitialize(
            GetCurrentProcess(), 0, 0)
            )
            throw std::runtime_error(
                makeExceptionText("sym initialize error")
            );

        DWORD64 moduleBase = SymLoadModuleEx(
            GetCurrentProcess(),
            0,
            "C:\\Windows\\System32\\ntdll.dll",
            0,
            0,
            0,
            0,
            0
        );
        if (!moduleBase)
            throw std::runtime_error(
                makeExceptionText("error while load current module")
            );

        SYMBOL_INFO_PACKAGE sip;
        ZeroMemory(&sip, sizeof sip);
        sip.si.SizeOfStruct = sizeof SYMBOL_INFO;
        sip.si.MaxNameLen = MAX_SYM_NAME;

        if (!SymFromName(
            GetCurrentProcess(), functionName.c_str(), &sip.si)
            )
            throw std::runtime_error(
                makeExceptionText("error while finding function")
            );

        //address
        std::cout << functionName << " " << "address: 0x" << std::hex << sip.si.Address << std::endl;

        //rva
        DWORD64 delta = sip.si.Address - moduleBase;
        std::cout << functionName << " " << "rva: 0x" << std::hex << delta << std::endl;

        std::getchar();
        SymCleanup(GetCurrentProcess());
    }
    catch (const std::exception& e) {
        std::cerr << "[nt-researcher] -> " << e.what() << std::endl;
    }

    return 0;
}