from pathlib import Path
p=Path('source/game_sa/PedGeometryAnalyser.cpp');s=p.read_text()
def replace(addr,body):
 global s
 start=s.index('// 0x'+addr); b=s.index('{',start); end=s.index('\n}',b);s=s[:b+1]+'\n'+body+s[end:]
replace('5F1750','''    for (auto i = 0; i < 4; ++i) {
        a3[i] = { -(corners[i].y - center.y), corners[i].x - center.x, 0.f };
        a4[i] = -((a3[i].z * corners[i].z + a3[i].y * corners[i].y) + a3[i].x * corners[i].x);
    }
    // The original leaves its input corner pointer in EAX.
    return const_cast<CVector*>(corners);''')
replace('5F1500','''    CVector forward, right;
    if (entity.m_matrix) {
        forward = entity.m_matrix->GetForward();
        right = entity.m_matrix->GetRight();
    } else {
        const auto heading = entity.GetHeading();
        float sine, cosine;
        __asm { fld heading fsin fstp sine fld heading fcos fstp cosine }
        forward = { -sine, cosine, 0.f };
        right = { cosine, sine, 0.f };
    }
    posn[0] = forward;
    posn[1] = -right;
    posn[2] = -forward;
    posn[3] = right;
    return posn + 3;''')
replace('5F1450','''    for (auto side = 0; side < 4; ++side) {
        const auto previous = (side + 3) % 4;
        const auto& before = point2[previous];
        const auto& after = point2[side];
        const auto p = (before.y * point1.y + before.z * point1.z) + point1.x * before.x + x[previous];
        const auto q = (after.y * point1.y + after.z * point1.z) + point1.x * after.x + x[side];
        if (p >= 0.f && q < 0.f) {
            return side;
        }
    }
    return 0;''')
replace('5F3AC0','''    CVector corners[4], normals[4], center;
    float dots[4];
    ComputeEntityBoundingBoxCornersUncached(point.z, entity, corners);
    ComputeEntityBoundingBoxCentreUncached(point.z, corners, center);
    ComputeEntityBoundingBoxSegmentPlanesUncached(corners, center, normals, dots);
    return ComputeEntityHitSide(point, normals, dots);''')
replace('5F1E70','''    auto direction = -posn;
    direction.Normalise();
    CVector dirs[4];
    ComputeEntityDirs(ped, dirs);
    const auto dot = [&direction](const CVector& v) { return (v.z * direction.z + v.y * direction.y) + v.x * direction.x; };
    float maximum = -1.f;
    const auto first = dot(dirs[0]);
    int32 side = std::bit_cast<int32>(first);
    if (first >= maximum) { maximum = first; side = 0; }
    for (auto i = 1; i < 4; ++i) {
        const auto value = dot(dirs[i]);
        if (value >= maximum) { maximum = value; side = i; }
    }
    return side;''')
replace('5F3C20','''    CVector corners[4], center;
    ComputeEntityBoundingBoxCornersUncached(ped.GetPosition().z, entity, corners);
    ComputeEntityBoundingBoxCentre(ped.GetPosition().z, entity, center);
    auto maximum = 0.f;
    for (const auto& corner : corners) {
        const auto delta = corner - center;
        const auto squared = (delta.z * delta.z + delta.y * delta.y) + delta.x * delta.x;
        if (squared > maximum) { maximum = squared; }
    }
    a3.Set(std::sqrt(maximum) * 1.1f, center, static_cast<eSurfaceType>(0), 0, tColLighting{0xFF});''')
s=s.replace('__asm { fld heading fsin fstp sine fld heading fcos fstp cosine }','__asm {\n            fld heading\n            fsin\n            fstp sine\n            fld heading\n            fcos\n            fstp cosine\n        }')
p.write_text(s)
