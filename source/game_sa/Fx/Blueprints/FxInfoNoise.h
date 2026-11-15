#pragma once

#include "FxInfo.h"
#include "FxInterpInfo32.h"

class NOTSA_EXPORT_VTABLE FxInfoNoise_c : public FxInfo_c {
protected:
    FxInterpInfo32_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoNoise_c();
    ~FxInfoNoise_c() override = default;
    void DestructorBody(); // 0x4A6E80 (lote 217A, sin validar): cuerpo del destructor del EXE
    FxInfoNoise_c* ScalarDeletingDestructor(uint8 flags); // 0x4A6E60 (lote 217A, sin validar): ranura 0 de la vtable del EXE, `ret 4`

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;

private:
    FxInfoNoise_c* Constructor() { this->FxInfoNoise_c::FxInfoNoise_c(); return this; } // 0x004A5270 (lote 215A, sin validar): constructor del EXE, thiscall sin argumentos, devuelve this
};
