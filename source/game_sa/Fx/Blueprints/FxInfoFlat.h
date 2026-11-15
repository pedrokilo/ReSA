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
};
