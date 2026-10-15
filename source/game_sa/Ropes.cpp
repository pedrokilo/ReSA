/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRopes
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"

#include "Rope.h"
#include "Ropes.h"
#include "X87Math.h"

void CRopes::InjectHooks() {
    RH_ScopedClass(CRopes);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x555DC0);
    RH_ScopedInstall(Shutdown, 0x556B10);
    RH_ScopedInstall(Update, 0x558D70);
    RH_ScopedInstall(Render, 0x556AE0);
    RH_ScopedInstall(RegisterRope, 0x556B40);
    RH_ScopedInstall(FindPickupHeight, 0x556760);
    RH_ScopedInstall(FindRope, 0x556000);
    RH_ScopedInstall(FindCoorsAlongRope, 0x555E40);
    RH_ScopedInstall(CreateRopeForSwatPed, 0x558D10);
    RH_ScopedInstall(IsCarriedByRope, 0x555F80);
    RH_ScopedInstall(SetSpeedOfTopNode, 0x555DF0);
}

// 0x555DC0
void CRopes::Init() {
    for (auto& rope : aRopes) {
        rope.m_nType = eRopeType::NONE;
    }
    PlayerControlsCrane = eControlledCrane::NONE;
}

// 0x556B10
void CRopes::Shutdown() {
    for (auto& rope : aRopes) {
        if (rope.m_nType == eRopeType::NONE)
            continue;

        rope.Remove();
    }
}

// 0x558D70
void CRopes::Update() {
    ZoneScoped;

    if (CReplay::Mode == MODE_PLAYBACK)
        return;

    for (auto& rope : aRopes) {
        if (rope.m_nType != eRopeType::NONE)
            rope.Update();
    }
}

// 0x556AE0
void CRopes::Render() {
    ZoneScoped;

    for (auto& rope : aRopes) {
        if (rope.m_nType != eRopeType::NONE)
            rope.Render();
    }
}

// Must be used in loop to make attached to holder
// 0x556B40
bool CRopes::RegisterRope(uint32 ropeID, uint32 ropeType, CVector startPos, bool bExpires, uint8 segmentCount, uint8 flags, CPhysical* holder, uint32 timeExpire) {
    // Como el original: primero busca una cuerda activa con el mismo id y solo la actualiza
    for (auto i = 0; i < MAX_NUM_ROPES; i++) {
        CRope& existing = aRopes.data()[i];
        if (existing.m_nType == eRopeType::NONE || existing.m_nId != ropeID) {
            continue;
        }

        existing.m_aSegments.data()[0] = startPos;
        existing.m_aSpeed.data()[0] = CVector{0.0f, 0.0f, 0.0f};
        existing.m_nFlags2 |= 1;
        existing.m_nSegments = segmentCount;
        // Como el original: bucle do-while de 0 a m_nSegments inclusive (sin limitar a 32)
        auto j = 0u;
        do {
            existing.m_aSegments.data()[j] = startPos;
            existing.m_aSpeed.data()[j] = CVector{0.0f, 0.0f, 0.0f};
            j++;
        } while ((int32)j <= (int32)existing.m_nSegments);
        existing.CreateHookObjectForRope();
        return true;
    }

    // Si no existe, usa el primer hueco libre
    auto freeIdx = 0;
    for (; freeIdx < MAX_NUM_ROPES; freeIdx++) {
        if (aRopes.data()[freeIdx].m_nType == eRopeType::NONE) {
            break;
        }
    }
    if (freeIdx == MAX_NUM_ROPES) {
        return false;
    }

    CRope& rope = aRopes.data()[freeIdx];
    rope.m_nId = ropeID;
    rope.m_aSegments.data()[0] = startPos;
    rope.m_aSpeed.data()[0] = CVector{0.0f, 0.0f, 0.0f};
    // Como el original: bit 2 = flags & 1 y bit 0 siempre activo; el resto se conserva
    rope.m_nFlags2 = (uint8)(((flags & 1) << 2) | (rope.m_nFlags2 & 0xF9) | 1);
    rope.m_nSegments = segmentCount;
    rope.m_fGroundZ = 0.0f;
    rope.m_pAttachedEntity = nullptr;
    rope.m_pRopeAttachObject = nullptr;
    rope.m_fSegmentLength = (holder && holder->GetType() == ENTITY_TYPE_VEHICLE) ? 0.9f : 0.5f; // 0x3F666666 / 0x3F000000
    rope.m_nFlags1 = 0;
    rope.m_pRopeHolder = holder;
    rope.m_nType = (eRopeType)(uint8)ropeType;
    if (holder) {
        holder->RegisterReference(reinterpret_cast<CEntity**>(&rope.m_pRopeHolder));
    }
    rope.m_nTime = bExpires ? CTimer::m_snTimeInMilliseconds + timeExpire : 0;

    // Como el original: (offset 0x308, offset 0x30C) segun el tipo; 3 -> (10, ..), 4 -> (50, ..), 5..7 -> (68, ..), resto -> (20, ..)
    switch ((uint8)rope.m_nType) {
    case 3:
        rope.m_fMass = 10.0f;
        rope.m_fTotalLength = std::bit_cast<float>(0x3EA5294Au);
        break;
    case 4:
        rope.m_fMass = 50.0f;
        rope.m_fTotalLength = std::bit_cast<float>(0x3FCE739Du);
        break;
    case 5:
    case 6:
    case 7:
        rope.m_fMass = 68.0f;
        rope.m_fTotalLength = std::bit_cast<float>(0x400C6319u);
        break;
    default:
        rope.m_fMass = 20.0f;
        rope.m_fTotalLength = std::bit_cast<float>(0x3F25294Au);
        break;
    }

    const float step = rope.m_fTotalLength;
    if ((uint8)rope.m_nType >= 4 && (uint8)rope.m_nType <= 7) {
        // Cuerda colgando: cada segmento un paso por debajo del anterior
        for (auto j = 1; j < (int32)NUM_ROPE_SEGMENTS; j++) {
            const CVector& prev = rope.m_aSegments.data()[j - 1];
            rope.m_aSegments.data()[j] = CVector{prev.x, prev.y, prev.z - step};
        }
    } else {
        // Cuerda en zigzag horizontal: x suma el paso en los impares y lo resta en los pares; la velocidad se pone a cero
        for (auto j = 1; j < (int32)NUM_ROPE_SEGMENTS; j++) {
            const CVector& prev = rope.m_aSegments.data()[j - 1];
            const float newX = (j & 1) ? step + prev.x : prev.x - step;
            rope.m_aSegments.data()[j] = CVector{newX, prev.y, prev.z};
            rope.m_aSpeed.data()[j] = CVector{0.0f, 0.0f, 0.0f};
        }
    }

    rope.CreateHookObjectForRope();
    return true;
}

