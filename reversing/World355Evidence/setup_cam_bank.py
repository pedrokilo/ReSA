from pathlib import Path
p=Path('reversing/CamCore355');p.mkdir(exist_ok=True)
for name in ['build_probe.py','run_local.py']:
 s=Path('reversing/ProjectileCore355',name).read_text(encoding='utf-8-sig').replace('ProjectileCore355','CamCore355').replace('projectile-core355','cam-core355')
 if name=='build_probe.py':s=s.replace("'source/game_sa/ProjectileInfo.cpp', 'source/game_sa/ProjectileInfo.h', 'source/game_sa/Entity/Object/Projectile.h'","'source/game_sa/Cam.cpp', 'source/game_sa/Cam.h', 'source/game_sa/Camera.h'")
 (p/name).write_text(s,encoding='utf8')
s=Path('reversing/DecisionPending/LiveComparison.hpp').read_text().replace('DecisionPendingLive','CamCore355Live').replace('decision-pending','cam-core355').replace('#include "DecisionMakers/Decision.h"','#include "Cam.h"')
start=s.index('inline const std::array<Fn,2>');end=s.index('inline ReversibleHooks',start)
s=s[:start]+'''inline const std::array<Fn,2> fns{{
{"CCam","Using3rdPersonMouseCam",0x50A850,0,-1,1024},
{"CCam","KeepTrackOfTheSpeed",0x509DF0,0,-1,1024}
}};
'''+s[end:]
s=s.replace(' bool passed=true;uint32 pairCount=0;',''' bool passed=true;uint32 pairCount=0;
 std::array<uint8,0x34> saved{};memcpy(saved.data(),reinterpret_cast<void*>(0xB6FF5C),saved.size());const auto savedReset=*reinterpret_cast<uint8*>(0xB6F052),savedMouse=*reinterpret_cast<uint8*>(0xB6EC2E);''')
start=s.index('    alignas(CDecision)');end=s.index('\n   };',start)
s=s[:start]+'''    alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCam*>(raw.data());
    for(size_t k=0;k<12;++k)*reinterpret_cast<float*>(0xB6FF5C+k*4)=float((i*19+int(k)*7)%73-36)*.125f;
    *reinterpret_cast<uint32*>(0xB6FF8C)=uint32(i%64)|0xABCD0000u;*reinterpret_cast<uint8*>(0xB6F052)=uint8((i/64)%2);*reinterpret_cast<uint8*>(0xB6EC2E)=uint8(i%2);
    self->m_nMode=eCamMode((i/2)%12);
    const CVector source{float(i)*.25f,float(i%31),-float(i%7)},target{-float(i)*.125f,float(i%17),float(i%13)},up{float(i%19)*.0625f,float(i%23),float(i%29)};
    const float alpha=float(i%127-63)*.125f,beta=float(i%89-44)*.25f,fov=float(i%97)*.125f;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;bool result=false;
    if(fn.address==0x50A850)result=reinterpret_cast<bool(__thiscall*)(const CCam*)>(fn.address)(self);
    else reinterpret_cast<void(__thiscall*)(CCam*,const CVector&,const CVector&,const CVector&,const float&,const float&,const float&)>(fn.address)(self,source,target,up,alpha,beta,fov);
    return json{{"bytes",ReversingProbe::Bytes(raw.data(),raw.size())},{"globals",ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6FF5C),0x34)},{"result",result},{"switches",switched}};''' +s[end:]
s=s.replace(' Write({{"event",passed?', ' memcpy(reinterpret_cast<void*>(0xB6FF5C),saved.data(),saved.size());*reinterpret_cast<uint8*>(0xB6F052)=savedReset;*reinterpret_cast<uint8*>(0xB6EC2E)=savedMouse;\n Write({{"event",passed?')
(p/'LiveComparison.hpp').write_text(s,encoding='utf8')
