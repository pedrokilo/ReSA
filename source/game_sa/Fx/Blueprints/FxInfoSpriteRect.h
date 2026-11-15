#pragma once

#include "FxInfo.h"
#include "FxInterpInfo255.h"

class NOTSA_EXPORT_VTABLE FxInfoSpriteRect_c : public FxInfo_c {
protected:
    FxInterpInfo255_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoSpriteRect_c();
    ~FxInfoSpriteRect_c() override = default;

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoSpriteRect_c* Constructor(); // 0x4A60B0 (lote 216A)
};
