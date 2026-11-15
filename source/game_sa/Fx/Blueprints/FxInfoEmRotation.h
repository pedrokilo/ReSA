#pragma once

#include "FxInfo.h"
#include "FxInterpInfoFloat.h"

class NOTSA_EXPORT_VTABLE FxInfoEmRotation_c : public FxInfo_c {
protected:
    FxInterpInfoFloat_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoEmRotation_c();
    ~FxInfoEmRotation_c() override = default;
    void DestructorBody(); // 0x4A6E10 (lote 217A, sin validar): cuerpo del destructor del EXE
    FxInfoEmRotation_c* ScalarDeletingDestructor(uint8 flags); // 0x4A6DF0 (lote 217A, sin validar): ranura 0 de la vtable del EXE, `ret 4`

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;

private:
    FxInfoEmRotation_c* Constructor() { this->FxInfoEmRotation_c::FxInfoEmRotation_c(); return this; } // 0x004A51B0 (lote 215A, sin validar): constructor del EXE, thiscall sin argumentos, devuelve this
};
