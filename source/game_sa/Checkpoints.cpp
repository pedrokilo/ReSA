/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCheckpoints
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Checkpoints.h"
#include "Checkpoint.h"
#include "X87Math.h"

void CCheckpoints::InjectHooks() {
    RH_ScopedClass(CCheckpoints);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x722880);
    RH_ScopedInstall(Shutdown, 0x7228F0);
    RH_ScopedInstall(Update, 0x7229C0);
    RH_ScopedInstall(Render, 0x726060);
    RH_ScopedNamedInstall(PlaceMarker_OG, "PlaceMarker", 0x722C40);
    RH_ScopedInstall(DeleteCP, 0x722FC0);
    RH_ScopedInstall(SetHeading, 0x722970);
    RH_ScopedInstall(UpdatePos, 0x722900);
}

// 0x722880
void CCheckpoints::Init() {
    rng::for_each(m_aCheckPtArray, &CCheckpoint::Init);
    NumActiveCPts = 0;
}

// 0x7228F0
void CCheckpoints::Shutdown() {
    // NOP
}

// NOTSA
CCheckpoint* CCheckpoints::FindById(uint32 id) {
    for (auto& checkpoint : m_aCheckPtArray) {
        if (checkpoint.m_ID == id) {
            return &checkpoint;
        }
    }
    return nullptr;
}

// 0x722970
void CCheckpoints::SetHeading(uint32 id, float headingDeg) {
    if (auto* cp = FindById(id)) {
        // Como el original: fcos/fsin del x87 (precisión completa, X87Math.cpp) sobre el ángulo ya redondeado a float.
        const float rad = headingDeg * DEG_TO_RAD;
        const float outCos = TrigSimpleCosCpp(rad);
        const float outSin = TrigSimpleSinCpp(rad);
        cp->m_Fwd.x = outCos;
        cp->m_Fwd.y = outSin;
        cp->m_Fwd.Normalise();
    }
}

// 0x7229C0
void CCheckpoints::Update() {
    // Como el original: la posición del jugador se pide una sola vez para todos los checkpoints.
    const CVector player = FindPlayerCoors(-1);

    for (auto& cp : m_aCheckPtArray) {
        // Como el original: dos comprobaciones independientes; solo se escribe si cambia.
        const auto UpdateRotFlag = [&cp](float maxSz, float minSz) {
            if (cp.m_MultiSize > maxSz && cp.m_RotFlag) {
                cp.m_RotFlag = false;
            }
            if (cp.m_MultiSize < minSz && !cp.m_RotFlag) {
                cp.m_RotFlag = true;
            }
        };
        // Como el original: dy² + dx² con las diferencias jugador - checkpoint.
        const auto Dist2DSq = [&cp, &player] {
            const float dx = player.x - cp.m_Pos.x;
            const float dy = player.y - cp.m_Pos.y;
            return dy * dy + dx * dx;
        };

        if (cp.m_Type == eCheckpointType::TORUSROT) { // 0x7229E7
            UpdateRotFlag(0.5f, -0.5f);
            cp.m_MultiSize = cp.m_RotFlag ? 0.1f : -0.1f;
            cp.m_Fwd.x = cp.m_MultiSize + cp.m_Fwd.x;
            cp.m_Fwd.y = cp.m_Fwd.y + cp.m_MultiSize;
            cp.m_Fwd.z = cp.m_MultiSize + cp.m_Fwd.z;
            cp.m_Fwd.Normalise(); // 0x722A4E
        }

        if (cp.m_Type == eCheckpointType::TORUSTHROUGH) { // 0x722A53
            UpdateRotFlag(0.75f, 0.f);
            // Como el original: paso de 0,01 (0x858C58). TODO/BUG: no usa el timestep.
            cp.m_MultiSize = cp.m_RotFlag ? cp.m_MultiSize + 0.01f : cp.m_MultiSize - 0.01f;
            if (cp.m_MultiSize < 0.f) { // 0x722AA7
                cp.m_Type = eCheckpointType::NA;
            }
        }

        if (cp.m_Type == eCheckpointType::TORUS_UPDOWN) { // 0x722ABD
            // Como el original (fcompp + test ah,1): baja si tamaño² >= distancia²; con NaN no baja.
            if (cp.m_Size * cp.m_Size >= Dist2DSq()) {
                cp.m_Type      = eCheckpointType::TORUS_DOWN;
                cp.m_MultiSize = -0.1f;
            } else { // 0x722B2E
                UpdateRotFlag(0.2f, -0.2f);
                cp.m_MultiSize = cp.m_RotFlag ? cp.m_MultiSize + 0.02f : cp.m_MultiSize - 0.02f;
                cp.m_Pos.z = cp.m_MultiSize + cp.m_Pos.z;
            }
        }

        if (cp.m_Type == eCheckpointType::TORUS_DOWN) { // 0x722B84
            if (cp.m_Size * cp.m_Size < Dist2DSq()) { // 0x722BE1
                cp.m_Type      = eCheckpointType::TORUS_UPDOWN;
                cp.m_RotFlag   = true;
                cp.m_MultiSize = 0.02f;
            }

            // Como el original: con NaN tampoco se mueve (el EXE imprime "Done").
            if (player.z - 0.75f < cp.m_Pos.z) {
                cp.m_Pos.z = cp.m_MultiSize + cp.m_Pos.z;
            } else {
                NOTSA_LOG_DEBUG("Done"); // OG
            }
        }
    }
}

