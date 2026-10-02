#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../../source/game_sa/Tasks/TaskTypes/Interior/TaskInteriorSitAtDesk.h"
#include "../../source/game_sa/Interior/InteriorInfo_t.h"
#include "../../source/game_sa/Interior/InteriorManager_c.h"
#include "../../source/game_sa/Animation/AnimManager.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"

// Banco local de CTaskInteriorSitAtDesk::ProcessPed (0x677920). Alterna el hook ya
// instalado en la misma dirección, usando el jugador real (clump real) pero una
// instancia sintética e independiente de CTaskInteriorSitAtDesk por cada lado del
// par, siguiendo el mismo patrón que CEntryExitTransition/CTaskSimpleUseGun.
//
// LÍMITE DOCUMENTADO sobre m_bTaskAborting (rama de MakeAbortable/aborto no
// inmediato): esta rama empieza siempre llamando a
// InteriorManager_c::AreAnimsLoaded(2), que resuelve "int_office" con
// GetAnimationBlockIndex(const char*) (índice dentro de ms_aAnimBlocks, que
// crece dinámicamente según el orden real de streaming) y LUEGO usa ESE MISMO
// índice como si fuera un AssocGroupId para `GetAnimationBlock((AssocGroupId)
// idx)->IsLoaded`, sin comprobar null — confirmado con dumpbin que el ASM
// original (0x5980F0) hace exactamente eso. En una partida nueva estándar
// "int_office" ocupa, por el orden normal de carga, la posición 78, que NO
// coincide con su propio AssocGroupId real (ANIM_GROUP_INT_OFFICE=85).
// Se probaron tres formas de satisfacer esta precondición dentro de un banco
// aislado: (1) forzar el streaming del IFP y llamar
// CAnimManager::CreateAnimAssocGroups() manualmente — GetAnimationBlock
// seguía devolviendo null y el ASM original abortaba con acceso a memoria
// inválido (0xC0000005, confirmado con WinEvent: `movzx eax,[eax+10h]` sobre
// un CAnimBlock* nulo); (2) interceptar AreAnimsLoaded con un parche de
// entrada igual que RNG/BlendAnimation — abortaba con una excepción de
// instrucción privilegiada (0xC0000096) en memoria fuera de cualquier módulo
// cargado; (3) enlazar manualmente GetAssocGroups()[78].m_AnimBlock al
// CAnimBlock real de "int_office" (saltándose por completo el problema de
// índices) y forzar el hook de AreAnimsLoaded a reconstruido — la llamada
// aislada a InteriorManager_c::AreAnimsLoaded(2) funcionó bien (devolvió
// `true`), pero el MISMO caso, ejecutado dentro de ProcessPed con el hook de
// ProcessPed deshabilitado (lado "original"), seguía abortando con acceso a
// memoria inválido (0xC0000005); con `__try`/`__except` alrededor de la
// llamada se confirmó que el aborto ocurre solo en el lado "original", nunca
// en el candidato, y es independiente del estado del hook de AreAnimsLoaded.
// No se identificó la causa exacta dentro del tiempo disponible: todo apunta
// a que el ASM original de ProcessPed, al ejecutarse de forma aislada sin
// haber pasado antes por la secuencia normal de inicialización de una
// partida real, depende de algún estado adicional del motor no replicado
// aquí.
//
// CUARTO INTENTO (misma línea de investigación, sesión posterior): se
// localizó CPed::Initialise (0x5DEBB0, llamado una sola vez durante el
// arranque normal del juego, antes de que exista cualquier CPed jugable) ->
// CCarEnterExit::SetAnimOffsetForEnterOrExitVehicle (0x64F860, también
// guardado por ms_bPedOffsetsCalculated para no repetirse), que YA solicita
// y carga "int_house"/"int_office" por el camino normal del streaming
// (CStreaming::RequestModel + LoadAllRequestedModels), exactamente como
// ocurre en una partida real. La hipótesis era que, para cuando la sonda
// alcanza "player_ready" (después de que el juego cargue una partida real),
// este paso ya ocurrió y AreAnimsLoaded(2) debería resolver sin parches ni
// streaming manual. Se añadió un diagnóstico de solo lectura (sin parchear
// nada, con __try/__except aislado en su propia función) que llamaba a
// InteriorManager_c::AreAnimsLoaded(2) una sola vez justo después de
// "player_ready", antes del banco de 144 pares. Al probarlo repetidamente
// (6 ejecuciones consecutivas vía el runner compartido, más una ejecución
// manual observada directamente con PowerShell/Get-Process), el juego real
// nunca llegó a escribir "player_ready" en ninguna de ellas, ni dentro de
// 500 s ni observado vivo durante 75 s con el proceso corriendo y
// respondiendo (Get-Process confirmó "Responding=True"): se quedó
// indefinidamente en "waiting_for_player", es decir, ANTES de que el propio
// diagnóstico pudiera ejecutarse siquiera una vez. Como ese tramo de código
// (FindPlayerPed/CCutsceneMgr::IsRunning/InjectHooks/búsqueda del hook) es
// idéntico byte a byte al de la versión ya validada con 144 pares, no hay
// evidencia de que el diagnóstico en sí causara el atasco; es más probable
// que se debiera a contención de recursos del sistema en el momento de la
// prueba (el Visor de sucesos de Windows mostraba, en paralelo, un servicio
// de terceros —Daemon Tools— reiniciándose en bucle cada ~10 minutos durante
// toda la ventana de pruebas). El diagnóstico se retiró (revertido a este
// mismo archivo) sin dejar ningún cambio sin probar en el banco; los 144
// pares ya aprobados no se tocaron ni se repitieron. No se descarta repetir
// este cuarto intento en una sesión con el sistema en reposo: la hipótesis
// (CPed::Initialise ya carga "int_office" por el camino normal, antes de
// que exista ningún jugador) sigue sin refutarse, solo sin confirmarse en
// vivo por inestabilidad del entorno de pruebas, no del propio código.
// Por seguridad, el banco deja m_bTaskAborting siempre en `false`
// (bit decodificado pero descartado en Fixture) y NO cubre: el retorno
// anticipado `return true` cuando AreAnimsLoaded es falso, las transiciones
// de blend delta/velocidad de m_bTaskAborting para IN/rango IDLE_LOOP..WATCH/
// 2IDLE_180, ni StartGettingUp cuando aborta. Limitación explícita; no se
// afirma que ProcessPed tenga un error real, solo que esta rama no pudo
// validarse de forma segura en un banco aislado.
// FASE C (sesión 2026-10-01, tras CTaskInteriorUseInfo): CAUSA RAÍZ de los
// intentos 1-5 identificada y corregida en el C++. InteriorManager_c::AreAnimsLoaded
// (0x5980F0) es __thiscall en el original (el llamador hace `mov ecx,0BAF670h;
// push 2; call 5980F0` y la función termina con `ret 4`), pero estaba declarada
// `static` (cdecl, `ret` sin limpiar). Cuando el ASM ORIGINAL de ProcessPed la
// llamaba a través del hook, la pila quedaba 4 bytes desplazada y el epílogo del
// original saltaba a basura (0xC0000005 solo en el lado original). Además la
// reconstrucción leía GetAnimationBlock((AssocGroupId)idx)->IsLoaded (tabla de
// grupos, puntero posiblemente nulo) en vez de ms_aAnimBlocks[idx].IsLoaded como
// el original (`shl eax,5; mov cl,[eax+0B5D4B0h]`). Lo que se afirmó en los
// intentos anteriores ("el original hace exactamente eso") era FALSO.
// Con ambas correcciones, los pares 144..311 cubren m_bTaskAborting y !m_Anim.
// IsLoaded del bloque "int_office" se fija por caso y se restaura tras cada lado;
// el lado original deshabilita también el hook de AreAnimsLoaded.
namespace SitAtDeskLive {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& r) { log << r.dump() << '\n'; log.flush(); }
inline int Report(int type, char* msg, int*) {
    if (type != _CRT_ASSERT) return FALSE;
    Write({{"event", "assertion"}, {"message", msg ? msg : ""}});
    ExitProcess(91);
}

