/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: FxInfoGroundCollide_c
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

#include "FxInfoGroundCollide.h"

// Lote 216A (sin validar): ver reversing/PendienteValidar/216A.md
void FxInfoGroundCollide_c::InjectHooks() {
    RH_ScopedClass(FxInfoGroundCollide_c);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(Constructor, 0x004A5980);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x4A7090);
    RH_ScopedInstall(DestructorBody, 0x4A70B0);
}

// 0x4A5980
FxInfoGroundCollide_c::FxInfoGroundCollide_c() : FxInfo_c() {
    m_nType = FX_INFO_GROUNDCOLLIDE_DATA;
    m_InterpInfo.Allocate(3);
}

// 0x4A5980 (lote 216A, sin validar): constructor del EXE (thiscall, devuelve this; con SEH propio del compilador). Llama al constructor C++ de arriba, que reproduce las escrituras del EXE.
FxInfoGroundCollide_c* FxInfoGroundCollide_c::Constructor() {
    this->FxInfoGroundCollide_c::FxInfoGroundCollide_c();
    return this;
}

// 0x4A59F0
void FxInfoGroundCollide_c::Load(FILESTREAM file, int32 version) {
    m_InterpInfo.Load(file);
}

// 0x4A7100
void FxInfoGroundCollide_c::GetValue(float currentTime, float mult, float totalTime, float len, bool useConst, void* info) {
    if (!m_bTimeModeParticle) {
        mult = currentTime / len;
    }

    auto& movement = *static_cast<MovementInfo_t*>(info);

    CVector origin = movement.m_Pos;
    auto diff = movement.m_Pos.z - movement.m_Vel.z;
    CColPoint colPoint;
    CEntity* colEntity;
    if (!CWorld::ProcessVerticalLine(origin, diff, colPoint, colEntity, true))
        return;

    if (colPoint.m_vecPoint.z > origin.z || movement.m_Pos.z >= colPoint.m_vecPoint.z)
        return;

    float values[16];
    m_InterpInfo.GetVal(values, mult);

    CVector in((float)(CGeneral::GetRandomNumber() % 10'000) / 5000.0f - 1.0f);
    in.Normalise();
    in *= values[2] * totalTime * 5.0f;

    auto v13 = colPoint.m_vecNormal.SquaredMagnitude() * values[0];
    CVector out;
    out = movement.m_Vel - colPoint.m_vecNormal * (v13 + v13);
    out += in;
    out.Normalise();

    movement.m_Pos.z = colPoint.m_vecPoint.z;
    movement.m_Vel = values[1] * out.Magnitude() * out;
}


// 0x4A70B0 (67 bytes con marco SEH que solo desenrolla y aqui se omite; thiscall sin argumentos, `ret`): cuerpo del destructor de FxInfoGroundCollide_c del EXE. Como el original: destruye el miembro +8
// (FxInterpInfo32_c, 0x4A89B0: solo deja la vtable de FxInterpInfo_c, 0x85A968) y despues deja la vtable de la base FxInfo_c (0x85A7DC). El destructor generado por el compilador dejaria vtables propias
// de la DLL; aqui se escriben las del EXE.
void FxInfoGroundCollide_c::DestructorBody() {
    *reinterpret_cast<uint32*>(&m_InterpInfo) = 0x85A968;
    *reinterpret_cast<uint32*>(this)          = 0x85A7DC;
}

// 0x4A7090 (27 bytes, thiscall, un argumento de pila, `ret 4`): destructor con borrado de FxInfoGroundCollide_c del EXE (ranura 0 de su vtable). `push esi; mov esi,ecx; call 0x4A70B0; test byte [esp+8],1; je ..;
// push esi; call 0x8214BD (operator delete); add esp,4; mov eax,esi; pop esi; ret 4`: destruye con el cuerpo, libera `this` solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor virtual).
FxInfoGroundCollide_c* FxInfoGroundCollide_c::ScalarDeletingDestructor(uint8 flags) {
    DestructorBody();
    if (flags & 1) {
        ::operator delete(this);
    }
    return this;
}