// NOTSA: firma cómoda con CRGBA
CCheckpoint* CCheckpoints::PlaceMarker(
    uint32 id,
    eCheckpointTypeU16 type,
    const CVector& pos,
    const CVector& direction,
    float size,
    CRGBA color,
    uint16 pulsePeriod,
    float pulseFraction,
    int16 rotateRate
) {
    return PlaceMarker_OG(id, (uint16)(eCheckpointType)type, pos, direction, size, color.r, color.g, color.b, color.a, pulsePeriod, pulseFraction, rotateRate);
}

// 0x722C40
CCheckpoint* CCheckpoints::PlaceMarker_OG(
    uint32 id,
    uint16 type,
    const CVector& pos,
    const CVector& direction,
    float size,
    uint8 r,
    uint8 g,
    uint8 b,
    uint8 a,
    uint16 pulsePeriod,
    float pulseFraction,
    int16 rotateRate
) {
    const float cpDistToPlayer2D = CVector2D::Dist(pos, FindPlayerCentreOfWorld(0));

    CCheckpoint* cp{};

    // 0x722C94 - Como el original: solo se reutiliza un checkpoint en uso con el mismo id
    for (auto& v : m_aCheckPtArray) {
        if (v.m_IsUsed && v.m_ID == id) {
            cp = &v;
            break;
        }
    }

    if (cp) {
        switch (type) {
        case (uint16)eCheckpointType::TORUS:
        case (uint16)eCheckpointType::TORUSROT:
        case (uint16)eCheckpointType::TORUS_UPDOWN: {
            // Como el original: cuatro llamadas a FindPlayerCoors y fsqrt(dy*dy + dx*dx) < 2 (NaN no cambia el tipo)
            const auto p1 = FindPlayerCoors(-1);
            const auto p2 = FindPlayerCoors(-1);
            const auto p3 = FindPlayerCoors(-1);
            const float dy = pos.y - p3.y;
            const auto p4 = FindPlayerCoors(-1);
            if (std::sqrt((pos.y - p4.y) * dy + (pos.x - p2.x) * (pos.x - p1.x)) < 2.f) {
                type = type == (uint16)eCheckpointType::TORUS_UPDOWN
                    ? (uint16)eCheckpointType::TORUS_DOWN
                    : (uint16)eCheckpointType::TORUSTHROUGH;
            }
            break;
        }
        }
    } else {
        //> 0x722D7A - Try finding an unused one
        for (auto& v : m_aCheckPtArray) {
            if (v.m_Type == eCheckpointType::NA) {
                cp = &v;
                break;
            }
        }

        //> 0x722DA6 - Find furthest one from player and use that
        if (!cp) {
            for (auto& v : m_aCheckPtArray) {
                if (cpDistToPlayer2D < v.m_DistToCam2D && (!cp || v.m_DistToCam2D > cp->m_DistToCam2D)) { // NOTE/BUG: Not checking if the cp is in use!
                    cp = &v;
                }
            }
            // El original pone el tipo a NA (0x722F02), pero se sobrescribe abajo.
        }
    }

    if (cp) { // 0x722F07
        cp->m_DistToCam2D   = cpDistToPlayer2D;
        cp->m_Colour        = CRGBA{ r, g, b, a };
        cp->m_Size          = size;
        cp->m_RotateRate    = rotateRate;
        cp->m_Pos           = pos;
        cp->m_Fwd           = (direction - pos).Normalized();
        cp->m_ID            = id;
        cp->m_Type          = (eCheckpointType)type;
        cp->m_PulsePeriod   = pulsePeriod;
        cp->m_IsUsed        = true;
        cp->m_PulseFraction = pulseFraction;
    }

    return cp; // May be null
}

// 0x722900
void CCheckpoints::UpdatePos(uint32 id, const CVector& posn) {
    if (const auto cp = FindById(id)) {
        cp->SetPosition(posn);
    }
}

// 0x722FC0
void CCheckpoints::DeleteCP(uint32 id, uint16 type) {
    for (auto& cp : m_aCheckPtArray) {
        if (cp.m_IsUsed && cp.m_ID == id && cp.m_Type == (eCheckpointType)type) {
            cp.MarkAsDeleted();
        }
    }
}

// 0x726060
void CCheckpoints::Render() {
    ZoneScoped;

    for (auto& checkpoint : m_aCheckPtArray) {
        if (checkpoint.m_IsUsed) {
            checkpoint.Render();
        }
    }
    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
}
