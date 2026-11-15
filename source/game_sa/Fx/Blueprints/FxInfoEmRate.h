#pragma once

#include "FxInfo.h"
#include "FxInterpInfoFloat.h"

class NOTSA_EXPORT_VTABLE FxInfoEmRate_c : public FxInfo_c {
protected:
    FxInterpInfoFloat_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoEmRate_c();
    ~FxInfoEmRate_c() override = default; // 0x4A6A90

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoEmRate_c* Constructor(); // 0x4A6A00 (lote 216A)
    void Destructor(); // 0x4A6A90 (lote 216A)
    FxInfoEmRate_c* ScalarDeletingDestructor(uint8 flags); // 0x4A6A70 (lote 216A)
};
