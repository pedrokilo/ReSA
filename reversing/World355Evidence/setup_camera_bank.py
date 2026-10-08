from pathlib import Path
p=Path('reversing/CameraLinear355');p.mkdir(exist_ok=True)
for name in ['build_probe.py','run_local.py']:
 s=Path('reversing/ProjectileCore355',name).read_text(encoding='utf-8-sig').replace('ProjectileCore355','CameraLinear355').replace('projectile-core355','camera-linear355')
 if name=='build_probe.py':s=s.replace("'source/game_sa/ProjectileInfo.cpp', 'source/game_sa/ProjectileInfo.h', 'source/game_sa/Entity/Object/Projectile.h'","'source/game_sa/Camera.cpp', 'source/game_sa/Camera.h'")
 (p/name).write_text(s,encoding='utf8')
s=Path('reversing/DecisionPending/LiveComparison.hpp').read_text().replace('DecisionPendingLive','CameraLinear355Live').replace('decision-pending','camera-linear355').replace('#include "DecisionMakers/Decision.h"','#include "Camera.h"')
start=s.index('inline const std::array<Fn,2>');end=s.index('inline ReversibleHooks',start)
names=[('VectorMoveLinear',0x50D160),('VectorTrackLinear',0x50D1D0),('ProcessVectorTrackLinear-0',0x50D350),('ProcessVectorTrackLinear-1',0x516440),('ProcessVectorMoveLinear-0',0x50D430),('ProcessVectorMoveLinear-1',0x5164A0),('ProcessFOVLerp-0',0x50D510)]
s=s[:start]+'inline const std::array<Fn,7> fns{{\n'+''.join('{"CCamera","%s",0x%X,0,-1,1024},\n'%(n,a) for n,a in names)+'}};\n'+s[end:]
s=s.replace(' bool passed=true;uint32 pairCount=0;',' bool passed=true;uint32 pairCount=0;const auto savedTime=CTimer::GetTimeInMS();')
start=s.index('    alignas(CDecision)');end=s.index('\n   };',start)
s=s[:start]+'''    alignas(CCamera) std::array<uint8,sizeof(CCamera)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCamera*>(raw.data());
    constexpr std::array<uint32,8> times{0,1,1000,0x7FFFFF,0x7FFFFFFF,0x80000000,0xFFFFFF00,0xFFFFFFFF};
    const auto time=times[i%times.size()];*reinterpret_cast<uint32*>(0xB7CB84)=time;
    const float now=float(time);
    self->m_fTrackLinearStartTime=now-float(i%7)*10.0f;self->m_fTrackLinearEndTime=now+float(i%5-2)*10.0f;
    self->m_fMoveLinearStartTime=self->m_fTrackLinearStartTime;self->m_fMoveLinearEndTime=self->m_fTrackLinearEndTime;
    self->m_vecTrackLinearEndPoint=CVector{float(i)*.125f,-float(i%31),float(i%19)};self->m_vecTrackLinearStartPoint=CVector{-float(i%73),float(i)*.0625f,float(i%29)};
    self->m_vecMoveLinearPosnStart=self->m_vecTrackLinearEndPoint;self->m_vecMoveLinearPosnEnd=self->m_vecTrackLinearStartPoint;
    self->m_bTrackLinearWithEase=self->m_bMoveLinearWithEase=i%2!=0;self->m_nZoomMode=uint8(i%3);
    self->m_fZoomInFactor=float(i%73)-10.f;self->m_fZoomOutFactor=float(i%37)*1.125f;
    self->m_bVecTrackLinearProcessed=self->m_bVecMoveLinearProcessed=self->m_bFOVLerpProcessed=false;
    self->m_bCameraPersistPosition=(i/2)%2!=0;self->m_bCameraPersistTrack=(i/4)%2!=0;
    CVector from=self->m_vecTrackLinearEndPoint,to=self->m_vecTrackLinearStartPoint;
    const float ratio=float(i%97-24)/48.f;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    if(fn.address==0x50D160||fn.address==0x50D1D0)reinterpret_cast<void(__thiscall*)(CCamera*,CVector*,CVector*,float,bool)>(fn.address)(self,&to,&from,float(i%31-5)*20.f,i%2!=0);
    else if(fn.address==0x516440||fn.address==0x5164A0)reinterpret_cast<void(__thiscall*)(CCamera*)>(fn.address)(self);
    else reinterpret_cast<void(__thiscall*)(CCamera*,float)>(fn.address)(self,ratio);
    return json{{"bytes",ReversingProbe::Bytes(raw.data()+0xC5C,0xD00-0xC5C)},{"from",ReversingProbe::Bytes(&from,sizeof(from))},{"to",ReversingProbe::Bytes(&to,sizeof(to))},{"time",CTimer::GetTimeInMS()},{"switches",switched}};''' +s[end:]
s=s.replace(' Write({{"event",passed?', ' *reinterpret_cast<uint32*>(0xB7CB84)=savedTime;\n Write({{"event",passed?')
(p/'LiveComparison.hpp').write_text(s,encoding='utf8')
