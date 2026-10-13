/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlantLocTri
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PlantLocTri.h"
#include "PlantMgr.h"
#include "PlantSurfPropMgr.h"
#include "ProcObjectMan.h"
#include "X87Math.h"

// 0x5DC210
// Usa el área real del triángulo como multiplicador espacial de las densidades
// configuradas para su superficie.
float GetPlantDensity(CPlantLocTri* plant) {
    // The magnitude of the cross product of 2 vectors give the area of a paralellogram they span.
    // Como el original: suma z*z + y*y + x*x y multiplica por 0.5
    const auto c = (plant->m_V2 - plant->m_V1).Cross(plant->m_V3 - plant->m_V1);
    return std::sqrt(c.z * c.z + c.y * c.y + c.x * c.x) * 0.5f;
}

// Como el original: 0x5DC210 recibe el triángulo en EAX y devuelve el resultado en ST0. EntryEaxCpp debe ser lo primero que se ejecuta.
#pragma runtime_checks("", off)
__declspec(safebuffers) static float GetPlantDensity_EaxEntry() {
    return GetPlantDensity(reinterpret_cast<CPlantLocTri*>(EntryEaxCpp()));
}
#pragma runtime_checks("", restore)

/**
 * Registra creación y liberación de triángulos capaces de producir plantas u objetos.
 */
void CPlantLocTri::InjectHooks() {
    RH_ScopedClass(CPlantLocTri);
    RH_ScopedCategory("Plant");

    RH_ScopedInstall(Add, 0x5DC290);
    RH_ScopedInstall(Release, 0x5DB6D0);

    RH_ScopedNamedGlobalInstall(GetPlantDensity_EaxEntry, "GetPlantDensity", 0x5DC210); // Como el original: argumento en EAX
}

// 0x5DC290
/**
 * Inicializa geometría, superficie y reglas de generación para un triángulo.
 *
 * Cuando genera plantas calcula máximos por especie a partir del área y las
 * propiedades de superficie; después mueve el nodo desde la lista libre al slot
 * de proximidad correspondiente. Los triángulos sólo de objetos usan el slot 3.
 */
CPlantLocTri* CPlantLocTri::Add(const CVector& p1, const CVector& p2, const CVector& p3, uint8 surface, tColLighting lightning, bool createsPlants, bool createsObjects) {
    m_V1 = p1;
    m_V2 = p2;
    m_V3 = p3;

    m_nLighting = lightning;
    m_SurfaceId = surface;
    m_createsObjects = createsObjects;
    m_createsPlants = createsPlants;
    m_createdObjects = false;

    // Como el original: (p2 + p1) + p3 multiplicado por 0.33333334f
    m_Center = (p2 + p1 + p3) * 0.33333334f;

    // Como el original: distancia con la suma z*z + y*y + x*x
    const auto d = m_Center - m_V1;
    m_SphereRadius = std::sqrt(d.z * d.z + d.y * d.y + d.x * d.x) * 1.75f;
    if (m_createsObjects && !m_createsPlants) {
        CPlantMgr::MoveLocTriToList(CPlantMgr::m_UnusedLocTriListHead, CPlantMgr::m_CloseLocTriListHead[3], this);
        return this;
    }

    auto properties = CPlantSurfPropMgr::GetSurfProperties(surface);
    auto densityMult = GetPlantDensity(this);
    // Como el original: _ftol2 (truncado de 64 bits) y se queda la palabra baja
    auto density1 = (uint16)(int64)(densityMult * properties->m_Plants[0].density);
    auto density2 = (uint16)(int64)(densityMult * properties->m_Plants[1].density);
    auto density3 = (uint16)(int64)(densityMult * properties->m_Plants[2].density);
    if (density1 + density2 + density3 > 0) {
        rng::generate(m_Seed, [] { return CGeneral::GetRandomNumberInRange(0.0f, 1.0f); });
        m_nMaxNumPlants[0] = density1;
        m_nMaxNumPlants[1] = density2;
        m_nMaxNumPlants[2] = density3;
        CPlantMgr::MoveLocTriToList(CPlantMgr::m_UnusedLocTriListHead, CPlantMgr::m_CloseLocTriListHead[properties->m_SlotId], this);
        return this;
    }

    if (m_createsObjects) {
        // m_nFlags = m_nFlags & 0xFE;
        m_createsPlants = false;

        CPlantMgr::MoveLocTriToList(CPlantMgr::m_UnusedLocTriListHead, CPlantMgr::m_CloseLocTriListHead[3], this);
        return this;
    }

    return nullptr;
}

// 0x5DB6D0
/**
 * Retira objetos procedurales si llegaron a crearse, desenlaza el triángulo de su
 * lista activa y lo devuelve a la lista global de libres.
 */
void CPlantLocTri::Release() {
    rng::fill(m_nMaxNumPlants, 0u);
    if (m_createdObjects)
        g_procObjMan.ProcessTriangleRemoved(this);

    if (!m_createsObjects || m_createsPlants) {
        auto head = &CPlantMgr::m_CloseLocTriListHead[CPlantSurfPropMgr::GetSurfProperties(m_SurfaceId)->m_SlotId];
        if (auto prev = m_PrevTri) {
            if (auto next = m_NextTri) {
                next->m_PrevTri = prev;
                prev->m_NextTri = next;
            } else {
                prev->m_NextTri = nullptr;
            }
        } else {
            auto next = m_NextTri;
            *head = next;
            if (next)
                next->m_PrevTri = nullptr;
        }
        m_NextTri = CPlantMgr::m_UnusedLocTriListHead;
        m_PrevTri = nullptr;
        CPlantMgr::m_UnusedLocTriListHead = this;
        if (m_NextTri) {
            m_NextTri->m_PrevTri = this;
        }

        m_SurfaceId = 0xFF;
    } else {
        if (auto prev = m_PrevTri) {
            if (auto next = m_NextTri) {
                next->m_PrevTri = prev;
                prev->m_NextTri = next;
            } else {
                prev->m_NextTri = nullptr;
            }
        } else {
            CPlantMgr::m_CloseLocTriListHead[3] = m_NextTri;
            if (CPlantMgr::m_CloseLocTriListHead[3])
                CPlantMgr::m_CloseLocTriListHead[3]->m_PrevTri = nullptr;
        }
        m_NextTri = CPlantMgr::m_UnusedLocTriListHead;
        m_PrevTri = nullptr;
        CPlantMgr::m_UnusedLocTriListHead = this;
        if (m_NextTri) {
            m_NextTri->m_PrevTri = this;
        }

        m_SurfaceId = 0xFE;
    }

    // m_nFlags = m_nFlags & 0xF8;
    m_createsPlants = false;
    m_createsObjects = false;
    m_createdObjects = false;
}
