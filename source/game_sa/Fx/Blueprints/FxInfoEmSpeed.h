#pragma once

#include "FxInfo.h"
#include "FxInterpInfo32.h"

class NOTSA_EXPORT_VTABLE FxInfoEmSpeed_c : public FxInfo_c {
protected:
    FxInterpInfo32_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoEmSpeed_c();
    ~FxInfoEmSpeed_c() override = default; // 0x4A6B70

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    void Destructor(); // 0x4A6B70 (lote 216A)
    FxInfoEmSpeed_c* ScalarDeletingDestructor(uint8 flags); // 0x4A6B50 (lote 216A)

private:
    FxInfoEmSpeed_c* Constructor() { this->FxInfoEmSpeed_c::FxInfoEmSpeed_c(); return this; } // 0x004A4D00 (lote 215A, sin validar): constructor del EXE, thiscall sin argumentos, devuelve this
};
