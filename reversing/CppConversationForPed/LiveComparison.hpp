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

namespace CppConversationForPedLive {
using namespace CppConversationForPedProbe;
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log<<row.dump()<<'\n';log.flush(); }
inline int Report(int type,char* message,int*) { if(type!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",message ? message : ""}});ExitProcess(91); }
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline Fn fns[]{{"CConversationForPed","IsPlayerInPositionForConversation",0x43AC40,0,-1,2414},{"CConversationForPed","Update",0x43C190,0,-1,2288}};
// Copia exacta de las familias de posicion de ../ConversationForPed/LiveComparison.hpp (Cases(), lineas 21-62).
// El orden y los nombres (familia-indice global) coinciden con el banco historico.
inline void PositionCases(std::vector<Spec>& cases) {
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
}
// Copia exacta de las familias de Update del banco historico (lineas 63-80). Siguen a las de posicion con el mismo
// contador global, como en el historico.
inline void UpdateCases(std::vector<Spec>& cases) {
    auto add=[&](Spec s,const char* family) { s.name=std::string(family)+"-"+std::to_string(cases.size());cases.push_back(s); };
    for(uint32 state : {0u,1u,2u,3u,0xFFFFFFFFu})for(uint32 base : {0u,60000u,0xFFFFFF00u})
        for(uint32 delta : {0u,399u,400u,401u,3999u,4000u,4001u,9999u,10000u,10001u})for(int flags=0;flags<8;++flags) {
            Spec s;s.state=state;s.last=base;s.close=base;s.time=base+delta;s.yes=flags&1;s.no=flags&2;s.suppress=flags&4;add(s,"state-timer");
        }
    for(int16 y : {-1,0,1,49})for(int16 n : {-1,0,2,49})for(int flags=0;flags<8;++flags)
        for(int speech : {-700,-1,0,42,32768,65535,65536,65537}) {
            Spec s;s.nodeYes=y;s.nodeNo=n;s.yes=flags&1;s.no=flags&2;s.suppress=flags&4;s.speechY=speech;s.speechN=speech;add(s,"reply-branches");
        }
    for(int speech : {-700,-1,0,42,32768,65535,65536,65537})for(int flags=0;flags<4;++flags) {
        Spec s;s.close=0;s.speech=speech;s.suppress=flags&1;s.state=flags&2 ? 1u : 0u;add(s,"question-audio");
    }
    for(uint32 state=0;state<4;++state)for(int gate=0;gate<8;++gate) {
        Spec s;s.state=state;s.close=0;
        if(gate==0)s.participant=false;if(gate==1)s.enabled=false;if(gate==2)s.pos=CVector{5.f,0.f,0.f};
        if(gate==3)s.forward=CVector{0.f,1.f,0.f};if(gate==4)s.speed=CVector{.011f,0.f,0.f};
        if(gate==5){s.damaged=true;s.damageTime=s.time;}if(gate==6)s.playerReady=false;if(gate==7)s.pedReady=false;
        add(s,"inactive-gates");
    }
}
inline std::vector<Spec> Cases() {
    std::vector<Spec> cases;
    PositionCases(cases);
    UpdateCases(cases);
    return cases;
}
// Fixture identico al historico (SetUp/Run). Cambio inevitable: ConversationYesJustDown/NoJustDown son inline
// en Pad.h y el candidato /O2 las incrusta, asi que no hay funcion de la DLL que sustituir. Se fija el estado
// real de CPad::Pads[0] (identico para original y candidato) y desaparecen las trazas "yes"/"no" de ambos lados.
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
        // Tecla "pulsada ahora" = Nuevo && !Viejo (Pad.h IsDPadRightPressed/IsDPadLeftPressed); controles habilitados.
        auto& pad=*CPad::GetPad(0);
        pad.DisablePlayerControls=0;
        pad.OldState.DPadRight=0;pad.OldState.DPadLeft=0;
        pad.NewState.DPadRight=spec.yes ? 255 : 0;pad.NewState.DPadLeft=spec.no ? 255 : 0;
        std::memset(gString,0x5A,sizeof(gString));gString[0]=0;
        CTimer::m_snTimeInMilliseconds=spec.time;trace=json::array();
    }
    json Run() {
        SetUp();
        const auto pBefore=playerBytes;const auto nBefore=pedBytes;
        const auto nodesBefore=ReversingProbe::Bytes(CConversations::m_Nodes.data(),sizeof(CConversations::m_Nodes));
        const auto padBefore=ReversingProbe::Bytes(CPad::GetPad(0),sizeof(CPad));
        bool result{};
        if(spec.positionOnly)result=reinterpret_cast<bool(__thiscall*)(CConversationForPed*,bool)>(0x43AC40)(&conv,spec.random);
        else reinterpret_cast<void(__thiscall*)(CConversationForPed*)>(0x43C190)(&conv);
        return {{"result",result},{"first",conv.m_FirstNode},{"current",conv.m_CurrentNode},{"ped",Id(conv.m_pPed)},
            {"state",uint32(conv.m_Status)},{"last",conv.m_LastChange},{"close",conv.m_LastTimeWeWereCloseEnough},
            {"enabled",conv.m_Enabled},{"suppress",conv.m_SuppressSubtitles},{"string",ReversingProbe::Bytes(gString,sizeof(gString))},
            {"calls",trace},{"inputs_intact",pBefore==playerBytes && nBefore==pedBytes && nodesBefore==ReversingProbe::Bytes(CConversations::m_Nodes.data(),sizeof(CConversations::m_Nodes))
                && padBefore==ReversingProbe::Bytes(CPad::GetPad(0),sizeof(CPad))}};
    }
};
inline BOOL CALLBACK CloseOwnWindow(HWND hwnd,LPARAM) {
    DWORD pid{};GetWindowThreadProcessId(hwnd,&pid);char name[128]{};GetClassNameA(hwnd,name,128);
    if(pid==GetCurrentProcessId() && std::strcmp(name,"Grand theft auto San Andreas")==0)PostMessage(hwnd,WM_CLOSE,0,0);
    return TRUE;
}
// Ruta efectiva de AMBAS entradas (los dos hooks se conmutan juntos, como en el historico): original = bytes de
// entrada originales y sin salto a la DLL; candidato = salto a la DLL. Sustituye a updateCalls/positionCalls
// que insertaba instrument.py en el cuerpo candidato.
inline bool Routes(bool candidate) {
    struct Entry{uintptr_t address;std::vector<uint8> original;};
    static const Entry entries[]{{0x43AC40,{0x83,0xEC,0x0C,0x56,0x57}},{0x43C190,{0x56,0x8B,0xF1,0x8B,0x46}}};
    for(const auto& e : entries) {
        const bool jumps=ReversingProbe::JumpsToDll(e.address);
        if(candidate){if(!jumps)return false;}
        else if(jumps || ReversingProbe::Bytes(reinterpret_cast<void*>(e.address),5)!=e.original)return false;
    }
    return true;
}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpp-conversation-for-ped-runtime.start")){done=true;return;}
    if(!log.is_open()){
        log.open(dir/"cpp-conversation-for-ped-runtime.jsonl");ReversingProbe::InstallFailureCapture("cpp-conversation-for-ped");
        Write({{"event","waiting_for_player"}});
    }
    auto* livePlayer=FindPlayerPed();if(!livePlayer||CCutsceneMgr::IsRunning())return;
    done=true;
    auto oldReport=_CrtSetReportHook(Report);
    ReversibleHooks::ReversibleHook::Base *updateHook{},*positionHook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CConversationForPed")for(auto& h:c.Items()) {
        if(h->Name()=="Update")updateHook=h.get();if(h->Name()=="IsPlayerInPositionForConversation")positionHook=h.get();
    }});
    if(!updateHook || !positionHook){Write({{"event","error"},{"reason","hook missing"}});_CrtSetReportHook(oldReport);return;}
    const auto savedNodes=ReversingProbe::Bytes(CConversations::m_Nodes.data(),sizeof(CConversations::m_Nodes));
    const auto savedString=ReversingProbe::Bytes(gString,sizeof(gString));
    const auto savedPad=ReversingProbe::Bytes(CPad::GetPad(0),sizeof(CPad));
    const auto liveBytes=ReversingProbe::Bytes(livePlayer,sizeof(CPlayerPed));const auto oldTime=CTimer::GetTimeInMS();
    const auto oldControl=_control87(0,0);
    const bool uEnabled=updateHook->Hooked(),uLocked=updateHook->Locked(),pEnabled=positionHook->Hooked(),pLocked=positionHook->Locked();
    updateHook->LockState(false);positionHook->LockState(false);
    const auto cases=Cases();
    int expectedTotal=0;for(const auto& f : fns)expectedTotal+=f.cases;
    bool pass=cases.size()==size_t(expectedTotal);
    int pairs=0;
    int perCases[2]{},perFailures[2]{},perPositive[2]{},perNegative[2]{};
    {
        using ReversingProbe::EntryPatch;using std::bit_cast;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        auto patch=[&](uintptr_t a,auto f){patches.push_back(std::make_unique<EntryPatch>(a,f));};
        // Mismos proveedores que el banco historico (EXE) y los mismos en las funciones de la DLL que llama el
        // cuerpo candidato (antes se sustituian en el texto fuente). Sin Yes/No: ver Fixture.
        patch(0x56E210,&Player);patch(bit_cast<uintptr_t>(&FindPlayerPed),&Player);
        patch(0x56E010,&Coors);patch(bit_cast<uintptr_t>(&FindPlayerCoors),&Coors);
        patch(0x43ABA0,&ReadyABI);patch(bit_cast<uintptr_t>(&CPed::PedIsReadyForConversation),&ReadyABI);
        patch(0x5F6AA0,&MembersABI);patch(bit_cast<uintptr_t>(&CPedGroupMembership::CountMembersExcludingLeader),&MembersABI);
        patch(0x69DD30,&ClearMessages);patch(bit_cast<uintptr_t>(&CMessages::ClearSmallMessagesOnly),&ClearMessages);
        patch(0x6A0050,&TextABI);patch(bit_cast<uintptr_t>(&CText::Get),&TextABI);
        patch(0x69F1E0,&Jump);patch(bit_cast<uintptr_t>(&CMessages::AddMessageJump),&Jump);
        patch(0x69F0B0,&CppConversationForPedProbe::Queue);patch(bit_cast<uintptr_t>(&CMessages::AddMessageQ),&CppConversationForPedProbe::Queue);
        patch(0x5EFFE0,&SayABI);patch(bit_cast<uintptr_t>(&CPed::Say),&SayABI);
        patch(0x43A810,&Awkward);patch(bit_cast<uintptr_t>(&CConversations::AwkwardSay),&Awkward);
        Fixture a,b;
        for(size_t i=0;i<cases.size();++i) {
            const auto& s=cases[i];
            const int fi=s.positionOnly ? 0 : 1;
            const char* qualified=fi==0 ? "CConversationForPed::IsPlayerInPositionForConversation" : "CConversationForPed::Update";
            const int caseIndex=perCases[fi];
            auto run=[&](bool candidate,Fixture& f) {
                spec=s;_control87(s.precision==0xFFFFFFFFu ? oldControl : s.precision,_MCW_PC);
                updateHook->State(candidate);positionHook->State(candidate);
                const bool before=Routes(candidate);
                auto row=f.Run();
                row["switches"]=before && Routes(candidate);
                row["fp_control"]=_control87(0,0);
                return row;
            };
            Write({{"event","begin"},{"pair",pairs},{"function",qualified},{"case",caseIndex},{"name",s.name}});
            auto original=run(false,a);auto candidate=run(true,b);
            const bool contract=original["switches"]==true && candidate["switches"]==true
                && original["inputs_intact"]==true && candidate["inputs_intact"]==true;
            const bool equal=original==candidate && contract;
            pass&=equal;++perCases[fi];perFailures[fi]+=!equal;
            bool positive{};
            if(fi==0)positive=original["result"]==true;
            else positive=original["state"]!=s.state || original["current"]!=0 || original["last"]!=s.last || original["close"]!=s.close || !original["calls"].empty();
            if(positive)++perPositive[fi];else ++perNegative[fi];
            Write({{"event","comparison"},{"function",qualified},{"pair",pairs++},{"case",caseIndex},{"name",s.name},
                {"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
        }
    }
    updateHook->State(uEnabled);updateHook->LockState(uLocked);positionHook->State(pEnabled);positionHook->LockState(pLocked);
    std::memcpy(CConversations::m_Nodes.data(),savedNodes.data(),savedNodes.size());std::memcpy(gString,savedString.data(),savedString.size());
    std::memcpy(CPad::GetPad(0),savedPad.data(),savedPad.size());
    CTimer::m_snTimeInMilliseconds=oldTime;_control87(oldControl,_MCW_PC);
    const bool restored=savedNodes==ReversingProbe::Bytes(CConversations::m_Nodes.data(),sizeof(CConversations::m_Nodes))
        && savedString==ReversingProbe::Bytes(gString,sizeof(gString)) && savedPad==ReversingProbe::Bytes(CPad::GetPad(0),sizeof(CPad))
        && liveBytes==ReversingProbe::Bytes(livePlayer,sizeof(CPlayerPed))
        && CTimer::GetTimeInMS()==oldTime && _control87(0,0)==oldControl
        && updateHook->Hooked()==uEnabled && updateHook->Locked()==uLocked && positionHook->Hooked()==pEnabled && positionHook->Locked()==pLocked;
    for(int fi=0;fi<2;++fi) {
        pass&=perCases[fi]==fns[fi].cases && perPositive[fi]>0 && perNegative[fi]>0;
        Write({{"event","function_summary"},{"function",std::string(fns[fi].cls)+"::"+fns[fi].name},{"cases",perCases[fi]},{"failures",perFailures[fi]},
            {"positive",perPositive[fi]},{"negative",perNegative[fi]}});
    }
    pass&=restored;
    Write({{"event",pass ? "complete":"error"},{"pairs",pairs},{"expected",cases.size()},{"state_restored",restored}});
    _CrtSetReportHook(oldReport);EnumWindows(CloseOwnWindow,0);
}
}
