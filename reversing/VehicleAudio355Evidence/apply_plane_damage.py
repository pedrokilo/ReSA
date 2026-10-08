from pathlib import Path
p=Path('source/game_sa/Entity/Vehicle/Plane.cpp');s=p.read_text();a=s.index('void CPlane::VehicleDamage(');b=s.index('// 0x6CAB90',a);native='''static float PlaneMassThreshold355(float mass, float threshold, bool minimumOne) {
    const float scale = PLANE_DAMAGE_SCALE_MASS, one = 1.f;
    float result;
    __asm {
        fld mass
        fdiv scale
        cmp minimumOne, 0
        je threshold_multiply
        fld one
        fcomp st(1)
        fnstsw ax
        test ah, 41h
        jne threshold_multiply
        fstp st(0)
        fld one
    threshold_multiply:
        fmul threshold
        fstp result
    }
    return result;
}

void CPlane::VehicleDamage(float damageIntensity, eVehicleCollisionComponent component, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) {
    if (m_nModelIndex == 539) {
        CAutomobile::VehicleDamage(damageIntensity, component, damager, vecCollisionCoors, vecCollisionDirection, weapon);
        return;
    }
    auto* bytes = reinterpret_cast<uint8*>(this);
    if (!(bytes[0x42A] & 0x20)) return;
    if (GetStatus() != STATUS_PLAYER && (m_nPhysicalFlags & 0x400000)) {
        auto* oldDamager = m_pDamageEntity;
        if (oldDamager != FindPlayerPed(-1)) {
            oldDamager = m_pDamageEntity;
            if (oldDamager != FindPlayerVehicle(-1, false)) return;
        }
    }
    float damageThreshold = PLANE_DAMAGE_THRESHHOLD;
    float partThreshold = std::bit_cast<float>(0x3EAA7EFAu);
    bool explosive = false;
    if (damageIntensity == 0.f) {
        damageIntensity = m_fDamageIntensity;
        partThreshold = PlaneMassThreshold355(m_fMass, partThreshold, true);
        damageThreshold = PlaneMassThreshold355(m_fMass, damageThreshold, false);
        vecCollisionCoors = &m_vecLastCollisionPosn;
        if (damageIntensity <= 0.f || (m_nPhysicalFlags & 0x100000) || (m_pDamageEntity && m_pDamageEntity->GetIsTypeObject())) return;
    } else {
        bool fireDamage = false;
        if (!CanVehicleBeDamaged(damager, weapon, fireDamage)) return;
        if (weapon != static_cast<eWeaponType>(19) && weapon != static_cast<eWeaponType>(20) && weapon != static_cast<eWeaponType>(21) && weapon != static_cast<eWeaponType>(51)) return;
        explosive = true;
        damageIntensity = 0.f;
    }
    const float healthDamage = damageIntensity * m_pHandlingData->m_fCollisionDamageMultiplier;
    const float previousHealth = m_fHealth;
    float factor;
    if (FindPlayerVehicle(-1, false) == this) factor = bytes[0x429] & 0x80 ? StaticRef<float>(0x85F0A0) : .5f;
    else if (bytes[0x429] & 0x80) factor = StaticRef<float>(0x85F0FC);
    else if (m_pDamageEntity && m_pDamageEntity == FindPlayerVehicle(-1, false)) factor = StaticRef<float>(0x862D68);
    else factor = StaticRef<float>(0x858C84);
    float* health = &m_fHealth;
    __asm {
        mov eax, health
        fld healthDamage
        fmul factor
        fsubr dword ptr [eax]
        fstp dword ptr [eax]
    }
    if (m_fHealth <= 0.f && previousHealth > 0.f) m_fHealth = 1.f;
    if (damageIntensity > damageThreshold || (m_fHealth <= 1.f && previousHealth > 0.f)) {
        BlowUpCar(this, false);
        return;
    }
    if (!(damageIntensity > partThreshold) && !explosive) return;
    if (GetStatus() == STATUS_WRECKED || !(m_fMass > 1000.f)) return;
    const CVector relative = m_matrix->InverseTransformVector(*vecCollisionCoors - GetPosition());
    float closest = 1000.f;
    int32 selected = -1;
    for (int32 i = 12; i < 25; ++i) {
        if (auto* node = m_aCarNodes[i]) {
            const auto delta = CVector{node->modelling.pos} - relative;
            const float squared = delta.SquaredMagnitude();
            if (squared < closest * closest) {
                closest = std::sqrt(squared);
                selected = i;
            }
        }
    }
    if (selected < 0) return;
    if (selected == 16 && (CGeneral::GetRandomNumber() & 1)) {
        if (m_aCarNodes[17] && (CGeneral::GetRandomNumber() & 1)) selected = 17;
        else if (m_aCarNodes[18]) selected = 18;
    }
    if (m_damageManager.ProgressAeroplaneDamage(uint8(selected))) {
        const auto status = m_damageManager.GetAeroplaneCompStatus(uint8(selected));
        if (status == 1 && selected >= 16 && selected <= 22) {
            auto* panels = reinterpret_cast<CBouncingPanel*>(bytes + 0x6AC);
            for (int32 i = 0; i < 4; ++i) {
                if (panels[i].m_nFrameId == uint16(-1)) {
                    panels[i].SetPanel(int16(selected), 0, std::bit_cast<float>(0xBCA3D70Au));
                    break;
                }
                if (int32(int16(panels[i].m_nFrameId)) == selected) break;
            }
        } else if (m_damageManager.GetAeroplaneCompStatus(uint8(selected)) == 2) {
            SetComponentVisibility(m_aCarNodes[selected], 2);
        }
    }
    dmgDrawCarCollidingParticles(*vecCollisionCoors, damageIntensity, weapon);
    const CVector smokePosition{m_aCarNodes[selected]->modelling.pos};
    if (m_pSmokeParticle) {
        m_pSmokeParticle->Kill();
        m_pSmokeParticle = nullptr;
    }
    auto* matrix = GetModellingMatrix();
    if (!matrix) return;
    m_pSmokeParticle = g_fxMan.CreateFxSystem("fire_med", smokePosition, matrix, false);
    if (!m_pSmokeParticle) return;
    m_pSmokeParticle->Play();
    m_pSmokeParticle->SetVelAdd(-m_vecMoveSpeed * 5.f);
    m_pSmokeParticle->SetLocalParticles(true);
    m_nSmokeTimer = uint32(CGeneral::GetRandomIntInRange(2000, 4000));
}

''';p.write_text(s[:a]+native+s[b:]);Path('reversing/VehicleAudio355Evidence/Plane_DamageNative.txt').write_text(native)
