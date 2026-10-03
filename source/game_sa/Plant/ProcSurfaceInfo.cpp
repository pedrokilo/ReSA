/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: ProcSurfaceInfo_c
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ProcSurfaceInfo.h"
#include "ProcObjectMan.h"
#include "PlantLocTri.h"

void ProcSurfaceInfo_c::InjectHooks() {
    RH_ScopedClass(ProcSurfaceInfo_c);
    RH_ScopedCategory("Plant");

    RH_ScopedInstall(Init, 0x5A2EB0);
    RH_ScopedInstall(Exit, 0x5A3270);
    RH_ScopedInstall(AddObject, 0x5A32D0);
    RH_ScopedInstall(AddObjects, 0x5A3850);
}

// 0x5A2EB0
// Como el original: devuelve false si no existe el modelo y true al terminar.
bool ProcSurfaceInfo_c::Init(
    const char* surfaceType,
    const char* modelName,
    float       spacing,
    float       minDist,
    int32       minRot,
    int32       maxRot,
    float       minScale,
    float       maxScale,
    float       minScaleZ,
    float       maxScaleZ,
    float       zOffsetMin,
    float       zOffsetMax,
    bool        align,
    uint8       useGrid
) {
    m_SurfaceId = g_surfaceInfos.GetSurfaceIdFromName(surfaceType);
    auto mi     = CModelInfo::GetModelInfo(modelName, &m_ModelIndex); // todo: int16 cast here, why?
    if (!mi) {
        return false;
    }

    // Como el original: la distancia mínima se limita a 80 como máximo (con NaN se conserva).
    if (minDist > 80.0f) {
        minDist = 80.0f;
    }

    m_fMinScale             = minScale;
    m_fMaxScale             = maxScale;
    m_fSpacing              = spacing;
    m_fMinScaleZ            = minScaleZ;
    m_fMaxScaleZ            = maxScaleZ;
    m_fOffsetMinZ           = zOffsetMin;
    m_fOffsetMaxZ           = zOffsetMax;
    m_Align                 = align;
    m_fSquaredSpacingRadius = 1.0f / (spacing * spacing);
    m_fSquaredMinDistance   = minDist * minDist;
    m_fMinRotInRadians      = DegreesToRadians(float(minRot));
    m_fMaxRotInRadians      = DegreesToRadians(float(maxRot));
    m_fUseGrid              = float(useGrid);
    return true;
}

// 0x5A3270
void ProcSurfaceInfo_c::Exit() {
    for (auto it = m_Objects.GetHead(); it;) {
        auto* next = m_Objects.GetNext(it);

        m_Objects.RemoveItem(it);
        g_procObjMan.m_ObjectsList.AddItem(it);
        it->m_Obj->DeleteRwObject();
        CWorld::Remove(it->m_Obj);
        delete std::exchange(it->m_Obj, nullptr);

        it = next;
    }
}

// Como el original: rand() del juego escalado por 1/32767 e interpolado como min + (max - min) * r.
static float ProcSurfaceRandomInRange(float min, float max) {
    const float r = static_cast<float>(CGeneral::GetRandomNumber()) * RAND_MAX_FLOAT_RECIPROCAL;
    return (max - min) * r + min;
}

// Producto en el x87 como el EXE (conserva los denormales que SSE vacía a cero).
static void ProcSurfaceMulX87(float& v, float k) {
    float* pv = &v;
    __asm {
        mov  eax, pv
        fld  dword ptr [eax]
        fmul k
        fstp dword ptr [eax]
    }
}

