#pragma once

#include "FxInfo.h"
#include "FxInterpInfoU255.h"

class NOTSA_EXPORT_VTABLE FxInfoUnderwater_c : public FxInfo_c {
protected:
    FxInterpInfoU255_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoUnderwater_c();
    ~FxInfoUnderwater_c() override = default; // 0x4A7570
    void DestructorBody(); // 0x4A7570 (lote 217A, sin validar): cuerpo del destructor del EXE
    FxInfoUnderwater_c* ScalarDeletingDestructor(uint8 flags); // 0x4A7550 (lote 217A, sin validar): ranura 0 de la vtable del EXE, `ret 4`

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoUnderwater_c* Constructor(); // 0x4A5E20 (lote 216A)
};
