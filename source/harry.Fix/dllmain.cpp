#include <iostream>
#include <Windows.h>

#include "stdafx.h"
#include "IniReader.h"

namespace harryFix {
    hook::pattern res;
    uint32_t resX, resY;

    void readIniSettings() {
        CIniReader iniReader("");

        resX = iniReader.ReadInteger("MAIN", "resX", -1);
        resY = iniReader.ReadInteger("MAIN", "resY", -1);
    }

    void applyResolution() {
        if (!resX || !resY) {
            std::tie(resX, resY) = GetDesktopRes();
        }
        else if (resX == -1 || resY == -1) {
            return;
        }

        hook::pattern res = hook::pattern("C7 45 80 80 02 00 00 C7 45 84 E0 01 00 00"), res2 = hook::pattern("C7 45 88 78 00 00 00 33 C0 89");

        injector::WriteMemory(res.count(1).get(0).get<uint32_t>(3), resX, true); //0x64C948 + 0x3
        injector::WriteMemory(res.count(1).get(0).get<uint32_t>(10), resY, true); //0x64C94F + 0x3
        injector::WriteMemory(res2.count(1).get(0).get<uint32_t>(3), 168, true); //0x64C955 + 0x3
    }

    void applyIniSettings() {
        applyResolution();
    }

    void Init() {
        /*
        AllocConsole();
        freopen("CONIN$", "r", stdin);
        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        SetConsoleTitleA("Harry Potter fix");
        */

        readIniSettings();
        applyIniSettings();
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    if (reason == DLL_PROCESS_ATTACH) {
        std::call_once(CallbackHandler::flag, []() {
            CallbackHandler::RegisterCallback(harryFix::Init, hook::pattern("8B 35 9C 33 71 00"));
        });
    }
    return TRUE;
}
