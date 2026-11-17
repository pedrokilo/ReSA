/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInfoEmWeather_c
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "FxInfoEmWeather.h"
#include "EmissionInfo.h"

void FxInfoEmWeather_c::InjectHooks() {
    RH_ScopedClass(FxInfoEmWeather_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4A50E0); // lote 215A (sin validar)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4A6D80);
    RH_ScopedInstall(DestructorBody, 0x4A6DA0);
}

// 0x4A50E0
FxInfoEmWeather_c::FxInfoEmWeather_c() : m_InterpInfo() { // la base FxInfo_c por defecto (sin `()`): el EXE no escribe +6 ni +7
    m_nType = FX_INFO_EMWEATHER_DATA;
    m_InterpInfo.Allocate(4);
}

// 0x4A5150
void FxInfoEmWeather_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A5170
void FxInfoEmWeather_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    float values[4];
    m_InterpInfo.GetVal(values, currentTime);

    auto& emission = *static_cast<EmissionInfo_t*>(info);
    emission.m_fMinWind = values[0];
    emission.m_fMaxWind = values[1];
    emission.m_fMinRain = values[2];
    emission.m_fMaxRain = values[3];
}


// 0x4A6DA0 (67 bytes con marco SEH que solo desenrolla y aqui se omite; thiscall sin argumentos, `ret`): cuerpo del destructor de FxInfoEmWeather_c del EXE. Como el original: destruye el miembro +8
// (FxInterpInfoFloat_c, 0x4A8460: solo deja la vtable de FxInterpInfo_c, 0x85A968) y despues deja la vtable de la base FxInfo_c (0x85A7DC). El destructor generado por el compilador dejaria vtables propias
// de la DLL; aqui se escriben las del EXE.
void FxInfoEmWeather_c::DestructorBody() {
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}

// 0x4A6D80 (27 bytes, thiscall, un argumento de pila, `ret 4`): destructor con borrado de FxInfoEmWeather_c del EXE (ranura 0 de su vtable). `push esi; mov esi,ecx; call 0x4A6DA0; test byte [esp+8],1; je ..;
// push esi; call 0x8214BD (operator delete); add esp,4; mov eax,esi; pop esi; ret 4`: destruye con el cuerpo, libera `this` solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor virtual).
FxInfoEmWeather_c* FxInfoEmWeather_c::ScalarDeletingDestructor(uint8 flags) {
    DestructorBody();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}
