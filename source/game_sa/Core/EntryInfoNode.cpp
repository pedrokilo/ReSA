/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEntryInfoNode
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EntryInfoNode.h"

void CEntryInfoNode::InjectHooks()
{
    RH_ScopedClass(CEntryInfoNode);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(operator new, 0x536DC0);
    RH_ScopedInstall(operator delete, 0x536DD0);
}

void* CEntryInfoNode::operator new(unsigned size)
{
    return GetEntryInfoNodePool()->New();
}

void CEntryInfoNode::operator delete(void* ptr, size_t sz)
{
    GetEntryInfoNodePool()->Delete(reinterpret_cast<CEntryInfoNode*>(ptr));
}

void CEntryInfoNode::AddToList(CEntryInfoNode* next)
{
    m_previous = nullptr;
    m_next = next;
    if (next) {
        next->m_previous = this;
    }
}