// Función separada sin objetos C++ locales: __try/__except no puede convivir
// con unwinding de objetos en la misma función (C2712).
inline bool SehCall(bool(__thiscall* entry)(CTaskInteriorSitAtDesk*, CPed*), CTaskInteriorSitAtDesk* task, CPed* ped, bool* ok, unsigned* code) {
    __try {
        *ok = true;
        return entry(task, ped);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        *ok = false;
        *code = GetExceptionCode();
        return false;
    }
}

inline unsigned rngValue{}, rngCalls{};
inline uint16 __cdecl FakeRandom() { ++rngCalls; SitAtDeskProbe::Record("RNG", float(rngValue), int(rngCalls)); return uint16(rngValue); }

// Sustituye CAnimManager::BlendAnimation(clump, groupId, animId, blendDelta)
// (0x4D4610) para no crear ni dejar asociaciones reales en el clump del jugador
// durante las 368 comparaciones; cada lado reutiliza su propia CAnimBlendAssociation
// sintética. AreAnimsLoaded y la consulta de asociación existente del final del
// bTaskFinished sí continúan siendo dependencias reales del motor.
inline CAnimBlendAssociation* __cdecl FakeBlend(RpClump*, AssocGroupId groupId, AnimationId animId, float blendDelta) {
    SitAtDeskProbe::Record("BlendAnimation", blendDelta, int(animId));
    auto* a = SitAtDeskProbe::nextAnim;
    if (a) {
        a->m_AnimId = animId;
        a->m_BlendDelta = blendDelta;
        a->m_Speed = 1.f;
    }
    return a;
}

