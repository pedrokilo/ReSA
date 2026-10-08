#include "StdInc.h"
#include "config.h"
#include <cstdlib>

#include "WinPlatform.h"
#include "extensions/CommandLine.h"
#include "extensions/debug.hpp"
#include "extensions/Configuration.hpp"
#include "reversiblehooks/RootHookCategory.h"
#include "reversiblehooks/ReversibleHook/Simple.h"
#include "reversiblehooks/ReversibleHook/Virtual.h"
#include <fstream>
#include <unordered_set>

void InjectHooksMain(HMODULE hThisDLL);

static constexpr auto DEFAULT_INI_FILENAME = "gta-reversed.ini";

#include "extensions/Configs/FastLoader.hpp"
#include "extensions/Configs/Miscellaneous.hpp"

void LoadConfigurations() {
    // Firstly load the INI into the memory.
    g_ConfigurationMgr.Load(DEFAULT_INI_FILENAME);

    // Then load all specific configurations.
    g_FastLoaderConfig.Load();
    g_MiscConfig.Load();
    // ...
}

static void ApplyCommandLineHookSettings() {
    using namespace ReversibleHooks;

    const auto ResultText = [](SetCatOrItemStateResult res) {
        switch (res) {
        case SetCatOrItemStateResult::NotFound: return "not found";
        case SetCatOrItemStateResult::Locked:   return "locked";
        case SetCatOrItemStateResult::Done:     return "done";
        default:                                NOTSA_UNREACHABLE();
        }
    };

    if (CommandLine::s_UnhookAll || !CommandLine::s_UnhookExcept.empty()) {
        GetRootCategory().SetAllItemsEnabled(false);

        NOTSA_LOG_DEBUG("Unhooked all via command-line");
        for (const auto& item : CommandLine::s_UnhookExcept) {
            const auto res = SetCategoryOrItemStateByPath(item, true);

            if (res == SetCatOrItemStateResult::Done) {
                NOTSA_LOG_DEBUG("Rehooked '{}' via command-line.", item);
            } else {
                NOTSA_LOG_WARN("Couldn't rehook '{}' via command-line: {}", item, ResultText(res));
            }
        }
        return;
    }

    if (!CommandLine::s_UnhookSome.empty()) {
        for (const auto& item : CommandLine::s_UnhookSome) {
            const auto res = SetCategoryOrItemStateByPath(item, false);

            if (res == SetCatOrItemStateResult::Done) {
                NOTSA_LOG_DEBUG("Unhooked '{}' via command-line.", item);
            } else {
                NOTSA_LOG_WARN("Couldn't unhook '{}' via command-line: {}", item, ResultText(res));
            }
        }
    }
}

// Opt-in historical catalog selection; unknown functions are disabled.
static void ApplyHistoricalHookSettings() {
    const char* selection = nullptr;
    for (int i = 1; i < __argc; ++i) {
        const std::string_view arg{__argv[i]};
        if (arg.starts_with("--historical-hooks=")) {
            selection = __argv[i] + sizeof("--historical-hooks=") - 1;
        }
    }
    if (!selection) {
        return;
    }
    std::ifstream input{selection};
    std::unordered_set<std::string> allowed;
    for (std::string line; std::getline(input, line);) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (!line.empty()) allowed.insert(std::move(line));
    }
    std::ofstream report{std::string{selection} + ".runtime.tsv"};
    bool valid = !allowed.empty() && report.good();
    size_t matched = 0;
    report << "class\tfunction\taddress\ttype\tselected\thooked\tlocked_before\n";
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat) {
        using namespace ReversibleHooks::ReversibleHook;
        for (auto& item : cat.Items()) {
            uintptr_t address = 0;
            if (item->Type() == Base::HookType::Simple) {
                address = reinterpret_cast<uintptr_t>(std::static_pointer_cast<Simple>(item)->GetHookGTAAddress());
            } else if (item->Type() == Base::HookType::Virtual) {
                address = reinterpret_cast<uintptr_t>(std::static_pointer_cast<Virtual>(item)->GetHookGTAAddress());
            }
            const auto key = std::format("{}\t{}\t{:08X}\t{}", cat.Name(), item->Name(), address, item->Symbol());
            // Historical catalogs use zero as the virtual-function address sentinel.
            const auto catalogKey = item->Type() == Base::HookType::Virtual
                ? std::format("{}\t{}\t00000000\t{}", cat.Name(), item->Name(), item->Symbol())
                : key;
            const bool selected = allowed.contains(key) || allowed.contains(catalogKey);
            const bool locked = item->Locked();
            item->LockState(false);
            cat.SetItemEnabled(item, selected);
            // Keep the chosen profile fixed for this session.
            item->LockState(true);
            valid = valid && item->Hooked() == selected;
            matched += selected;
            report << key << '\t' << selected << '\t' << item->Hooked() << '\t' << locked << '\n';
        }
    });
    report.flush();
    valid = valid && report.good() && matched == allowed.size();
    if (!valid) {
        MessageBoxA(nullptr, "Historical hook selection failed. See the runtime report.", "GTA reversed", MB_ICONERROR);
        ExitProcess(1);
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH: {
        // Fail if RenderWare has already been started
        if (*(RwCamera**)0xC1703C) {
            MessageBox(NULL, "gta_reversed failed to load (RenderWare has already been started)", "Error", MB_ICONERROR | MB_OK);
            return FALSE;
        }

        std::setlocale(LC_ALL, "en_US.UTF-8");

        notsa::debug::DisplayConsole();
        CommandLine::Load(__argc, __argv);
        if (CommandLine::s_WaitForDebugger) {
            notsa::debug::WaitForDebugger();
        }

        LoadConfigurations();

        InjectHooksMain(hModule);
        ApplyCommandLineHookSettings();
        ApplyHistoricalHookSettings();
        break;
    }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
