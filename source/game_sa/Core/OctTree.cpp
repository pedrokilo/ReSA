/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: COctTree
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "OctTree.h"
#include "PoolRaw.h"
auto& gpTmpOctTree = StaticRef<COctTree*>(0xBC12D8);

// Lote 203A (sin validar). Funciones del EXE de COctTree (arbol de octantes de la paleta de texturas; objetos de 0x28 bytes en el pool 0xBC12E4). Disposicion: +0 vtable (0x867170 para COctTree, 0x867178 para COctTreeBase),
// +4 nivel, +8 BYTE "ultimo paso", +0xA ocho indices de hijo de 16 bits (-1 = vacio), +0x1C/+0x20/+0x24 sumas de rojo/verde/azul. La vtable del EXE (0x867170) tiene solo dos ranuras: InsertTree (0x5A75B0) y FillPalette (0x5A70F0), no hay
// destructor virtual: 0x5A7490 es un destructor con borrado de uso directo.
namespace {
constexpr uint32 OCTTREE_EXE_VTABLE = 0x867170;

// Los objetos que reserva o libera el EXE llevan su vtable (la de la DLL no vale: el EXE los despacha por la suya).
void SetExeVtable(COctTree* tree) {
    *reinterpret_cast<uint32*>(tree) = OCTTREE_EXE_VTABLE;
}

// Cuerpo comun de 0x5A74F0 (RemoveChildren) y del bucle en linea de 0x5A6FC0 (empty): para cada hijo con indice >= 0 (con signo de 16 bits; los negativos ni se tocan) cuya ranura del pool este ocupada (bit 7 del estado apagado)
// y no sea nula, deja la vtable del EXE en el hijo, lo vacia (0x5A6FC0, recursivo) y libera su ranura en el pool (0x5A73C0 en empty; en linea en RemoveChildren: mismas escrituras). El indice del hijo se pone a -1 por todas las salidas
// (tambien con la ranura ya libre).
void ReleaseChildren(COctTree* self) {
    for (int16& child : self->m_aChildrens) {
        if (child < 0) {
            continue;
        }
        if (COctTree* const elem = COctTree::ms_octTreePool.GetAt(child)) {
            SetExeVtable(elem);
            elem->empty();
            notsa::pool_raw::ReleaseSlot(&COctTree::ms_octTreePool, elem, sizeof(COctTree)); // sin las comprobaciones de CPool::Delete (ver PoolRaw.h)
        }
        child = -1;
    }
}

// 0x5A6DB0 (thiscall, sin argumentos de pila, `ret`, 41 bytes): constructor de COctTree del EXE. Vtable 0x867170, nivel (+4) y las tres sumas (+0x1C, +0x20, +0x24) a cero, los ocho hijos (+0xA, cuatro dwords) a -1 y el BYTE +8 a cero.
// Devuelve `this`. No lo llaman destructores: lo usa InsertTree (0x5A75B0) tras reservar la ranura.
COctTree* __fastcall OctTreeConstructorExe(COctTree* self, void* /*edx*/) {
    ::new (static_cast<void*>(self)) COctTree; // mismas escrituras que el EXE (ver COctTree::COctTree) y despues la vtable del EXE
    SetExeVtable(self);
    return self;
}

// 0x5A7490 (thiscall, un argumento de pila, `ret 4`, 83 bytes): destructor con borrado de COctTree del EXE (no es de la vtable). Deja la vtable 0x867170, vacia el arbol (0x5A6FC0) y, con el bit 0 del BYTE de banderas puesto, libera la
// ranura de `this` en el pool en linea (estado |= 0x80; cursor = min(cursor, indice) con signo). Devuelve `this`.
COctTree* __fastcall OctTreeScalarDeletingDestructorExe(COctTree* self, void* /*edx*/, uint8 flags) {
    SetExeVtable(self);
    self->empty();
    if (flags & 1) {
        notsa::pool_raw::ReleaseSlot(&COctTree::ms_octTreePool, self, sizeof(COctTree));
    }
    return self;
}

// 0x5A75B0 (thiscall, tres argumentos de pila, `ret 0Ch`, 339 bytes; ranura 0 de la vtable 0x867170 y llamada directa desde COctTreeBase::InsertTree 0x5A7710; el hook es S para cubrir las dos entradas): inserta un color (octante por los bits
// del nivel actual: ((rojo << nivel) >> 5 & 4) + ((verde << nivel) >> 6 & 2) + ((azul << nivel) >> 7 & 1), desplazamientos de 32 bits con la cuenta enmascarada a 5 bits y SAR con signo).
//  1. Siempre: ms_level (0xBC12E0) += 1; sumas de rojo, verde y azul += cada canal; +4 (nivel) += 1.
//  2. Si ms_level == 8 o el BYTE +8 vale EXACTAMENTE 1: ms_level = 0, BYTE +8 = 1, RemoveChildren (0x5A74F0) y devuelve (nivel +4) == 1.
//  3. Si no: con el hijo ya asignado (indice >= 0 con signo) toma la ranura del pool (nulo si esta libre: el EXE no lo comprueba y despacha igualmente, con fallo si es nulo); sin hijo reserva una ranura (0x5A7350), la construye (0x5A6DB0) y
//     guarda su indice (ptr - datos) / 0x28; sin ranura pone ms_bFailed (0xBC12DC) a 1 y devuelve false (ms_level se queda incrementado).
//  4. Llama al InsertTree VIRTUAL (ranura 0) del hijo con los mismos tres canales. Si ms_bFailed esta puesto y el nivel del hijo (+4, comparacion CON SIGNO) es menor que 2: destruye el hijo con borrado (0x5A7490 con 1), su indice a -1 y devuelve false.
//     En cualquier otro caso devuelve lo que devolvio el hijo.
bool __fastcall OctTreeInsertTreeExe(COctTree* self, void* /*edx*/, uint8 red, uint8 green, uint8 blue) {
    const int32 shift = static_cast<int32>(COctTree::ms_level & 31u);
    const int32 childIdx = (((int32(green) << shift) >> 6) & 2) + (((int32(blue) << shift) >> 7) & 1) + (((int32(red) << shift) >> 5) & 4);
    COctTree::ms_level += 1;

    self->m_nRedComponent += red;
    self->m_nGreenComponent += green;
    self->m_nBlueComponent += blue;
    self->m_nLevel += 1;

    if (COctTree::ms_level == 8 || *reinterpret_cast<const uint8*>(&self->m_bLastStep) == 1) {
        COctTree::ms_level = 0;
        self->m_bLastStep = true;
        self->RemoveChildren();
        return self->m_nLevel == 1;
    }

    COctTree* child;
    if (self->m_aChildrens[childIdx] >= 0) {
        child = COctTree::ms_octTreePool.GetAt(self->m_aChildrens[childIdx]);
    } else {
        child = COctTree::ms_octTreePool.New();
        if (child) {
            child = OctTreeConstructorExe(child, nullptr);
        }
        if (!child) {
            COctTree::ms_bFailed = true;
            return false;
        }
        self->m_aChildrens[childIdx] = static_cast<int16>(COctTree::ms_octTreePool.GetIndex(child));
    }

    const bool inserted = child->InsertTree(red, green, blue); // virtual, como el EXE (call [vtable])

    if (COctTree::ms_bFailed && static_cast<int32>(child->m_nLevel) < 2) {
        OctTreeScalarDeletingDestructorExe(child, nullptr, 1);
        self->m_aChildrens[childIdx] = -1;
        return false;
    }
    return inserted;
}

// 0x5A7350 (thiscall, sin argumentos de pila, `ret`, 104 bytes): CPool<COctTree>::New del EXE (lo salta COctTree::operator new 0x5A7410). Busca desde cursor + 1 (con una sola vuelta completa), marca la ranura como usada y suma 1 a su identificador de 7 bits;
// sin hueco devuelve nulo con el cursor a 0. No toca el objeto. Misma politica que CPool::New de la DLL.
COctTree* __fastcall OctTreePoolNewExe(COctTreePool* pool, void* /*edx*/) {
    return pool->New();
}

// 0x5A73C0 (thiscall, un argumento de pila, `ret 4`, 49 bytes): CPool<COctTree>::Delete del EXE: indice = (puntero - datos) / 0x28; estado |= 0x80; cursor = min(cursor, indice) con signo; sin comprobar nulo ni el rango.
void __fastcall OctTreePoolDeleteExe(COctTreePool* pool, void* /*edx*/, COctTree* obj) {
    notsa::pool_raw::ReleaseSlot(pool, obj, sizeof(COctTree));
}

// 0x5A6E60 y 0x5A72B0 (thiscall, sin argumentos de pila, `ret`, 54 bytes cada una, sin llamadores ni referencias: copias muertas de la plantilla CPool<T>::Flush; la misma que el codigo en linea de ShutdownPool 0x5A6F70 pero con el cursor y el
// contador a cero): con el contador (+8, int32 CON SIGNO) mayor que cero: si el BYTE +0x10 (es duena) es distinto de cero libera el bloque de datos (+0) y despues el de estados (+4) con operator delete (0x8214BD) sin comprobar nulos, y SIEMPRE
// pone +0, +4, +8 y +0xC a cero en ese orden. No toca el BYTE +0x10 ni +0x11. Cada direccion lleva su propia funcion porque un hook Simple reescribe el comienzo de la funcion de la DLL.
__declspec(noinline) void PoolFlushBody(void* self) {
    auto* const bytes = reinterpret_cast<uint8*>(self);
    if (*reinterpret_cast<int32*>(bytes + 8) > 0) {
        if (bytes[0x10] != 0) {
            ::operator delete(*reinterpret_cast<void**>(bytes + 0));
            ::operator delete(*reinterpret_cast<void**>(bytes + 4));
        }
        *reinterpret_cast<uint32*>(bytes + 0x0) = 0;
        *reinterpret_cast<uint32*>(bytes + 0x4) = 0;
        *reinterpret_cast<uint32*>(bytes + 0x8) = 0;
        *reinterpret_cast<uint32*>(bytes + 0xC) = 0;
    }
}
// 0x5A6E60
__declspec(noinline) void __fastcall PoolFlush5A6E60(void* self, void* /*edx*/) { PoolFlushBody(self); }
// 0x5A72B0
__declspec(noinline) void __fastcall PoolFlush5A72B0(void* self, void* /*edx*/) { PoolFlushBody(self); }
} // namespace

