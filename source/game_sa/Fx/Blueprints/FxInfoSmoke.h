#pragma once

#include "FxInfo.h"
#include "FxInterpInfoU255.h"

class NOTSA_EXPORT_VTABLE FxInfoSmoke_c : public FxInfo_c {
protected:
    FxInterpInfoU255_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoSmoke_c();
    ~FxInfoSmoke_c() override = default; // 0x4A7AB0

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoSmoke_c* Constructor(); // 0x4A68E0 (lote 216A)
};
