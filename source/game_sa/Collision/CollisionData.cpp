/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCollisionData
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

#include "CollisionData.h"
#include "ColHelpers.h"

/**
 * Registra las operaciones de administración y consulta de los datos de colisión.
 */
void CCollisionData::InjectHooks() {
    RH_ScopedClass(CCollisionData);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(RemoveCollisionVolumes, 0x40F070);
    RH_ScopedInstall(RemoveTrianglePlanes, 0x40F6A0);
    RH_ScopedInstall(Copy, 0x40F120);
    RH_ScopedInstall(GetTrianglePoint, 0x40F5E0);
    RH_ScopedInstall(GetShadTrianglePoint, 0x40F640);
    RH_ScopedInstall(CalculateTrianglePlanes, 0x40F590);
    RH_ScopedInstall(SetLinkPtr, 0x40F6C0);
    RH_ScopedInstall(GetLinkPtr, 0x40F6E0);
}

// 0x40F030
/**
 * Inicializa una descripción de colisión vacía sin reservar volúmenes.
 *
 * Los distintos arrays se asignan posteriormente de forma independiente o como
 * vistas dentro de un bloque único creado por CColModel::AllocateData().
 */
CCollisionData::CCollisionData() {
    bUsesDisks = false;
    bHasFaceGroups = false;
    bHasShadowInfo = false;

    m_nNumSpheres = 0;
    m_nNumBoxes = 0;
    m_nNumTriangles = 0;
    m_nNumLines = 0;

    m_pSpheres = nullptr;
    m_pBoxes = nullptr;
    m_pLines = nullptr;
    m_pVertices = nullptr;
    m_pTriangles = nullptr;
    m_pTrianglePlanes = nullptr;

    m_nNumShadowTriangles = 0;
    m_nNumShadowVertices = 0;
    m_pShadowTriangles = nullptr;
    m_pShadowVertices = nullptr;
    /*
     * todo: initialize bHasFaceGroups, bHasShadow, m_pDisks as NOTSA or something
     * */
}

// 0x40F070
/**
 * Libera todos los arrays de geometría cuya propiedad corresponde a este objeto.
 *
 * También elimina el bloque auxiliar de planos de triángulo y deja los contadores
 * y punteros principales en estado vacío para impedir reutilizar memoria liberada.
 */
void CCollisionData::RemoveCollisionVolumes() {
    // Como el original: solo se llama a Free con punteros no nulos.
    const auto FreeIfSet = [](void* p) {
        if (p) {
            CMemoryMgr::Free(p);
        }
    };
    FreeIfSet(m_pSpheres);
    FreeIfSet(m_pLines);
    FreeIfSet(m_pBoxes);
    FreeIfSet(m_pVertices);
    FreeIfSet(m_pTriangles);
    FreeIfSet(m_pShadowTriangles);
    FreeIfSet(m_pShadowVertices);

    CCollision::RemoveTrianglePlanes(this);

    m_nNumSpheres = 0;
    m_nNumLines = 0;
    m_nNumBoxes = 0;
    m_nNumTriangles = 0;
    m_pSpheres = nullptr;
    m_pLines = nullptr;
    m_pBoxes = nullptr;
    m_pVertices = nullptr;
    m_pTriangles = nullptr;
    m_pShadowTriangles = nullptr;
    m_pShadowVertices = nullptr;
}

// 0x40F120
/**
 * Realiza una copia profunda de los volúmenes y mallas de @p src.
 *
 * Como el original: cada bloque se libera si el origen está vacío y se reserva
 * de nuevo sólo cuando cambia el contador; los vértices (principales y de sombra)
 * se reservan siempre que haya triángulos, con tantos elementos como el índice
 * máximo + 1. Los face groups no se copian.
 */
