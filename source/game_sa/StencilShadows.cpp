/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CStencilShadows
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "StencilShadowObject.h"

#include "World.h"
#include "Pools/Pools.h"
#include "Pools/VehiclePool.h"
#include "Entity/Vehicle/Bike.h"

static inline auto& s_ShadowTrianglePointsUnk         = StaticRef<RxVertexIndex*>(0xC6A170);
static inline auto& s_ShadowTrianglePoints            = StaticRef<CVector*>(0xC6A174);
static inline auto& s_TransformedShadowTrianglePoints = StaticRef<CVector*>(0xC6A178);
static inline auto& s_SunPosNrm                       = StaticRef<CVector>(0x8D5244); // CVector(1.0, 1.0, -2.0)

// Como el original: los dos contadores de Process son globales del EXE (0xC6A899 / 0xC6A898), no estaticos locales
static inline auto& s_RegisterShadowCounter = StaticRef<uint8>(0xC6A899);
static inline auto& s_RenderForObjCounter   = StaticRef<uint8>(0xC6A898);

namespace {
// Como el original: nodo crudo de las listas de punteros (item +0, siguiente +4); se leen sin validar contra los pools
struct RawListNode {
    CEntity*     item;
    RawListNode* next;
};

// Como el original: distancia maxima de registro/descarte de una sombra (0x8598B0)
constexpr float kShadowMaxDistSq = 2500.0f;

// Como el original: multiplicacion matriz * punto (0x59C890) con su orden de sumas; incluye la traslacion
CVector StencilMulPoint(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z,
    };
}

// Como el original: 0x5334F0 (CEntity::TransformFromObjectSpace con salida). Con matriz usa 0x59C890 (suma en el orden
// up, forward, right, posicion; no es CMatrix::TransformPoint) y sin ella 0x54ECE0 (fcos/fsin x87 completos)
CVector StencilTransformFromObjectSpace(const CEntity* entity, const CVector& offset) {
    if (entity->m_matrix) {
        return StencilMulPoint(*entity->m_matrix, offset);
    }
    CVector result;
    plugin::Call<0x54ECE0, CVector*, const CSimpleTransform*, const CVector*>(&result, &entity->m_placement, &offset);
    return result;
}

// Como el original: 0x711160 / 0x7111F0 (identicas), con la posicion de la camara en ESI y la entidad en EBX.
// Distancia con signo al cuadrado entre la camara y la superficie de la esfera envolvente: negativa si la camara esta dentro
float StencilSignedDistSq(const CVector& cam, CEntity* entity) {
    const CColModel* colModel = entity->GetColModel();
    const CVector    centre   = StencilTransformFromObjectSpace(entity, colModel->m_boundSphere.m_vecCenter);
    const float      radius   = colModel->m_boundSphere.m_fRadius;
    const float      dx       = cam.x - centre.x;
    const float      dy       = cam.y - centre.y;
    const float      dz       = cam.z - centre.z;
    const float      dist     = std::sqrt((dz * dz + dy * dy) + dx * dx);
    const float      sign     = dist < radius ? -1.0f : 1.0f;
    const float      d        = dist - radius;
    return (sign * d) * d;
}

// Como el original: 0x710BA0, busca el objeto de sombra activo de una entidad
CStencilShadowObject* StencilFindForOwner(const CEntity* owner) {
    for (auto* obj = CStencilShadows::pFirstActiveStencilShadowObject; obj; obj = obj->m_pNext) {
        if (obj->m_pOwner == owner) {
            return obj;
        }
    }
    return nullptr;
}

// Como el original: un objeto que ya no es visible, explotado o roto deja de proyectar sombra
bool StencilObjectInvalid(CEntity* owner) {
    if (!owner->m_bIsVisible) {
        return true;
    }
    auto* object = static_cast<CObject*>(owner);
    return object->objectFlags.bIsExploded || object->objectFlags.bIsBroken;
}

