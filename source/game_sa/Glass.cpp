/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGlass
 * Reconstrucción: COMPLETA — 19/19 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 18/19 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Glass.h"
#include "FallingGlassPane.h"
#include "Shadows.h"

void CGlass::InjectHooks() {
    RH_ScopedClass(CGlass);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x71A8D0);
    RH_ScopedInstall(HasGlassBeenShatteredAtCoors, 0x71CB70);
    RH_ScopedInstall(CarWindscreenShatters, 0x71C2B0);
    RH_ScopedInstall(WasGlassHitByBullet, 0x71C0D0);
    RH_ScopedInstall(WindowRespondsToCollision, 0x71BC40);
    RH_ScopedInstall(GeneratePanesForWindow, 0x71B620);
    RH_ScopedInstall(Update, 0x71B0D0);
    RH_ScopedInstall(Render, 0x71CE20);
    RH_ScopedInstall(FindWindowSectorList<CPtrListSingleLink<CPhysical*>>, 0x71AFC0);
    RH_ScopedInstall(RenderReflectionPolys, 0x71AED0);
    RH_ScopedInstall(RenderShatteredPolys, 0x71AE30);
    RH_ScopedInstall(RenderHiLightPolys, 0x71ADA0);
    RH_ScopedInstall(CalcAlphaWithNormal, 0x71ACF0);
    RH_ScopedInstall(AskForObjectToBeRenderedInGlass, 0x71ACD0);
    RH_ScopedInstall(FindFreePane, 0x71ACA0);
    RH_ScopedInstall(WindowRespondsToSoftCollision, 0x71AF70);
    RH_ScopedInstall(BreakGlassPhysically, 0x71CF50);
    RH_ScopedInstall(WindowRespondsToExplosion, 0x71C1A0);
    RH_ScopedInstall(IsObjectGlass, 0x46A760);
}

namespace {
// Como el original: Multiply3x3 (0x59C790) con el orden de sumas del EXE
CVector Multiply3x3_59C790(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y,
    };
}

// Como el original: CMatrix * CVector (0x59C890) con el orden de sumas del EXE
CVector MultiplyPoint_59C890(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z,
    };
}

// Como el original (inline): Z mínima y máxima de los 4 vértices, por parejas (2,3) y (0,1)
void FindMinMaxZOfVertices(const CVector (&v)[4], float& minZ, float& maxZ) {
    const float min23 = v[2].z < v[3].z ? v[2].z : v[3].z;
    const float min01 = v[0].z < v[1].z ? v[0].z : v[1].z;
    minZ = min01 < min23 ? min01 : min23;

    const float max23 = v[2].z > v[3].z ? v[2].z : v[3].z;
    const float max01 = v[0].z > v[1].z ? v[0].z : v[1].z;
    maxZ = max01 > max23 ? max01 : max23;
}

// Como el original (inline): vértice 1..3 más lejano (distancia 2D con raíz) del vértice 0; en empate gana el primero
int32 FindVertexFurthestFromV0(const CVector (&v)[4]) {
    const auto Dist2D = [&](const CVector& p) {
        const float dx = p.x - v[0].x, dy = p.y - v[0].y;
        return std::sqrt(dy * dy + dx * dx);
    };
    int32 furthest = 1;
    float maxDist  = Dist2D(v[1]);
    for (int32 i = 2; i <= 3; i++) {
        const float dist = Dist2D(v[i]);
        if (dist > maxDist) {
            maxDist  = dist;
            furthest = i;
        }
    }
    return furthest;
}
} // namespace

// 0x71A8D0
void CGlass::Init() {
    for (auto& pane : aGlassPanes) {
        pane.m_bExist = false;
    }

    // Como el original: centro = suma de los 3 vértices · (1/3); la primera pieza suma 0+1+2 y las demás 2+1+0
    constexpr auto OneThird = std::bit_cast<float>(0x3EAAAAABu); // 0x859040
    for (auto i = 0u; i < std::size(PanePolyPositions); i++) {
        const auto& poly = PanePolyPositions[i];
        if (i == 0) {
            PanePolyCenterPositions[i].x = (poly[0].x + poly[1].x + poly[2].x) * OneThird;
            PanePolyCenterPositions[i].y = (poly[0].y + poly[1].y + poly[2].y) * OneThird;
        } else {
            PanePolyCenterPositions[i].x = (poly[2].x + poly[1].x + poly[0].x) * OneThird;
            PanePolyCenterPositions[i].y = (poly[2].y + poly[1].y + poly[0].y) * OneThird;
        }
    }
}

