/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRideAnimData
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "RideAnimData.h"

namespace {
// 0x6D0B10 (lote 153A, banco Fn153A): constructor por defecto de CRideAnimData del EXE (thiscall, sin argumentos de pila, `ret`, 25 bytes: `mov eax,ecx; xor ecx,ecx` y siete `mov [eax+k],ecx` con k = 0, 4, .., 0x18):
// pone a cero los siete dwords del objeto (grupo de animacion, angulos y animaciones de inclinacion) y devuelve `this`. El listado de pendientes lo atribuia a CAEVehicleAudioEntity.
CRideAnimData* __fastcall RideAnimDataDefaultConstructorExe(CRideAnimData* self, void* /*edx*/) {
    *self = CRideAnimData{};
    return self;
}

// 0x6D0B30 (lote 153A, banco Fn153A): destructor de CRideAnimData del EXE (thiscall, `ret`, 1 byte): no hace nada. Lo llaman los iteradores de destructores de vectores (cinco sitios, entre ellos tres de
// la tabla de destructores estaticos del CRT). El listado de pendientes lo atribuia a CAEVehicleAudioEntity.
void __fastcall RideAnimDataDestructorExe(CRideAnimData* /*self*/, void* /*edx*/) {
}
} // namespace

void CRideAnimData::InjectHooks() {
    RH_ScopedClass(CRideAnimData);
    RH_ScopedCategory("Vehicle");

    RH_ScopedNamedGlobalInstall(RideAnimDataDefaultConstructorExe, "DefaultConstructor", 0x6D0B10); // lote 153A (banco Fn153A)
    RH_ScopedNamedGlobalInstall(RideAnimDataDestructorExe, "Destructor", 0x6D0B30);                 // lote 153A (banco Fn153A)
}
