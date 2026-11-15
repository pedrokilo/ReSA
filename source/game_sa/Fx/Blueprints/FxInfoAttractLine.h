#pragma once

#include "FxInfo.h"
#include "FxInterpInfo32.h"

class NOTSA_EXPORT_VTABLE FxInfoAttractLine_c : public FxInfo_c {
protected:
    FxInterpInfo32_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoAttractLine_c();
    ~FxInfoAttractLine_c() override = default; // 0x4A7040
    void DestructorBody(); // 0x4A7040 (lote 217A, sin validar): cuerpo del destructor del EXE
    FxInfoAttractLine_c* ScalarDeletingDestructor(uint8 flags); // 0x4A7020 (lote 217A, sin validar): ranura 0 de la vtable del EXE, `ret 4`

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;

private:
    FxInfoAttractLine_c* Constructor() { this->FxInfoAttractLine_c::FxInfoAttractLine_c(); return this; } // 0x004A57C0 (lote 215A, sin validar): constructor del EXE, thiscall sin argumentos, devuelve this
};
