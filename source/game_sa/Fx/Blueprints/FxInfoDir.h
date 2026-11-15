#pragma once

#include "FxInfo.h"
#include "FxInterpInfoFloat.h"

class NOTSA_EXPORT_VTABLE FxInfoDir_c : public FxInfo_c {
protected:
    FxInterpInfoFloat_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoDir_c();
    ~FxInfoDir_c() override = default; // 0x4A7880

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoDir_c* Constructor(); // 0x4A6440 (lote 216A)
};
