/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CStencilShadowObject
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

#include "StencilShadowObject.h"

void CStencilShadowObject::InjectHooks() {
    RH_ScopedClass(CStencilShadowObject);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x70F8D0);
    RH_ScopedInstall(Destructor, 0x70F8E0);
    RH_ScopedInstall(Destroy, 0x711310);
}

// 0x711310
void CStencilShadowObject::Destroy() {
    CEntity::SafeCleanUpRef(m_pOwner);
    m_pOwner                = nullptr;
    m_Type                  = eStencilShadowObjType::NONE;
    m_SizeOfShadowFacesData = 0;
    m_FaceID                = 0;

    if (m_ShadowFacesData) {
        CMemoryMgr::Free(std::exchange(m_ShadowFacesData, nullptr));
    }

    if (m_pPrev) {
        if (m_pNext) {
            m_pNext->m_pPrev = m_pPrev;
            m_pPrev->m_pNext = m_pNext;
        } else {
            m_pPrev->m_pNext = nullptr;
        }
    } else {
        CStencilShadows::pFirstActiveStencilShadowObject = m_pNext;
        if (CStencilShadows::pFirstActiveStencilShadowObject) {
            CStencilShadows::pFirstActiveStencilShadowObject->m_pPrev = nullptr;
        }
    }
    m_pNext = CStencilShadows::pFirstAvailableStencilShadowObject;
    m_pPrev = nullptr;
    CStencilShadows::pFirstAvailableStencilShadowObject = this;

    if (m_pNext) {
        m_pNext->m_pPrev = this;
    }
}
