/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAnimatedBuilding
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AnimatedBuilding.h"

/**
 * Registra los overrides mínimos de edificios que poseen jerarquía/animación RenderWare.
 */
namespace {
// Destructor de CEntity (0x535E90, con hook: CEntity::Destructor) llamado por el cuerpo del destructor de CAnimatedBuilding como el `jmp 0x535E90` del EXE. Funcion propia sin expansion en linea para que
// el banco pueda sustituirla (el hook de CEntity::Destructor lleva la misma llamada, pero esta es la que llama este cuerpo).
__declspec(noinline) void __fastcall AnimatedBuildingEntityDestructor(CEntity* self, void* /*edx*/) { self->CEntity::~CEntity(); }

// 0x537A70 (lote 140A, banco Fn140A): cuerpo del destructor de CAnimatedBuilding del EXE (thiscall, sin argumentos, 5 bytes: `jmp 0x535E90`): no escribe la vtable (CAnimatedBuilding no tiene destructor
// propio) y salta al destructor de CEntity. Lo llama el destructor con borrado (0x537A50).
__declspec(noinline) void __fastcall AnimatedBuildingDestructorBody(CAnimatedBuilding* self, void* /*edx*/) { AnimatedBuildingEntityDestructor(self, nullptr); }

// 0x537A50 (lote 140A, banco Fn140A): destructor con borrado de CAnimatedBuilding del EXE (ranura 0 de su vtable 0x8639B0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x537A70; test byte [esp+8],1; je ..; push esi; call 0x4040A0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x537A70),
// libera `this` con CBuilding::operator delete (0x4040A0, sin hook: pool de edificios 0xB74498) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CAnimatedBuilding* __fastcall AnimatedBuildingScalarDeletingDestructor(CAnimatedBuilding* self, void* /*edx*/, uint8 flags) {
    AnimatedBuildingDestructorBody(self, nullptr);
    if (flags & 1) {
        CBuilding::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote140AAnimatedBuildingEntityDestructor() { return reinterpret_cast<uintptr_t>(&AnimatedBuildingEntityDestructor); } // para el banco Fn140A

void CAnimatedBuilding::InjectHooks()
{
    RH_ScopedVirtualClass(CAnimatedBuilding, 0x8639B0, 22);
    RH_ScopedCategory("Entity");

    RH_ScopedNamedGlobalInstall(AnimatedBuildingDestructorBody, "DestructorBody", 0x537A70);                       // lote 140A (banco Fn140A)
    RH_ScopedNamedGlobalInstall(AnimatedBuildingScalarDeletingDestructor, "ScalarDeletingDestructor", 0x537A50);   // lote 140A (banco Fn140A)

    RH_ScopedVMTInstall(ProcessControl, 0x537A30);
    RH_ScopedVMTInstall(ProcessCollision, 0x537A40);
    RH_ScopedVMTInstall(PreRender, 0x403F00);
}

// 0x537A30
void CAnimatedBuilding::ProcessControl()
{}

// 0x537A40
// El edificio animado no integra física aquí; sólo confirma que su posición es segura.
void CAnimatedBuilding::ProcessCollision()
{
    SetIsInSafePosition(true);
}

// 0x403F00
/**
 * Ejecuta el pre-render base y sincroniza frame/HAnim antes de dibujar la geometría.
 */
void CAnimatedBuilding::PreRender()
{
    if (!GetRwObject())
        return;

    CEntity::PreRender();
    CEntity::UpdateRwFrame();
    CEntity::UpdateRpHAnim();
}
