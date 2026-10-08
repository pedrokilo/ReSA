from pathlib import Path
p=Path('source/game_sa/PedGeometryAnalyser.cpp');s=p.read_text();start=s.index('// 0x5F1FA0');b=s.index('{',start);end=s.index('\n}',b)
body='''
    if (entity.GetIsTypeBuilding() && entity.m_bIsTempBuilding && g_interiorMan.GetBoundingBox(&entity, corners)) {
        for (auto i = 0; i < 4; ++i) { corners[i].z = zPos; }
        return;
    }
    auto& matrix = entity.GetMatrix();
    const auto* model = entity.GetColModel();
    const auto* data = model->m_pColData;
    auto lower = model->m_boundBox.m_vecMin;
    auto upper = model->m_boundBox.m_vecMax;
    if (entity.GetIsTypeObject() && data && upper.z - lower.z > 6.f && (data->m_nNumSpheres || data->m_nNumBoxes)) {
        lower = CVector{FLT_MAX, FLT_MAX, FLT_MAX};
        upper = CVector{-FLT_MAX, -FLT_MAX, -FLT_MAX};
        const auto consider = [&](const CVector& lo, const CVector& hi) {
            const auto worldLo = matrix.TransformPoint(lo), worldHi = matrix.TransformPoint(hi);
            if ((zPos - 1.f <= worldLo.z || zPos - 1.f <= worldHi.z) && (worldLo.z <= zPos + 1.f || worldHi.z <= zPos + 1.f)) {
                lower.x = lower.x < lo.x ? lower.x : lo.x;
                lower.y = lower.y < lo.y ? lower.y : lo.y;
                lower.z = lower.z < lo.z ? lower.z : lo.z;
                upper.x = hi.x < upper.x ? upper.x : hi.x;
                upper.y = hi.y < upper.y ? upper.y : hi.y;
                upper.z = hi.z < upper.z ? upper.z : hi.z;
            }
        };
        for (auto i = 0; i < data->m_nNumBoxes; ++i) { consider(data->m_pBoxes[i].m_vecMin, data->m_pBoxes[i].m_vecMax); }
        for (auto i = 0; i < data->m_nNumSpheres; ++i) {
            const auto& sphere = data->m_pSpheres[i];
            const auto radius = CVector{sphere.m_fRadius, sphere.m_fRadius, sphere.m_fRadius};
            consider(sphere.m_vecCenter - radius, sphere.m_vecCenter + radius);
        }
    }
    const auto padding = *reinterpret_cast<const float*>(0x8D22B0);
    const CVector expandedLower{lower.x - padding, lower.y - padding, lower.z - padding};
    const CVector expandedUpper{upper.x + padding, upper.y + padding, upper.z + padding};
    const auto extent = (expandedUpper - expandedLower) * .5f;
    const auto center = matrix.TransformPoint((expandedUpper + expandedLower) * .5f);
    const CVector axes[]{matrix.GetRight(), matrix.GetForward(), matrix.GetUp()};
    const float half[]{extent.x, extent.y, extent.z};
    float widths[3];
    for (auto i = 0; i < 3; ++i) {
        const auto norm = axes[i].y * axes[i].y + axes[i].x * axes[i].x;
        widths[i] = half[i] * norm + half[i] * norm;
    }
    const auto selected = widths[0] > widths[1] && widths[0] > widths[2] ? 0 : widths[1] > widths[2] ? 1 : 2;
    const CVector axis{axes[selected].x, axes[selected].y, 0.f};
    const auto positive = axis * half[selected] + center;
    const auto negative = center - axis * half[selected];
    auto normal = axis;
    normal.Normalise();
    float along = 0.f, across = 0.f;
    // Other axes are visited in the same order as the executable's three cases.
    const int others[3][2]{{1, 2}, {0, 2}, {0, 1}};
    for (auto j : others[selected]) {
        auto parallel = (normal.y * axes[j].y + normal.x * axes[j].x) * half[j];
        auto perpendicular = (normal.x * axes[j].y - normal.y * axes[j].x) * half[j];
        if (parallel < 0.f) { parallel = -parallel; }
        if (perpendicular < 0.f) { perpendicular = -perpendicular; }
        along += parallel;
        across += perpendicular;
    }
    const auto extra = normal * along;
    const CVector side{across * normal.y, -normal.x * across, across * 0.f};
    corners[0] = positive + extra - side;
    corners[1] = negative - extra - side;
    corners[2] = negative - extra + side;
    corners[3] = positive + extra + side;
    for (auto i = 0; i < 4; ++i) { corners[i].z = zPos; }
'''
s=s[:b+1]+body+s[end:];s=s.replace('#include "PedGeometryAnalyser.h"','#include "PedGeometryAnalyser.h"\n#include "Interior/InteriorManager_c.h"');p.write_text(s)
