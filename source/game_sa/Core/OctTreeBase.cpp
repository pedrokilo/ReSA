/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: COctTreeBase
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "OctTreeBase.h"

auto& gOctTreeBase = StaticRef<COctTreeBase>(0xBC1290);

namespace {
// 0x5A7570 (lote 203A, sin validar; thiscall, sin argumentos de pila, `ret`, 41 bytes, sin llamadores directos salvo el inicializador de CRT 0x84FE25 del objeto global gOctTreeBase): constructor por defecto de COctTreeBase del EXE.
// Es el cuerpo de COctTree::COctTree en linea (nivel +4 y las tres sumas +0x1C, +0x20 y +0x24 a cero, los ocho hijos a -1 y el BYTE +8 a cero) pero con la vtable de COctTreeBase 0x867178; NO toca +0x28 (m_nNumBranches) ni +0x2C
// (m_bHasTransparentPixels). Devuelve `this`. Inicializacion por defecto (sin poner a cero los miembros propios) y despues la vtable del EXE.
COctTreeBase* __fastcall OctTreeBaseConstructorExe(COctTreeBase* self, void* /*edx*/) {
    ::new (static_cast<void*>(self)) COctTreeBase;
    *reinterpret_cast<uint32*>(self) = 0x867178;
    return self;
}
} // namespace

void COctTreeBase::InjectHooks() {
    RH_ScopedClass(COctTreeBase);
    RH_ScopedCategory("Core");

    RH_ScopedNamedGlobalInstall(OctTreeBaseConstructorExe, "Constructor", 0x5A7570); // lote 203A (sin validar)
}

// 0x5A7710
bool COctTreeBase::InsertTree(uint8 red, uint8 green, uint8 blue) {
    ms_level = 0;

    if (!COctTree::InsertTree(red, green, blue))
        return false;

    m_nNumBranches--;

    return true;
}

// 0x5A7280
void COctTreeBase::FillPalette(uint8* data) {
    ms_level = 0;

    if (m_bHasTransparentPixels) {
        auto* colors = *reinterpret_cast<RwRGBA**>(data);
        colors->red   = 0;
        colors->green = 0;
        colors->blue  = 0;
        colors->alpha = 0;

        ms_level = 1;
    }

    COctTree::FillPalette(data);
}

// 0x5A7260
void COctTreeBase::Init(int32 numBranches) {
    ms_level = 0;

    m_nNumBranches = numBranches;
    m_bHasTransparentPixels = 0;

    empty();
}

// 0x5A7750
bool COctTreeBase::Insert(uint8 red, uint8 green, uint8 blue) {
    while (true) {
        ms_bFailed = false;

        if (InsertTree(red, green, blue))
            break;

        if (ms_bFailed) {
            Reduce();
            continue;
        }

        return false;
    }

    if (m_nNumBranches <= 0) {
        Reduce();
    }

    return true;
}

// 0x5A7840
void COctTreeBase::ReduceBranches(int32 newBranchesCount) {
    const int32 branchesToCount = m_bHasTransparentPixels ? newBranchesCount + 1 : newBranchesCount;

    // Signed-unsigned conversion check
    // TODO: Replace this with the overflow-check cast in notsa::script.
    assert(m_nNumBranches < static_cast<uint32>(std::numeric_limits<int32>::max()));
    while (static_cast<int32>(m_nNumBranches) < branchesToCount) {
        Reduce();
    }
}

void COctTreeBase::Reduce() {
    gpTmpOctTree = nullptr;
    ReduceTree();
    m_nNumBranches += gpTmpOctTree->NoOfChildren() - 1;

    gpTmpOctTree->m_bLastStep = true;
    gpTmpOctTree->RemoveChildren();
}
