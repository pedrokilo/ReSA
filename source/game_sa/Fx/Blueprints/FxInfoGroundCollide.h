#pragma once

#include "FxInfo.h"
#include "FxInterpInfo32.h"

class NOTSA_EXPORT_VTABLE FxInfoGroundCollide_c : public FxInfo_c {
protected:
    FxInterpInfo32_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoGroundCollide_c();
    ~FxInfoGroundCollide_c() override = default;
    void DestructorBody(); // 0x4A70B0 (lote 217A, sin validar): cuerpo del destructor del EXE
    FxInfoGroundCollide_c* ScalarDeletingDestructor(uint8 flags); // 0x4A7090 (lote 217A, sin validar): ranura 0 de la vtable del EXE, `ret 4`

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoGroundCollide_c* Constructor(); // 0x4A5980 (lote 216A)
};
