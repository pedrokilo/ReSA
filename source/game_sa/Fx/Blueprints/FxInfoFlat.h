#pragma once

#include "FxInfo.h"
#include "FxInterpInfoFloat.h"

class NOTSA_EXPORT_VTABLE FxInfoFlat_c : public FxInfo_c {
protected:
    FxInterpInfoFloat_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoFlat_c();
    ~FxInfoFlat_c() override = default;

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoFlat_c* Constructor(); // 0x4A6320 (lote 216A)
    void Destructor(); // 0x4A7810 (lote 218A)
    FxInfoFlat_c* ScalarDeletingDestructor(uint8 flags); // 0x4A77F0 (lote 218A)
};
