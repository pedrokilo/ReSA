// Prueba del plantado EXACTO de T-1 con 64 bits de mantisa (FlcX87::Commit con FlcPlanExt / FlcPlantExt1 / FlcStoreExt de X87Math.cpp):
//   1) _CIasin / _CIacos del EXE (0x821E70 / 0x822380) frente a FlcCrtAsin / FlcCrtAcos + Commit, FNSAVE completo (registros vacios incluidos), con +-1 exacto y PE limpio (el caso
//      que dejaba +-pi/2 y pi de 64 bits en T-1);
//   2) Commit frente a una referencia de hardware (fld tbyte de valores de 80 bits y vaciado de la pila) a profundidades 1..8, con y sin `fstp dword` final: T-1 con mantisa de 64 bits
//      arbitraria; T-2 y mas profundos con valores de 53 bits (limite documentado en el README del banco).
// Este fichero usa __asm: es herramienta de prueba (fuera de source/). Uso: bld.bat y obj\ext_test.exe [casos] [0=asin | 1=acos | 2=commit]
#include "exe_env.h"
#include "X87Math.h"
#include <map>

static u64 rngs = 88172645463325252ull;
static u32 R32() { rngs ^= rngs << 13; rngs ^= rngs >> 7; rngs ^= rngs << 17; return u32(rngs >> 16); }
static u64 R64() { return (u64(R32()) << 32) ^ R32() ^ (u64(R32()) << 17); }
static const u32 pool[] = {
    0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x3F000000, 0xBF000000, 0x40000000, 0x40490FDB, 0xC0490FDB, 0x7F7FFFFF, 0xFF7FFFFF, 0x00000001, 0x80000001, 0x007FFFFF, 0x807FFFFF, 0x00800000,
    0x7F800000, 0xFF800000, 0x7FC00000, 0xFFC00000, 0x7FC12345, 0xFFC54321, 0x3F7FFFFF, 0x3F800001, 0xBF7FFFFF, 0xBF800001, 0x4B000000, 0x5F000000, 0x1F800000, 0x0DA24260, 0x3EAAAAAB, 0x3F333333,
    0x358637BD, 0x3F7D70A4, 0x3C23D70A, 0x42C80000, 0x3F666666, 0x3DCCCCCD
};
static float FBits(u32 b) { float f; memcpy(&f, &b, 4); return f; }
static u32 RandF(bool unit) {
    if (R32() % 3 == 0) return pool[R32() % (sizeof(pool) / 4)];
    if (R32() % 7 == 0) return pool[R32() % (sizeof(pool) / 4)] + (int)(R32() % 5) - 2;
    float v = (float(R32() % 2000001) / 1000000.0f - 1.0f) * (unit ? 1.0f : 100.0f);
    u32 b; memcpy(&b, &v, 4); return b;
}
static int gErrno;
static int* __cdecl MErrno() { return &gErrno; }

__declspec(noinline) static void CallX87(uintptr_t fn, const double* a, const void* prime, Img* out, double* res, u32* mxcsrOut) {
    u8* o = out->b;
    u32 m0 = 0x1F80, m1 = 0;
    __asm {
        ldmxcsr m0
        mov eax, prime
        frstor [eax]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        mov eax, a
        fld qword ptr [eax]
        mov eax, 0A5123456h
        mov edx, 0C0DE5921h
        mov ecx, 0B1234567h
        mov ebx, fn
        call ebx
        stmxcsr m1
        mov ebx, res
        fstp qword ptr [ebx]
        mov eax, o
        fnsave [eax]
    }
    *mxcsrOut = m1;
}
static u32 SetMx(u32 v) { __asm { ldmxcsr v } return v; }
static u32 GetMx() { u32 m = 0; __asm { stmxcsr m } return m; }