// (float, float, float ) & unused
// 0x71CB70
bool CGlass::HasGlassBeenShatteredAtCoors(CVector point) {
    // Como el original: sector = floor((c +/- 30) · 0.02 + 60) truncado con _ftol2, recortado a [0, 119]
    const auto SectorOf = [](float coord) {
        return (int32)(int64)std::floor(coord * 0.02f + 60.f);
    };
    const int32 sx0 = SectorOf(point.x - 30.f), startSectorX = sx0 > 0 ? sx0 : 0;
    const int32 sy0 = SectorOf(point.y - 30.f), startSectorY = sy0 > 0 ? sy0 : 0;
    const int32 sx1 = SectorOf(point.x + 30.f), endSectorX   = sx1 < MAX_SECTORS_X - 1 ? sx1 : MAX_SECTORS_X - 1;
    const int32 sy1 = SectorOf(point.y + 30.f), endSectorY   = sy1 < MAX_SECTORS_Y - 1 ? sy1 : MAX_SECTORS_Y - 1;

    CWorld::AdvanceCurrentScanCode();

    // Como el original: la misma función (0x71AFC0) recorre los objetos de los sectores repetidos y los dummies
    using ListType = CPtrListSingleLink<CPhysical*>;

    float maxDist = 20.f;
    CEntity* entity{};
    for (int32 sectorY = startSectorY; sectorY <= endSectorY; ++sectorY) {
        for (int32 sectorX = startSectorX; sectorX <= endSectorX; ++sectorX) {
            FindWindowSectorList<ListType>(reinterpret_cast<ListType&>(CWorld::GetRepeatSector(sectorX, sectorY).Objects), maxDist, entity, point);
            FindWindowSectorList<ListType>(reinterpret_cast<ListType&>(CWorld::GetSector(sectorX, sectorY).Dummies), maxDist, entity, point);
        }
    }
    // Como el original: se devuelve el bit 0x20 (cristal roto del todo), no el 0x10
    return entity && !entity->GetIsTypeDummy() && entity->AsObject()->objectFlags.bGlassBrokenAltogether;
}

