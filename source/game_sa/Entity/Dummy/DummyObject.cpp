/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDummyObject
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "DummyObject.h"
#include "TheScripts.h"

namespace {
CDummyObject* __fastcall DummyObjectConstructorBody(CDummyObject* self, void* edx);
CDummyObject* __fastcall DummyObjectConstructorFromObjectBody(CDummyObject* self, void* edx, CObject* obj);
void __fastcall DummyObjectDestructorBody(CDummyObject* self, void* edx);
void DummyObjectAdoptObject(CDummyObject* self, CObject* obj);
} // namespace

/**
 * Registra la conversión bidireccional entre CDummyObject y CObject real.
 */
void CDummyObject::InjectHooks() {
    RH_ScopedVirtualClass(CDummyObject, 0x866E78, 22);
    RH_ScopedCategory("Entity/Dummy");

    RH_ScopedInstall(ScalarDeletingDestructor, 0x59EB50);
    RH_ScopedInstall(CreateObject, 0x59EAC0);
    RH_ScopedInstall(UpdateFromObject, 0x59EB70);
    RH_ScopedNamedGlobalInstall(DummyObjectConstructorBody, "Constructor", 0x59EA00); // lote 161A (banco Fn161A)
    RH_ScopedNamedGlobalInstall(DummyObjectConstructorFromObjectBody, "ConstructorFromObject", 0x59EA20); // lote 161A (banco Fn161A)
    RH_ScopedNamedGlobalInstall(DummyObjectDestructorBody, "DestructorBody", 0x59EAB0); // lote 161A (banco Fn161A)
}

// 0x59EA00
CDummyObject::CDummyObject() : CDummy() {
    // NOP
}

// 0x59EA20
/**
 * Adopta temporalmente el RwObject de un CObject y conserva IPL/area sin duplicar
 * el recurso RenderWare. El objeto fuente queda separado del RwObject.
 */
CDummyObject::CDummyObject(CObject* obj) : CDummy() {
    DummyObjectAdoptObject(this, obj);
}

// 0x59EAC0
/**
 * Materializa un CObject dinámico a partir del dummy, transfiere su LOD y desactiva
 * visibilidad/colisión del proxy mientras el objeto real queda activo.
 */
CObject* CDummyObject::CreateObject() {
    m_bImBeingRendered = true; // BUG? Seems like that flag doesn't fit here
    auto* obj = CObject::Create(this);
    m_bImBeingRendered = false;

    if (obj) {
        CTheScripts::ScriptsForBrains.CheckIfNewEntityNeedsScript(obj, 1, nullptr);
        SetIsVisible(false);
        SetUsesCollision(false);

        obj->SetLod(GetLod());
        SetLod(nullptr);
    }

    return obj;
}

// 0x59EB70
/**
 * Recupera el RwObject/estado relevante de un CObject antes de volver a usar el dummy
 * como representación de streaming y transfiere de nuevo la relación LOD.
 */
void CDummyObject::UpdateFromObject(CObject* obj) {
    SetIsVisible(true);
    SetUsesCollision(true);

    obj->m_bImBeingRendered = true;
    CEntity::AttachToRwObject(obj->GetRwObject(), false);
    obj->m_bImBeingRendered = false;
    CEntity::UpdateRwMatrix();
    obj->DetachFromRwObject();

    if (obj->GetIplIndex() && CIplStore::HasDynamicStreamingDisabled(obj->GetIplIndex())) {
        m_bRenderDamaged = obj->m_bRenderDamaged;
        SetIsVisible(obj->GetIsVisible());
        SetUsesCollision(obj->GetUsesCollision());
    }

    SetLod(obj->GetLod());
    obj->SetLod(nullptr);
}

