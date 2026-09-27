/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "BaseModelInfo.h"

struct tVehicleComponentFlag {
    const char* m_ucName;
    uint32      m_nFlag;
};

/**
 * Model info cuya representación RenderWare es un único RpAtomic.
 *
 * Administra la plantilla compartida que se clona al crear instancias del mundo,
 * sus referencias de TXD/AnimBlock, efectos 2DFX y configuración de pipeline.
 */
class NOTSA_EXPORT_VTABLE CAtomicModelInfo : public CBaseModelInfo {
public:
    CAtomicModelInfo() : CBaseModelInfo() {}

public:
    static void InjectHooks();

    // vtable overrides
    CAtomicModelInfo* AsAtomicModelInfoPtr() override;
    ModelInfoType GetModelType() override;
    void Init() override;
    void DeleteRwObject() override;
    uint32 GetRwModelType() const override { return rpATOMIC; }
    RwObject* CreateInstance() override;
    RwObject* CreateInstance(RwMatrix* matrix) override;

    // vtable added methods
    // Adopta el atomic como plantilla del model info y registra sus dependencias.
    virtual void SetAtomic(RpAtomic* atomic);

    // class methods
    // Devuelve la plantilla sólo cuando supera la prueba de distancia LOD.
    struct RpAtomic* GetAtomicFromDistance(float distance);
    void SetupVehicleUpgradeFlags(const char* name);
};

void SetAtomicModelInfoFlags(CAtomicModelInfo* modelInfo, uint32 dwFlags);

VALIDATE_SIZE(CAtomicModelInfo, 0x20);