// 0x71C2B0
void CGlass::CarWindscreenShatters(CVehicle* vehicle) {
    const auto colModel = vehicle->GetColModel();
    if (!colModel) {
        return;
    }
    const auto colData = colModel->m_pColData;
    if (!colData || (int16)colData->m_nNumTriangles < 2) {
        return;
    }

    // Como el original: se usan los dos primeros triángulos de cristal (no tienen por qué ser consecutivos)
    const CColTriangle* firstTri{};
    const CColTriangle* secondTri{};
    int32 firstTriIdx{ -1 };
    for (int32 i = 0; i < (int16)colData->m_nNumTriangles; i++) {
        if (!g_surfaceInfos.IsGlass(colData->m_pTriangles[i].m_nMaterial)) {
            continue;
        }
        if (!firstTri) {
            firstTri    = &colData->m_pTriangles[i];
            firstTriIdx = i;
        } else {
            secondTri = &colData->m_pTriangles[i];
            break;
        }
    }
    if (!secondTri) {
        return;
    }

    CCollision::CalculateTrianglePlanes(colModel);
    if (!colData->m_pTrianglePlanes) {
        return;
    }

    const auto& vehMat = vehicle->GetMatrix();

    // Normal del cristal en el espacio del mundo
    const auto normal = Multiply3x3_59C790(vehMat, colData->m_pTrianglePlanes[(int16)firstTriIdx].GetNormal());

    // Como el original: fwd = derecha x normal, right = normal x arriba (en ese orden)
    auto fwd = CrossProduct(vehMat.GetRight(), normal);
    fwd.Normalise();
    auto right = CrossProduct(normal, vehMat.GetUp());
    right.Normalise();

    CVector triVertices[6];
    const CColTriangle* const tris[2]{ firstTri, secondTri };
    for (auto t = 0; t < 2; t++) {
        for (auto v = 0; v < 3; v++) {
            triVertices[t * 3 + v] = MultiplyPoint_59C890(vehMat, colData->m_pVertices[tris[t]->m_vertIndices[v]]);
        }
    }

    // Como el original: productos escalares con el orden de sumas x, z, y
    float rightDots[6], fwdDots[6];
    for (auto i = 0; i < 6; i++) {
        const auto& v = triVertices[i];
        fwdDots[i]   = fwd.x * v.x + fwd.z * v.z + fwd.y * v.y;
        rightDots[i] = right.x * v.x + right.z * v.z + right.y * v.y;
    }

    // Como el original: se parte del vértice 0 y solo se sustituye con comparaciones estrictas
    float minSum   = rightDots[0] + fwdDots[0];
    float maxFwd   = fwdDots[0];
    float maxRight = rightDots[0];
    int16 minIdx   = 0;
    for (int16 i = 1; i < 6; i++) {
        const float sum = rightDots[i] + fwdDots[i];
        if (sum < minSum) {
            minSum = sum;
            minIdx = i;
        }
        if (fwdDots[i] > maxFwd) {
            maxFwd = fwdDots[i];
        }
        if (maxRight < rightDots[i]) {
            maxRight = rightDots[i];
        }
    }

    const float extentFwd   = maxFwd - fwdDots[minIdx];
    const float extentRight = maxRight - rightDots[minIdx];

    // Como el original: suena la rotura del parabrisas
    AudioEngine.ReportGlassCollisionEvent(AE_GLASS_BREAK_FAST, vehicle->GetPosition());

    const auto& blPos = triVertices[minIdx];
    const float halfRight = extentRight * 0.5f, halfFwd = extentFwd * 0.5f;
    const CVector center{
        blPos.x + fwd.x * halfFwd + right.x * halfRight,
        blPos.y + fwd.y * halfFwd + right.y * halfRight,
        blPos.z + fwd.z * halfFwd + right.z * halfRight,
    };
    GeneratePanesForWindow(
        ePaneType::CAR,
        blPos,
        CVector{ fwd.x * extentFwd, fwd.y * extentFwd, fwd.z * extentFwd },
        CVector{ right.x * extentRight, right.y * extentRight, right.z * extentRight },
        vehicle->m_vecMoveSpeed,
        center,
        0.1f,
        false,
        false,
        2,
        true
    );
}

bool IsGlassObjectWithCol(CEntity* entity) {
    if (entity->GetIsTypeObject() && entity->GetUsesCollision()) {
        if (const auto ami = entity->GetModelInfo()->AsAtomicModelInfoPtr()) {
            return ami->IsGlass();
        }
    }
    return false;
}

// 0x71C0D0
void CGlass::WasGlassHitByBullet(CEntity* entity, CVector hitPos) {
    if (!IsGlassObjectWithCol(entity))
        return;

    const auto object = entity->AsObject();
    if (object->objectFlags.bHasBrokenGlass) {
        if (CGeneral::GetRandomNumber() % 4 == 2) {
            WindowRespondsToCollision(entity, 0.0f, {}, hitPos, false);
        }
    } else {
        object->objectFlags.bHasBrokenGlass = true; // Just mark it as broken
    }
}

