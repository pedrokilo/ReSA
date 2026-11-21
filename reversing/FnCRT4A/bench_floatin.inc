// Parte de texto -> coma flotante del banco CRT4A (se incluye desde bench_crt4a.cpp): _strgtold12, _ld12cvt y ayudantes, _mtold12, _atodbl, _atoflt, _fassign.
static std::string rand_numtext() {
    static const char alpha[] = "0123456789000.eE+-  dD,x";
    std::string s;
    const uint32_t kind = rnd(6);
    if (kind == 0) { // numero bien formado
        if (rnd(3) == 0) s += rnd(2) ? '-' : '+';
        const int ni = (int)rnd(rnd(4) ? 8 : 30); for (int i = 0; i < ni; i++) s += (char)('0' + rnd(10));
        if (rnd(2)) { s += '.'; const int nf = (int)rnd(rnd(4) ? 8 : 30); for (int i = 0; i < nf; i++) s += (char)('0' + rnd(10)); }
        if (rnd(2)) { s += rnd(2) ? 'e' : 'E'; if (rnd(2)) s += rnd(2) ? '-' : '+'; const int ne = 1 + (int)rnd(rnd(8) ? 3 : 6); for (int i = 0; i < ne; i++) s += (char)('0' + rnd(10)); }
    } else if (kind == 1) { // numeros de aspecto "limpio"
        const char* t[] = {"1", "0", "0.5", "123.456", "1e10", "1e-10", "3.14159265358979323846", "0.1", "1e308", "1e309", "1e-320", "1e-400", "4.9e-324", "2.2250738585072014e-308", "1.7976931348623157e308", "1.7976931348623159e308",
            "16777216", "16777217", "9007199254740993", "340282350000000000000000000000000000000", "340282356779733661637539395458142568448", "1e39", "1e-45", "1.4e-45", "7e-46", "0.000001", "99999999999999999999999999", "5e-324", "2.4703282292062327e-324", "2.4703282292062328e-324"};
        s = t[rnd(sizeof t / sizeof t[0])]; if (rnd(4) == 0) s = "-" + s;
    } else { // cadenas aleatorias
        const int n = (int)rnd(24); for (int i = 0; i < n; i++) s += alpha[rnd(sizeof alpha - 1)];
    }
    if (rnd(5) == 0) s += "tail";
    if (rnd(8) == 0) s = std::string(rnd(3), ' ') + s;
    return s;
}