// Referencia de hardware: apila n valores de 80 bits con fld tbyte (vals[n-1] primero, de modo que el que acabe en la cima es vals[0] del vector recibido) y los saca todos; el ultimo en salir (T-1) con `fstp dword` si store != 0.
__declspec(noinline) static void HwReference(const void* prime, const u8 (*vals)[10], int n, int store, Img* out) {
    u8* o = out->b;
    float sink;
    __asm {
        mov eax, prime
        frstor [eax]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        mov ecx, n
        mov edx, vals
        mov eax, ecx
        imul eax, eax, 10
        add edx, eax
    push_loop:
        sub edx, 10
        fld tbyte ptr [edx]
        dec ecx
        jnz push_loop
        mov ecx, n
        dec ecx
        jz last
    pop_loop:
        fstp st(0)
        dec ecx
        jnz pop_loop
    last:
        cmp store, 0
        jne do_store
        fstp st(0)
        jmp done
    do_store:
        fstp dword ptr [sink]
    done:
        mov eax, o
        fnsave [eax]
    }
}


static void BytesFromExt(const FlcExt& e, u8* b) {
    unsigned short se = 0;
    u64 sig = e.sig;
    if (e.kind == FLC_ZERO) { sig = 0; se = 0; }
    else if (e.kind == FLC_INF || e.kind == FLC_NAN) se = 0x7FFF;
    else se = (unsigned short)(e.exp + 16383);
    se |= (unsigned short)(e.neg ? 0x8000 : 0);
    memcpy(b, &sig, 8); memcpy(b + 8, &se, 2);
}
static FlcExt RandExt() {
    FlcExt e{};
    const u32 k = R32() % 12;
    if (k == 0) { e.kind = FLC_ZERO; e.neg = R32() & 1; return e; }
    if (k == 1) { e.kind = FLC_NORMAL; e.neg = R32() & 1; e.sig = 0xC90FDAA22168C235ull; e.exp = (int)(R32() % 3) - 1; return e; }     // +-pi/2, +-pi, +-pi/4
    if (k == 2) { e.kind = FLC_INF; e.neg = R32() & 1; e.sig = 1ull << 63; return e; }
    if (k == 3) { e.kind = FLC_NAN; e.neg = R32() & 1; e.sig = 0xC000000000000000ull; return e; }
    if (k <= 5) { double d = FBits(RandF(false)); unsigned int f = 0; u64 b; memcpy(&b, &d, 8); FlcExt x = FlcFromDouble(b, &f); if (x.kind == FLC_NORMAL) return x; e.kind = FLC_ZERO; return e; }   // un float: entra por 53 bits
    e.kind = FLC_NORMAL; e.neg = R32() & 1;
    e.sig = R64() | (1ull << 63);
    if (k == 6) e.sig &= ~0x7FFull | (1ull << 63);                         // 53 bits justos
    const u32 r = R32() % 8;
    e.exp = r < 5 ? (int)(R32() % 61) - 30 : (r == 5 ? (int)(R32() % 2000) - 1000 : (r == 6 ? 1023 - (int)(R32() % 3) : -1022 + (int)(R32() % 3)));
    return e;
}

struct Cmp { long n = 0, fail = 0; };

