/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CQuadTreeNode<T>
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/14 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Rect.h"
#include "PtrListSingleLink.h"
#include "Pool.h"
#include "Pools/PoolRawDelete.h"

/*
node level 2
 +----------------------+--
 | node level 1         +
 |  +--------+--------+ +
 |  |node    |node    | +
 |  |level 0 |level 0 | +
 |  |index 0 |index 1 | +
 |  +--------+--------+ +
 |  |node    |node    | +
 |  |level 0 |level 0 | +
 |  |index 2 |index 3 | +
 |  +--------+--------+ +
 +----------------------+--
 |                      |

 Total rectangles = 4^startLevel
*/

template<typename T>
struct CQuadTreeNodeLegacyPool;

template<typename T>
class CQuadTreeNode {
public:
    using ListType                   = CPtrListSingleLink<T>;
    using CQuadTreeNodeRectCallBack  = void (*)(const CRect& rect, T item);
    using CQuadTreeNodeVec2DCallBack = void (*)(const CVector2D& rect, T item);

protected:
    using CQuadTreeNodePool = CPool<CQuadTreeNode<void*>>;
    static inline auto& ms_pQuadTreeNodePool = StaticRef<CQuadTreeNodePool*>(0xB745BC);

public:
    // Como el original (0x552B50 / 0x552C90): política original del pool (ver CQuadTreeNodeLegacyPool).
    static void* operator new(size_t sz) {
        assert(sz == sizeof(CQuadTreeNode<void*>));
        return AllocateStorageExe();
    }

    // 0x552C80 (lote 192A, sin validar; el listado decia CQuadTreeNode<T>: es el operator new del nodo; cdecl, SIN argumentos de pila, `ret`, 11 bytes: `mov ecx,[0xB745BC]; jmp 0x552B50`):
    // reserva un nodo en el pool de nodos (CQuadTreeNodeLegacyPool::New = 0x552B50) y devuelve la direccion del nodo o nulo si el pool esta lleno. Funcion propia sin expansion en linea para enganchar la direccion; la llama el operator new
    // como el `call 0x552C80` de los llamadores del EXE (0x405F4E, 0x411480, 0x43F8BB). No comprueba que el pool exista. Se engancha como S.
    __declspec(noinline) static void* AllocateStorageExe() {
        return static_cast<CQuadTreeNodeLegacyPool<T>*>(ms_pQuadTreeNodePool)->New();
    }

