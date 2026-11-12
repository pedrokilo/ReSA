/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDamageAtomicModelInfo
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

#include "DamageAtomicModelInfo.h"
#include "CustomBuildingRenderer.h"
#include "CarFXRenderer.h"

namespace CBaseModelInfoBody {
CBaseModelInfo* __fastcall Constructor(CBaseModelInfo* self, void* edx); // BaseModelInfo.cpp: 0x4C4A60
}

// 0x4C55A0 (lote 161A, banco Fn161A): constructor de CDamageAtomicModelInfo del EXE (thiscall, sin argumentos, `ret`, 18 bytes: `push esi; mov esi,ecx; call 0x4C4A60; mov [esi],0x85BC30; mov eax,esi; pop esi; ret`):
// llama al cuerpo del constructor de CBaseModelInfo (0x4C4A60, con hook) y deja la vtable de CDamageAtomicModelInfo del EXE (0x85BC30); devuelve `this`. Lo pasan a los iteradores de vectores de los almacenes
// de modelos (CStore). Funcion propia sin expansion en linea: el banco sustituye el cuerpo base.
__declspec(noinline) static CDamageAtomicModelInfo* __fastcall DamageAtomicModelInfoConstructorBody(CDamageAtomicModelInfo* self, void* /*edx*/) {
    CBaseModelInfoBody::Constructor(self, nullptr);
    *reinterpret_cast<uint32*>(self) = 0x85BC30;
    return self;
}

// 0x4C55D0 (lote 173A, banco Fn173A): cuerpo del destructor de CDamageAtomicModelInfo del EXE (thiscall, sin argumentos, `ret`, 7 bytes: `mov dword [ecx],0x85BB9C`): solo deja la vtable de la clase base y no llama
// a nada (como 0x4C5590 y 0x4C5620, los de CAtomicModelInfo y CBaseModelInfo). Funcion propia sin expansion en linea.
__declspec(noinline) static void __fastcall DamageAtomicModelInfoDestructorBody(CDamageAtomicModelInfo* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85BB9C;
}

/**
 * Registra las operaciones del model info con variante dañada de RpAtomic.
 */
void CDamageAtomicModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CDamageAtomicModelInfo, 0x85BC30, 16);
    RH_ScopedCategory("Models");

    RH_ScopedNamedGlobalInstall(DamageAtomicModelInfoConstructorBody, "ConstructorBody", 0x4C55A0); // lote 161A (banco Fn161A)
    RH_ScopedNamedGlobalInstall(DamageAtomicModelInfoDestructorBody, "DestructorBody", 0x4C55D0); // lote 173A (banco Fn173A)
    RH_ScopedVMTInstall(Init, 0x4C48B0);
    RH_ScopedVMTInstall(AsDamageAtomicModelInfoPtr, 0x4C55C0);
    RH_ScopedVMTInstall(DeleteRwObject, 0x4C49D0);
    RH_ScopedVMTOverloadedInstall(CreateInstance, "void", 0x4C4960, RwObject * (CDamageAtomicModelInfo::*)()); // lote 173A (banco Fn173A)
    RH_ScopedVMTOverloadedInstall(CreateInstance, "rwmat", 0x4C4910, RwObject * (CDamageAtomicModelInfo::*)(RwMatrix*)); // lote 173A (banco Fn173A)

    RH_ScopedInstall(SetDamagedAtomic, 0x4C48D0);
}

// 0x4C55C0
CDamageAtomicModelInfo* CDamageAtomicModelInfo::AsDamageAtomicModelInfoPtr()
{
    return this;
}

// 0x4C48B0
void CDamageAtomicModelInfo::Init()
{
    CBaseModelInfo::Init();
    m_pDamagedAtomic = nullptr;
}