CCollisionData& CCollisionData::Copy(const CCollisionData& src) {
    const auto FreeIfSet = [](void* p) {
        if (p) {
            CMemoryMgr::Free(p);
        }
    };

    // ----- SPHERES ----- 0x40F125
    if (!src.m_nNumSpheres) {
        m_nNumSpheres = 0;
        FreeIfSet(m_pSpheres);
        m_pSpheres = nullptr;
    } else {
        if (m_nNumSpheres != src.m_nNumSpheres) {
            m_nNumSpheres = src.m_nNumSpheres;
            FreeIfSet(m_pSpheres);
            m_pSpheres = static_cast<CColSphere*>(CMemoryMgr::Malloc((int16)m_nNumSpheres * sizeof(CColSphere), 0));
        }
        for (uint32 i = 0; i < (uint32)(int16)m_nNumSpheres; ++i) {
            std::memcpy(&m_pSpheres[i], &src.m_pSpheres[i], sizeof(CColSphere)); // rep movs
        }
    }

    // ----- LINES / DISKS ----- 0x40F1A8
    if (!src.m_nNumLines) {
        m_nNumLines = 0;
        FreeIfSet(m_pLines);
        m_pLines = nullptr;
    } else if (src.bUsesDisks) {
        if (m_nNumLines != src.m_nNumLines || !bUsesDisks) {
            m_nNumLines = src.m_nNumLines;
            FreeIfSet(m_pLines);
            m_pDisks = static_cast<CColDisk*>(CMemoryMgr::Malloc((int8)m_nNumLines * sizeof(CColDisk), 0));
        }
        for (uint32 i = 0; i < (uint32)(int8)m_nNumLines; ++i) {
            std::memcpy(&m_pDisks[i], &src.m_pDisks[i], sizeof(CColDisk));
        }
    } else {
        if (m_nNumLines != src.m_nNumLines || bUsesDisks) {
            m_nNumLines = src.m_nNumLines;
            FreeIfSet(m_pLines);
            m_pLines = static_cast<CColLine*>(CMemoryMgr::Malloc((int8)m_nNumLines * sizeof(CColLine), 0));
        }
        for (uint32 i = 0; i < (uint32)(int8)m_nNumLines; ++i) {
            std::memcpy(&m_pLines[i], &src.m_pLines[i], sizeof(CColLine));
        }
    }

    // 0x40F297 - Como el original: se copian siempre los bits de discos y de sombras
    bUsesDisks     = src.bUsesDisks;
    bHasShadowInfo = src.bHasShadowInfo;

    // ----- BOXES ----- 0x40F2B4
    if (!src.m_nNumBoxes) {
        m_nNumBoxes = 0;
        FreeIfSet(m_pBoxes);
        m_pBoxes = nullptr;
    } else {
        if (m_nNumBoxes != src.m_nNumBoxes) {
            m_nNumBoxes = src.m_nNumBoxes;
            FreeIfSet(m_pBoxes);
            m_pBoxes = static_cast<CColBox*>(CMemoryMgr::Malloc((int16)m_nNumBoxes * sizeof(CColBox), 0));
        }
        for (uint32 i = 0; i < (uint32)(int16)m_nNumBoxes; ++i) {
            // Como el original: CColBox::operator= (0x40EEC0) copia 0x1B bytes (no el último byte de la superficie)
            std::memcpy(&m_pBoxes[i], &src.m_pBoxes[i], 0x1B);
        }
    }

    // Índice de vértice máximo (comparación sin signo de 16 bits) + 1, en 32 bits
    const auto GetNumVerts = [](const CColTriangle* tris, uint32 numTris) -> uint32 {
        uint32 highest = 0;
        for (uint32 i = 0; i < numTris; ++i) {
            for (const uint16 v : { tris[i].vA, tris[i].vB, tris[i].vC }) {
                if (v > (uint16)highest) {
                    highest = v;
                }
            }
        }
        return highest + 1;
    };

    // ----- TRIANGLES & VERTICES ----- 0x40F32C
    if (!src.m_nNumTriangles) {
        m_nNumTriangles = 0;
        FreeIfSet(m_pTriangles);
        m_pTriangles = nullptr;
        FreeIfSet(m_pVertices);
        m_pVertices = nullptr;
    } else {
        const auto numVerts = GetNumVerts(src.m_pTriangles, (uint32)(int16)src.m_nNumTriangles);
        FreeIfSet(m_pVertices);
        if ((uint16)numVerts) { // Como el original: con 0x10000 vértices no se reserva (y el puntero queda como estaba)
            m_pVertices = static_cast<CompressedVector*>(CMemoryMgr::Malloc((uint16)numVerts * sizeof(CompressedVector), 0));
            for (uint32 i = 0; i < (uint16)numVerts; ++i) {
                m_pVertices[i] = src.m_pVertices[i];
            }
        }

        if (m_nNumTriangles != src.m_nNumTriangles) {
            m_nNumTriangles = src.m_nNumTriangles;
            FreeIfSet(m_pTriangles);
            m_pTriangles = static_cast<CColTriangle*>(CMemoryMgr::Malloc((int16)m_nNumTriangles * sizeof(CColTriangle), 0));
        }
        for (uint32 i = 0; i < (uint32)(int16)m_nNumTriangles; ++i) {
            std::memcpy(&m_pTriangles[i], &src.m_pTriangles[i], sizeof(CColTriangle));
        }
    }

    // ----- SHADOW TRIANGLES & SHADOW VERTICES ----- 0x40F444
    if (src.m_nNumShadowTriangles && src.bHasShadowInfo) {
        const auto numVerts = GetNumVerts(src.m_pShadowTriangles, src.m_nNumShadowTriangles);
        FreeIfSet(m_pShadowVertices);
        if ((uint16)numVerts) {
            m_pShadowVertices = static_cast<CompressedVector*>(CMemoryMgr::Malloc((uint16)numVerts * sizeof(CompressedVector), 0));
            for (uint32 i = 0; i < (uint16)numVerts; ++i) {
                m_pShadowVertices[i] = src.m_pShadowVertices[i];
            }
        }

        if (m_nNumShadowTriangles != src.m_nNumShadowTriangles) {
            m_nNumShadowTriangles = src.m_nNumShadowTriangles;
            FreeIfSet(m_pShadowTriangles);
            m_pShadowTriangles = static_cast<CColTriangle*>(CMemoryMgr::Malloc(m_nNumShadowTriangles * sizeof(CColTriangle), 0));
        }
        for (uint32 i = 0; i < m_nNumShadowTriangles; ++i) {
            std::memcpy(&m_pShadowTriangles[i], &src.m_pShadowTriangles[i], sizeof(CColTriangle));
        }
        m_nNumShadowVertices = src.m_nNumShadowVertices;
    } else { // 0x40F543
        m_nNumShadowTriangles = 0;
        m_nNumShadowVertices = 0;
        FreeIfSet(m_pShadowTriangles);
        m_pShadowTriangles = nullptr;
        FreeIfSet(m_pShadowVertices);
        m_pShadowVertices = nullptr;
    }

    return *this;
}

