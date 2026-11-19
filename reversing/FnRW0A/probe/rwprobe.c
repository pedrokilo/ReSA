/*
 * rwprobe.asi - sonda SOLO del arnes de la prueba de juego RW0A. No forma parte de la DLL de produccion
 * y no cambia su comportamiento: se carga despues de gta_reversed.asi (nombre zz_rwprobe.asi) y
 *
 *  1. lee la lista de direcciones del catalogo (RWPROBE_ADDRS) y comprueba en memoria cuales
 *     quedaron realmente enganchadas (primer byte E9 con destino dentro de gta_reversed.asi);
 *  2. antepone a cada gancho un trampolin de 22 bytes que cuenta las llamadas (lock inc) y
 *     anota la ultima funcion llamada; si el destino es un thunk de enlace incremental (E9),
 *     se cuenta tambien en el thunk, y asi se ven las llamadas de la propia DLL (C++ -> C++);
 *  3. cada 5 s escribe una linea de estado (fotograma, estado del juego, menu, posicion del jugador);
 *     cada 20 s vuelca los contadores; ante una excepcion grave o una peticion del arnes escribe
 *     la pila con simbolos (dbghelp) y la funcion de RenderWare activa.
 *
 * Todo el registro es en flujo y con limite de tamano. Entorno: RWPROBE_OUT (carpeta de salida),
 * RWPROBE_ADDRS (archivo con una direccion hexadecimal por linea, 1.er campo).
 */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dbghelp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xmmintrin.h>

#define MAXH 1200
typedef struct { DWORD addr; DWORD target; DWORD thunk; int tid; int installed; } Hook;
static Hook H[MAXH];
static int NH;
static volatile LONG cntExe[MAXH];
static volatile LONG cntThunk[MAXH];
static DWORD thunkAddr[MAXH];
static int NT;
static volatile DWORD lastExe, lastThunk;
static char OUTD[MAX_PATH];
static DWORD dllBase, dllSize;
static volatile LONG stopFlag;
static volatile LONG crashCount;
static DWORD mainTid;
static HANDLE hProc;
static BYTE *tramp;
static int fpTrap;                 /* RWPROBE_FPTRAP=1: desenmascara la excepcion SSE de operacion invalida en el hilo principal */
typedef struct { DWORD eip; long count; DWORD first_ms; DWORD ret[6]; } FpSite;
static FpSite FS[400];
static int NFS;
static volatile LONG fpRearm;
static DWORD tStart;
static volatile DWORD firstNanMs;
static size_t trampUsed;

static void put32(BYTE *p, DWORD v) { memcpy(p, &v, 4); }

static int inDll(DWORD a) { return a >= dllBase && a < dllBase + dllSize; }

static void path(char *dst, const char *name) { snprintf(dst, MAX_PATH, "%s\\%s", OUTD, name); }

static BYTE *newTramp(void) { BYTE *p = tramp + trampUsed; trampUsed += 32; return p; }

static void writeJmp(DWORD at, DWORD to) {
    DWORD old;
    BYTE b[5];
    b[0] = 0xE9;
    put32(b + 1, to - (at + 5));
    VirtualProtect((void *)at, 5, PAGE_EXECUTE_READWRITE, &old);
    memcpy((void *)at, b, 5);
    VirtualProtect((void *)at, 5, old, &old);
    FlushInstructionCache(hProc, (void *)at, 5);
}

static BYTE *mkTramp(DWORD *lastVar, DWORD id, volatile LONG *counter, DWORD dest) {
    BYTE *p = newTramp(), *q = p;
    *q++ = 0xC7; *q++ = 0x05; put32(q, (DWORD)lastVar); q += 4; put32(q, id); q += 4; /* mov [last], id */
    *q++ = 0xF0; *q++ = 0xFF; *q++ = 0x05; put32(q, (DWORD)counter); q += 4;          /* lock inc [counter] */
    *q++ = 0xE9; put32(q, dest - ((DWORD)q + 4)); q += 4;                             /* jmp dest */
    return p;
}

