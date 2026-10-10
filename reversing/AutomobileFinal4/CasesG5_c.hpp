#pragma once
// Grupo G5 (parte c): banco de CAutomobile::ProcessAI (0x6B4800, ranura 66 de la vtable).
#include "CasesG5_b.hpp"

namespace AutomobileFinal4Live {

inline void G5RunAI(Ctx& c){
    G5Reset(); G5Globals(c,true);
    for(int k=3;k>=1;--k){ c.BuildBase(k,c.carModel()); At<uint8>(Cr(k),G5o::Status)=uint8((c.below(13)<<3)|2);
        for(int q=0;q<3;++q) At<float>(Cr(k),G5o::MoveSpd+4u*unsigned(q))=c.chance(2) ? 0.f : c.uni(-0.3f,0.3f); }
    static constexpr uint32 kStat[13]{0,3,4,5,8,9,10,12,2,0,3,8,4};
    const uint32 th = uint32(c.i)%16u;                                                       // tema: fuerza combinaciones raras
    const bool sp = c.chance(3);
    int model = sp ? (c.chance(2) ? 441 : kG5Special[c.below(uint32(std::size(kG5Special)))]) : c.carModel();
    if(th==7) model = 441;                                                                   // rcbandit
    else if(th==1||th==3||th==5||th==9||th==11||th==15) { while(model==420||model==438||model==441||model==495||model==424) model = c.carModel(); }
    const uint32 sub = (model==447||model==417) ? 3u : (model==460||model==464) ? 4u : c.chance(14) ? 3u : c.chance(14) ? 4u : c.chance(6) ? 1u : 0u;
    c.BuildBase(0,model,c.chance(4) ? 0.0f : 0.3f); uint8* car=Cr(0);
    G5Handling(c,0,int(c.below(100)));
    const uint32 st = (c.i%2==0) ? kStat[(c.i/2)%13] : (c.chance(14) ? uint32(c.pick<int>({1,6,7,11})) : kStat[c.below(13)]);
    G5ColModel(c,0,c.chance(4) ? 0u : c.chance(3) ? 1u : 4u, false);
    if(c.chance(10)) At<void*>(w.colm[0],0x2C)=nullptr;                                    // modelo de colision sin datos
    G5FillCar(c,car,st,model,sub,true);
    // --- lo especifico de ProcessAI ---
    At<int8>(car,G5o::RecId)= c.chance(3) ? int8(c.below(16)) : int8(-1);
    At<uint8>(car,G5o::ApCtrl)=uint8(c.next());                                           // banderas del piloto automatico (los dos primeros bits se limpian)
    At<float>(car,G5o::ApSpeed)=c.uni(0.f,60.f);
    At<uint8>(car,G5o::Mission)=uint8(c.chance(2) ? c.pick<int>({1,1,2,3,4,5,0}) : int(c.below(60)));
    At<float>(car,G5o::SpringLen)=c.uni(0.1f,0.5f);
    {   // aparcar: (contador de fotogramas + semilla) & 0xF == 0 en la mitad de los casos
        const uint32 seed=At<uint8>(car,G5o::Seed); uint32 fr=c.next()&~0xFu;
        if(c.chance(2)) fr += (16u-(seed&0xFu))&0xFu; AtA<uint32>(0xB7CB4C)=fr; }
    // jugador: nivel de busqueda, vehiculo, velocidad y posicion
    At<int32>(g5aux,0x100+0x2C)=int32(c.below(7));
    g5.pPed=g5ped[3]; At<uint32>(g5ped[3],0x598)=0;
    const uint32 pv=c.below(6); g5.pVeh = pv<3 ? static_cast<void*>(Cr(1)) : pv==3 ? static_cast<void*>(car) : nullptr;
    { const float m=c.pick<float>({0.1f,0.29f,0.35f,0.45f,0.6f}); g5.pSpeed[0]=m; g5.pSpeed[1]=c.uni(-0.05f,0.05f); g5.pSpeed[2]=c.uni(-0.02f,0.02f); }
    { auto* ms=reinterpret_cast<float*>(car+G5o::MoveSpd); const float m=c.pick<float>({0.0f,0.1f,0.25f,0.35f}); ms[0]=m; ms[1]=c.uni(-0.05f,0.05f); ms[2]=0.f; }
    g5.pCoors[0]=At<float>(car,4)+c.pick<float>({0.f,10.f,60.f,-70.f}); g5.pCoors[1]=At<float>(car,8)+c.uni(-5.f,5.f); g5.pCoors[2]=At<float>(car,12);
    g5.plosRet=c.chance(2); g5.plosHit = c.chance(3) ? static_cast<void*>(car) : c.chance(2) ? static_cast<void*>(Cr(1)) : nullptr;
    g5.remoteSlot=c.pick<int32>({-1,0,1,0});
    g5.taskMask=c.below(8); g5.fat=c.uni(0.5f,1.5f);
    g5.gunDown=c.chance(3); g5.handbrake=c.chance(3); g5.accel=c.chance(2); g5.rcHit=c.chance(3);
    g5.steerLR=c.pick<int32>({0,0,12,-12,64,-64,127,-128,3,-3}); g5.steerUD=c.pick<int32>({0,0,12,-12,64,-64,127,-128,3,-3});
    g5.blowWrecks=c.chance(2); g5.takeChanges=c.chance(2); g5.takeTarget=car;
    g5.soilOr=c.chance(3) ? c.pick<uint32>({1u,2u,8u}) : 0u;
    // mundo de jugadores: vehiculo remoto y contador de ruedas
    AtA<void*>(0xB7CD98+0xB0)=c.chance(3) ? static_cast<void*>(car) : c.chance(2) ? static_cast<void*>(Cr(1)) : nullptr;
    AtA<uint32>(0xB7CD98+0x100)=c.chance(2) ? c.below(400) : 500u+c.below(1500);
    // --- temas (indice de caso % 16) ---
    {
        auto SetSt=[&](uint32 s){ At<uint8>(car,G5o::Status)=uint8((s<<3)|2); };
        auto Fl=[&](size_t o,float x){ At<float>(car,o)=x; };
        auto* v=reinterpret_cast<CAutomobile*>(car);
        switch(th){
        case 1:  SetSt(3); g5.pVeh=Cr(1); At<int32>(g5aux,0x100+0x2C)=int32(4+c.below(3)); At<uint8>(car,G5o::Mission)=uint8(2+c.below(4));      // embestir al jugador
                 g5.pSpeed[0]=0.5f; g5.pSpeed[1]=0.f; g5.pSpeed[2]=0.f; { auto* ms=reinterpret_cast<float*>(car+G5o::MoveSpd); ms[0]=c.chance(2) ? 0.35f : 0.1f; ms[1]=0.f; ms[2]=0.f; }
                 g5.pCoors[0]=At<float>(car,4)+(c.chance(2) ? 60.f : 10.f); g5.pCoors[1]=At<float>(car,8); g5.pCoors[2]=At<float>(car,12); break;
        case 3:  SetSt(3); v->vehicleFlags.bCanPark=true; v->vehicleFlags.bParking=false; At<uint8>(car,G5o::CreatedBy)=uint8(c.below(2)); At<uint8>(car,G5o::Mission)=1;
                 { const uint32 seed=At<uint8>(car,G5o::Seed); AtA<uint32>(0xB7CB4C)=(c.next()&~0xFu)+((16u-(seed&0xFu))&0xFu); }
                 g5.plosRet=c.chance(2); g5.plosHit=c.chance(2) ? static_cast<void*>(car) : static_cast<void*>(Cr(1)); g5.pVeh=nullptr; break;    // aparcar
        case 5:  SetSt(c.chance(2) ? 3u : 12u); AtA<float>(0xC81308)=c.chance(2) ? 0.f : c.uni(0.1f,0.9f); v->vehicleFlags.bIsBeingCarJacked=c.chance(4); g5.pVeh=nullptr;   // arena
                 for(int q=0;q<4;++q){ Fl(G5o::Comp+4u*unsigned(q),0.5f); At<uint8>(car,G5o::WheelCol+0x2Cu*unsigned(q)+0x23)=g5NumSand ? g5Sand[c.below(uint32(g5NumSand))] : uint8(28); } break;
        case 7:  SetSt(8); g5.pVeh=nullptr; AtA<uint8>(0xC1CC00)=c.chance(3) ? 1 : 0; AtA<uint8>(0xC1CC01)=c.chance(3) ? 1 : 0;                                               // rcbandit remoto
                 AtA<void*>(0xB7CD98+0xB0)=c.chance(2) ? static_cast<void*>(car) : nullptr; break;
        case 9:  SetSt(0); G5InitPed(0,0u,uint32(c.below(40))); At<void*>(car,G5o::Driver)=g5ped[0]; At<uint8>(car,G5o::Contact)=0; At<uint32>(car,G5o::SubType)=0;      // jugador sin ruedas en el suelo
                 AtA<uint32>(0xB7CD98+0x100)=500u+c.below(1500); g5.pVeh=nullptr; break;
        case 11: SetSt(9); G5InitPed(0,0u,c.chance(3) ? 0x3Fu : uint32(c.below(40))); At<void*>(car,G5o::Driver)=g5ped[0]; g5.taskMask=c.below(8);                            // parada forzosa
                 { auto* ms=reinterpret_cast<float*>(car+G5o::MoveSpd); ms[0]=0.2f; ms[1]=0.f; ms[2]=0.f; } break;
        case 13: SetSt(2); g5.pVeh=nullptr; break;                                                                                                                                  // simple
        case 15: SetSt(c.chance(2) ? 0u : 8u); G5InitPed(0,0u,uint32(c.below(40))); At<void*>(car,G5o::Driver)=g5ped[0]; At<uint8>(car,G5o::Contact)=0; At<uint32>(car,G5o::SubType)=0;
                 AtA<uint8>(0xB6EC2E)=1; AtA<float>(0xB73424)=c.uni(-120.f,120.f); AtA<float>(0xB73428)=c.uni(-120.f,120.f); g5.steerLR=0; g5.steerUD=c.chance(2) ? 0 : 3;       // raton
                 AtA<uint32>(0xB7CD98+0x100)=c.chance(2) ? 0u : 500u+c.below(1500); g5.pVeh=nullptr; break;
        default: break;
        }
    }
    G5SetVirtuals();
    uint32 extra = c.chance(2) ? 0u : c.pick<uint32>({1u,2u,8u,0x10u,3u,9u});
    const uint32 extra0=extra;
    uint32 r=0;
    { auto P=MakePatches(G_RNG,5,G5_ALL); r=CallThis<uint8>(c.a,car,&extra); }
    c.ret=bool(r&0xFFu);
    c.out["extra"]=extra; c.out["extra0"]=extra0; c.out["g5"]=G5Fnv();
}
}
