from pathlib import Path
import re
p=Path('source/game_sa/Interior/InteriorGroup_c.cpp');s=p.read_text(encoding='utf8')
bodies={
'Init':'''    std::fill(std::begin(m_interiors), std::end(m_interiors), nullptr);
    std::fill(std::begin(m_peds), std::end(m_peds), nullptr);
    std::fill(std::begin(m_pedsToRemove), std::end(m_pedsToRemove), nullptr);
    m_numInteriors = 0;
    m_pathSetupComplete = m_updatePeds = m_animBlockReferenced = 0;
    m_isVisible = m_lastIsVisible = false;
    m_pEntity = entity;
    m_groupId = uint8(id);''',
'AddInterior':'''    for (int32 i = 0; i < 8; ++i) {
        if (!m_interiors[i]) {
            m_interiors[i] = interior;
            ++m_numInteriors;
            return i;
        }
    }
    return 8;''',
'GetPed':'    return m_peds[idx];',
'GetEntity':'    return m_pEntity;',
'ContainsInteriorType':'''    for (auto* interior : m_interiors) {
        if (interior && *reinterpret_cast<const uint8*>(interior->m_box) == a2) {
            return 1;
        }
    }
    return 0;''',
'GetNumInteriorInfos':'''    int32 count = 0;
    for (auto* interior : m_interiors) {
        if (!interior) continue;
        for (int32 i = 0; i < interior->m_interiorInfosCount; ++i) {
            if (*reinterpret_cast<const int8*>(&interior->m_interiorInfos[i].Type) == a2) ++count;
        }
    }
    return count;''',
'GetRandomInterior':'''    const float value = float(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0f / 32768.0f);
    const int32 selected = int32(value * int8(m_numInteriors));
    int32 count = 0;
    for (auto* interior : m_interiors) {
        if (interior && count++ == selected) return int32(uintptr_t(interior));
    }
    return 0;''',
'FindInteriorInfo':'''    InteriorInfo_t* infos[64];
    Interior_c* interiors[64];
    int32 count = 0;
    for (auto* interior : m_interiors) {
        if (!interior) continue;
        for (int32 i = 0; i < interior->m_interiorInfosCount; ++i) {
            auto* info = &interior->m_interiorInfos[i];
            if (*reinterpret_cast<const int8*>(&info->Type) == int32(infoType) && !info->IsInUse) {
                infos[count] = info;
                interiors[count++] = interior;
            }
        }
    }
    if (count <= 0) {
        *a3 = nullptr;
        *a4 = nullptr;
        return false;
    }
    const float value = float(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0f / 32768.0f);
    const auto index = int32(value * count);
    *a3 = infos[index];
    *a4 = interiors[index];
    return true;''',
'CalcIsVisible':'''    m_lastIsVisible = m_isVisible;
    m_isVisible = false;
    for (int32 i = 0; i < int8(m_numInteriors); ++i) {
        if (m_interiors[i]->IsVisible()) {
            m_isVisible = true;
            break;
        }
    }''',
'Setup':'''    if (ContainsInteriorType(2)) m_groupType = 0;
    else if (ContainsInteriorType(0) || ContainsInteriorType(6)) m_groupType = 1;
    else m_groupType = ContainsInteriorType(1) ? 2 : uint8(-1);
    ReferenceAnims();'''}
for name,b in bodies.items():
 pattern=r'([^\n]+InteriorGroup_c::'+name+r'\([^\n]*\) \{)\n.*?\n\}'
 s,n=re.subn(pattern,lambda m:m[1]+'\n'+b+'\n}',s,flags=re.S)
 assert n==1,(name,n)
 s=re.sub(r'(RH_ScopedInstall\('+name+r', 0x[0-9A-F]+), \{ \.reversed = false \}',r'\1',s)
s=s.replace('int8 InteriorGroup_c::CalcIsVisible()', 'void InteriorGroup_c::CalcIsVisible()')
p.write_text(s,encoding='utf8')
p=Path('source/game_sa/Interior/InteriorGroup_c.h');s=p.read_text().replace('int8 CalcIsVisible();','void CalcIsVisible();');p.write_text(s)