// Como el original: 0x70FA70, anade la arista (i0, i1) o la quita si ya estaba (en cualquier sentido); registros ECX/EDX/EDI + 1 pila
void StencilToggleEdge(RxVertexIndex* edges, uint16& count, uint16 i0, uint16 i1) {
    const uint16 n = count;
    for (uint16 k = 0; k < count; ++k) {
        const uint16 a = edges[2 * k];
        const uint16 b = edges[2 * k + 1];
        if ((a == i0 && b == i1) || (a == i1 && b == i0)) {
            if (count > 1) {
                edges[2 * k]     = edges[2 * (n - 1)];
                edges[2 * k + 1] = edges[2 * count - 1];
            }
            --count;
            return;
        }
    }
    edges[2 * n]       = i0;
    edges[2 * count + 1] = i1;
    ++count;
}

// Como el original: 0x54EEF0 sin enganchar (copia la matriz a una RwMatrix estatica y transforma `count` puntos)
void StencilTransformPoints(CVector* out, int32 count, const CMatrix* matrix, const CVector* in) {
    plugin::Call<0x54EEF0, CVector*, int32, const CMatrix*, const CVector*>(out, count, matrix, in);
}

// Como el original: RenderForVehicle (0x70FAE0) y RenderForObject (0x710310) son la misma construccion del volumen
// de sombra (caras que miran a la luz, caras extruidas y aristas de silueta); solo cambian la matriz (la de
// inclinacion en las motos) y la longitud de extrusion (40/5 en aviones-helicopteros/resto, 60 en objetos).
bool BuildShadowVolume(CStencilShadowObject* obj, bool forVehicle) {
    CEntity* const entity = obj->m_pOwner;

    float extrude = 60.0f;
    if (forVehicle) {
        extrude = 5.0f;
        if (entity->GetType() == ENTITY_TYPE_VEHICLE) {
            const auto subType = static_cast<CVehicle*>(entity)->m_nVehicleSubType;
            if (subType == VEHICLE_TYPE_PLANE || subType == VEHICLE_TYPE_HELI) {
                extrude = 40.0f;
            }
        }
    }

    CCollisionData* const colData = entity->GetColModel()->m_pColData;
    if (!colData || (int32)colData->m_nNumShadowTriangles != (int32)obj->m_NumShadowFaces) {
        return false;
    }

    if (!entity->m_matrix) {
        entity->AllocateMatrix();
        entity->m_placement.UpdateMatrix(entity->m_matrix);
    }
    CMatrix* matrix = entity->m_matrix;
    if (forVehicle && entity->GetType() == ENTITY_TYPE_VEHICLE && static_cast<CVehicle*>(entity)->m_nVehicleType == VEHICLE_TYPE_BIKE) {
        auto* bike = static_cast<CBike*>(static_cast<CVehicle*>(entity));
        bike->CalculateLeanMatrix();
        matrix = &bike->m_mLeanMatrix;
    }

    CMatrix invMatrix;
    Invert(*matrix, invMatrix);

    // Como el original: la direccion de la luz en el espacio del objeto = T(sol) - T(origen) con la inversa
    const CVector sun     = s_SunPosNrm;
    const CVector origin  = StencilMulPoint(invMatrix, CVector{ 0.0f, 0.0f, 0.0f });
    const CVector sunObj  = StencilMulPoint(invMatrix, sun);
    const float   lightZ  = sunObj.z - origin.z;
    const float   lightY  = sunObj.y - origin.y;
    const float   lightX  = sunObj.x - origin.x;

    RxVertexIndex* const edges        = s_ShadowTrianglePointsUnk;
    CVector* const       objPoints    = s_ShadowTrianglePoints;
    CVector* const       worldPoints  = s_TransformedShadowTrianglePoints;

    obj->m_SizeOfShadowFacesData = 0;
    const int32 numFaces         = (int32)(int16)obj->m_NumShadowFaces;
    const int32 numVerts         = (int32)colData->m_nNumShadowVertices;
    if (numVerts > 0) {
        for (int32 j = 0; j < numVerts; ++j) {
            colData->GetShadTrianglePoint(objPoints[j], j);
        }
    }
    StencilTransformPoints(worldPoints, numVerts, matrix, objPoints);

    uint16 edgeCount = 0;

    // Como el original: en el desbordamiento del buffer (usado + n >= capacidad) se deja de añadir caras y pasa a las aristas
    const auto Used     = [&]() -> int32 { return (int32)obj->m_SizeOfShadowFacesData; };
    const auto Capacity = [&]() -> int32 { return (int32)obj->m_FaceID; };

    for (int32 i = 0; i < numFaces; ++i) {
        const auto* tri = reinterpret_cast<const uint16*>(reinterpret_cast<const uint8*>(colData->m_pShadowTriangles) + i * 8);
        const uint16 idxA = tri[0];
        const uint16 idxB = tri[1];
        const uint16 idxC = tri[2];

        const CVector p0 = objPoints[idxA];
        const CVector p1 = objPoints[idxB];
        const CVector p2 = objPoints[idxC];
        const CVector w0 = worldPoints[idxA];
        const CVector w1 = worldPoints[idxB];
        const CVector w2 = worldPoints[idxC];

        // Como el original: normal = e1 x e2 con el orden de productos del x87, y luz . normal = (Lz*Nz + Ny*Ly) + Nx*Lx
        const float e1z = p1.z - p0.z;
        const float e1y = p1.y - p0.y;
        const float e1x = p1.x - p0.x;
        const float e2z = p2.z - p0.z;
        const float e2y = p2.y - p0.y;
        const float e2x = p2.x - p0.x;
        const float nx  = (e2z * e1y) - (e2y * e1z);
        const float ny  = (e2x * e1z) - (e2z * e1x);
        const float nz  = (e2y * e1x) - (e2x * e1y);
        const float dot = (lightZ * nz + ny * lightY) + nx * lightX;

        if (dot >= 0.0f) {
            // Cara que mira a la luz: se guardan sus aristas y su triangulo
            StencilToggleEdge(edges, edgeCount, idxA, idxB);
            StencilToggleEdge(edges, edgeCount, idxB, idxC);
            StencilToggleEdge(edges, edgeCount, idxC, idxA);
            if (Used() + 3 >= Capacity()) {
                break;
            }
            CVector* dst = obj->m_ShadowFacesData + Used();
            dst[0]       = w0;
            dst[1]       = w2;
            dst[2]       = w1;
        } else {
            // Cara que no mira a la luz: su copia extruida
            const float ez = sun.z * extrude;
            const float ey = sun.y * extrude;
            const float ex = sun.x * extrude;
            const CVector e0{ ex + w0.x, w0.y + ey, w0.z + ez };
            const CVector e1{ w1.x + ex, w1.y + ey, w1.z + ez };
            const CVector e2{ w2.x + ex, w2.y + ey, w2.z + ez };
            if (Used() + 3 >= Capacity()) {
                break;
            }
            CVector* dst = obj->m_ShadowFacesData + Used();
            dst[0]       = e0;
            dst[1]       = e2;
            dst[2]       = e1;
        }
        obj->m_SizeOfShadowFacesData = (uint32)(Used() + 3);
    }

    // Como el original: cada arista de silueta se extruye en dos triangulos (seis vertices)
    if (edgeCount > 0) {
        const float ez = sun.z * extrude;
        const float ey = sun.y * extrude;
        const float ex = sun.x * extrude;
        for (uint16 k = 0; k < edgeCount; ++k) {
            const CVector wa = worldPoints[edges[2 * k]];
            const CVector wb = worldPoints[edges[2 * k + 1]];
            const CVector wae{ ex + wa.x, wa.y + ey, ez + wa.z };
            const CVector wbe{ wb.x + ex, wb.y + ey, wb.z + ez };
            if (Used() + 6 >= Capacity()) {
                break;
            }
            CVector* dst = obj->m_ShadowFacesData + Used();
            dst[0]       = wa;
            dst[1]       = wb;
            dst[2]       = wae;
            dst[3]       = wb;
            dst[4]       = wbe;
            dst[5]       = wae;
            obj->m_SizeOfShadowFacesData = (uint32)(Used() + 6);
        }
    }
    return true;
}
} // namespace

