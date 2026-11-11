/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMatrixLinkList
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "MatrixLinkList.h"

// Gestor global de matrices enlazadas situado en la misma dirección usada por el juego.
auto& gMatrixList = StaticRef<CMatrixLinkList>(0xB74288);

namespace {
void __fastcall CMatrixLinkListDestructorBody(CMatrixLinkList* self, void* edx);
}

/**
 * Registra las operaciones del pool global de CMatrixLink.
 */
void CMatrixLinkList::InjectHooks()
{
    RH_ScopedClass(CMatrixLinkList);
    RH_ScopedCategory("Core");

    RH_ScopedNamedGlobalInstall(CMatrixLinkListDestructorBody, "Destructor", 0x54EB70); // lote 161A (banco Fn161A)
    RH_ScopedInstall(Init, 0x54F0D0);
    RH_ScopedInstall(Shutdown, 0x54E990);
    RH_ScopedInstall(AddToList1, 0x54E9D0);
    RH_ScopedInstall(AddToList2, 0x54EA10);
    RH_ScopedInstall(MoveToList1, 0x54EA60);
    RH_ScopedInstall(MoveToList2, 0x54EA90);
    RH_ScopedInstall(MoveToFreeList, 0x54EAD0);
    RH_ScopedInstall(GetNumFree, 0x54EB10);
    RH_ScopedInstall(GetNumUsed1, 0x54EB30);
    RH_ScopedInstall(GetNumUsed2, 0x54EB50);
}

namespace {
static_assert(offsetof(CMatrixLinkList, m_head) == 0x00 && offsetof(CMatrixLinkList, m_tail) == 0x54 && offsetof(CMatrixLinkList, m_allocatedListHead) == 0xA8
           && offsetof(CMatrixLinkList, m_allocatedListTail) == 0xFC && offsetof(CMatrixLinkList, m_freeListHead) == 0x150 && offsetof(CMatrixLinkList, m_freeListTail) == 0x1A4);

// 0x54EB70 (lote 161A, banco Fn161A; thiscall, sin argumentos, `ret`, 140 bytes, marco SEH que solo desenrolla): destructor de CMatrixLinkList del EXE (lo llama el destructor estatico de gMatrixList, 0x856495).
// Destruye los seis centinelas en orden inverso (+0x1A4, +0x150, +0xFC, +0xA8, +0x54 y +0) con el cuerpo del destructor de CMatrix del EXE (0x59ACD0, con hook: CMatrix::Destructor); no toca el bloque m_pObjects.
// El destructor generado por el compilador usaria ~CMatrix de la DLL (Detach), que no hace lo mismo.
__declspec(noinline) void __fastcall CMatrixLinkListDestructorBody(CMatrixLinkList* self, void* /*edx*/) {
    self->m_freeListTail.Destructor();
    self->m_freeListHead.Destructor();
    self->m_allocatedListTail.Destructor();
    self->m_allocatedListHead.Destructor();
    self->m_tail.Destructor();
    self->m_head.Destructor();
}
} // namespace

/**
 * Inicializa los nodos centinela y deja el almacenamiento del pool sin asignar.
 * Init() es quien construye posteriormente las tres listas intrusivas.
 */
CMatrixLinkList::CMatrixLinkList() : m_head(), m_tail(), m_allocatedListHead(), m_allocatedListTail(), m_freeListHead(), m_freeListTail()
{
    m_pObjects = nullptr;
}

/**
 * Reserva @p count matrices y las coloca inicialmente en la lista de libres.
 *
 * Se utilizan tres listas con nodos centinela: dos para matrices asignadas con
 * distinta prioridad/uso y una para entradas disponibles. Los objetos se insertan
 * en orden inverso para que el primer elemento del array quede al frente al acabar.
 */
void CMatrixLinkList::Init(int32 count)
{
    m_pObjects = new CMatrixLink[count];

    m_head.m_pNext = &m_tail;
    m_tail.m_pPrev = &m_head;

    m_freeListHead.m_pNext = &m_freeListTail;
    m_freeListTail.m_pPrev = &m_freeListHead;

    m_allocatedListHead.m_pNext = &m_allocatedListTail;
    m_allocatedListTail.m_pPrev = &m_allocatedListHead;

    for (int32 i = count - 1; i >= 0; --i) {
        auto& mat = m_pObjects[i];
        m_freeListHead.Insert(&mat);
    }
}

/**
 * Libera el bloque contiguo de matrices reservado por Init().
 *
 * No intenta recorrer ni destruir listas por separado porque todos sus nodos
 * pertenecen al mismo array m_pObjects.
 */
void CMatrixLinkList::Shutdown()
{
    delete[] m_pObjects;
    m_pObjects = nullptr;
}

/**
 * Obtiene la primera matriz libre y la mueve a la lista de uso principal.
 * @return La matriz reservada, o nullptr cuando el pool está agotado.
 */
CMatrixLink* CMatrixLinkList::AddToList1()
{
    if (IsFull())
        return nullptr;

    auto pNextFree = m_freeListHead.m_pNext;
    CMatrixLinkList::MoveToList1(pNextFree);
    return pNextFree;
}

/**
 * Obtiene la primera matriz libre y la mueve a la segunda lista de uso.
 * @return La matriz reservada, o nullptr cuando el pool está agotado.
 */
CMatrixLink* CMatrixLinkList::AddToList2()
{
    if (IsFull())
        return nullptr;

    auto pNextFree = m_freeListHead.m_pNext;
    CMatrixLinkList::MoveToList2(pNextFree);
    return pNextFree;
}

// Reubica una matriz ya enlazada al frente de la lista de uso principal.
void CMatrixLinkList::MoveToList1(CMatrixLink* mat)
{
    mat->Remove();
    m_head.Insert(mat);
}

// Reubica una matriz ya enlazada al frente de la segunda lista de uso.
void CMatrixLinkList::MoveToList2(CMatrixLink* mat)
{
    mat->Remove();
    m_allocatedListHead.Insert(mat);
}

// Devuelve una matriz al frente de la lista de libres sin liberar su almacenamiento.
void CMatrixLinkList::MoveToFreeList(CMatrixLink* mat)
{
    mat->Remove();
    m_freeListHead.Insert(mat);
}

// Cuenta los nodos disponibles recorriendo la lista hasta su centinela final.
int32 CMatrixLinkList::GetNumFree()
{
    int32 result = 0;
    auto cur = m_freeListHead.m_pNext;
    while (cur != &m_freeListTail) {
        cur = cur->m_pNext;
        ++result;
    }

    return result;
}

// Cuenta las matrices presentes actualmente en la lista de uso principal.
int32 CMatrixLinkList::GetNumUsed1()
{
    int32 result = 0;
    auto cur = m_head.m_pNext;
    while (cur != &m_tail) {
        cur = cur->m_pNext;
        ++result;
    }

    return result;
}

// Cuenta las matrices presentes actualmente en la segunda lista de uso.
int32 CMatrixLinkList::GetNumUsed2()
{
    int32 result = 0;
    auto cur = m_allocatedListHead.m_pNext;
    while (cur != &m_allocatedListTail) {
        cur = cur->m_pNext;
        ++result;
    }

    return result;
}
