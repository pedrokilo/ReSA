/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CColModel
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

#include "ColModel.h"

//#define COL_EXTRA_DEBUG

#ifdef COL_DEBUG
#define DEV_LOG_COL(...) NOTSA_LOG_DEBUG(__VA_ARGS__)
#else
#define DEV_LOG_COL(...)
#endif

/**
 * Registra las operaciones de ciclo de vida y almacenamiento de CColModel.
 */
void CColModel::InjectHooks() {
    RH_ScopedClass(CColModel);
    RH_ScopedCategory("Collision");

    RH_ScopedInstall(operator new, 0x40FC30);
    RH_ScopedInstall(operator delete, 0x40FC40);
    RH_ScopedInstall(operator=, 0x40F7C0);
    RH_ScopedInstall(MakeMultipleAlloc, 0x40F740);
    RH_ScopedOverloadedInstall(AllocateData, "void", 0x40F810, void(CColModel::*)());
    RH_ScopedOverloadedInstall(AllocateData, "params", 0x40F870, void(CColModel::*)(int32, int32, int32, int32, int32, bool));
    RH_ScopedInstall(RemoveCollisionVolumes, 0x40F9E0);
    RH_ScopedInstall(CalculateTrianglePlanes, 0x40FA30);
    RH_ScopedInstall(RemoveTrianglePlanes, 0x40FA40);
}

/**
 * Crea un modelo de colisión activo pero todavía sin CCollisionData asociado.
 */
CColModel::CColModel() : m_boundBox() {
    m_nColSlot              = 0;
    m_pColData              = nullptr;
    m_bHasCollisionVolumes  = false;
    m_bIsSingleColDataAlloc = false;
    m_IsActive             = true;
}

/**
 * Libera la geometría sólo cuando la instancia participa del ciclo de vida normal.
 * m_IsActive permite representar modelos que no deben apropiarse/liberar sus datos.
 */
CColModel::~CColModel() {
    // Como el original (0x40F700): libera como RemoveCollisionVolumes pero sin anular m_pColData.
    if (!m_IsActive || !m_pColData) {
        return;
    }
    if (m_bIsSingleColDataAlloc) {
        CCollision::RemoveTrianglePlanes(m_pColData);
        CMemoryMgr::Free(m_pColData);
    } else {
        m_pColData->RemoveCollisionVolumes();
        delete m_pColData;
    }
}

// 0x40F7C0
/**
 * Copia los límites y reutiliza el almacenamiento de colisión ya presente en destino.
 *
 * Esta operación no asigna m_pColData cuando el destino carece de él; conserva el
 * comportamiento reconstruido existente y deja esa situación marcada por el break
 * de depuración que ya forma parte del código.
 */
CColModel& CColModel::operator=(const CColModel& colModel) {
    assert(&colModel != this); // BUG(Prone) No self assignment check

    m_boundSphere = colModel.m_boundSphere;
    m_boundBox    = colModel.m_boundBox;

    // Como el original: sin m_pColData en destino no se copia nada (ni se detiene).
    if (m_pColData) {
        m_pColData->Copy(*colModel.m_pColData);
    }

    return *this;
}

/**
 * Convierte un modelo almacenado en un único bloque en asignaciones independientes.
 *
 * Se crea un CCollisionData nuevo, se realiza una copia profunda y finalmente se
 * libera el bloque contiguo anterior. Tras ello cada array puede gestionarse por separado.
 */
void CColModel::MakeMultipleAlloc() {
    if (!m_bIsSingleColDataAlloc)
        return;

    const auto colData = new CCollisionData();
    assert(colData);

    colData->Copy(*m_pColData);
    CMemoryMgr::Free(m_pColData); // Como el original: el bloque único se libera con CMemoryMgr::Free.

    m_bIsSingleColDataAlloc = false;
    m_pColData = colData;
}

// Crea sólo la cabecera CCollisionData; sus arrays se asignarán posteriormente.
void CColModel::AllocateData() {
    m_bIsSingleColDataAlloc = false;
    m_pColData = new CCollisionData();
    assert(m_pColData);
}

// Disposición de m_pColData: | CCollisionData | CColSphere[] | CColLine[]/CColDisk[] | CColBox[] | Vertices[] | CColTriangle[] |
/**
 * Reserva en un único bloque la cabecera y todos los arrays solicitados.
 *
 * Los punteros internos se convierten en vistas sobre offsets del mismo bloque;
 * los triángulos se alinean a 4 bytes. m_bIsSingleColDataAlloc registra este modo
 * para que RemoveCollisionVolumes() use una única liberación.
 */