namespace {
// 0x59EA00 (lote 161A, banco Fn161A): cuerpo del constructor por defecto de CDummyObject del EXE (thiscall, sin argumentos, `ret`, 18 bytes: `push esi; mov esi,ecx; call 0x532540; mov [esi],0x866E78; mov eax,esi;
// pop esi; ret`): cuerpo del constructor de CDummy (0x532540, con hook) y vtable de CDummyObject del EXE (0x866E78; la de la DLL no vale: las subclases del EXE la sobrescriben); devuelve this.
__declspec(noinline) CDummyObject* __fastcall DummyObjectConstructorBody(CDummyObject* self, void* /*edx*/) {
    CDummyBody::Constructor(self, nullptr);
    *reinterpret_cast<uint32*>(self) = 0x866E78;
    return self;
}

// Pasos del constructor a partir de un CObject (0x59EA3E..0x59EA8F), en el orden del EXE (cada lectura del objeto va justo antes de su uso: si `obj` solapa `this` el resultado depende del orden):
// 1) SetModelIndexNoCreate (0x533700, virtual, llamada directa) con el indice de modelo del objeto (WORD en +0x22, con signo); 2) si el RwObject del objeto (+0x18) no es nulo, AttachToRwObject
// (0x533ED0) con actualizar la matriz; 3) DetachFromRwObject (0x533FB0) del objeto; 4) copia el byte de IPL (+0x2E) y el de area (+0x2F) del objeto a this; 5) CIplStore::IncludeEntity (0x404C90) con el
// byte de IPL (sin signo) y this.
void DummyObjectAdoptObject(CDummyObject* self, CObject* obj) {
    auto* const o = reinterpret_cast<uint8*>(obj);
    auto* const s = reinterpret_cast<uint8*>(self);
    self->CEntity::SetModelIndexNoCreate(static_cast<uint32>(*reinterpret_cast<int16*>(o + 0x22)));
    if (auto* const rw = *reinterpret_cast<RwObject**>(o + 0x18)) {
        self->CEntity::AttachToRwObject(rw, true);
    }
    obj->CEntity::DetachFromRwObject();
    const uint8 iplIndex = *(o + 0x2E);
    *(s + 0x2E) = iplIndex;
    *(s + 0x2F) = *(o + 0x2F);
    CIplStore::IncludeEntity(iplIndex, self);
}

// 0x59EA20 (lote 161A, banco Fn161A): constructor de CDummyObject a partir de un CObject del EXE (thiscall, un argumento de pila, `ret 4`, 135 bytes, marco SEH que solo desenrolla): cuerpo del constructor de CDummy
// (0x532540, con hook), vtable de CDummyObject del EXE (0x866E78) y los pasos de DummyObjectAdoptObject. Devuelve this. El constructor de C++ usa los mismos pasos (antes pasaba al SetModelIndexNoCreate el indice de
// modelo del propio dummy, que acababa de construirse, en lugar del del objeto).
__declspec(noinline) CDummyObject* __fastcall DummyObjectConstructorFromObjectBody(CDummyObject* self, void* /*edx*/, CObject* obj) {
    CDummyBody::Constructor(self, nullptr);
    *reinterpret_cast<uint32*>(self) = 0x866E78;
    DummyObjectAdoptObject(self, obj);
    return self;
}

// 0x59EAB0 (lote 125A, banco Fn125A; con hook propio desde el lote 161A, banco Fn161A): cuerpo del destructor de CDummyObject del EXE (thiscall, sin argumentos, `ret`, 11 bytes: `mov dword [ecx],0x866E78;
// jmp 0x532560`): vtable de CDummyObject del EXE y salto al cuerpo del destructor de CDummy (0x532560, con hook).
// Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama ScalarDeletingDestructor como el `call 0x59EAB0` del EXE.
__declspec(noinline) void __fastcall DummyObjectDestructorBody(CDummyObject* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x866E78;
    CDummyBody::Destructor(self, nullptr);
}
} // namespace

uintptr_t Lote125ADummyObjectDestructorBody() { return reinterpret_cast<uintptr_t>(&DummyObjectDestructorBody); } // para el banco Fn125A

// 0x59EB50 (lote 125A, banco Fn125A) : destructor con borrado de CDummyObject del EXE (ranura 0 de su vtable 0x866E78; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x59EAB0; test byte [esp+8],1; je ..; push esi; call 0x5326F0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x59EAB0; aqui su envoltura Destructor()), libera `this` con CDummy::operator delete (0x5326F0, sin hook: pool de dummies 0xB744A0) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. 
CDummyObject* CDummyObject::ScalarDeletingDestructor(uint8 flags) {
    DummyObjectDestructorBody(this, nullptr);
    if (flags & 1) {
        CDummy::operator delete(this);
    }
    return this;
}
