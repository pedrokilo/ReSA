/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include "Ped.h"
#include "eModelID.h"

enum eCopType : uint32 { // TODO: Fix naming
    COP_TYPE_CITYCOP,
    COP_TYPE_LAPDM1,
    COP_TYPE_SWAT1,
    COP_TYPE_SWAT2,
    COP_TYPE_FBI,
    COP_TYPE_ARMY,
    COP_TYPE_CSHER = 7
};

/**
 * CPed policial con partner y una lista fija de hasta cinco criminales perseguidos.
 *
 * Los punteros de partner/objetivos se registran mediante CEntity references para
 * anularse automáticamente si la entidad correspondiente desaparece.
 */
class NOTSA_EXPORT_VTABLE CCopPed : public CPed {
public:
    bool     m_bDontPursuit;
    bool     field_79D;
    uint32   m_nCopTypeOrModelID; /* if it's a valid eCopType then eCopType, otherwise a modelID */
    uint32   field_7A4;
    CCopPed* m_pCopPartner;            //!< Partner policial; referencia segura, no owned.
    CPed*    m_apCriminalsToKill[5];   //!< Objetivos perseguidos; referencias seguras.
    bool     m_isTheDriver; //< Whenever this ped should be a passenger or a driver when entering a vehicle

public:
    //! Devuelve el model ID usado por un eCopType.
    static eModelID GetPedModelForCopType(eCopType ctype);

    /* May also be called with a modelID */
    explicit CCopPed(uint32_t copTypeOrModelID);
    ~CCopPed() override;

    void SetPartner(CCopPed* partner);
    int32 AddCriminalToKill(CPed* criminal);
    void RemoveCriminalToKill(int32 unk, int32 nCriminalLocalIdx);
    void ClearCriminalsToKill();
    void ClearCriminalListFromDeadPeds();
    bool IsCriminalInList(CPed* criminal);
    size_t GetEmptySlotForCriminalToKill();
    void ProcessControl() override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CCopPed* Constructor(uint32_t copTypeOrModelID);
    CCopPed* Destructor();

};

VALIDATE_SIZE(CCopPed, 0x7C4);
