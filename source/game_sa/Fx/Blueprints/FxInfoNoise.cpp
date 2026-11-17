/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInfoNoise_c
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

#include "FxInfoNoise.h"

void FxInfoNoise_c::InjectHooks() {
    RH_ScopedClass(FxInfoNoise_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x4A5270); // lote 215A (sin validar)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4A6E60);
    RH_ScopedInstall(DestructorBody, 0x4A6E80);
}

// 0x4A5270
FxInfoNoise_c::FxInfoNoise_c() : FxInfo_c() {
    m_nType = FX_INFO_NOISE_DATA;
    m_InterpInfo.Allocate(1);
}

void FxInfoNoise_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A5300
void FxInfoNoise_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    auto& movement = *static_cast<MovementInfo_t*>(info);
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    float value;
    m_InterpInfo.GetVal(&value, mult);

    CVector in((float)(CGeneral::GetRandomNumber() % 10'000) / 5000.0f - 1.0f);
    in.Normalise();
    in *= value * totalTime;

    auto length = movement.m_Vel.Magnitude();
    movement.m_Vel += in;

    if (movement.m_Vel.Magnitude() > 0.0f) {
        movement.m_Vel.Normalise();
    }

    movement.m_Vel *= length;
}


// 0x4A6E80 (67 bytes con marco SEH que solo desenrolla y aqui se omite; thiscall sin argumentos, `ret`): cuerpo del destructor de FxInfoNoise_c del EXE. Como el original: destruye el miembro +8
// (FxInterpInfo32_c, 0x4A89B0: solo deja la vtable de FxInterpInfo_c, 0x85A968) y despues deja la vtable de la base FxInfo_c (0x85A7DC). El destructor generado por el compilador dejaria vtables propias
// de la DLL; aqui se escriben las del EXE.
void FxInfoNoise_c::DestructorBody() {
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}

// 0x4A6E60 (27 bytes, thiscall, un argumento de pila, `ret 4`): destructor con borrado de FxInfoNoise_c del EXE (ranura 0 de su vtable). `push esi; mov esi,ecx; call 0x4A6E80; test byte [esp+8],1; je ..;
// push esi; call 0x8214BD (operator delete); add esp,4; mov eax,esi; pop esi; ret 4`: destruye con el cuerpo, libera `this` solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor virtual).
FxInfoNoise_c* FxInfoNoise_c::ScalarDeletingDestructor(uint8 flags) {
    DestructorBody();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}