// 0x4C49D0
// Destruye primero la plantilla dañada y delega después la limpieza de la normal a la base.
void CDamageAtomicModelInfo::DeleteRwObject()
{
    if (m_pDamagedAtomic) {
        auto frame = RpAtomicGetFrame(m_pDamagedAtomic);
        RpAtomicDestroy(m_pDamagedAtomic);
        RwFrameDestroy(frame);
        m_pDamagedAtomic = nullptr;
    }

    CAtomicModelInfo::DeleteRwObject();
}

/**
 * Crea la versión normal o dañada según el selector global ms_bCreateDamagedVersion.
 * La variante dañada se clona con un frame propio igual que una instancia atomic normal.
 */
// 0x4C4960 (lote 173A, banco Fn173A; ranura 11 de la vtable 0x85BC30; thiscall, sin argumentos, `ret`, 103 bytes; el listado decia CDamageAtomicModelInfo): con el selector 0xA9B0B0 puesto, sin plantilla danada
// (+0x20) devuelve nulo; si no, clona esa plantilla (0x749E60), crea un frame (0x7F0410) y se lo asigna (0x74BF20). Sin el selector hace lo mismo con la plantilla normal (+0x1C) entre AddRef (0x4C4BA0) y
// RemoveRef (0x4C4BB0), en linea en el EXE y aqui con CAtomicModelInfo::CreateInstance (0x4C4530, que lee +0x1C dos veces igual que el EXE).
RwObject* CDamageAtomicModelInfo::CreateInstance()
{
    if (!CDamageAtomicModelInfo::ms_bCreateDamagedVersion)
        return CAtomicModelInfo::CreateInstance();

    if (!m_pDamagedAtomic)
        return nullptr;

    auto atomic = RpAtomicClone(m_pDamagedAtomic);
    auto frame = RwFrameCreate();
    RpAtomicSetFrame(atomic, frame);
    return reinterpret_cast<RwObject*>(atomic);
}

// 0x4C4910 (lote 173A, banco Fn173A; ranura 10 de la vtable 0x85BC30; thiscall, un argumento de pila, `ret 4`; la funcion del EXE son los fragmentos 0x4C4910 (24 bytes), 0x4C4923 (48) y 0x4C4953 (`jmp 0x4C44D0`);
// el listado los atribuia a CDamageAtomicModelInfo): como CreateInstance() pero copia la matriz (64 bytes con `rep movsd` hacia delante) sobre la matriz de modelado del frame (+0x10) antes de asignarlo. Sin el selector
// salta a CAtomicModelInfo::CreateInstance(RwMatrix*) (0x4C44D0).
RwObject* CDamageAtomicModelInfo::CreateInstance(RwMatrix* matrix)
{
    if (!CDamageAtomicModelInfo::ms_bCreateDamagedVersion)
        return CAtomicModelInfo::CreateInstance(matrix);

    if (!m_pDamagedAtomic)
        return nullptr;

    auto atomic = RpAtomicClone(m_pDamagedAtomic);
    auto frame = RwFrameCreate();
    __movsd(reinterpret_cast<unsigned long*>(RwFrameGetMatrix(frame)), reinterpret_cast<const unsigned long*>(matrix), sizeof(RwMatrix) / sizeof(unsigned long));
    RpAtomicSetFrame(atomic, frame);
    return reinterpret_cast<RwObject*>(atomic);
}

/**
 * Registra la plantilla dañada y le aplica el pipeline especializado correspondiente.
 * El puntero pasa a ser gestionado por este model info y se destruye en DeleteRwObject().
 */
void CDamageAtomicModelInfo::SetDamagedAtomic(RpAtomic* atomic)
{
    m_pDamagedAtomic = atomic;
    if (CCustomBuildingRenderer::IsCBPCPipelineAttached(atomic))
    {
        CCustomBuildingRenderer::AtomicSetup(atomic);
    }
    else if (CCarFXRenderer::IsCCPCPipelineAttached(atomic))
    {
        CCarFXRenderer::SetCustomFXAtomicRenderPipelinesVMICB(atomic, nullptr);
    }
}