static void run_floatin(uint64_t n, uint64_t& fails, uint64_t& total) {
    typedef int(__cdecl* StrFn)(Ld12*, const char**, const char*, int, int, int, int);
    typedef int(__cdecl* AtodFn)(double*, const char*); typedef int(__cdecl* AtofFn)(float*, const char*);
    typedef void(__cdecl* FassFn)(int, void*, const char*);
    typedef int(__cdecl* CvtFn)(const Ld12*, uint32_t*, const FloatFormat*);
    typedef void(__cdecl* MtFn)(const char*, unsigned, Ld12*);
    typedef void(__cdecl* ShFn)(uint32_t*, int); typedef int(__cdecl* RdFn)(uint32_t*, int); typedef uint32_t(__cdecl* TzFn)(const uint32_t*, int); typedef uint32_t(__cdecl* AoFn)(uint32_t*, int);
    for (uint64_t c = 0; c < n; c++) {
        total++;
        const std::string txt = rand_numtext();
        // _strgtold12
        {
            const int mf = rnd(2), sc = rnd(4) ? 0 : (int)rnd(10) - 5, ad = rnd(4) ? 0 : (int)rnd(10) - 5, md = rnd(5) ? 0 : 1;
            Ld12 a, b; memset(&a, 0xCC, 12); memset(&b, 0xCC, 12); const char *ea = nullptr, *eb = nullptr;
            const int ra = ((StrFn)0x8346E2)(&a, &ea, txt.c_str(), mf, sc, ad, md), rb = Strgtold12(&b, &eb, txt.c_str(), mf, sc, ad, md);
            if (ra != rb || ea != eb || memcmp(&a, &b, 12)) { if (fails++ < 10) printf("DIFERENCIA strgtold12 '%s' mf=%d sc=%d ad=%d md=%d ret %d/%d fin %d/%d\n  exe=%08x%08x%08x\n  own=%08x%08x%08x\n", txt.c_str(), mf, sc, ad, md, ra, rb, (int)(ea - txt.c_str()), (int)(eb - txt.c_str()), a.d[2], a.d[1], a.d[0], b.d[2], b.d[1], b.d[0]); }
        }
        // _atodbl / _atoflt / _fassign
        {
            double da = 1, db = 1; float fa = 1, fb = 1;
            const int r1 = ((AtodFn)0x832C21)(&da, txt.c_str()), r2 = Atodbl(&db, txt.c_str());
            if (r1 != r2 || memcmp(&da, &db, 8)) { if (fails++ < 10) printf("DIFERENCIA atodbl '%s' %016llx/%016llx ret %d/%d\n", txt.c_str(), (unsigned long long)dbits(da), (unsigned long long)dbits(db), r1, r2); }
            const int r3 = ((AtofFn)0x832CA8)(&fa, txt.c_str()), r4 = Atoflt(&fb, txt.c_str());
            if (r3 != r4 || memcmp(&fa, &fb, 4)) { if (fails++ < 10) printf("DIFERENCIA atoflt '%s' %08x/%08x ret %d/%d\n", txt.c_str(), *(uint32_t*)&fa, *(uint32_t*)&fb, r3, r4); }
            uint8_t xa[16], xb[16]; memset(xa, 0x5A, 16); memset(xb, 0x5A, 16); const int isl = rnd(2);
            ((FassFn)0x826B40)(isl, xa, txt.c_str()); Fassign(isl, xb, txt.c_str());
            if (memcmp(xa, xb, 16)) { if (fails++ < 10) printf("DIFERENCIA fassign '%s' long=%d\n", txt.c_str(), isl); }
        }
        // _mtold12 con digitos aleatorios
        {
            char dg[0x20]; const unsigned cnt = rnd(rnd(4) ? 20 : 25); for (unsigned i = 0; i < cnt; i++) dg[i] = (char)rnd(10); if (cnt && rnd(3) == 0) dg[0] = 0;
            bool allz = true; for (unsigned i = 0; i < cnt; i++) if (dg[i]) allz = false;
            if (!allz) { Ld12 a, b; memset(&a, 0xCC, 12); memset(&b, 0xCC, 12); ((MtFn)0x835E89)(dg, cnt, &a); Mtold12(dg, cnt, &b);
                if (memcmp(&a, &b, 12)) { if (fails++ < 10) printf("DIFERENCIA mtold12 cnt=%u\n", cnt); } }
        }
        // _ld12cvt con ld12 aleatorios (double y float) y los ayudantes
        {
            Ld12 x; x.d[0] = (uint32_t)rng(); x.d[1] = (uint32_t)rng(); x.d[2] = (uint32_t)rng(); uint16_t* w = (uint16_t*)&x;
            const uint32_t k = rnd(6);
            if (k == 0) w[5] = (uint16_t)((rnd(2) ? 0x8000 : 0) | (0x3FFF - 1030 + rnd(2070)));
            else if (k == 1) w[5] = (uint16_t)((rnd(2) ? 0x8000 : 0) | (0x3FFF - 140 + rnd(290)));
            else if (k == 2) w[5] = (uint16_t)((rnd(2) ? 0x8000 : 0) | rnd(0x8000));
            else if (k == 3) { w[5] = (uint16_t)((rnd(2) ? 0x8000 : 0) | (0x3FFF + rnd(5) - 2)); }
            else if (k == 4) { w[5] = (uint16_t)(0x3FFF - 1022 - 60 + rnd(70)); w[4] |= 0x8000; if (rnd(2)) { x.d[1] &= 0xFFFF; x.d[0] &= 0xFFFF0000u; } }
            else { w[5] = (uint16_t)(0x3FFF + 1023 - 2 + rnd(5)); w[4] |= 0x8000; w[0] = rnd(2) ? 0xFFFF : (uint16_t)rng(); x.d[1] = 0xFFFFFFFFu; }
            if (rnd(4) == 0) { w[0] = 0x8000; }
            if (rnd(6) == 0) { x.d[0] = 0; x.d[1] = 0; w[4] = 0; if (rnd(2)) w[5] &= 0x8000; }
            for (int f = 0; f < 2; f++) {
                uint32_t oa[2] = {0xAAAAAAAA, 0xBBBBBBBB}, ob[2] = {0xAAAAAAAA, 0xBBBBBBBB};
                const FloatFormat* fm = (const FloatFormat*)(f ? 0x8E39D0 : 0x8E39B8);
                const int ra = ((CvtFn)0x832A21)(&x, oa, fm), rb = Ld12Cvt(&x, ob, fm);
                if (ra != rb || oa[0] != ob[0] || oa[1] != ob[1]) { if (fails++ < 10) printf("DIFERENCIA ld12cvt f=%d ld=%08x%08x%08x ret %d/%d out %08x%08x / %08x%08x\n", f, x.d[2], x.d[1], x.d[0], ra, rb, oa[1], oa[0], ob[1], ob[0]); }
            }
            uint32_t ha[3] = {(uint32_t)rng(), (uint32_t)rng(), (uint32_t)rng()}; uint32_t hb[3] = {ha[0], ha[1], ha[2]}; const int bit = 1 + (int)rnd(70);
            if (rnd(2)) { ha[1] &= 0xFFFF0000u; ha[2] = 0; hb[1] = ha[1]; hb[2] = 0; }
            if (((TzFn)0x832875)(ha, bit) != Ld12TailZero(hb, bit)) { if (fails++ < 10) printf("DIFERENCIA tailzero bit=%d\n", bit); }
            { uint32_t a1[3] = {ha[0], ha[1], ha[2]}, b1[3] = {ha[0], ha[1], ha[2]}; const uint32_t r1 = ((AoFn)0x8328A7)(a1, bit), r2 = Ld12AddOne(b1, bit);
              if (r1 != r2 || memcmp(a1, b1, 12)) { if (fails++ < 10) printf("DIFERENCIA addone bit=%d\n", bit); } }
            { uint32_t a1[3] = {ha[0], ha[1], ha[2]}, b1[3] = {ha[0], ha[1], ha[2]}; const int r1 = ((RdFn)0x8328F4)(a1, bit), r2 = Ld12Round(b1, bit);
              if (r1 != r2 || memcmp(a1, b1, 12)) { if (fails++ < 10) printf("DIFERENCIA round bit=%d\n", bit); } }
            { uint32_t a1[3] = {ha[0], ha[1], ha[2]}, b1[3] = {ha[0], ha[1], ha[2]}; ((ShFn)0x8329A6)(a1, bit); Ld12ShiftRight(b1, bit);
              if (memcmp(a1, b1, 12)) { if (fails++ < 10) printf("DIFERENCIA shiftright bits=%d\n", bit); } }
        }
    }
}
