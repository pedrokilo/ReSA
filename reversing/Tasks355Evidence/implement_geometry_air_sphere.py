from pathlib import Path
p=Path('source/game_sa/PedGeometryAnalyser.cpp');s=p.read_text()
def replace(addr,body):
 global s
 start=s.index('// 0x'+addr);b=s.index('{',start);end=s.index('\n}',b);s=s[:b+1]+'\n'+body+s[end:]
replace('5F13F0','''    const auto* origin = &ped.GetPosition();
    const auto* target = &posn;
    const auto rotation = ped.m_fCurrentRotation;
    constexpr float quarter = std::bit_cast<float>(0x3F490FDBu);
    constexpr float full = std::bit_cast<float>(0x40C90FDBu);
    constexpr float scale = std::bit_cast<float>(0x3F22F983u);
    int32 result;
    // Keep the original extended-precision atan and truncation sequence.
    __asm {
        mov ecx, origin
        mov edx, target
        fld dword ptr [edx]
        fsub dword ptr [ecx]
        fchs
        fld dword ptr [edx+4]
        fsub dword ptr [ecx+4]
        fpatan
        fsub rotation
        fadd quarter
        fldz
        fxch st(1)
        fcom st(1)
        fnstsw ax
        fstp st(1)
        test ah, 5
        jp nonnegative
        fadd full
    nonnegative:
        fmul scale
        mov eax, 0821B40h
        call eax
        mov result, eax
    }
    return result;''')
replace('5F1CB0','''    if (ped.bInVehicle) { return false; }
    auto& manager = ped.GetTaskManager();
    if (manager.GetActiveTask()) {
        if (ped.GetIntelligence()->GetTaskSwim() || ped.GetIntelligence()->GetTaskJetPack() || manager.GetSimplestActiveTask()->GetTaskType() == static_cast<eTaskType>(0xFE)) { return false; }
    }
    bool falling = false;
    if (manager.GetActiveTask()) { falling = manager.GetActiveTask()->GetTaskType() == static_cast<eTaskType>(0xD3); }
    auto origin = ped.GetPosition();
    CColPoint point;
    CEntity* entity;
    auto ground = CWorld::ProcessVerticalLine(origin, ped.GetPosition().z - 1.5f, point, entity, true, true, false, true, false, false, nullptr);
    if (!ground && !falling) {
        origin.z -= 1.f;
        ground = CWorld::TestSphereAgainstWorld(origin, .15f, const_cast<CPed*>(&ped), true, false, false, false, false, false) != nullptr;
    }
    return !ground;''')
replace('5F1890','''    const auto& origin = ped.GetPosition();
    a5 = a4;
    CVector first, second;
    if (sphere.IntersectPoint(a4)) {
        auto direction = a4 - a3;
        direction.Normalise();
        if (sphere.IntersectRay(origin, direction, first, second)) { a5 = second; }
    }
    auto direction = a5 - origin;
    direction.Normalise();
    if (!sphere.IntersectRay(a5, direction, first, second)) { a6 = a5; return false; }
    const auto distanceSquared = [](const CVector& v) { return (v.z * v.z + v.y * v.y) + v.x * v.x; };
    if (distanceSquared(a5 - origin) < distanceSquared(first - origin)) { a6 = a5; return false; }
    if (sphere.IntersectRay(origin, direction, first, second)) {
        const auto delta = sphere.m_vecCenter - origin;
        const auto projection = (delta.z * direction.z + delta.y * direction.y) + delta.x * direction.x;
        auto normal = direction * projection + origin - sphere.m_vecCenter;
        normal.Normalise();
        a6 = normal * sphere.m_fRadius + sphere.m_vecCenter;
    }
    return true;''')
p.write_text(s)
