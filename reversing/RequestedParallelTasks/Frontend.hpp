#pragma once
#include <map>
#include <new>
#include "TaskLifetimeComparison.hpp"
#include "TaskLifetimeExpanded/Expanded.hpp"
namespace RequestedTaskFrontend {
inline void Update(){
    char path[MAX_PATH]{};GetModuleFileNameA(nullptr,path,MAX_PATH);
    const auto dir=std::filesystem::path(path).parent_path();
    const auto dispatch=[&](const char* prefix,auto update){
        const auto runtimeMarker=dir/(std::string(prefix)+"-runtime.start");
        if(!std::filesystem::exists(runtimeMarker))return;
        const auto bankMarker=dir/(std::string(prefix)+".start");
        const auto bankLog=dir/(std::string(prefix)+".jsonl");
        if(!std::filesystem::exists(bankMarker)){std::ofstream f(bankMarker);f<<"frontend probe\n";}
        update();
        // Keep frozen bank filenames; adapt only the frontend wrapper to the shared runner.
        if(std::filesystem::exists(bankLog))std::filesystem::copy_file(bankLog,dir/(std::string(prefix)+"-runtime.jsonl"),std::filesystem::copy_options::overwrite_existing);
        std::filesystem::remove(bankMarker);
    };
    static bool done{};if(done)return;
    if(!std::filesystem::exists(dir/"requested-task-lifetime-runtime.start")&&!std::filesystem::exists(dir/"requested-task-expanded-runtime.start"))return;
    done=true;
    dispatch("requested-task-lifetime",[]{RequestedTaskLifetime::Update();});
    dispatch("requested-task-expanded",[]{RequestedTaskLifetimeExpanded::Update();});
}
}
