#pragma once

#include "FxInfo.h"
#include "FxInterpInfoFloat.h"

class NOTSA_EXPORT_VTABLE FxInfoEmPos_c : public FxInfo_c {
protected:
    FxInterpInfoFloat_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoEmPos_c();
    ~FxInfoEmPos_c() override = default; // 0x4A6D30
    void DestructorBody(); // 0x4A6D30 (lote 217A, sin validar): cuerpo del destructor del EXE

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    void Destructor(); // 0x4A6D30 (lote 216A)
    FxInfoEmPos_c* ScalarDeletingDestructor(uint8 flags); // 0x4A6D10 (lote 216A)
};
