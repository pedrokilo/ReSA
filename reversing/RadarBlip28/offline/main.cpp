// Ensayo SIN juego del banco RadarBlip28 (CRadar::DrawEntityBlip, 0x587000). No sustituye a la corrida dentro del juego, que NO se ha ejecutado.
// Mapea los bytes REALES del EXE (secciones del fichero) en 0x400000 dentro de un proceso hijo suspendido de este mismo ejecutable (32 bits, base 0x10000000) y ejecuta el Run() REAL de ../Core.hpp:
// misma fixture, mismos falsos con traza, mismas compuertas y el mismo registro .jsonl que el banco del juego. El lado "original" es la entrada 0x587000 de esa copia del EXE; el lado "candidato"
// es CRadar::DrawEntityBlip de Radar.cpp compilada con las opciones del proyecto (prepare.py extrae su texto byte a byte), con RadarAirstripRotateCpp de X87Math.cpp (/arch:IA32).
// Los colaboradores se sustituyen por los mismos falsos que el banco: en la copia del EXE con un `jmp` en la entrada de cada funcion (EntryPatch de Core.hpp); en la DLL porque los simbolos del
// proyecto que referencia la funcion estan definidos aqui como envoltorios de los falsos. _ftol2 (0x821B40) y el constructor de CRGBA (0x7170C0) son los REALES del EXE.
// El HookRoute de este ensayo hace lo mismo que el real cuando no hay hook RH registrado: parchea la entrada 0x587000 hacia el cuerpo de la DLL solo durante el lado candidato.
#include <cstdio>
#include <fstream>
#include "Radar.h"
#include "World.h"
#include "Camera.h"
#include "Pickups.h"
#include "Pickup.h"
#include "Scripts/TheScripts.h"
#include "Sprite2d.h"
#include "EntryExit.h"
#include "Entity/Placeable.h"
#include "Pools/Pools.h"
#include "../../ProbeSupport.hpp"

namespace OriginalDependencies499 {
class HookRoute {
    uintptr_t m_entry, m_body;
public:
    struct RegisteredTarget {};
    HookRoute(uintptr_t entry, uintptr_t body) : m_entry(entry), m_body(body) {}
    uintptr_t Body() const { return m_body; }
    bool Registered() const { return true; }
    template<class F> auto Invoke(bool candidate, F call) {
        if (candidate) {
            ReversingProbe::EntryPatch patch(m_entry, reinterpret_cast<void*>(m_body));
            return call();
        }
        return call();
    }
    bool Restore() { return true; }
};
}
#include "../Core.hpp"

using namespace RadarBlip28Live;

// ---- simbolos del proyecto que referencia DrawEntityBlip = envoltorios de los falsos ----
auto& FrontEndMenuManager = StaticRef<CMenuManager>(0xBA6748);
auto& TheCamera = StaticRef<CCamera>(0xB6F028);
CPedPool* GetPedPool() { return StaticRef<CPedPool*>(0xB74490); }
CVehiclePool* GetVehiclePool() { return StaticRef<CVehiclePool*>(0xB74494); }
CObjectPool* GetObjectPool() { return StaticRef<CObjectPool*>(0xB7449C); }
CVehicle* FindPlayerVehicle(int32 playerId, bool includeRemote) { return static_cast<CVehicle*>(SVeh(playerId, includeRemote ? 1u : 0u)); }
CVector FindPlayerCentreOfWorldForMap(int32 playerId) { CVector out{}; SCentre(reinterpret_cast<u32*>(&out), playerId); return out; }
CVector2D CRadar::TransformRealWorldPointToRadarSpace(const CVector2D& in) { CVector2D out{}; SXform(reinterpret_cast<u32*>(&out), reinterpret_cast<const u32*>(&in)); return out; }
float CRadar::LimitRadarPoint(CVector2D& point) { return SLimit(reinterpret_cast<u32*>(&point)); }
CVector2D CRadar::TransformRadarPointToScreenSpace(const CVector2D& in) { CVector2D out{}; SScreen(reinterpret_cast<u32*>(&out), reinterpret_cast<const u32*>(&in)); return out; }
uint32 CRadar::GetRadarTraceColour(eBlipColour color, bool bright, bool friendly) { return SColour(uint32(color), bright ? 1u : 0u, friendly ? 1u : 0u); }
void CRadar::ShowRadarMarker(CVector posn, uint32 color, float radius) { SMarker(Bits(posn.x), Bits(posn.y), Bits(posn.z), color, Bits(radius)); }
void CRadar::ShowRadarTraceWithHeight(float x, float y, uint32 size, uint32 r, uint32 g, uint32 b, uint32 a, eRadarTraceHeight height) { STraceHeight(Bits(x), Bits(y), size, r, g, b, a, uint32(height)); }
void CRadar::DrawRotatingRadarSprite(CSprite2d& sprite, float x, float y, float angle, uint32 width, uint32 height, CRGBA color) { SRotating(&sprite, Bits(x), Bits(y), Bits(angle), width, height, std::bit_cast<uint32>(color)); }
void CRadar::DrawRadarSprite(eRadarSprite spriteId, float x, float y, uint8 alpha) { SSprite(int32(spriteId), Bits(x), Bits(y), alpha); }
void CRadar::AddBlipToLegendList(bool noSprite, int32 blipIndex) { SLegend(noSprite ? 1u : 0u, blipIndex); }
bool CRadar::HasThisBlipBeenRevealed(int32 blipIndex) { return SRevealed(blipIndex); }
void CSprite2d::DrawCircleAtNearClip(const CVector2D& posn, float size, const CRGBA& color, int32 angle) { SCircle(reinterpret_cast<const u32*>(&posn), Bits(size), reinterpret_cast<const u32*>(&color), angle); }
void CEntryExit::GetPositionRelativeToOutsideWorld(CVector& outPos) { SEnex(this, nullptr, &outPos.x); }
float CPlaceable::GetHeading() const { return SHeading(const_cast<CPlaceable*>(this)); }
int32 CPickups::GetActualPickupIndex(tPickupReference pickupRef) { return SPickupIndex(uint32(pickupRef.num)); }
int32 CTheScripts::GetActualScriptThingIndex(int32 ref, eScriptThingType type) { return SScriptIndex(ref, int32(type)); }

