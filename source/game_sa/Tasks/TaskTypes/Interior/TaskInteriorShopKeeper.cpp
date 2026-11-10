/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskInteriorShopKeeper
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskInteriorShopKeeper.h"
#include "TaskInteriorUseInfo.h"
#include "TaskSimpleRunAnim.h"

#include "InteriorGroup_c.h"
#include "InteriorInfo_t.h"
#include "InteriorManager_c.h"

namespace {
// 0x6756A0 (lote 146A, banco Fn146A): cuerpo del destructor de CTaskInteriorShopKeeper que usa el destructor con borrado del EXE (thiscall, sin argumentos de pila; el hook Destructor de la clase). Funcion propia sin expansion
// en linea para que el banco pueda sustituirla; el destructor con borrado la llama como el `call 0x6756A0` del EXE.
__declspec(noinline) void __fastcall ShopKeeperDestructorBody(CTaskInteriorShopKeeper* self, void* /*edx*/) { self->CTaskInteriorShopKeeper::~CTaskInteriorShopKeeper(); }

// 0x6764A0 (lote 146A, banco Fn146A): destructor con borrado de CTaskInteriorShopKeeper del EXE (ranura 0 de su vtable 0x870290; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6756A0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x6756A0), libera `this`
// con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskInteriorShopKeeper* __fastcall ShopKeeperScalarDeletingDestructor(CTaskInteriorShopKeeper* self, void* /*edx*/, uint8 flags) {
    ShopKeeperDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote146AShopKeeperDestructorBody() { return reinterpret_cast<uintptr_t>(&ShopKeeperDestructorBody); } // para el banco Fn146A

void CTaskInteriorShopKeeper::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorShopKeeper, 0x870290, 11);
    RH_ScopedCategory("Tasks/TaskTypes/Interior");
    RH_ScopedNamedGlobalInstall(ShopKeeperScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6764A0); // lote 146A (banco Fn146A)

    RH_ScopedInstall(Constructor, 0x675660);
    RH_ScopedInstall(Destructor, 0x6756A0);

    RH_ScopedVMTInstall(Clone, 0x6756C0);
    RH_ScopedVMTInstall(GetTaskType, 0x675690);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6764C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x676550);
    RH_ScopedVMTInstall(ControlSubTask, 0x675740);
}

// 0x675660
CTaskInteriorShopKeeper::CTaskInteriorShopKeeper(InteriorGroup_c* intGrp, bool bIsSetup) :
    m_bIsSetup{bIsSetup},
    m_IntGrp{intGrp}
{
}

// 0x6756C0
CTaskInteriorShopKeeper::CTaskInteriorShopKeeper(const CTaskInteriorShopKeeper& o) :
    CTaskInteriorShopKeeper{o.m_IntGrp, o.m_bIsSetup}
{
    // Como el original: Clone también copia la info y el interior ya encontrados
    m_IntInfo = o.m_IntInfo;
    m_Int     = o.m_Int;
}

// 0x6756A0
CTaskInteriorShopKeeper::~CTaskInteriorShopKeeper() {
    if (m_IntInfo) {
        m_IntInfo->IsInUse = false;
    }
}

// 0x6764C0
CTask* CTaskInteriorShopKeeper::CreateNextSubTask(CPed* ped) {
    return new CTaskInteriorUseInfo{ m_IntInfo, m_Int };
}

// 0x676550
CTask* CTaskInteriorShopKeeper::CreateFirstSubTask(CPed* ped) {
    if (!m_IntInfo) {
        m_IntGrp->FindInteriorInfo(eInteriorInfoType::UNK_10, &m_IntInfo, &m_Int); // Como el original: sin comprobar el resultado
    }
    m_IntInfo->IsInUse = true;
    // Como el original: la primera vez marca m_bIsSetup y crea la subtarea con el flag a true; después con false
    if (m_bIsSetup) {
        return new CTaskInteriorUseInfo{ m_IntInfo, m_Int, 0, false };
    }
    m_bIsSetup = true;
    return new CTaskInteriorUseInfo{ m_IntInfo, m_Int, 0, true };
}

// 0x675740
CTask* CTaskInteriorShopKeeper::ControlSubTask(CPed* ped) {
    if (m_bStartServeAnim) {
        m_bStartServeAnim = false; // Como el original: consume el aviso
        return new CTaskSimpleRunAnim{ ANIM_GROUP_INT_SHOP, ANIM_ID_SHOP_CASHIER, 4.f, false };
    }
    return m_pSubTask;
}
