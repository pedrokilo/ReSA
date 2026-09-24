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
 * Evidencia aprobada vinculada: 0/2 entradas.
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
        if (DistanceBetweenPoints(TheCamera.GetPosition(), vecPos) < 200.0f) {
            Array[Number] = {
                .m_vecPos = vecPos,
                .m_iActive = iActive,
                .m_fPower = fPower
            };
            Number++;
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
        if (!wm.m_iActive)
            continue;

        const auto absZDist = fabs(vecPos.z + 15.f - wm.m_vecPos.z);
        if (absZDist >= 40.0f)
            continue;

        const auto diff = vecPos - wm.m_vecPos;
        const auto dist = diff.Magnitude();
        if (dist >= 50.f)
            continue;

        const float distProg = dist >= 20.f ? 1.f - invLerp(20.f, 50.f, dist) : 1.f;
        const float distZProg = 1.f - invLerp(0.f, 40.f, absZDist);
        const float power = distZProg * (distProg * wm.m_fPower) / 5.f;

        // NOTE: Possible bug? They don't seem to re-use the last value, but rather overwrite it..
        //       shouldn't they add up all the values or something?
        posX = diff.x / dist * power;
        posY = diff.y / dist * power;

        appliedAny = true;
    }

    if (!appliedAny)
        return false;

    const float rnd = 1.0f + (float)(CGeneral::GetRandomNumber() % 32 - 16) / 2000.f;
    *outX += posX * rnd;
    *outY += posY * rnd;

    return true;
}