void CColModel::AllocateData(int32 numSpheres, int32 numBoxes, int32 numLines, int32 numVertices, int32 numTriangles, bool bUsesDisks) {
    const auto baseSize = sizeof(CCollisionData);
    const auto spheresSize = numSpheres * sizeof(CColSphere);
    const auto linesOrDisksSize = bUsesDisks ? (numLines * sizeof(CColDisk)) : (numLines * sizeof(CColLine));
    const auto vertsSize = numVertices * sizeof(CompressedVector);
    const auto boxesSize = numBoxes * sizeof(CColBox);
    const auto trianglesSize = numTriangles * sizeof(CColTriangle);

    const auto spheresOffset = baseSize;
    const auto linesOrDisksOffset = spheresOffset + spheresSize;
    const auto boxesOffset = linesOrDisksOffset + linesOrDisksSize;
    const auto vertsOffset = boxesOffset + boxesSize;

    // ORIGINAL ALIGNMENT LOGIC, with possible bug
    // BUG?: Seems like it could clip the last 3 bytes of vertices array
    //  const auto trianglesOffset = (vertsOffset + vertsSize) & 0xFFFFFFFC; // Align the offset to 4 bytes boundary

    auto* pTrisStart = reinterpret_cast<void*>(vertsOffset + vertsSize);
    auto space = trianglesSize + 4;
    auto* pAlignedAddress = std::align(4, trianglesSize, pTrisStart, space); // 4 bytes aligned address
    const auto trianglesOffset = reinterpret_cast<uint32>(pAlignedAddress);
    assert(trianglesOffset && trianglesOffset >= (vertsOffset + vertsSize)); // Just to make sure that the alignment works properly

    AllocateData(trianglesOffset + trianglesSize);
    m_pColData->m_nNumSpheres = numSpheres;
    m_pColData->m_nNumLines = numLines;
    m_pColData->m_nNumBoxes = numBoxes;
    m_pColData->m_nNumTriangles = numTriangles;

    m_pColData->m_pSpheres = m_pColData->GetPointerToColArray<CColSphere>(spheresOffset);
    if (bUsesDisks)
        m_pColData->m_pDisks = numLines ? m_pColData->GetPointerToColArray<CColDisk>(linesOrDisksOffset) : nullptr;
    else
        m_pColData->m_pLines = numLines ? m_pColData->GetPointerToColArray<CColLine>(linesOrDisksOffset) : nullptr;

    m_pColData->m_pBoxes = numBoxes ? m_pColData->GetPointerToColArray<CColBox>(boxesOffset) : nullptr;
    m_pColData->m_pVertices = numVertices ? m_pColData->GetPointerToColArray<CompressedVector>(vertsOffset) : nullptr;
    m_pColData->m_pTriangles = numTriangles ? m_pColData->GetPointerToColArray<CColTriangle>(trianglesOffset) : nullptr;
    m_pColData->m_pTrianglePlanes = nullptr;

    m_pColData->bUsesDisks = bUsesDisks;
}

// Reserva el bloque contiguo bruto y marca el modelo como single-allocation.
void CColModel::AllocateData(int32 size) {
    DEV_LOG_COL("AllocateData[Size]: {} [ColSlot: {}; Size: {}]", LOG_PTR(this), m_nColSlot, size);

    m_bIsSingleColDataAlloc = true;
    m_pColData = static_cast<CCollisionData*>(CMemoryMgr::Malloc(size, 0));
    assert(m_pColData);
}

/**
 * Libera m_pColData según el modo en que fue creado.
 *
 * En single-allocation la geometría comparte el mismo bloque; en modo múltiple
 * CCollisionData es responsable de liberar primero cada uno de sus arrays.
 */
void CColModel::RemoveCollisionVolumes() {
    if (!m_pColData) {
        return;
    }

    DEV_LOG_COL("Removing: {} [ColSlot: {}]", LOG_PTR(this), m_nColSlot);

    if (m_bIsSingleColDataAlloc) {
        CCollision::RemoveTrianglePlanes(m_pColData);
        CMemoryMgr::Free(m_pColData);
    } else {
        m_pColData->RemoveCollisionVolumes();
        delete m_pColData;
    }

    m_pColData = nullptr;
}

// Construye la caché derivada sólo cuando existe información de colisión.
void CColModel::CalculateTrianglePlanes() {
    if (m_pColData)
        m_pColData->CalculateTrianglePlanes();
}

// Descarta la caché de planos sin eliminar el resto de la geometría.
void CColModel::RemoveTrianglePlanes() {
    if (m_pColData)
        m_pColData->RemoveTrianglePlanes();
}

// Los CColModel dinámicos se obtienen del pool dedicado, no del heap general.
void* CColModel::operator new(unsigned size) {
    return GetColModelPool()->New();
}

// Devuelve la entrada a CColModelPool conservando el modelo de memoria del juego.
void CColModel::operator delete(void* data) {
    GetColModelPool()->Delete(static_cast<CColModel*>(data));
}