void COctTree::InjectHooks() {
    RH_ScopedClass(COctTree);
    RH_ScopedCategory("Core");

    RH_ScopedNamedGlobalInstall(OctTreeConstructorExe, "Constructor", 0x5A6DB0);                           // lote 203A (sin validar)
    RH_ScopedNamedGlobalInstall(PoolFlush5A6E60, "PoolFlush5A6E60", 0x5A6E60);                             // lote 203A (sin validar)
    RH_ScopedInstall(empty, 0x5A6FC0);                                                                       // lote 203A (sin validar)
    RH_ScopedNamedGlobalInstall(PoolFlush5A72B0, "PoolFlush5A72B0", 0x5A72B0);                             // lote 203A (sin validar)
    RH_ScopedNamedGlobalInstall(OctTreePoolNewExe, "PoolNew", 0x5A7350);                                   // lote 203A (sin validar)
    RH_ScopedNamedGlobalInstall(OctTreePoolDeleteExe, "PoolDelete", 0x5A73C0);                             // lote 203A (sin validar)
    RH_ScopedNamedGlobalInstall(OctTreeScalarDeletingDestructorExe, "ScalarDeletingDestructor", 0x5A7490); // lote 203A (sin validar)
    RH_ScopedInstall(RemoveChildren, 0x5A74F0);                                                              // lote 203A (sin validar)
    RH_ScopedNamedGlobalInstall(OctTreeInsertTreeExe, "InsertTree", 0x5A75B0);                             // lote 203A (sin validar)
}

