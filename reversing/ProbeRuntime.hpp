#pragma once
#include <crtdbg.h>
#include <rtcapi.h>
#include <fstream>
#include "ProbeSupport.hpp"
namespace ReversingProbe {
inline std::filesystem::path failureLog;
inline void Fail(const json& record,UINT code){std::ofstream file(failureLog,std::ios::app);file<<record.dump()<<'\n';file.flush();ExitProcess(code);}
inline int __cdecl RuntimeFailure(int,const wchar_t*,int,const wchar_t*,const wchar_t*,...){Fail({{"event","error"},{"reason","RTC failure before or during probe"}},92);return 0;}
inline void __cdecl AssertFailure(const wchar_t* expr,const wchar_t* file,unsigned line){RECT rc{};if(RsGlobal.ps)GetClientRect(PSGLOBAL(window),&rc);char e[2048]{},f[2048]{};WideCharToMultiByte(CP_UTF8,0,expr,-1,e,2048,nullptr,nullptr);WideCharToMultiByte(CP_UTF8,0,file,-1,f,2048,nullptr,nullptr);Fail({{"event","assertion"},{"expression",e},{"file",f},{"line",line},{"maximum_width",RsGlobal.maximumWidth},{"maximum_height",RsGlobal.maximumHeight},{"client_width",rc.right},{"client_height",rc.bottom}},93);}
inline LONG CALLBACK ExceptionFailure(PEXCEPTION_POINTERS p){const auto code=p->ExceptionRecord->ExceptionCode;if(code==EXCEPTION_ACCESS_VIOLATION||code==EXCEPTION_ILLEGAL_INSTRUCTION||code==EXCEPTION_INT_DIVIDE_BY_ZERO){Fail({{"event","error"},{"reason","exception"},{"code",code},{"eip",p->ContextRecord->Eip},{"ecx",p->ContextRecord->Ecx},{"edx",p->ContextRecord->Edx},{"eax",p->ContextRecord->Eax}},94);}return EXCEPTION_CONTINUE_SEARCH;}
inline void InstallFailureCapture(const char* prefix){char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/(std::string(prefix)+"-runtime.start")))return;failureLog=dir/(std::string(prefix)+"-runtime.jsonl");_set_error_mode(_OUT_TO_STDERR);_set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE);_CrtSetReportMode(_CRT_ERROR,_CRTDBG_MODE_FILE);_RTC_SetErrorFuncW(RuntimeFailure);static const auto assertion=std::make_unique<EntryPatch>(reinterpret_cast<uintptr_t>(&_wassert),AssertFailure);
 // Let local SEH handlers (including graphics drivers) handle first-chance
 // exceptions. Only an unhandled exception terminates this unattended probe.
 SetUnhandledExceptionFilter(ExceptionFailure);}
}
