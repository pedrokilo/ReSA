#pragma once
#include <Windows.h>
#if !defined(_M_IX86)
#error Fixed diagnostic expects an x86 process.
#endif
namespace MemoryRejectionDiagnostic {
// Proposal only: no shared bank includes this file.
struct Record {
    DWORD magic;
    DWORD version;
    DWORD recordBytes;
    DWORD kind;
    DWORD sequence;
    DWORD thread;
    DWORD owner;
    DWORD scopeSerial;
    DWORD rejectedSerial;
    DWORD scopeActive;
    DWORD pending;
    DWORD count;
    DWORD rejectedEip;
    DWORD rejectedEsp;
    DWORD exceptionCode;
    DWORD exceptionFlags;
    DWORD exceptionAddress;
    DWORD contextFlags;
    DWORD eip;
    DWORD esp;
    DWORD eflags;
    DWORD dr0;
    DWORD dr1;
    DWORD dr2;
    DWORD dr3;
    DWORD dr6;
    DWORD dr7;
    DWORD eax;
    DWORD ebp;
    DWORD returnAddress;
    DWORD argument0;
    DWORD argument1;
    DWORD matchMask;
    DWORD filterResult;
    DWORD writeErrorPrior;
    DWORD missedPrior;
};
static_assert(sizeof(Record)==144);
struct State {
    DWORD owner,scopeSerial,rejectedSerial,scopeActive,pending,count;
    DWORD rejectedEip,rejectedEsp,returnAddress,argument0,argument1;
};
inline HANDLE file=INVALID_HANDLE_VALUE;
inline volatile LONG busy{},sequence{},missed{},lastError{};
// Preopen outside VEH/SEH. Caller retains ownership and closes after observer stops.
inline void SetPreopenedHandle(HANDLE handle) {file=handle;}
inline void WriteFixed(DWORD kind,const EXCEPTION_POINTERS* exception,const State& s,
    DWORD matchMask,DWORD filterResult) {
    if(file==INVALID_HANDLE_VALUE||file==nullptr){InterlockedIncrement(&missed);return;}
    if(InterlockedCompareExchange(&busy,1,0)!=0){InterlockedIncrement(&missed);return;}
    const DWORD priorError=GetLastError();
    Record r{};r.magic=0x4D52444Bu;r.version=1;r.recordBytes=sizeof(r);r.kind=kind;
    r.sequence=DWORD(InterlockedIncrement(&sequence));r.thread=GetCurrentThreadId();
    r.owner=s.owner;r.scopeSerial=s.scopeSerial;r.rejectedSerial=s.rejectedSerial;
    r.scopeActive=s.scopeActive;r.pending=s.pending;r.count=s.count;
    r.rejectedEip=s.rejectedEip;r.rejectedEsp=s.rejectedEsp;
    r.returnAddress=s.returnAddress;r.argument0=s.argument0;r.argument1=s.argument1;
    r.matchMask=matchMask;r.filterResult=filterResult;
    r.writeErrorPrior=DWORD(InterlockedCompareExchange(&lastError,0,0));
    r.missedPrior=DWORD(InterlockedCompareExchange(&missed,0,0));
    if(exception&&exception->ExceptionRecord&&exception->ContextRecord){
        const auto& e=*exception->ExceptionRecord;const auto& c=*exception->ContextRecord;
        r.exceptionCode=e.ExceptionCode;r.exceptionFlags=e.ExceptionFlags;
        r.exceptionAddress=DWORD(reinterpret_cast<ULONG_PTR>(e.ExceptionAddress));
        r.contextFlags=c.ContextFlags;r.eip=c.Eip;r.esp=c.Esp;r.eflags=c.EFlags;
        r.dr0=c.Dr0;r.dr1=c.Dr1;r.dr2=c.Dr2;r.dr3=c.Dr3;r.dr6=c.Dr6;r.dr7=c.Dr7;
        r.eax=c.Eax;r.ebp=c.Ebp;
    }
    if(r.sequence<=256){
        DWORD written{};
        const BOOL ok=WriteFile(file,&r,sizeof(r),&written,nullptr);
        if(!ok||written!=sizeof(r)){
            InterlockedExchange(&lastError,LONG(ok?ERROR_WRITE_FAULT:GetLastError()));
            InterlockedIncrement(&missed);
        }
    }else InterlockedIncrement(&missed);
    SetLastError(priorError);InterlockedExchange(&busy,0);
}
} // namespace MemoryRejectionDiagnostic
