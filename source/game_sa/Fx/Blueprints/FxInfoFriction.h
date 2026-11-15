#pragma once

#include "FxInfo.h"
#include "FxInterpInfo32.h"

class NOTSA_EXPORT_VTABLE FxInfoFriction_c : public FxInfo_c {
protected:
    FxInterpInfo32_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoFriction_c();
    ~FxInfoFriction_c() override = default; // 0x4A6F60
    void DestructorBody(); // 0x4A6F60 (lote 217A, sin validar): cuerpo del destructor del EXE
    FxInfoFriction_c* ScalarDeletingDestructor(uint8 flags); // 0x4A6F40 (lote 217A, sin validar): ranura 0 de la vtable del EXE, `ret 4`

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;

private:
    FxInfoFriction_c* Constructor() { this->FxInfoFriction_c::FxInfoFriction_c(); return this; } // 0x004A5550 (lote 215A, sin validar): constructor del EXE, thiscall sin argumentos, devuelve this
};