// Disposición: | CColTrianglePlane[] | CLink<CCollisionData*>* alineado a 4 bytes |
// 0x40F590
/**
 * Precalcula un plano por triángulo para acelerar posteriores pruebas de colisión.
 *
 * Se reserva una entrada adicional porque el espacio inmediatamente posterior al
 * array también se reutiliza para almacenar el puntero de enlace de la caché.
 */
void CCollisionData::CalculateTrianglePlanes() {
    // Como el original: el contador se lee con signo (movsx).
    m_pTrianglePlanes = static_cast<CColTrianglePlane*>(CMemoryMgr::Malloc(((int16)m_nNumTriangles + 1) * sizeof(CColTrianglePlane), 0));
    for (int32 i = 0; i < (int16)m_nNumTriangles; ++i) {
        m_pTrianglePlanes[i].Set(m_pVertices, m_pTriangles[i]);
    }
}

// 0x40F6A0
// Libera exclusivamente la caché de planos; la malla de triángulos permanece intacta.
void CCollisionData::RemoveTrianglePlanes() {
    if (m_pTrianglePlanes) { // Como el original: Free solo con puntero no nulo
        CMemoryMgr::Free(m_pTrianglePlanes);
    }
    m_pTrianglePlanes = nullptr;
}

// 0x40F5E0
// Expande un vértice comprimido de la malla principal al vector de salida.
void CCollisionData::GetTrianglePoint(CVector& outVec, int32 vertId) {
    outVec = m_pVertices[vertId];
}