    static void operator delete(void* data, size_t sz) {
        assert(sz == sizeof(CQuadTreeNode<void*>));
        static_cast<CQuadTreeNodeLegacyPool<T>*>(ms_pQuadTreeNodePool)->Delete(static_cast<CQuadTreeNode<void*>*>(data));
    }

public:
    static void InjectHooks() {
        RH_ScopedClass(CQuadTreeNode<T>);
        RH_ScopedCategory("Core");

        RH_ScopedInstall(InitPool, 0x552C00);
        RH_ScopedInstall(Constructor, 0x552830);                                                      // lote 192A (sin validar)
        RH_ScopedNamedGlobalInstall(PoolConstructorExe, "PoolConstructor", 0x552AF0);                 // lote 192A (sin validar)
        RH_ScopedNamedGlobalInstall(CQuadTreeNodeLegacyPool<T>::New, "PoolNew", 0x552B50);            // lote 192A (sin validar)
        RH_ScopedNamedGlobalInstall(CQuadTreeNodeLegacyPool<T>::Delete, "PoolDelete", 0x552BC0);      // lote 192A (sin validar)
        RH_ScopedNamedGlobalInstall(AllocateStorageExe, "AllocateStorage", 0x552C80);                 // lote 192A (sin validar)
        RH_ScopedNamedGlobalInstall(DeleteStorageExe, "DeleteStorage", 0x552C90);                     // lote 192A (sin validar)
        RH_ScopedNamedInstall(ScalarDeletingDestructorExe, "ScalarDeletingDestructor", 0x404590); // lote 150A (banco Fn150A)
        RH_ScopedNamedInstall(DestructorBodyExe, "DestructorBody", 0x552520); // lote 175A (banco Fn175A)
        RH_ScopedOverloadedInstall(FindSector, "rect", 0x5525A0, int32(CQuadTreeNode<T>::*)(const CRect&));
        RH_ScopedOverloadedInstall(FindSector, "vec", 0x552640, int32(CQuadTreeNode<T>::*)(const CVector2D&));
        RH_ScopedInstall(InSector, 0x5526A0);
        RH_ScopedOverloadedInstall(DeleteItem, "", 0x552A40, void(CQuadTreeNode<T>::*)(T));
        RH_ScopedOverloadedInstall(DeleteItem, "rect", 0x552A90, void(CQuadTreeNode<T>::*)(T, const CRect&));
        RH_ScopedInstall(AddItem, 0x552CD0);
        RH_ScopedInstall(GetAll, 0x552870);
        RH_ScopedOverloadedInstall(GetAllMatching, "rect", 0x5528C0, void(CQuadTreeNode<T>::*)(const CRect&, ListType&));
        RH_ScopedOverloadedInstall(GetAllMatching, "vec", 0x552930, void(CQuadTreeNode<T>::*)(const CVector2D&, ListType&));
        RH_ScopedOverloadedInstall(ForAllMatching, "rect", 0x552980, void(CQuadTreeNode<T>::*)(const CRect&, CQuadTreeNodeRectCallBack));
        RH_ScopedOverloadedInstall(ForAllMatching, "vec", 0x5529F0, void(CQuadTreeNode<T>::*)(const CVector2D&, CQuadTreeNodeVec2DCallBack));
    }

    // 0x552C00
    static void InitPool() {
        ZoneScoped;

        if (ms_pQuadTreeNodePool) {
            return;
        }
        // Como el original: operator new de 0x14 bytes y llamada al constructor del pool (0x552AF0, PoolConstructorExe; objetos y mapa con operator new (no new[]) y sin relleno del
        // almacenamiento; cada byte del mapa queda en 0x80, el byte +0x11 no se escribe y el nombre no se usa). Si la primera reserva falla el puntero queda nulo.
        constexpr int32 SIZE = 400;
        void* const pool = ::operator new(sizeof(CQuadTreeNodePool));
        if (pool) {
            ms_pQuadTreeNodePool = static_cast<CQuadTreeNodePool*>(PoolConstructorExe(pool, nullptr, SIZE, "QuadTreeNodes"));
        } else {
            ms_pQuadTreeNodePool = nullptr;
        }
    }

    // 0x552AF0 (lote 192A, sin validar; el listado decia CQuadTreeNode<T> sin nombre: es el constructor de CPool<CQuadTreeNode<void*>> en el EXE; thiscall, dos argumentos de pila (capacidad y nombre), `ret 8`, 92 bytes sin marco SEH):
    // el almacenamiento de objetos es operator new (0x82119A) de capacidad * 40 bytes (+0x0) y el mapa de estados operator new de capacidad bytes (+0x4), sin rellenar; +0x10 = 1 (BYTE), +0x8 = capacidad, +0xC = -1; con capacidad > 0
    // cada byte del mapa se pone a 0x80 (la lectura-modificacion-escritura `or 0x80` / `and 0x80` deja siempre 0x80); el byte +0x11 y el nombre no se usan. Devuelve `pool` (EAX = ECX). No comprueba que las reservas tengan exito.
    // Funcion propia sin expansion en linea; la llama InitPool como el `call 0x552AF0` del EXE. Se engancha como S.
    __declspec(noinline) static void* __fastcall PoolConstructorExe(void* pool, void* /*edx*/, int32 size, const char* /*name*/) {
        auto* const bytes = static_cast<uint8*>(pool);
        *reinterpret_cast<void**>(bytes + 0x0) = ::operator new(static_cast<size_t>(size) * sizeof(CQuadTreeNode<void*>));
        uint8* const flags = static_cast<uint8*>(::operator new(static_cast<size_t>(size)));
        *reinterpret_cast<uint8**>(bytes + 0x4) = flags;
        bytes[0x10]                            = 1;
        *reinterpret_cast<int32*>(bytes + 0x8) = size;
        *reinterpret_cast<int32*>(bytes + 0xC) = -1;
        for (int32 i = 0; i < size; i++) {
            flags[i] = 0x80;
        }
        return pool;
    }

