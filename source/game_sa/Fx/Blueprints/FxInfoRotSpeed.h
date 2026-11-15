#pragma once

#include "FxInfo.h"
#include "FxInterpInfoFloat.h"

class NOTSA_EXPORT_VTABLE FxInfoRotSpeed_c : public FxInfo_c {
protected:
    FxInterpInfoFloat_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoRotSpeed_c();
    ~FxInfoRotSpeed_c() override = default;

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoRotSpeed_c* Constructor(); // 0x4A5CB0 (lote 216A)
};