// 0x5A6DB0
COctTree::COctTree() {
    m_nLevel = 0;
    m_nRedComponent = 0;
    m_nGreenComponent = 0;
    m_nBlueComponent = 0;
    std::ranges::fill(m_aChildrens, -1);
    m_bLastStep = false;
}

// 0x5A7490
COctTree::~COctTree() {
    empty();
}

// 0x5A7410
void* COctTree::operator new(size_t size) {
    return ms_octTreePool.New();
}

// 0x5A7420
void COctTree::operator delete(void* data) {
    ms_octTreePool.Delete(static_cast<COctTree*>(data));
}

// 0x5A7460
void COctTree::InitPool(void* data, int32 dataSize) {
    const auto cap = dataSize / (sizeof(COctTree) + 1);
    ms_octTreePool = {
        cap,
        data,
        (uint8*)(data) + sizeof(COctTree) * cap
    };
    ms_octTreePool.SetDealWithNoMemory(true);
}

// 0x5A6F70
void COctTree::ShutdownPool() {
    ms_octTreePool.Flush();
}

// 0x5A75B0 (lote 203A, sin validar): el cuerpo del EXE esta en OctTreeInsertTreeExe (hook S: la entrada se llama tambien de forma directa desde COctTreeBase::InsertTree 0x5A7710).
bool COctTree::InsertTree(uint8 red, uint8 green, uint8 blue) {
    return OctTreeInsertTreeExe(this, nullptr, red, green, blue);
}