// 0x71BC40
void CGlass::WindowRespondsToCollision(CEntity* entity, float fDamageIntensity, CVector vecMoveSpeed, CVector vecPoint, bool max1PaneSection) {
    auto object = entity->AsObject();
    if (object->objectFlags.bGlassBrokenAltogether)
        return;

    object->objectFlags.bHasBrokenGlass = true;

    // Como el original: el modelo de colisión se toma directamente del model info
    if (const auto cd = CModelInfo::GetModelInfo(object->m_nModelIndex)->GetColModel()->m_pColData; cd && cd->m_nNumTriangles == 2) {
        // Object space vertices
        CVector verticesOS[4];
        for (auto i = 0; i < 4; i++) {
            verticesOS[i] = cd->m_pVertices[i];
        }

        float minZ, maxZ;
        FindMinMaxZOfVertices(verticesOS, minZ, maxZ);
        const auto& furthest = verticesOS[FindVertexFurthestFromV0(verticesOS)];

        // Transform vertices to world space
        const auto vert0Pos          = MultiplyPoint_59C890(object->GetMatrix(), { verticesOS[0].x, verticesOS[0].y, minZ });
        const auto furthestFromV0Pos = MultiplyPoint_59C890(object->GetMatrix(), { furthest.x, furthest.y, minZ });

        AudioEngine.ReportGlassCollisionEvent(AE_GLASS_BREAK_FAST, object->GetPosition());
        GeneratePanesForWindow(
            fDamageIntensity > 300.f ? ePaneType::DEFAULT : ePaneType::DELAYED, // Como el original: NaN -> DELAYED
            vert0Pos,
            { 0.f, 0.f, maxZ - minZ },
            furthestFromV0Pos - vert0Pos,
            vecMoveSpeed,
            vecPoint,
            0.1f,
            //object->objectFlags.bGlassBroken,
            (bool)((object->m_nObjectFlags >> 4) & 0xFFFFFF01),
            max1PaneSection,
            1,
            false
        );
    }

    object->SetUsesCollision(false);
    object->SetIsVisible(false);
    object->objectFlags.bGlassBrokenAltogether = true;
}