template<class T> struct Storage {
    alignas(T) std::array<uint8, sizeof(T)> bytes{};
    T* get() { return reinterpret_cast<T*>(bytes.data()); }
};

struct Fixture {
    Storage<CPlayerPed>  ped;
    Storage<CMatrixLink> matrix;
    CAnimBlendAssociation anim{};
    InteriorInfo_t         interiorInfo{};
    CTaskInteriorSitAtDesk task{1200, &interiorInfo, false};

    bool officeLoaded = true;

    Fixture(CPlayerPed* source, int index) {
        std::memcpy(ped.get(), source, sizeof(CPlayerPed));
        std::memcpy(matrix.get(), source->m_matrix, sizeof(CMatrixLink));
        ped.get()->m_matrix = matrix.get();

        interiorInfo.Pos = CVector{10.f, -5.f, 3.f} * ((index / 2) % 2 ? 1.f : -1.f);
        interiorInfo.Dir = CVector{.7071f, .7071f, 0.f};

        constexpr std::array<AnimationId, 6> animCats{
            ANIM_ID_OFF_SIT_IN, ANIM_ID_OFF_SIT_2IDLE_180, ANIM_ID_OFF_SIT_IDLE_LOOP,
            ANIM_ID_OFF_SIT_BORED_LOOP, ANIM_ID_OFF_SIT_WATCH, ANIM_ID_IDLE
        };
        // Grupo único: m_Anim ya asociado (6 animId x 2 updatePedPos x 2
        // finished x 3 timerState = 72, x2 variantes de RNG = 144). Las ramas
        // m_bTaskAborting y `!m_Anim` quedan fuera: ambas empiezan llamando a
        // InteriorManager_c::AreAnimsLoaded(2), cuyo ASM original aborta con
        // acceso a memoria inválido al ejecutarse de forma aislada (ver LÍMITE
        // DOCUMENTADO arriba); se intentó primero excluir solo
        // m_bTaskAborting, pero `!m_Anim` tiene la MISMA llamada incondicional
        // al principio y se comprobó que también aborta.
        constexpr int group = 6 * 2 * 2 * 3; // 72
        constexpr int perRng = group;

        if (index >= 2 * perRng) { // Fase C
            SetupPhaseC(index - 2 * perRng, animCats);
            return;
        }

        const int rngVariant = (index / perRng) % 2;
        const int base = index % perRng;
        int r = base;
        const int  timerState = r % 3; r /= 3;
        const bool updatePos  = r % 2; r /= 2;
        const bool finished   = r % 2; r /= 2;
        const int  animCatIdx = r % 6;

        task.m_bTaskFinished = finished;
        task.m_bTaskAborting = false;
        task.m_bUpdatePedPos = updatePos;
        anim.m_AnimId        = animCats[animCatIdx];
        task.m_Anim          = &anim;
        if (timerState == 1) {
            task.m_TaskTimer.SetAsOutOfTime();
        } else if (timerState == 2) {
            task.m_AnimTimer.SetAsOutOfTime();
        }
        rngValue = rngVariant == 0 ? 0x1000u : 0x5000u;
    }

