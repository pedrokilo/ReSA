/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDecision
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <bit>

#include "Decision.h"

void CDecision::InjectHooks() {    
    RH_ScopedClass(CDecision);
    RH_ScopedCategory("DecisionMakers");

    RH_ScopedInstall(SetDefault, 0x600530);
    RH_ScopedInstall(Set, 0x600570);
    RH_ScopedInstall(Add, 0x600600);
    RH_ScopedInstall(From, 0x6006B0);
    //RH_ScopedInstall(HasResponse, 0x600710, { .reversed = false });
    RH_ScopedInstall(MakeDecision, 0x6040D0);
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
    // Como el original: truncado a 64 bits con signo (fistp con redondeo hacia cero); sólo se guarda el byte bajo.
    return static_cast<uint8>(static_cast<int64>(value));
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

// 0x6007A0 (colaborador de 0x6040D0, sin otro llamador): elige una tarea de las `tasks` (6 entradas) con las probabilidades acumuladas `cumul`.
// Primero busca `taskToSeek` (si no es -1); si no la encuentra, tira rand() del EXE (0x821B1E) * (1/32767, 0x858C7C) y toma la primera entrada con roll <= cumul[i].
// Deja en `taskSlot` la tarea (200 = ninguna). El EXE tambien escribe -1/indice en un tercer puntero que nadie lee.
static void PickTaskFromCumulative(const int32* tasks, const float* cumul, int32 taskToSeek, int16& taskSlot) {
    taskSlot = 200;
    if (taskToSeek != -1) {
        for (int32 i = 0; i < CDecision::MAX_NUM_CHOICES; i++) {
            if (taskToSeek == tasks[i]) {
                taskSlot = static_cast<int16>(tasks[i]);
                break;
            }
        }
    }
    if (taskSlot != 200) {
        return;
    }
    const float roll = static_cast<float>(reinterpret_cast<int(__cdecl*)()>(0x821B1E)()) * std::bit_cast<float>(0x38000100u); // 0x858C7C
    for (int32 i = 0; i < CDecision::MAX_NUM_CHOICES; i++) {
        if (roll <= cumul[i]) { // FCOMP + TEST AH,41h: falso con NaN
            taskSlot = static_cast<int16>(tasks[i]);
            return;
        }
    }
}

// 0x6040D0
// Reune las opciones validas (probabilidad del tipo de fuente > 0, tarea != -1 y distinta de las tres a evitar, permitida segun vaya o no en vehiculo),
// construye sus probabilidades acumuladas normalizadas y elige una (PickTaskFromCumulative). `facialSlot` siempre queda en -1.
void CDecision::MakeDecision(int32 eventSourceType, bool bIsPedInVehicle, int32 taskIdToAvoid1, int32 taskIdToAvoid2, int32 taskIdToAvoid3, int32 taskIdToSeek, int16& taskSlot, int16& facialSlot) {
    int32 tasks[MAX_NUM_CHOICES];
    uint8 probs[MAX_NUM_CHOICES] = {};
    for (auto& t : tasks) {
        t = 200;
    }

    const auto* const raw = reinterpret_cast<const uint8*>(this);
    int32 count = 0;
    for (int32 i = 0; i < MAX_NUM_CHOICES; i++) {
        const uint8 prob = raw[0x18 + i * 4 + eventSourceType]; // m_Probs[i][eventSourceType] (sin comprobar limites, como el EXE)
        if (prob == 0) {
            continue;
        }
        const int32 task = static_cast<int32>(m_Tasks[i]);
        if (task == -1 || task == taskIdToAvoid1 || task == taskIdToAvoid2 || task == taskIdToAvoid3) {
            continue;
        }
        if (!raw[0x30 + i * 2 + (bIsPedInVehicle ? 1 : 0)]) { // m_Bools[i][inVehicle]
            continue;
        }
        tasks[count] = task;
        probs[count] = prob;
        count++;
    }

    taskSlot = 200;
    if (count <= 0) {
        facialSlot = -1;
        return;
    }

    float cumul[MAX_NUM_CHOICES];
    float total = 0.0f;
    for (int32 i = 0; i < MAX_NUM_CHOICES; i++) {
        total += static_cast<float>(probs[i]);
        cumul[i] = total;
    }
    const float inverse = 1.0f / total; // fdivr m32 (1.0f, 0x858624): con el x87 a 24 bits el cociente se redondea a float
    for (int32 i = 0; i < MAX_NUM_CHOICES; i++) {
        cumul[i] = inverse * cumul[i]; // fmul m32 + fstp m32: un unico redondeo
    }

    PickTaskFromCumulative(tasks, cumul, taskIdToSeek, taskSlot);
    facialSlot = -1;
}

// 0x600600: busca la primera de las 6 ranuras vacia (tarea -1, las 4 probabilidades a 0 y los 2 bools a 0) y la rellena: tarea, cada probabilidad = parte baja del
// truncado de la float (_ftol2, 0x821B40) y cada bool = (flags[i] != 0). Si no hay ranura vacia no hace nada. No escribe en `chances` ni en `flags`.
void CDecision::Add(eTaskType taskId, const float* chances, const int32* flags) {
    auto* const raw = reinterpret_cast<uint8*>(this);
    for (int32 i = 0; i < MAX_NUM_CHOICES; i++) {
        if (*reinterpret_cast<const int32*>(raw + i * 4) != -1) {
            continue;
        }
        bool occupied = false;
        for (int32 j = 0; j < 4; j++) {
            if (raw[0x18 + i * 4 + j] > 0) {
                occupied = true;
                break;
            }
        }
        if (occupied) {
            continue;
        }
        for (int32 j = 0; j < 2; j++) {
            if (raw[0x30 + i * 2 + j] != 0) {
                occupied = true;
                break;
            }
        }
        if (occupied) {
            continue;
        }
        *reinterpret_cast<int32*>(raw + i * 4) = static_cast<int32>(taskId);
        for (int32 j = 0; j < 4; j++) {
            raw[0x18 + i * 4 + j] = static_cast<uint8>(static_cast<int64>(chances[j]));
        }
        raw[0x30 + i * 2]     = flags[0] != 0;
        raw[0x30 + i * 2 + 1] = flags[1] != 0;
        return;
    }
}

/*
// 0x600710
bool CDecision::HasResponse() {

}
*/
