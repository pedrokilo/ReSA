from pathlib import Path
p=Path('source/game_sa/ProjectileInfo.cpp');s=p.read_text(encoding='utf8')
s=s.replace('    plugin::Call<0x737B40>();','''    ms_apProjectile.fill(nullptr);
    for (auto& info : gaProjectileInfo) {
        info.m_nWeaponType = WEAPON_GRENADE;
        info.m_pCreator = nullptr;
        info.m_nDestroyTime = 0;
        info.m_bActive = false;
        info.m_pFxSystem = nullptr;
    }''')
s=s.replace('    plugin::Call<0x737BC0>();','''    for (auto& info : gaProjectileInfo) {
        info.RemoveFXSystem(true);
    }''')
s=s.replace('    return plugin::CallAndReturn<CProjectileInfo*, 0x737BF0, int32>(infoId);','    return gaProjectileInfo.data() + infoId;')
s=s.replace('    plugin::Call<0x737C00, CEntity*, eWeaponType, CVector>(creator, weaponType, pos);','''    eExplosionType type;
    switch (weaponType) {
    case WEAPON_GRENADE:
    case WEAPON_REMOTE_SATCHEL_CHARGE: type = EXPLOSION_GRENADE; break;
    case WEAPON_MOLOTOV: type = EXPLOSION_MOLOTOV; break;
    case WEAPON_ROCKET:
    case WEAPON_ROCKET_HS: type = EXPLOSION_ROCKET; break;
    default: return;
    }
    CExplosion::AddExplosion(nullptr, creator, type, pos, 0, 1, -1.0f, 0);''')
s=s.replace('    plugin::Call<0x738860>();','''    for (size_t i = 0; i < MAX_PROJECTILES; ++i) {
        auto& info = gaProjectileInfo[i];
        if (!info.m_bActive || info.m_nWeaponType != WEAPON_REMOTE_SATCHEL_CHARGE) {
            continue;
        }
        auto* projectile = ms_apProjectile[i];
        CExplosion::AddExplosion(nullptr, info.m_pCreator, EXPLOSION_GRENADE, projectile->GetPosition(), 0, 1, -1.0f, 0);
        info.m_bActive = false;
        info.RemoveFXSystem(false);
        projectile->bRemoveFromWorld = true;
    }''')
s=s.replace('    plugin::CallMethod<0x737B80, CProjectileInfo*, bool>(this, bInstantly);','''    if (!m_pFxSystem) {
        return;
    }
    if (bInstantly) {
        g_fxMan.DestroyFxSystem(m_pFxSystem);
    } else {
        m_pFxSystem->Kill();
    }
    m_pFxSystem = nullptr;''')
for name in ['Initialise','Shutdown','GetProjectileInfo','RemoveNotAdd','RemoveDetonatorProjectiles','RemoveFXSystem']:
 import re
 s=re.sub(r'(RH_ScopedInstall\('+name+r', 0x[0-9A-F]+), \{ \.reversed = false \}',r'\1',s)
p.write_text(s,encoding='utf8')