// 0x556760
float CRopes::FindPickupHeight(CEntity* entity) {
    return CModelInfo::GetModelInfo(entity->m_nModelIndex)->GetColModel()->GetBoundingBox().m_vecMax.z;
}

// Returns id to array
// 0x556000
int32 CRopes::FindRope(uint32 id) {
    for (auto ropeId = 0; ropeId < MAX_NUM_ROPES; ropeId++) {
        if (aRopes[ropeId].m_nType != eRopeType::NONE && aRopes[ropeId].m_nId == id)
            return ropeId;
    }
    return -1;
}

// a4 always nullptr
// 0x555E40
bool CRopes::FindCoorsAlongRope(uint32 ropeId, float fDistAlongRope, CVector* outPosn, CVector* outSpeed) {
    auto ropeIdx = 0;
    for (; ropeIdx < MAX_NUM_ROPES; ropeIdx++) {
        if (aRopes.data()[ropeIdx].m_nType != eRopeType::NONE && aRopes.data()[ropeIdx].m_nId == ropeId) {
            break;
        }
    }
    if (ropeIdx == MAX_NUM_ROPES) {
        return false;
    }
    const CRope& rope = aRopes.data()[ropeIdx];

    // Como el original: se limita a [0, 0.999] y se escala a 31 tramos; se interpola entre dos segmentos
    float dist = fDistAlongRope;
    if (0.0f > dist) {
        dist = 0.0f;
    } else if (0.999f < dist) {
        dist = 0.999f;
    }
    // Como el original: dist * 31 y el interpolado quedan en la pila x87 sin redondear a float (ayudante compartido en X87Math)
    const int32 segIdx = RopesInterpolateCpp(dist, &rope.m_aSegments.data()[0].x, &outPosn->x);
    if (outSpeed) {
        *outSpeed = rope.m_aSpeed.data()[segIdx + 1];
    }
    return true;
}

// 0x558D10
int32 CRopes::CreateRopeForSwatPed(const CVector& startPos) {
    const int32 newRopeId = (int32)(m_nRopeIdCreationCounter + 100);
    // Como el original: el resultado de RegisterRope (0/1) se compara con signo (nunca es negativo),
    // asi que el contador siempre avanza (modulo 256) y se devuelve el id.
    if ((int32)RegisterRope((uint32)newRopeId, static_cast<uint32>(eRopeType::SWAT), startPos, true, 0, 0, nullptr, 4000) < 0) {
        return -1;
    }

    m_nRopeIdCreationCounter = (m_nRopeIdCreationCounter + 1) & 0xFF;
    return newRopeId;
}

// 0x555F80
bool CRopes::IsCarriedByRope(CPhysical* entity) {
    if (!entity)
        return false;

    for (auto& rope : aRopes) {
        if (rope.m_nType != eRopeType::NONE && rope.m_pRopeAttachObject == entity)
            return true;
    }
    return false;
}

// 0x555DF0
void CRopes::SetSpeedOfTopNode(uint32 ropeId, CVector dirSpeed) {
    for (auto& rope : aRopes) {
        if (rope.m_nType != eRopeType::NONE && rope.m_nId == ropeId) {
            rope.m_aSpeed[0] = dirSpeed;
            return;
        }
    }
}
