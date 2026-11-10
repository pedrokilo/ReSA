#pragma once

namespace Securom {
    void InjectHooks();
}

void SecuromDisplayInit();
void SecuromStateDisplay();
bool SecuromAnyPairPending();   // 0x744D50
bool SecuromFirstPairPending(); // 0x744D90
