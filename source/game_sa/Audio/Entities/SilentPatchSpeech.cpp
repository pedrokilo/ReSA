/*
 * Correcciones de voces y dialogos tomadas de SilentPatch (The MIT License, Copyright (c) 2024 Adrian Zdanowicz (Silent); docs/THIRD_PARTY_NOTICES.md).
 * Tablas propias con solo las entradas que SilentPatch cambia respecto a las tablas de voz del EXE 1.0 US; ver SilentPatchSpeech.h.
 */
#include "StdInc.h"

#include <array>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <utility>

#include "SilentPatchSpeech.h"
#include "AEPedSpeechAudioEntity.h"
#include "extensions/SilentPatchConfig.h"
#include "Zone.h"
#include "PedGroups.h"
#include "Events/EventPedToFlee.h"
#include "GangWars.h"
#include "PedToPlayerConversations.h"
#include "Pad.h"
#include "Crime.h"
#include "PedModelInfo.h"

namespace SilentPatchSpeech {
namespace {
int s_force = -1;

constexpr const char* kOptNames[(size_t)eOpt::NUM] = {
    "SpeechWeatherConversations", "SpeechCriminalsFlee", "SpeechPedVoiceNames", "SpeechMaccerVoice", "SpeechVBFYST2", "SpeechTurfCheers",
    "SpeechSolicitMood", "SpeechGirlfriendShopkeeperLines", "SpeechShopkeepersSpooked", "SpeechVWMOTR1", "SpeechGangTaunts", "SpeechCJJackingCars",
    "SpeechCrashGenericFallback", "SpeechPainSprayed", "SpeechChasedOneStar", "SpeechShootMouseKeyboard", "SpeechCopsCoverLines", "SpeechFAM3",
    "SpeechLAPDM1", "SpeechGangAttackPlayer", "SpeechWhereYouFromReply", "GoodCitizenBonus", "SpeechDruggedRecruit", "SpeechCarHitPedPassenger",
    "PoliceScannerNames",
};

// Asignaciones de contexto especifico a contextos globales que en el EXE estan a -1 (SilentPatchSA.cpp, Patches::PatchGlobalSpeechContexts).
struct tGlobalRule {
    int16 GCtx;
    int16 PedType;
    int16 SCtx;
    eOpt  Opt;
};
// clang-format off
constexpr tGlobalRule kGlobalRules[] = {
    // L129: CJ celebra la toma de territorio
    {CTX_GLOBAL_TAKE_TURF_LAS_COLINAS, PED_TYPE_PLAYER, CTX_PLY_TAKE_TURF_LAS_COLINAS, eOpt::TurfCheers},
    {CTX_GLOBAL_TAKE_TURF_LOS_FLORES, PED_TYPE_PLAYER, CTX_PLY_TAKE_TURF_LOS_FLORES, eOpt::TurfCheers},
    {CTX_GLOBAL_TAKE_TURF_EAST_BEACH, PED_TYPE_PLAYER, CTX_PLY_TAKE_TURF_EAST_BEACH, eOpt::TurfCheers},
    {CTX_GLOBAL_TAKE_TURF_EAST_LS, PED_TYPE_PLAYER, CTX_PLY_TAKE_TURF_EAST_LS, eOpt::TurfCheers},
    {CTX_GLOBAL_TAKE_TURF_JEFFERSON, PED_TYPE_PLAYER, CTX_PLY_TAKE_TURF_JEFFERSON, eOpt::TurfCheers},
    {CTX_GLOBAL_TAKE_TURF_GLEN_PARK, PED_TYPE_PLAYER, CTX_PLY_TAKE_TURF_GLEN_PARK, eOpt::TurfCheers},
    {CTX_GLOBAL_TAKE_TURF_IDLEWOOD, PED_TYPE_PLAYER, CTX_PLY_TAKE_TURF_IDLEWOOD, eOpt::TurfCheers},
    {CTX_GLOBAL_TAKE_TURF_GANTON, PED_TYPE_PLAYER, CTX_PLY_TAKE_TURF_GANTON, eOpt::TurfCheers},
    {CTX_GLOBAL_TAKE_TURF_LITTLE_MEXICO, PED_TYPE_PLAYER, CTX_PLY_TAKE_TURF_LITTLE_MEXICO, eOpt::TurfCheers},
    {CTX_GLOBAL_TAKE_TURF_WILLOWFIELD, PED_TYPE_PLAYER, CTX_PLY_TAKE_TURF_WILLOWFIELD, eOpt::TurfCheers},
    {CTX_GLOBAL_TAKE_TURF_PLAYA_DEL_SEVILLE, PED_TYPE_PLAYER, CTX_PLY_TAKE_TURF_PLAYA_DEL_SEVILLE, eOpt::TurfCheers},
    {CTX_GLOBAL_TAKE_TURF_TEMPLE, PED_TYPE_PLAYER, CTX_PLY_TAKE_TURF_TEMPLE, eOpt::TurfCheers},
    // L131: lineas sin usar de novias y tenderos
    {CTX_GLOBAL_CONV_IGNORED, PED_TYPE_GFD, CTX_GFD_CONV_IGNORED, eOpt::GfdLines},
    {CTX_GLOBAL_GUN_COOL, PED_TYPE_GFD, CTX_GFD_GUN_COOL, eOpt::GfdLines},
    {CTX_GLOBAL_LIKE_DISMISS_REPLY, PED_TYPE_GFD, CTX_GFD_LIKE_DISMISS_REPLY, eOpt::GfdLines},
    {CTX_GLOBAL_PCONV_AGREE_BAD, PED_TYPE_GFD, CTX_GFD_PCONV_AGREE_BAD, eOpt::GfdLines},
    {CTX_GLOBAL_PCONV_AGREE_GOOD, PED_TYPE_GFD, CTX_GFD_PCONV_AGREE_GOOD, eOpt::GfdLines},
    {CTX_GLOBAL_PCONV_ANS_NO, PED_TYPE_GFD, CTX_GFD_PCONV_ANS_NO, eOpt::GfdLines},
    {CTX_GLOBAL_PCONV_DISMISS, PED_TYPE_GFD, CTX_GFD_PCONV_DISMISS, eOpt::GfdLines},
    {CTX_GLOBAL_PCONV_GREET_FEM, PED_TYPE_GFD, CTX_GFD_PCONV_GREET_FEM, eOpt::GfdLines},
    {CTX_GLOBAL_PCONV_GREET_MALE, PED_TYPE_GFD, CTX_GFD_PCONV_GREET_MALE, eOpt::GfdLines},
    {CTX_GLOBAL_PCONV_PART_FEM, PED_TYPE_GFD, CTX_GFD_PCONV_PART_FEM, eOpt::GfdLines},
    {CTX_GLOBAL_PCONV_PART_MALE, PED_TYPE_GFD, CTX_GFD_PCONV_PART_MALE, eOpt::GfdLines},
    {CTX_GLOBAL_PCONV_QUESTION, PED_TYPE_GFD, CTX_GFD_PCONV_QUESTION, eOpt::GfdLines},
    {CTX_GLOBAL_PCONV_STATE_BAD, PED_TYPE_GFD, CTX_GFD_PCONV_STATE_BAD, eOpt::GfdLines},
    {CTX_GLOBAL_PCONV_STATE_GOOD, PED_TYPE_GFD, CTX_GFD_PCONV_STATE_GOOD, eOpt::GfdLines},
    {CTX_GLOBAL_SAVED, PED_TYPE_GFD, CTX_GFD_SAVED, eOpt::GfdLines},
    {CTX_GLOBAL_SHOCKED, PED_TYPE_GFD, CTX_GFD_SHOCKED, eOpt::GfdLines},
    {CTX_GLOBAL_TRAPPED, PED_TYPE_GFD, CTX_GFD_TRAPPED, eOpt::GfdLines},
    {CTX_GLOBAL_GUN_RUN, PED_TYPE_GFD, CTX_GFD_GUN_RUN, eOpt::GfdLines},
    // SHOOT_GENERIC para peatones y novias/tenderos (sin uso en el juego: lo habilita otro mod)
    {CTX_GLOBAL_SHOOT_GENERIC, PED_TYPE_GEN, CTX_GEN_SHOOT_GENERIC, eOpt::GfdLines},
    {CTX_GLOBAL_SHOOT_GENERIC, PED_TYPE_GFD, CTX_GFD_SHOOT_GENERIC, eOpt::GfdLines},
    // L134: insultos de banda contra Triadas, Rifa y Da Nang Boys
    {CTX_GLOBAL_ABUSE_TRIAD, PED_TYPE_GANG, CTX_GNG_ABUSE_GANG_TRIADS, eOpt::GangTaunts},
    {CTX_GLOBAL_ABUSE_RIFA, PED_TYPE_GANG, CTX_GNG_ABUSE_GANG_SFR, eOpt::GangTaunts},
    {CTX_GLOBAL_ABUSE_DA_NANG, PED_TYPE_GANG, CTX_GNG_ABUSE_GANG_DNB, eOpt::GangTaunts},
};

// Errata en las tablas por voz: los sonidos de un contexto estan en un contexto con el nombre mal escrito (SilentPatchSA.cpp, tryMoveContext y Patch*SpeechContexts).
struct tMoveRule {
    eTable Table;
    int16  Src;
    int16  Dst;
    int16  OnlyVoice; // -1 = todas las voces
    eOpt   Opt;
};
constexpr tMoveRule kMoveRules[] = {
    // L127: VBFYST2 tiene sus contextos con nombres cortados
    {eTable::GEN, CTX_GEN_CHA, CTX_GEN_CHAT, -1, eOpt::VBFYST2},
    {eTable::GEN, CTX_GEN_RUN_FROM_FIGH, CTX_GEN_RUN_FROM_FIGHT, -1, eOpt::VBFYST2},
    {eTable::GEN, CTX_GEN_SAVE, CTX_GEN_SAVED, -1, eOpt::VBFYST2},
    {eTable::GEN, CTX_GEN_SHOCKE, CTX_GEN_SHOCKED, -1, eOpt::VBFYST2},
    // L133: VWMOTR1 tiene MUGGED asignado a MUGGING
    {eTable::GEN, CTX_GEN_MUGGING, CTX_GEN_MUGGED, VOICE_GEN_VWMOTR1, eOpt::VWMOTR1},
    // L142: LAPDM1 (y los demas EMG con la errata)
    {eTable::EMG, CTX_EMG_ARREST_CRIMINAL, CTX_EMG_ARREST_CRIM, -1, eOpt::LAPDM1},
    // L132: tenderos asustados (SHOOT_GENERIC de GFD contiene los datos de SHOP_CLOSED)
    {eTable::GFD, CTX_GFD_SHOOT_GENERIC, CTX_GFD_SHOP_CLOSED, -1, eOpt::ShopSpooked},
    // L131: CAR_ON_FIRE es un contexto incorrecto de GFD
    {eTable::GFD, CTX_GFD_CAR_ON_FIRE, CTX_GFD_CAR_FIRE, -1, eOpt::GfdLines},
    // L135: CJ robando coches a mujeres
    {eTable::PLAYER, CTX_PLY_JACKING_CAR_FEMALE, CTX_PLY_JACKING_CAR_FEM, -1, eOpt::CJJackingWomen},
    // L141: FAM3 (y las demas voces de pandilla con la errata)
    {eTable::GANG, CTX_GNG_ATTACK_PLAYER_LIKE, CTX_GNG_ATTACK_BY_PLAYER_LIKE, -1, eOpt::FAM3},
};
// clang-format on

constexpr int16 kNoRange16[2] = {-1, -1};
constexpr uint8 kNoRange8[2]  = {0xFF, 0xFF};
} // namespace

void ForceForTest(int state) {
    s_force = state;
}

bool On(eOpt opt) {
    if (s_force >= 0) {
        return s_force != 0;
    }
    static std::array<int8, (size_t)eOpt::NUM> cache = [] {
        std::array<int8, (size_t)eOpt::NUM> a{};
        a.fill(-1);
        return a;
    }();
    auto& c = cache[(size_t)opt];
    if (c < 0) {
        c = SilentPatchOption(kOptNames[(size_t)opt], true) ? 1 : 0; // por defecto encendido (no cambian el ritmo ni la jugabilidad; solo anaden o arreglan voces)
    }
    return c != 0;
}

int16 SpecificContext(int16 gCtx, int16 pedAudioType, int16 exeValue) {
    if (exeValue != -1) {
        return exeValue;
    }
    for (const auto& r : kGlobalRules) {
        if (r.GCtx == gCtx && r.PedType == pedAudioType && On(r.Opt)) {
            return r.SCtx;
        }
    }
    return exeValue;
}

const int16* Range(eTable table, uintptr_t rangesAddr, int32 numVoices, int16 sCtx, int16 voice, const int16* exeRange) {
    if (voice < 0 || voice >= numVoices) {
        return exeRange;
    }
    for (const auto& r : kMoveRules) {
        if (r.Table != table || (sCtx != r.Src && sCtx != r.Dst) || (r.OnlyVoice >= 0 && r.OnlyVoice != voice) || !On(r.Opt)) {
            continue;
        }
        const auto* const src = reinterpret_cast<const int16*>(rangesAddr + ((uintptr_t)r.Src * numVoices + voice) * 4);
        const auto* const dst = reinterpret_cast<const int16*>(rangesAddr + ((uintptr_t)r.Dst * numVoices + voice) * 4);
        // Como tryMoveContext: solo si el origen tiene sonidos y el destino esta vacio (el origen queda vacio)
        if (src[0] != -1 && src[1] != -1 && dst[0] == -1 && dst[1] == -1) {
            return sCtx == r.Dst ? src : kNoRange16;
        }
    }
    return exeRange;
}

const uint8* GenRange(uintptr_t rangesAddr, int32 numVoices, int16 sCtx, int16 voice, const uint8* exeRange) {
    if (voice < 0 || voice >= numVoices) {
        return exeRange;
    }
    for (const auto& r : kMoveRules) {
        if (r.Table != eTable::GEN || (sCtx != r.Src && sCtx != r.Dst) || (r.OnlyVoice >= 0 && r.OnlyVoice != voice) || !On(r.Opt)) {
            continue;
        }
        const auto* const src = reinterpret_cast<const uint8*>(rangesAddr + ((uintptr_t)r.Src * numVoices + voice) * 2);
        const auto* const dst = reinterpret_cast<const uint8*>(rangesAddr + ((uintptr_t)r.Dst * numVoices + voice) * 2);
        if (src[0] != 0xFF && src[1] != 0xFF && dst[0] == 0xFF && dst[1] == 0xFF) {
            return sCtx == r.Dst ? src : kNoRange8;
        }
    }
    return exeRange;
}

bool SpecialPedVoice(uint32 modelNameHash, int16& voice, int16& type, uint8& isFemale) {
    static const uint32 maccer = CKeyGen::GetUppercaseKey("maccer");
    if (modelNameHash == maccer && On(eOpt::MaccerVoice)) {
        voice    = VOICE_GNG_MACCER;
        type     = PED_TYPE_GANG;
        isFemale = 0;
        return true;
    }
    return false;
}

ePedSpeechVoiceS16 GetVoice(const char* name, eAudioPedTypeS16 type) {
    const auto result = CAEPedSpeechAudioEntity::GetVoice(name, type);
    if (result < 0 && std::strcmp(name, "0") != 0 && On(eOpt::PedVoiceNames)) {
        // peds.ide -> nombre del EXE (los dos escritos al reves/abreviados)
        static constexpr std::pair<const char*, const char*> typosAndFixes[] = {
            {"VOICE_GEN_BYMPI", "VOICE_GEN_BMYPI"},
            {"VOICE_GEN_WMYSGRD", "VOICE_GEN_WMYSGRAD"},
        };
        for (const auto& [exeName, ideName] : typosAndFixes) {
            if (std::strcmp(ideName, name) == 0) {
                return CAEPedSpeechAudioEntity::GetVoice(exeName, type);
            }
        }
    }
    return result;
}

void FixPedModelVoiceNames(const char* modelName, char* voice1, char* voice2) {
    if (!On(eOpt::PedVoiceNames)) {
        return;
    }
    // El nombre del modelo no distingue mayusculas; los de voz si
    const auto fix = [&](const char* model, const char* wrong, const char* right) {
        if (_stricmp(modelName, model) == 0 && std::strcmp(voice1, wrong) == 0 && std::strcmp(voice2, wrong) == 0) {
            strcpy_s(voice1, 60, right);
            strcpy_s(voice2, 60, right);
        }
    };
    fix("sbfystr", "VOICE_GEN_SBFYST", "VOICE_GEN_SBFYSTR");
    fix("swfystr", "VOICE_GEN_SWFYST", "VOICE_GEN_SWFYSTR");
}

eGlobalSpeechContext TurfContext(const CZone* zone) {
    // Como el EXE (0x444080..0x4442C1): _stricmp en este orden y sin coincidencia no dice nada. La etiqueta de East LS es "ELS".
    static constexpr std::pair<const char*, eGlobalSpeechContext> mapping[]{
        {"CHC", CTX_GLOBAL_TAKE_TURF_LAS_COLINAS},
        {"LFL", CTX_GLOBAL_TAKE_TURF_LOS_FLORES},
        {"EBE", CTX_GLOBAL_TAKE_TURF_EAST_BEACH},
        {"ELS", CTX_GLOBAL_TAKE_TURF_EAST_LS},
        {"JEF", CTX_GLOBAL_TAKE_TURF_JEFFERSON},
        {"GLN", CTX_GLOBAL_TAKE_TURF_GLEN_PARK},
        {"IWD", CTX_GLOBAL_TAKE_TURF_IDLEWOOD},
        {"GAN", CTX_GLOBAL_TAKE_TURF_GANTON},
        {"LMEX", CTX_GLOBAL_TAKE_TURF_LITTLE_MEXICO},
        {"LIND", CTX_GLOBAL_TAKE_TURF_WILLOWFIELD},
        {"PLS", CTX_GLOBAL_TAKE_TURF_PLAYA_DEL_SEVILLE},
        {"SUN", CTX_GLOBAL_TAKE_TURF_TEMPLE},
    };
    // SilentPatch (MIT, Silent): se compara la etiqueta de texto (+8), no la de informacion (+0) como hace el EXE
    const char* const label = On(eOpt::TurfCheers) ? zone->m_TextLabel : zone->m_InfoLabel;
    for (const auto& [l, context] : mapping) {
        if (_stricmp(label, l) == 0) {
            return context;
        }
    }
    return CTX_GLOBAL_NO_SPEECH;
}

bool PedHasGroupForSpeech(CPed* ped) {
    // SilentPatch (MIT, Silent): los policias nunca estan en un grupo de peds; con mas de uno persiguiendo se considera que si
    if (On(eOpt::CopsGroupLines) && ped->m_nPedType == PED_TYPE_COP) {
        const auto* const wanted = FindPlayerWanted(-1);
        return wanted && wanted->m_NumCopsInPursuit > 1;
    }
    return CPedGroups::GetPedsGroup(ped) != nullptr;
}

const char* ScannerLabel(int32 index, const char* exeLabel) {
    // SilentPatch (MIT, Silent): Queens (WESTP) y Temple se llaman bien en el escaner de policia (indices 113 y 134 de la lista de zonas)
    static constexpr char kWestp[8] = {'W', 'E', 'S', 'T', 'P', 0, 0, 0};
    static constexpr char kNone[8]  = {'?', '?', '?', '?', 0, 0, 0, 0};
    if (On(eOpt::ScannerNames)) {
        if (index == 113) {
            return kWestp;
        }
        if (index == 134) {
            return kNone;
        }
    }
    return exeLabel;
}

namespace {
CPed* s_chasingCop = nullptr;
}

bool IsPedFleeingFromCops(CPed* ped) {
    // Si la bandera esta puesta ya se dio el bonus una vez: no se vuelve a mirar el evento
    if (ped->bBeingChasedByPolice) {
        s_chasingCop = nullptr;
        return true;
    }
    CEvent* const event = ped->GetIntelligence()->GetEventHandler().GetHistory().GetCurrentEvent();
    if (event && event->GetEventType() == EVENT_PED_TO_FLEE) {
        CPed* const cop = static_cast<CEventPedToFlee*>(event)->m_ped;
        if (cop && cop->m_nPedType == PED_TYPE_COP) {
            s_chasingCop = cop;
            return true;
        }
    }
    return false;
}

bool GiveGoodCitizenBonus(CPed* ped) {
    if (ped->bBeingChasedByPolice) { // ya dado
        return false;
    }
    if (!ped->IsStateDying()) {
        ped->bBeingChasedByPolice = true;
        if (s_chasingCop) {
            s_chasingCop->Say(CTX_GLOBAL_GOOD_CITIZEN);
        }
        return true;
    }
    return false;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Banco en proceso. Reimplementa de forma independiente (copias locales de las tablas del EXE + el codigo de SilentPatch portado literalmente) y compara con lo
// que devuelven las funciones del juego con la opcion apagada (debe ser el dato del EXE) y encendida (debe ser el de SilentPatch).
// ---------------------------------------------------------------------------------------------------------------------------------------------------
namespace {
struct tTableSpec {
    const char* Name;
    eAudioPedType Type;
    uintptr_t Ranges;
    int32 NumVoices;
    int32 NumContexts;
    bool Is8;
};
constexpr tTableSpec kSpecs[] = {
    {"GEN", PED_TYPE_GEN, 0x8AF8A8, 0xD1, 0x68, true},
    {"EMG", PED_TYPE_EMG, 0x8BA4D0, 0x2E, 0x23, false},
    {"PLAYER", PED_TYPE_PLAYER, 0x8BBEF8, 0x14, 0x70, false},
    {"GANG", PED_TYPE_GANG, 0x8BE620, 0x34, 0x71, false},
    {"GFD", PED_TYPE_GFD, 0x8C42B0, 0x12, 0x79, false},
};

// Copia local de una tabla de rangos (siempre como int16 para comparar; los GEN con 0xFF = sin sonidos se guardan como -1)
using Copy = std::vector<std::array<int16, 2>>;
Copy CopyOf(const tTableSpec& s) {
    Copy c((size_t)s.NumContexts * s.NumVoices);
    for (int32 ctx = 0; ctx < s.NumContexts; ctx++) {
        for (int32 v = 0; v < s.NumVoices; v++) {
            auto& e = c[(size_t)ctx * s.NumVoices + v];
            if (s.Is8) {
                const auto* p = reinterpret_cast<const uint8*>(s.Ranges + ((uintptr_t)ctx * s.NumVoices + v) * 2);
                e = {(int16)(p[0] == 0xFF ? -1 : p[0]), (int16)(p[1] == 0xFF ? -1 : p[1])};
            } else {
                const auto* p = reinterpret_cast<const int16*>(s.Ranges + ((uintptr_t)ctx * s.NumVoices + v) * 4);
                e = {p[0], p[1]};
            }
        }
    }
    return c;
}

// tryMoveContext de SilentPatch (SilentPatchSA.cpp) sobre la copia
void SpTryMove(Copy& c, const tTableSpec& s, size_t voice, int16 src, int16 dst) {
    auto& a = c[(size_t)src * s.NumVoices + voice];
    auto& b = c[(size_t)dst * s.NumVoices + voice];
    if (a[0] != -1 && a[1] != -1 && b[0] == -1 && b[1] == -1) {
        b = a;
        a = {-1, -1};
    }
}

void SpPatch(Copy& c, const tTableSpec& s) {
    for (size_t i = 0; i < (size_t)s.NumVoices; i++) {
        switch (s.Type) {
        case PED_TYPE_GEN:
            SpTryMove(c, s, i, CTX_GEN_CHA, CTX_GEN_CHAT);
            SpTryMove(c, s, i, CTX_GEN_RUN_FROM_FIGH, CTX_GEN_RUN_FROM_FIGHT);
            SpTryMove(c, s, i, CTX_GEN_SAVE, CTX_GEN_SAVED);
            SpTryMove(c, s, i, CTX_GEN_SHOCKE, CTX_GEN_SHOCKED);
            break;
        case PED_TYPE_EMG: SpTryMove(c, s, i, CTX_EMG_ARREST_CRIMINAL, CTX_EMG_ARREST_CRIM); break;
        case PED_TYPE_GFD:
            SpTryMove(c, s, i, CTX_GFD_SHOOT_GENERIC, CTX_GFD_SHOP_CLOSED);
            SpTryMove(c, s, i, CTX_GFD_CAR_ON_FIRE, CTX_GFD_CAR_FIRE);
            break;
        case PED_TYPE_PLAYER: SpTryMove(c, s, i, CTX_PLY_JACKING_CAR_FEMALE, CTX_PLY_JACKING_CAR_FEM); break;
        case PED_TYPE_GANG: SpTryMove(c, s, i, CTX_GNG_ATTACK_PLAYER_LIKE, CTX_GNG_ATTACK_BY_PLAYER_LIKE); break;
        default: break;
        }
    }
    if (s.Type == PED_TYPE_GEN) {
        SpTryMove(c, s, VOICE_GEN_VWMOTR1, CTX_GEN_MUGGING, CTX_GEN_MUGGED);
    }
}
} // namespace

void AutoTest() {
    static bool done = false;
    static const bool enabled = [] {
        char v[8]{};
        return GetEnvironmentVariableA("GTASA_SILENTPATCH_SELFTEST_VOCES", v, sizeof(v)) != 0 && (v[0] == '1' || v[0] == '3'); // 1 = banco en proceso; 3 = banco en proceso y en juego
    }();
    if (done || !enabled) {
        return;
    }
    done = true;
    FILE* const f = fopen("logs/silentpatch_voces_selftest.log", "w");
    if (!f) {
        return;
    }
    fprintf(f, "Banco de voces y dialogos de SilentPatch (en proceso). Opciones por defecto (General=%d):", SilentPatchEnabled() ? 1 : 0);
    for (size_t i = 0; i < (size_t)eOpt::NUM; i++) {
        fprintf(f, " %s=%d", kOptNames[i], On((eOpt)i) ? 1 : 0);
    }
    fprintf(f, "\n");
    int totalMismatch = 0;

    // ---- tablas de rangos por voz ----
    for (const auto& s : kSpecs) {
        const Copy raw = CopyOf(s);
        Copy       sp  = raw;
        SpPatch(sp, s);
        int changed = 0;
        for (size_t i = 0; i < raw.size(); i++) {
            changed += raw[i] != sp[i];
        }
        for (const int state : {0, 1}) {
            ForceForTest(state);
            int mismatch = 0, compared = 0;
            for (int32 ctx = 0; ctx < s.NumContexts; ctx++) {
                for (int32 v = 0; v < s.NumVoices; v++) {
                    int16 first = -2, last = -2;
                    CAEPedSpeechAudioEntity::ProbeSoundRange(s.Type, (int16)ctx, (int16)v, first, last);
                    const auto& e = (state ? sp : raw)[(size_t)ctx * s.NumVoices + v];
                    // los GEN son bytes (0xFF = sin sonidos): se comparan como en la copia
                    const int16 f2 = s.Is8 ? (int16)((uint8)first == 0xFF ? -1 : (uint8)first) : first;
                    const int16 l2 = s.Is8 ? (int16)((uint8)last == 0xFF ? -1 : (uint8)last) : last;
                    compared++;
                    if (f2 != e[0] || l2 != e[1]) {
                        mismatch++;
                        if (mismatch <= 5) {
                            fprintf(f, "  DIFERENCIA %s ctx=%d voz=%d juego=(%d,%d) esperado=(%d,%d)\n", s.Name, ctx, v, f2, l2, e[0], e[1]);
                        }
                    }
                }
            }
            totalMismatch += mismatch;
            fprintf(f, "TABLA %-6s opcion=%s comparadas=%d entradas que SilentPatch cambia=%d diferencias=%d\n", s.Name, state ? "ENCENDIDA" : "APAGADA  ", compared, changed, mismatch);
        }
        // Detalle de las voces afectadas (encendida)
        for (int32 ctx = 0; ctx < s.NumContexts; ctx++) {
            for (int32 v = 0; v < s.NumVoices; v++) {
                const auto& a = raw[(size_t)ctx * s.NumVoices + v];
                const auto& b = sp[(size_t)ctx * s.NumVoices + v];
                if (a != b) {
                    fprintf(f, "  CAMBIO %-6s ctx=%3d voz=%3d: EXE (%d,%d) -> SilentPatch (%d,%d)\n", s.Name, ctx, v, a[0], a[1], b[0], b[1]);
                }
            }
        }
    }

    // ---- tabla global de contextos ----
    {
        constexpr uintptr_t G = 0x8C6A68;
        std::vector<std::array<int16, 8>> rows;
        for (int i = 0;; i++) {
            std::array<int16, 8> r;
            std::memcpy(r.data(), reinterpret_cast<const void*>(G + (uintptr_t)i * 16), 16);
            if (r[0] == -1) {
                rows.push_back(r); // centinela
                break;
            }
            rows.push_back(r);
        }
        auto sp = rows;
        for (const auto& rule : kGlobalRules) { // setSpecificSpeechContext de SilentPatch (con los datos de las reglas)
            for (auto& r : sp) {
                if (r[0] == -1) {
                    break;
                }
                if (r[0] == rule.GCtx) {
                    if (r[rule.PedType + 1] == -1) {
                        r[rule.PedType + 1] = rule.SCtx;
                    }
                    break;
                }
            }
        }
        const auto lookup = [](const std::vector<std::array<int16, 8>>& t, int16 g, int16 type) -> int16 {
            for (const auto& r : t) {
                if (r[0] == -1) {
                    return -1;
                }
                if (r[0] == g) {
                    return r[1 + type];
                }
            }
            return -1;
        };
        int changed = 0;
        for (int16 g = 0; g < CTX_GLOBAL_NUM; g++) {
            for (int16 t = 0; t < PED_TYPE_SPC; t++) {
                changed += lookup(rows, g, t) != lookup(sp, g, t);
            }
        }
        for (const int state : {0, 1}) {
            ForceForTest(state);
            int mismatch = 0, compared = 0;
            for (int16 g = 0; g < CTX_GLOBAL_NUM; g++) {
                for (int16 t = 0; t < PED_TYPE_SPC; t++) {
                    const auto game = CAEPedSpeechAudioEntity::GetSpecificSpeechContext((eGlobalSpeechContext)g, (eAudioPedType)t);
                    const auto exp  = lookup(state ? sp : rows, g, t);
                    compared++;
                    if (game != exp) {
                        mismatch++;
                        if (mismatch <= 5) {
                            fprintf(f, "  DIFERENCIA GLOBAL g=%d tipo=%d juego=%d esperado=%d\n", g, t, game, exp);
                        }
                    }
                }
            }
            totalMismatch += mismatch;
            fprintf(f, "TABLA GLOBAL opcion=%s comparadas=%d entradas que SilentPatch asigna=%d diferencias=%d\n", state ? "ENCENDIDA" : "APAGADA  ", compared, changed, mismatch);
        }
    }

    // ---- extremo a extremo: GetSoundAndBankIDs con la voz afectada ----
    {
        struct tCase {
            const char* Name;
            eAudioPedType Type;
            int16 Voice;
            eGlobalSpeechContext GCtx;
        };
        const tCase cases[] = {
            {"VBFYST2 CHAT", PED_TYPE_GEN, VOICE_GEN_VBFYST2, CTX_GLOBAL_CHAT},
            {"VBFYST2 RUN_FROM_FIGHT", PED_TYPE_GEN, VOICE_GEN_VBFYST2, CTX_GLOBAL_RUN_FROM_FIGHT},
            {"VBFYST2 SAVED", PED_TYPE_GEN, VOICE_GEN_VBFYST2, CTX_GLOBAL_SAVED},
            {"VBFYST2 SHOCKED", PED_TYPE_GEN, VOICE_GEN_VBFYST2, CTX_GLOBAL_SHOCKED},
            {"VWMOTR1 MUGGED", PED_TYPE_GEN, VOICE_GEN_VWMOTR1, CTX_GLOBAL_MUGGED},
            {"LAPDM1 ARREST_CRIM", PED_TYPE_EMG, 27, CTX_GLOBAL_ARREST_CRIM},
            {"FAM5 (modelo FAM3) ATTACK_BY_PLAYER_LIKE", PED_TYPE_GANG, VOICE_GNG_FAM5, CTX_GLOBAL_ATTACK_BY_PLAYER_LIKE},
            {"GFD voz1 SHOP_CLOSED", PED_TYPE_GFD, 1, CTX_GLOBAL_SHOP_CLOSED},
            {"GFD voz9 CAR_FIRE", PED_TYPE_GFD, 9, CTX_GLOBAL_CAR_FIRE},
            {"GFD voz1 CONV_IGNORED", PED_TYPE_GFD, 1, CTX_GLOBAL_CONV_IGNORED},
            {"GANG FAM3 ABUSE_TRIAD", PED_TYPE_GANG, VOICE_GNG_FAM3, CTX_GLOBAL_ABUSE_TRIAD},
            {"GANG FAM3 ABUSE_DA_NANG", PED_TYPE_GANG, VOICE_GNG_FAM3, CTX_GLOBAL_ABUSE_DA_NANG},
            {"GANG LSV1 ABUSE_RIFA", PED_TYPE_GANG, VOICE_GNG_LSV1, CTX_GLOBAL_ABUSE_RIFA},
        };
        for (const auto& c : cases) {
            fprintf(f, "EXTREMO A EXTREMO %-28s", c.Name);
            for (const int state : {0, 1}) {
                ForceForTest(state);
                int16 id = -1, first = -1;
                const bool ok = CAEPedSpeechAudioEntity::ProbeGetSoundAndBankIDs(c.Type, c.Voice, c.GCtx, id, first);
                fprintf(f, "  %s: %s%s", state ? "ENCENDIDA" : "APAGADA", ok ? "suena" : "mudo", ok ? (" (sonidos desde " + std::to_string(first) + ", elegido " + std::to_string(id) + ")").c_str() : "");
            }
            fprintf(f, "\n");
        }
    }

    // ---- extremo a extremo de cada asignacion de la tabla global: primera voz del tipo con sonidos en ese contexto especifico (datos del EXE) ----
    for (const auto& rule : kGlobalRules) {
        if (rule.PedType == PED_TYPE_PLAYER) {
            continue; // CJ depende del humor: lo cubre el banco en juego
        }
        const tTableSpec* spec = nullptr;
        for (const auto& sp : kSpecs) {
            if (sp.Type == (eAudioPedType)rule.PedType) {
                spec = &sp;
            }
        }
        int16 voice = -1, first = 0, last = 0;
        int   nVoices = 0;
        ForceForTest(0);
        for (int16 v = 0; spec && v < spec->NumVoices; v++) {
            if (CAEPedSpeechAudioEntity::ProbeSoundRange((eAudioPedType)rule.PedType, rule.SCtx, v, first, last) && (spec->Is8 ? (uint8)first != 0xFF : first != -1)) {
                nVoices++;
                if (voice < 0) {
                    voice = v;
                }
            }
        }
        fprintf(f, "ASIGNACION global=%3d tipo=%d -> especifico=%3d: voces con sonidos=%d", rule.GCtx, rule.PedType, rule.SCtx, nVoices);
        if (voice >= 0) {
            for (const int state : {0, 1}) {
                ForceForTest(state);
                int16 id = -1, f0 = -1;
                const bool ok = CAEPedSpeechAudioEntity::ProbeGetSoundAndBankIDs((eAudioPedType)rule.PedType, voice, (eGlobalSpeechContext)rule.GCtx, id, f0);
                fprintf(f, "  voz %d %s: %s", voice, state ? "ENCENDIDA" : "APAGADA", ok ? "suena" : "mudo");
            }
        }
        fprintf(f, "\n");
    }

    // ---- ped especial Maccer ----
    for (const char* name : {"maccer", "maddogg", "macca"}) {
        fprintf(f, "PED ESPECIAL %-8s", name);
        for (const int state : {0, 1}) {
            ForceForTest(state);
            int16 v = -9, t = -9, fem = -9;
            const bool ok = CAEPedSpeechAudioEntity::ProbeSpecialPed(CKeyGen::GetUppercaseKey(name), v, t, fem);
            fprintf(f, "  %s: %s (voz=%d tipo=%d mujer=%d)", state ? "ENCENDIDA" : "APAGADA", ok ? "con voz" : "sin voz", ok ? v : -1, ok ? t : -1, ok ? fem : -1);
        }
        fprintf(f, "\n");
    }

    // ---- nombres de voz del peds.ide ----
    for (const char* name : {"VOICE_GEN_BMYPI", "VOICE_GEN_WMYSGRAD", "VOICE_GEN_BYMPI", "VOICE_GEN_WMYSGRD", "VOICE_GEN_VBFYST2", "VOICE_GEN_SBFYST", "VOICE_GEN_SBFYSTR"}) {
        fprintf(f, "VOZ DEL IDE %-20s", name);
        for (const int state : {0, 1}) {
            ForceForTest(state);
            fprintf(f, "  %s: %d", state ? "ENCENDIDA" : "APAGADA", (int)GetVoice(name, PED_TYPE_GEN));
        }
        fprintf(f, "\n");
    }
    for (const char* model : {"sbfystr", "SWFYSTR", "sbfyst"}) {
        fprintf(f, "MODELO %-8s (voces SBFYST/SWFYST del peds.ide)", model);
        for (const int state : {0, 1}) {
            ForceForTest(state);
            char v1[60], v2[60];
            strcpy_s(v1, _stricmp(model, "swfystr") == 0 ? "VOICE_GEN_SWFYST" : "VOICE_GEN_SBFYST");
            strcpy_s(v2, v1);
            FixPedModelVoiceNames(model, v1, v2);
            fprintf(f, "  %s: %s / %s", state ? "ENCENDIDA" : "APAGADA", v1, v2);
        }
        fprintf(f, "\n");
    }

    // ---- celebracion de territorio ----
    for (const auto& [info, text] : {std::pair{"XXX", "CHC"}, std::pair{"CHC", "XXX"}, std::pair{"ELS", "ELS"}, std::pair{"SUN", "ZZZ"}, std::pair{"ZZZ", "SUN"}}) {
        CZone z{};
        strcpy_s(z.m_InfoLabel, info);
        strcpy_s(z.m_TextLabel, text);
        fprintf(f, "TURF info=%s texto=%s", info, text);
        for (const int state : {0, 1}) {
            ForceForTest(state);
            fprintf(f, "  %s: contexto %d", state ? "ENCENDIDA" : "APAGADA", (int)TurfContext(&z));
        }
        fprintf(f, "\n");
    }

    // ---- escaner de policia ----
    for (const int32 idx : {112, 113, 134, 135}) {
        const char* exe = reinterpret_cast<const char*>(0x8C8198) + idx * 8;
        fprintf(f, "ESCANER indice %d EXE=%.8s", idx, exe);
        for (const int state : {0, 1}) {
            ForceForTest(state);
            fprintf(f, "  %s: %.8s", state ? "ENCENDIDA" : "APAGADA", ScannerLabel(idx, exe));
        }
        fprintf(f, "\n");
    }

    ForceForTest(-1);
    fprintf(f, "RESULTADO: diferencias totales en las tablas = %d\n", totalMismatch);
    fclose(f);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------------
// Banco en juego: ver el encabezado. Una accion cada 60 fotogramas (CJ no puede decir dos frases a la vez ni repetir el mismo contexto enseguida).
// ---------------------------------------------------------------------------------------------------------------------------------------------------
void GameTimeTick() {
    static const bool enabled = [] {
        char v[8]{};
        return GetEnvironmentVariableA("GTASA_SILENTPATCH_SELFTEST_VOCES", v, sizeof(v)) != 0 && (v[0] == '2' || v[0] == '3');
    }();
    if (!enabled) {
        return;
    }
    static int   frames = 0, step = -1;
    static FILE* f      = nullptr;
    auto* const  player = FindPlayerPed();
    if (!player || !player->IsAlive() || player->bInVehicle) {
        frames = 0;
        return;
    }
    if (++frames < 240 || (frames - 240) % 60 != 0) { // ~4 s de juego y luego una accion cada 60 fotogramas
        return;
    }
    struct tSay {
        const char*          Name;
        eCJMood              Mood;
        int16                Gang, Fat, Dressed;
        eGlobalSpeechContext GCtx;
    };
    static const tSay say[] = {
        {"CJ JACKING_CAR_FEM (humor CD)", MOOD_CR, 0, 0, 1, CTX_GLOBAL_JACKING_CAR_FEM},
        {"CJ JACKING_CAR_FEM (humor CG)", MOOD_CR, 1, 0, 0, CTX_GLOBAL_JACKING_CAR_FEM},
        {"CJ JACKING_CAR_FEM (humor WG)", MOOD_WR, 1, 0, 0, CTX_GLOBAL_JACKING_CAR_FEM},
        {"CJ JACKING_CAR_FEM (humor CR)", MOOD_CR, 0, 0, 0, CTX_GLOBAL_JACKING_CAR_FEM},
        {"CJ WEATHER_DISL_REPLY (humor CR)", MOOD_CR, 0, 0, 0, CTX_GLOBAL_WEATHER_DISL_REPLY},
        {"CJ WEATHER_LIKE_REPLY (humor CR)", MOOD_CR, 0, 0, 0, CTX_GLOBAL_WEATHER_LIKE_REPLY},
        {"CJ TAKE_TURF_GANTON (humor CR)", MOOD_CR, 0, 0, 0, CTX_GLOBAL_TAKE_TURF_GANTON},
        {"CJ TAKE_TURF_TEMPLE (humor CR)", MOOD_CR, 0, 0, 0, CTX_GLOBAL_TAKE_TURF_TEMPLE},
    };
    constexpr int nSay = (int)std::size(say);
    if (step < 0) {
        f = fopen("logs/silentpatch_voces_juego.log", "w");
        if (!f) {
            step = 1 << 20;
            return;
        }
        fprintf(f, "Banco en juego de voces de SilentPatch. Opciones por defecto (General=%d):", SilentPatchEnabled() ? 1 : 0);
        for (size_t i = 0; i < (size_t)eOpt::NUM; i++) {
            fprintf(f, " %s=%d", kOptNames[i], On((eOpt)i) ? 1 : 0);
        }
        fprintf(f, "\n");
        // voces que el peds.ide da a los modelos con nombre de voz mal escrito (se leen del ModelInfo ya cargado)
        for (const int id : {71, 244, 249, 256, 257}) {
            const auto* const mi = CModelInfo::GetModelInfo(id) ? CModelInfo::GetModelInfo(id)->AsPedModelInfoPtr() : nullptr;
            if (mi) {
                fprintf(f, "MODELO %d: tipo de audio=%d voz minima=%d voz maxima=%d voz actual=%d\n", id, (int)mi->m_nPedAudioType, (int)mi->m_nVoiceMin, (int)mi->m_nVoiceMax, (int)mi->m_nVoiceId);
            }
        }
        // peatones reales de la calle: tipo de voz, voz y si hablarian del tiempo (CONV_DISL_WEATHER)
        int total = 0, weather = 0;
        auto* const pool = GetPedPool();
        for (int32 i = 0; i < (int32)pool->GetSize(); i++) {
            auto* const ped = pool->GetAt(i);
            if (!ped || ped == player) {
                continue;
            }
            total++;
            weather += ped->GetSpeechAE().WillPedChatAboutTopic(7) ? 1 : 0;
        }
        fprintf(f, "PEATONES con voz en el pool: %d, de ellos pueden hablar del tiempo: %d\n", total, weather);
        step = 0;
    }
    if (step == 0) {
        // rangos de sonidos del contexto JACKING_CAR_FEM de las voces del jugador afectadas (SilentPatch los mueve desde JACKING_CAR_FEMALE)
        for (const int16 voice : {0, 4, 8, 16, 10}) {
            fprintf(f, "RANGO jugador voz %d JACKING_CAR_FEM:", (int)voice);
            for (const int st : {0, 1}) {
                ForceForTest(st);
                int16 first = -2, last = -2;
                CAEPedSpeechAudioEntity::ProbeSoundRange(PED_TYPE_PLAYER, CTX_PLY_JACKING_CAR_FEM, voice, first, last);
                fprintf(f, "  %s=(%d,%d)", st ? "ENCENDIDA" : "APAGADA", (int)first, (int)last);
            }
            fprintf(f, "\n");
        }
        ForceForTest(-1);
    }
    if (step >= 2 * nSay) {
        if (step == 2 * nSay) {
            // celebracion de territorio: CheerVictory con una zona de pruebas (etiqueta de texto "CHC"; la de informacion no)
            CZone zone{};
            strcpy_s(zone.m_InfoLabel, "XXX");
            strcpy_s(zone.m_TextLabel, "CHC");
            auto* const saved = CGangWars::pZoneToFightOver;
            CGangWars::pZoneToFightOver = &zone;
            for (const int state : {0, 1}) {
                ForceForTest(state);
                player->GetSpeechAE().StopCurrentSpeech();
                CGangWars::CheerVictory();
                fprintf(f, "CheerVictory zona(info=XXX texto=CHC) %s: CJ habla=%d\n", state ? "ENCENDIDA" : "APAGADA", (int)player->GetSpeechAE().GetPedTalking());
            }
            CGangWars::pZoneToFightOver = saved;
            // conversacion "de donde eres" (L144): se pone a un peaton cercano en el estado PEDHASOPENED (tema 8, apertura positiva) y se pulsa "si" (cruceta derecha) en el pad 0
            CPed* talker = nullptr;
            for (int32 i = 0; i < (int32)GetPedPool()->GetSize() && !talker; i++) {
                auto* const p = GetPedPool()->GetAt(i);
                talker = (p && p != player && p->IsAlive()) ? p : nullptr;
            }
            const char* const names[] = {"INACTIVE", "PEDHASOPENED", "WAITINGFORFINALWORD", "WAITINGTOFINISH"};
            if (talker) {
                using Conv = CPedToPlayerConversations;
                for (const int state : {0, 1}) {
                    ForceForTest(state);
                    CPad* const pad = CPad::GetPad(0);
                    Conv::m_pPed             = talker;
                    Conv::m_Topic            = 8;
                    Conv::m_bPositiveOpening = true;
                    Conv::m_State            = Conv::eP2pState::PEDHASOPENED;
                    Conv::m_StartTime        = CTimer::GetTimeInMS();
                    const auto oldNew = pad->NewState.DPadRight, oldOld = pad->OldState.DPadRight;
                    pad->NewState.DPadRight = 255;
                    pad->OldState.DPadRight = 0;
                    Conv::Update();
                    pad->NewState.DPadRight = oldNew;
                    pad->OldState.DPadRight = oldOld;
                    const auto afterYes = Conv::m_State;
                    uint32 afterWait = 99;
                    if (afterYes == Conv::eP2pState::WAITINGFORFINALWORD) {
                        Conv::m_StartTime = CTimer::GetTimeInMS() - 3500; // pasan los 3 s de espera de la ultima palabra
                        Conv::Update();
                        afterWait = (uint32)Conv::m_State;
                    }
                    fprintf(f, "CONVERSACION de donde eres (si) %s: tras pulsar si estado=%s; tras 3,5 s estado=%s\n", state ? "ENCENDIDA" : "APAGADA  ", names[(uint32)afterYes < 4 ? (uint32)afterYes : 0], afterWait < 4 ? names[afterWait] : "(no aplica)");
                    Conv::EndConversation();
                    Conv::m_State = Conv::eP2pState::INACTIVE;
                }
            } else {
                fprintf(f, "CONVERSACION: no hay peaton cercano en el pool\n");
            }
            // Good Citizen Bonus (L145): una victima que huye de un policia (evento EVENT_PED_TO_FLEE) y el jugador la golpea: +50 $ una sola vez
            {
                CPed *victim = nullptr, *copLike = nullptr;
                for (int32 i = 0; i < (int32)GetPedPool()->GetSize(); i++) {
                    auto* const p = GetPedPool()->GetAt(i);
                    if (p && p != player && p->IsAlive() && p->m_nPedType != PED_TYPE_COP) {
                        (victim ? copLike : victim) = p;
                        if (copLike) {
                            break;
                        }
                    }
                }
                if (victim && copLike) {
                    for (const int state : {0, 1}) {
                        ForceForTest(state);
                        auto&      players  = CWorld::Players[CWorld::PlayerInFocus];
                        const auto oldType  = copLike->m_nPedType;
                        copLike->m_nPedType = PED_TYPE_COP;
                        victim->bBeingChasedByPolice = false;
                        auto& handler = victim->GetIntelligence()->GetEventHandler();
                        handler.ResetHistory();
                        CEventPedToFlee flee{copLike};
                        handler.GetHistory().RecordCurrentEvent(victim, flee); // CEventHandler::RecordActiveEvent no esta reconstruida (assert); esto es lo que hace el EXE al registrar el evento en curso
                        const auto m0 = players.m_nMoney;
                        fprintf(f, "  (diagnostico: nivel de busqueda=%d, tipo del evento actual de la victima=%d, IsPedFleeingFromCops=%d, victima valida=%d)\n", (int)player->GetWantedLevel(), (int)handler.GetCurrentEventType(), (int)IsPedFleeingFromCops(victim), (int)IsPedPointerValid(victim));
                        CCrime::ReportCrime(CRIME_DAMAGED_PED, victim, player);
                        const auto m1 = players.m_nMoney;
                        const bool flag1 = victim->bBeingChasedByPolice;
                        FindPlayerWanted()->SetWantedLevel(eWantedLevel::WANTED_CLEAN);
                        CCrime::ReportCrime(CRIME_DAMAGED_PED, victim, player); // segundo golpe: no se da otra vez
                        const auto m2 = players.m_nMoney;
                        fprintf(f, "GOOD CITIZEN %s: dinero +%d tras el primer golpe (bandera de bonus dado=%d), +%d tras el segundo\n", state ? "ENCENDIDA" : "APAGADA  ", (int)(m1 - m0), (int)flag1, (int)(m2 - m1));
                        FindPlayerWanted()->SetWantedLevel(eWantedLevel::WANTED_CLEAN);
                        copLike->m_nPedType = oldType;
                        victim->bBeingChasedByPolice = false;
                        handler.ResetHistory();
                    }
                } else {
                    fprintf(f, "GOOD CITIZEN: no hay dos peatones en el pool\n");
                }
            }
            ForceForTest(-1);
            fprintf(f, "FIN\n");
            fclose(f);
            f = nullptr;
            step++;
        }
        return;
    }
    const auto& c     = say[step / 2];
    const int   state = step % 2;
    ForceForTest(state);
    CAEPedSpeechAudioEntity::SetCJMood(c.Mood, 30000, c.Gang, c.Fat, c.Dressed);
    player->GetSpeechAE().StopCurrentSpeech();
    const uint32 now = CTimer::GetTimeInMS(), next = player->GetSpeechAE().GetNextPlayTime(c.GCtx);
    const auto r = player->Say(c.GCtx, 0, 1.0f, true, true, true);
    fprintf(f, "DECIR %-36s %s: AddSayEvent=%d (%s) [reloj=%u proxima vez permitida=%u]\n", c.Name, state ? "ENCENDIDA" : "APAGADA  ", (int)r, r >= 0 ? "suena" : "mudo", now, next);
    fflush(f);
    ForceForTest(-1);
    step++;
}

} // namespace SilentPatchSpeech
