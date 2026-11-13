/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CHandObject
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

/**
 * Registra sincronización, pre-render y render de objetos adheridos a la mano de un ped.
 */
void CHandObject::InjectHooks()
{
    RH_ScopedVirtualClass(CHandObject, 0x866EE0, 23);
    RH_ScopedCategory("Entity/Object");

    RH_ScopedVMTInstall(ProcessControl, 0x59EC40);
    RH_ScopedVMTInstall(PreRender, 0x59ECD0);
    RH_ScopedVMTInstall(Render, 0x59EE80);

    RH_ScopedInstall(Constructor, 0x59EEB0);              // lote 180A (sin validar)
    RH_ScopedInstall(ScalarDeletingDestructor, 0x59EFE0); // lote 180A (sin validar)
}

namespace CObjectBody { void __fastcall Destructor(CObject* self, void* edx); } // 0x59F660, Object.cpp (sin hook)

/**
 * Configura el objeto decorativo, selecciona antebrazo izquierdo/derecho y reutiliza
 * una textura del TXD del ped para el material del hand object.
 */
CHandObject::CHandObject(int32 handModelIndex, CPed* ped, bool bLeftHand) : CObject()
{
    InitFromPed(handModelIndex, ped, bLeftHand);
}

// 0x59EEB0 (lote 180A, sin validar; thiscall, tres dwords de pila, `ret 0xC`, 297 bytes con marco SEH que solo desenrolla y aqui se omite): constructor del EXE. Como el original: constructor por defecto de CObject del EXE (0x5A1D10,
// sin hook: el de la DLL escribe vtables propias), vtable de la clase del EXE (0x866EE0) y el resto del cuerpo en el orden del EXE (ver InitFromPed). Devuelve `this`. Lo llaman los constructores de las tareas de senal con la mano
// (0x61B044 y siguientes).
CHandObject* CHandObject::Constructor(int32 handModelIndex, CPed* ped, bool bLeftHand)
{
    plugin::CallMethod<0x5A1D10, CObject*>(static_cast<CObject*>(this));
    *reinterpret_cast<uint32*>(this) = 0x866EE0;
    InitFromPed(handModelIndex, ped, bLeftHand);
    return this;
}

// 0x59EFE0 (lote 180A, sin validar; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x59F000; test byte [esp+8],1; je ..; push esi; call 0x5A1F20; add esp,4; mov eax,esi; pop esi; ret 4`):
// destructor con borrado de CHandObject del EXE (ranura 0 de su vtable 0x866EE0). 0x59F000 es solo `jmp 0x59F660` (cuerpo del destructor de CObject, aqui CObjectBody::Destructor): la clase no escribe vtable ni libera nada mas. Libera
// `this` con CObject::operator delete (0x5A1F20, pool de objetos) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CHandObject* CHandObject::ScalarDeletingDestructor(uint8 flags)
{
    CObjectBody::Destructor(static_cast<CObject*>(this), nullptr);
    if (flags & 1) {
        CObject::operator delete(this);
    }
    return this;
}

