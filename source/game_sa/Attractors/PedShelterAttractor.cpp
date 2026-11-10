/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedShelterAttractor
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

#include "PedShelterAttractor.h"
#include "X87Math.h"

#include "Tasks/TaskTypes/TaskComplexGoToAttractor.h"

// 0x5EF420
CVector& CPedShelterAttractor::GetDisplacement(int32 pedId) {
    // Como el original: la tabla se rellena la primera vez con 5 desplazamientos aleatorios separados al menos 1 unidad
    if (ms_displacements.size() == 0) {
        constexpr auto RECIP = std::bit_cast<float>(0x38000100u); // 0x858C7C
        constexpr auto TWOPI = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
        for (int32 n = 0; n < 5; n++) {
            CVector candidate;
            bool    tooClose;
            do {
                tooClose = false;
                // Como el original: ángulo = rand · (1/32767) · 2π y radio = 2 · (rand · (1/32767)) (coseno y seno x87)
                const float ang = static_cast<float>(CGeneral::GetRandomNumber()) * RECIP * TWOPI;
                const float rad = (static_cast<float>(CGeneral::GetRandomNumber()) * RECIP) * 2.0f;
                // x87: cos/sin de ang por rad, redondeados a float una sola vez (X87Math.cpp)
                const float outX = TrigSimpleCosMulCpp(ang, rad);
                const float outY = TrigSimpleSinMulCpp(ang, rad);
                candidate.x = outX;
                candidate.y = outY;
                candidate.z = 0.0f;
                // Como el original: la distancia a cada elemento ya guardado resta solo x e y (z se toma tal cual)
                const auto count = static_cast<int32>(ms_displacements.size());
                for (int32 k = 0; k < count; k++) {
                    const CVector& e = ms_displacements._First[k];
                    const float dx = e.x - candidate.x;
                    const float dy = e.y - candidate.y;
                    const float dz = e.z;
                    const float distSq = (dz * dz + dy * dy) + dx * dx;
                    if (distSq < 1.0f) { // con NaN no entra
                        tooClose = true;
                        break;
                    }
                }
            } while (tooClose);
            ms_displacements.emplace_back(candidate);
        }
    }
    // Como el original: sin comprobar el índice
    return ms_displacements._First[pedId];
}

// 0x5EFC40
void CPedShelterAttractor::ComputeAttractPos(int32 pedId, CVector& outPos) {
    if (m_Fx) {
        const CVector& displacement = GetDisplacement(pedId);
        outPos.x = displacement.x + m_Pos.x;
        outPos.y = displacement.y + m_Pos.y;
        outPos.z = displacement.z + m_Pos.z;
    }
}

// 0x5E9690
void CPedShelterAttractor::ComputeAttractHeading(int32 bQueue, float& heading) {
    // Como el original: rand · (1/32767) · 2π con las constantes del EXE (0x858C7C y 0x858CBC)
    heading = static_cast<float>(CGeneral::GetRandomNumber()) * std::bit_cast<float>(0x38000100u) * std::bit_cast<float>(0x40C90FDBu);
}

// 0x5EF570
bool CPedShelterAttractor::BroadcastDeparture(CPed* ped) {
    // Como el original: el peatón que se va se busca en la cola (m_ArrivedPeds)
    const auto numArrived = static_cast<int32>(m_ArrivedPeds.size());
    int32 idx = 0;
    for (; idx < numArrived; ++idx) {
        if (m_ArrivedPeds.begin()[idx] == ped) {
            break;
        }
    }
    if (idx >= numArrived) {
        return false;
    }
    // Como el original: sin avanzar a los de detrás; se borra su pareja tarea (si existe) y su sitio en la cola
    if (const auto erased = rng::find(m_PedTaskPairs, ped, &CPedTaskPair::Ped); erased != m_PedTaskPairs.end()) {
        m_PedTaskPairs.erase(erased);
    }
    m_ArrivedPeds.erase(m_ArrivedPeds.begin() + idx);
    // Como el original: cada atraído recibe un desplazamiento al hueco que le toca tras la cola
    const auto numAttract = static_cast<int32>(m_AttractPeds.size());
    for (int32 i = 0; i < numAttract; ++i) {
        CPed* const attractedPed = m_AttractPeds.begin()[i];
        const auto slot = static_cast<int32>(m_ArrivedPeds.size());
        CVector pos;
        // Como el original: se llama por la vtable (clase final: sin la conversión a la base el compilador desvirtualiza)
        static_cast<CPedAttractor*>(this)->ComputeAttractPos(slot, pos);
        float heading;
        static_cast<CPedAttractor*>(this)->ComputeAttractHeading(slot, heading);
        const float time = m_AchieveQueueTime;
        SetTaskForPed(attractedPed, new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, PEDMOVE_WALK });
    }
    return true;
}

namespace {
// 0x5E9800 / 0x5E9850 (lote 147A, banco Fn147A; thiscall, sin argumentos de pila, `ret`, 33 y 32 bytes): `size()` de dos instancias de vector (disposicion STL de MSVC 6, como SArray: +4 inicio, +8 fin) que
// el EXE no expande en linea en el codigo de insercion: elementos de 20 bytes (0x5E9800: `imul 0x66666667; sar 3`) y de 12 bytes (0x5E9850, la de CVector de ms_displacements: `imul 0x2AAAAAAB; sar 1`).
// Sin inicio devuelve 0 (EAX = el inicio nulo); si no, la diferencia de bytes (palabra de 32 bits con signo) entre el final y el inicio dividida por el tamano del elemento, truncada hacia cero.
__declspec(noinline) int32 __fastcall SArraySizeStride20(const void* self, void* /*edx*/) {
    const uint32* const words = static_cast<const uint32*>(self);
    if (words[1] == 0) {
        return 0;
    }
    return static_cast<int32>(words[2] - words[1]) / 20;
}
__declspec(noinline) int32 __fastcall SArraySizeStride12(const void* self, void* /*edx*/) {
    const uint32* const words = static_cast<const uint32*>(self);
    if (words[1] == 0) {
        return 0;
    }
    return static_cast<int32>(words[2] - words[1]) / 12;
}
} // namespace

void CPedShelterAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CPedShelterAttractor, 0x86C5B4, 6);
    RH_ScopedCategory("Attractors");

    RH_ScopedNamedGlobalInstall(SArraySizeStride20, "SArraySizeStride20", 0x5E9800); // lote 147A (banco Fn147A)
    RH_ScopedNamedGlobalInstall(SArraySizeStride12, "SArraySizeStride12", 0x5E9850); // lote 147A (banco Fn147A)

    RH_ScopedInstall(GetDisplacement, 0x5EF420);
    RH_ScopedVMTInstall(ComputeAttractPos, 0x5EFC40);
    RH_ScopedVMTInstall(ComputeAttractHeading, 0x5E9690);
    RH_ScopedVMTInstall(BroadcastDeparture, 0x5EF570);
}
