#pragma once

#include "FxInfo.h"
#include "FxInterpInfoU255.h"

class NOTSA_EXPORT_VTABLE FxInfoEmLife_c : public FxInfo_c {
protected:
    FxInterpInfoU255_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoEmLife_c();
    ~FxInfoEmLife_c() override = default; // 0x4A6CC0

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    void Destructor(); // 0x4A6CC0 (lote 216A)
    FxInfoEmLife_c* ScalarDeletingDestructor(uint8 flags); // 0x4A6CA0 (lote 216A)

private:
    FxInfoEmLife_c* Constructor() { this->FxInfoEmLife_c::FxInfoEmLife_c(); return this; } // 0x004A4F50 (lote 215A, sin validar): constructor del EXE, thiscall sin argumentos, devuelve this
};