/*
 * 0x71B620
 *
 * Corners:
 * TL ----- TR
 * |  0 | 1 |
 * | ------ | fwd
 * |  2 | 3 |
 * BL ----- BR
 *  right
 *
 * - Neither 'size' vectors are normalized!
 *
 * type                    - @ePaneType
 * point                   - BL
 * fwd, right              - As illustrated above
 * center                  - The centre of the above rectangle (each pane is a piece of it)
 * velocity                - How fast the panes fly
 * velocityCenterDragCoeff - Modify the velocity's direction to be more towards the center point
 * bShatter                - Render shatter polys
 * numSectionsMax1         - Limit no. of sections to 1 (a count of 0 stays 0)
 * numSections             - No. of sections of each axis, the total number of sections will be `numSections ^ 2` (squared)
**/
void CGlass::GeneratePanesForWindow(ePaneType type, CVector point, CVector fwd, CVector right, CVector velocity, CVector center, float velocityCenterDragCoeff,
                                    bool bShatter, bool numSectionsMax1, int32 numSections, bool unk) {

    // Como el original: módulos con el orden de sumas z, y, x
    const float totalSizeY = std::sqrt(fwd.z * fwd.z + fwd.y * fwd.y + fwd.x * fwd.x);
    const float totalSizeX = std::sqrt(right.z * right.z + right.y * right.y + right.x * right.x);

    // Como el original: max(tamaño + 0.75, 1) truncado (_ftol2), por numSections, recortado a 3 (sin signo) y después a 1
    const auto CountOf = [&](float axisSize) {
        float rounded = axisSize + 0.75f;
        if (1.f > rounded) {
            rounded = 1.f;
        }
        auto count = (uint32)(int64)rounded * (uint32)numSections;
        if (count > 3) {
            count = 3;
        }
        if (numSectionsMax1 && count > 1) {
            count = 1;
        }
        return count;
    };
    const uint32 countY  = CountOf(totalSizeY);
    const uint32 countX  = CountOf(totalSizeX);
    const float  countYf = (float)countY, countXf = (float)countX;
    const float  sizeY   = totalSizeY / countYf;
    const float  sizeX   = totalSizeX / countXf;

    bool hitGround{};
    float groundZ = CWorld::FindGroundZFor3DCoord(point, &hitGround, nullptr);
    if (!hitGround)
        groundZ = point.z - 2.f;

    for (auto posY = 0u; posY < countY; posY++) {
        // Como el original: desplazamiento = índice · longitud / secciones
        const float offsetY = (float)posY * totalSizeY / countYf;
        for (auto posX = 0u; posX < countX; posX++) {
            const float offsetX = (float)posX * totalSizeX / countXf;
            for (auto piece = 0u; piece < std::size(PanePolyPositions); piece++) {
                CFallingGlassPane* pane = FindFreePane();
                if (!pane)
                    continue;

                pane->m_nPieceIndex = (uint8)piece;

                // Calculate matrix (Como el original: (1/módulo · componente) · tamaño)
                auto& mat = pane->m_Matrix;
                const float invRight = 1.f / totalSizeX;
                mat.GetRight() = CVector{ invRight * right.x * sizeX, invRight * right.y * sizeX, invRight * right.z * sizeX };
                const float invFwd = 1.f / totalSizeY;
                mat.GetUp() = CVector{ invFwd * fwd.x * sizeY, invFwd * fwd.y * sizeY, invFwd * fwd.z * sizeY };
                auto forward = CrossProduct(mat.GetRight(), mat.GetUp());
                forward.Normalise();
                mat.GetForward() = forward;

                // Matrix's position is the center of the glass pane
                const float paneX = sizeX * PanePolyCenterPositions[piece].x + offsetX;
                const float paneY = sizeY * PanePolyCenterPositions[piece].y + offsetY;
                mat.GetPosition() = CVector{
                    invFwd * fwd.x * paneY + invRight * right.x * paneX + point.x,
                    invFwd * fwd.y * paneY + invRight * right.y * paneX + point.y,
                    invFwd * fwd.z * paneY + invRight * right.z * paneX + point.z,
                };

                {
                    // Como el original: (rand & 0x7F) - 64 por 0.0015 (0x863E38); la Z se copia tal cual
                    constexpr auto VelocityFactor = std::bit_cast<float>(0x3AC49BA6u);
                    const auto rx = (int32)(CGeneral::GetRandomNumber() & 0x7F) - 64;
                    const auto ry = (int32)(CGeneral::GetRandomNumber() & 0x7F) - 64;
                    pane->m_Velocity = CVector{ velocity.x + (float)rx * VelocityFactor, velocity.y + (float)ry * VelocityFactor, velocity.z };
                }

                if (velocityCenterDragCoeff != 0.0f) {
                    auto dir = mat.GetPosition() - center;
                    dir.Normalise();
                    pane->m_Velocity.x = dir.x * velocityCenterDragCoeff + pane->m_Velocity.x;
                    pane->m_Velocity.y = dir.y * velocityCenterDragCoeff + pane->m_Velocity.y;
                    pane->m_Velocity.z = dir.z * velocityCenterDragCoeff + pane->m_Velocity.z;
                }

                {
                    // Como el original: por 0.002 (0x858F44); la X usa el segundo rand y la Y el primero
                    constexpr auto RandomFactor = std::bit_cast<float>(0x3B03126Fu);
                    const auto r0 = (int32)(CGeneral::GetRandomNumber() & 0x7F) - 64;
                    const auto r1 = (int32)(CGeneral::GetRandomNumber() & 0x7F) - 64;
                    const auto r2 = (int32)(CGeneral::GetRandomNumber() & 0x7F) - 64;
                    pane->m_RandomNumbers = CVector{ (float)r1 * RandomFactor, (float)r0 * RandomFactor, (float)r2 * RandomFactor };
                }

                switch (type) {
                case ePaneType::DELAYED: {
                    // Como el original: distancia con orden z, y, x; tiempo (fild) + dist · 100 redondeado a float y truncado
                    const auto& pos = mat.GetPosition();
                    const float dx = pos.x - center.x, dy = pos.y - center.y, dz = pos.z - center.z;
                    const float dist = std::sqrt(dz * dz + dy * dy + dx * dx);
                    const auto now = CTimer::GetTimeInMS();
                    const double nowF = (int32)now < 0 ? (double)(float)((double)(int32)now + 4294967296.0) : (double)(int32)now;
                    pane->m_nCreatedTime = (uint32)(int64)(float)(nowF + (double)(dist * 100.f));
                    break;
                }
                case ePaneType::CAR:
                case ePaneType::DEFAULT:{
                    pane->m_nCreatedTime = CTimer::GetTimeInMS();
                    break;
                }
                }

                pane->m_fGroundZ = groundZ;
                pane->m_bRenderShatter = bShatter;
                pane->m_fSize = sizeY;
                pane->m_f6F = unk;
                pane->m_bExist = true;
            }
        }
    }
}

