#pragma once

#include "AtomicModelInfo.h"

/**
 * CAtomicModelInfo que mantiene una segunda plantilla visual para el estado dañado.
 *
 * ms_bCreateDamagedVersion es un selector global temporal utilizado por CreateInstance()
 * para decidir cuál de las dos plantillas debe clonarse.
 */
class NOTSA_EXPORT_VTABLE CDamageAtomicModelInfo : public CAtomicModelInfo {
public:
    RpAtomic* m_pDamagedAtomic; //!< Plantilla dañada propiedad del model info.

    static inline auto& ms_bCreateDamagedVersion = StaticRef<bool>(0xA9B0B0);

public:
    CDamageAtomicModelInfo() : CAtomicModelInfo() {}

    CDamageAtomicModelInfo* AsDamageAtomicModelInfoPtr() override;
    void Init() override;
    void DeleteRwObject() override;
    struct RwObject* CreateInstance() override;
    struct RwObject* CreateInstance(RwMatrix* matrix) override;

    void SetDamagedAtomic(RpAtomic* atomic);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};

VALIDATE_SIZE(CDamageAtomicModelInfo, 0x24);
