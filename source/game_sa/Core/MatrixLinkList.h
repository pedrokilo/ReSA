#pragma once

#include "MatrixLink.h"

/**
 * Pool fijo de CMatrixLink organizado mediante tres listas intrusivas.
 *
 * m_head/m_tail delimitan la lista principal, m_allocatedListHead/Tail la segunda
 * lista de matrices asignadas y m_freeListHead/Tail las entradas disponibles.
 * Mover una matriz entre listas no cambia su dirección ni transfiere ownership:
 * toda la memoria pertenece al array m_pObjects reservado por Init().
 */
class CMatrixLinkList {
public:
    CMatrixLinkList();

public:
    CMatrixLink  m_head;              //!< Centinela inicial de la lista principal.
    CMatrixLink  m_tail;              //!< Centinela final de la lista principal.
    CMatrixLink  m_allocatedListHead; //!< Centinela inicial de la segunda lista de uso.
    CMatrixLink  m_allocatedListTail; //!< Centinela final de la segunda lista de uso.
    CMatrixLink  m_freeListHead;      //!< Centinela inicial de entradas disponibles.
    CMatrixLink  m_freeListTail;      //!< Centinela final de entradas disponibles.
    CMatrixLink* m_pObjects;          //!< Bloque contiguo propiedad de este gestor.

public:
    static void InjectHooks();

public:
    // Reserva el pool y coloca todas sus matrices en la lista de libres.
    void         Init(int32 count);
    // Libera el bloque del pool; no libera CPlaceable asociados.
    void         Shutdown();
    // Reserva una matriz libre para la lista principal.
    CMatrixLink* AddToList1();
    // Reserva una matriz libre para la segunda lista de uso.
    CMatrixLink* AddToList2();
    inline void  MoveToList1(CMatrixLink* mat);
    inline void  MoveToList2(CMatrixLink* mat);
    inline void  MoveToFreeList(CMatrixLink* mat);
    int32        GetNumFree();
    int32        GetNumUsed1();
    int32        GetNumUsed2();

public:
    // El pool está lleno cuando no existe ningún nodo entre los centinelas de libres.
    inline bool IsFull() { return m_freeListHead.m_pNext == &m_freeListTail; }
    // La cola de la lista principal representa el enlace menos reciente.
    inline CMatrixLink* GetOldestLink() { return m_tail.m_pPrev; }
};

VALIDATE_SIZE(CMatrixLinkList, 0x1FC);

extern CMatrixLinkList& gMatrixList;
