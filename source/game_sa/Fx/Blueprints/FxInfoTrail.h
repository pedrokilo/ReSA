#pragma once

#include "FxInfo.h"
#include "FxInterpInfoU255.h"

class NOTSA_EXPORT_VTABLE FxInfoTrail_c : public FxInfo_c {
protected:
    FxInterpInfoU255_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoTrail_c();
    ~FxInfoTrail_c() override = default; // 0x4A77A0

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoTrail_c* Constructor(); // 0x4A6220 (lote 216A)
};
