#include "StdInc.h"

#include <cmath>
#include <cstring>
#include <cstdarg>
#include <string>
#include <format>

#include "SkinVigilancia.h"
#if __has_include("SkinVigilanciaActivaPorDefecto.h")
#include "SkinVigilanciaActivaPorDefecto.h" // solo existe al compilar el exe de vigilancia (no se versiona): define GTASA_SKIN_VIGILANCIA_POR_DEFECTO
#endif
#include "Timer.h"
#include "AccionesGrabador.h"
#include "Pools/Pools.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"

SkinVigilanciaEstado g_SkinVigilanciaEstado = {};

namespace {
constexpr float kLimiteAbs      = 2000.0f; // las matrices de hueso (skinToBone * pose * inversa del mundo) llevan traslaciones de metros; mas de 2000 es una anomalia dura
constexpr float kEscalaMax      = 8.0f;    // columnas 3x3 con longitud mayor son sospechosas (aviso suave)
constexpr int   kPeriodoResumen = 2500;
constexpr size_t kTopeLog       = 6u * 1024u * 1024u;  // tope de skin.log (6 MB); al llegar se deja de escribir (una linea final)
constexpr int   kTopeCapturas   = 30;                  // eventos con PNG como maximo
constexpr uint32 kAnchoMax      = 480;                 // las capturas se reducen a <= 480 px de ancho

int Activa() {
    static int cache = -1;
    if (cache < 0) {
        char buf[16] = {};
        const DWORD n = GetEnvironmentVariableA("GTASA_SKIN_VIGILANCIA", buf, sizeof(buf));
#ifdef GTASA_SKIN_VIGILANCIA_POR_DEFECTO
        cache = (n > 0 && n < sizeof(buf) && buf[0] == '0') ? 0 : 1;
#else
        cache = (n > 0 && n < sizeof(buf) && buf[0] != '0') ? 1 : 0;
#endif
        if (cache) {
            g_SkinVigilanciaEstado.magic = 0x47564B53u;
        }
    }
    return cache;
}

// ---------------------------------------------------------------- log propio (WinAPI; logs\skin.log; tope de tamano)
HANDLE g_log = INVALID_HANDLE_VALUE;
size_t g_logBytes = 0;
bool   g_logLleno = false;
bool   g_logIntentado = false;

void Escribir(const std::string& linea) {
    if (g_logLleno) { return; }
    if (!g_logIntentado) {
        g_logIntentado = true;
        CreateDirectoryA("logs", nullptr);
        // si el log anterior ya paso del tope se aparta (un solo respaldo) para no llenar el disco
        WIN32_FILE_ATTRIBUTE_DATA fad;
        if (GetFileAttributesExA("logs\\skin.log", GetFileExInfoStandard, &fad) && fad.nFileSizeLow > kTopeLog / 2) {
            MoveFileExA("logs\\skin.log", "logs\\skin.old.log", MOVEFILE_REPLACE_EXISTING);
        }
        g_log = CreateFileA("logs\\skin.log", FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (g_log != INVALID_HANDLE_VALUE) {
            LARGE_INTEGER sz{};
            GetFileSizeEx(g_log, &sz);
            g_logBytes = static_cast<size_t>(sz.QuadPart);
        }
    }
    if (g_log == INVALID_HANDLE_VALUE) { return; }
    std::string s = linea;
    s += "\r\n";
    if (g_logBytes + s.size() > kTopeLog) {
        static const char fin[] = "[SKINVIG] LOG LLENO (tope 6 MB): no se escriben mas lineas en esta sesion\r\n";
        DWORD w; WriteFile(g_log, fin, sizeof(fin) - 1, &w, nullptr);
        g_logLleno = true;
        return;
    }
    DWORD w = 0;
    WriteFile(g_log, s.data(), static_cast<DWORD>(s.size()), &w, nullptr);
    g_logBytes += w;
}

std::string Reloj() {
    SYSTEMTIME t; GetLocalTime(&t);
    return std::format("{:04}-{:02}-{:02} {:02}:{:02}:{:02}.{:03} juego_ms={} fotograma={}", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds, CTimer::GetTimeInMS(), CTimer::GetFrameCounter());
}
void Log(const std::string& texto) { Escribir("[" + Reloj() + "] " + texto); }

bool g_anomaliaEsteFotograma = false;

bool Finito(float v) { return std::isfinite(v); }

std::string Mat(const RwMatrix* m) {
    if (!m) { return "(null)"; }
    const float* f = reinterpret_cast<const float*>(m);
    return std::format("right({:.4g},{:.4g},{:.4g}) up({:.4g},{:.4g},{:.4g}) at({:.4g},{:.4g},{:.4g}) pos({:.4g},{:.4g},{:.4g})", f[0], f[1], f[2], f[4], f[5], f[6], f[8], f[9], f[10], f[12], f[13], f[14]);
}

// ---------------------------------------------------------------- contexto: dueno del clump, modo del jugador, animaciones
std::string Animaciones(RpClump* clump) {
    std::string r;
    if (!clump) { return r; }
    int n = 0;
    for (auto* a = RpAnimBlendClumpGetFirstAssociation(clump); a && n < 14; a = RpAnimBlendGetNextAssociation(a), ++n) {
        r += std::format("[id{} g{} mezcla={:.3g} t={:.3g} vel={:.3g} flags={:#x}] ", static_cast<int>(a->GetAnimId()), static_cast<int>(a->m_AnimGroupId), a->m_BlendAmount, a->m_CurrentTime, a->m_Speed, a->m_Flags);
    }
    return r;
}

std::string DescPed(CPed* ped) {
    std::string r = std::format("ped={} modelo={} {} estado={} tipo={}", static_cast<const void*>(ped), ped->m_nModelIndex, ped->IsPlayer() ? "ES_EL_JUGADOR" : "peaton", static_cast<int>(ped->m_nPedState), static_cast<int>(ped->m_nPedType));
    if (ped->bInVehicle && ped->m_pVehicle) {
        auto* v = ped->m_pVehicle;
        r += std::format(" EN_VEHICULO(modelo={} tipo={} vel={:.3f} giro={:.3f} volante={:.2f})", v->m_nModelIndex, static_cast<int>(v->m_nVehicleType), v->m_vecMoveSpeed.Magnitude(), v->m_vecTurnSpeed.Magnitude(), v->m_fSteerAngle);
    } else {
        r += std::format(" A_PIE(vel={:.3f})", ped->m_vecMoveSpeed.Magnitude());
    }
    return r;
}

std::string Contexto(const void* clump) {
    std::string r;
    CPed* dueno = nullptr;
    CVehicle* duenoV = nullptr;
    if (clump) {
        auto* pp = GetPedPool();
        for (size_t i = 0; pp && i < static_cast<size_t>(pp->GetSize()); ++i) {
            if (auto* p = pp->GetAt(i); p && static_cast<const void*>(p->GetRwObject()) == clump) { dueno = p; break; }
        }
        if (!dueno) {
            auto* vp = GetVehiclePool();
            for (size_t i = 0; vp && i < static_cast<size_t>(vp->GetSize()); ++i) {
                if (auto* v = vp->GetAt(i); v && static_cast<const void*>(v->GetRwObject()) == clump) { duenoV = v; break; }
            }
        }
    }
    if (dueno) { r += "DUENO: " + DescPed(dueno) + " anims: " + Animaciones(reinterpret_cast<RpClump*>(dueno->GetRwObject())); }
    else if (duenoV) { r += std::format("DUENO: vehiculo={} modelo={} tipo={} vel={:.3f}", static_cast<const void*>(duenoV), duenoV->m_nModelIndex, static_cast<int>(duenoV->m_nVehicleType), duenoV->m_vecMoveSpeed.Magnitude()); }
    else { r += "DUENO: desconocido"; }
    if (auto* jugador = FindPlayerPed(); jugador && jugador != dueno) {
        r += " | JUGADOR: " + DescPed(jugador) + " anims: " + Animaciones(reinterpret_cast<RpClump*>(jugador->GetRwObject()));
    }
    return r;
}

// ---------------------------------------------------------------- capturas (anillo de 3 fotogramas reducidos del backbuffer)
struct Captura {
    uint32 w = 0, h = 0;
    std::string bgr; // 3 bytes por pixel, filas de arriba a abajo
    uint32 frame = 0;
    bool valida = false;
};
Captura g_anillo[3];
int g_anilloIdx = -1;
IDirect3DSurface9* g_sys = nullptr;
IDirect3DSurface9* g_resuelto = nullptr; // render target sin MSAA (pila por defecto): se libera antes de cada Reset con SkinVigilanciaLiberarRecursos
UINT g_resW = 0, g_resH = 0; D3DFORMAT g_resFmt = D3DFMT_UNKNOWN;
UINT g_sysW = 0, g_sysH = 0;
D3DFORMAT g_sysFmt = D3DFMT_UNKNOWN;
int g_fallosCaptura = 0;
int g_capturasEscritas = 0;

uint32 Crc32(const uint8* d, size_t n, uint32 crc = 0xFFFFFFFFu) {
    static uint32 tabla[256]; static bool lista = false;
    if (!lista) {
        for (uint32 i = 0; i < 256; ++i) { uint32 c = i; for (int k = 0; k < 8; ++k) { c = (c & 1) ? 0xEDB88320u ^ (c >> 1) : c >> 1; } tabla[i] = c; }
        lista = true;
    }
    for (size_t i = 0; i < n; ++i) { crc = tabla[(crc ^ d[i]) & 0xFF] ^ (crc >> 8); }
    return crc;
}
void Be32(std::string& s, uint32 v) { s.push_back(char(v >> 24)); s.push_back(char(v >> 16)); s.push_back(char(v >> 8)); s.push_back(char(v)); }
void Chunk(std::string& out, const char* tipo, const std::string& datos) {
    Be32(out, static_cast<uint32>(datos.size()));
    std::string td(tipo, 4); td += datos;
    out += td;
    Be32(out, ~Crc32(reinterpret_cast<const uint8*>(td.data()), td.size()));
}
// PNG RGB 8 bits sin dependencias: deflate con bloques "almacenados" (sin comprimir); ~390 KB por captura de 480x270
bool EscribirPng(const char* ruta, const Captura& c) {
    std::string raw;
    raw.reserve((c.w * 3 + 1) * c.h);
    for (uint32 y = 0; y < c.h; ++y) {
        raw.push_back(0);
        for (uint32 x = 0; x < c.w; ++x) {
            const char* p = &c.bgr[(y * c.w + x) * 3];
            raw.push_back(p[2]); raw.push_back(p[1]); raw.push_back(p[0]);
        }
    }
    std::string z; z.push_back(char(0x78)); z.push_back(char(0x01));
    for (size_t pos = 0; pos < raw.size();) {
        const size_t n = std::min<size_t>(65535, raw.size() - pos);
        z.push_back(pos + n == raw.size() ? 1 : 0);
        z.push_back(char(n & 0xFF)); z.push_back(char(n >> 8)); z.push_back(char(~n & 0xFF)); z.push_back(char((~n >> 8) & 0xFF));
        z.append(raw, pos, n); pos += n;
    }
    uint32 a = 1, b = 0;
    for (unsigned char ch : raw) { a = (a + ch) % 65521; b = (b + a) % 65521; }
    Be32(z, (b << 16) | a);
    std::string png("\x89PNG\r\n\x1a\n", 8), ihdr;
    Be32(ihdr, c.w); Be32(ihdr, c.h); ihdr.push_back(8); ihdr.push_back(2); ihdr.push_back(0); ihdr.push_back(0); ihdr.push_back(0);
    Chunk(png, "IHDR", ihdr); Chunk(png, "IDAT", z); Chunk(png, "IEND", "");
    HANDLE f = CreateFileA(ruta, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (f == INVALID_HANDLE_VALUE) { return false; }
    DWORD w; WriteFile(f, png.data(), static_cast<DWORD>(png.size()), &w, nullptr); CloseHandle(f);
    return true;
}

void VolcarCapturas() {
    if (g_capturasEscritas >= kTopeCapturas) { return; }
    ++g_capturasEscritas;
    CreateDirectoryA("logs", nullptr);
    CreateDirectoryA("logs\\skin_cap", nullptr);
    SYSTEMTIME t; GetLocalTime(&t);
    for (int k = 0; k < 3; ++k) {  // k=0 el fotograma anomalo, 1 y 2 los anteriores
        const Captura& c = g_anillo[(g_anilloIdx - k + 6) % 3];
        if (!c.valida) { continue; }
        char ruta[200];
        std::snprintf(ruta, sizeof(ruta), "logs\\skin_cap\\evento_%02d_%02d%02d%02d_f%u_%s.png", g_capturasEscritas, t.wHour, t.wMinute, t.wSecond, c.frame, k == 0 ? "anomalo" : (k == 1 ? "previo1" : "previo2"));
        EscribirPng(ruta, c);
    }
    Log(std::format("CAPTURA guardada en logs\\skin_cap (evento {} de {}): fotograma anomalo y 2 previos", g_capturasEscritas, kTopeCapturas));
}

bool g_eventoLimitadoPorFotograma(uint32 frame) {
    static uint32 ultimo = 0xFFFFFFFFu; static int n = 0;
    if (frame != ultimo) { ultimo = frame; n = 0; }
    return ++n > 6; // como mucho 6 registros por fotograma
}
} // namespace

bool SkinVigilanciaActiva() { return Activa() != 0; }

void SkinVigilanciaComprobar(const char* origen, float* out, int numNodes, const RwMatrix* skinToBone, const RwMatrix* hierarchyMatrices, const RwMatrix* inverseWorld, const void* atomic, const void* hierarchy, const void* clump) {
    if (!Activa() || out == nullptr || numNodes <= 0) {
        return;
    }
    auto& e = g_SkinVigilanciaEstado;
    static bool inicio = false;
    if (!inicio) {
        inicio = true;
        Log("[SKINVIG] ===== vigilancia de skinning ACTIVA (inicio de sesion) =====");
    }
    const uint32 frame = CTimer::GetFrameCounter();
    ++e.comprobaciones;
    // prueba de la propia vigilancia: NaN a proposito en el hueso 3 de CJ (cada ~600 fotogramas, una vez por fotograma)
    static int prueba = -1;
    if (prueba < 0) { char b[8] = {}; prueba = (GetEnvironmentVariableA("GTASA_SKIN_VIGILANCIA_PRUEBA", b, sizeof(b)) > 0 && b[0] != '0') ? 1 : 0; }
    if (prueba && numNodes > 3 && clump != nullptr) {
        static uint32 ultimoFrame = 0;
        auto* j = FindPlayerPed();
        if (j && static_cast<const void*>(j->GetRwObject()) == clump && frame % 600 == 0 && frame != ultimoFrame) {
            ultimoFrame = frame;
            out[3 * 12 + 0] = std::numeric_limits<float>::quiet_NaN();
            Log("[SKINVIG] PRUEBA: inyectado NaN en out[hueso 3][0] del jugador (esto es voluntario)");
        }
    }
    bool anomalo = false;
    float maxAbs = 0.0f, maxEsc = 0.0f;
    int primerHueso = -1;
    for (int b = 0; b < numNodes; ++b) {
        const float* m = out + b * 12;
        bool malo = false;
        for (int k = 0; k < 12; ++k) {
            if (!Finito(m[k]) || std::fabs(m[k]) > kLimiteAbs) {
                malo = true;
            } else {
                maxAbs = std::max(maxAbs, std::fabs(m[k]));
            }
        }
        if (!malo) {
            for (int c = 0; c < 3; ++c) {
                const float l = std::sqrt(m[c] * m[c] + m[4 + c] * m[4 + c] + m[8 + c] * m[8 + c]);
                maxEsc = std::max(maxEsc, l);
            }
        }
        if (malo && primerHueso < 0) {
            primerHueso = b;
        }
        anomalo |= malo;
    }
    if (!anomalo) {
        e.maxAbs = std::max(e.maxAbs, maxAbs);
        e.maxEscala = std::max(e.maxEscala, maxEsc);
    }
    if (anomalo) {
        ++e.anomalias;
        e.ultimoFotograma = frame;
        g_anomaliaEsteFotograma = true;
        if (!g_eventoLimitadoPorFotograma(frame)) {
            Log(std::format("[SKINVIG] ANOMALIA en matrices de hueso (subida al shader) origen={} atomo={} clump={} jerarquia={} huesos={} primer hueso malo={}", origen, atomic, clump, hierarchy, numNodes, primerHueso));
            Log("[SKINVIG]   " + Contexto(clump));
            int mostrados = 0;
            for (int b = 0; b < numNodes && mostrados < 4; ++b) {
                const float* m = out + b * 12;
                bool malo = false;
                for (int k = 0; k < 12; ++k) { malo |= (!Finito(m[k]) || std::fabs(m[k]) > kLimiteAbs); }
                if (!malo) { continue; }
                ++mostrados;
                Log(std::format("[SKINVIG]   hueso {}: {:.6g} {:.6g} {:.6g} {:.6g} | {:.6g} {:.6g} {:.6g} {:.6g} | {:.6g} {:.6g} {:.6g} {:.6g}", b, m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11]));
                if (skinToBone) { Log("[SKINVIG]     skinToBone: " + Mat(&skinToBone[b])); }
                if (hierarchyMatrices) { Log("[SKINVIG]     jerarquia : " + Mat(&hierarchyMatrices[b])); }
            }
            Log("[SKINVIG]     inversaMundo: " + Mat(inverseWorld));
        }
    } else if (maxEsc > kEscalaMax) {
        static int limite = 0;
        if (limite++ < 100) {
            Log(std::format("[SKINVIG] escala alta origen={} atomo={} huesos={} maxEscala={:.4g} maxAbs={:.4g}", origen, atomic, numNodes, maxEsc, maxAbs));
            Log("[SKINVIG]   " + Contexto(clump));
        }
    }
    if ((e.comprobaciones % kPeriodoResumen) == 0) {
        Log(std::format("[SKINVIG] resumen: comprobaciones={} anomalias={} maxAbs={:.4g} maxEscala={:.4g}", e.comprobaciones, e.anomalias, e.maxAbs, e.maxEscala));
    }
}

void SkinVigilanciaNodo(const char* etapa, int nodo, int numNodes, const RwMatrix& local, const RwMatrix* producto, const float* keyFrame, const void* hierarchy) {
    if (!Activa()) {
        return;
    }
    const auto malo = [](const float* f) {
        static constexpr int rot[9] = { 0, 1, 2, 4, 5, 6, 8, 9, 10 };
        for (int k : rot) {
            if (!std::isfinite(f[k]) || std::fabs(f[k]) > 16.0f) { return true; }
        }
        return false;
    };
    const float* l = reinterpret_cast<const float*>(&local);
    const float* p = producto ? reinterpret_cast<const float*>(producto) : nullptr;
    const bool mLocal = malo(l);
    const bool mProd = p && malo(p);
    if (!mLocal && !mProd) {
        return;
    }
    g_anomaliaEsteFotograma = true;
    static int limite = 0;
    if (limite++ >= 200 || g_eventoLimitadoPorFotograma(CTimer::GetFrameCounter())) {
        return;
    }
    Log(std::format("[SKINVIG] NODO {} nodo={}/{} jerarquia={} keyframe_ptr={} local_malo={} producto_malo={} keyframe(q xyzw | t xyz | tiempo ..) = {:.6g} {:.6g} {:.6g} {:.6g} | {:.6g} {:.6g} {:.6g} | {:.6g} {:.6g}",
        etapa, nodo, numNodes, hierarchy, static_cast<const void*>(keyFrame), mLocal, mProd, keyFrame[0], keyFrame[1], keyFrame[2], keyFrame[3], keyFrame[4], keyFrame[5], keyFrame[6], keyFrame[7], keyFrame[8]));
    Log("[SKINVIG]     local: " + Mat(&local) + "  producto: " + Mat(producto));
}

namespace {
struct VehData { int n; bool malo; std::string desc; };
RpAtomic* VehAtomo(RpAtomic* atomic, void* data) {
    auto* d = static_cast<VehData*>(data);
    ++d->n;
    RwFrame* fr = RpAtomicGetFrame(atomic);
    if (!fr) { return atomic; }
    const float* f = reinterpret_cast<const float*>(&fr->modelling);
    bool malo = false;
    for (int k = 0; k < 16; ++k) { if (k % 4 == 3) { continue; } if (!std::isfinite(f[k])) { malo = true; } }
    for (int c = 0; c < 3; ++c) {
        const float l = std::sqrt(f[c] * f[c] + f[4 + c] * f[4 + c] + f[8 + c] * f[8 + c]);
        if (l > 4.0f || (l < 0.05f && l == l)) { malo = true; }
    }
    for (int k : { 12, 13, 14 }) { if (std::fabs(f[k]) > 100000.0f) { malo = true; } }
    if (malo && !d->malo) { d->malo = true; d->desc = std::format("atomo {} frame {} modelling: {}", static_cast<const void*>(atomic), static_cast<const void*>(fr), Mat(&fr->modelling)); }
    return atomic;
}
} // namespace

void SkinVigilanciaVehiculo(const CVehicle* vehicle) {
    if (!Activa() || !vehicle || !vehicle->GetRwObject()) {
        return;
    }
    VehData d{ 0, false, {} };
    RpClumpForAllAtomics(reinterpret_cast<RpClump*>(vehicle->GetRwObject()), VehAtomo, &d);
    bool malaEntidad = false;
    if (const auto* mt = vehicle->m_matrix) {
        const float* f = reinterpret_cast<const float*>(mt);
        for (int k = 0; k < 16; ++k) { if (k % 4 == 3) { continue; } if (!std::isfinite(f[k])) { malaEntidad = true; } }
        for (int c = 0; c < 3; ++c) {
            const float l = std::sqrt(f[c] * f[c] + f[4 + c] * f[4 + c] + f[8 + c] * f[8 + c]);
            if (l > 2.0f || l < 0.5f || !std::isfinite(l)) { malaEntidad = true; }
        }
    }
    if (!d.malo && !malaEntidad) {
        return;
    }
    g_anomaliaEsteFotograma = true;
    static int limite = 0;
    if (limite++ >= 100 || g_eventoLimitadoPorFotograma(CTimer::GetFrameCounter())) {
        return;
    }
    Log(std::format("[SKINVIG] ANOMALIA en vehiculo={} modelo={} tipo={} vel={:.3f} matriz_entidad_mala={} frames_malos={} {}", static_cast<const void*>(vehicle), vehicle->m_nModelIndex, static_cast<int>(vehicle->m_nVehicleType), vehicle->m_vecMoveSpeed.Magnitude(), malaEntidad, d.malo, d.desc));
    Log("[SKINVIG]   " + Contexto(vehicle->GetRwObject()));
}

void SkinVigilanciaLiberarRecursos() {
    if (g_resuelto) { g_resuelto->Release(); g_resuelto = nullptr; }
}

void SkinVigilanciaFinFotograma(void* deviceRaw) {
    if (!Activa()) {
        return;
    }
    {   // latido corto de 1 s (reloj de pared): posicion, vehiculo, velocidad, inclinacion y animaciones de CJ, para cruzar con la hora de un fallo aunque no dispare la deteccion
        static DWORD ultimoLatido = 0;
        const DWORD ahora = GetTickCount();
        if (ahora - ultimoLatido >= 1000) {
            ultimoLatido = ahora;
            if (auto* j = FindPlayerPed()) {
                const CVector p = j->GetPosition();
                std::string r = std::format("LATIDO pos=({:.1f},{:.1f},{:.1f}) estado={} ", p.x, p.y, p.z, static_cast<int>(j->m_nPedState));
                if (j->bInVehicle && j->m_pVehicle) {
                    auto* v = j->m_pVehicle;
                    const auto& m = v->GetMatrix();
                    r += std::format("VEHICULO modelo={} tipo={} vel={:.3f} giro={:.3f} volante={:.2f} inclinacion(right.z)={:.3f} up.z={:.3f} ", v->m_nModelIndex, static_cast<int>(v->m_nVehicleType), v->m_vecMoveSpeed.Magnitude(), v->m_vecTurnSpeed.Magnitude(), v->m_fSteerAngle, m.GetRight().z, m.GetUp().z);
                } else {
                    r += std::format("A_PIE vel={:.3f} ", j->m_vecMoveSpeed.Magnitude());
                }
                r += "anims: " + Animaciones(reinterpret_cast<RpClump*>(j->GetRwObject()));
                Log("[SKINVIG] " + r);
            }
        }
    }
    // Las capturas del backbuffer estan APAGADAS por defecto: con MSAA (el juego usa 4x) la resolucion StretchRect+GetRenderTargetData dejo el juego colgado en las pruebas (ver informe);
    // solo se activan con GTASA_SKIN_CAPTURAS=1 (experimental). El vigilante externo (reversing/FnSkin/vigilante_skin.py) captura la ventana sin tocar el juego.
    static int capturas = -1;
    if (capturas < 0) { char b[8] = {}; capturas = (GetEnvironmentVariableA("GTASA_SKIN_CAPTURAS", b, sizeof(b)) > 0 && b[0] != '0') ? 1 : 0; }
    if (!capturas) { g_anomaliaEsteFotograma = false; return; }
    IDirect3DDevice9* const dev = static_cast<IDirect3DDevice9*>(deviceRaw);
    const uint32 frame = CTimer::GetFrameCounter();
    static bool primera = true;
    if (primera) { primera = false; Log("[SKINVIG] FinFotograma: primera llamada (captura de backbuffer activa)"); }
    const bool volcar = g_anomaliaEsteFotograma;
    g_anomaliaEsteFotograma = false;
    if (dev == nullptr || g_fallosCaptura >= 5) {
        return;
    }
    // copia del backbuffer a memoria de sistema (sin recursos en la pila por defecto: no estorba a los Reset del dispositivo) y reduccion por salto de pixeles
    IDirect3DSurface9* bb = nullptr;
    if (dev->GetRenderTarget(0, &bb) != D3D_OK || bb == nullptr) { ++g_fallosCaptura; Log("[SKINVIG] captura: GetRenderTarget fallo"); return; }
    D3DSURFACE_DESC d{};
    bb->GetDesc(&d);
    if (d.Format != D3DFMT_X8R8G8B8 && d.Format != D3DFMT_A8R8G8B8) { Log(std::format("[SKINVIG] captura: formato no soportado fmt={} msaa={} {}x{}", static_cast<int>(d.Format), static_cast<int>(d.MultiSampleType), d.Width, d.Height)); bb->Release(); ++g_fallosCaptura; return; }
    IDirect3DSurface9* origen = bb;
    if (d.MultiSampleType != D3DMULTISAMPLE_NONE) { // backbuffer con MSAA: GetRenderTargetData no lo admite; se resuelve antes con StretchRect a un RT sin MSAA
        if (g_resuelto == nullptr || g_resW != d.Width || g_resH != d.Height || g_resFmt != d.Format) {
            SkinVigilanciaLiberarRecursos();
            if (const HRESULT h3 = dev->CreateRenderTarget(d.Width, d.Height, d.Format, D3DMULTISAMPLE_NONE, 0, FALSE, &g_resuelto, nullptr); h3 != D3D_OK) { Log(std::format("[SKINVIG] captura: CreateRenderTarget hr={:#x}", static_cast<uint32>(h3))); bb->Release(); ++g_fallosCaptura; return; }
            g_resW = d.Width; g_resH = d.Height; g_resFmt = d.Format;
        }
        if (const HRESULT h4 = dev->StretchRect(bb, nullptr, g_resuelto, nullptr, D3DTEXF_NONE); h4 != D3D_OK) { Log(std::format("[SKINVIG] captura: StretchRect hr={:#x}", static_cast<uint32>(h4))); bb->Release(); ++g_fallosCaptura; return; }
        origen = g_resuelto;
    }
    if (g_sys == nullptr || g_sysW != d.Width || g_sysH != d.Height || g_sysFmt != d.Format) {
        if (g_sys) { g_sys->Release(); g_sys = nullptr; }
        if (const HRESULT h2 = dev->CreateOffscreenPlainSurface(d.Width, d.Height, d.Format, D3DPOOL_SYSTEMMEM, &g_sys, nullptr); h2 != D3D_OK) { Log(std::format("[SKINVIG] captura: CreateOffscreenPlainSurface hr={:#x}", static_cast<uint32>(h2))); bb->Release(); ++g_fallosCaptura; return; }
        g_sysW = d.Width; g_sysH = d.Height; g_sysFmt = d.Format;
    }
    const HRESULT hr = dev->GetRenderTargetData(origen, g_sys);
    bb->Release();
    if (hr != D3D_OK) { ++g_fallosCaptura; Log(std::format("[SKINVIG] captura: GetRenderTargetData hr={:#x}", static_cast<uint32>(hr))); return; }
    D3DLOCKED_RECT lr;
    if (g_sys->LockRect(&lr, nullptr, D3DLOCK_READONLY) != D3D_OK) { ++g_fallosCaptura; Log("[SKINVIG] captura: LockRect fallo"); return; }
    const uint32 paso = std::max<uint32>(1, (d.Width + kAnchoMax - 1) / kAnchoMax);
    g_anilloIdx = (g_anilloIdx + 1) % 3;
    Captura& c = g_anillo[g_anilloIdx];
    c.w = d.Width / paso; c.h = d.Height / paso; c.frame = frame; c.valida = true;
    c.bgr.resize(static_cast<size_t>(c.w) * c.h * 3);
    for (uint32 y = 0; y < c.h; ++y) {
        const uint8* fila = static_cast<const uint8*>(lr.pBits) + static_cast<size_t>(y * paso) * lr.Pitch;
        char* dst = &c.bgr[static_cast<size_t>(y) * c.w * 3];
        for (uint32 x = 0; x < c.w; ++x) {
            const uint8* px = fila + static_cast<size_t>(x * paso) * 4;
            dst[x * 3 + 0] = static_cast<char>(px[0]); dst[x * 3 + 1] = static_cast<char>(px[1]); dst[x * 3 + 2] = static_cast<char>(px[2]);
        }
    }
    g_sys->UnlockRect();
    if (volcar) {
        AccionesGrabadorVolcarEvento();
        VolcarCapturas();
    }
}
