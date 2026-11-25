#pragma once

/*
 * Correcciones de voces y dialogos tomadas de SilentPatch (The MIT License, Copyright (c) 2024 Adrian Zdanowicz (Silent);
 * ver docs/THIRD_PARTY_NOTICES.md). Son DESVIACIONES DELIBERADAS del EXE y opcionales (docs/DesviacionesConocidas.md, seccion 15.4);
 * con el interruptor general apagado (GTASA_SILENTPATCH=0) o con la opcion de cada grupo apagada el comportamiento es el del EXE.
 *
 * Aqui estan las TABLAS propias (solo las entradas que SilentPatch cambia respecto a las tablas de voz del EXE 1.0 US) y las funciones
 * pequenas que usan los sitios de llamada. Las tablas del EXE (0x8C6A68, 0x8AF8A8, ...) no se modifican: CAEPedSpeechAudioEntity las
 * consulta a traves de las funciones de este archivo (con la opcion apagada devuelven el dato del EXE tal cual).
 * Referencia: docs/CodigoVivoExe/silentpatch2-voces.md y docs/CodigoVivoExe/silentpatch-clasificacion/voces.csv.
 */

#include <Base.h>
#include <Audio/Enums/PedSpeechContexts.h>
#include <Audio/Enums/PedSpeechVoices.h>
#include <Audio/Enums/ePedAudioType.h>

class CPed;
class CZone;

namespace SilentPatchSpeech {

// Una opcion por grupo (nombre = cadena de SilentPatchOption; GTASA_SILENTPATCH_<NOMBRE EN MAYUSCULAS> / SilentPatch_<Nombre> en el ini).
enum class eOpt : uint8 {
    WeatherConversations,  // L125 SpeechWeatherConversations
    CriminalsFlee,         // L126 SpeechCriminalsFlee
    PedVoiceNames,         // L127/L128 SpeechPedVoiceNames (WMYSGRD, BMYPIMP, SBFYSTR, SWFYSTR)
    MaccerVoice,           // L127 SpeechMaccerVoice
    VBFYST2,               // L127 SpeechVBFYST2
    TurfCheers,            // L129 SpeechTurfCheers
    SolicitMood,           // L130 SpeechSolicitMood
    GfdLines,              // L131 SpeechGirlfriendShopkeeperLines
    ShopSpooked,           // L132 SpeechShopkeepersSpooked
    VWMOTR1,               // L133 SpeechVWMOTR1
    GangTaunts,            // L134 SpeechGangTaunts
    CJJackingWomen,        // L135 SpeechCJJackingCars
    CrashGeneric,          // L136 SpeechCrashGenericFallback
    PainSprayed,           // L137 SpeechPainSprayed
    ArrestChased,          // L138 SpeechChasedOneStar
    ShootMouse,            // L139 SpeechShootMouseKeyboard
    CopsGroupLines,        // L140 SpeechCopsCoverLines
    FAM3,                  // L141 SpeechFAM3
    LAPDM1,                // L142 SpeechLAPDM1
    GangAttackPlayer,      // L143 SpeechGangAttackPlayer
    WhereYouFrom,          // L144 SpeechWhereYouFromReply
    GoodCitizen,           // L145 GoodCitizenBonus
    DruggedRecruit,        // L146 SpeechDruggedRecruit
    CarHitPed,             // L202 SpeechCarHitPedPassenger
    ScannerNames,          // L184 PoliceScannerNames
    NUM
};

bool On(eOpt opt);
// Solo para pruebas: -1 = segun la configuracion; 0/1 = fuerza todas las opciones de voz (el interruptor general se ignora).
void ForceForTest(int state);

// ---- Tablas de voz (CAEPedSpeechAudioEntity) ----
enum class eTable : uint8 { GEN, EMG, PLAYER, GANG, GFD, PAIN };

// Contexto especifico por tipo de voz de un contexto global (fila de 0x8C6A68): solo asigna si el EXE tiene -1.
int16 SpecificContext(int16 gCtx, int16 pedAudioType, int16 exeValue);
// Rangos [primero, ultimo] de sonidos por [contexto especifico][voz] (int16 en EMG/PLAYER/GANG/GFD/PAIN).
const int16* Range(eTable table, uintptr_t rangesAddr, int32 numVoices, int16 sCtx, int16 voice, const int16* exeRange);
// Idem para los peatones genericos (uint8; 0xFF = sin sonidos).
const uint8* GenRange(uintptr_t rangesAddr, int32 numVoices, int16 sCtx, int16 voice, const uint8* exeRange);
// Voz de los peds especiales (modelo "maccer"): true si SilentPatch la anade.
bool SpecialPedVoice(uint32 modelNameHash, int16& voice, int16& type, uint8& isFemale);

// ---- Nombres de voz del peds.ide (L127/L128) ----
// GetVoice con los dos errores tipograficos conocidos (VOICE_GEN_BMYPI -> VOICE_GEN_BYMPI, VOICE_GEN_WMYSGRAD -> VOICE_GEN_WMYSGRD).
ePedSpeechVoiceS16 GetVoice(const char* name, eAudioPedTypeS16 type);
// SBFYSTR/SWFYSTR: el peds.ide les da la voz de SBFYST/SWFYST.
void FixPedModelVoiceNames(const char* modelName, char* voice1, char* voice2);

// ---- Pequenas piezas de logica ----
// L129: contexto global de la celebracion de territorio a partir de la zona (etiqueta de texto con la opcion, de informacion como el EXE).
eGlobalSpeechContext TurfContext(const CZone* zone);
// L140: "tiene grupo" para las frases de rodear/cubrirme: los policias cuentan si hay mas de uno persiguiendo.
bool PedHasGroupForSpeech(CPed* ped);
// L184: etiqueta (8 bytes) de la entrada `index` de la lista del escaner de policia (0x8C8198).
const char* ScannerLabel(int32 index, const char* exeLabel);
// L145: Good Citizen Bonus. ReportCrime: true si hay bonus que dar / ya dado (sustituye la comprobacion de la bandera heredada).
bool IsPedFleeingFromCops(CPed* ped);
// ... y, si no esta muriendo ni se dio ya, lo marca, hace que el policia lo felicite y devuelve true.
bool GiveGoodCitizenBonus(CPed* ped);

// Banco en proceso (GTASA_SILENTPATCH_SELFTEST_VOCES=1 o 3): vuelca logs/silentpatch_voces_selftest.log. Se llama desde CAEPedSpeechAudioEntity::StaticInitialise.
void AutoTest();
// Banco en juego (GTASA_SILENTPATCH_SELFTEST_VOCES=2 o 3): con el jugador ya creado, hace decir frases a CJ con las opciones apagadas y encendidas y vuelca
// logs/silentpatch_voces_juego.log (voces de los modelos de peds del peds.ide incluidas). Se llama cada fotograma desde CAEPedSpeechAudioEntity::Service.
void GameTimeTick();

} // namespace SilentPatchSpeech
