/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventSoundQuiet
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

#include "EventSoundQuiet.h"

namespace CEventEditableBody {
void __fastcall EditableResponseDestructorBody(CEventEditableResponse* self, void* edx); // EventEditableResponse.cpp: 0x4AC480
}

namespace {
// 0x5DEA00 (lote 165A, banco Fn165A): cuerpo no virtual de ~CEventSoundQuiet del EXE (thiscall, sin argumentos de pila, `ret`, 90 bytes con marco SEH que solo desenrolla y aqui se omite). Deja la vtable de
// CEventSoundQuiet del EXE (0x86C2B0), libera con CEntity::CleanUpOldReference (0x571A00) la referencia no nula en +0x14 (ECX = la entidad, argumento = la direccion de la casilla) y llama al cuerpo de
// ~CEventEditableResponse (0x4AC480, con hook), que deja la vtable de CEvent. Funcion propia sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall SoundQuietDestructorBody(CEventSoundQuiet* self, void* /*edx*/) {
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(bytes) = 0x86C2B0;
    CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0x14));
    CEventEditableBody::EditableResponseDestructorBody(self, nullptr);
}

// 0x5DEAB0 (lote 165A, banco Fn165A): destructor con borrado de CEventSoundQuiet del EXE (ranura 0 de su vtable 0x86C2B0; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call
// 0x5DEA00; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5DEA00), libera `this` con CEvent::operator
// delete (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CEventSoundQuiet* __fastcall SoundQuietScalarDeletingDestructor(CEventSoundQuiet* self, void* /*edx*/, uint8 flags) {
    SoundQuietDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote165ABody_CEventSoundQuiet() { return reinterpret_cast<uintptr_t>(&SoundQuietDestructorBody); } // para el banco Fn165A

void CEventSoundQuiet::InjectHooks()
{
    RH_ScopedVirtualClass(CEventSoundQuiet, 0x86C2B0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(SoundQuietDestructorBody, "Destructor", 0x5DEA00);                                  // lote 165A (banco Fn165A)
    RH_ScopedNamedGlobalInstall(SoundQuietScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5DEAB0);          // lote 165A
    RH_ScopedVMTInstall(GetEventType, 0x5DEA60);                // lote 165A: ranura 1 de la vtable 0x86C2B0
    RH_ScopedVMTInstall(GetEventPriority, 0x5DEAD0);            // lote 165A: ranura 2
    RH_ScopedVMTInstall(GetLifeTime, 0x5DEA70);                 // lote 165A: ranura 3
    RH_ScopedVMTInstall(GetSourceEntity, 0x5DEA80);             // lote 165A
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x5DEA90);          // lote 165A
    RH_ScopedVMTInstall(CanBeInterruptedBySameEvent, 0x5DEAA0); // lote 165A

    RH_ScopedInstall(Constructor, 0x5E05B0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B5240);
    RH_ScopedVMTInstall(CloneEditable, 0x5E0670);
}

// 0x5E05B0
CEventSoundQuiet::CEventSoundQuiet(CEntity* entity, float fLocalSoundLevel, uint32 startTime, const CVector& position)
{
    m_fLocalSoundLevel = fLocalSoundLevel;
    m_startTimeInMs = startTime;
    m_entity = entity;
    m_position = position;
    CEntity::SafeRegisterRef(m_entity);
    if (m_startTimeInMs == (uint32)(-1)) {
        m_startTimeInMs = CTimer::GetTimeInMS();
        m_position = m_entity->GetPosition();
    }
}

CEventSoundQuiet::~CEventSoundQuiet()
{
    CEntity::SafeCleanUpRef(m_entity);
}

CEventSoundQuiet* CEventSoundQuiet::Constructor(CEntity* entity, float fLocalSoundLevel, uint32 startTime, const CVector& position)
{
    this->CEventSoundQuiet::CEventSoundQuiet(entity, fLocalSoundLevel, startTime, position);
    return this;
}

// 0x4B5240
bool CEventSoundQuiet::AffectsPed(CPed* ped)
{
    if (ped->IsPlayer() || !ped->IsAlive()|| !GetSourceEntity())
        return false;

    float soundLevel = GetSoundLevel(nullptr, ped->GetPosition());
    if (soundLevel < 30.0f)
        return false;

    // Como el original: el tipo y la entidad origen del evento actual se piden por su vtable. Con un puntero a
    // CEventSoundQuiet (clase final) el compilador desvirtualizaba GetEventType (siempre 62) y GetSourceEntity
    // (lectura directa de +0x14) aunque el evento actual fuese de otra clase.
    CEvent* const current = ped->GetEventHandlerHistory().GetCurrentEvent();
    if (current &&
        current->GetEventType() == EVENT_SOUND_QUIET && current->GetSourceEntity() == GetSourceEntity()) {
        const auto currentEvent = static_cast<CEventSoundQuiet*>(current);
        CVector distance = m_position - currentEvent->m_position;
        // Como el original: diferencia de tiempo con signo y un NaN en la distancia cuenta como "lejos".
        if ((int32)(m_startTimeInMs - currentEvent->m_startTimeInMs) >= 2000 && !(distance.SquaredMagnitude() < 1.0f))
            return true;
        if (soundLevel - currentEvent->GetSoundLevel(nullptr, ped->GetPosition()) < 3.0f)
            return false;
    }
    return true;
}

// 0x5DEA60 (lote 165A, banco Fn165A; thiscall, `ret`, 5 bytes: `mov eax,3Eh`): EVENT_SOUND_QUIET (62); no lee el objeto.
eEventType CEventSoundQuiet::GetEventType() const {
    return EVENT_SOUND_QUIET;
}

// 0x5DEAD0 (lote 165A, banco Fn165A; thiscall, `ret`, 6 bytes: `mov eax,8`): prioridad 8; no lee el objeto.
int32 CEventSoundQuiet::GetEventPriority() const {
    return 8;
}

// 0x5DEA70 (lote 165A, banco Fn165A; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto.
int32 CEventSoundQuiet::GetLifeTime() {
    return 0;
}

// 0x5DEA80 (lote 165A, banco Fn165A; thiscall, `ret`, 4 bytes: `mov eax,[ecx+14h]`): la entidad origen (m_entity) tal cual, sin comprobar nada.
CEntity* CEventSoundQuiet::GetSourceEntity() const {
    return m_entity;
}

// 0x5DEA90 (lote 165A, banco Fn165A; thiscall, `ret`, 4 bytes: `fld dword [ecx+18h]`): el nivel de sonido local (m_fLocalSoundLevel) cargado en ST0.
float CEventSoundQuiet::GetLocalSoundLevel() {
    return m_fLocalSoundLevel;
}

// 0x5DEAA0 (lote 165A, banco Fn165A; thiscall, `ret`, 3 bytes: `mov al,1`): siempre verdadero; no lee el objeto.
bool CEventSoundQuiet::CanBeInterruptedBySameEvent() {
    return true;
}

// 0x5E0670
CEventEditableResponse* CEventSoundQuiet::CloneEditable() const noexcept
{
    return new CEventSoundQuiet(m_entity, m_fLocalSoundLevel, m_startTimeInMs, m_position);
}
