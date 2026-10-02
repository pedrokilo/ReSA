/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: BreakObject_c
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "BreakObject_c.h"
#include "Plugins/BreakablePlugin/BreakablePlugin.h"
#include "PostEffects.h"

void BreakObject_c::InjectHooks() {
    RH_ScopedClass(BreakObject_c);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(CalcGroupCenter, 0x59D190);
    RH_ScopedInstall(SetGroupData, 0x59D570);
    RH_ScopedInstall(SetBreakInfo, 0x59D7F0);
    RH_ScopedInstall(Exit, 0x59DDD0);
    RH_ScopedInstall(DoCollisionResponse, 0x59DE40);
    RH_ScopedInstall(DoCollision, 0x59E1F0);
    RH_ScopedInstall(Update, 0x59E220);
    RH_ScopedInstall(Render, 0x59E480);
    RH_ScopedInstall(Init, 0x59E750);
}

// 0x59D170
BreakObject_c::BreakObject_c() {
    m_JustFaces = false;
    m_bActive = false;
    m_BreakGroups = nullptr;
}

// 0x59C890 con su orden de sumas (CMatrix * CVector)
static CVector MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) {
    return {
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// 0x59E750
bool BreakObject_c::Init(CObject* object, const CVector* velocity, float fVelocityRand, int32 bJustFaces) {
    if (!object->GetRwObject() || RwObjectGetType(object->GetRwObject()) != rpATOMIC)
        return false;

    auto* info = BREAKABLEPLG(RpAtomicGetGeometry(object->GetRpAtomic()), m_pBreakableInfo);
    if (!info)
        return false;

    SetBreakInfo(info, bJustFaces);
    auto ltm = RwFrameGetLTM(RpAtomicGetFrame(object->GetRpAtomic()));
    SetGroupData(ltm, velocity, fVelocityRand);

    // El original copia bytes en crudo (sin normalizar a bool)
    reinterpret_cast<uint8&>(m_JustFaces) = (uint8)bJustFaces;
    m_bActive = true;
    m_FramesActive = 0;
    reinterpret_cast<uint8&>(m_AddSparks) = (uint8)object->m_pObjectInfo->m_nSparksOnImpact;
    m_bDrawLast = object->m_bDrawLast;

    CVector vecOrigin;
    const auto& bb = object->GetModelInfo()->GetColModel()->GetBoundingBox();
    if (info->m_uiPosRule != eBreakablePluginPositionRule::OBJECT_ORIGIN) {
        const CVector usedPoint{
            (bb.m_vecMax.x - bb.m_vecMin.x) * 0.5f + bb.m_vecMin.x,
            (bb.m_vecMax.y - bb.m_vecMin.y) * 0.5f + bb.m_vecMin.y,
            ((bb.m_vecMax.z - bb.m_vecMin.z) * 0.5f + bb.m_vecMin.z) + 0.25f,
        };
        vecOrigin = MatrixTimesPoint59C890(*object->m_matrix, usedPoint);
    } else {
        vecOrigin = *RwMatrixGetPos(ltm);
        vecOrigin.z = (vecOrigin.z + bb.m_vecMin.z) + 0.25f;
    }

    CColPoint colPoint;
    CEntity* colEntity;
    if (CWorld::ProcessVerticalLine(vecOrigin, -1000.0f, colPoint, colEntity, true)) {
        m_GroundZ = colPoint.m_vecPoint.z;
        m_VecNormal = colPoint.m_vecNormal;
        if (std::fabs(m_VecNormal.x) < 0.01f &&
            std::fabs(m_VecNormal.y) < 0.01f &&
            std::fabs(m_VecNormal.z) < 0.01f
        ) {
            m_VecNormal.Set(0.0f, 0.0f, 1.0f);
        }
    } else {
        m_GroundZ = -1000.0f;
        m_VecNormal.Set(0.0f, 0.0f, 1.0f);
    }

    return true;
}

// 0x59DDD0
void BreakObject_c::Exit() {
    // Como el original: sin grupos no se libera nada; operator delete (no delete[]) y solo punteros no nulos.
    if (m_BreakGroups) {
        for (auto i = 0; i < m_NumBreakGroups; ++i) {
            auto& group = m_BreakGroups[i];
            if (group.m_Texture) {
                RwTextureDestroy(group.m_Texture);
                group.m_Texture = nullptr;
            }
            if (group.m_RenderInfo) {
                operator delete(group.m_RenderInfo);
            }
        }
        operator delete(m_BreakGroups);
    }
    m_bActive = false;
}

// 0x59D190
void BreakObject_c::CalcGroupCenter(BreakGroup_t* group) {
    CBoundingBox bbox(
        { +9999999.0f, +9999999.0f, +9999999.0f }, // todo: replace one million - 1 with a more comprehensible
        { -9999999.0f, -9999999.0f, -9999999.0f }
    );
    auto& vMin = bbox.m_vecMin;
    auto& vMax = bbox.m_vecMax;

    for (auto& info : group->GetRenderInfos()) {
        for (auto& position : info.positions) {
            vMin.x = std::min(vMin.x, position.x);
            vMax.x = std::max(vMax.x, position.x);
            vMin.y = std::min(vMin.y, position.y);
            vMax.y = std::max(vMax.y, position.y);
            vMin.z = std::min(vMin.z, position.z);
            vMax.z = std::max(vMax.z, position.z);
        }
    }

    auto vecCenter = bbox.GetCenter();
    for (auto& info : group->GetRenderInfos()) {
        for (auto& position : info.positions) {
            position -= vecCenter;
        }
    }

    vMin -= vecCenter;
    vMax -= vecCenter;

    RwV3d vecTransformedCenter;
    RwV3dTransformVector(&vecTransformedCenter, &vecCenter, &group->m_Matrix);
    auto* pos = RwMatrixGetPos(&group->m_Matrix);
    RwV3dAdd(pos, pos, &vecTransformedCenter);

    auto width  = bbox.GetWidth();
    auto length = bbox.GetLength();
    auto height = bbox.GetHeight();

    if (width <= length && width <= height) {
        group->m_Type = BreakGroupType::RIGHT;
        group->m_BoundingSize = width / 2.0f;
    } else if (length <= width && length <= height) {
        group->m_Type = BreakGroupType::UP;
        group->m_BoundingSize = length / 2.0f;
    } else if (height <= length && height <= width) {
        group->m_Type = BreakGroupType::AT;
        group->m_BoundingSize = height / 2.0f;
    }
}

// 0x59D570
// Fórmulas en línea del original: unidad aleatoria rand * (1/32767), sumas en su orden y RwV3dNormalize.
void BreakObject_c::SetGroupData(const RwMatrix* matrix, const CVector* vecVelocity, float fVelocityRand) {
    const auto RandUnit = [] { return (float)CGeneral::GetRandomNumber() * (1.0f / 32767.0f); };
    const float minVel = -fVelocityRand, rangeVel = fVelocityRand - minVel;

    for (auto& group : GetBreakGroups()) {
        group.m_Matrix = *matrix;

        CalcGroupCenter(&group);

        if (vecVelocity) {
            group.m_Velocity = *vecVelocity;
            if (fVelocityRand != 0.0f) {
                group.m_Velocity.x = (rangeVel * RandUnit() + group.m_Velocity.x) + minVel;
                group.m_Velocity.y = (rangeVel * RandUnit() + group.m_Velocity.y) + minVel;
                group.m_Velocity.z = (rangeVel * RandUnit() + group.m_Velocity.z) + minVel;
            }
        } else {
            group.m_Velocity.x = rangeVel * RandUnit() + minVel;
            group.m_Velocity.y = rangeVel * RandUnit() + minVel;
            group.m_Velocity.z = rangeVel * RandUnit() + minVel;
        }

        group.m_RotationSpeed = RandUnit() * 3.0f + 3.0f;

        group.m_RotationAxis.x = RandUnit() * 2.0f - 1.0f;
        group.m_RotationAxis.y = RandUnit() * 2.0f - 1.0f;
        group.m_RotationAxis.z = RandUnit() * 2.0f - 1.0f;
        RwV3dNormalize(&group.m_RotationAxis, &group.m_RotationAxis);

        group.m_bStoppedMoving = false;
    }
}

// 0x59D7F0
void BreakObject_c::SetBreakInfo(BreakInfo_t* info, int32 bJustFaces) {
    static auto& ambientRed = StaticRef<float>(0x8D0A0C); // 32.0f
    static auto& ambientGreen = StaticRef<float>(0x8D0A08); // 32.0f
    static auto& ambientBlue = StaticRef<float>(0x8D0A04); // 32.0f

    // El original compara bJustFaces con 1 (no con distinto de cero) y reserva con operator new.
    const bool justFaces = bJustFaces == 1;
    m_NumBreakGroups = justFaces ? info->m_usNumTriangles : info->m_usNumMaterials;
    m_BreakGroups = static_cast<BreakGroup_t*>(operator new(m_NumBreakGroups * sizeof(BreakGroup_t)));

    for (auto i = 0; i < m_NumBreakGroups; ++i) {
        auto& group = m_BreakGroups[i];
        group.m_NumTriangles = 0;
        group.m_Texture = nullptr;

        auto numMaterials = 0u;
        if (justFaces)
            numMaterials = 1u;
        else {
            auto* pIndex = info->m_pTrianglesMaterialIndices;
            for (auto triInd = 0; triInd < info->m_usNumTriangles; ++triInd) {
                if (*pIndex == i)
                    ++numMaterials;

                ++pIndex;
            }
        }

        group.m_RenderInfo = static_cast<BreakGroupRenderInfo_t*>(operator new(numMaterials * sizeof(BreakGroupRenderInfo_t)));
        group.m_FramesToLive = 256 + CGeneral::GetRandomIntInRange(0, 32); // El original: 256 - (int)(rand * (1/32768) * -32)
    }

    for (auto i = 0; i < info->m_usNumTriangles; ++i) {
        auto curIndex = i;
        if (!justFaces)
            curIndex = info->m_pTrianglesMaterialIndices[curIndex];

        auto& triangle = info->m_pTriangles[i];
        auto& group = m_BreakGroups[curIndex];
        // BUG: (?) Compiler shows that invalid access can happen in next line, possibly something wrong with reversed code
        auto& curRenderInfo = group.m_RenderInfo[group.m_NumTriangles];
        for (auto ind = 0; ind < NUM_BREAK_GROUP_RENDER_INFO; ++ind) {
            curRenderInfo.positions[ind] = info->m_pVertexPos[triangle.vertIndex[ind]];
        }

        auto textureInd = justFaces ? info->m_pTrianglesMaterialIndices[i] : curIndex;
        group.m_Texture = info->m_pTextures[textureInd];

        if (!CPostEffects::IsVisionFXActive()) {
            ambientRed   = AmbientLightColourForFrame.red   * 255.0f;
            ambientGreen = AmbientLightColourForFrame.green * 255.0f;
            ambientBlue  = AmbientLightColourForFrame.blue  * 255.0f;
        }

        const auto& matColor = info->m_pMaterialProperties[textureInd];
        for (auto ind = 0; ind < NUM_BREAK_GROUP_RENDER_INFO; ++ind) {
            auto& vertColor = info->m_pVertexColors[triangle.vertIndex[ind]];
            auto red   = (uint8)std::min(255.0f, (float)vertColor.red   * matColor.red   + ambientRed);
            auto green = (uint8)std::min(255.0f, (float)vertColor.green * matColor.green + ambientGreen);
            auto blue  = (uint8)std::min(255.0f, (float)vertColor.blue  * matColor.blue  + ambientBlue);

            curRenderInfo.colors[ind].Set(red, green, blue);
            curRenderInfo.texCoords[ind] = info->m_pTexCoors[triangle.vertIndex[ind]];
        }

        ++group.m_NumTriangles;
    }

    for (auto& group : GetBreakGroups()) {
        if (group.m_Texture) {
            RwTextureAddRef(group.m_Texture);
        }
    }
}

// 0x59DE40
// Transcrito del original: unidad aleatoria rand * (1/32767), RwV3dNormalize/RwV3dLength y sumas en su orden.
void BreakObject_c::DoCollisionResponse(BreakGroup_t* group, float timeStep, const CVector* vecNormal, float groundZ) const {
    static auto& dotMultiplier = StaticRef<float>(0x8D0A18);   // 0.85f
    static auto& timestepScaling = StaticRef<float>(0x8D0A14); // 0.05f
    static auto& velocityScaling = StaticRef<float>(0x8D0A10); // 0.8f

    const auto RandUnit = [] { return (float)CGeneral::GetRandomNumber() * (1.0f / 32767.0f); };

    const auto& vel = group->m_Velocity;
    float d = (vel.x * vecNormal->x + vel.y * vecNormal->y + vel.z * vecNormal->z) * dotMultiplier;
    d = d + d;

    CVector newVelocity{ vel.x - d * vecNormal->x, vel.y - d * vecNormal->y, vel.z - d * vecNormal->z };

    CVector vecRand;
    vecRand.x = RandUnit() * 2.0f - 1.0f;
    vecRand.y = RandUnit() * 2.0f - 1.0f;
    vecRand.z = RandUnit() * 2.0f - 1.0f;
    RwV3dNormalize(&vecRand, &vecRand);

    const float fTimeScale = timestepScaling * timeStep;
    vecRand.x *= fTimeScale;
    vecRand.y *= fTimeScale;
    vecRand.z *= fTimeScale;

    const float fOriginalVelocityScale = RwV3dLength(&newVelocity);
    newVelocity.x = vecRand.x + newVelocity.x;
    newVelocity.y = vecRand.y + newVelocity.y;
    newVelocity.z = vecRand.z + newVelocity.z;
    RwV3dNormalize(&newVelocity, &newVelocity);
    newVelocity.x = fOriginalVelocityScale * newVelocity.x;
    newVelocity.y = fOriginalVelocityScale * newVelocity.y;
    newVelocity.z = fOriginalVelocityScale * newVelocity.z;

    group->m_RotationSpeed = 0.0f;
    group->m_Velocity.x = velocityScaling * newVelocity.x;
    group->m_Velocity.y = velocityScaling * newVelocity.y;
    group->m_Velocity.z = velocityScaling * newVelocity.z;
    RwMatrixGetPos(&group->m_Matrix)->z = groundZ + group->m_BoundingSize;

    if (fOriginalVelocityScale < 0.05f) {
        group->m_bStoppedMoving = true;
        if (m_JustFaces) {
            group->m_FramesToLive = 32 + CGeneral::GetRandomIntInRange(0, 32); // 32 - (int)(rand * (1/32768) * -32)
            return;
        }
    } else if (m_JustFaces) {
        return;
    }

    const FxPrtMult_c mult(1.0f, 1.0f, 1.0f, 0.1f, 0.3f, 0.0f, 0.15f);
    for (auto i = 0; i < 4; ++i) {
        const CVector& groupPos = *RwMatrixGetPos(&group->m_Matrix);
        CVector particlePos = groupPos;
        particlePos.x = (RandUnit() + particlePos.x) - 0.5f;
        particlePos.y = (RandUnit() + particlePos.y) - 0.5f;
        CVector particleVel;
        particleVel.x = RandUnit() * 0.3f - 0.15f;
        particleVel.y = RandUnit() * 0.3f - 0.15f;
        particleVel.z = 0.0f;
        g_fx.m_SmokeII3expand->AddParticle(particlePos, particleVel, 0.0f, mult);
    }

    if (m_AddSparks) {
        const CVector groupPos = *RwMatrixGetPos(&group->m_Matrix);
        const CVector particleVelocity = { 0.0f, 0.0f, 1.0f };
        const auto amount = static_cast<int32>(RwV3dLength(&group->m_Velocity) * 100.0f);
        g_fx.AddSparks(groupPos, particleVelocity, 2.0f, amount, CVector(0.0f, 0.0f, 0.0f), SPARK_PARTICLE_SPARK, 0.4f, 1.0f);
    }
}

// 0x59E1F0
void BreakObject_c::DoCollision(BreakGroup_t* group, float timeStep) {
    if (RwMatrixGetPos(&group->m_Matrix)->z - group->m_BoundingSize < m_GroundZ) {
        DoCollisionResponse(group, timeStep, &m_VecNormal, m_GroundZ);
    }
}

// acos del CRT del juego (0x822394): fpatan(sqrt((1 + x)(1 - x)), x) conservando la precisión del FPU
// (palabra de control (anterior & 0x300) | 0x7F, como 0x828F05).
static double __declspec(naked) __cdecl AcosX87(double x) {
    __asm {
        sub    esp, 4
        fnstcw word ptr [esp]
        mov    ax, word ptr [esp]
        and    ax, 300h
        or     ax, 7Fh
        mov    word ptr [esp + 2], ax
        fldcw  word ptr [esp + 2]
        fld    qword ptr [esp + 8]
        fld1
        fadd   st, st(1)
        fld1
        fsub   st, st(2)
        fmulp  st(1), st
        fsqrt
        fxch   st(1)
        fpatan
        fldcw  word ptr [esp]
        add    esp, 4
        ret
    }
}

// 0x59E220
void BreakObject_c::Update(float timeStep) {
    if (m_NumBreakGroups <= 0) {
        Exit();
        ++m_FramesActive;
        return;
    }

    auto bToBeRemoved = true;
    for (auto& group : GetBreakGroups()) {
        if (!group.m_bStoppedMoving) {
            group.m_Velocity.z -= timeStep * 0.008f; // El original multiplica por 0.008 (1/125)

            auto* pos = RwMatrixGetPos(&group.m_Matrix);
            pos->x = timeStep * group.m_Velocity.x + pos->x;
            pos->y = timeStep * group.m_Velocity.y + pos->y;
            pos->z = timeStep * group.m_Velocity.z + pos->z;

            if (m_FramesActive >= 5) {
                RwV3d* vecFacing = [&]{
                    switch (group.m_Type) {
                    case BreakGroupType::UP: return RwMatrixGetUp(&group.m_Matrix);
                    case BreakGroupType::AT: return RwMatrixGetAt(&group.m_Matrix);
                    case BreakGroupType::RIGHT:
                    default:                 return RwMatrixGetRight(&group.m_Matrix);
                    }
                }();

                // acos en doble precisión (CRT del original); el ángulo se guarda como float
                const float dot = m_VecNormal.x * vecFacing->x + m_VecNormal.y * vecFacing->y + m_VecNormal.z * vecFacing->z;
                const double angle = AcosX87((double)dot);
                const float fAngleRad = (float)angle;
                if (std::fabs(angle) > 0.01f) {
                    RwV3d axis{
                        vecFacing->y * m_VecNormal.z - vecFacing->z * m_VecNormal.y,
                        vecFacing->z * m_VecNormal.x - vecFacing->x * m_VecNormal.z,
                        vecFacing->x * m_VecNormal.y - vecFacing->y * m_VecNormal.x,
                    };
                    RwV3dNormalize(&axis, &axis);

                    const float fAngleDeg = fAngleRad * 57.295776f * timeStep * 0.05f;
                    const CVector savedPos = *pos;
                    RwMatrixRotate(&group.m_Matrix, &axis, fAngleDeg, RwOpCombineType::rwCOMBINEPOSTCONCAT);
                    *pos = savedPos;
                }
            } else {
                RwMatrixRotate(&group.m_Matrix, &group.m_RotationAxis, timeStep * group.m_RotationSpeed, RwOpCombineType::rwCOMBINEPRECONCAT);
            }

            DoCollision(&group, timeStep);
        }

        --group.m_FramesToLive;
        if (group.m_FramesToLive <= 0)
            group.m_FramesToLive = 0;
        else
            bToBeRemoved = false;
    }

    if (bToBeRemoved)
        Exit();

    ++m_FramesActive;
}

// 0x59E480
void BreakObject_c::Render(uint8 drawLastState) const {
    if (drawLastState != static_cast<uint8>(m_bDrawLast))
        return;
    if (m_NumBreakGroups <= 0) // Sin grupos el original sale sin llamar a RenderEnd
        return;

    RwRaster* lastRaster = nullptr;
    for (auto& group : GetBreakGroups()) {
        RwRaster* curRaster = nullptr;
        if (group.m_Texture)
            curRaster = RwTextureGetRaster(group.m_Texture);

        if (lastRaster != curRaster) {
            RenderEnd();
            lastRaster = curRaster;
            RenderBegin(curRaster, nullptr, RwIm3DTransformFlags::rwIM3D_VERTEXUV);
        }

        if (!curRaster && !lastRaster) {
            RenderEnd();
            lastRaster = nullptr;
            RenderBegin(nullptr, nullptr, RwIm3DTransformFlags::rwIM3D_VERTEXUV);
        }

        for (auto& renderInfo : group.GetRenderInfos()) {
            RwV3d vertices[NUM_BREAK_GROUP_RENDER_INFO];
            RwV3dTransformPoints(vertices, renderInfo.positions.data(), NUM_BREAK_GROUP_RENDER_INFO, &group.m_Matrix);

            int32 alpha = group.m_FramesToLive * (m_JustFaces ? 8 : 2);
            alpha = std::min(alpha, 255);

            auto colors{ renderInfo.colors };
            std::ranges::for_each(colors, [&](auto& color) { color.a = alpha; });

            RenderAddTri(
                vertices[0], vertices[1], vertices[2],
                renderInfo.texCoords[0], renderInfo.texCoords[1], renderInfo.texCoords[2],
                colors[0], colors[1], colors[2]
            );
        }
    }

    RenderEnd();
}