// La imagen del EXE se mapea en un proceso hijo SUSPENDIDO de este mismo ejecutable (antes de que el cargador reparta direcciones bajas): en el proceso normal 0x400000.. ya esta ocupado.
static bool MapExeInto(HANDLE proc, const char* path) {
    HANDLE f = CreateFileA(path, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
    if (f == INVALID_HANDLE_VALUE) { std::printf("no se pudo abrir %s\n", path); return false; }
    const DWORD size = GetFileSize(f, nullptr); std::vector<uint8> img(size); DWORD got{};
    ReadFile(f, img.data(), size, &got, nullptr); CloseHandle(f);
    auto* dos = reinterpret_cast<IMAGE_DOS_HEADER*>(img.data()); auto* nt = reinterpret_cast<IMAGE_NT_HEADERS32*>(img.data() + dos->e_lfanew);
    void* base = VirtualAllocEx(proc, reinterpret_cast<void*>(nt->OptionalHeader.ImageBase), nt->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (base != reinterpret_cast<void*>(nt->OptionalHeader.ImageBase)) { std::printf("no se pudo reservar la imagen en 0x%08X (tamano 0x%X)\n", nt->OptionalHeader.ImageBase, nt->OptionalHeader.SizeOfImage); return false; }
    SIZE_T written{};
    WriteProcessMemory(proc, base, img.data(), nt->OptionalHeader.SizeOfHeaders, &written);
    auto* sec = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i)
        if (!WriteProcessMemory(proc, reinterpret_cast<uint8*>(base) + sec[i].VirtualAddress, img.data() + sec[i].PointerToRawData, std::min<DWORD>(sec[i].SizeOfRawData, sec[i].Misc.VirtualSize), &written)) return false;
    return true;
}
static int Launcher(int argc, char** argv) {
    char self[MAX_PATH]; GetModuleFileNameA(nullptr, self, MAX_PATH);
    std::string cmd = std::string("\"") + self + "\" --child";
    for (int i = 1; i < argc; ++i) cmd += std::string(" \"") + argv[i] + "\"";
    STARTUPINFOA si{}; si.cb = sizeof(si); PROCESS_INFORMATION pi{};
    if (!CreateProcessA(nullptr, cmd.data(), nullptr, nullptr, TRUE, CREATE_SUSPENDED, nullptr, nullptr, &si, &pi)) { std::printf("CreateProcess fallo\n"); return 2; }
    if (!MapExeInto(pi.hProcess, argc > 1 ? argv[1] : "D:\\Grand Theft Auto San Andreas\\gta-sa.exe")) { TerminateProcess(pi.hProcess, 3); return 3; }
    ResumeThread(pi.hThread);
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD code{}; GetExitCodeProcess(pi.hProcess, &code);
    return int(code);
}
static LONG CALLBACK Veh(PEXCEPTION_POINTERS e) {
    if (e->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION) {
        std::printf("EXCEPCION acceso invalido: eip=%08X dir=%08X tipo=%u  eax=%08X ecx=%08X edx=%08X esp=%08X ebp=%08X\n", unsigned(e->ContextRecord->Eip), unsigned(e->ExceptionRecord->ExceptionInformation[1]),
                    unsigned(e->ExceptionRecord->ExceptionInformation[0]), unsigned(e->ContextRecord->Eax), unsigned(e->ContextRecord->Ecx), unsigned(e->ContextRecord->Edx), unsigned(e->ContextRecord->Esp), unsigned(e->ContextRecord->Ebp));
        ExitProcess(9);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

int main(int argc, char** argv) {
    // Uso: h.exe [ruta del gta-sa.exe] [casos (todos por defecto)] [registro .jsonl]   (el proceso hijo recibe --child delante)
    if (argc < 2 || std::strcmp(argv[1], "--child") != 0) return Launcher(argc, argv);
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    AddVectoredExceptionHandler(1, Veh);
    Env env;
    env.cases = (argc > 3 && std::atoi(argv[3]) > 0) ? unsigned(std::atoi(argv[3])) : kCases;
    env.asi = ModuleOf(uintptr_t(&ModuleMarker)); env.game = 0x400000;
    RadarBlip28Live::log.open(argc > 4 ? argv[4] : "radarblip28-runtime.jsonl");
    const uintptr_t body = std::bit_cast<uintptr_t>(&CRadar::DrawEntityBlip);
    env.route.reset(new HookRoute(0x587000, body));
    json registered = json::object(), bodies = json::object();
    registered["CRadar::DrawEntityBlip"] = true; bodies["CRadar::DrawEntityBlip"] = body;
    Run(env, true, registered, bodies);
    std::printf("fin: %u casos\n", env.cases);
    return 0;
}
