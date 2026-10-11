#pragma once
#include <Windows.h>
#include <array>
#include <cstdint>

#if !defined(_M_IX86)
#error This observer requires an x86 GTA process and x86 CONTEXT.
#endif

namespace DependenciesMemoryCpp16Live {
// Instrumentación exclusivamente. Nunca se incluye en fuente de producción.
struct HardwareEvent {
    DWORD entry{}, eip{}, esp{}, returnAddress{}, argument0{}, argument1{};
    DWORD ownerThread{}, scopeSerial{}, sequence{}, dr6{}, dr7{};
    bool stackValid{}, contextPreserved{};
};
struct ExpectedEvent {DWORD entry{}, argument0{}, argument1{};};
struct DebugSnapshot {DWORD dr0{}, dr1{}, dr2{}, dr3{}, dr6{}, dr7{};};

class HardwareTrace {
    // Intel SDM 18.2.3: reserved-one bits plus inactive BLD/RTM reasons.
    // Windows may expose zero for a thread whose debug registers are disabled.
    // Only our armed context is initialized; saved/restored values stay raw.
    static constexpr DWORD IdleDebugStatus=0xFFFF0FF0u;
    enum Command : LONG { Arm=1, Verify=2, Restore=3, Exit=4, CrossThreadCall=5 };
    inline static HardwareTrace* current{};
    HANDLE ownerHandle{}, worker{}, request{}, finished{};
    PVOID handlerToken{};
    volatile LONG command{}, workerOK{}, scopeActive{}, rejectionPending{};
    DWORD owner{}, stackLow{}, stackHigh{};
    bool savedValid{}, configured{}, workerClosed{}, handlerRemoved{}, registersRestored{};
    DebugSnapshot saved{}, armed{}, lastReadback{};
    DWORD apiError{}, outsideHits{}, foreignHits{};
    unsigned expectedCount{}, count{};
    DWORD serial{}, rejectedEip{}, rejectedEsp{}, rejectedSerial{};
    bool rejected{}, overflow{}, foreign{}, contextOK{true};
    std::array<ExpectedEvent, 2> expected{};
    std::array<HardwareEvent, 16> events{};