static void installHooks(const char *addrFile) {
    FILE *f = fopen(addrFile, "r");
    char line[512];
    char p[MAX_PATH];
    FILE *st;
    if (!f) return;
    tramp = (BYTE *)VirtualAlloc(NULL, MAXH * 2 * 32, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    while (NH < MAXH && fgets(line, sizeof line, f)) {
        DWORD a = (DWORD)strtoul(line, NULL, 16);
        int dup = 0, i;
        if (a < 0x401000) continue;
        for (i = 0; i < NH; i++) if (H[i].addr == a) dup = 1;
        if (dup) continue;
        H[NH].addr = a; H[NH].tid = -1;
        NH++;
    }
    fclose(f);
    path(p, "hooks_state.csv");
    st = fopen(p, "w");
    if (st) fprintf(st, "addr,installed,dll_target,thunk,thunk_target\n");
    for (int i = 0; i < NH; i++) {
        DWORD a = H[i].addr;
        BYTE *m = (BYTE *)a;
        DWORD thunkTarget = 0;
        if (m[0] == 0xE9) {
            DWORD t = a + 5 + *(DWORD *)(m + 1);
            if (inDll(t)) {
                H[i].installed = 1;
                H[i].target = t;
                /* thunk de enlace incremental: salto E9 dentro de la DLL */
                if (*(BYTE *)t == 0xE9) {
                    DWORD real = t + 5 + *(DWORD *)((BYTE *)t + 1);
                    int k;
                    for (k = 0; k < NT; k++) if (thunkAddr[k] == t) break;
                    if (k == NT) {
                        thunkAddr[NT++] = t;
                        BYTE *tr = mkTramp((DWORD *)&lastThunk, (DWORD)k, &cntThunk[k], real);
                        writeJmp(t, (DWORD)tr);
                    }
                    H[i].thunk = t; H[i].tid = k; thunkTarget = real;
                }
                /* trampolin del lado del EXE */
                {
                    BYTE *tr = mkTramp((DWORD *)&lastExe, (DWORD)i, &cntExe[i], t);
                    writeJmp(a, (DWORD)tr);
                }
            }
        }
        if (st) fprintf(st, "0x%08X,%d,0x%08X,%d,0x%08X\n", a, H[i].installed, H[i].target ? H[i].target - dllBase : 0, H[i].thunk ? 1 : 0, thunkTarget ? thunkTarget - dllBase : 0);
    }
    if (st) fclose(st);
}

static void dumpCounters(void) {
    char p[MAX_PATH], q[MAX_PATH];
    FILE *f;
    path(p, "counters.csv.tmp"); path(q, "counters.csv");
    f = fopen(p, "w");
    if (!f) return;
    fprintf(f, "addr,installed,exe_calls,total_calls\n");
    for (int i = 0; i < NH; i++) {
        LONG e = cntExe[i];
        LONG t = H[i].tid >= 0 ? cntThunk[H[i].tid] : -1;
        fprintf(f, "0x%08X,%d,%ld,%ld\n", H[i].addr, H[i].installed, e, t);
    }
    fprintf(f, "#last_exe,%lu,last_thunk,%lu\n", (unsigned long)lastExe, (unsigned long)lastThunk);
    fclose(f);
    MoveFileExA(p, q, MOVEFILE_REPLACE_EXISTING);
}

/* ---------- pila con simbolos ---------- */
static int symInit;
static void ensureSyms(void) {
    if (symInit) return;
    symInit = 1;
    SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);
    char sp[MAX_PATH * 2];
    snprintf(sp, sizeof sp, "%s", OUTD);
    char *e = getenv("RWPROBE_PDBDIR");
    if (e) snprintf(sp, sizeof sp, "%s", e);
    SymInitialize(hProc, sp, TRUE);
}

static const char *rwFuncFor(DWORD ip, DWORD *base) {
    DWORD best = 0; int bi = -1;
    for (int i = 0; i < NH; i++) if (H[i].addr <= ip && H[i].addr > best) { best = H[i].addr; bi = i; }
    if (bi >= 0 && ip - best < 0x4000 && ip >= 0x401000 && ip < 0xC00000) { *base = best; return "exe"; }
    return NULL;
}