void CStencilShadows::InjectHooks() {
    RH_ScopedClass(CStencilShadows);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x70F9E0);
    RH_ScopedInstall(Shutdown, 0x711390);
    RH_ScopedInstall(Process, 0x711D90);
    RH_ScopedInstall(GraphicsHighQuality, 0x70F9B0);
    RH_ScopedInstall(UpdateHierarchy, 0x710BC0);
    RH_ScopedInstall(RegisterStencilShadows, 0x711760);
    RH_ScopedInstall(RenderStencilShadows, 0x7113B0);
    RH_ScopedInstall(RenderForVehicle, 0x70FAE0);
    RH_ScopedInstall(RenderForObject, 0x710310);
    RH_ScopedInstall(Render, 0x710D50);
    RH_ScopedInstall(RenderBuffer, 0x710B50);
    RH_ScopedInstall(SunSetPositionFromEntity, 0x710AF0);
    RH_ScopedInstall(sub_710CC0, 0x710CC0);
}

// 0x70F9E0
bool CStencilShadows::Init() {
    ZoneScoped;

    RwD3D9SetStencilClear(0);
    pFirstAvailableStencilShadowObject = m_StencilShadowObjects.data();
    pFirstActiveStencilShadowObject = nullptr;

    for (auto&& [i, obj] : rngv::enumerate(m_StencilShadowObjects)) {
        obj.m_pOwner                = nullptr;
        obj.m_NumShadowFaces        = 0;
        obj.m_Type                  = eStencilShadowObjType::NONE;
        obj.m_SizeOfShadowFacesData = 0;
        obj.m_FaceID                = 0;
        obj.m_ShadowFacesData       = 0;

        obj.m_pPrev = i ? &m_StencilShadowObjects[i - 1] : nullptr;
        obj.m_pNext = (i != m_StencilShadowObjects.size() - 1) ? &m_StencilShadowObjects[i + 1] : nullptr;
    }
    return true; // Como el original: devuelve 1 en AL
}

