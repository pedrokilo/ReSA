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

/**
 * Registra las operaciones de asignación y liberación del pool de CEntryInfoNode.
 */
void CEntryInfoNode::InjectHooks()
{
    RH_ScopedClass(CEntryInfoNode);
    RH_ScopedCategory("Core");

    RH_ScopedInstall(operator new, 0x536DC0);
    RH_ScopedInstall(operator delete, 0x536DD0);
}

/**
 * Obtiene almacenamiento del pool global dedicado a CEntryInfoNode.
 *
 * El parámetro size forma parte de la firma de operator new, pero el tamaño real
 * del bloque está fijado por el tipo del pool.
 */
void* CEntryInfoNode::operator new(unsigned size)
{
    return GetEntryInfoNodePool()->New();
}

// Devuelve el slot al pool correspondiente; no utiliza el heap general de C++.
void CEntryInfoNode::operator delete(void* ptr, size_t sz)
{
    GetEntryInfoNodePool()->Delete(reinterpret_cast<CEntryInfoNode*>(ptr));
}

/**
 * Prepara este nodo como nueva cabeza delante de @p next.
 *
 * El llamador conserva la responsabilidad de actualizar el puntero de cabeza de
 * CEntryInfoList. Aquí sólo se configuran los enlaces internos entre nodos.
 */
void CEntryInfoNode::AddToList(CEntryInfoNode* next)
{
    m_previous = nullptr;
    m_next = next;
    if (next) {
        next->m_previous = this;
    }
}