    // Grupo C1 (72): m_bTaskAborting=true con m_Anim asociado.
    //   6 animId x 2 updatePedPos x 3 timerState x 2 IsLoaded(int_office).
    // Grupo C2 (96): m_Anim nulo.
    //   6 m_PrevAnimId x 2 doInstantly x 2 m_bTaskAborting x 2 IsLoaded x 2 RNG.
    void SetupPhaseC(int j, const std::array<AnimationId, 6>& animCats) {
        task.m_bTaskFinished = false;
        if (j < 72) {
            const int timerState = j % 3;
            const bool updatePos = (j / 3) % 2;
            officeLoaded = (j / 6) % 2;
            const int animCatIdx = (j / 12) % 6;
            task.m_bTaskAborting = true;
            task.m_bUpdatePedPos = updatePos;
            anim.m_AnimId = animCats[animCatIdx];
            task.m_Anim = &anim;
            if (timerState == 1) task.m_TaskTimer.SetAsOutOfTime();
            else if (timerState == 2) task.m_AnimTimer.SetAsOutOfTime();
            rngValue = officeLoaded ? 0x1000u : 0x5000u;
            return;
        }
        const int k = j - 72;
        constexpr std::array<AnimationId, 6> prevCats{
            ANIM_ID_UNDEFINED, ANIM_ID_OFF_SIT_IN, ANIM_ID_OFF_SIT_CRASH,
            ANIM_ID_OFF_SIT_WATCH, ANIM_ID_OFF_SIT_2IDLE_180, ANIM_ID_IDLE
        };
        task.m_PrevAnimId    = prevCats[k % 6];
        task.m_bDoInstantly  = (k / 6) % 2;
        task.m_bTaskAborting = (k / 12) % 2;
        officeLoaded         = (k / 24) % 2;
        rngValue             = (k / 48) % 2 ? 0x5000u : 0x1000u;
        task.m_Anim          = nullptr;
    }

    json Snapshot(bool result) {
        const auto pos = ped.get()->GetPosition();
        return {
            {"result", result},
            {"duration", task.m_Duration},
            {"doInstantly", task.m_bDoInstantly},
            {"hasAnim", task.m_Anim != nullptr},
            {"animId", task.m_Anim ? int(task.m_Anim->m_AnimId) : -1},
            {"animSpeed", task.m_Anim ? task.m_Anim->m_Speed : 0.f},
            {"animBlendDelta", task.m_Anim ? task.m_Anim->m_BlendDelta : 0.f},
            {"prevAnimId", int(task.m_PrevAnimId)},
            {"updatePedPos", task.m_bUpdatePedPos},
            {"taskFinished", task.m_bTaskFinished},
            {"taskAborting", task.m_bTaskAborting},
            {"taskTimerStarted", task.m_TaskTimer.IsStarted()},
            {"taskTimerInterval", task.m_TaskTimer.m_nInterval},
            {"animTimerStarted", task.m_AnimTimer.IsStarted()},
            {"animTimerInterval", task.m_AnimTimer.m_nInterval},
            {"pedPos", {pos.x, pos.y, pos.z}},
            {"pedHeading", ped.get()->m_fCurrentRotation},
            {"pedAimRotation", ped.get()->m_fAimingRotation},
            {"pedAnimShift", {ped.get()->m_vecAnimMovingShiftLocal.x, ped.get()->m_vecAnimMovingShiftLocal.y}},
            {"events", SitAtDeskProbe::events}
        };
    }
};

inline bool Equal(const json& a, const json& b) {
    if (a.is_object()) {
        if (!b.is_object() || a.size() != b.size()) return false;
        for (auto it = a.begin(); it != a.end(); ++it) {
            if (!b.contains(it.key()) || !Equal(it.value(), b[it.key()])) return false;
        }
        return true;
    }
    if (a.is_array()) {
        if (!b.is_array() || a.size() != b.size()) return false;
        for (size_t i = 0; i < a.size(); ++i) if (!Equal(a[i], b[i])) return false;
        return true;
    }
    if (a.is_number_float() || b.is_number_float()) {
        if (!a.is_number() || !b.is_number()) return false;
        double x = a, y = b;
        return std::isfinite(x) && std::isfinite(y) && std::abs(x - y) <= .00001;
    }
    return a == b;
}

