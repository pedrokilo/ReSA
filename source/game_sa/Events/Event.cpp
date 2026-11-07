/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEvent
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

// Sin PCH (CMakeLists.txt): StdInc.h redefine NOMINMAX y el aviso C4005 es error; mismo recurso que Task.cpp.
#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)
#include "X87Math.h"
#include "Event.h"


namespace {
// 0x4AC010 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEvent que usan los destructores del EXE (58 llamadores, muchos con `jmp` final desde los destructores de los eventos).
// Es `mov dword ptr [ecx], 0x85AB38; ret`: deja la vtable de CEvent del EXE y conserva EAX, ECX y EDX (algun llamador, p. ej. el destructor con borrado, sigue usando ECX tras la llamada), asi que el codigo no puede
// llevar marco ni comprobaciones: se optimiza por pragma (CMakeLists.txt: /Zi y BasicRuntimeChecks=Default para este archivo). __fastcall con un segundo registro sin usar equivale a thiscall sin argumentos. (Lote 107A, banco Fn107A.)
#pragma runtime_checks("", off)
#pragma optimize("gty", on)
void __fastcall EventDestructorBody(CEvent* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
#pragma optimize("", on)
#pragma runtime_checks("", restore)
} // namespace

void CEvent::InjectHooks() {
    RH_ScopedVirtualClass(CEvent, 0x85AB38, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(EventDestructorBody, "Destructor", 0x4AC010);

    RH_ScopedInstall(operator new, 0x4B5620);
    RH_ScopedInstall(Constructor, 0x4ABFC0);
    RH_ScopedInstall(CalcSoundLevelIncrement, 0x4AC050);
    RH_ScopedInstall(GetSoundLevel, 0x4B2850);
    RH_ScopedInstall(TakesPriorityOverBase, 0x4ABC30);
}

// 0x4ABC30: thiscall (`ret 4`), bool en AL. Como el original: primero la prioridad del otro evento (ranura 2 de su vtabla), despues la propia; comparacion con signo `>=`.
bool CEvent::TakesPriorityOverBase(const CEvent& other) {
    const int32 otherPriority = other.GetEventPriority();
    const int32 thisPriority  = GetEventPriority();
    return thisPriority >= otherPriority;
}

// 0x4B5620
void* CEvent::operator new(unsigned) {
    return GetEventPool()->New();
}

// 0x4B5630
void CEvent::operator delete(void* object) {
    GetEventPool()->Delete(static_cast<CEvent*>(object));
}

// 0x4ABFC0
CEvent::CEvent() {
    m_nTimeActive = 0;
    m_bValid = false;
}

CEvent::~CEvent() {
    // nothing here
}

// 0x4ABFC0
CEvent* CEvent::Constructor() {
    this->CEvent::CEvent();
    return this;
}

// 0x4B2850
float CEvent::GetSoundLevel(const CEntity* entity, CVector& position) {
    if ((entity && entity != GetSourceEntity()) || GetLocalSoundLevel() == 0.0f)
        return 0.0f;

    float distance = 1000.0f;
    const CVector* sourcePos = nullptr;
    if (GetEventType() == EVENT_SOUND_QUIET) {
        sourcePos = &static_cast<CEventSoundQuiet*>(this)->m_position;
    } else if (GetSourceEntity()) {
        sourcePos = &GetSourceEntity()->GetPosition();
    }
    if (sourcePos) {
        // Como el original: distancia con la suma (z² + y²) + x² y mínimo de 1 (un NaN se conserva).
        const float dx = position.x - sourcePos->x;
        const float dy = position.y - sourcePos->y;
        const float dz = position.z - sourcePos->z;
        distance = std::sqrt(dz * dz + dy * dy + dx * dx);
        if (distance < 1.0f) {
            distance = 1.0f;
        }
    }

    // Como el original: log10 con fldlg2/fyl2x a precisión interna del FPU, producto por 10 redondeado
    // una sola vez y distancia multiplicada por 0.01f (0x858C58), no dividida entre 100.
    const float localSoundLevel = GetLocalSoundLevel();
    const float distanceSq      = distance * distance;
    const float log10Times10    = Log10Times10Cpp(distanceSq); // x 10.0f (0x85862C)
    return (localSoundLevel - log10Times10) - distance * 0.01f;
}

// 0x4AC050
float CEvent::CalcSoundLevelIncrement(float level1, float level2) {
    if (level2 == 0.0f)
        return 0.0f;

    if (level1 == 0.0f)
        return level2;

    // Como el original: pow del CRT en doble precisión; la primera potencia se guarda como float, la suma y
    // el cociente se redondean a float en el x87 y el log10 usa fldlg2/fyl2x a precisión interna.
    const float  power1 = (float)std::pow(10.0, (double)(level1 * 0.1f));
    const double power2 = std::pow(10.0, (double)(level2 * 0.1f));
    return SoundLevelIncrementCpp(power2, power1);
}
