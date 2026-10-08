from pathlib import Path
p=Path('source/game_sa/PedGeometryAnalyser.cpp');s=p.read_text()
def replace(addr,body):
 global s
 start=s.index('// 0x'+addr);b=s.index('{',start);end=s.index('\n}',b);s=s[:b+1]+'\n'+body+s[end:]
replace('5F2F70','''    if (std::abs(from.z - to.z) > maxHeightChange) { return WanderPathClearness::BLOCKED_HEIGHT; }
    const auto z = from.z < to.z ? from.z : to.z;
    if (!CWorld::GetIsLineOfSightClear({from.x, from.y, z}, {to.x, to.y, z}, true, false, false, false, false, false, false)) { return WanderPathClearness::BLOCKED_LOS; }
    auto direction = to - from;
    const auto length = std::sqrt((direction.z * direction.z + direction.x * direction.x) + direction.y * direction.y);
    const auto samples = std::min(maxSamples, static_cast<int32>(reinterpret_cast<double(__cdecl*)(double)>(0x8219F0)(length)));
    if (samples != 0) {
        direction.Normalise();
        for (auto i = 1; i < samples; ++i) {
            const auto sample = direction * float(i) + from;
            float water;
            if (CWaterLevel::GetWaterLevel(sample.x, sample.y, sample.z, water, 0, nullptr)) {
                CColPoint point; CEntity* entity = nullptr;
                if (!CWorld::ProcessVerticalLine({sample.x, sample.y, water}, from.z > to.z ? from.z : to.z, point, entity, true, false, false, false, false, false, nullptr)) { return WanderPathClearness::BLOCKED_WATER; }
            }
        }
        CColPoint point; CEntity* entity = nullptr;
        if (!CWorld::ProcessVerticalLine(from, from.z - 5.f, point, entity, true, false, false, false, false, false, nullptr)) { return WanderPathClearness::BLOCKED_SHARP_DROP; }
        auto previous = point.m_vecPoint.z + .5f;
        for (auto i = 1; i < samples; ++i) {
            const auto sample = direction * float(i) + from;
            if (!CWorld::ProcessVerticalLine({sample.x, sample.y, previous}, previous - 2.f, point, entity, true, false, false, false, false, false, nullptr)) { return WanderPathClearness::BLOCKED_SHARP_DROP; }
            if (std::abs(point.m_vecPoint.z - previous) > 1.f) { return WanderPathClearness::BLOCKED_SHARP_DROP; }
            previous = point.m_vecPoint.z + .5f;
        }
    }
    return WanderPathClearness::CLEAR;''')
replace('5F5A30','''    auto origin = ped.GetPosition();
    auto target = a2;
    CColSphere sphere;
    ComputeEntityBoundingSphere(ped, entity, sphere);
    auto direction = target - origin;
    direction.Normalise();
    CVector first, second;
    if (!sphere.IntersectRay(origin, direction, first, second)) { return true; }
    CVector corners[4], planes[4];
    float dots[4];
    const auto z = ped.GetPosition().z;
    ComputeEntityBoundingBoxCornersUncached(z, entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(z, corners, &planes, dots);
    a4 = 0.f;
    const auto epsilon = *reinterpret_cast<const float*>(0x8D22B0);
    const auto classify = [epsilon](float distance) { return distance > epsilon ? 1 : distance < -epsilon ? -1 : 0; };
    for (auto i = 0; i < 4; ++i) {
        const auto& n = planes[i];
        const auto p = (origin.z * n.z + origin.y * n.y) + origin.x * n.x + dots[i];
        const auto q = (target.z * n.z + target.y * n.y) + target.x * n.x + dots[i];
        const auto before = classify(p), after = classify(q);
        if (before < 0) {
            if (after > 0) {
                const auto denominator = (direction.z * n.z + direction.y * n.y) + direction.x * n.x;
                if (denominator > .001f) { target = direction * ((-1.f / denominator) * p) + origin; }
            }
        } else {
            if (after >= 0) { return true; }
            if (before >= 1) {
                const auto denominator = (direction.z * n.z + direction.y * n.y) + direction.x * n.x;
                if (denominator > .001f) { origin = direction * ((-1.f / denominator) * p) + origin; }
            }
        }
    }
    const auto delta = target - origin;
    a4 = std::sqrt((delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x);
    return false;''')
replace('5F5D80','''    a3 = a2;
    const auto& origin = ped.GetPosition();
    const auto epsilon = *reinterpret_cast<const float*>(0x8D22B0);
    const auto avoid = [&](CEntity* entity) {
        if (!entity) { return; }
        const auto delta = entity->GetPosition() - a3;
        if ((delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x < 25.f && LiesInsideBoundingBox(ped, a3, *entity)) {
            float distance;
            if (!GetIsLineOfSightClear(ped, a3, *entity, distance)) {
                auto direction = a3 - origin;
                direction.Normalise();
                a3 -= direction * (epsilon + distance);
            }
        }
    };
    for (auto entity : ped.GetIntelligence()->m_vehicleScanner.m_apEntities) { avoid(entity); }
    for (auto entity : ped.GetIntelligence()->m_pedScanner.m_apEntities) { avoid(entity); }
    auto direction = origin - a3;
    direction.Normalise();
    direction *= epsilon;
    const auto samples = static_cast<int32>(5.f / epsilon) + 1;
    for (auto i = 0; i < samples; ++i) {
        const auto delta = origin - a3;
        if ((delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x >= 25.f || (delta.x * direction.x + delta.y * direction.y) + direction.z * delta.z < 0.f) { return; }
        CColPoint point; CEntity* entity;
        CWorld::ProcessLineOfSight(origin, a3, point, entity, true, false, false, false, true, false, false, false);
        if (CWorld::ms_iProcessLineNumCrossings % 2 != 1) { return; }
        a3 += direction;
    }''')
p.write_text(s)
