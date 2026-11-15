#pragma once

#include "FxInfo.h"
#include "FxInterpInfo32.h"

class NOTSA_EXPORT_VTABLE FxInfoEmSize_c : public FxInfo_c {
protected:
    FxInterpInfo32_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoEmSize_c();
    ~FxInfoEmSize_c() override = default;

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    void Destructor(); // 0x4A6B00 (lote 216A)
    FxInfoEmSize_c* ScalarDeletingDestructor(uint8 flags); // 0x4A6AE0 (lote 216A)

private:
    FxInfoEmSize_c* Constructor() { this->FxInfoEmSize_c::FxInfoEmSize_c(); return this; } // 0x004A4C20 (lote 215A, sin validar): constructor del EXE, thiscall sin argumentos, devuelve this
};
