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
 * Validación registrada: 12/12 entradas.
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
    // Como el original: por entrada solo escribe el puntero, los usuarios, el índice (-1), el primer byte del
    // nombre y el tamaño (el resto del nombre y el relleno quedan como estaban), y pone a cero el tamaño
    // máximo y la cuenta de scripts.
    for (auto& scr : m_aScripts) {
        scr.m_StreamedScriptMemory  = nullptr;
        scr.m_NumberOfUsers         = 0;
        scr.m_IndexUsedByScriptFile = -1;
        scr.m_Filename[0]           = 0;
        scr.m_SizeInBytes           = 0;
    }
    m_nLargestExternalSize = 0;
    m_nCountOfScripts      = 0;
}

// 0x470840
bool CStreamedScripts::LoadStreamedScript(RwStream* stream, int32 index) {
    // Como el original: devuelve el resultado de la comprobación de versión (falso) o cierto tras leer.
    if (!CTheScripts::CheckStreamedScriptVersion(stream, "UnKown")) {
        return false;
    }
    auto& scr = m_aScripts[index];

    scr.m_StreamedScriptMemory = new uint8[scr.m_SizeInBytes];
    RwStreamRead(stream, scr.m_StreamedScriptMemory, scr.m_SizeInBytes);
    return true;
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
    // Como el original: la cuenta se lee sin signo y se compara con _stricmp(nombre registrado, buscado).
    for (int32 i = 0; i < (int32)(uint16)m_nCountOfScripts; i++) {
        if (!_stricmp(m_aScripts.data()[i].m_Filename, scriptName)) {
            return i;
        }
    }
    return -1;
}

// 0x470810
int16 CStreamedScripts::GetProperIndexFromIndexUsedByScript(int16 scmIndex) {
    // Como el original: recorre las 82 entradas, no solo las registradas.
    for (int16 i = 0; i < NUM_STREAMED_SCRIPTS; i++) {
        if (m_aScripts[i].m_IndexUsedByScriptFile == scmIndex) {
            return i;
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
    // Como el original: llama siempre a operator delete[] (también con puntero nulo) y luego anula el puntero.
    auto& scr = m_aScripts[index];
    ::operator delete[](scr.m_StreamedScriptMemory);
    scr.m_StreamedScriptMemory = nullptr;
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
    // Como el original: el bloque de scripts en streaming se alcanza con tres saltos fijos desde el inicio
    // del SCM (cuarto bloque, sin buscar por índice) y se recorren tantas entradas como scripts registrados
    // (no las del bloque). Cada nombre se copia a un búfer local de 20 bytes; si no se encuentra (-1) se
    // escribe en la entrada -1, como el EXE.
    const uint8* const base = CTheScripts::ScriptSpace.data();
    const auto Jump = [base](uint32 offset) { return *reinterpret_cast<const uint32*>(base + 3 + offset); };
    const uint8* entry = base + Jump(Jump(Jump(0))) + 8;

    m_nLargestExternalSize = *reinterpret_cast<const int32*>(entry);
    entry += 8;
    for (uint16 i = 0; i < (uint16)m_nCountOfScripts; i++, entry += 0x1C) {
        char name[20];
        std::memcpy(name, entry, sizeof(name));
        auto& scr                   = m_aScripts.data()[FindStreamedScript(name)];
        scr.m_SizeInBytes           = *reinterpret_cast<const int32*>(entry + 0x18);
        scr.m_IndexUsedByScriptFile = (int16)i;
    }
}

// 0x4706C0
int32 CStreamedScripts::RegisterScript(const char* scriptName) {
    // Como el original: cuenta sin signo, strcpy sin límite y devuelve la cuenta nueva (movzx) menos uno.
    strcpy(m_aScripts.data()[(uint16)m_nCountOfScripts].m_Filename, scriptName);
    m_nCountOfScripts++;
    return (int32)(uint16)m_nCountOfScripts - 1;
}

// 0x470910
uint32 CStreamedScripts::GetStreamedScriptWithThisStartAddress(uint8* dataPtr)
{
    // Como el original: si no lo encuentra devuelve 82 (NUM_STREAMED_SCRIPTS).
    uint16 i = 0;
    for (; i < NUM_STREAMED_SCRIPTS; i++) {
        if (m_aScripts[i].m_StreamedScriptMemory == dataPtr) {
            break;
        }
    }
    return i;
}