// 0x71B0D0
void CGlass::Update() {
    ZoneScoped;

    for (auto& pane : aGlassPanes) {
        if (pane.m_bExist) {
            pane.Update();
        }
    }
}

// 0x71CE20
void CGlass::Render() {
    ZoneScoped;

    H1iLightPolyVerticesIdx = 0;
    HiLightPolyIndicesIdx = 0;

    ShatteredVerticesBaseIdx = 1024;
    ShatteredIndicesBaseIdx = 2048;
    ReflectionPolyVertexBaseIdx = 1536;
    ReflectionPolyIndexBaseIdx = 3072;

    // Como el original: color de niebla con los componentes uint16 sin recortar: ((0xFFFFFF00 | r) << 8 | g) << 8 | b
    const uint32 fogColor = ((((0xFFFFFF00u | (uint32)CTimeCycle::GetSkyBottomRed()) << 8) | (uint32)CTimeCycle::GetSkyBottomGreen()) << 8) | (uint32)CTimeCycle::GetSkyBottomBlue();

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER,     RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGCOLOR,          RWRSTATE(fogColor));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));

    for (auto& pane : aGlassPanes) {
        if (pane.m_bExist) {
            pane.Render();
        }
    }

    RenderHiLightPolys();
    RenderShatteredPolys();
    RenderReflectionPolys();

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,  RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,     RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,    RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,    RWRSTATE(FALSE));
}

// (..., CEntity**, float, float, float)
// 0x71AFC0
template<typename PtrListType>
void CGlass::FindWindowSectorList(PtrListType& objList, float& outDist, CEntity*& outEntity, CVector point) {
    // Como el original: recorre los nodos directamente (item en +0, siguiente en +4); el EXE también le pasa listas de dummies
    for (auto* node = objList.GetNode(); node;) {
        auto* const entity = static_cast<CEntity*>(node->Item);
        node = node->Next;

        if (!entity->GetIsTypeObject())
            continue;

        const auto ami = CModelInfo::GetModelInfo(entity->m_nModelIndex)->AsAtomicModelInfoPtr();
        if (!ami || !ami->IsGlass())
            continue;

        if (entity->IsScanCodeCurrent())
            continue;
        entity->SetScanCode(CWorld::ms_nCurrentScanCode);

        // Como el original: distancia con orden de sumas z, y, x
        const auto& pos = entity->GetPosition();
        const float dx = point.x - pos.x, dy = point.y - pos.y, dz = point.z - pos.z;
        const float dist = std::sqrt(dz * dz + dy * dy + dx * dx);
        if (dist < outDist) {
            outEntity = entity;
            outDist = dist;
        }
    }
}

// 0x71AED0
void CGlass::RenderReflectionPolys() {
    if (ReflectionPolyVertexBaseIdx != 1536) {

        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpShadowHeadLightsTex)));
        RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDINVSRCALPHA));

        if (RwIm3DTransform(ReflectionPolyVertexBuffer.data(), ReflectionPolyVertexBaseIdx - 1536, nullptr, rwIM3D_VERTEXUV))
        {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, &aTempBufferIndices[3072], ReflectionPolyIndexBaseIdx - 3072);
            RwIm3DEnd();
        }

        ReflectionPolyVertexBaseIdx = 1536;
        ReflectionPolyIndexBaseIdx = 3072;
    }
}

// 0x71AE30
void CGlass::RenderShatteredPolys() {
    if (ShatteredVerticesBaseIdx != 1024)
    {
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpCrackedGlassTex)));
        RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDINVSRCALPHA));

        if (RwIm3DTransform(ShatteredPolyVertexBuffer.data(), ShatteredVerticesBaseIdx - 1024, nullptr, rwIM3D_VERTEXUV))
        {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, &aTempBufferIndices[2048], ShatteredIndicesBaseIdx - 2048);
            RwIm3DEnd();
        }

        ShatteredVerticesBaseIdx = 1024;
        ShatteredIndicesBaseIdx = 2048;
    }
}

