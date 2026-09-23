#include "StdInc.h"

#include "EntityScanner.h"

void CEntityScanner::InjectHooks() {
    RH_ScopedVirtualClass(CEntityScanner, 0x86CD40, 1);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x5FF990);
    RH_ScopedInstall(Destructor, 0x603480);

    RH_ScopedInstall(Clear, 0x5FF9D0);
    RH_ScopedVMTInstall(ScanForEntitiesInRange, 0x5FFA20);
}

// 0x5FF990
CEntityScanner::CEntityScanner() {
    m_timer = {};

    rng::fill(m_apEntities, nullptr);
    m_pClosestEntityInRange = nullptr;

    m_timer.SetPeriod(MAX_NUM_ENTITIES);
}

// 0x603480
CEntityScanner::~CEntityScanner() {
    Clear();
}

// 0x5FF9D0
void CEntityScanner::Clear() {
    for (auto& entity : m_apEntities) {
        CEntity::ClearReference(entity);
    }

    CEntity::ClearReference(m_pClosestEntityInRange);
}

// Preserve x87 intermediate precision and the original (z*z + x*x) + y*y order.
static double ScannerDistance(const CVector& a, const CVector& b) {
    double result;
    __asm {
        mov eax, a
        mov ecx, b
        fld dword ptr [eax]
        fsub dword ptr [ecx]
        fld dword ptr [eax + 4]
        fsub dword ptr [ecx + 4]
        fld dword ptr [eax + 8]
        fsub dword ptr [ecx + 8]
        fld st(0)
        fmul st(0), st(1)
        fld st(3)
        fmul st(0), st(4)
        faddp st(1), st(0)
        fld st(2)
        fmul st(0), st(3)
        faddp st(1), st(0)
        fstp result
        fstp st(0)
        fstp st(0)
        fstp st(0)
    }
    return result;
}

static int32 ScannerSector(float position, float delta) {
    const float scale = 0.02f, origin = 60.f;
    double result;
    __asm {
        fld position
        fadd delta
        fmul scale
        fadd origin
        fstp result
    }
    return int32(std::floor(result));
}

// 0x5FFA20
void CEntityScanner::ScanForEntitiesInRange(const eRepeatSectorList sectorList, const CPed& ped) {
    if (!m_timer.Tick()) {
        return;
    }
    Clear();
    const bool isPlayer = FindPlayerPed() == &ped;
    const auto* intelligence = ped.GetIntelligence();
    const float range = intelligence->m_fHearingRange > intelligence->m_fSeeingRange
        ? intelligence->m_fHearingRange : intelligence->m_fSeeingRange;
    const auto& position = ped.GetPosition();
    const int32 minX = std::max(0, ScannerSector(position.x, -range));
    const int32 minY = std::max(0, ScannerSector(position.y, -range));
    const int32 maxX = std::min(119, ScannerSector(position.x, range));
    const int32 maxY = std::min(119, ScannerSector(position.y, range));
    double rangeSquared;
    __asm {
        fld range
        fmul range
        fstp rangeSquared
    }
    std::array<float, MAX_NUM_ENTITIES> distances;
    distances.fill(std::numeric_limits<float>::max());
    int32 count = 0;
    CWorld::AdvanceCurrentScanCode();
    const_cast<CPed&>(ped).SetCurrentScanCode();
    const auto scan = [&](const auto& list) {
        for (auto* node = list.GetNode(); node; node = node->Next) {
            CEntity* entity = node->Item;
            if (entity->IsScanCodeCurrent()) {
                continue;
            }
            entity->SetCurrentScanCode();
            if (sectorList == REPEATSECTOR_PEDS && !isPlayer && static_cast<CPed*>(entity)->m_nPedState == PEDSTATE_DEAD) {
                continue;
            }
            const double distance = ScannerDistance(entity->GetPosition(), position);
            if (!(distance < rangeSquared)) {
                continue;
            }
            for (int32 slot = 0; slot < int32(MAX_NUM_ENTITIES); ++slot) {
                if (!m_apEntities[slot]) {
                    m_apEntities[slot] = entity;
                    distances[slot] = float(distance);
                    ++count;
                    break;
                }
                if (distance < distances[slot]) {
                    for (int32 j = std::min(count, int32(MAX_NUM_ENTITIES) - 1); j > slot; --j) {
                        m_apEntities[j] = m_apEntities[j - 1];
                        distances[j] = distances[j - 1];
                    }
                    m_apEntities[slot] = entity;
                    distances[slot] = float(distance);
                    count = std::min(count + 1, int32(MAX_NUM_ENTITIES));
                    break;
                }
            }
        }
    };
    for (int32 y = minY; y <= maxY; ++y) {
        for (int32 x = minX; x <= maxX; ++x) {
            const auto& sector = CWorld::GetRepeatSector(x, y);
            switch (sectorList) {
            case REPEATSECTOR_VEHICLES: scan(sector.Vehicles); break;
            case REPEATSECTOR_PEDS:     scan(sector.Peds); break;
            case REPEATSECTOR_OBJECTS:  scan(sector.Objects); break;
            }
        }
    }
    for (int32 i = 0; i < count; ++i) {
        CEntity::SafeRegisterRef(m_apEntities[i]);
    }
    if (m_apEntities[0]) {
        m_pClosestEntityInRange = m_apEntities[0];
        CEntity::SafeRegisterRef(m_pClosestEntityInRange);
    }
}
