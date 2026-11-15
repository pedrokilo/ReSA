#pragma once

#include "FxInfo.h"
#include "FxInterpInfo32.h"

class NOTSA_EXPORT_VTABLE FxInfoAnimTexture_c : public FxInfo_c {
protected:
    FxInterpInfo32_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoAnimTexture_c();
    ~FxInfoAnimTexture_c() override = default; // 0x4A78F0

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoAnimTexture_c* Constructor(); // 0x4A6550 (lote 216A)
};
