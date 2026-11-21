// Parte de scanf del banco CRT4A (se incluye desde bench_crt4a.cpp).
static uint64_t g_inOk = 0;
static int call_input(int which, ExeCrt::File* f, const char* fmt, va_list va) {
    typedef int(__cdecl* InFn)(ExeCrt::File*, const unsigned char*, va_list);
    __try { return which == 0 ? ((InFn)0x8290D4)(f, (const unsigned char*)fmt, va) : ExeCrt::Input(f, (const unsigned char*)fmt, va); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return -999; }
}
static void run_input(uint64_t n, uint64_t& fails, uint64_t& total) {
    static const char alpha[] = "0123456789  abcdefABCDEF+-.exXpP%\t\n,;z_";
    for (uint64_t c = 0; c < n; c++) {
        total++;
        std::string fmt; int nconv = 1 + rnd(4);
        for (int k = 0; k < nconv; k++) {
            const uint32_t pre = rnd(8);
            if (pre == 0) fmt += ' '; else if (pre == 1) fmt += ','; else if (pre == 2) fmt += "ab"; else if (pre == 3) fmt += '\t';
            fmt += '%';
            const bool star = rnd(8) == 0;
            if (star) fmt += '*';
            if (rnd(3) == 0) fmt += std::to_string(rnd(rnd(4) ? (rnd(2) ? 5 : 12) : 60));
            const char* types = "diuoxXpcsefgEGn[";
            const char t = types[rnd(16)];
            const bool intT = (t == 'd' || t == 'i' || t == 'u' || t == 'o' || t == 'x' || t == 'X');
            const uint32_t sz = rnd(8);
            if (intT) { if (sz == 0) fmt += 'h'; else if (sz == 1) fmt += 'l'; else if (sz == 2) fmt += "I64"; else if (sz == 3) fmt += "I32"; }
            else if (t == 'e' || t == 'f' || t == 'g' || t == 'E' || t == 'G') { if (sz == 0) fmt += 'l'; else if (sz == 1) fmt += 'L'; }
            else if (t == 's' || t == 'c') { if (sz == 0) fmt += 'l'; else if (sz == 1) fmt += 'w'; else if (sz == 2) fmt += 'h'; }
            if (t == '[') { fmt += '['; if (rnd(3) == 0) fmt += '^'; const char* sets[] = {"0-9", "a-f", "abc", "xyz.", "0-9a-fA-F", "-+", "a-", " ,;", "]a"}; fmt += sets[rnd(9)]; fmt += ']'; }
            else fmt += t;
        }
        if (rnd(4) == 0) fmt += "%n";
        char src[3][96]; const int len = rnd(40);
        for (int i = 0; i < len; i++) src[0][i] = alpha[rnd(sizeof alpha - 1)];
        src[0][len] = 0;
        if (rnd(4) == 0 && len >= 4) { const char* pre[] = {"0x", "-0x", "+0X", "0", "0x0", " 0x"}; const char* q = pre[rnd(6)]; memcpy(src[0], q, strlen(q)); } // prefijos de base
        uint8_t dest[2][12][160]; ExeCrt::File ff[2]; int ret[2];
        for (int w = 0; w < 2; w++) {
            memcpy(src[w + 1], src[0], 96);
            memset(dest[w], 0xCC, sizeof dest[w]);
            ff[w].ptr = src[w + 1]; ff[w].cnt = len; ff[w].base = src[w + 1]; ff[w].flag = 0x49; ff[w].file = 0; ff[w].charbuf = 0; ff[w].bufsiz = 0; ff[w].tmpfname = nullptr;
            std::vector<uint32_t> av; for (int i = 0; i < 12; i++) av.push_back((uint32_t)(uintptr_t)dest[w][i]);
            va_list va = (va_list)av.data();
            ret[w] = call_input(w, &ff[w], fmt.c_str(), va);
        }
        const bool same = ret[0] == ret[1] && ff[0].cnt == ff[1].cnt && (ff[0].ptr - src[1]) == (ff[1].ptr - src[2]) && ff[0].flag == ff[1].flag && !memcmp(dest[0], dest[1], sizeof dest[0]);
        if (ret[0] > 0) g_inOk++;
        if (!same) { if (fails++ < 10) printf("DIFERENCIA input fmt='%s' src='%s' ret exe=%d own=%d cnt %d/%d\n", fmt.c_str(), src[0], ret[0], ret[1], ff[0].cnt, ff[1].cnt); }
    }
}

