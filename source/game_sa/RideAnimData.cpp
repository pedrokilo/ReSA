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
