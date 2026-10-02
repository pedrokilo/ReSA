/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CStreamedScripts
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "StreamedScripts.h"
#include "TheScripts.h"
#include "SCMChunks.hpp"

void CStreamedScripts::InjectHooks() {
    RH_ScopedClass(CStreamedScripts);
    RH_ScopedCategory("Scripts");

    RH_ScopedInstall(Initialise, 0x470660);
    RH_ScopedInstall(LoadStreamedScript, 0x470840);
    RH_ScopedInstall(ReInitialise, 0x4706A0);
    RH_ScopedInstall(FindStreamedScript, 0x470740);
    RH_ScopedInstall(FindStreamedScriptQuiet, 0x4706F0);
    RH_ScopedInstall(GetProperIndexFromIndexUsedByScript, 0x470810);
    RH_ScopedInstall(GetStreamedScriptFilename, 0x470900);
    RH_ScopedInstall(RemoveStreamedScriptFromMemory, 0x4708E0);
    RH_ScopedInstall(StartNewStreamedScript, 0x470890);
    RH_ScopedInstall(ReadStreamedScriptData, 0x470750);
    RH_ScopedInstall(RegisterScript, 0x4706C0);
    RH_ScopedInstall(GetStreamedScriptWithThisStartAddress, 0x470910);
}

// 0x470660
void CStreamedScripts::Initialise() {
    rng::fill(m_aScripts, CStreamedScriptInfo{});
}

// 0x470840
void CStreamedScripts::LoadStreamedScript(RwStream* stream, int32 index) {
    if (CTheScripts::CheckStreamedScriptVersion(stream, "UnKown")) {
        auto& scr = m_aScripts[index];

        scr.m_StreamedScriptMemory = new uint8[scr.m_SizeInBytes];
        RwStreamRead(stream, scr.m_StreamedScriptMemory, scr.m_SizeInBytes);
    }
}

// 0x4706A0
void CStreamedScripts::ReInitialise() {
    for (auto& scr : m_aScripts) {
        // Remove all references so CStreaming::RemoveLeastUsedModel
        // will garbage collect (i.e. remove) it later.
        scr.m_NumberOfUsers = 0;
    }
}

// 0x470740 -- inlined
int32 CStreamedScripts::FindStreamedScript(const char* scriptName) {
    return FindStreamedScriptQuiet(scriptName);
}

// 0x4706F0 -- inlined
int32 CStreamedScripts::FindStreamedScriptQuiet(const char* scriptName) {
    for (const auto&& [i, scr] : GetActiveScriptsWithIndex()) {
        if (notsa::ci_string_view{ scr.m_Filename } == scriptName) {
            return i;
        }
    }
    return -1;
}

// 0x470810
int16 CStreamedScripts::GetProperIndexFromIndexUsedByScript(int16 scmIndex) {
    for (const auto&& [i, scr] : GetActiveScriptsWithIndex()) {
        if (scr.m_IndexUsedByScriptFile == scmIndex) {
            return (int16)(i);
        }
    }
    return -1;
}

// 0x470900
const char* CStreamedScripts::GetStreamedScriptFilename(uint16 index) {
    return m_aScripts[index].m_Filename;
}

// 0x4708E0
void CStreamedScripts::RemoveStreamedScriptFromMemory(int32 index) {
    delete[] std::exchange(m_aScripts[index].m_StreamedScriptMemory, nullptr);
}

// 0x470890
CRunningScript* CStreamedScripts::StartNewStreamedScript(int32 index)
{
    auto& scr = m_aScripts[index];
    if (auto* baseIP = scr.m_StreamedScriptMemory)
    {
        CRunningScript* S = CTheScripts::StartNewScript(baseIP);
        S->SetBaseIp(baseIP);
        S->SetExternal(true);

        scr.m_NumberOfUsers++;
        CStreaming::SetMissionDoesntRequireModel(SCMToModelId(index));
        return S;
    }

    return nullptr;
}

// 0x470750
void CStreamedScripts::ReadStreamedScriptData()
{
    const auto* streamedScrChunk = CTheScripts::GetSCMChunk<tSCMStreamedScriptFileInfoChunk>();
    m_nLargestExternalSize = streamedScrChunk->m_LargestStreamScriptSize;

    int16 streamIdx = 0;
    for (const auto& streamed : streamedScrChunk->GetStreamedScripts()) {
        if (streamIdx >= m_nCountOfScripts) {
            break;
        }

        // NOTSA: `streamed.m_FileName` was copied into a local string before calling find.
        const auto i = FindStreamedScript(streamed.m_FileName);
        assert(i != -1); // Find returning -1 ends up with OOB access
        auto& scr                   = m_aScripts[i];
        scr.m_SizeInBytes           = streamed.m_Size;
        scr.m_IndexUsedByScriptFile = streamIdx++;
    }
}

// 0x4706C0
int32 CStreamedScripts::RegisterScript(const char* scriptName) {
    strcpy_s(m_aScripts[m_nCountOfScripts].m_Filename, scriptName);
    return m_nCountOfScripts++;
}

// 0x470910
uint32 CStreamedScripts::GetStreamedScriptWithThisStartAddress(uint8* dataPtr)
{
    for (const auto&& [i, scr] : rngv::enumerate(m_aScripts)) {
        if (scr.m_StreamedScriptMemory == dataPtr) {
            return i;
        }
    }
    // Originally returns '0', possibly unintended.
    NOTSA_UNREACHABLE();
}