// Prueba suelta: bench_crt4a.exe <exe> --input "<formato>" "<texto>"
static void one_input(const char* fmt, const char* text) {
    char src[2][96]; ExeCrt::File ff[2]; int ret[2]; uint8_t dest[2][12][160];
    for (int w = 0; w < 2; w++) {
        strcpy(src[w], text); memset(dest[w], 0xCC, sizeof dest[w]);
        ff[w].ptr = src[w]; ff[w].cnt = (int)strlen(text); ff[w].base = src[w]; ff[w].flag = 0x49; ff[w].file = 0; ff[w].charbuf = 0; ff[w].bufsiz = 0; ff[w].tmpfname = nullptr;
        std::vector<uint32_t> av; for (int i = 0; i < 12; i++) av.push_back((uint32_t)(uintptr_t)dest[w][i]);
        ret[w] = call_input(w, &ff[w], fmt, (va_list)av.data());
        printf("%s: ret=%d cnt=%d ptr=%d flag=%x\n", w ? "propio" : "EXE   ", ret[w], ff[w].cnt, (int)(ff[w].ptr - src[w]), ff[w].flag);
    }
}

// ---------------------------------------------------------------- ctype / _ungetc_nolock / _getbuf
static void run_misc(uint64_t n, uint64_t& fails, uint64_t& total) {
    typedef int(__cdecl* IFn)(int);
    // ctype con la tabla real del EXE (-1..255) y con una tabla aleatoria
    static uint16_t rtab[1 + 256]; for (auto& t : rtab) t = (uint16_t)rng();
    unsigned char* loc = (unsigned char*)*(void**)0x8E2F14; uint32_t keep = *(uint32_t*)(loc + 0x48);
    for (int pass = 0; pass < 2; pass++) {
        if (pass == 1) *(uint32_t*)(loc + 0x48) = (uint32_t)(uintptr_t)(rtab + 1);
        for (int c = -1; c < 256; c++) {
            total += 4;
            if (((IFn)0x825D50)(c) != ExeCrt::Tolower(c)) { fails++; printf("DIFERENCIA tolower %d\n", c); }
            if (((IFn)0x822789)(c) != ExeCrt::Isdigit(c)) { fails++; printf("DIFERENCIA isdigit %d\n", c); }
            if (((IFn)0x822802)(c) != ExeCrt::Isspace(c)) { fails++; printf("DIFERENCIA isspace %d\n", c); }
            if (((IFn)0x8227C3)(c) != ExeCrt::Isxdigit(c)) { fails++; printf("DIFERENCIA isxdigit %d\n", c); }
        }
    }
    *(uint32_t*)(loc + 0x48) = keep;
    // _ungetc_nolock y _getbuf
    typedef int(__cdecl* UnFn)(int, ExeCrt::File*); typedef void(__cdecl* GbFn)(ExeCrt::File*);
    for (uint64_t c = 0; c < n; c++) {
        total++;
        char bufA[2][64]; ExeCrt::File f[2]; int r[2];
        const int flag = (int)(rng() & 0x1FF) | (rnd(2) ? 1 : 0); const int cnt = (int)rnd(5), pos = (int)rnd(4); const int ch = rnd(8) == 0 ? -1 : (int)rnd(256);
        const bool noBase = rnd(3) == 0;
        const int flagUse = noBase ? (flag & ~0x40) : flag; // con buffer nuevo y _IOSTRG el EXE compara con memoria sin inicializar (no determinista)
        for (int w = 0; w < 2; w++) {
            memset(bufA[w], 'a', sizeof bufA[w]); bufA[w][5] = 'b';
            f[w].flag = flagUse; f[w].cnt = cnt; f[w].base = noBase ? nullptr : bufA[w]; f[w].ptr = noBase ? nullptr : bufA[w] + pos; f[w].file = 0; f[w].charbuf = 0; f[w].bufsiz = 0; f[w].tmpfname = nullptr;
            r[w] = w == 0 ? ((UnFn)0x83375D)(ch, &f[w]) : ExeCrt::UngetcNolock(ch, &f[w]);
        }
        const bool basesOk = noBase ? ((f[0].base == nullptr) == (f[1].base == nullptr)) : true;
        const long pOff0 = noBase ? (f[0].base ? f[0].ptr - f[0].base : 0) : f[0].ptr - bufA[0], pOff1 = noBase ? (f[1].base ? f[1].ptr - f[1].base : 0) : f[1].ptr - bufA[1];
        if (r[0] != r[1] || f[0].cnt != f[1].cnt || f[0].flag != f[1].flag || pOff0 != pOff1 || f[0].bufsiz != f[1].bufsiz || !basesOk || (!noBase && memcmp(bufA[0], bufA[1], 64))) {
            if (fails++ < 10) printf("DIFERENCIA ungetc flag=%x cnt=%d pos=%d ch=%d base=%d ret %d/%d cnt %d/%d flag %x/%x\n", flag, cnt, pos, ch, !noBase, r[0], r[1], f[0].cnt, f[1].cnt, f[0].flag, f[1].flag);
        }
        if (noBase) { if (f[0].base && f[0].base != (char*)&f[0] + 0x14) HeapFree(GetProcessHeap(), 0, f[0].base); if (f[1].base && f[1].base != (char*)&f[1] + 0x14) HeapFree(GetProcessHeap(), 0, f[1].base); }
    }
}
