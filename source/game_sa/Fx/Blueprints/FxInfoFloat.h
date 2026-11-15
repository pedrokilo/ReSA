#pragma once

#include "FxInfo.h"
#include "FxInterpInfoFloat.h"

class NOTSA_EXPORT_VTABLE FxInfoFloat_c : public FxInfo_c {
protected:
    FxInterpInfoFloat_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoFloat_c();
    ~FxInfoFloat_c() override = default; // 0x4A7500
    void DestructorBody(); // 0x4A7500 (lote 217A, sin validar): cuerpo del destructor del EXE
    FxInfoFloat_c* ScalarDeletingDestructor(uint8 flags); // 0x4A74E0 (lote 217A, sin validar): ranura 0 de la vtable del EXE, `ret 4`

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoFloat_c* Constructor(); // 0x4A5DA0 (lote 216A)
};
