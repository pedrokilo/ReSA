#pragma once

#include "FxInfo.h"
#include "FxInterpInfoFloat.h"

struct EmissionInfo_t;

class NOTSA_EXPORT_VTABLE FxInfoEmWeather_c : public FxInfo_c {
protected:
    FxInterpInfoFloat_c m_InterpInfo;

public:
    static void InjectHooks();

    FxInfoEmWeather_c();
    ~FxInfoEmWeather_c() override = default;
    void DestructorBody(); // 0x4A6DA0 (lote 217A, sin validar): cuerpo del destructor del EXE
    FxInfoEmWeather_c* ScalarDeletingDestructor(uint8 flags); // 0x4A6D80 (lote 217A, sin validar): ranura 0 de la vtable del EXE, `ret 4`

    void Load(FILESTREAM file, int32 version) override;
    void GetValue(float currentTime, float mult, float totalTime, float length, bool useConst, void* info) override;

private:
    FxInfoEmWeather_c* Constructor() { this->FxInfoEmWeather_c::FxInfoEmWeather_c(); return this; } // 0x004A50E0 (lote 215A, sin validar): constructor del EXE, thiscall sin argumentos, devuelve this
};
