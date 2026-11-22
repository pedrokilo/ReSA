#pragma once

#include "Base.h"

class CAnimBlendAssociation;

// Una cabina de telefono (0x34 bytes). El estado esta en +0x2C (CPhoneInfo::Update / StartCall lo mueven entre 6, 7 y 8); el resto del contenido no esta reconstruido.
struct CPhone {
    uint8 m_data[0x34];
};
VALIDATE_SIZE(CPhone, 0x34);

// Gestor de cabinas de telefono (global ThePhoneInfo en 0x977BE8). Las posiciones de +0 y +4 son el maximo y el numero de cabinas.
class CPhoneInfo {
public:
    uint32 m_nMax;
    uint32 m_nCount;
    CPhone m_aPhones[50];

public:
    static void InjectHooks();

    // 0x4541E0 (lote 208A, sin validar): constructor del EXE (el listado lo atribuia a CPathFind)
    CPhoneInfo* Constructor();

    // 0x453F40 (lote 208A, sin validar): callback de fin de la animacion de telefono (el listado lo atribuia a CPathFind)
    static void PhoneAnimFinishCallback(CAnimBlendAssociation* assoc, void* ped);

    // 0x453E40 (lote P6B, sin validar): callback de fin de la animacion de descolgar (asociacion, cabina)
    static void PhoneAnimAnswerCallback(CAnimBlendAssociation* assoc, void* phone);
};
VALIDATE_SIZE(CPhoneInfo, 0xA30);

static inline auto& ThePhoneInfo = StaticRef<CPhoneInfo>(0x977BE8);
