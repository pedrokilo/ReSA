/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CWindModifiers
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

#include "WindModifiers.h"

void CWindModifiers::InjectHooks() {
    RH_ScopedClass(CWindModifiers);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(RegisterOne, 0x72C8B0);
    RH_ScopedInstall(FindWindModifier, 0x72C950);
}

// 0x72C8B0
void CWindModifiers::RegisterOne(CVector vecPos, int32 iActive, float fPower) {
    if (Number < MAX_NUM_MODIFIERS) {
        // Como el original: distancia a la cámara sumando z² + y² + x² en ese orden.
        const auto& camPos = TheCamera.GetPosition();
        const float dx = vecPos.x - camPos.x, dy = vecPos.y - camPos.y, dz = vecPos.z - camPos.z;
        if (std::sqrt(dz * dz + dy * dy + dx * dx) < 200.0f) {
            // Como el original: el índice se lee una vez y al final se escribe índice + 1 (con un índice
            // fuera de rango la escritura del elemento puede pisar el propio contador)
            const int32 idx = Number;
            Array[idx] = {
                .m_vecPos = vecPos,
                .m_iActive = iActive,
                .m_fPower = fPower
            };
            Number = idx + 1;
        }
    }
}

// 0x72C950
bool CWindModifiers::FindWindModifier(CVector vecPos, float* outX, float* outY) {
    if (Number <= 0)
        return false;

    float posX{}, posY{};
    bool appliedAny{};
    for (auto i = 0; i < Number; i++) {
        const auto& wm = Array[i];
        if (wm.m_iActive != 1) // Como el original: solo cuenta el valor 1 (dec eax; jne)
            continue;

        const auto absZDist = fabs(vecPos.z + 15.f - wm.m_vecPos.z);
        if (!(absZDist < 40.0f))
            continue;

        // Como el original: los cocientes del EXE son productos por constantes float (0.025, 1/30, 0.2,
        // recíproco de la distancia) y la magnitud suma z² + y² + x².
        const float distZProg = 1.f - absZDist * 0.025f;
        const auto diff = vecPos - wm.m_vecPos;
        const float dist = std::sqrt(diff.z * diff.z + diff.y * diff.y + diff.x * diff.x);
        if (!(dist < 50.f))
            continue;

        const float distProg = dist < 20.f ? 1.f : 1.f - (dist - 20.f) * std::bit_cast<float>(0x3D088889u);
        const float power = distZProg * (distProg * wm.m_fPower) * 0.2f;
        const float invDist = 1.0f / dist;

        // NOTE: Possible bug? They don't seem to re-use the last value, but rather overwrite it..
        //       shouldn't they add up all the values or something?
        posX = diff.x * invDist * power;
        posY = invDist * diff.y * power;

        appliedAny = true;
    }

    if (!appliedAny)
        return false;

    const float rnd = (float)(CGeneral::GetRandomNumber() % 32 - 16) * 0.0005f + 1.0f;
    *outX += posX * rnd;
    *outY += rnd * posY;

    return true;
}
