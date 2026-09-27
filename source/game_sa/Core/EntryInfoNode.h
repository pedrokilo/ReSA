#pragma once

#include "PtrListDoubleLink.h"
#include "PtrNodeDoubleLink.h"

class CRepeatSector;
class CPhysical;

/**
 * Nodo que vincula un CPhysical con la lista espacial concreta en la que fue insertado.
 *
 * m_doubleLinkList/m_doubleLink permiten retirar la entidad de la estructura de
 * sectores sin buscarla de nuevo; m_repeatSector identifica el sector relacionado.
 * Los enlaces m_previous/m_next forman además la lista de entradas perteneciente
 * al propio CPhysical.
 */
class CEntryInfoNode {
public:
    CPtrListDoubleLink<CPhysical*>* m_doubleLinkList; //!< Lista espacial que contiene m_doubleLink.
    CPtrNodeDoubleLink<CPhysical*>* m_doubleLink;     //!< Nodo que referencia al CPhysical dentro de esa lista.
    CRepeatSector*                  m_repeatSector;   //!< Sector repetido asociado a esta entrada.
    CEntryInfoNode*                 m_previous;       //!< Entrada anterior del mismo CPhysical.
    CEntryInfoNode*                 m_next;           //!< Entrada siguiente del mismo CPhysical.

public:
    static void InjectHooks();

    static void* operator new(unsigned size);
    static void  operator delete(void* ptr, size_t sz);

    // Enlaza este nodo delante de pNext; el llamador actualiza la cabeza externa.
    void         AddToList(CEntryInfoNode* pNext); // Fully inlined in final game
};
VALIDATE_SIZE(CEntryInfoNode, 0x14);
