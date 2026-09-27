/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "ClumpModelInfo.h"
#include "eWeaponType.h"

/**
 * Model info para armas renderizadas como RpClump.
 *
 * Añade la asociación con eWeaponType y aplica configuración especial al componente
 * gunflash al cargar el clump.
 */
class NOTSA_EXPORT_VTABLE CWeaponModelInfo : public CClumpModelInfo {
private:
    eWeaponType m_weaponInfo; //!< Tipo lógico de arma representado por este modelo.

public:
    CWeaponModelInfo() : CClumpModelInfo() {}

    // VTable

    void Init() override;

    ModelInfoType GetModelType() override;
    void SetClump(RpClump* clump) override;

    // inlined
    void SetWeaponInfo(eWeaponType weapon) { m_weaponInfo = weapon; }
    eWeaponType GetWeaponInfo() { return m_weaponInfo; }

private: // NOTSA:
    friend void InjectHooksMain();
    static void InjectHooks();
};

VALIDATE_SIZE(CWeaponModelInfo, 0x28);
