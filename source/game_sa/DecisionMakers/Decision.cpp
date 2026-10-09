/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDecision
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

#include "Decision.h"

void CDecision::InjectHooks() {    
    RH_ScopedClass(CDecision);
    RH_ScopedCategory("DecisionMakers");

    RH_ScopedInstall(SetDefault, 0x600530);
    RH_ScopedInstall(Set, 0x600570);
    //RH_ScopedInstall(Add, 0x600600, { .reversed = false });
    RH_ScopedInstall(From, 0x6006B0);
    //RH_ScopedInstall(HasResponse, 0x600710, { .reversed = false });
    //RH_ScopedInstall(MakeDecision, 0x6040D0, { .reversed = false });
}

// 0x6040C0
CDecision::CDecision() {
    SetDefault();
}

// 0x600530
void CDecision::SetDefault() {
    for (size_t i = 0; i < MAX_NUM_CHOICES; ++i) {
        m_Tasks[i] = static_cast<eTaskType>(-1);
        m_Probs[i].fill(0);
        m_Bools[i].fill(false);
    }
}

// 0x6006B0
void CDecision::From(const CDecision& rhs) {
    *this = rhs;
}

static uint8 ProbabilityByte(float value) {
    int64 result;
    uint16 savedControl, truncateControl;
    __asm {
        fnstcw savedControl
        mov ax, savedControl
        or ax, 0C00h
        mov truncateControl, ax
        fldcw truncateControl
        fld value
        fistp qword ptr result
        fldcw savedControl
    }
    return static_cast<uint8>(result);
}

// 0x600570
void CDecision::Set(
    notsa::mdarray<int32, MAX_NUM_CHOICES>&    tasks,
    notsa::mdarray<float, MAX_NUM_CHOICES, 4>& probs,
    notsa::mdarray<int32, MAX_NUM_CHOICES, 2>& bools,
    notsa::mdarray<float, MAX_NUM_CHOICES, 6>& facialProbs
) {
    // _ftol2 truncates to a signed 64-bit integer; only its low byte is stored.
    // Preserve its indefinite integer result for NaN and overflowing inputs too.
    for (size_t i = 0; i < MAX_NUM_CHOICES; ++i) {
        m_Tasks[i] = static_cast<eTaskType>(tasks[i]);
        for (size_t j = 0; j < 4; ++j) {
            m_Probs[i][j] = ProbabilityByte(probs[i][j]);
        }
        for (size_t j = 0; j < 2; ++j) {
            m_Bools[i][j] = bools[i][j] != 0;
        }
    }
}

/*
// 0x6040D0
void CDecision::MakeDecision(int32, bool, int32, int32, int32, int32, int16&, int16&) {

}

// 0x600710
bool CDecision::HasResponse() {

}

// 0x600600
void CDecision::Add(int32, float*, int32*) {

}
*/