    static DebugSnapshot Snapshot(const CONTEXT& c) {
        return {c.Dr0,c.Dr1,c.Dr2,c.Dr3,c.Dr6,c.Dr7};
    }
    static bool Same(const DebugSnapshot& a,const DebugSnapshot& b,bool allowOwnStatus=false) {
        return a.dr0==b.dr0&&a.dr1==b.dr1&&a.dr2==b.dr2&&a.dr3==b.dr3&&a.dr7==b.dr7
            &&(allowOwnStatus?(a.dr6&~7u)==(b.dr6&~7u):a.dr6==b.dr6);
    }
    static void Put(CONTEXT& c,const DebugSnapshot& s) {
        c.ContextFlags=CONTEXT_DEBUG_REGISTERS;
        c.Dr0=s.dr0;c.Dr1=s.dr1;c.Dr2=s.dr2;c.Dr3=s.dr3;c.Dr6=s.dr6;c.Dr7=s.dr7;
    }
    static std::uint32_t ProtectedHash(const CONTEXT& c) {
        std::uint32_t hash=2166136261u;
        const DWORD words[]{c.ContextFlags,c.SegGs,c.SegFs,c.SegEs,c.SegDs,c.Edi,c.Esi,c.Ebx,
            c.Edx,c.Ecx,c.Eax,c.Ebp,c.Eip,c.SegCs,c.EFlags&~0x10000u,c.Esp,c.SegSs};
        for(auto word:words){hash^=word;hash*=16777619u;}
        const auto* floating=reinterpret_cast<const unsigned char*>(&c.FloatSave);
        for(unsigned i=0;i<sizeof(c.FloatSave);++i){hash^=floating[i];hash*=16777619u;}
        for(auto byte:c.ExtendedRegisters){hash^=byte;hash*=16777619u;}
        return hash;
    }
    bool WorkerOperation(LONG operation) {
        const DWORD prior=SuspendThread(ownerHandle);
        if(prior==DWORD(-1)){apiError=GetLastError();return false;}
        bool ok=prior==0;
        CONTEXT c{};c.ContextFlags=CONTEXT_DEBUG_REGISTERS|CONTEXT_CONTROL;
        if(ok&&!GetThreadContext(ownerHandle,&c)){apiError=GetLastError();ok=false;}
        if(ok&&operation==Arm){
            saved=Snapshot(c);savedValid=true;
            // Never steal a debugger's slots, TF, GD, or pending debug reason.
            ok=!(c.Dr7&0x20FFu)&&!(c.Dr6&0xE00Fu)&&!(c.EFlags&0x100u);
            if(ok){
                armed=saved;armed.dr0=0x824257;armed.dr1=0x824269;armed.dr2=0x82413F;
                armed.dr6=IdleDebugStatus;armed.dr7=(saved.dr7&~0x0FFF00FFu)|0x15u;
                Put(c,armed);
                if(!SetThreadContext(ownerHandle,&c)){apiError=GetLastError();ok=false;}
                else configured=true;
            }
        }else if(ok&&operation==Restore){
            ok=savedValid;
            if(ok){Put(c,saved);if(!SetThreadContext(ownerHandle,&c)){apiError=GetLastError();ok=false;}}
        }
        if(ok){
            CONTEXT after{};after.ContextFlags=CONTEXT_DEBUG_REGISTERS|CONTEXT_CONTROL;
            if(!GetThreadContext(ownerHandle,&after)){apiError=GetLastError();ok=false;}
            else{lastReadback=Snapshot(after);ok=Same(lastReadback,operation==Restore?saved:armed,operation!=Restore)&&!(after.EFlags&0x100u);}
        }
        if(operation==Restore&&ok){registersRestored=true;configured=false;}
        // Undo only this helper's one suspend increment, even on API failure.
        if(ResumeThread(ownerHandle)==DWORD(-1)){apiError=GetLastError();ok=false;}
        return ok;
    }
    static DWORD WINAPI Worker(void* context) {
        auto* self=static_cast<HardwareTrace*>(context);
        for(;;){
            if(WaitForSingleObject(self->request,INFINITE)!=WAIT_OBJECT_0)return 1;
            const LONG operation=InterlockedCompareExchange(&self->command,0,0);
            if(operation==Exit){InterlockedExchange(&self->workerOK,1);SetEvent(self->finished);return 0;}
            if(operation==CrossThreadCall){
                auto* memory=reinterpret_cast<void*(__cdecl*)(DWORD)>(0x824257)(11);
                const bool ok=memory!=nullptr;
                if(memory)reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(memory);
                InterlockedExchange(&self->workerOK,ok?1:0);
            }else InterlockedExchange(&self->workerOK,self->WorkerOperation(operation)?1:0);
            SetEvent(self->finished);
        }
    }
    bool Request(LONG operation) {
        if(!worker||!request||!finished)return false;
        ResetEvent(finished);InterlockedExchange(&command,operation);
        return SetEvent(request)&&WaitForSingleObject(finished,10000)==WAIT_OBJECT_0&&InterlockedCompareExchange(&workerOK,0,0)==1;
    }
    static LONG WINAPI Handler(EXCEPTION_POINTERS* exception) {
        auto* self=current;
        if(!self||GetCurrentThreadId()!=self->owner||exception->ExceptionRecord->ExceptionCode!=EXCEPTION_SINGLE_STEP)
            return EXCEPTION_CONTINUE_SEARCH;
        auto& c=*exception->ContextRecord;
        unsigned slot=3;
        if(c.Eip==0x824257)slot=0;else if(c.Eip==0x824269)slot=1;else if(c.Eip==0x82413F)slot=2;
        if(slot==3||c.Dr6!=(IdleDebugStatus|(1u<<slot))||c.Dr7!=self->armed.dr7
            ||c.Dr0!=self->armed.dr0||c.Dr1!=self->armed.dr1||c.Dr2!=self->armed.dr2||c.Dr3!=self->armed.dr3){
            ++self->foreignHits;self->foreign=true;return EXCEPTION_CONTINUE_SEARCH;
        }
        const auto before=ProtectedHash(c);
        const DWORD dr6=c.Dr6;
        c.Dr6&=~7u;
        if(InterlockedCompareExchange(&self->scopeActive,0,0)==0){
            ++self->outsideHits;c.EFlags|=0x10000u;
            self->contextOK&=before==ProtectedHash(c);
            return EXCEPTION_CONTINUE_EXECUTION;
        }
        self->rejectedEip=c.Eip;
        if(self->count>=self->events.size()){
            self->overflow=true;self->rejected=true;
            self->rejectedEsp=c.Esp;self->rejectedSerial=self->serial;
            InterlockedExchange(&self->rejectionPending,1);
            InterlockedExchange(&self->scopeActive,0);
            return EXCEPTION_CONTINUE_SEARCH;
        }
        auto& event=self->events[self->count];
        event.entry=c.Eip;event.eip=c.Eip;event.esp=c.Esp;event.ownerThread=self->owner;
        event.scopeSerial=self->serial;event.sequence=self->count;event.dr6=dr6;event.dr7=c.Dr7;
        const DWORD bytes=slot==1?12u:8u;
        event.stackValid=c.Esp>=self->stackLow&&c.Esp<=self->stackHigh&&self->stackHigh-c.Esp>=bytes;
        if(event.stackValid){
            const auto* stack=reinterpret_cast<const DWORD*>(c.Esp);
            event.returnAddress=stack[0];event.argument0=stack[1];if(slot==1)event.argument1=stack[2];
        }
        event.contextPreserved=before==ProtectedHash(c);self->contextOK&=event.contextPreserved;
        const bool valid=event.stackValid&&event.contextPreserved&&self->count<self->expectedCount
            &&event.entry==self->expected[self->count].entry&&event.argument0==self->expected[self->count].argument0
            &&event.argument1==self->expected[self->count].argument1;
        ++self->count;
        if(!valid){
            self->rejected=true;self->rejectedEsp=c.Esp;self->rejectedSerial=self->serial;
            // Freeze the rejected call before dispatch/unwind can allocate.
            // Its first instruction remains unexecuted; only its matching SEH
            // filter may consume this pending rejection.
            InterlockedExchange(&self->rejectionPending,1);
            InterlockedExchange(&self->scopeActive,0);
            return EXCEPTION_CONTINUE_SEARCH;
        }
        c.EFlags|=0x10000u;
        self->contextOK&=before==ProtectedHash(c);
        return EXCEPTION_CONTINUE_EXECUTION;
    }
public:
    DWORD expectedGateReturn{};
    bool Start() {
        owner=GetCurrentThreadId();
        const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb());
        stackLow=DWORD(reinterpret_cast<std::uintptr_t>(tib->StackLimit));
        stackHigh=DWORD(reinterpret_cast<std::uintptr_t>(tib->StackBase));
        if(!stackLow||stackLow>=stackHigh||current)return false;
        ownerHandle=OpenThread(THREAD_SUSPEND_RESUME|THREAD_GET_CONTEXT|THREAD_SET_CONTEXT|THREAD_QUERY_INFORMATION,FALSE,owner);
        request=CreateEventA(nullptr,FALSE,FALSE,nullptr);finished=CreateEventA(nullptr,TRUE,FALSE,nullptr);
        if(!ownerHandle||!request||!finished){apiError=GetLastError();Stop();return false;}
        current=this;handlerToken=AddVectoredExceptionHandler(1,&Handler);
        if(!handlerToken){apiError=GetLastError();Stop();return false;}
        worker=CreateThread(nullptr,0,&Worker,this,0,nullptr);
        if(!worker){apiError=GetLastError();Stop();return false;}
        if(!Request(Arm)){Stop();return false;}
        return VerifyArmed();
    }
    bool VerifyArmed(){return configured&&Request(Verify)&&contextOK&&!foreign;}
    bool CrossThreadCalibration(){return configured&&Request(CrossThreadCall);}
    void Begin(const std::array<ExpectedEvent,2>& sequence,unsigned length,DWORD scope) {
        expected=sequence;expectedCount=length;serial=scope;count=0;rejected=false;overflow=false;
        rejectedEip=0;rejectedEsp=0;rejectedSerial=0;expectedGateReturn=0;events={};
        InterlockedExchange(&rejectionPending,0);
        if(length>2||!length||!scope){rejected=true;return;}
        InterlockedExchange(&scopeActive,1);
    }
    bool End() {
        InterlockedExchange(&scopeActive,0);
        return !rejected&&!overflow&&!foreign&&contextOK&&count==expectedCount;
    }
    int Filter(EXCEPTION_POINTERS* exception) {
        return GetCurrentThreadId()==owner&&rejected&&rejectedSerial==serial
            &&exception->ExceptionRecord->ExceptionCode==EXCEPTION_SINGLE_STEP
            &&exception->ContextRecord->Eip==rejectedEip
            &&exception->ContextRecord->Esp==rejectedEsp
            &&InterlockedCompareExchange(&rejectionPending,0,1)==1
            ?EXCEPTION_EXECUTE_HANDLER:EXCEPTION_CONTINUE_SEARCH;
    }
    bool Stop() {
        InterlockedExchange(&scopeActive,0);
        bool ok=true;
        if(worker){
            if(savedValid)ok=Request(Restore)&&ok;
            else registersRestored=!configured;
            const bool exitOK=Request(Exit)&&WaitForSingleObject(worker,10000)==WAIT_OBJECT_0;
            workerClosed=exitOK;ok&=exitOK;
            if(exitOK){CloseHandle(worker);worker=nullptr;}
        }else{workerClosed=true;registersRestored=!configured;}
        // Never unregister a consumer while its hardware breakpoints remain.
        if(registersRestored&&workerClosed){
            if(handlerToken){handlerRemoved=RemoveVectoredExceptionHandler(handlerToken)!=0;if(handlerRemoved)handlerToken=nullptr;}
            else handlerRemoved=true;
            if(handlerRemoved)current=nullptr;
            if(ownerHandle){CloseHandle(ownerHandle);ownerHandle=nullptr;}
            if(request){CloseHandle(request);request=nullptr;}
            if(finished){CloseHandle(finished);finished=nullptr;}
        }
        return ok&&registersRestored&&workerClosed&&handlerRemoved;
    }
    unsigned Count()const{return count;}
    const HardwareEvent& Event(unsigned index)const{return events[index];}
    bool Rejected()const{return rejected;}
    DWORD Owner()const{return owner;}
    DWORD Serial()const{return serial;}
    DWORD OutsideHits()const{return outsideHits;}
    DWORD ForeignHits()const{return foreignHits;}
    DWORD ApiError()const{return apiError;}
    bool ContextOK()const{return contextOK;}
    bool RegistersRestored()const{return registersRestored;}
    bool WorkerClosed()const{return workerClosed;}
    bool HandlerRemoved()const{return handlerRemoved;}
    DebugSnapshot Saved()const{return saved;}
    DebugSnapshot Armed()const{return armed;}
    DebugSnapshot Readback()const{return lastReadback;}
};
inline HardwareTrace hardwareTrace;
} // namespace DependenciesMemoryCpp16Live
