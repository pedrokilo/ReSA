#pragma once
#include <fstream>
#include <bit>
#include <crtdbg.h>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "MatrixLink.h"
#include "PlayerPedData.h"
#include "../ProbeRuntime.hpp"
#include "Probe.hpp"

namespace CppConversationPositionLive {
using namespace CppConversationPositionProbe;
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log<<row.dump()<<'\n';log.flush(); }
inline int Report(int type,char* message,int*) { if(type!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",message ? message : ""}});ExitProcess(91); }
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{{"CConversationForPed","IsPlayerInPositionForConversation",0x43AC40,0,-1,2414}};
// Copia exacta de las familias de posicion de ../ConversationForPed/LiveComparison.hpp (Cases(), lineas 17-61);
// se omiten las familias de Update. El orden y los nombres (familia-indice) coinciden con el banco historico.
inline std::vector<Spec> PositionCases() {
    std::vector<Spec> cases;
    auto add=[&](Spec s,const char* family) { s.name=std::string(family)+"-"+std::to_string(cases.size());cases.push_back(s); };
    for(bool random : {false,true}) {
        for(float distance : {0.f,3.999f,std::nextafter(4.f,0.f),4.f,std::nextafter(4.f,5.f),4.001f,100.f})
            for(int axis=0;axis<3;++axis)for(float sign : {-1.f,1.f}) {
                Spec s;s.positionOnly=true;s.random=random;s.pos=CVector{0.f,0.f,0.f};s.pos[axis]=distance*sign;add(s,"distance");
            }
        for(float dot : {-1.f,-0.000001f,0.f,0.000001f,1.f}) {
            Spec s;s.positionOnly=true;s.random=random;s.forward=CVector{1.f,dot,0.f};add(s,"facing");
        }
        for(float speed : {0.f,std::nextafter(.01f,0.f),.01f,std::nextafter(.01f,1.f),.011f})
            for(int axis=0;axis<3;++axis)for(float sign : {-1.f,1.f}) {
                Spec s;s.positionOnly=true;s.random=random;s.speed[axis]=speed*sign;add(s,"speed");
            }
        for(uint32 damage : {0u,60000u,0xFFFFFF00u,0x80000000u})
            for(uint32 delta : {5999u,6000u,6001u})for(bool same : {false,true}) {
                Spec s;s.positionOnly=true;s.random=random;s.damageTime=damage;s.time=damage+delta;s.damaged=same;add(s,"damage");
            }
        for(int followers : {-1,0,1,7})for(int flags=0;flags<4;++flags) {
            Spec s;s.positionOnly=true;s.random=random;s.followers=followers;s.playerReady=flags&1;s.pedReady=flags&2;add(s,"availability");
        }
        Spec s;s.positionOnly=true;s.random=random;s.matrix=false;s.pos=CVector{5.f,0.f,0.f};add(s,"placement-early-out");
        // Mixed axes and positions expose premature float rounding at the thresholds.
        for(int i=0;i<180;++i)for(float step : {-1.f,0.f,1.f}) {
            const float angle=float(i)*.034906585f;
            Spec s;s.positionOnly=true;s.random=random;
            float radius=step<0 ? std::nextafter(4.f,0.f) : step>0 ? std::nextafter(4.f,5.f) : 4.f;
            s.pos=CVector{radius*std::cos(angle),radius*std::sin(angle),0.f};add(s,"mixed-distance");
            s.pos=CVector{0.f,2.f,0.f};radius=step<0 ? std::nextafter(.01f,0.f) : step>0 ? std::nextafter(.01f,1.f) : .01f;
            s.speed=CVector{radius*std::cos(angle),radius*std::sin(angle),.5f};add(s,"mixed-speed");
        }
    }
    for(uint32 precision : {uint32(_PC_24),uint32(_PC_53),uint32(_PC_64)})for(int test=0;test<6;++test) {
        Spec s;s.positionOnly=true;s.precision=precision;
        if(test==0)s.pos=CVector{4.f,std::ldexp(1.f,-12),0.f};
        if(test==1)s.speed=CVector{.01f,std::ldexp(1.f,-22),0.f};
        if(test==2 || test==3) {
            float tiny=std::ldexp(1.f,test==2 ? -12 : -28);
            s.playerForward=CVector{1.f,tiny,1.f};s.forward=CVector{-1.f,tiny,1.f};
        }
        if(test==4)s.pos=CVector{4.f,0.f,std::ldexp(1.f,-12)};
        if(test==5){s.playerPos=CVector{4096.f,0.f,0.f};s.pos=CVector{4092.f,std::ldexp(1.f,-12),0.f};}
        add(s,"x87-precision");
    }
    return cases;
}
// Fixture identico al historico (SetUp/Run), con la llamada fija a 0x43AC40.
struct Fixture {
    alignas(CPlayerPed) std::array<uint8,sizeof(CPlayerPed)> playerBytes{};
    alignas(CPed) std::array<uint8,sizeof(CPed)> pedBytes{};
    CMatrixLink playerMatrix{},pedMatrix{};
    CPlayerPedData data{};
    CConversationForPed conv{};
    void SetUp() {
        playerBytes.fill(0);pedBytes.fill(0);
        player=reinterpret_cast<CPlayerPed*>(playerBytes.data());ped=reinterpret_cast<CPed*>(pedBytes.data());
        playerMatrix.SetUnity();pedMatrix.SetUnity();
        playerMatrix.GetPosition()=spec.playerPos;pedMatrix.GetPosition()=spec.pos;
        playerMatrix.GetForward()=spec.playerForward;pedMatrix.GetForward()=spec.forward;
        player->m_matrix=&playerMatrix;ped->m_matrix=spec.matrix ? &pedMatrix : nullptr;ped->m_placement.m_vPosn=spec.pos;
        player->m_vecMoveSpeed=spec.speed;
        *reinterpret_cast<CPlayerPedData**>(playerBytes.data()+0x480)=&data;data.m_nPlayerGroup=0;
        ped->m_pLastEntityDamage=spec.damaged ? player : nullptr;ped->field_768=static_cast<int32>(spec.damageTime);
        conv={};conv.m_FirstNode=7;conv.m_CurrentNode=0;conv.m_pPed=spec.participant ? ped : nullptr;
        conv.m_Status=static_cast<CConversationForPed::eStatus>(spec.state);conv.m_LastChange=spec.last;conv.m_LastTimeWeWereCloseEnough=spec.close;
        conv.m_Enabled=spec.enabled;conv.m_SuppressSubtitles=spec.suppress;conversation=&conv;
        auto& nodes=CConversations::m_Nodes;
        std::memset(nodes.data(),0,sizeof(nodes));
        for(size_t i=0;i<nodes.size();++i) {
            sprintf_s(nodes[i].m_Name,"N%02u",unsigned(i));nodes[i].m_NodeYes=spec.nodeYes;nodes[i].m_NodeNo=spec.nodeNo;
            nodes[i].m_Speech=spec.speech;nodes[i].m_SpeechY=spec.speechY;nodes[i].m_SpeechN=spec.speechN;
        }
        std::memset(gString,0x5A,sizeof(gString));gString[0]=0;
        CTimer::m_snTimeInMilliseconds=spec.time;trace=json::array();
    }
    json Run() {
        SetUp();
        const auto pBefore=playerBytes;const auto nBefore=pedBytes;
        const auto nodesBefore=ReversingProbe::Bytes(CConversations::m_Nodes.data(),sizeof(CConversations::m_Nodes));
        const bool result=reinterpret_cast<bool(__thiscall*)(CConversationForPed*,bool)>(0x43AC40)(&conv,spec.random);
        return {{"result",result},{"first",conv.m_FirstNode},{"current",conv.m_CurrentNode},{"ped",Id(conv.m_pPed)},
            {"state",uint32(conv.m_Status)},{"last",conv.m_LastChange},{"close",conv.m_LastTimeWeWereCloseEnough},
            {"enabled",conv.m_Enabled},{"suppress",conv.m_SuppressSubtitles},{"string",ReversingProbe::Bytes(gString,sizeof(gString))},
            {"calls",trace},{"inputs_intact",pBefore==playerBytes && nBefore==pedBytes && nodesBefore==ReversingProbe::Bytes(CConversations::m_Nodes.data(),sizeof(CConversations::m_Nodes))}};
    }
};
inline BOOL CALLBACK CloseOwnWindow(HWND hwnd,LPARAM) {
    DWORD pid{};GetWindowThreadProcessId(hwnd,&pid);char name[128]{};GetClassNameA(hwnd,name,128);
    if(pid==GetCurrentProcessId() && std::strcmp(name,"Grand theft auto San Andreas")==0)PostMessage(hwnd,WM_CLOSE,0,0);
    return TRUE;
}
// Ruta efectiva de 0x43AC40: original = bytes de entrada originales y sin salto a la DLL; candidato = salto a la DLL.
// Sustituye a los contadores positionCalls que insertaba instrument.py en el cuerpo candidato.
inline bool Routes(bool candidate) {
    constexpr uintptr_t address=0x43AC40;
    const bool jumps=ReversingProbe::JumpsToDll(address);
    if(candidate)return jumps;
    return !jumps && ReversingProbe::Bytes(reinterpret_cast<void*>(address),5)==std::vector<uint8>{0x83,0xEC,0x0C,0x56,0x57};
}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpp-conversation-position-runtime.start")){done=true;return;}
    if(!log.is_open()){
        log.open(dir/"cpp-conversation-position-runtime.jsonl");ReversingProbe::InstallFailureCapture("cpp-conversation-position");
        Write({{"event","waiting_for_player"}});
    }
    auto* livePlayer=FindPlayerPed();if(!livePlayer||CCutsceneMgr::IsRunning())return;
    done=true;
    auto oldReport=_CrtSetReportHook(Report);
    ReversibleHooks::ReversibleHook::Base* positionHook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CConversationForPed")for(auto& h:c.Items())
        if(h->Name()=="IsPlayerInPositionForConversation")positionHook=h.get();});
    if(!positionHook){Write({{"event","error"},{"reason","hook missing"}});_CrtSetReportHook(oldReport);return;}
    const auto savedNodes=ReversingProbe::Bytes(CConversations::m_Nodes.data(),sizeof(CConversations::m_Nodes));
    const auto savedString=ReversingProbe::Bytes(gString,sizeof(gString));
    const auto liveBytes=ReversingProbe::Bytes(livePlayer,sizeof(CPlayerPed));const auto oldTime=CTimer::GetTimeInMS();
    const auto oldControl=_control87(0,0);
    const bool pEnabled=positionHook->Hooked(),pLocked=positionHook->Locked();
    positionHook->LockState(false);
    const auto cases=PositionCases();
    bool pass=cases.size()==size_t(fns[0].cases);int pairs=0,failures=0,positive=0,negative=0;
    {
        using ReversingProbe::EntryPatch;using std::bit_cast;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        auto patch=[&](uintptr_t a,auto f){patches.push_back(std::make_unique<EntryPatch>(a,f));};
        // Mismos proveedores que el banco historico (EXE) y, nuevo, los mismos en las funciones de la DLL que
        // llama el cuerpo candidato (antes se sustituian en el texto fuente).
        patch(0x56E210,&Player);patch(bit_cast<uintptr_t>(&FindPlayerPed),&Player);
        patch(0x56E010,&Coors);patch(bit_cast<uintptr_t>(&FindPlayerCoors),&Coors);
        patch(0x43ABA0,&ReadyABI);patch(bit_cast<uintptr_t>(&CPed::PedIsReadyForConversation),&ReadyABI);
        patch(0x5F6AA0,&MembersABI);patch(bit_cast<uintptr_t>(&CPedGroupMembership::CountMembersExcludingLeader),&MembersABI);
        Fixture a,b;
        for(size_t i=0;i<cases.size();++i) {
            const auto& s=cases[i];
            auto run=[&](bool candidate,Fixture& f) {
                spec=s;_control87(s.precision==0xFFFFFFFFu ? oldControl : s.precision,_MCW_PC);
                positionHook->State(candidate);
                const bool before=Routes(candidate);
                auto row=f.Run();
                row["switches"]=before && Routes(candidate);
                row["fp_control"]=_control87(0,0);
                return row;
            };
            Write({{"event","begin"},{"pair",pairs},{"function","CConversationForPed::IsPlayerInPositionForConversation"},{"case",i},{"name",s.name}});
            auto original=run(false,a);auto candidate=run(true,b);
            const bool contract=original["switches"]==true && candidate["switches"]==true
                && original["inputs_intact"]==true && candidate["inputs_intact"]==true;
            const bool equal=original==candidate && contract;
            pass&=equal;failures+=!equal;
            if(original["result"]==true)++positive;else ++negative;
            Write({{"event","comparison"},{"function","CConversationForPed::IsPlayerInPositionForConversation"},{"pair",pairs++},{"case",i},{"name",s.name},
                {"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
        }
    }
    positionHook->State(pEnabled);positionHook->LockState(pLocked);
    std::memcpy(CConversations::m_Nodes.data(),savedNodes.data(),savedNodes.size());std::memcpy(gString,savedString.data(),savedString.size());
    CTimer::m_snTimeInMilliseconds=oldTime;_control87(oldControl,_MCW_PC);
    const bool restored=savedNodes==ReversingProbe::Bytes(CConversations::m_Nodes.data(),sizeof(CConversations::m_Nodes))
        && savedString==ReversingProbe::Bytes(gString,sizeof(gString)) && liveBytes==ReversingProbe::Bytes(livePlayer,sizeof(CPlayerPed))
        && CTimer::GetTimeInMS()==oldTime && _control87(0,0)==oldControl && positionHook->Hooked()==pEnabled && positionHook->Locked()==pLocked;
    pass&=restored && positive>0 && negative>0;
    Write({{"event","function_summary"},{"function","CConversationForPed::IsPlayerInPositionForConversation"},{"cases",pairs},{"failures",failures},{"positive",positive},{"negative",negative}});
    Write({{"event",pass ? "complete":"error"},{"pairs",pairs},{"expected",cases.size()},{"state_restored",restored}});
    _CrtSetReportHook(oldReport);EnumWindows(CloseOwnWindow,0);
}
}
