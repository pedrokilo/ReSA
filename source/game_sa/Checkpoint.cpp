/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCheckpoint
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Checkpoint.h"
#include "3dMarkers.h"

void CCheckpoint::InjectHooks() {
    RH_ScopedClass(CCheckpoint);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Render, 0x725C00);
}

// 0x725C00
void CCheckpoint::Render() {
    switch (m_Type) {
    case eCheckpointType::TUBE:
    case eCheckpointType::ENDTUBE:
    case eCheckpointType::EMPTYTUBE: {
        //> 0x725C1C - Make sure it's above the water
        float waterZ;
        // Como el original: solo se descarta si el agua queda por debajo (fcomp + test ah,1; con NaN también se sube)
        const auto wasUnderwater = CWaterLevel::GetWaterLevelNoWaves(m_Pos, &waterZ) && !(waterZ < m_Pos.z);
        if (wasUnderwater) {
            m_Pos.z = waterZ;
        }

        //> 0x725CB4 - Render flag for finish checkpoint
        if (m_Type == eCheckpointType::ENDTUBE) {
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(RwTextureGetRaster(gpFinishFlagTex)));
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE)); // Como el original: 0x725CBD escribe 0
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
            RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS,    RWRSTATE(rwTEXTUREADDRESSWRAP));

            CVector flagPos;
            CVector2D flagSz;
            if (CSprite::CalcScreenCoors(m_Pos + CVector{ 0.f, 0.f, 3.f }, &flagPos, &flagSz.x, &flagSz.y, false, true)) {
                CSprite::RenderOneXLUSprite(
                    flagPos,
                    CVector2D{ flagSz.x * m_Size * 0.5f, flagSz.y * m_Size * 0.5f }, // Como el original: (w * tamaño) * 0.5
                    m_Colour.r,
                    m_Colour.g,
                    m_Colour.b,
                    255,
                    1.f / flagPos.z,
                    m_Colour.a,
                    0,
                    0
                );
            }

            RwRenderStateSet(rwRENDERSTATETEXTURERASTER,     RWRSTATE(0));
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
            RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE)); // Como el original: 0x725DA4 escribe 0
        }

        //> 0x725DEE - Render the marker (Checkpoints are basically just markers with some extra stuff)
        C3dMarkers::PlaceMarker(
            m_ID,
            e3dMarkerType::MARKER3D_TUBE,
            m_Pos,
            m_Size * 1.5f,
            m_Colour.r,
            m_Colour.g,
            m_Colour.b,
            128,
            m_PulsePeriod,
            m_PulseFraction,
            1,
            { 0.f, 0.f, 0.f },
            !wasUnderwater // `!` needed because the variable is inverted
        );
        
        //> 0x725DF6 - Render direction arrow too
        if (m_Type == eCheckpointType::TUBE) { // There was a variable that was only set to true if m_Type != eCheckpointType::ENDTUBE, so this condition boils down to this....
            C3dMarkers::DirectionArrowSet(
                m_Pos,
                m_Size * 0.625f,
                255,
                64,
                64,
                255,
                -m_Fwd.x,
                -m_Fwd.y,
                -m_Fwd.z
            );
        }

        break;
    }
    case eCheckpointType::TORUS: { // 0x725E6D
        // Como el original: distancia con fsqrt de (dz² + dy²) + dx²; se coloca solo si 2 * tamaño <= distancia (NaN no coloca)
        const auto playerPos = FindPlayerCoors();
        const float dx = playerPos.x - m_Pos.x, dy = playerPos.y - m_Pos.y, dz = playerPos.z - m_Pos.z;
        const float dist = std::sqrt(dz * dz + dy * dy + dx * dx);
        if (!(m_Size + m_Size <= dist)) {
            break;
        }
        [[fallthrough]];
    }
    case eCheckpointType::TORUS_NOFADE:
    case eCheckpointType::TORUSROT:
    case eCheckpointType::TORUS_UPDOWN:
    case eCheckpointType::TORUS_DOWN: { // 0x725FE1
        C3dMarkers::PlaceMarker(
            m_ID,
            MARKER3D_TORUS,
            m_Pos,
            m_Size,
            m_Colour.r,
            m_Colour.g,
            m_Colour.b,
            m_Colour.a,
            m_PulsePeriod,
            m_PulseFraction,
            m_RotateRate,
            m_Fwd,
            false
        );
        break;
    }
    case eCheckpointType::TORUSTHROUGH: { // 0x725F05
        const auto PlaceTorusMarker = [this](size_t i) {
            // NOTE: Originally this would underflow... Not a desired behaviour.
            const auto a = (int32)m_Colour.a - (int32)(16 * i);
            if constexpr (notsa::IsFixBugs()) {
                if (a < 0) {
                    return;
                }
            }
            // Como el original: (fwd * i) * multiplicador + pos; con i == 0 se pasa m_Pos directamente
            const float fi = (float)i;
            CVector p{ m_Fwd.x * fi * m_MultiSize + m_Pos.x, m_Fwd.y * fi * m_MultiSize + m_Pos.y, m_Fwd.z * fi * m_MultiSize + m_Pos.z };
            C3dMarkers::PlaceMarker(
                m_ID,
                MARKER3D_TORUS,
                i ? p : m_Pos,
                m_Size,
                m_Colour.r,
                m_Colour.g,
                m_Colour.b,
                (uint8)a,
                m_PulsePeriod,
                m_PulseFraction,
                m_RotateRate,
                m_Fwd,
                false
            );
        };
        for (size_t i = 1; i < 4; i++) {
            PlaceTorusMarker(i);
        }
        PlaceTorusMarker(0);
        break;
    }
    case eCheckpointType::NA:
        break; // See `CCheckpoint::Update` - This type is used for something (Perhaps to make a checkpoint invisible? Not sure.)
    default:
        break; // Como el original: cualquier tipo > 8 no dibuja nada (ja 0x72602F)
    }
}