// 0x5A32D0
ProcObjectListItem* ProcSurfaceInfo_c::AddObject(CVector pos, CVector normal, tColLighting lighting) {
    //Original function has some exception handling built in, this is ignored in reversed code
    auto head = g_procObjMan.m_ObjectsList.RemoveHead();
    if (!head || m_Align && normal.z < 0.95f && g_procObjMan.m_numAllocatedMatrices >= 200) {
        return nullptr;
    }

    auto modelInfo  = CModelInfo::GetModelInfo(m_ModelIndex);
    bool isBuilding = modelInfo->m_nObjectInfoIndex == -1;
    if (isBuilding) {
        GetBuildingPool()->SetDealWithNoMemory(true);
        head->m_Obj                           = new CBuilding();
        GetBuildingPool()->SetDealWithNoMemory(false);
        if (!head->m_Obj) {
            return nullptr;
        }

        head->m_Obj->SetModelIndexNoCreate(m_ModelIndex);
    } else {
        if (CObject::nNoTempObjects >= 150) {
            return nullptr;
        }

        GetObjectPool()->SetDealWithNoMemory(true);
        head->m_Obj                         = new CObject(m_ModelIndex, false);
        GetObjectPool()->SetDealWithNoMemory(false);
        if (!head->m_Obj) {
            return nullptr;
        }

        head->m_Obj->AsObject()->m_nObjectType = eObjectType::OBJECT_TYPE_DECORATION;
        head->m_Obj->AsObject()->SetVelocity({ 0.f, 0.f, 0.f });
        ++CObject::nNoTempObjects;
    }

    if (g_surfaceInfos.IsWater(m_SurfaceId)) {
        head->m_Obj->m_bUnderwater = true;
    }
    head->m_Obj->m_bIsProcObject = true;

    const auto xyScale     = ProcSurfaceRandomInRange(m_fMinScale, m_fMaxScale);
    const auto zScale      = ProcSurfaceRandomInRange(m_fMinScaleZ, m_fMaxScaleZ);
    const auto rotation    = ProcSurfaceRandomInRange(m_fMinRotInRadians, m_fMaxRotInRadians);
    const auto zOffset     = ProcSurfaceRandomInRange(m_fOffsetMinZ, m_fOffsetMaxZ);
    const auto minBounding = modelInfo->GetColModel()->GetBoundingBox().m_vecMin;

    if (m_Align && normal.z < 0.95f) {
        CMatrix tempMatrix{};

        // Como el original: fcos/fsin del FPU (precisión completa), redondeados a float al guardar.
        float rotCosValue, rotSinValue;
        __asm {
            fld   rotation
            fcos
            fstp  rotCosValue
            fld   rotation
            fsin
            fstp  rotSinValue
        }
        CVector rotVec = { rotCosValue, rotSinValue, 0.f };

        // Como el original: CrossProduct (0x59C730) y Normalise (0x59C910) en el sitio.
        CVector forwardVec;
        CrossProduct(&forwardVec, &normal, &rotVec);
        forwardVec.Normalise();
        CVector rightVec;
        CrossProduct(&rightVec, &forwardVec, &normal);
        rightVec.Normalise();
        const auto zMult = zOffset - minBounding.z;

        tempMatrix.GetRight()    = rightVec;
        tempMatrix.GetForward()  = forwardVec;
        tempMatrix.GetUp()       = normal;
        tempMatrix.GetPosition() = pos + (normal * zMult);

        head->m_Obj->AllocateStaticMatrix();
        head->m_Obj->SetMatrix(tempMatrix);
        head->m_bAllocatedMatrix = true;
        ++g_procObjMan.m_numAllocatedMatrices;
    } else {
        // Como el original: (z - min.z) + desplazamiento.
        pos.z = pos.z - minBounding.z + zOffset;
        head->m_Obj->SetPosn(pos);
        head->m_Obj->SetOrientation(0.f, 0.f, rotation);
        head->m_bAllocatedMatrix = false;
    }

    head->m_Obj->SetAreaCode(static_cast<eAreaCodes>(CGame::currArea));
    head->m_Obj->SetIsStatic(true);
    head->m_Obj->CreateRwObject();
    head->m_Obj->UpdateRwMatrix();
    head->m_Obj->UpdateRwFrame();
    CWorld::Add(head->m_Obj);
    m_Objects.AddItem(head);
    // Como el original: el escalado solo se aplica a los edificios, cuando cualquiera de las dos escalas
    // es distinta de 1.
    if (isBuilding && (xyScale != 1.0f || zScale != 1.0f)) {
        auto* rwObject = head->m_Obj->GetRwObject();
        if (rwObject && rwObjectGetParent(rwObject) != (void*)-16) { // I don't understand this if, there must be some missing macro
            auto* modellingMatrix = head->m_Obj->GetModellingMatrix();

            auto tempMatrix = CMatrix(modellingMatrix, false);
            // Como el original (0x5A2E60): cada fila se multiplica componente a componente por (xy, xy, z).
            // Los productos se hacen en el x87 como el EXE: conserva los denormales que SSE vacía a cero.
            for (CVector* row : { &tempMatrix.GetRight(), &tempMatrix.GetForward(), &tempMatrix.GetUp() }) {
                ProcSurfaceMulX87(row->x, xyScale);
                ProcSurfaceMulX87(row->y, xyScale);
                ProcSurfaceMulX87(row->z, zScale);
            }
            // Como el original (0x5A3803): pos.z - (min.z * zScale - min.z), todo en el x87
            {
                float* pz = &tempMatrix.GetPosition().z;
                const float minZ = minBounding.z;
                __asm {
                    fld  minZ
                    fmul zScale
                    fsub minZ
                    mov  eax, pz
                    fld  dword ptr [eax]
                    fsub st, st(1)
                    fstp dword ptr [eax]
                    fstp st(0)
                }
            }
            tempMatrix.UpdateRW();

            head->m_Obj->UpdateRwFrame();
        }
    }
    return head;
}