// 0x711390
void CStencilShadows::Shutdown() {
    for (auto* obj = pFirstActiveStencilShadowObject; obj;) {
        auto* next = obj->m_pNext;
        obj->Destroy();
        obj = next;
    }
}

// 0x710D50
bool CStencilShadows::Render(const CRGBA& color) {
    uiTempBufferIndicesStored  = 0;
    uiTempBufferVerticesStored = 0;

    // Como el original: se lee el color una vez y se reordena a ARGB (A<<24 | R<<16 | G<<8 | B)
    const uint32 intColor = (uint32(color.a) << 24) | (uint32(color.r) << 16) | (uint32(color.g) << 8) | uint32(color.b);

    for (auto* shadow = pFirstActiveStencilShadowObject; shadow;) {
        auto* const next = shadow->m_pNext;
        CVector* data    = shadow->m_ShadowFacesData;
        const int32 used = (int32)shadow->m_SizeOfShadowFacesData;

        // Como el original: grupos de seis vectores = dos triangulos
        const int32 groups = used / 6;
        for (int32 g = 0; g < groups; ++g, data += 6) {
            sub_710CC0(6, 6);
            const int32 base       = uiTempBufferIndicesStored;
            const int32 vertexBase = uiTempBufferVerticesStored;
            for (int32 k = 0; k < 6; ++k) {
                aTempBufferIndices[base + k] = (RxVertexIndex)(base + k);
            }
            for (int32 k = 0; k < 6; ++k) {
                auto& vertex           = TempBufferVertices.m_3d[vertexBase + k];
                vertex.objVertex.x     = data[k].x;
                vertex.objVertex.y     = data[k].y;
                vertex.objVertex.z     = data[k].z;
                vertex.color           = intColor;
            }
            uiTempBufferIndicesStored  = base + 6;
            uiTempBufferVerticesStored = vertexBase + 6;
        }

        // Como el original: si sobra un triangulo (usados / 3 impar) se emite aparte
        if (((used / 3) & 1) != 0) {
            const CVector* tri = data;
            sub_710CC0(3, 3);
            const int32 base       = uiTempBufferIndicesStored;
            const int32 vertexBase = uiTempBufferVerticesStored;
            aTempBufferIndices[base + 0] = (RxVertexIndex)(base + 0);
            aTempBufferIndices[base + 1] = (RxVertexIndex)(base + 1);
            aTempBufferIndices[base + 2] = (RxVertexIndex)(base + 2);
            for (int32 k = 0; k < 3; ++k) {
                auto& vertex           = TempBufferVertices.m_3d[vertexBase + k];
                vertex.objVertex.x     = tri[k].x;
                vertex.objVertex.y     = tri[k].y;
                vertex.objVertex.z     = tri[k].z;
                vertex.color           = intColor;
            }
            uiTempBufferIndicesStored  = base + 3;
            uiTempBufferVerticesStored = vertexBase + 3;
        }
        shadow = next;
    }

    if (uiTempBufferIndicesStored && uiTempBufferVerticesStored) {
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nullptr);
        LittleTest();
        if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA)) {
            RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, uiTempBufferIndicesStored);
            RwIm3DEnd();
        }
        uiTempBufferVerticesStored = uiTempBufferIndicesStored = 0;
    }
    return true;
}

