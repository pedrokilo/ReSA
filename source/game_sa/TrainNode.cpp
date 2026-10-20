/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTrainNode
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
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

#include "TrainNode.h"

void CTrainNode::InjectHooks() {
    RH_ScopedClass(CTrainNode);
    RH_ScopedCategory("Vehicle");

    RH_ScopedNamedInstall(GetLightingFromCollisionExeAbi, "GetLightingFromCollision", 0x6F5F80);
}

// 0x6F5370
CTrainNode::CTrainNode() {
    m_bSurfLightingFound = false;
}

// 0x6F5380
void CTrainNode::SetX(float X) {
    x = int16(X * 8.0f);
}

// 0x6F53A0
void CTrainNode::SetY(float Y) {
    y = int16(Y * 8.0f);
}

// 0x6F53C0
void CTrainNode::SetZ(float Z) {
    z = int16(Z * 8.0f);
}

// 0x6F53E0
float CTrainNode::GetX() const {
    return (float)x * 0.125f;
}

// 0x6F5400
float CTrainNode::GetY() const {
    return (float)y * 0.125f;
}

// 0x6F5420
float CTrainNode::GetZ() const {
    return (float)z * 0.125f;
}

// 0x6F5440
CVector CTrainNode::GetPosn() const {
    return { GetX(), GetY(), GetZ() };
}

// 0x6F5490
void CTrainNode::SetDistanceFromStart(float dist) {
    m_nDistanceFromStart = int16(dist * 3.0f);
}

// 0x6F54B0
float CTrainNode::GetDistanceFromStart() const {
    // Como el original (0x6F54B0): uint16 * 0.33333334f (0x859040), no / 3.0f
    return static_cast<float>(m_nDistanceFromStart) * std::bit_cast<float>(0x3EAAAAABu);
}

// 0x6F5F80
// Iluminacion de la superficie bajo el nodo: se calcula una sola vez con una linea vertical hacia abajo (solo edificios) y se cachea
// el byte de iluminacion del lado B del punto de colision. Sin colision devuelve 0x48 (dia 8, noche 4) sin cachear.
tColLighting CTrainNode::GetLightingFromCollision() {
    if (m_bSurfLightingFound) {
        return m_nSurfaceLighting;
    }

    CColPoint cp;
    CEntity*  entity;
    CVector   pos = GetPosn();
    pos.z += 1.0f;
    if (!CWorld::ProcessVerticalLine(pos, -1000.0f, cp, entity, true, false, false, false, false, false, nullptr)) {
        return tColLighting{ 0x48 };
    }

    m_nSurfaceLighting   = cp.m_nLightingB;
    m_bSurfLightingFound = true;
    return m_nSurfaceLighting;
}

// 0x6F5F80 (destino del hook)
uint8 CTrainNode::GetLightingFromCollisionExeAbi() {
    return GetLightingFromCollision().value;
}