// 0x40F640
// Expande un vértice comprimido perteneciente a la malla de sombras.
void CCollisionData::GetShadTrianglePoint(CVector& outVec, int32 vertId) {
    outVec = m_pShadowVertices[vertId];
}

// 0x40F6C0
/**
 * Guarda el enlace de caché en el espacio alineado situado después de los planos.
 *
 * Esta disposición evita añadir otro campo a CCollisionData y reproduce el layout
 * esperado por el juego para los planos calculados dinámicamente.
 */
void CCollisionData::SetLinkPtr(CLink<CCollisionData*>* link) {
    // Original calculation method:
    // const auto dwLinkAddress = (reinterpret_cast<uint32>(&m_pTrianglePlanes[m_nNumTriangles]) + 3) & 0xFFFFFFFC; // 4 bytes aligned address

    auto* linkPtr = static_cast<void*>(&m_pTrianglePlanes[m_nNumTriangles]);
    auto space = sizeof(CColTrianglePlane);
    auto* alignedAddress = std::align(4, sizeof(CLink<CCollisionData*>*), linkPtr, space); // 4 bytes aligned address
    *static_cast<CLink<CCollisionData*>**>(alignedAddress) = link;
}

// 0x40F6E0
// Recupera el enlace almacenado tras el array de planos y comprueba que apunte a este objeto.
CLink<CCollisionData*>* CCollisionData::GetLinkPtr() {
    // Original calculation method:
    // const auto dwLinkAddress = (reinterpret_cast<uint32>(&m_pTrianglePlanes[m_nNumTriangles]) + 3) & 0xFFFFFFFC; // 4 bytes aligned address

    auto* linkPtr = static_cast<void*>(&m_pTrianglePlanes[m_nNumTriangles]);
    auto space = sizeof(CColTrianglePlane);
    auto* alignedAddress = std::align(4, sizeof(CLink<CCollisionData*>*), linkPtr, space); // 4 bytes aligned address
    const auto l = *static_cast<CLink<CCollisionData*>**>(alignedAddress);
    assert(l->data == this); // Sanity check
    return l;
}

// Los face groups se encuentran físicamente antes de m_pTriangles en ciertos
// modelos cargados como bloque único; no existe un puntero dedicado en la estructura.
auto CCollisionData::GetNumFaceGroups() const -> uint32 {
    if (bHasFaceGroups) {
        assert(m_pTriangles);
        return *reinterpret_cast<uint32*>(reinterpret_cast<byte*>(m_pTriangles) - sizeof(uint32)); // See `CCollisionData` header for explanation :)
    }
    return 0;
}

// Reconstruye una vista de los face groups mediante aritmética sobre el layout cargado.
auto CCollisionData::GetFaceGroups() const -> std::span<ColHelpers::TFaceGroup> {
    using namespace ColHelpers;
    if (const auto numfg = GetNumFaceGroups()) {
        assert(numfg);
        return std::span{ // See `CCollisionData` header for explanation
            reinterpret_cast<TFaceGroup*>(reinterpret_cast<uint8*>(m_pTriangles) - sizeof(uint32) - sizeof(TFaceGroup) * numfg),
            numfg
        };
    }
    return {};
}

// Expande los tres índices de un triángulo a coordenadas de vértice.
auto CCollisionData::GetTriVertices(const CColTriangle& tri) const->std::array<CVector, 3> {
    std::array<CVector, 3> verts;
    for (auto&& [i, j] : rngv::enumerate(tri.m_vertIndices)) {
        verts[i] = m_pVertices[j];
    }
    return verts;
}

// NOTSA
// Reserva un array independiente de líneas y actualiza su contador.
void CCollisionData::AllocateLines(uint32 num) {
    m_nNumLines = num;
    m_pLines = static_cast<CColLine*>(CMemoryMgr::Malloc(sizeof(CColLine) * num));
}

// NOTSA
// Copia en el almacenamiento ya reservado exactamente m_nNumSpheres entradas.
void CCollisionData::SetSpheres(const CColSphere* spheres) {
    for (auto i = 0; i < m_nNumSpheres; ++i) {
        m_pSpheres[i] = spheres[i];
    }
}