// 0x5A70F0
void COctTree::FillPalette(uint8* data) {
    if (m_bLastStep) {
        auto* colors = *reinterpret_cast<RwRGBA**>(data);
        colors[ms_level].red   = m_nRedComponent   / m_nLevel;
        colors[ms_level].green = m_nGreenComponent / m_nLevel;
        colors[ms_level].blue  = m_nBlueComponent  / m_nLevel;
        colors[ms_level].alpha = 128;
        m_nLevel = ms_level++;
        return;
    }

    for (auto& children : m_aChildrens) {
        if (auto elem = ms_octTreePool.GetAt(children)) {
            elem->FillPalette(data);
        }
    }
}

// 0x5A71E0
uint32 COctTree::FindNearestColour(uint8 red, uint8 green, uint8 blue) {
    if (m_bLastStep) {
        return m_nLevel;
    }

    COctTree* treeElement = this;
    do {
        auto child = ((red >> 5) & 4) + ((green >> 6) & 2) + (blue >> 7); // todo: magic
        auto treeIndex = treeElement->m_aChildrens[child];
        treeElement = ms_octTreePool.GetAt(treeIndex);
        // useless permutation
        red   *= 2;
        green *= 2;
        blue  *= 2;
    } while (!treeElement->m_bLastStep);

    return treeElement->m_nLevel;
}

// 0x5A6DE0
uint32 COctTree::NoOfChildren() {
    uint32 numOfChildren = 0;
    for (auto& m_aChildren : m_aChildrens) {
        if (m_aChildren >= 0) {
            numOfChildren++;
        }
    }
    return numOfChildren;
}

// 0x5A7040
void COctTree::ReduceTree() {
    if (m_bLastStep) {
        return;
    }

    ms_level++;

    uint32 currentLevel = 0;
    uint32 totalLevels = 0;
    static uint32 lastLevel;
    for (auto& children : m_aChildrens) {
        if (children < 0) {
            continue;
        }
        currentLevel++;
        if (COctTree* tree = ms_octTreePool.GetAt(children)) {
            tree->ReduceTree();
            totalLevels += tree->m_nLevel;
        }
    }

    if (currentLevel >= 2 && (totalLevels < lastLevel || !gpTmpOctTree)) {
        lastLevel = totalLevels;
        gpTmpOctTree = this;
    }

    ms_level--;
}

// 0x5A74F0 (lote 203A, sin validar; thiscall, sin argumentos, `ret`, 125 bytes): libera todos los hijos (ver ReleaseChildren). No toca el nivel, las sumas ni el BYTE "ultimo paso".
void COctTree::RemoveChildren() {
    ReleaseChildren(this);
}

// 0x5A6FC0 (lote 203A, sin validar; thiscall, sin argumentos, `ret`, 122 bytes): nivel (+4) y las tres sumas (+0x1C, +0x20, +0x24) a cero (en ese orden), libera los hijos (el EXE lleva el bucle en linea y llama a 0x5A73C0 para cada
// ranura; ver ReleaseChildren) y por ultimo pone el BYTE +8 (ultimo paso) a cero.
void COctTree::empty() {
    m_nLevel = 0;
    m_nRedComponent = 0;
    m_nGreenComponent = 0;
    m_nBlueComponent = 0;
    ReleaseChildren(this);
    m_bLastStep = false;
}