    static CQuadTreeNodePool* GetPool() {
        return ms_pQuadTreeNodePool;
    }

public:
    CQuadTreeNode(const CRect& size, int32 startLevel) :
        m_ItemList(),
        m_apChildren{ nullptr } {
        m_Rect   = size;
        m_nLevel = startLevel;
    }

    // 0x552830 (lote 192A, sin validar): constructor de CQuadTreeNode del EXE (thiscall, dos argumentos de pila (rectangulo por puntero y nivel), `ret 8`, 63 bytes): copia el rectangulo (+0x0..+0xC) como cuatro dwords, deja la lista
    // de elementos vacia (+0x10 = 0), los cuatro hijos a nulo (+0x14..+0x20) y el nivel (+0x24); devuelve this. Se engancha como S (envoltura sobre el constructor de la clase).
    CQuadTreeNode* Constructor(const CRect& size, int32 startLevel) {
        this->CQuadTreeNode::CQuadTreeNode(size, startLevel);
        return this;
    }

    ~CQuadTreeNode() {
        for (auto& child : m_apChildren) {
            delete child;
        }
        m_ItemList.Flush();
    }

    // 0x552BC0 (lote 175A, banco Fn175A): liberacion de un nodo en el pool de nodos (thiscall, ECX = pool, un argumento de pila, `ret 4`, 46 bytes; sin hook: es el CPool<CQuadTreeNode>::Delete del EXE, instanciacion
    // propia con division por 0x28). Como el original: indice = (nodo - almacenamiento) / 0x28 (con signo, truncado hacia cero, sin comprobar que el nodo sea del pool), estados[indice] |= 0x80 y, si indice < cursor
    // (con signo), cursor = indice. Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama el cuerpo del destructor como el `call 0x552BC0` del EXE.
    __declspec(noinline) static void __fastcall PoolFreeNodeExe(void* pool, void* /*edx*/, void* node) {
        PoolFreeSlotRaw(pool, node, sizeof(CQuadTreeNode<void*>));
    }

    // 0x552520 (lote 150A, banco Fn150A; hook S desde el lote 175A, banco Fn175A): cuerpo del destructor de CQuadTreeNode del EXE (thiscall, sin argumentos de pila, `ret`, 114 bytes, marco SEH que solo desenrolla; el
    // listado decia CPtrNodeDoubleLinkPool). Como el original: para cada uno de los cuatro hijos (+0x14..+0x20, leidos de uno en uno) no nulo, destruye el hijo con este mismo cuerpo (0x552520) y libera su nodo
    // en el pool de nodos ([0xB745BC], leido tras la destruccion) con 0x552BC0; al final vacia la lista simple de elementos (+0x10) con el Flush del EXE (0x552400, con hook). No escribe nada mas ni lee el nivel.
    // Funcion propia sin expansion en linea (el banco la sustituye); la llama el destructor con borrado como el `call 0x552520` del EXE. Un solo cuerpo para todas las instanciaciones: es codigo de plantilla que el
    // EXE comparte, y el enganche se instala solo para T = void*.
    __declspec(noinline) static void __fastcall DestructorBodyExe(CQuadTreeNode* self, void* /*edx*/) {
        for (int32 i = 0; i < 4; i++) {
            CQuadTreeNode* const child = self->m_apChildren[i];
            if (child) {
                DestructorBodyExe(child, nullptr);
                PoolFreeNodeExe(ms_pQuadTreeNodePool, nullptr, child);
            }
        }
        details::CPtrListSingleLink__FlushExe(&self->m_ItemList, nullptr);
    }

