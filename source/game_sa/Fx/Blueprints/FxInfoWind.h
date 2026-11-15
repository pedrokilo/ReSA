#pragma once

#include "FxInfo.h"
#include "FxInterpInfo32.h"

class NOTSA_EXPORT_VTABLE FxInfoWind_c : public FxInfo_c {
protected:
    FxInterpInfo32_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoWind_c();
    ~FxInfoWind_c() override = default;
    void DestructorBody(); // 0x4A73B0 (lote 217A, sin validar): cuerpo del destructor del EXE
    FxInfoWind_c* ScalarDeletingDestructor(uint8 flags); // 0x4A7390 (lote 217A, sin validar): ranura 0 de la vtable del EXE, `ret 4`

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoWind_c* Constructor(); // 0x4A5A10 (lote 216A)
};