// 0x5A3850
int32 ProcSurfaceInfo_c::AddObjects(CPlantLocTri* plant) {
    const CVector& v1 = plant->m_V1;
    const CVector& v2 = plant->m_V2;
    const CVector& v3 = plant->m_V3;

    // Como el original: (v1 + v2 + v3) * 0.3333 (no / 3).
    const CVector centerPos{
        (v1.x + v2.x + v3.x) * 0.3333f,
        (v2.y + v1.y + v3.y) * 0.3333f,
        (v2.z + v1.z + v3.z) * 0.3333f,
    };
    const auto& camPos = TheCamera.GetPosition();
    {
        // Como el original: distancia al cuadrado sumada en el orden z, y, x.
        const float dx = camPos.x - centerPos.x;
        const float dy = camPos.y - centerPos.y;
        const float dz = camPos.z - centerPos.z;
        if (dz * dz + dy * dy + dx * dx < m_fSquaredMinDistance) {
            return 0;
        }
    }

    auto vec1   = v2 - v1;
    auto vec2   = v3 - v2;
    auto vec3   = v3 - v1;
    CVector normal;
    CrossProduct(&normal, &vec3, &vec1);

    auto numAdded = 0;
    if (m_fUseGrid == 0.f) {
        // Como el original: |normal| sumado en el orden z, y, x.
        auto density = std::sqrt(normal.z * normal.z + normal.y * normal.y + normal.x * normal.x) * 0.5f * m_fSquaredSpacingRadius;

        // Como el original: la semilla suma cada vértice como (z + y) + x, después v3 + v2 + v1 y el modelo,
        // y se trunca con _ftol2 (64 bits).
        const float randSeed = (v3.z + v3.y + v3.x) + (v2.z + v2.y + v2.x) + (v1.z + v1.y + v1.x) + static_cast<float>(m_ModelIndex);
        const auto  seed        = static_cast<uint32>(static_cast<int64>(randSeed));
        const auto  oldRandSeed = rand(); // R* usage of srand and rand seems rather weird.
        srand(seed);

        if (density > 0.0f) {
            do {
                float cutOff = 1.f;
                if (density < 1.f) {
                    cutOff = static_cast<float>(CGeneral::GetRandomNumber()) * RAND_MAX_FLOAT_RECIPROCAL;
                }

                if (cutOff < density) {
                    const float offset1 = static_cast<float>(CGeneral::GetRandomNumber()) * RAND_MAX_FLOAT_RECIPROCAL;
                    const float offset2 = static_cast<float>(CGeneral::GetRandomNumber()) * RAND_MAX_FLOAT_RECIPROCAL * offset1;

                    //FIX_BUGS candidate probably, seems like the result of this calculation can fall outside of the triangle that it's based on
                    const CVector objPos{
                        vec1.x * offset1 + v1.x + vec2.x * offset2,
                        vec1.y * offset1 + v1.y + vec2.y * offset2,
                        vec1.z * offset1 + v1.z + vec2.z * offset2,
                    };
                    // Como el original: la normal se vuelve a normalizar en cada objeto.
                    normal.Normalise();
                    auto addedObj = AddObject(objPos, normal, plant->m_nLighting);
                    if (addedObj) {
                        numAdded++;
                        addedObj->m_LocTri = plant;
                    }
                }
                density -= 1.f;
            } while (density > 0.f);
        }
        srand(oldRandSeed);
        return numAdded;
    }

    // Como el original: mínimos y máximos con las comparaciones del EXE.
    float minX = v1.x < v2.x ? v1.x : v2.x;
    if (!(minX < v3.x)) {
        minX = v3.x;
    }
    float minY = v1.y < v2.y ? v1.y : v2.y;
    if (!(minY < v3.y)) {
        minY = v3.y;
    }
    float maxX = v1.x > v2.x ? v1.x : v2.x;
    if (!(maxX > v3.x)) {
        maxX = v3.x;
    }
    float maxY = v1.y > v2.y ? v1.y : v2.y;
    if (!(maxY > v3.y)) {
        maxY = v3.y;
    }

    // A mess, but required one, could do that with some floorf and a single cast per line, but then the results would differ from original one for negative numbers
    const auto step = 1.f / m_fSpacing;
    const auto xStartI = static_cast<int32>(static_cast<int64>(static_cast<float>(static_cast<int32>(static_cast<int64>(minX * step))) * m_fSpacing));
    const auto xEndI   = static_cast<int32>(static_cast<int64>(static_cast<float>(static_cast<int32>(static_cast<int64>(maxX * step)) + 1) * m_fSpacing));
    const auto yStartI = static_cast<int32>(static_cast<int64>(static_cast<float>(static_cast<int32>(static_cast<int64>(minY * step))) * m_fSpacing));
    const auto yEndI   = static_cast<int32>(static_cast<int64>(static_cast<float>(static_cast<int32>(static_cast<int64>(maxY * step)) + 1) * m_fSpacing));
    normal.Normalise();

    float       xCur   = static_cast<float>(xStartI);
    const float xEnd   = static_cast<float>(xEndI);
    if (!(xCur < xEnd)) {
        return numAdded;
    }
    const float yStart = static_cast<float>(yStartI);
    const float yEnd   = static_cast<float>(yEndI);

    do {
        // Como el original: cada columna vuelve a empezar en la primera fila.
        for (float yCur = yStart; yCur < yEnd; yCur += m_fSpacing) {
            float zPos;
            if (!IsPtInTriangle2D(xCur, yCur, v1, v2, v3, normal, &zPos)) {
                continue;
            }

            auto addedObj = AddObject({ xCur, yCur, zPos }, normal, plant->m_nLighting);
            if (addedObj) {
                numAdded++;
                addedObj->m_LocTri = plant;
            }
        }
        xCur += m_fSpacing;
    } while (xCur < xEnd);

    return numAdded;
}

// 0x5A2F80, Fucked up calling convention, can't hook
// Como el original: cada prueba rechaza el punto si la expresión no es <= 0 (también con NaN).
bool ProcSurfaceInfo_c::IsPtInTriangle2D(float x, float y, CVector v1, CVector v2, CVector v3, CVector normal, float* z) {
    if (!((v1.x - v2.x) * (v1.y - y) + (v2.y - v1.y) * (v1.x - x) <= 0.0f)
        || !((v2.x - v3.x) * (v2.y - y) + (v3.y - v2.y) * (v2.x - x) <= 0.0f)
        || !((v1.y - v3.y) * (v3.x - x) + (v3.x - v1.x) * (v3.y - y) <= 0.0f)) {
        return false;
    }

    *z = (-(normal.x * x) - normal.y * y - -(v1.z * normal.z + normal.y * v1.y + normal.x * v1.x)) / normal.z;
    return true;
}
