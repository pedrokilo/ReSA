/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDecisionMaker
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "DecisionMaker.h"

namespace {
// 0x5BD4E0 (lote 181A, sin validar; el listado decia CPopulation): operator= de CDecisionMaker del EXE (thiscall, un puntero de origen en la pila, `ret 4`, 45 bytes: `push ebx; push ebp; push esi; push edi; ..; para 41
// decisiones (0x3C bytes, en orden creciente) CDecision::From (0x6006B0, con hook) con ECX = decision de destino y la de origen en la pila; mov eax,ebp (this); ret 4`). Igual que From (0x5BA020) pero devuelve `this` en EAX.
// Sin llamadores directos en el EXE (los compiladores lo llevan en linea).
__declspec(noinline) CDecisionMaker* __fastcall DecisionMakerAssignExe(CDecisionMaker* self, void* /*edx*/, const CDecisionMaker* source) {
    self->From(*source);
    return self;
}

// 0x4634D0 (lote 222A, sin validar; el listado decia CEntity): pone por defecto un decision maker del EXE (thiscall, ECX = this = primera CDecision, sin argumentos de pila, `ret`, 32 bytes: `push esi; push edi;
// mov esi,ecx; mov edi,29h; ..: mov ecx,esi; call 0x600530; add esi,3Ch; dec edi; jne ..; pop edi; pop esi; ret`). Para cada una de las 41 decisiones (0x3C bytes, en orden creciente) llama a CDecision::SetDefault
// (0x600530, con hook) con ECX = la decision. Es el mismo bucle que el constructor 0x4650A0 trae en linea; sin llamadores directos en el EXE. No devuelve nada (EAX es el de la ultima llamada).
__declspec(noinline) void __fastcall DecisionMakerSetDefaultExe(CDecisionMaker* self, void* /*edx*/) {
    static_assert(DM_TOTAL_NUM_EVENTS == 0x29 && sizeof(CDecision) == 0x3C);
    for (size_t i = 0; i < self->m_aDecisions.size(); ++i) {
        self->m_aDecisions[i].SetDefault();
    }
}

// 0x4650A0 (lote 223A, sin validar; el listado decia CTheScripts): constructor de CDecisionMaker del EXE (thiscall, ECX = this = primera CDecision, sin argumentos de pila, `ret`, 54 bytes: `push ebx; push esi; mov ebx,ecx;
// push edi; mov esi,ebx; mov edi,29h; ..: mov ecx,esi; call 0x6040C0; add esi,3Ch; dec edi; jne ..; mov esi,ebx; mov edi,29h; ..: mov ecx,esi; call 0x600530; add esi,3Ch; dec edi; jne ..; ..; mov eax,ebx; pop ebx; ret`).
// Como el original: primer bucle de 41 decisiones (0x3C bytes, en orden creciente) con el constructor de CDecision (0x6040C0, con hook; en C++ ya incluye un SetDefault) y segundo bucle de 41 CDecision::SetDefault (0x600530,
// con hook) sobre las mismas; devuelve `this` en EAX. Llamador del EXE: 0x607D3A (CDecisionMakerTypesFileLoader::LoadDecisionMaker, que construye un decision maker temporal).
__declspec(noinline) CDecisionMaker* __fastcall DecisionMakerConstructorExe(CDecisionMaker* self, void* /*edx*/) {
    static_assert(DM_TOTAL_NUM_EVENTS == 0x29 && sizeof(CDecision) == 0x3C);
    for (size_t i = 0; i < self->m_aDecisions.size(); ++i) {
        self->m_aDecisions[i].Constructor();
    }
    for (size_t i = 0; i < self->m_aDecisions.size(); ++i) {
        self->m_aDecisions[i].SetDefault();
    }
    return self;
}
} // namespace

void CDecisionMaker::InjectHooks() {
    RH_ScopedClass(CDecisionMaker);
    RH_ScopedCategory("DecisionMakers");

    RH_ScopedInstall(MakeDecision, 0x606060);
    RH_ScopedInstall(From, 0x5BA020); // lote 144A (banco Fn144A)
    RH_ScopedNamedGlobalInstall(DecisionMakerAssignExe, "operator=", 0x5BD4E0); // lote 181A (sin validar)
    RH_ScopedNamedGlobalInstall(DecisionMakerSetDefaultExe, "SetDefault", 0x4634D0); // lote 222A (sin validar)
    RH_ScopedNamedGlobalInstall(DecisionMakerConstructorExe, "Constructor", 0x4650A0); // lote 223A (sin validar)
}

// 0x4650A0
CDecisionMaker::CDecisionMaker() {
    for (auto& decision : m_aDecisions) {
        decision.SetDefault();
    }
}

// 0x5BA020 (lote 144A, banco Fn144A; thiscall, un argumento de pila, `ret 4`, 39 bytes; el listado lo atribuia a CVehicleAnimGroup): copia de un decision maker. Para cada una de las 41 decisiones (0x3C
// bytes, en orden creciente) llama a CDecision::From (0x6006B0, con hook) con ECX = decision destino y la decision de origen en la pila (origen + i * 0x3C). No devuelve nada (EAX es el de la ultima llamada;
// el EXE no la referencia desde ningun llamador directo).
void CDecisionMaker::From(const CDecisionMaker& rhs) {
    static_assert(DM_TOTAL_NUM_EVENTS == 0x29);
    for (size_t i = 0; i < m_aDecisions.size(); ++i) {
        m_aDecisions[i].From(rhs.m_aDecisions[i]);
    }
}

// 0x606060: ECX = this (primera CDecision del decision maker), nueve argumentos en pila (ret 24h). Delega en CDecision::MakeDecision (0x6040D0) de la decision
// this + eventIndex * 0x3C (imul de 32 bits que da la vuelta, sin comprobar limites) pasandole los otros ocho argumentos tal cual. CDecision::MakeDecision es la ultima
// llamada; el unico llamador del EXE (CDecisionMakerTypes::MakeDecision para peds) no lee EAX/ECX/EDX despues.
void CDecisionMaker::MakeDecision(int32 eventIndex, int32 eventSourceType, bool bIsPedInVehicle, int32 taskTypeToAvoid1, int32 taskTypeToAvoid2, int32 taskTypeToAvoid3, int32 taskTypeToSeek, int16& taskType, int16& facialTaskType) {
    auto* const decision = reinterpret_cast<CDecision*>(reinterpret_cast<uint8*>(this) + static_cast<uint32>(eventIndex) * static_cast<uint32>(sizeof(CDecision)));
    decision->MakeDecision(eventSourceType, bIsPedInVehicle, taskTypeToAvoid1, taskTypeToAvoid2, taskTypeToAvoid3, taskTypeToSeek, taskType, facialTaskType);
}