int main(int argc, char** argv) {
    if (argc < 2 || std::strcmp(argv[1], "--child") != 0) return Launcher(argc, argv);
    setvbuf(stdout, nullptr, _IONBF, 0);
    unsigned cw = 0; _controlfp_s(&cw, _PC_24, _MCW_PC);
    Hook(0x82B181, (void*)&MErrno);
    *(u32*)0xC9C400 = 1; *(u32*)0xC9C404 = 1;
    *(u32*)0xC9ABFC = 0;
    const int N = argc > 2 ? atoi(argv[2]) : 20000;
    const int which = argc > 3 ? atoi(argv[3]) : -1;
    const char* names[] = {"asin", "acos"};
    long totalFail = 0;
    for (int fn = 0; fn < 2; ++fn) {
        if (which >= 0 && which != fn) continue;
        Cmp c; int shown = 0; long exact = 0, exactClean = 0;
        for (int n = 0; n < N; ++n) {
            double a;
            { float fa = FBits(RandF(true)); a = fa; }
            if (R32() % 2 == 0) a = (R32() & 1) ? 1.0 : -1.0;                       // +-1 exacto en la mitad de los casos
            if (R32() % 12 == 0) { u64 x = ((u64)R32() << 32) | R32(); memcpy(&a, &x, 8); if ((x >> 52 & 0x7FF) > 0x600) a = FBits(RandF(true)); }
            const u32 fsws[6] = {0, 0x20, 0x3F, 0x01, 0x4500, 0x0004};
            const u32 fsw = fsws[R32() % 6];
            u8 prime[108]; BuildPrime(prime, fsw);
            Img ex, sw; double exRes = 0, swRes = 0; u32 exMx, swMx;
            gErrno = 0;
            CallX87(fn == 0 ? 0x821E70 : 0x822380, &a, prime, &ex, &exRes, &exMx);
            const int exErr = gErrno;
            gErrno = 0;
            {
                FlcX87 m;
                SetMx(0x1F80);
                HwPrime(prime);
                m.FldD(a); if (fn == 0) FlcCrtAsin(m); else FlcCrtAcos(m); m.FstpD(swRes);
                swMx = GetMx();
                HwPrime(prime);
                m.Commit();
                HwSave(sw.b);
            }
            const int swErr = gErrno;
            ++c.n;
            if (a == 1.0 || a == -1.0) { ++exact; if (!(fsw & 0x20)) ++exactClean; }
            std::string d = DiffImg(ex.b, sw.b);
            if (memcmp(&exRes, &swRes, 8) && !(exRes != exRes && swRes != swRes)) d += " res";
            if (exErr != swErr) d += " errno";
            if (((exMx ^ swMx) & 0x3F)) d += " mxcsr";
            if (!d.empty()) { ++c.fail; if (shown++ < 12) { u64 ab; memcpy(&ab, &a, 8); printf("[%s] FALLO a=%016llx fsw0=%04x:%s\n", names[fn], ab, fsw, d.c_str()); } }
        }
        printf("%s: %ld casos (|x|==1: %ld, de ellos PE limpio en la entrada: %ld), %ld diferencias\n", names[fn], c.n, exact, exactClean, c.fail);
        totalFail += c.fail;
    }
    // ---------------------------------------------------------------- 2) Commit frente a referencia de hardware
    if (which < 0 || which == 2) {
        Cmp c; int shown = 0; long ext = 0;
        for (int n = 0; n < N; ++n) {
            const int depth = 1 + (int)(R32() % 8);
            const int store = (int)(R32() % 3 == 0);
            FlcExt vals[8]; u8 bytes[8][10]; bool anyExt = false;
            for (int i = 0; i < depth; ++i) { vals[i] = RandExt(); if (i >= 1 && vals[i].kind == FLC_NORMAL) { unsigned int fl = 0; vals[i] = FlcFromDouble(FlcToDouble(vals[i]), &fl); } BytesFromExt(vals[i], bytes[i]); if (vals[i].kind == FLC_NORMAL && (vals[i].sig & 0x7FF)) anyExt = true; }   // T-1 con 64 bits; el resto de 53 (limite documentado)
            // Orden de apilado: bytes[0] primero (queda en T-1), bytes[depth-1] el ultimo (cima). HwReference apila el vector de atras hacia delante: se le pasa invertido.
            u8 ordered[8][10];
            for (int i = 0; i < depth; ++i) memcpy(ordered[i], bytes[depth - 1 - i], 10);
            const u32 fsws[6] = {0, 0x20, 0x3F, 0x01, 0x4500, 0x0004};
            const u32 fsw = fsws[R32() % 6];
            u8 prime[108]; BuildPrime(prime, fsw);
            Img ref, sw;

            HwReference(prime, ordered, depth, store, &ref);
            {
                FlcX87 m;
                SetMx(0x1F80);
                HwPrime(prime);
                for (int i = 0; i < depth; ++i) m.FldExt(vals[i]);
                for (int i = depth - 1; i >= 1; --i) m.Pop();
                if (store) { float sink; m.Fstp(sink); } else m.Pop();
                HwPrime(prime);
                m.Commit();
                HwSave(sw.b);
            }
            ++c.n; ext += anyExt;
            std::string d = DiffImg(ref.b, sw.b);
            if (!d.empty()) {
                ++c.fail;
                if (shown++ < 12) { printf("[commit] FALLO depth=%d store=%d fsw0=%04x:%s\n   vals:", depth, store, fsw, d.c_str()); for (int i = 0; i < depth; ++i) printf(" %s", Hex(bytes[i], 10).c_str()); printf("\n"); }
            }
        }
        printf("commit: %ld casos (con algun valor de mas de 53 bits: %ld), %ld diferencias\n", c.n, ext, c.fail);
        totalFail += c.fail;
    }
    return totalFail ? 1 : 0;
}
