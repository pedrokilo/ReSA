#pragma once

#include "FxInfo.h"
#include "FxInterpInfo32.h"

class NOTSA_EXPORT_VTABLE FxInfoForce_c : public FxInfo_c {
protected:
    FxInterpInfo32_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoForce_c();
    ~FxInfoForce_c() override = default; // 0x4A6EF0
    void DestructorBody(); // 0x4A6EF0 (lote 217A, sin validar): cuerpo del destructor del EXE
    FxInfoForce_c* ScalarDeletingDestructor(uint8 flags); // 0x4A6ED0 (lote 217A, sin validar): ranura 0 de la vtable del EXE, `ret 4`

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
};
