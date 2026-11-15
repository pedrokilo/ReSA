#pragma once

#include "FxInterpInfo.h"

class NOTSA_EXPORT_VTABLE FxInterpInfo32_c : public FxInterpInfo_c {
protected:
    int16** m_Keys;

public:
    static void InjectHooks();

    FxInterpInfo32_c();
    ~FxInterpInfo32_c() = default; // 0x4A89B0

    void Load(FILESTREAM file) override;

    void Allocate(int32 count);
    void GetVal(float* outValues, float delta);
private:
    FxInterpInfo32_c* Constructor(); // 0x4A8990 (lote 218A)
    void Destructor(); // 0x4A89B0 (lote 218A)
    FxInterpInfo32_c* ScalarDeletingDestructor(uint8 flags); // 0x4A8D70 (lote 218A)
};
