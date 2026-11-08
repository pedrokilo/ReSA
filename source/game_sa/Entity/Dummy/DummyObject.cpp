/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDummyObject
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

#include "DummyObject.h"
#include "TheScripts.h"

/**
 * Registra la conversión bidireccional entre CDummyObject y CObject real.
 */
void CDummyObject::InjectHooks() {
    RH_ScopedVirtualClass(CDummyObject, 0x866E78, 22);
    RH_ScopedCategory("Entity/Dummy");

    RH_ScopedInstall(ScalarDeletingDestructor, 0x59EB50);
    RH_ScopedInstall(CreateObject, 0x59EAC0);
    RH_ScopedInstall(UpdateFromObject, 0x59EB70);
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
    CEntity::SetModelIndexNoCreate(GetModelIndex());
    if (obj->GetRwObject())
        CEntity::AttachToRwObject(obj->GetRwObject(), true);

    obj->DetachFromRwObject();
    SetIplIndex(obj->GetIplIndex());
    SetAreaCode(obj->GetAreaCode());
    CIplStore::IncludeEntity(GetIplIndex(), this);
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
// 0x59EAB0 (lote 125A, banco Fn125A): cuerpo del destructor de CDummyObject del EXE (vtable 0x866E78 y salto al destructor de la base; sin hook propio: el destructor de la clase es el generado por el compilador).
// Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama ScalarDeletingDestructor como el `call 0x59EAB0` del EXE.
__declspec(noinline) void __fastcall DummyObjectDestructorBody(CDummyObject* self, void* /*edx*/) {
    self->CDummyObject::~CDummyObject();
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
