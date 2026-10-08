from pathlib import Path
p=Path('source/game_sa/PedGeometryAnalyser.cpp');s=p.read_text();start=s.index('// 0x5F3DD0');b=s.index('{',start);end=s.index('\n}',b)
body='''
    pointRoute.Clear();
    auto& padding = *reinterpret_cast<float*>(0x8D22B0);
    const auto savedPadding = padding;
    padding = .175f;
    CVector corners[4], planes[4], temporaryCorners[4];
    float dots[4];
    const auto z = ped.GetPosition().z;
    ComputeEntityBoundingBoxCornersUncached(z, entity, corners);
    ComputeEntityBoundingBoxCornersUncached(z, entity, temporaryCorners);
    ComputeEntityBoundingBoxPlanesUncached(z, temporaryCorners, &planes, dots);
    padding = savedPadding;
    const auto planeDistance = [](const CVector& p, const CVector& n, float d) { return (p.z * n.z + p.x * n.x) + p.y * n.y + d; };
    for (auto i = 0; i < 4; ++i) {
        if (planeDistance(a2, planes[i], dots[i]) > 0.f && planeDistance(a4, planes[i], dots[i]) > 0.f) { return 0; }
    }
    const auto sideAt = [&entity](const CVector& p) {
        CVector vertices[4], center, normals[4]; float distances[4];
        ComputeEntityBoundingBoxCornersUncached(p.z, entity, vertices);
        ComputeEntityBoundingBoxCentreUncached(p.z, vertices, center);
        ComputeEntityBoundingBoxSegmentPlanesUncached(vertices, center, normals, distances);
        return ComputeEntityHitSide(p, normals, distances);
    };
    if (sideAt(a2) == sideAt(a4)) { return 0; }
    ComputeEntityBoundingBoxCornersUncached(z, entity, corners);
    ComputeEntityBoundingBoxCornersUncached(z, entity, temporaryCorners);
    ComputeEntityBoundingBoxPlanesUncached(z, temporaryCorners, &planes, dots);
    auto before = a2, after = a4;
    const auto originalBefore = before;
    auto direction = after - before;
    direction.Normalise();
    bool adjusted = false;
    const auto classify = [](float value) { return value > .2f ? 1 : value < -.2f ? -1 : 0; };
    for (auto i = 0; i < 4; ++i) {
        const auto p = planeDistance(before, planes[i], dots[i]);
        const auto q = planeDistance(after, planes[i], dots[i]);
        const auto first = classify(p), second = classify(q);
        const auto crossing = [&] {
            const auto denominator = (direction.z * planes[i].z + direction.x * planes[i].x) + direction.y * planes[i].y;
            return direction * (-planeDistance(originalBefore, planes[i], dots[i]) / denominator) + originalBefore;
        };
        if (first == -1) {
            if (second == 1) { after = crossing(); }
        } else if (second == -1) {
            if (first == 1) { before = crossing(); }
        } else {
            adjusted = true;
            if (first == 0) { before += planes[i] * (.2f - p); }
            if (second == 0) { after += planes[i] * (.2f - q); }
        }
    }
    if (!adjusted) {
        CColSphere sphere;
        ComputeEntityBoundingSphere(ped, entity, sphere);
        CVector planar{a4.x - a2.x, a4.y - a2.y, 0.f};
        if (std::sqrt(planar.y * planar.y + planar.x * planar.x) == 0.f) { return 0; }
        planar.Normalise();
        if (!sphere.IntersectRay(a2, planar, before, after)) { return 0; }
    }
    const auto segmentOrigin = before;
    direction = after - before;
    direction.Normalise();
    int enter = -1, leave = -1;
    for (auto i = 0; i < 4; ++i) {
        const auto p = planeDistance(before, planes[i], dots[i]);
        const auto q = planeDistance(after, planes[i], dots[i]);
        if (p == 0.f && q == 0.f) { continue; }
        const auto denominator = (direction.x * planes[i].x + direction.y * planes[i].y) + direction.z * planes[i].z;
        const auto crossing = direction * (-planeDistance(segmentOrigin, planes[i], dots[i]) / denominator) + segmentOrigin;
        if (p >= 0.f && q < 0.f) { enter = i; before = crossing; }
        else if (p < 0.f && q >= 0.f) { leave = i; after = crossing; }
    }
    if (enter < 0 || leave < 0) { return 0; }
    CVector paths[2][6];
    int sizes[2]{2, 2};
    paths[0][0] = paths[1][0] = a2;
    auto index = (enter + 3) % 4;
    const auto lastReverse = leave % 4;
    paths[0][1] = corners[index];
    while (index != lastReverse) {
        index = (index + 3) % 4;
        paths[0][sizes[0]++] = corners[index];
    }
    paths[0][sizes[0]++] = a4;
    index = enter % 4;
    const auto lastForward = (leave + 3) % 4;
    paths[1][1] = corners[index];
    while (index != lastForward) {
        index = (index + 1) % 4;
        paths[1][sizes[1]++] = corners[index];
    }
    paths[1][sizes[1]++] = a4;
    int selected = a6 == 1 ? 0 : 1;
    if (a6 == 0) {
        CVector center;
        center.x = ((corners[0].x + corners[1].x) + corners[2].x + corners[3].x) * .25f;
        center.y = ((corners[0].y + corners[1].y) + corners[2].y + corners[3].y) * .25f;
        center.z = ((corners[0].z + corners[1].z) + corners[2].z + corners[3].z) * .25f;
        struct RouteTest { bool clear; int stop; CEntity* obstacle; float length; } tests[2];
        for (auto path = 0; path < 2; ++path) {
            tests[path] = {true, sizes[path], nullptr, 0.f};
            for (auto i = 1; i < sizes[path]; ++i) {
                const auto& from = paths[path][i-1]; const auto& to = paths[path][i];
                auto planar = CVector{to.x - from.x, to.y - from.y, 0.f};
                planar.Normalise();
                CVector offset{planar.y * .5f, -planar.x * .5f, 0.f};
                const auto relative = to - center;
                if ((relative.z * 0.f + relative.y * offset.y) + relative.x * offset.x < 0.f) { offset *= -1.f; }
                CColPoint hit;
                auto& obstacle = tests[path].obstacle;
                if (CWorld::ProcessLineOfSight(from, to, hit, obstacle, true, true, true, true, false, false, false, false) || CWorld::ProcessLineOfSight(from + offset, to + offset, hit, obstacle, true, true, true, true, false, false, false, false)) {
                    if (obstacle != &entity && obstacle != &ped) {
                        const auto passPed = obstacle->GetIsTypePed() && reinterpret_cast<CPed*>(obstacle)->m_nPedState != static_cast<ePedState>(1) && [&] {
                            const auto& forward = obstacle->GetMatrix().GetForward();
                            const auto movement = to - from;
                            return (movement.x * forward.x + movement.y * forward.y) + movement.z * forward.z >= 0.f;
                        }();
                        if (!passPed) { tests[path].clear = false; tests[path].stop = i; break; }
                    } else { obstacle = nullptr; }
                }
            }
            for (auto i = 1; i < sizes[path]; ++i) {
                const auto delta = paths[path][i] - paths[path][i-1];
                tests[path].length += std::sqrt((delta.x * delta.x + delta.y * delta.y) + delta.z * delta.z);
            }
        }
        if (!tests[0].clear) {
            if (tests[1].clear) { selected = 1; }
            else if (tests[0].stop == 1 && tests[1].stop >= 2) { selected = 1; }
            else if (tests[0].stop > 1 && tests[1].stop == 1) { selected = 0; }
            else if (tests[0].obstacle == tests[1].obstacle) { selected = tests[0].length < tests[1].length ? 0 : 1; }
            else { selected = tests[0].obstacle->GetColModel()->m_boundSphere.m_fRadius < tests[1].obstacle->GetColModel()->m_boundSphere.m_fRadius ? 0 : 1; }
        } else if (!tests[1].clear) { selected = 0; }
        else { selected = tests[0].length < tests[1].length ? 0 : 1; }
    }
    for (auto i = 1; i < sizes[selected]-1; ++i) { pointRoute.AddUnlessFull(paths[selected][i]); }
    pointRoute.AddUnlessFull(a4);
    if (pointRoute.GetSize() > 1 && sideAt(a2) == sideAt(pointRoute.m_Entries[1])) {
        for (auto i = 0u; i < pointRoute.GetSize()-1; ++i) { pointRoute.m_Entries[i] = pointRoute.m_Entries[i+1]; }
        --pointRoute.m_NumEntries;
    }
    --pointRoute.m_NumEntries;
    return selected == 0 ? 2 : 1;
'''
s=s[:b+1]+body+s[end:];p.write_text(s)
