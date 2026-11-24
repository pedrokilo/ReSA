#include "StdInc.h"

#include "Verif.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <windows.h>

namespace DatosExe::Verif {
namespace {
struct Estado {
    std::vector<uint8_t> iniObj, iniMem; // valores al arrancar (por entrada, concatenados con desplazamiento)
    std::vector<unsigned> off;
    std::vector<uint16_t> logs;
    FILE* f = nullptr;
    DWORD t0 = 0;
};
Estado g;
bool gIniciado = false;

// copia protegida: la memoria del EXE/imagen puede no estar mapeada del todo
bool LeerSeguro(void* dst, const void* src, size_t n) {
    __try {
        memcpy(dst, src, n);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return false;
    }
}

void Hex(char* out, const uint8_t* b, unsigned n) {
    const unsigned m = n > 32 ? 32 : n;
    for (unsigned i = 0; i < m; i++) sprintf(out + i * 2, "%02X", b[i]);
    if (n > m) strcpy(out + m * 2, "..");
}

void Linea(const char* tag, unsigned i, const uint8_t* ob, const uint8_t* me) {
    const Entrada& e = kTabla[i];
    char a[100], b[100], c[100], d[100];
    Hex(a, ob, e.tam);
    Hex(b, me, e.tam);
    Hex(c, &g.iniObj[g.off[i]], e.tam);
    Hex(d, &g.iniMem[g.off[i]], e.tam);
    unsigned first = 0, nd = 0;
    for (unsigned k = 0; k < e.tam; k++)
        if (ob[k] != me[k]) { if (!nd) first = k; nd++; }
    fprintf(g.f, "%s t=%lu 0x%08X %s tam=%u dif=%u@%u obj=%s mem=%s iniObj=%s iniMem=%s\n", tag, (unsigned long)(GetTickCount() - g.t0), e.addr, e.nombre, e.tam, nd, first, a, b, c, d);
    fflush(g.f);
}

DWORD WINAPI Hilo(LPVOID) {
    const char* r = getenv("DATOSEXE_VERIF_RETARDO");
    const char* p = getenv("DATOSEXE_VERIF_MS");
    Sleep(r ? (DWORD)atoi(r) : 15000);
    for (unsigned i = 0; i < kTablaN; i++) { // valores de partida: tras los inicializadores estaticos
        memcpy(&g.iniObj[g.off[i]], kTabla[i].objeto, kTabla[i].tam);
        LeerSeguro(&g.iniMem[g.off[i]], (const void*)(uintptr_t)kTabla[i].addr, kTabla[i].tam);
    }
    for (unsigned i = 0; i < kCadenasN; i++) { // cadenas de .rdata: el texto propio debe coincidir con el de la direccion
        const Cadena& c = kCadenas[i];
        char buf[256] = {};
        const size_t n = strlen(c.texto) + 1;
        if (n > sizeof(buf) || !LeerSeguro(buf, (const void*)(uintptr_t)c.addr, n) || memcmp(buf, c.texto, n)) {
            fprintf(g.f, "CADENA_DIV 0x%08X %s propia=\"%s\" mem=\"%.60s\"\n", c.addr, c.nombre, c.texto, buf);
        }
    }
    fprintf(g.f, "CADENAS comprobadas=%u\n", kCadenasN);
    fflush(g.f);
    const DWORD per = p ? (DWORD)atoi(p) : 2000;
    std::vector<uint8_t> ob, me, ob2, me2;
    std::vector<uint8_t> prevOb, prevMem; // ultimo estado divergente registrado
    std::vector<uint8_t> conocido(kTablaN, 0);
    for (unsigned pass = 0;; pass++) {
        unsigned ndiv = 0;
        for (unsigned i = 0; i < kTablaN; i++) {
            const Entrada& e = kTabla[i];
            if (ob.size() < e.tam) { ob.resize(e.tam); me.resize(e.tam); ob2.resize(e.tam); me2.resize(e.tam); }
            if (!LeerSeguro(me.data(), (const void*)(uintptr_t)e.addr, e.tam)) continue;
            memcpy(ob.data(), e.objeto, e.tam);
            if (!memcmp(ob.data(), me.data(), e.tam)) continue;
            // confirmacion: debe repetirse con los mismos bytes unos instantes despues (evita escrituras a medias)
            Sleep(1);
            if (!LeerSeguro(me2.data(), (const void*)(uintptr_t)e.addr, e.tam)) continue;
            memcpy(ob2.data(), e.objeto, e.tam);
            if (memcmp(ob.data(), ob2.data(), e.tam) || memcmp(me.data(), me2.data(), e.tam)) continue;
            ndiv++;
            if (g.logs[i] < 6) {
                Linea(g.logs[i] == 0 ? "DIV" : "CAMBIO", i, ob.data(), me.data());
                g.logs[i]++;
            }
            conocido[i] = 1;
        }
        fprintf(g.f, "PASADA %u t=%lu divergentes_ahora=%u\n", pass, (unsigned long)(GetTickCount() - g.t0), ndiv);
        fflush(g.f);
        Sleep(per);
    }
    return 0;
}
} // namespace

void Iniciar() {
    if (gIniciado) return;
    const char* path = getenv("DATOSEXE_VERIF");
    if (!path || !*path) return;
    gIniciado = true;
    g.f = fopen(path, "a");
    if (!g.f) return;
    g.t0 = GetTickCount();
    g.off.resize(kTablaN + 1);
    unsigned tot = 0;
    for (unsigned i = 0; i < kTablaN; i++) { g.off[i] = tot; tot += kTabla[i].tam; }
    g.off[kTablaN] = tot;
    g.iniObj.assign(tot, 0);
    g.iniMem.assign(tot, 0);
    g.logs.assign(kTablaN, 0);
    fprintf(g.f, "INICIO objetos=%u bytes=%u\n", kTablaN, tot);
    fflush(g.f);
    CreateThread(nullptr, 0, Hilo, nullptr, 0, nullptr);
}

namespace {
// Al cargar el modulo (ASI) o al ejecutarse los inicializadores estaticos del independiente.
struct Arranque {
    Arranque() { Iniciar(); }
} gArranque;
} // namespace
} // namespace DatosExe::Verif
