#pragma once
#include "LiveComparison.hpp"
namespace RequestedTaskCtorDtorFrontend {
inline void Update() {
    static bool done{};if(done)return;
    char path[MAX_PATH]{};GetModuleFileNameA(nullptr,path,MAX_PATH);
    const auto dir=std::filesystem::path(path).parent_path();
    if(!std::filesystem::exists(dir/"requested-task-ctor-dtor-runtime.start"))return;
    done=true;
    const auto marker=dir/"requested-task-ctor-dtor.start";
    if(!std::filesystem::exists(marker)){std::ofstream file(marker);file<<"frontend probe\n";}
    RequestedTaskCtorDtorLive::Update();
    const auto log=dir/"requested-task-ctor-dtor.jsonl";
    if(std::filesystem::exists(log))std::filesystem::copy_file(log,dir/"requested-task-ctor-dtor-runtime.jsonl",std::filesystem::copy_options::overwrite_existing);
    std::filesystem::remove(marker);
}
}
