from pathlib import Path
p=Path('source/game_sa/PedGeometryAnalyser.cpp');s=p.read_text()
def replace(addr,body):
 global s
 start=s.index('// 0x'+addr);b=s.index('{',start);end=s.index('\n}',b);s=s[:b+1]+'\n'+body+s[end:]
replace('5F1B00','''    if (entity.m_bIsTempBuilding) { return false; }
    const auto& origin = ped.GetPosition();
    const auto target = ped.GetForward() + origin;
    return CWorld::GetIsLineOfSightClear(origin, target, true, false, false, true, false, false, false);''')
replace('5F32D0','''    auto origin = ped.GetPosition();
    auto direction = ped.GetForward();
    if (entity.m_bIsTempBuilding) { return false; }
    if (g_surfaceInfos.IsShallowWater(ped.m_nContactSurface)) { return true; }
    if (contactNormal.z > std::bit_cast<float>(0x3E2E147Bu)) {
        if (contactNormal.z > .9f) { return false; }
        const auto& sphere = ped.GetColModel()->m_pColData->m_pSpheres[0];
        origin.z += sphere.m_vecCenter.z - sphere.m_fRadius * contactNormal.z;
        const auto horizontal = std::sqrt(contactNormal.y * contactNormal.y + contactNormal.x * contactNormal.x);
        if (contactNormal.z > .5f) {
            direction = CVector{-contactNormal.x, -contactNormal.y, 0.f} / horizontal;
            direction += direction * horizontal * sphere.m_fRadius;
            direction *= std::min(2.f / horizontal, 4.f);
        } else {
            direction += direction * horizontal * sphere.m_fRadius;
        }
    } else if (!CPedGroups::IsInPlayersGroup(const_cast<CPed*>(&ped))) {
        origin.z -= .15f;
    }
    const auto target = direction + origin;
    if (!CWorld::GetIsLineOfSightClear(origin, target, true, false, false, true, false, false, false)) { return false; }
    const auto landing = direction * 3.f + origin;
    bool found = false;
    const auto ground = CWorld::FindGroundZFor3DCoord(landing, &found, nullptr);
    return found && landing.z - ground < 3.f;''')
p.write_text(s)