// Cuerpo del constructor posterior al constructor de CObject, en el orden del EXE (0x59EECF..0x59EFC1): anim hierarchy del clump del ped, m_pPed, UpdateRpHAnim del ped, hueso del antebrazo (23 derecho, 33 izquierdo), SetModelIndex
// directo (0x532AE0), RpAnimBlendClumpInit (0x4D6720), textura del TXD del ped (por hash de nombre; si no, la primera), estado SIMPLE, sin colision, objeto ligero y sin borrado por streaming, tipo 5, SetIsStatic(false) (0x5A0760, llamada
// directa a la clase CObject), alfa 255 del modelo de la mano y m_bUpdatedMatricesArray.
void CHandObject::InitFromPed(int32 handModelIndex, CPed* ped, bool bLeftHand)
{
    auto* animHierarchy = GetAnimHierarchyFromSkinClump(ped->GetRpClump());
    m_pPed = ped;
    ped->UpdateRpHAnim();

    if (bLeftHand)
        m_nBoneIndex = RpHAnimIDGetIndex(animHierarchy, eBoneTag::BONE_L_FORE_ARM);
    else
        m_nBoneIndex = RpHAnimIDGetIndex(animHierarchy, eBoneTag::BONE_R_FORE_ARM);

    CEntity::SetModelIndex(handModelIndex);
    RpAnimBlendClumpInit(GetRpClump());

    auto* pedModelInfo = CModelInfo::GetModelInfo(ped->m_nModelIndex);
    auto* txd = CTxdStore::ms_pTxdPool->GetAt(pedModelInfo->m_nTxdIndex);
    m_pTexture = RwTexDictionaryFindHashNamedTexture(txd->m_pRwDictionary, pedModelInfo->m_nKey);
    if (!m_pTexture)
        m_pTexture = GetFirstTexture(txd->m_pRwDictionary);

    SetStatus(STATUS_SIMPLE);
    SetUsesCollision(false);
    m_bLightObject = true;
    m_bStreamingDontDelete = true;
    m_nObjectType = eObjectType::OBJECT_TYPE_DECORATION;

    CObject::SetIsStatic(false);
    CModelInfo::GetModelInfo(handModelIndex)->m_nAlpha = 0xFF;
    m_bUpdatedMatricesArray = true;

}

// 0x59EC40
// Copia la matriz world del bone objetivo y actualiza la pertenencia espacial del objeto.
void CHandObject::ProcessControl()
{
    auto* animHierarchy = GetAnimHierarchyFromSkinClump(m_pPed->GetRpClump());
    auto* matArr = RpHAnimHierarchyGetMatrixArray(animHierarchy);
    const auto boneMat = CMatrix(&matArr[m_nBoneIndex], false);
    *static_cast<CMatrix*>(m_matrix) = boneMat;

    SetIsInSafePosition(true);
    CPhysical::RemoveAndAdd();
}

// 0x59ECD0
/**
 * Fuerza actualización HAnim del ped, adopta la matriz del bone y neutraliza matrices
 * descendientes cuando corresponde antes de sincronizar el clump del objeto.
 */
void CHandObject::PreRender()
{
    auto* animHierarchy = GetAnimHierarchyFromSkinClump(m_pPed->GetRpClump());
    m_pPed->UpdateRpHAnim();
    m_pPed->m_bDontUpdateHierarchy = true;

    auto* matArr = RpHAnimHierarchyGetMatrixArray(animHierarchy);
    *static_cast<CMatrix*>(m_matrix) = CMatrix(&matArr[m_nBoneIndex], false);

    if (m_bUpdatedMatricesArray)
        m_bUpdatedMatricesArray = false;
    else
    {
        auto nBoneInd = m_nBoneIndex + 1;
        auto iStackCounter = 0;
        while (iStackCounter >= 0)
        {
            auto* boneMat = &matArr[nBoneInd];
            *RwMatrixGetAt(boneMat) = { 0.0F, 0.0F, 0.0F };
            *RwMatrixGetUp(boneMat) = { 0.0F, 0.0F, 0.0F };
            *RwMatrixGetRight(boneMat) = { 0.0F, 0.0F, 0.0F };

            const auto unFlags = static_cast<uint32>(animHierarchy->pNodeInfo[nBoneInd].flags);
            if ((unFlags & rpHANIMPUSHPARENTMATRIX) != 0)
                ++iStackCounter;
            else if ((unFlags & rpHANIMPOPPARENTMATRIX) != 0)
                --iStackCounter;

            ++nBoneInd;
        }
    }

    m_fContactSurfaceBrightness = m_pPed->m_fContactSurfaceBrightness;
    CEntity::UpdateRwMatrix();
    CEntity::UpdateRwFrame();
    CEntity::UpdateRpHAnim();
}

// 0x59EE80
// Sustituye la textura del primer material por la seleccionada del ped y delega el render.
void CHandObject::Render()
{
    auto* firstAtomic = GetFirstAtomic(GetRpClump());
    RpMaterialSetTexture(RpGeometryGetMaterial(RpAtomicGetGeometry(firstAtomic), 0), m_pTexture);
    CObject::Render();
}
