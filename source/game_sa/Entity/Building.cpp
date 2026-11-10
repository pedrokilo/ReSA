/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBuilding
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Building.h"
#include "Pools/PoolRawDelete.h"

namespace {
// Destructor de CEntity (0x535E90, con hook: CEntity::Destructor) llamado por el cuerpo del destructor de CBuilding como el `jmp 0x535E90` del EXE. Funcion propia sin expansion en linea para que el banco
// Fn149A pueda sustituirla (el hook de CEntity::Destructor lleva la misma llamada, pero esta es la que llama este cuerpo).
__declspec(noinline) void __fastcall BuildingEntityDestructor(CEntity* self, void* /*edx*/) { self->CEntity::~CEntity(); }

// 0x404180 (lote 149A, banco Fn149A): cuerpo del destructor de CBuilding del EXE (thiscall, sin argumentos, 5 bytes: `jmp 0x535E90`): no escribe la vtable (CBuilding no tiene destructor propio) y salta al
// destructor de CEntity. Lo llama el destructor con borrado (0x404130). Funcion propia sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall BuildingDestructorBody(CBuilding* self, void* /*edx*/) { BuildingEntityDestructor(self, nullptr); }

// 0x404130 (lote 149A, banco Fn149A): destructor con borrado de CBuilding del EXE (ranura 0 de su vtable 0x8585C8; thiscall, un argumento de pila, `ret 4`, 78 bytes: `push esi; push edi; mov edi,ecx;
// call 0x404180; test byte [esp+0Ch],1; je ..;` y, con el bit 0 del BYTE de banderas, el operator delete de CBuilding EN LINEA (la ranura del pool de edificios 0xB74498, como el hook de 0x4040A0); devuelve `this`).
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CBuilding* __fastcall BuildingScalarDeletingDestructor(CBuilding* self, void* /*edx*/, uint8 flags) {
    BuildingDestructorBody(self, nullptr);
    if (flags & 1) {
        PoolFreeSlotRaw(GetBuildingPool(), self, sizeof(CBuilding));
    }
    return self;
}
} // namespace

uintptr_t Lote149ABuildingEntityDestructor() { return reinterpret_cast<uintptr_t>(&BuildingEntityDestructor); } // para el banco Fn149A

/**
 * Registra las operaciones específicas de edificios estáticos y su pool.
 */
void CBuilding::InjectHooks()
{
    RH_ScopedVirtualClass(CBuilding, 0x8585C8, 22);
    RH_ScopedCategory("Entity");
    RH_ScopedNamedGlobalInstall(BuildingDestructorBody, "DestructorBody", 0x404180);                       // lote 149A (banco Fn149A)
    RH_ScopedNamedGlobalInstall(BuildingScalarDeletingDestructor, "ScalarDeletingDestructor", 0x404130);   // lote 149A (banco Fn149A)

    RH_ScopedInstall(ReplaceWithNewModel, 0x403EC0);
    RH_ScopedInstall(operator new, 0x404090);    // lote 146A (banco Fn146A)
    RH_ScopedInstall(operator delete, 0x4040A0); // lote 146A (banco Fn146A)
    RH_ScopedGlobalInstall(IsBuildingPointerValid, 0x4040E0);
}

CBuilding::CBuilding() : CEntity()
{
    SetTypeBuilding();
    SetUsesCollision(true);
}

// 0x404090 (lote 146A, banco Fn146A): `mov ecx,[0xB74498]; jmp 0x403FA0` (cdecl con un argumento de pila que no se lee; `ret`). Los edificios se asignan exclusivamente desde CBuildingPool: toma una ranura con
// CBuildingPool::New (0x403FA0, con hook); devuelve nulo si esta lleno.
void* CBuilding::operator new(unsigned size)
{
    return GetBuildingPool()->New();
}

// 0x4040A0 (lote 146A, banco Fn146A): cdecl con un argumento de pila, `ret`, 51 bytes. Libera la ranura de `data` en el pool de edificios (0xB74498) en linea, como el EXE: indice =
// (data - almacenamiento) / 56 (division con signo truncada hacia cero, sin comprobar que pertenezca al pool), estados[indice] |= 0x80 y, si indice < cursor (con signo), cursor = indice.
void CBuilding::operator delete(void* data)
{
    PoolFreeSlotRaw(GetBuildingPool(), data, sizeof(CBuilding));
}

/**
 * Elimina el recurso RenderWare actual y cambia el model index.
 *
 * Si el modelo anterior ya no tiene referencias solicita a streaming que libere
 * también sus recursos compartidos.
 */
void CBuilding::ReplaceWithNewModel(int32 newModelIndex)
{
    DeleteRwObject();
    if (!CModelInfo::GetModelInfo(GetModelIndex())->m_nRefCount)
        CStreaming::RemoveModel(GetModelIndex());

    m_nModelIndex = newModelIndex;
}

bool IsBuildingPointerValid(CBuilding* building)
{
    if (!building)
        return false;

    // Como el original (0x4040F2): índice = diferencia con signo de 32 bits / 0x38 (trunca hacia cero),
    // así que un puntero hasta 0x37 bytes antes del almacenamiento cuenta como ranura 0.
    const auto* const pool    = GetBuildingPool();
    const auto* const storage = *reinterpret_cast<const uint8* const*>(pool);                          // CPool::m_pObjects (+0x0)
    const auto* const flags   = *reinterpret_cast<const int8* const*>(reinterpret_cast<const uint8*>(pool) + 0x4); // CPool::m_byteMap (+0x4)
    const int32 size          = *reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(pool) + 0x8);       // CPool::m_nSize (+0x8)
    const int32 idx = int32(reinterpret_cast<uintptr_t>(building) - reinterpret_cast<uintptr_t>(storage)) / int32(sizeof(CBuilding));
    return idx >= 0 && idx < size && flags[idx] >= 0;
}
