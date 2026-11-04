#include "StdInc.h"

#include "RGBA.h"

void CRGBA::InjectHooks() {
    RH_ScopedClass(CRGBA);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(Constructor, 0x7170C0);
}

// 0x7170C0 (thiscall, `ret 10h`): CRGBA::CRGBA(r, g, b, a); escribe los cuatro bytes y devuelve this en EAX (ECX y EDX son de trabajo en el EXE).
CRGBA* CRGBA::Constructor(uint8 r, uint8 g, uint8 b, uint8 a) {
    this->CRGBA::CRGBA(r, g, b, a);
    return this;
}