// unused
// 0x710AF0
bool CStencilShadows::SunSetPositionFromEntity(const CEntity* entity) {
    if (!entity) {
        return false;
    }
    // Como el original: el EXE lee un puntero a una matriz en +4 del parametro y normaliza su posicion (+0x30)
    const auto* matrix = *reinterpret_cast<const CMatrix* const*>(reinterpret_cast<const uint8*>(entity) + 4);
    CVector     pos    = matrix->GetPosition();
    pos.Normalise();
    s_SunPosNrm = pos;
    return true;
}

// 0x710B50
bool CStencilShadows::RenderBuffer(const CVector& pos) {
    CVector normalised = pos;
    normalised.Normalise();
    s_SunPosNrm = normalised;
    return true;
}

// 0x710CC0
void CStencilShadows::sub_710CC0(int32 indices, int32 vertices) {
    if (uiTempBufferIndicesStored + indices < TOTAL_TEMP_BUFFER_INDICES
        && uiTempBufferVerticesStored + vertices < TOTAL_TEMP_BUFFER_3DVERTICES) {
        return;
    }

    if (!uiTempBufferIndicesStored || !uiTempBufferVerticesStored) {
        return;
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, nullptr);
    LittleTest();
    if (RwIm3DTransform(TempBufferVertices.m_3d, uiTempBufferVerticesStored, nullptr, rwIM3D_VERTEXXYZ | rwIM3D_VERTEXRGBA)) {
        RwIm3DRenderIndexedPrimitive(rwPRIMTYPETRILIST, aTempBufferIndices, uiTempBufferIndicesStored);
        RwIm3DEnd();
    }
    uiTempBufferVerticesStored = uiTempBufferIndicesStored = 0;
}

