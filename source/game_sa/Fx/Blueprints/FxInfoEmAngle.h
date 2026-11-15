#pragma once

#include "FxInfo.h"
#include "FxInterpInfoFloat.h"

class NOTSA_EXPORT_VTABLE FxInfoEmAngle_c : public FxInfo_c {
protected:
    FxInterpInfoFloat_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoEmAngle_c();
    ~FxInfoEmAngle_c() override = default; // 0x4A6C50

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    void Destructor(); // 0x4A6C50 (lote 216A)
    FxInfoEmAngle_c* ScalarDeletingDestructor(uint8 flags); // 0x4A6C30 (lote 216A)

private:
    FxInfoEmAngle_c* Constructor() { this->FxInfoEmAngle_c::FxInfoEmAngle_c(); return this; } // 0x004A4E90 (lote 215A, sin validar): constructor del EXE, thiscall sin argumentos, devuelve this
};