    // 0x552C90 (lote 150A, banco Fn150A): liberacion del nodo en el pool (cdecl, un argumento; sin hook: es CQuadTreeNode::operator delete). Funcion propia sin expansion en linea para que el banco pueda
    // sustituirla; la llama el destructor con borrado como el `call 0x552C90` del EXE.
    __declspec(noinline) static void DeleteStorageExe(void* data) {
        CQuadTreeNode::operator delete(data, sizeof(CQuadTreeNode<void*>));
    }

    // 0x404590 (lote 150A, banco Fn150A; el listado decia CRect): destructor con borrado de CQuadTreeNode del EXE (sin vtable ni llamadores; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi;
    // mov esi,ecx; call 0x552520; test byte [esp+8],1; je ..; push esi; call 0x552C90; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase, libera el nodo en el pool
    // solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S.
    static CQuadTreeNode* __fastcall ScalarDeletingDestructorExe(CQuadTreeNode* self, void* /*edx*/, uint8 flags) {
        DestructorBodyExe(self, nullptr);
        if (flags & 1) {
            DeleteStorageExe(self);
        }
        return self;
    }

    // 0x552CD0
    void AddItem(T item, const CRect& rect) {
        if (!m_nLevel) {
            m_ItemList.AddItem(item);
            return;
        }

        for (auto sector = 0; sector < 4; ++sector) {
            if (!InSector(rect, sector)) {
                continue;
            }

            if (!m_apChildren[sector]) {
                const CRect sectorRect = GetSectorRect(sector);
                m_apChildren[sector]   = new CQuadTreeNode(sectorRect, m_nLevel - 1);
            }

            m_apChildren[sector]->AddItem(item, rect);
        }
    }

    // 0x552A40
    void DeleteItem(T item) {
        m_ItemList.DeleteItem(item);

        for (auto& sector : m_apChildren) {
            if (sector) {
                sector->DeleteItem(item);
            }
        }
    }

    // 0x552A90
    void DeleteItem(T item, const CRect& rect) {
        m_ItemList.DeleteItem(item);

        // Como el original: primero InSector, sin comprobar que el hijo exista, y recursión con el mismo rectángulo.
        for (auto sector = 0; sector < 4; ++sector) {
            if (InSector(rect, sector)) {
                m_apChildren[sector]->DeleteItem(item, rect);
            }
        }
    }

    // Returns -1 if not found
    // 0x5525A0
    int32 FindSector(const CRect& rect) {
        const auto center = m_Rect.GetCenter();
        if (!m_nLevel) {
            return -1;
        }

        // Como el original: comparaciones del EXE con NaN (un rectángulo con NaN cae en -1).
        bool lowerHalf;
        if (rect.top < center.y) {
            lowerHalf = false;
        } else {
            if (!(rect.bottom > center.y)) {
                return -1;
            }
            lowerHalf = true;
        }
        if (rect.right < center.x) {
            return lowerHalf ? 0 : 2;
        }
        if (!(rect.left > center.x)) {
            return -1;
        }
        return lowerHalf ? 1 : 3;
    }

    // Returns -1 if not found
    // 0x552640
    int32 FindSector(const CVector2D& posn) {
        const auto center = m_Rect.GetCenter();
        if (!m_nLevel) {
            return -1;
        }

        // Como el original: un NaN en Y cuenta como mitad inferior (0/1) y en X como mitad derecha.
        const bool lowerHalf = !(center.y > posn.y);
        if (center.x > posn.x) {
            return lowerHalf ? 0 : 2;
        }
        return lowerHalf ? 1 : 3;
    }