// 0x7113B0
bool CStencilShadows::RenderStencilShadows() {
    ZoneScoped;

    if (!GraphicsHighQuality()) {
        return false;
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,             RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESTENCILENABLE,            RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,                RWRSTATE(rwSHADEMODEFLAT));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,                RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,            RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,        RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,                 RWRSTATE(rwBLENDZERO));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,                RWRSTATE(rwBLENDONE));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTIONMASK,      RWRSTATE(uint32(-1)));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTIONWRITEMASK, RWRSTATE(uint32(-1)));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTION,          RWRSTATE(rwSTENCILFUNCTIONALWAYS));
    RwRenderStateSet(rwRENDERSTATESTENCILFAIL,              RWRSTATE(rwSTENCILOPERATIONKEEP));
    RwRenderStateSet(rwRENDERSTATESTENCILZFAIL,             RWRSTATE(rwSTENCILOPERATIONKEEP));
    RwRenderStateSet(rwRENDERSTATESTENCILPASS,              RWRSTATE(rwSTENCILOPERATIONKEEP));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTIONREF,       RWRSTATE(0));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,              RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESTENCILZFAIL,             RWRSTATE(rwSTENCILOPERATIONINCR));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(rwCULLMODECULLFRONT));

    Render(CRGBA{ 0, 0, 0, 255 });

    RwRenderStateSet(rwRENDERSTATESTENCILZFAIL,             RWRSTATE(rwSTENCILOPERATIONDECR));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(rwCULLMODECULLBACK));

    Render(CRGBA{ 0, 0, 0, 255 });

    // WTF is up with these states?
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,        RWRSTATE(FALSE)); // same state
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,             RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,              RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESTENCILENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,                RWRSTATE(rwSHADEMODEGOURAUD));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(rwCULLMODECULLBACK));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,             RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,              RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESTENCILENABLE,            RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,                RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,        RWRSTATE(TRUE)); // same state
    RwRenderStateSet(rwRENDERSTATESRCBLEND,                 RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,                RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,                RWRSTATE(rwSHADEMODEFLAT));
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,            RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTIONREF,       RWRSTATE(1u));
    RwRenderStateSet(rwRENDERSTATESTENCILFUNCTION,          RWRSTATE(rwSTENCILFUNCTIONLESSEQUAL));
    RwRenderStateSet(rwRENDERSTATESTENCILFAIL,              RWRSTATE(rwSTENCILOPERATIONKEEP));
    RwRenderStateSet(rwRENDERSTATESTENCILZFAIL,             RWRSTATE(rwSTENCILOPERATIONKEEP));
    RwRenderStateSet(rwRENDERSTATESTENCILPASS,              RWRSTATE(rwSTENCILOPERATIONKEEP));

    CSprite2d::InitPerFrame();
    // Como el original: alfa = (fuerza de sombra (dword) * 50) >> 8 (desplazamiento aritmetico), solo el byte bajo
    const int32 strength = *reinterpret_cast<const int32*>(&CTimeCycle::m_CurrentColours.m_nShadowStrength);
    const auto  alpha    = (uint8)((int32)((uint32)strength * 50u) >> 8);
    CSprite2d::DrawRect(
        CRect{ 0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT },
        CRGBA{ 0, 0, 0, alpha }
    );

    RwRenderStateSet(rwRENDERSTATESTENCILENABLE,            RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,             RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,              RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,        RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE,                RWRSTATE(rwSHADEMODEGOURAUD));
    RwRenderStateSet(rwRENDERSTATECULLMODE,                 RWRSTATE(rwCULLMODECULLBACK));
    return true;
}

// 0x70FAE0
bool CStencilShadows::RenderForVehicle(CStencilShadowObject* object) {
    return BuildShadowVolume(object, true);
}

// 0x710310
bool CStencilShadows::RenderForObject(CStencilShadowObject* object) {
    return BuildShadowVolume(object, false);
}

