from pathlib import Path
p=Path('reversing/PlaneDamage355/LiveComparison.hpp');s=p.read_text();needle=' // Accepted explosive cases always have previous health .5 and reach BlowUpCar before smoke.'
insert=''' // 64 targeted nearest-node cases: identical off-axis vectors have an upward-rounded length.
 // EXE keeps squared distance extended; rounding both squares to float would wrongly retain node12.
 if(i%16==4){current->m_nModelIndex=476;current->SetStatus(STATUS_PLAYER);object[0x42A]=0x20;current->m_nPhysicalFlags=0;current->m_fMass=2000.f;current->m_fDamageIntensity=2.f;current->m_fHealth=100.f;for(int j=12;j<25;++j){frames[j].modelling.pos={0.0031638420186936855f,0.4666784703731537f,0.702342689037323f};current->m_aCarNodes[j]=&frames[j];}}
'''
s=s.replace(needle,insert+needle);p.write_text(s)
p=Path('reversing/PlaneDamage355/README.md');s=p.read_text();s+='\n64 targeted cases i%16==4 use identical off-axis vectors (0.0031638420186936855,0.4666784703731537,0.702342689037323), all13 nodes present with safe collision gates. Exact squared distance ~0.7110840574503235, rounded closest0.8432580232620239 squared ~0.7110840937957761; both rounded squares equal float0.7110840678215027. EXE therefore repeatedly selects later nodes while the former rounded implementation would retain node12. Ordered progress trace captures selected node24. Other nonaligned fixtures remain included.\n';p.write_text(s)