    void ForAllMatching(const CRect& rect, CQuadTreeNodeRectCallBack callback) {
        for (auto* const item : m_ItemList) {
            callback(rect, item);
        }

        for (auto sector = 0; sector < 4; ++sector) {
            if (m_apChildren[sector] && InSector(rect, sector)) {
                m_apChildren[sector]->ForAllMatching(rect, callback);
            }
        }
    }

    void ForAllMatching(const CVector2D& posn, CQuadTreeNodeVec2DCallBack callback) {
        for (auto* const item : m_ItemList) {
            callback(posn, item);
        }

        const auto sector = FindSector(posn);
        if (sector == -1 || !m_apChildren[sector]) {
            return;
        }

        m_apChildren[sector]->ForAllMatching(posn, callback);
    }

    void GetAll(ListType& list) {
        for (auto* const item : m_ItemList) {
            list.AddItem(item);
        }

        for (auto& sector : m_apChildren) {
            if (sector) {
                sector->GetAll(list);
            }
        }
    }

    // 0x5528C0
    void GetAllMatching(const CRect& rect, ListType& list) {
        for (auto* const item : m_ItemList) {
            list.AddItem(item);
        }

        for (auto sector = 0; sector < 4; ++sector) {
            if (m_apChildren[sector] && InSector(rect, sector)) {
                m_apChildren[sector]->GetAllMatching(rect, list);
            }
        }
    }

    void GetAllMatching(const CVector2D& posn, ListType& list) {
        for (auto* const item : m_ItemList) {
            list.AddItem(item);
        }

        const auto sector = FindSector(posn);
        if (sector == -1 || !m_apChildren[sector]) {
            return;
        }

        m_apChildren[sector]->GetAllMatching(posn, list);
    }

    // 0x5526A0
    bool InSector(const CRect& rect, int32 sector) const {
        if (!m_nLevel) {
            return false;
        }

        // Como el original: las comparaciones se niegan, así que un NaN no descarta el sector.
        const CRect sectorRect = GetSectorRect(sector);
        return !(sectorRect.left > rect.right)
            && !(sectorRect.right < rect.left)
            && !(sectorRect.bottom > rect.top)
            && !(sectorRect.top < rect.bottom);
    }

    CRect GetSectorRect(int32 sector) const {
        CRect      rect   = m_Rect;
        const auto center = rect.GetCenter();
        switch (sector) {
        case 0:
            rect.right  = center.x;
            rect.bottom = center.y;
            break;
        case 1:
            rect.left   = center.x;
            rect.bottom = center.y;
            break;
        case 2:
            rect.right = center.x;
            rect.top   = center.y;
            break;
        case 3:
            rect.left = center.x;
            rect.top  = center.y;
            break;
        default:
            assert(false); // Shouldn't ever get here
        }
        return rect;
    }

    bool LiesInside(const CRect& rect) const {
        return    m_Rect.left <= rect.right
               && m_Rect.right >= rect.left
               && m_Rect.bottom <= rect.top
               && m_Rect.top >= rect.bottom;
    };

public:
    CRect          m_Rect;
    ListType       m_ItemList;
    CQuadTreeNode* m_apChildren[4];
    uint32         m_nLevel; // 0 - last level
};
VALIDATE_SIZE(CQuadTreeNode<void*>, 0x28);

// Como el original (0x552B50 / 0x552C90): el pool de nodos busca a partir del hueco siguiente al último y no
// rellena la memoria al reservar ni al liberar (ver CPool::NewLegacy).
template<typename T>
struct CQuadTreeNodeLegacyPool : CPool<CQuadTreeNode<void*>> {
    CQuadTreeNode<void*>* New() { return NewLegacy(); }
    void Delete(CQuadTreeNode<void*>* node) { DeleteLegacy(node); }
};