// 0x711D90
bool CStencilShadows::Process(CVector& cameraPos) {
    ZoneScoped;

    if (!GraphicsHighQuality()) {
        return false;
    }

    // Como el original: cada 8 fotogramas se permite crear sombras nuevas (el argumento es "no crear")
    ++s_RegisterShadowCounter;
    RegisterStencilShadows(cameraPos, (s_RegisterShadowCounter & 7) != 0);

    // why do we even do this?
    s_ShadowTrianglePointsUnk         = (RxVertexIndex*)CMemoryMgr::Malloc(12'288 * sizeof(RxVertexIndex));
    s_ShadowTrianglePoints            = (CVector*)CMemoryMgr::Malloc(2'048 * sizeof(CVector));
    s_TransformedShadowTrianglePoints = (CVector*)CMemoryMgr::Malloc(2'048 * sizeof(CVector));

    auto i{ 0 };
    for (auto* obj = pFirstActiveStencilShadowObject; obj;) {
        auto* const next = obj->m_pNext;
        switch (obj->m_Type) {
        case eStencilShadowObjType::OBJECT:
            if ((i & 3) == s_RenderForObjCounter) {
                RenderForObject(obj);
            }
            ++i;
            break;
        case eStencilShadowObjType::VEHICLE:
            RenderForVehicle(obj);
            break;
        default:
            break;
        }
        obj = next;
    }
    s_RenderForObjCounter = (s_RenderForObjCounter + 1) & 3;

    CMemoryMgr::Free(std::exchange(s_ShadowTrianglePointsUnk, nullptr));
    CMemoryMgr::Free(std::exchange(s_ShadowTrianglePoints, nullptr));
    CMemoryMgr::Free(std::exchange(s_TransformedShadowTrianglePoints, nullptr));
    return true;
}

// 0x70F9B0
bool CStencilShadows::GraphicsHighQuality() {
    return ::GraphicsHighQuality();
}

// 0x710BC0
void CStencilShadows::UpdateHierarchy(CStencilShadowObject*& firstAvailable, CStencilShadowObject*& firstActive, CStencilShadowObject* newOne) {
    if (auto* prev = newOne->m_pPrev) {
        auto* next = newOne->m_pNext;
        if (next) {
            next->m_pPrev = prev;
            newOne->m_pPrev->m_pNext = newOne->m_pNext;
        } else {
            prev->m_pNext = nullptr;
        }
    } else {
        auto* next     = newOne->m_pNext;
        firstAvailable = next;
        if (next) {
            next->m_pPrev = nullptr;
        }
    }
    newOne->m_pNext = firstActive;
    newOne->m_pPrev = nullptr;
    firstActive     = newOne;

    if (newOne->m_pNext) {
        newOne->m_pNext->m_pPrev = newOne;
    }
}

// 0x711760
bool CStencilShadows::RegisterStencilShadows(CVector& cameraPos, bool doNotCreateNew) {
    if (doNotCreateNew) {
        // Como el original: solo se descartan las sombras cuyo propietario ya no es valido
        for (auto* obj = pFirstActiveStencilShadowObject; obj;) {
            auto* const next  = obj->m_pNext;
            auto* const owner = obj->m_pOwner;
            if (!owner || (owner->GetType() == ENTITY_TYPE_OBJECT && StencilObjectInvalid(owner))) {
                obj->Destroy();
            }
            obj = next;
        }
        return true;
    }

    // Como el original: primero se descartan las sombras no validas, demasiado lejanas o de otra zona
    for (auto* obj = pFirstActiveStencilShadowObject; obj;) {
        auto* const next  = obj->m_pNext;
        auto* const owner = obj->m_pOwner;
        bool destroy      = false;
        if (!owner) {
            destroy = true;
        } else if (owner->GetType() == ENTITY_TYPE_OBJECT && StencilObjectInvalid(owner)) {
            destroy = true;
        } else {
            switch (obj->m_Type) {
            case eStencilShadowObjType::OBJECT:
                if (StencilSignedDistSq(cameraPos, owner) > kShadowMaxDistSq) {
                    destroy = true;
                } else if (!owner->IsInCurrentArea()) {
                    destroy = true;
                }
                break;
            case eStencilShadowObjType::VEHICLE:
                if (StencilSignedDistSq(cameraPos, owner) > kShadowMaxDistSq) {
                    destroy = true;
                }
                break;
            default:
                break;
            }
        }
        if (destroy) {
            obj->Destroy();
        }
        obj = next;
    }

    if (!pFirstAvailableStencilShadowObject) {
        return true;
    }

    // Como el original: vehiculos del pool con sombra sin objeto de sombra
    auto* const vehiclePool = GetVehiclePool();
    const auto  poolSize    = static_cast<int32>(vehiclePool->GetSize());
    for (int32 i = 0; i < poolSize; ++i) {
        if (vehiclePool->IsFreeSlotAtIndex(i)) {
            continue;
        }
        CVehicle* const vehicle = vehiclePool->GetAt(i);
        if (!vehicle) {
            continue;
        }
        CColModel* const colModel = vehicle->GetColModel();
        if (!colModel) {
            continue;
        }
        CCollisionData* const colData = colModel->m_pColData;
        if (!colData || !colData->bHasShadowInfo) {
            continue;
        }
        if (!vehicle->m_pCollisionList.m_node) {
            continue;
        }
        if (StencilFindForOwner(vehicle)) {
            continue;
        }
        if (!(StencilSignedDistSq(cameraPos, vehicle) <= kShadowMaxDistSq)) {
            continue;
        }
        if (!pFirstAvailableStencilShadowObject) {
            return false;
        }
        pFirstAvailableStencilShadowObject->Create(vehicle, eStencilShadowObjType::VEHICLE);
        if (!pFirstAvailableStencilShadowObject) {
            return false;
        }
    }

    if (!pFirstAvailableStencilShadowObject) {
        return true;
    }

    // Como el original: sectores del mundo alrededor de la camara (buildings y objetos de los sectores repetidos)
    constexpr float kRadius = 50.0f; // 0x858B40
    const float xmin = cameraPos.x - kRadius;
    const float ymin = cameraPos.y - kRadius;
    const float xmax = cameraPos.x + kRadius;
    const float ymax = cameraPos.y + kRadius;
    const auto  Sector = [](float v) { return (int32)std::floor(v * 0.02f + 60.0f); }; // x * (1/50) + 60, no x / 50

    int32 startX = Sector(xmin);
    startX       = startX > 0 ? startX : 0;
    int32 startY = Sector(ymin);
    startY       = startY > 0 ? startY : 0;
    int32 endX   = Sector(xmax);
    endX         = endX < 0x77 ? endX : 0x77;
    int32 endY   = Sector(ymax);
    endY         = endY < 0x77 ? endY : 0x77;

    CWorld::AdvanceCurrentScanCode();
    if (startY > endY) {
        return true;
    }

    // Como el original: la misma comprobacion para las entidades de las dos listas
    const auto TryRegister = [&](CEntity* ent) -> int32 { // 0 = seguir, 1 = fin con falso
        if (ent->m_bIsProcObject) {
            return 0;
        }
        if (ent->GetType() == ENTITY_TYPE_OBJECT && StencilObjectInvalid(ent)) {
            return 0;
        }
        CColModel* const colModel = ent->GetColModel();
        if (!colModel) {
            return 0;
        }
        CCollisionData* const colData = colModel->m_pColData;
        if (!colData || !colData->bHasShadowInfo) {
            return 0;
        }
        if (ent->GetScanCode() == CWorld::ms_nCurrentScanCode) {
            return 0;
        }
        ent->SetScanCode(CWorld::ms_nCurrentScanCode);
        if (!ent->IsInCurrentArea()) {
            return 0;
        }
        if (CModelInfo::GetModelInfo((int32)(int16)ent->m_nModelIndex)->GetModelType() != MODEL_INFO_ATOMIC) {
            return 0;
        }
        if (StencilFindForOwner(ent)) {
            return 0;
        }
        if (!(StencilSignedDistSq(cameraPos, ent) <= kShadowMaxDistSq)) {
            return 0;
        }
        if (!pFirstAvailableStencilShadowObject) {
            return 1;
        }
        pFirstAvailableStencilShadowObject->Create(ent, eStencilShadowObjType::OBJECT);
        if (!pFirstAvailableStencilShadowObject) {
            return 1;
        }
        return 0;
    };

    for (int32 y = startY; y <= endY; ++y) {
        for (int32 x = startX; x <= endX; ++x) {
            auto& sector = CWorld::GetSector(x, y);
            for (auto* node = *reinterpret_cast<RawListNode* const*>(&sector.Buildings); node;) {
                auto* const ent = node->item;
                node            = node->next;
                if (TryRegister(ent)) {
                    return false;
                }
            }
            auto& repeatSector = CWorld::GetRepeatSector(x, y);
            for (auto* node = *reinterpret_cast<RawListNode* const*>(&repeatSector.Objects); node;) {
                auto* const ent = node->item;
                node            = node->next;
                if (TryRegister(ent)) {
                    return false;
                }
            }
        }
    }
    return true;
}
