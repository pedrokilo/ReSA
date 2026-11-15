#pragma once

#include "FxInfo.h"
#include "FxInterpInfo32.h"

class NOTSA_EXPORT_VTABLE FxInfoSize_c : public FxInfo_c {
protected:
    FxInterpInfo32_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoSize_c();
    ~FxInfoSize_c() override = default; // 0x4A7650
    void DestructorBody(); // 0x4A7650 (lote 217A, sin validar): cuerpo del destructor del EXE
    FxInfoSize_c* ScalarDeletingDestructor(uint8 flags); // 0x4A7630 (lote 217A, sin validar): ranura 0 de la vtable del EXE, `ret 4`

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float delta, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoSize_c* Constructor(); // 0x4A5FA0 (lote 216A)
};
