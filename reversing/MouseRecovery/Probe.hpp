#pragma once
#include <fstream>
namespace MouseProbe {
inline std::ofstream log;
inline uint64_t started{};
inline unsigned initialFailures{}, readFailures{}, readSuccesses{}, acquireSuccesses{}, delivered{};
inline bool complete{};
inline bool Active(){
    if(complete)return false;
    if(!started){
        if(!std::filesystem::exists("mouse-recovery-runtime.start"))return false;
        started=GetTickCount64();log.open("mouse-recovery-runtime.jsonl");
    }
    return true;
}
inline void Write(json row){row["ms"]=GetTickCount64()-started;log<<row.dump()<<'\n';log.flush();}
inline void Record(const char* event,HRESULT hr){
    if(!Active())return;
    const std::string_view kind{event};
    if(kind=="initial_acquire" && FAILED(hr))++initialFailures;
    if(kind=="reacquire" && SUCCEEDED(hr))++acquireSuccesses;
    const bool read=kind=="read"||kind=="retry_read";
    if(read){if(FAILED(hr))++readFailures;else ++readSuccesses;}
    if(!read || FAILED(hr) || readSuccesses<=2)
        Write({{"event",event},{"hr",uint32(hr)},{"ok",SUCCEEDED(hr)},
            {"window",reinterpret_cast<uintptr_t>(PSGLOBAL(window))},
            {"foreground",reinterpret_cast<uintptr_t>(GetForegroundWindow())},
            {"visible",bool(IsWindowVisible(PSGLOBAL(window)))}});
}
inline void Delivered(){
    if(!Active())return;
    if(++delivered<120)return;
    Write({{"event","complete"},{"initial_failures",initialFailures},{"read_failures",readFailures},
        {"read_successes",readSuccesses},{"reacquire_successes",acquireSuccesses},{"delivered",delivered}});
    complete=true;
}
}
