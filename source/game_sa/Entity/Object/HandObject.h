/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Object.h"

/**
 * CObject decorativo que sigue directamente un bone de antebrazo de un CPed.
 *
 * No usa colisión ni streaming normal y copia cada frame la transformación del bone.
 * m_pPed y m_pTexture son referencias a recursos externos.
 */
class NOTSA_EXPORT_VTABLE CHandObject : public CObject {
public:
    class CPed* m_pPed;       //!< Ped propietario visual; no owned.
    uint32      m_nBoneIndex; //!< Índice HAnim del antebrazo seguido.
    RwTexture*  m_pTexture;  //!< Textura tomada del TXD del ped; no owned.
    bool        m_bUpdatedMatricesArray;
    char        _pad[3];

public:
    static void InjectHooks();

    CHandObject(int32 handModelIndex, CPed* ped, bool bLeftHand);
    ~CHandObject() = default;

    void ProcessControl() override;
    void PreRender() override;
    void Render() override;

private:
};

VALIDATE_SIZE(CHandObject, 0x18C);
