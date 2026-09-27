/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "AtomicModelInfo.h"
#include "TimeInfo.h"

/**
 * Variante de CAtomicModelInfo cuya disponibilidad depende de un intervalo horario.
 *
 * m_timeInfo se embebe directamente en el model info y permite enlazar la variante
 * complementaria de día/noche sin crear un objeto auxiliar dinámico.
 */
class NOTSA_EXPORT_VTABLE CTimeModelInfo : public CAtomicModelInfo {
public:
    CTimeInfo m_timeInfo; //!< Intervalo horario y modelo complementario asociado.

public:
    static void InjectHooks();

    ModelInfoType GetModelType() override;
    CTimeInfo* GetTimeInfo() override;

private:
};

VALIDATE_SIZE(CTimeModelInfo, 0x24);