static void writeStack(const char *why, HANDLE thread, CONTEXT *ctx, FILE *f) {
    STACKFRAME64 sf;
    memset(&sf, 0, sizeof sf);
    sf.AddrPC.Offset = ctx->Eip; sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrFrame.Offset = ctx->Ebp; sf.AddrFrame.Mode = AddrModeFlat;
    sf.AddrStack.Offset = ctx->Esp; sf.AddrStack.Mode = AddrModeFlat;
    ensureSyms();
    fprintf(f, "=== %s ===\nEIP=%08lX ESP=%08lX EBP=%08lX EAX=%08lX ECX=%08lX EDX=%08lX EBX=%08lX ESI=%08lX EDI=%08lX\n",
            why, ctx->Eip, ctx->Esp, ctx->Ebp, ctx->Eax, ctx->Ecx, ctx->Edx, ctx->Ebx, ctx->Esi, ctx->Edi);
    fprintf(f, "dll_base=%08lX last_exe=%lu (0x%08lX) last_thunk=%lu\n", (unsigned long)dllBase, (unsigned long)lastExe,
            lastExe < (DWORD)NH ? (unsigned long)H[lastExe].addr : 0UL, (unsigned long)lastThunk);
    for (int n = 0; n < 48; n++) {
        char buf[sizeof(SYMBOL_INFO) + 256];
        SYMBOL_INFO *s = (SYMBOL_INFO *)buf;
        DWORD64 disp = 0;
        DWORD ip = (DWORD)sf.AddrPC.Offset;
        const char *name = "?";
        DWORD rb = 0;
        memset(buf, 0, sizeof buf);
        s->SizeOfStruct = sizeof(SYMBOL_INFO); s->MaxNameLen = 255;
        if (SymFromAddr(hProc, ip, &disp, s)) name = s->Name;
        fprintf(f, " #%02d %08lX %s+0x%llX", n, (unsigned long)ip, name, (unsigned long long)disp);
        if (inDll(ip)) fprintf(f, "  [gta_reversed+0x%lX]", (unsigned long)(ip - dllBase));
        else if (rwFuncFor(ip, &rb)) fprintf(f, "  [EXE; gancho previo 0x%08lX +0x%lX]", (unsigned long)rb, (unsigned long)(ip - rb));
        fprintf(f, "\n");
        if (!StackWalk64(IMAGE_FILE_MACHINE_I386, hProc, thread, &sf, ctx, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) break;
        if (!sf.AddrPC.Offset) break;
    }
    /* palabras crudas de la pila, por si los simbolos fallan */
    fprintf(f, "pila cruda:");
    __try {
        DWORD *sp = (DWORD *)ctx->Esp;
        for (int i = 0; i < 64; i++) fprintf(f, "%s%08lX", (i % 8) ? " " : "\n  ", (unsigned long)sp[i]);
    } __except (EXCEPTION_EXECUTE_HANDLER) {}
    fprintf(f, "\n");
    fflush(f);
}

typedef struct { CONTEXT ctx; HANDLE thread; char why[200]; } StackJob;
static DWORD WINAPI stackThread(LPVOID p) {
    StackJob *j = (StackJob *)p;
    char pth[MAX_PATH];
    FILE *f;
    path(pth, "crash.log");
    f = fopen(pth, "a");
    if (f) {
        long sz;
        fseek(f, 0, SEEK_END); sz = ftell(f);
        if (sz < 400000) writeStack(j->why, j->thread, &j->ctx, f);
        fclose(f);
    }
    return 0;
}

static void stackDump(const char *why, HANDLE thread, CONTEXT *ctx) {
    StackJob *j = (StackJob *)calloc(1, sizeof *j);
    HANDLE t;
    j->ctx = *ctx; j->thread = thread; snprintf(j->why, sizeof j->why, "%s", why);
    t = CreateThread(NULL, 0, stackThread, j, 0, NULL);
    if (t) { WaitForSingleObject(t, 60000); CloseHandle(t); }
}

static void fpLogSite(FpSite *x) {
    char p[MAX_PATH]; FILE *f;
    path(p, "fptrap.log");
    f = fopen(p, "a");
    if (!f) return;
    fprintf(f, "t=%lu ms eip=%08lX", (unsigned long)x->first_ms, (unsigned long)x->eip);
    if (inDll(x->eip)) fprintf(f, " [gta_reversed+0x%lX]", (unsigned long)(x->eip - dllBase));
    fprintf(f, " ret:");
    for (int i = 0; i < 6 && x->ret[i]; i++) {
        fprintf(f, " %08lX", (unsigned long)x->ret[i]);
        if (inDll(x->ret[i])) fprintf(f, "[+0x%lX]", (unsigned long)(x->ret[i] - dllBase));
    }
    fprintf(f, " last_exe=%lu(0x%08lX) last_thunk=%lu\n", (unsigned long)lastExe, lastExe < (DWORD)NH ? (unsigned long)H[lastExe].addr : 0UL, (unsigned long)lastThunk);
    fclose(f);
}

static int fpHandle(EXCEPTION_POINTERS *ep) {
    CONTEXT *cx = ep->ContextRecord;
    DWORD c = ep->ExceptionRecord->ExceptionCode;
    if (!fpTrap) return 0;
    if (c == EXCEPTION_SINGLE_STEP && fpRearm) {
        if (cx->ContextFlags & CONTEXT_EXTENDED_REGISTERS) {
            DWORD *mx = (DWORD *)&cx->ExtendedRegisters[24];
            *mx &= ~0x80u;
        } else {
            _mm_setcsr(_mm_getcsr() & ~0x80u);
        }
        cx->EFlags &= ~0x100u;
        fpRearm = 0;
        return 1;
    }
    if (c == EXCEPTION_FLT_INVALID_OPERATION || c == 0xC00002B4 || c == 0xC00002B5) {
        int ext = (cx->ContextFlags & CONTEXT_EXTENDED_REGISTERS) != 0;
        DWORD *mx = ext ? (DWORD *)&cx->ExtendedRegisters[24] : NULL;
        int i, found = -1;
        DWORD eip = cx->Eip;
        for (i = 0; i < NFS; i++) if (FS[i].eip == eip) { found = i; break; }
        if (found < 0 && NFS < 400) {
            DWORD *fp = (DWORD *)cx->Ebp;
            FpSite *x = &FS[NFS];
            x->eip = eip; x->first_ms = GetTickCount() - tStart;
            __try {
                for (i = 0; i < 6 && fp && (DWORD)fp > 0x10000; i++) { x->ret[i] = fp[1]; fp = (DWORD *)fp[0]; }
            } __except (EXCEPTION_EXECUTE_HANDLER) {}
            found = NFS++;
            fpLogSite(x);
        }
        if (found >= 0) InterlockedIncrement(&FS[found].count);
        if (ext) { *mx |= 0x80u; *mx &= ~0x3Fu; }   /* enmascara IM y limpia banderas */
        else _mm_setcsr((_mm_getcsr() | 0x80u) & ~0x3Fu);
        cx->EFlags |= 0x100u;    /* un paso y se vuelve a desenmascarar */
        fpRearm = 1;
        return 1;
    }
    return 0;
}

static LONG CALLBACK vectored(EXCEPTION_POINTERS *ep) {
    DWORD c = ep->ExceptionRecord->ExceptionCode;
    if (fpHandle(ep)) return EXCEPTION_CONTINUE_EXECUTION;
    if (fpTrap && (c == EXCEPTION_FLT_INVALID_OPERATION || c == 0xC00002B4 || c == 0xC00002B5)) return EXCEPTION_CONTINUE_SEARCH;
    int serious = c == EXCEPTION_ACCESS_VIOLATION || c == EXCEPTION_STACK_OVERFLOW || c == EXCEPTION_ILLEGAL_INSTRUCTION ||
                  c == EXCEPTION_INT_DIVIDE_BY_ZERO || c == EXCEPTION_PRIV_INSTRUCTION || c == EXCEPTION_ARRAY_BOUNDS_EXCEEDED ||
                  c == EXCEPTION_BREAKPOINT || c == EXCEPTION_IN_PAGE_ERROR || c == 0xC0000374 || c == 0xC0000409 ||
                  c == EXCEPTION_FLT_INVALID_OPERATION || c == EXCEPTION_INT_OVERFLOW;
    if (!serious) return EXCEPTION_CONTINUE_SEARCH;
    if (InterlockedIncrement(&crashCount) > 12) return EXCEPTION_CONTINUE_SEARCH;
    {
        char why[200];
        snprintf(why, sizeof why, "excepcion 0x%08lX en %p (primera oportunidad; puede estar controlada) hilo %lu",
                 (unsigned long)c, ep->ExceptionRecord->ExceptionAddress, (unsigned long)GetCurrentThreadId());
        stackDump(why, GetCurrentThread(), ep->ContextRecord);
    }
    dumpCounters();
    return EXCEPTION_CONTINUE_SEARCH;
}

static BOOL CALLBACK findWnd(HWND h, LPARAM l) {
    DWORD pid; DWORD tid = GetWindowThreadProcessId(h, &pid);
    if (pid == GetCurrentProcessId() && IsWindowVisible(h)) { *(DWORD *)l = tid; return FALSE; }
    return TRUE;
}

static int rd(DWORD a, void *dst, size_t n) {
    __try { memcpy(dst, (void *)a, n); return 1; } __except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

static DWORD WINAPI sampler(LPVOID unused) {
    char p[MAX_PATH];
    FILE *f;
    DWORD t0 = tStart, lastC = 0, lastS = 0;
    int armed = 0;
    long written = 0;
    (void)unused;
    path(p, "state.jsonl");
    f = fopen(p, "w");
    while (!stopFlag) {
        DWORD now = GetTickCount() - t0;
        char cmdp[MAX_PATH];
        path(cmdp, "rwprobe.cmd");
        {   /* ordenes del arnes */
            FILE *c = fopen(cmdp, "r");
            if (c) {
                char w[64] = {0};
                fgets(w, sizeof w, c); fclose(c); DeleteFileA(cmdp);
                if (strstr(w, "counters")) dumpCounters();
                if (strstr(w, "stack")) {
                    DWORD tid = 0; HANDLE th;
                    EnumWindows(findWnd, (LPARAM)&tid);
                    if (!tid) tid = mainTid;
                    th = OpenThread(THREAD_ALL_ACCESS, FALSE, tid);
                    if (th) {
                        CONTEXT cx; memset(&cx, 0, sizeof cx); cx.ContextFlags = CONTEXT_FULL;
                        SuspendThread(th);
                        if (GetThreadContext(th, &cx)) stackDump("peticion del arnes (hilo de la ventana)", th, &cx);
                        ResumeThread(th); CloseHandle(th);
                    }
                    dumpCounters();
                }
                {
                    char ack[MAX_PATH]; FILE *a; path(ack, "rwprobe.ack");
                    a = fopen(ack, "w"); if (a) { fprintf(a, "%s\n", w); fclose(a); }
                }
            }
        }
        {   /* armado de la trampa SSE y vigilancia rapida de NaN en la posicion del jugador */
            DWORD gs = 0, fc = 0, ped = 0, mtx = 0; float x = 0;
            rd(0xC8D4C0, &gs, 4); rd(0xB7CB4C, &fc, 4);
            if (gs == 9 && fc > 0 && getenv("RWPROBE_HOUR")) {   /* congela la hora del juego (hora fija, minutos corren) */
                BYTE h = (BYTE)atoi(getenv("RWPROBE_HOUR")), z0 = 0;
                DWORD old; VirtualProtect((void *)0xB70150, 8, PAGE_READWRITE, &old);
                *(BYTE *)0xB70153 = h; *(BYTE *)0xB70152 = z0;
            }
            if (fpTrap && !armed && gs == 9 && fc > 0) {
                DWORD tid = 0; HANDLE th;
                EnumWindows(findWnd, (LPARAM)&tid);
                th = tid ? OpenThread(THREAD_ALL_ACCESS, FALSE, tid) : NULL;
                if (th) {
                    CONTEXT cx; memset(&cx, 0, sizeof cx); cx.ContextFlags = CONTEXT_FULL | CONTEXT_EXTENDED_REGISTERS;
                    SuspendThread(th);
                    if (GetThreadContext(th, &cx)) {
                        DWORD *mx = (DWORD *)&cx.ExtendedRegisters[24];
                        *mx &= ~0x80u; *mx &= ~0x3Fu;
                        SetThreadContext(th, &cx);
                        armed = 1;
                        { char pp[MAX_PATH]; FILE *lf; path(pp, "fptrap.log"); lf = fopen(pp, "a"); if (lf) { fprintf(lf, "trampa SSE armada t=%lu ms hilo=%lu\n", (unsigned long)(GetTickCount() - tStart), (unsigned long)tid); fclose(lf); } }
                    }
                    ResumeThread(th); CloseHandle(th);
                }
            }
            if (!firstNanMs && gs == 9) {
                rd(0xB7CD98, &ped, 4);
                if (ped && rd(ped + 0x14, &mtx, 4) && mtx && rd(mtx + 0x30, &x, 4) && x != x) {
                    char pp[MAX_PATH]; FILE *lf;
                    firstNanMs = GetTickCount() - tStart; if (!firstNanMs) firstNanMs = 1;
                    path(pp, "fptrap.log"); lf = fopen(pp, "a");
                    if (lf) { fprintf(lf, "PRIMER NaN en la posicion del jugador t=%lu ms (frame %lu) last_exe=%lu last_thunk=%lu\n", (unsigned long)firstNanMs, (unsigned long)fc, (unsigned long)lastExe, (unsigned long)lastThunk); fclose(lf); }
                    dumpCounters();
                }
            }
        }
        if (now - lastS >= 5000 && f && written < 3000000) {
            DWORD gs = 0, fc = 0, tm = 0, ped = 0, mtx = 0; BYTE menu = 0, hh = 0, mm = 0; float x = 0, y = 0, z = 0;
            int nz = 0;
            lastS = now;
            rd(0xC8D4C0, &gs, 4); rd(0xB7CB4C, &fc, 4); rd(0xB7CB84, &tm, 4); rd(0xBA67A4, &menu, 1);
            rd(0xB70153, &hh, 1); rd(0xB70152, &mm, 1);
            rd(0xB7CD98, &ped, 4);
            if (ped && rd(ped + 0x14, &mtx, 4)) {
                DWORD pa = mtx ? mtx + 0x30 : ped + 4;
                rd(pa, &x, 4); rd(pa + 4, &y, 4); rd(pa + 8, &z, 4);
            }
            for (int i = 0; i < NH; i++) if (cntExe[i] || (H[i].tid >= 0 && cntThunk[H[i].tid])) nz++;
            written += fprintf(f, "{\"hora\":\"%02u:%02u\",\"t_ms\":%lu,\"game_state\":%lu,\"frame\":%lu,\"timer_ms\":%lu,\"menu\":%u,\"ped\":%s,\"x\":%.1f,\"y\":%.1f,\"z\":%.1f,\"hooks_called\":%d}\n",
                               hh, mm, (unsigned long)now, (unsigned long)gs, (unsigned long)fc, (unsigned long)tm, menu, ped ? "true" : "false", x, y, z, nz);
            fflush(f);
        }
        if (now - lastC >= 20000) { lastC = now; dumpCounters(); }
        Sleep(40);
    }
    if (f) fclose(f);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE h, DWORD reason, LPVOID r) {
    (void)r;
    if (reason == DLL_PROCESS_ATTACH) {
        const char *out = getenv("RWPROBE_OUT"), *addrs = getenv("RWPROBE_ADDRS");
        HMODULE g = GetModuleHandleA("gta_reversed.asi");
        DisableThreadLibraryCalls(h);
        if (!out || !addrs) return TRUE;
        snprintf(OUTD, sizeof OUTD, "%s", out);
        hProc = GetCurrentProcess(); mainTid = GetCurrentThreadId(); tStart = GetTickCount();
        if (getenv("RWPROBE_FPTRAP")) fpTrap = 1;
        if (g) {
            IMAGE_DOS_HEADER *d = (IMAGE_DOS_HEADER *)g;
            IMAGE_NT_HEADERS *nt = (IMAGE_NT_HEADERS *)((BYTE *)g + d->e_lfanew);
            dllBase = (DWORD)g; dllSize = nt->OptionalHeader.SizeOfImage;
            installHooks(addrs);
        } else {
            FILE *f; char p[MAX_PATH]; path(p, "hooks_state.csv");
            f = fopen(p, "w"); if (f) { fprintf(f, "ERROR: gta_reversed.asi no esta cargada\n"); fclose(f); }
        }
        AddVectoredExceptionHandler(1, vectored);
        CreateThread(NULL, 0, sampler, NULL, 0, NULL);
    }
    return TRUE;
}
