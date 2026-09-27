#pragma once

#include <stdint.h>

/**
 * Metadatos de visibilidad horaria para modelos que alternan entre variantes de día y noche.
 *
 * Además del intervalo activo conserva el índice del modelo complementario, permitiendo
 * que ambos compartan ciertos recursos como el CColModel cuando corresponde.
 */
class CTimeInfo {
public:
    uint8 m_nTimeOn;              //!< Hora inicial del intervalo visible.
    uint8 m_nTimeOff;             //!< Hora final del intervalo visible.
    int16 m_nOtherTimeModel = -1; //!< Índice de la variante complementaria, o -1.

public:
    static void InjectHooks();

public:
    // Busca la variante _dy/_nt correspondiente y memoriza su índice si existe.
    CTimeInfo* FindOtherTimeModel(const char* modelName);

    int32 GetOtherTimeModel() const { return m_nOtherTimeModel; }

    void SetOtherTimeModel(int16 otherTimeModel) { m_nOtherTimeModel = otherTimeModel; }

    uint8 GetTimeOn() const { return m_nTimeOn; }

    uint8 GetTimeOff() const { return m_nTimeOff; }

    void SetTimes(uint8 timeOn, uint8 timeOff) { m_nTimeOn = timeOn; m_nTimeOff = timeOff; }

    // Consulta CClock para decidir si el modelo debe estar activo en la hora actual.
    bool IsVisibleNow() const noexcept { return CClock::GetIsTimeInRange(GetTimeOn(), GetTimeOff()); }
};

VALIDATE_SIZE(CTimeInfo, 0x4);
