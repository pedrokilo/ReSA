#pragma once

#include "FxInfo.h"
#include "FxInterpInfoU255.h"

class NOTSA_EXPORT_VTABLE FxInfoColour_c : public FxInfo_c {
protected:
    FxInterpInfoU255_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoColour_c();
    ~FxInfoColour_c() override = default; // 0x4A75E0

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoColour_c* Constructor(); // 0x4A5EA0 (lote 216A)
};