// Based on 0x7229C0
void CCheckpoint::Update() {
    const auto UpdateRotFlag = [this](float maxSz, float minSz) {
        if (m_MultiSize > maxSz) {
            m_RotFlag = false;
        } else if (m_MultiSize < minSz) {
            m_RotFlag = true;
        }
    };

    if (m_Type == eCheckpointType::TORUSROT) { // 0x7229E7
        UpdateRotFlag(0.5f, -0.5f);
        m_MultiSize = m_RotFlag
            ? 0.1f
            : -0.1f;
        m_Fwd = (m_Fwd + CVector{m_MultiSize}).Normalized(); // 0x722A37
    }

    if (m_Type == eCheckpointType::TORUSTHROUGH) { // 0x722A53
        UpdateRotFlag(0.75f, 0.f);
        m_MultiSize += m_RotFlag // TODO/BUG: Use timestep here!
            ? 0.1f
            : -0.1f;
        if (m_MultiSize < 0.f) { // 0x722AA7
            m_Type = eCheckpointType::NA; // Weird?
        }
    }

    if (m_Type == eCheckpointType::TORUS_UPDOWN) { // 0x722ABD
        if (CVector2D::DistSqr(m_Pos, FindPlayerCoors()) < sq(m_Size)) { // 0x722B21
            m_Type      = eCheckpointType::TORUS_DOWN;
            m_MultiSize = -0.1f;
        } else { // 0x722B2E
            UpdateRotFlag(0.2f, -0.2f);
            m_MultiSize += m_RotFlag // TODO/BUG: Use timestep here!
                ? 0.02f
                : -0.02f;
            m_Pos.z += m_MultiSize;
        }
    }

    if (m_Type == eCheckpointType::TORUS_DOWN) { // 0x722B84
        if (CVector2D::DistSqr(m_Pos, FindPlayerCoors()) > sq(m_Size)) { // 0x722BE1
            m_Type      = eCheckpointType::TORUS_UPDOWN;
            m_RotFlag   = true;
            m_MultiSize = 0.02f;
        }

        if (FindPlayerCoors().z - 0.75f >= m_Pos.z) {
            NOTSA_LOG_DEBUG("Done"); // OG
        } else {
            m_Pos.z += m_MultiSize;
        }
    }

}

// Based on 0x722900
void CCheckpoint::SetPosition(const CVector& pos) {
    m_Pos.x = pos.x;
    m_Pos.y = pos.y;
    switch (m_Type) {
    case eCheckpointType::TORUS_UPDOWN: m_Pos.z = m_MultiSize + pos.z; break;
    case eCheckpointType::TORUS_DOWN:   break; // Ignore Z axis
    default:                            m_Pos.z = pos.z; break;
    }
}

// Based on 0x722FC0
void CCheckpoint::MarkAsDeleted() {
    m_IsUsed = false;
    m_Type   = eCheckpointType::NA;
    m_ID     = 0;
}

// Based on 0x722970
void CCheckpoint::SetHeading(float heading) {
    m_Fwd = CVector{
        std::cos(DegreesToRadians(heading)),
        std::sin(DegreesToRadians(heading)),
        m_Fwd.z
    }.Normalized();
}
