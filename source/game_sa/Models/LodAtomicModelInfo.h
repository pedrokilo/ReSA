#pragma once

#include "AtomicModelInfo.h"

/**
 * Variante de CAtomicModelInfo usada como representación LOD de otros modelos.
 *
 * Los contadores permiten conocer cuántos hijos dependen de este LOD y cuántos
 * fueron renderizados en el frame actual.
 */
class NOTSA_EXPORT_VTABLE CLodAtomicModelInfo : public CAtomicModelInfo {
public:
    int16 m_numChildren;         //!< Número de modelos hijos asociados a este LOD.
    int16 m_numChildrenRendered; //!< Hijos ya renderizados en el ciclo actual.

public:
    CLodAtomicModelInfo();

    CLodAtomicModelInfo* AsLodAtomicModelInfoPtr() override;
    ModelInfoType GetModelType() override;
    void Init() override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

};

VALIDATE_SIZE(CLodAtomicModelInfo, 0x24);
