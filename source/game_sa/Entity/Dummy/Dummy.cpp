/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDummy
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Dummy.h"

/**
 * Registra la validación de punteros para entidades CDummy.
 */
namespace {
CDummy* __fastcall DummyConstructorBody(CDummy* self, void* edx);
void __fastcall DummyDestructorBody(CDummy* self, void* edx);
} // namespace

void CDummy::InjectHooks() {
    RH_ScopedVirtualClass(CDummy, 0x8638C0, 22);
    RH_ScopedCategory("Entity/Dummy");

    RH_ScopedGlobalInstall(IsDummyPointerValid, 0x532730);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x532770); // lote 130A (banco Fn130A)
    RH_ScopedNamedGlobalInstall(DummyConstructorBody, "Constructor", 0x532540); // lote 141A (banco Fn141A)
    RH_ScopedNamedGlobalInstall(DummyDestructorBody, "Destructor", 0x532560); // lote 141A (banco Fn141A)
}

namespace {
// 0x532560 (lote 130A, banco Fn130A): cuerpo del destructor de CDummy del EXE, sin hook propio (`mov dword [ecx],0x8638C0; jmp CEntity::Destructor (0x535E90)`, thiscall sin argumentos). Funcion propia
// sin expansion en linea para que el banco pueda sustituirla; la llama ScalarDeletingDestructor como el `call 0x532560` del EXE.
// Desde el lote 141A (banco Fn141A) tiene hook propio (0x532560, tipo S, nombre "Destructor"); llama a la envoltura CEntity::Destructor() (0x535E90, con hook, sin expansion en linea).
__declspec(noinline) void __fastcall DummyDestructorBody(CDummy* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x8638C0;
    static_cast<CEntity*>(self)->Destructor();
}

// 0x532540 (lote 141A, banco Fn141A): cuerpo del constructor de CDummy del EXE (thiscall, sin argumentos, `ret`, 28 bytes: `push esi; mov esi,ecx; call 0x532A90; mov al,[esi+0x36]; and al,0xFD;
// or al,5; mov [esi+0x36],al; mov dword [esi],0x8638C0; mov eax,esi; pop esi; ret`). Base CEntity (0x532A90, con hook), tipo del byte +0x36 = dummy (bit 1 a cero, bits 0 y 2 a uno, resto
// intacto), vtable de CDummy del EXE (0x8638C0); devuelve this. Lo llaman CDummyObject (0x59EA03, 0x59EA3E) y 0x5DE2E3.
__declspec(noinline) CDummy* __fastcall DummyConstructorBody(CDummy* self, void* /*edx*/) {
    static_cast<CEntity*>(self)->Constructor();
    uint8* const flags = reinterpret_cast<uint8*>(self) + 0x36;
    *flags = uint8((*flags & 0xFDu) | 5u);
    *reinterpret_cast<uint32*>(self) = 0x8638C0;
    return self;
}
} // namespace

uintptr_t Lote130ADummyDestructorBody() { return reinterpret_cast<uintptr_t>(&DummyDestructorBody); } // para el banco Fn130A

// 0x532770 (lote 130A, banco Fn130A): destructor con borrado de CDummy del EXE (ranura 0 de su vtable 0x8638C0; thiscall, un argumento de pila, `ret 4`, 78 bytes). Como el original: destruye con el
// cuerpo de la clase (0x532560) y, solo si el bit 0 del BYTE de banderas esta puesto, libera `this` en linea (CDummy::operator delete 0x5326F0 expandido, sin asserts): indice = (this - almacenamiento)
// / 56 (division con signo, truncada hacia cero) sobre CPools::ms_pDummyPool (0xB744A0); estados[indice] |= 0x80 y, si indice < +0xC (con signo), +0xC = indice. Devuelve this.
CDummy* CDummy::ScalarDeletingDestructor(uint8 flags) {
    DummyDestructorBody(this, nullptr);
    if (flags & 1) {
        auto* const pool    = reinterpret_cast<uint8*>(GetDummyPool());
        auto* const storage = *reinterpret_cast<uint8**>(pool + 0x0);
        const int32 idx     = int32(reinterpret_cast<uintptr_t>(this) - reinterpret_cast<uintptr_t>(storage)) / int32(sizeof(CDummy));
        *(*reinterpret_cast<uint8**>(pool + 0x4) + idx) |= 0x80;
        if (idx < *reinterpret_cast<int32*>(pool + 0xC)) {
            *reinterpret_cast<int32*>(pool + 0xC) = idx;
        }
    }
    return this;
}

// 0x5326E0
// Obtiene almacenamiento del pool específico de dummies.
void* CDummy::operator new(size_t size) {
    return GetDummyPool()->New();
}

// 0x5326F0
void CDummy::operator delete(void* obj) {
    GetDummyPool()->Delete(static_cast<CDummy*>(obj));
}

// 0x532540
CDummy::CDummy() : CEntity() {
    SetTypeDummy();
}

// 0x532730
// Como el original: no mira si el slot está ocupado; sólo calcula el índice con división con signo
// (truncada hacia cero) sobre la diferencia de bytes y acepta 0 <= índice <= m_nSize (el `<=` es del EXE).
bool IsDummyPointerValid(CDummy* dummy) {
    if (!dummy) {
        return false;
    }
    const auto* const pool    = GetDummyPool();
    const auto* const storage = *reinterpret_cast<const uint8* const*>(pool); // CPool::m_pObjects (+0x0)
    const int32 idx = int32(reinterpret_cast<uintptr_t>(dummy) - reinterpret_cast<uintptr_t>(storage)) / int32(sizeof(CDummy));
    return idx >= 0 && idx <= *reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(pool) + 0x8); // CPool::m_nSize
}
