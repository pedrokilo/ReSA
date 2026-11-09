/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDecision
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
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

    RH_ScopedInstall(Constructor, 0x6040C0);
    RH_ScopedInstall(SetDefault, 0x600530);
    RH_ScopedInstall(Set, 0x600570);
    RH_ScopedInstall(Add, 0x600600);
    RH_ScopedInstall(From, 0x6006B0);
    //RH_ScopedInstall(HasResponse, 0x600710, { .reversed = false });
    RH_ScopedInstall(MakeDecision, 0x6040D0);
    RH_ScopedNamedInstall(Choices::Pick, "Pick", 0x6007A0); // lote 132A (banco Fn132A)
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

// 0x6007A0 (lote 132A, banco Fn132A; antes un ayudante estatico de MakeDecision): elige una tarea de las `Count` opciones de la estructura (tareas en +0, probabilidades acumuladas en +0x18, numero en +0x30).
// Deja `*taskSlot` = 200 (ninguna) y `*index` = -1; si `taskToSeek` no es -1 la busca entre las tareas (con signo, solo las `Count` primeras) y, si la encuentra, deja su word bajo y su indice. Si no,
// tira rand() del EXE (0x821B1E) * (1/32767, 0x858C7C), guardado como float, y toma la primera opcion con roll <= acumulada (FCOMP + TEST AH,41h: falso con NaN), dejando su word bajo y su indice.
// Los accesos van sobre la memoria cruda de la estructura (como el EXE, que no comprueba `Count`).
void CDecision::Choices::Pick(int32 taskToSeek, int16* taskSlot, int32* index) {
    const auto* const raw = reinterpret_cast<const uint8*>(this);
    *taskSlot = 200;
    *index    = -1;
    if (taskToSeek != -1) {
        const int32 count = *reinterpret_cast<const int32*>(raw + 0x30);
        for (int32 i = 0; i < count; i++) {
            if (taskToSeek == reinterpret_cast<const int32*>(raw)[i]) {
                *taskSlot = *reinterpret_cast<const int16*>(raw + i * 4);
                *index    = i;
                break;
            }
        }
    }
    if (*taskSlot != 200) {
        return;
    }
    const float roll = static_cast<float>(reinterpret_cast<int(__cdecl*)()>(0x821B1E)()) * std::bit_cast<float>(0x38000100u); // 0x858C7C
    const int32 count = *reinterpret_cast<const int32*>(raw + 0x30);
    for (int32 i = 0; i < count; i++) {
        if (roll <= *reinterpret_cast<const float*>(raw + 0x18 + i * 4)) { // FCOMP + TEST AH,41h: falso con NaN
            *taskSlot = *reinterpret_cast<const int16*>(raw + i * 4);
            *index    = i;
            return;
        }
    }
}

// 0x6040D0
// Reune las opciones validas (probabilidad del tipo de fuente > 0, tarea != -1 y distinta de las tres a evitar, permitida segun vaya o no en vehiculo),
// construye sus probabilidades acumuladas normalizadas y elige una (Choices::Pick). `facialSlot` siempre queda en -1.
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

    Choices choices;
    for (int32 i = 0; i < MAX_NUM_CHOICES; i++) {
        choices.Tasks[i]      = tasks[i];
        choices.Cumulative[i] = cumul[i];
    }
    choices.Count = count;
    int32 pickedIndex;
    choices.Pick(taskIdToSeek, &taskSlot, &pickedIndex);
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
