from pathlib import Path
p=Path('source/game_sa/PedGeometryAnalyser.cpp');s=p.read_text()
def replace(addr,body):
 global s
 start=s.index('// 0x'+addr);b=s.index('{',start);end=s.index('\n}',b);s=s[:b+1]+'\n'+body+s[end:]
replace('5F36F0','''    CVector corners[4];
    ComputeEntityBoundingBoxCornersUncached(posn.z, entity, corners);
    return ComputeClosestSurfacePoint(posn, corners, point);''')
replace('5F2C10','''    auto minimum = std::numeric_limits<float>::max();
    bool found = false;
    for (auto i = 0; i < 4; ++i) {
        const auto edge = corners[(i + 1) % 4] - corners[i];
        const auto length = std::sqrt((edge.z * edge.z + edge.y * edge.y) + edge.x * edge.x);
        const auto normal = edge * (1.f / length);
        const auto relative = posn - corners[i];
        const auto projection = (relative.z * normal.z + relative.y * normal.y) + relative.x * normal.x;
        if (projection >= 0.f && projection <= length) {
            const auto candidate = corners[i] + normal * projection;
            const auto delta = posn - candidate;
            const auto squared = (delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x;
            if (squared < minimum) { minimum = squared; point = candidate; found = true; }
        }
    }
    // Endpoint fallback is only performed when no edge projection was accepted.
    if (!found) {
        for (auto i = 0; i < 4; ++i) {
            const auto delta = corners[i] - posn;
            const auto squared = (delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x;
            if (squared < minimum) { minimum = squared; point = corners[i]; found = true; }
        }
    }
    return found;''')
replace('5F3590','''    auto pool = GetPedPool();
    auto minimum = std::numeric_limits<float>::max();
    CPed* nearest = nullptr;
    for (auto i = pool->GetSize(); i > 0; --i) {
        if (auto candidate = pool->GetAt(i - 1)) {
            const auto delta = point - candidate->GetPosition();
            const auto squared = (delta.z * delta.z + delta.x * delta.x) + delta.y * delta.y;
            if (squared < minimum) { minimum = squared; nearest = candidate; }
        }
    }
    return nearest;''')
replace('5F1B70','''    const auto delta = a2 - ped.GetPosition();
    if (a3) {
        const auto& forward = ped.GetForward();
        if ((delta.z * forward.z + delta.y * forward.y) + delta.x * forward.x < 0.f) { return false; }
    }
    if ((delta.z * delta.z + delta.x * delta.x) + delta.y * delta.y > 1600.f) { return false; }
    const auto origin = ped.GetPosition() + CVector{0.f, 0.f, .75f};
    return CWorld::GetIsLineOfSightClear(origin, a2, true, false, false, true, false, true, false);''')
replace('5F2F00','''    const CColLine line{a1, a2};
    auto colModel = a3.GetColModel();
    return !CCollision::TestLineOfSight(line, a3.GetMatrix(), *colModel, false, false);''')
replace('5F3880','''    const auto delta = posn - entity.GetPosition();
    const auto radius = entity.GetColModel()->m_boundSphere.m_fRadius;
    if (!(radius * radius > (delta.y * delta.y + delta.x * delta.x) + delta.z * delta.z)) { return false; }
    CVector corners[4], planes[4];
    float dots[4];
    const auto z = ped.GetPosition().z;
    ComputeEntityBoundingBoxCornersUncached(z, entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(z, corners, &planes, dots);
    for (auto i = 0; i < 4; ++i) {
        if ((posn.y * planes[i].y + planes[i].z * posn.z) + planes[i].x * posn.x + dots[i] < 0.f) { return true; }
    }
    return false;''')
replace('5F3970','''    const auto delta = entity->GetPosition() - point;
    if (!(std::abs(delta.z) <= 3.f)) { return false; }
    const auto radius = entity->GetColModel()->m_boundSphere.m_fRadius;
    // The executable compares squared radii with the unsquared XY distance.
    if (!(distance * distance + radius * radius >= std::sqrt(delta.x * delta.x + delta.y * delta.y))) { return false; }
    CVector corners[4], planes[4];
    float dots[4];
    const auto z = entity->GetPosition().z;
    ComputeEntityBoundingBoxCornersUncached(z, *entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(z, corners, &planes, dots);
    for (auto i = 0; i < 4; ++i) {
        if ((point.y * planes[i].y + planes[i].z * point.z) + planes[i].x * point.x + dots[i] + distance * .5f > 0.f) { return false; }
    }
    return true;''')
replace('5F3730','''    CVector corners[4], planes[4];
    float dots[4];
    const auto z = ped.GetPosition().z;
    ComputeEntityBoundingBoxCornersUncached(z, entity, corners);
    ComputeEntityBoundingBoxPlanesUncached(z, corners, &planes, dots);
    const auto& position = ped.GetPosition();
    const auto first = (planes[1].y * position.y + planes[1].z * position.z) + planes[1].x * position.x + dots[1];
    // Preserve the original third-plane Z product and branch-specific EAX values.
    const auto second = (planes[3].y * position.y + planes[3].z * planes[3].z) + planes[3].x * position.x + dots[3];
    if (first > 0.f) { outDirToAvoidEntity = planes[1]; return static_cast<int32>(reinterpret_cast<uintptr_t>(&outDirToAvoidEntity)); }
    if (second > 0.f) { outDirToAvoidEntity = planes[3]; return std::bit_cast<int32>(planes[3].z); }
    if (first > second) { outDirToAvoidEntity = planes[1]; return std::bit_cast<int32>(planes[1].y); }
    outDirToAvoidEntity = planes[3];
    return static_cast<int32>(reinterpret_cast<uintptr_t>(&outDirToAvoidEntity));''')
p.write_text(s)
