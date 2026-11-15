#pragma once

#include "FxInfo.h"
#include "FxInterpInfoU255.h"

class NOTSA_EXPORT_VTABLE FxInfoColourRange_c : public FxInfo_c {
protected:
    FxInterpInfoU255_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoColourRange_c();
    ~FxInfoColourRange_c() override = default; // 0x4A7960

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoColourRange_c* Constructor(); // 0x4A6630 (lote 216A)
    void Destructor(); // 0x4A7960 (lote 218A)
    FxInfoColourRange_c* ScalarDeletingDestructor(uint8 flags); // 0x4A7940 (lote 218A)
};
