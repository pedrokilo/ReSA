#pragma once

#include "FxInfo.h"
#include "FxInterpInfoU255.h"

class NOTSA_EXPORT_VTABLE FxInfoSelfLit_c : public FxInfo_c {
protected:
    FxInterpInfoU255_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoSelfLit_c();
    ~FxInfoSelfLit_c() override = default; // 0x4A79D0

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;
private:
    FxInfoSelfLit_c* Constructor(); // 0x4A6760 (lote 216A)
    void Destructor(); // 0x4A79D0 (lote 218A)
    FxInfoSelfLit_c* ScalarDeletingDestructor(uint8 flags); // 0x4A79B0 (lote 218A)
};
