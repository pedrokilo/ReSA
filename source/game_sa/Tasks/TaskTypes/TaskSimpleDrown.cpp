/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleDrown
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleDrown.h"
#include "TaskSimpleDie.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
// 0x62FEC0 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskSimpleDrown del EXE (thiscall, sin argumentos, `ret`, devuelve this). Base CTaskSimple (0x61A390); los dwords +8, +0x10, +0x14, +0x1C y +0x24 a cero, +0xC = 0x8C, +0x18 = 4.0f, los dos bits bajos del byte +0x20 a cero (el resto se conserva), vtable 0x86DE30 y byte +0x28 = 1.
CTaskSimpleDrown* __fastcall DrownConstructorBody(CTaskSimpleDrown* self, void* /*edx*/) {
    CTaskSimpleBody::Constructor(self, nullptr);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    const uint8 flags = bytes[0x20];
    *reinterpret_cast<uint32*>(bytes + 8) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x10) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x14) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x1C) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x24) = 0;
    *reinterpret_cast<uint32*>(bytes + 0xC) = 0x8C;
    *reinterpret_cast<uint32*>(bytes + 0x18) = 0x40800000;
    bytes[0x20] = flags & 0xFC;
    *reinterpret_cast<uint32*>(bytes) = 0x86DE30;
    bytes[0x28] = 1;
    return self;
}
} // namespace

void CTaskSimpleDrown::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDrown, 0x86DE30, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(DrownConstructorBody, "Constructor", 0x62FEC0);
    RH_ScopedVMTInstall(ProcessPed, 0x639900);
}

// 0x62FEC0
CTaskSimpleDrown::CTaskSimpleDrown() : CTaskSimpleDie(ANIM_GROUP_DEFAULT, ANIM_ID_DROWN, 4.0f, 0.0f) {
    m_bShouldExecute = true;
}

// 0x639900
bool CTaskSimpleDrown::ProcessPed(CPed* ped) {
    if (m_bShouldExecute) {
        ped->SetPedState(PEDSTATE_DIE);
        ped->bIsInTheAir = false; 
        if (ped == FindPlayerPed()) {
            CStats::IncrementStat(STAT_TIMES_DROWNED, 1.0f);
        }
        m_bShouldExecute = false;
    }
    return CTaskSimpleDie::ProcessPed(ped);
}