constexpr int kTotalPairs = 72 * 2 + 72 + 96; // 144 originales + 168 de la fase C

// CAnimManager::ms_aAnimBlocks (0xB5D4A0) es privado: se accede por dirección.
inline bool& OfficeLoadedFlag(int32 idx) { return reinterpret_cast<CAnimBlock*>(0xB5D4A0)[idx].IsLoaded; }

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{};
    GetModuleFileNameA(nullptr, exe, MAX_PATH);
    const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "sitatdesk-runtime.start")) { done = true; return; }
    if (!log.is_open()) {
        log.open(dir / "sitatdesk-runtime.jsonl");
        Write({{"event", "waiting_for_player"}});
    }
    auto* player = FindPlayerPed();
    if (!player || !player->m_matrix || CCutsceneMgr::IsRunning()) return;
    done = true;
    auto report = _CrtSetReportHook(Report);

    // NO llamar a CTaskInteriorSitAtDesk::InjectHooks() aquí: desde la
    // corrección de InjectHooksMain.cpp/TaskInteriorLieInBed.cpp (ver
    // README.md, "Hallazgo adicional"), el lambda `Interior()` de Tasks() SÍ
    // se invoca en el arranque normal del ASI y ya registra los hooks de las
    // 8 clases CTaskInterior*, incluida esta. Instalarlo también aquí produce
    // un registro duplicado del mismo nombre de hook y aborta el proceso con
    // la aserción `HookCategory.h:60 !FindItem(item->Name())` (causa
    // confirmada del primer cuadro de diálogo visto en el cuarto intento,
    // ver README.md). Esta sonda ahora solo BUSCA el hook ya instalado por el
    // camino normal, igual que CEntryExitTransition/LiveComparison.hpp.

    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if (c.Name() == "CTaskInteriorSitAtDesk") {
            for (auto& h : c.Items()) if (h->Name() == "ProcessPed") hook = h.get();
        }
    });
    ReversibleHooks::ReversibleHook::Base* animsHook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if (c.Name() == "InteriorManager_c") {
            for (auto& h : c.Items()) if (h->Name() == "AreAnimsLoaded") animsHook = h.get();
        }
    });
    const int32 officeIdx = CAnimManager::GetAnimationBlockIndex("int_office");
    if (!hook || !animsHook || officeIdx < 0) {
        Write({{"event", "error"}, {"reason", "missing hook"}});
        _CrtSetReportHook(report);
        return;
    }

    const auto livePed    = ReversingProbe::Bytes(player, sizeof(CPlayerPed));
    const auto liveMatrix = ReversingProbe::Bytes(player->m_matrix, sizeof(CMatrixLink));
    const bool enabled = hook->Hooked(), locked = hook->Locked();
    hook->LockState(false);
    const bool animsEnabled = animsHook->Hooked(), animsLocked = animsHook->Locked();
    animsHook->LockState(false);
    const bool officeBefore = OfficeLoadedFlag(officeIdx);
    Write({{"event", "player_ready"}, {"independent_inputs", true}});

    bool passed = true;
    int pairs = 0;
    {
        // Se parchea la propia función del DLL (no la dirección 0x4D4610 del
        // EXE): el candidato C++ llama a CAnimManager::BlendAnimation
        // directamente (llamada intra-DLL, nunca pasa por el trampolín del EXE),
        // mientras que el ASM original sí llama a 0x4D4610, que a su vez salta a
        // esta misma función del DLL porque su propio hook está activo. Parchear
        // aquí intercepta ambos caminos con un único punto.
        // ProcessPed original (0x677920) no llama a CGeneral::GetRandomNumber a
        // través de su propia dirección para decidir OneOffAnim/LoopAnim: en el
        // original esa llamada a GetRandomNumber está insertada en línea y
        // termina siendo una llamada directa al generador interno 0x821B1E
        // (el LCG `seed = seed*0x343FD + 0x269EC3; (seed>>16)&0x7FFF`, el mismo
        // que usa el resto del EXE), mientras que nuestra reconstrucción llama a
        // CGeneral::GetRandomNumber(), que delega en `rand()` de la CRT — dos
        // secuencias de aleatoriedad independientes incluso en juego real
        // (limitación ya documentada en General.cpp). Para que el banco compare
        // la MISMA decisión de rama en ambos lados hay que controlar las dos
        // direcciones, igual que hace CGeneral/LiveComparison.hpp.
        using BlendFn = CAnimBlendAssociation*(*)(RpClump*, AssocGroupId, AnimationId, float);
        ReversingProbe::EntryPatch rngPatch(std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber), FakeRandom);
        ReversingProbe::EntryPatch rawRngPatch(0x821B1E, FakeRandom);
        ReversingProbe::EntryPatch blendPatch(std::bit_cast<uintptr_t>(static_cast<BlendFn>(&CAnimManager::BlendAnimation)), FakeBlend);
        auto entry = reinterpret_cast<bool(__thiscall*)(CTaskInteriorSitAtDesk*, CPed*)>(0x677920);

        for (int i = 0; i < kTotalPairs && passed; ++i) {
            Fixture a(player, i), b(player, i);

            SitAtDeskProbe::events = json::array();
            SitAtDeskProbe::nextAnim = &a.anim;
            SitAtDeskProbe::recording = true;
            rngCalls = 0;
            const auto start = SitAtDeskProbe::calls;
            hook->State(false);
            animsHook->State(false);
            OfficeLoadedFlag(officeIdx) = a.officeLoaded;
            bool okA = false; unsigned codeA = 0;
            const bool ar = SehCall(entry, &a.task, a.ped.get(), &okA, &codeA);
            OfficeLoadedFlag(officeIdx) = officeBefore;
            if (!okA) {
                Write({{"event", "error"}, {"reason", "seh original"}, {"pair", i}, {"code", codeA}});
                _CrtSetReportHook(report);
                return;
            }
            SitAtDeskProbe::recording = false;
            const auto middle = SitAtDeskProbe::calls;
            const auto originalRng = rngCalls;
            const auto original = a.Snapshot(ar);

            SitAtDeskProbe::events = json::array();
            SitAtDeskProbe::nextAnim = &b.anim;
            SitAtDeskProbe::recording = true;
            rngCalls = 0;
            hook->State(true);
            animsHook->State(true);
            OfficeLoadedFlag(officeIdx) = b.officeLoaded;
            bool okB = false; unsigned codeB = 0;
            const bool br = SehCall(entry, &b.task, b.ped.get(), &okB, &codeB);
            OfficeLoadedFlag(officeIdx) = officeBefore;
            if (!okB) {
                Write({{"event", "error"}, {"reason", "seh candidate"}, {"pair", i}, {"code", codeB}});
                _CrtSetReportHook(report);
                return;
            }
            SitAtDeskProbe::recording = false;
            const auto end = SitAtDeskProbe::calls;
            const auto candidateRng = rngCalls;
            const auto candidate = b.Snapshot(br);

            const bool equal = Equal(original, candidate);
            passed = equal && middle == start && end == middle + 1 && originalRng == candidateRng;
            Write({
                {"event", "comparison"}, {"pair", i}, {"office_loaded", a.officeLoaded},
                {"original", original}, {"candidate", candidate}, {"equal", equal},
                {"original_candidate_calls", middle - start}, {"candidate_calls", end - middle},
                {"original_rng_calls", originalRng}, {"candidate_rng_calls", candidateRng},
                {"passed", passed}
            });
            ++pairs;
        }
    }

    hook->State(enabled);
    hook->LockState(locked);
    animsHook->State(animsEnabled);
    animsHook->LockState(animsLocked);
    OfficeLoadedFlag(officeIdx) = officeBefore;
    const bool restored = hook->Hooked() == enabled && hook->Locked() == locked
        && animsHook->Hooked() == animsEnabled && animsHook->Locked() == animsLocked
        && OfficeLoadedFlag(officeIdx) == officeBefore
        && ReversingProbe::Bytes(player, sizeof(CPlayerPed)) == livePed
        && ReversingProbe::Bytes(player->m_matrix, sizeof(CMatrixLink)) == liveMatrix;
    Write({{"event", passed && restored ? "complete" : "error"}, {"pairs", pairs}, {"state_restored", restored}});
    _CrtSetReportHook(report);
}
}