// 0x71ADA0
void CGlass::RenderHiLightPolys() {
    if (H1iLightPolyVerticesIdx != 0)
    {
        RwRenderStateSet(rwRENDERSTATESRCBLEND,      RWRSTATE(rwBLENDONE));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,     RWRSTATE(rwBLENDONE));
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(RwTextureGetRaster(gpShadowExplosionTex)));

        if (RwIm3DTransform(TempBufferVertices.m_3d, H1iLightPolyVerticesIdx, nullptr, rwIM3D_VERTEXUV)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, HiLightPolyIndicesIdx);
            RwIm3DEnd();
        }

        HiLightPolyIndicesIdx = 0;
        H1iLightPolyVerticesIdx = 0;
    }
}

// 0x71ACF0
uint8 CGlass::CalcAlphaWithNormal(const CVector& normal) {
    // Como el original: la dirección de la cámara sale de TheCamera.m_mCameraMatrix (0xB6F9AC), no de su CPlaceable
    const auto& camFwd = TheCamera.m_mCameraMatrix.GetForward();

    if (notsa::IsFixBugs()) {
        // reflect(I, N) = I - 2 * project(I, N) where `N` is assumed to be normalized (eg.: unit vector)
        const auto reflect = camFwd - 2.0f * camFwd.ProjectOnToNormal(normal);
        static const auto LightDir = CVector{ 1.0f, 1.0f, -1.0f }.Normalized();
        return static_cast<uint8>(std::pow(reflect.Dot(LightDir), 6.0f) * 235.0f + 20.0f);
    }

    // Como el original: reflejo con el producto escalar en orden y, z, x; luz (0.57, 0.57, -0.57) como (rx + ry) · 0.57 - rz · 0.57;
    // potencia 6 con 5 multiplicaciones; resultado truncado con _ftol2 (sin recortar a 0..255)
    constexpr auto LightFactor = std::bit_cast<float>(0x3F11EB85u); // 0.57 (0x872728)
    const float dot  = camFwd.y * normal.y + camFwd.z * normal.z + camFwd.x * normal.x;
    const float dot2 = dot + dot;
    const float rx = camFwd.x - dot2 * normal.x;
    const float ry = camFwd.y - dot2 * normal.y;
    const float rz = camFwd.z - dot2 * normal.z;
    const float specular = (rx + ry) * LightFactor - rz * LightFactor;

    float power = specular * specular;
    power = power * specular;
    power = power * specular;
    power = power * specular;
    power = power * specular;
    return (uint8)(int64)(power * 235.0f + 20.0f);
}

// 0x71ACD0
void CGlass::AskForObjectToBeRenderedInGlass(CEntity* entity) {
    if (NumGlassEntities < std::size(apEntitiesToBeRendered) - 1) {
        apEntitiesToBeRendered[NumGlassEntities++] = entity;
    }
}

// 0x71ACA0
CFallingGlassPane* CGlass::FindFreePane() {
    for (auto& pane : aGlassPanes) {
        if (!pane.m_bExist)
            return &pane;
    }
    return nullptr;
}

// 0x71AF70
void CGlass::WindowRespondsToSoftCollision(CEntity* entity, float fDamageIntensity) {
    if (entity->GetUsesCollision() && fDamageIntensity > 50.f && !entity->AsObject()->objectFlags.bHasBrokenGlass) {
        AudioEngine.ReportGlassCollisionEvent(AE_GLASS_HIT, entity->GetPosition());
        entity->AsObject()->objectFlags.bHasBrokenGlass = true;
    }
}

// 0x71CF50
void CGlass::BreakGlassPhysically(CVector point, float radius) {
    if (CTimer::GetTimeInMS() < LastColCheckMS + 1000 && CTimer::GetTimeInMS() >= LastColCheckMS)
        return;

    // Como el original: el pool de objetos se recorre desde el final hacia el principio
    auto* const pool = GetObjectPool();
    for (auto idx = (int32)pool->GetSize(); idx-- > 0;) {
        if (pool->IsFreeSlotAtIndex(idx))
            continue;

        auto& object = *pool->GetAt(idx);
        if (!IsGlassObjectWithCol(&object))
            continue;

        // Como el original: el modelo de colisión se toma directamente del model info
        const auto colModel = CModelInfo::GetModelInfo(object.m_nModelIndex)->GetColModel();
        const auto colData  = colModel->m_pColData;
        if (!colData)
            continue;

        if ((int16)colData->m_nNumTriangles < 2)
            continue;

        const auto& objPos = object.GetPosition();
        const auto& objMat = object.GetMatrix();
        // Test if point touches any of the model's triangles
        {
            // Como el original: el punto pasa al espacio del objeto con la traspuesta (productos escalares z, y, x)
            const float dx = point.x - objPos.x, dy = point.y - objPos.y, dz = point.z - objPos.z;
            const CColSphere sphere{
                CVector{
                    dz * objMat.GetRight().z   + dy * objMat.GetRight().y   + dx * objMat.GetRight().x,
                    dz * objMat.GetForward().z + dy * objMat.GetForward().y + dx * objMat.GetForward().x,
                    dz * objMat.GetUp().z      + dy * objMat.GetUp().y      + dx * objMat.GetUp().x,
                },
                radius
            };
            CCollision::CalculateTrianglePlanes(colModel);

            bool hasHit{};
            for (auto tri = 0; tri < (int16)colData->m_nNumTriangles; tri++) {
                if (CCollision::TestSphereTriangle(sphere, colData->m_pVertices, colData->m_pTriangles[tri], colData->m_pTrianglePlanes[tri])) {
                    hasHit = true;
                }
            }
            if (!hasHit)
                continue;
        }

        LastColCheckMS = CTimer::GetTimeInMS();

        if (!object.objectFlags.bHasBrokenGlass) {
            AudioEngine.ReportGlassCollisionEvent(AE_GLASS_HIT, objPos);
            object.objectFlags.bHasBrokenGlass = true;
            continue; // Como el original: sigue con el resto de objetos
        }

        CVector verticesObjSpace[4];
        for (auto i = 0u; i < std::size(verticesObjSpace); ++i) {
            verticesObjSpace[i] = colData->m_pVertices[i];
        }

        float minZ, maxZ;
        FindMinMaxZOfVertices(verticesObjSpace, minZ, maxZ);
        const auto& furthestOfV0 = verticesObjSpace[FindVertexFurthestFromV0(verticesObjSpace)];

        const auto v0Pos           = MultiplyPoint_59C890(objMat, { verticesObjSpace[0].x, verticesObjSpace[0].y, minZ });
        const auto furthestOfV0Pos = MultiplyPoint_59C890(objMat, { furthestOfV0.x, furthestOfV0.y, minZ });

        AudioEngine.ReportGlassCollisionEvent(AE_GLASS_BREAK_FAST, objPos); // Como el original: rotura rápida

        GeneratePanesForWindow(
            ePaneType::DELAYED,
            v0Pos,
            { 0.f, 0.f, maxZ - minZ },
            furthestOfV0Pos - v0Pos,
            {},
            point,
            0.1f,
            object.objectFlags.bHasBrokenGlass,
            false,
            1,
            false
        );
        object.SetUsesCollision(false);
        object.SetIsVisible(false);
        object.objectFlags.bGlassBrokenAltogether = true; // Como el original: bit 0x20 (no 0x10)
    }
}

// 0x71C1A0
void CGlass::WindowRespondsToExplosion(CEntity* entity, CVector pos) {
    if (!entity->GetUsesCollision()) {
        return;
    }

    const auto& entityPos = entity->GetPosition();
    const auto entityToPosDir = entityPos - pos;
    // Como el original: módulo con orden z, y, x; con NaN no se hace nada
    const auto dist = std::sqrt(entityToPosDir.z * entityToPosDir.z + entityToPosDir.y * entityToPosDir.y + entityToPosDir.x * entityToPosDir.x);
    if (dist < 10.f) {
        WindowRespondsToCollision(entity, 10000.f, entityToPosDir * (0.3f / dist), entityPos, true);
    } else if (dist < 30.f) {
        entity->AsObject()->objectFlags.bHasBrokenGlass = true;
    }
}

// 0x46A760
bool CGlass::IsObjectGlass(CEntity* entity) {
    if (!entity->GetIsTypeObject()) {
        return false;
    }

    auto mi = CModelInfo::GetModelInfo(entity->GetModelIndex());
    if (!mi->AsAtomicModelInfoPtr()) {
        return false;
    }

    return mi->IsGlass();
}
