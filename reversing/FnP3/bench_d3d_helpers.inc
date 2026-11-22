// Ayudantes usercall del EXE por plantillas de codigo en memoria ejecutable (sin __asm): se cargan EDI/ESI/EAX y se llama. Se incluye desde bench_d3d.cpp tras D3D9RasterDriver.cpp.
//   0x4CB7C0 (pila: raster; EDI = extension)   0x4CB9C0 (pila: raster; ESI = extension)   0x4CC5C0 (EAX = banderas; EDI = raster)   0x4CCD70 (ESI = extension; EDI = raster)
static void* make_thunk(const std::vector<uint8_t>& pre, uintptr_t target, const std::vector<uint8_t>& post) {
    uint8_t* mem = (uint8_t*)VirtualAlloc(nullptr, 64, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    std::vector<uint8_t> code = pre;
    const size_t at = code.size();
    code.push_back(0xE8);
    for (int i = 0; i < 4; i++) code.push_back(0);
    code.insert(code.end(), post.begin(), post.end());
    memcpy(mem, code.data(), code.size());
    const int32_t rel = (int32_t)(target - ((uintptr_t)mem + at + 5));
    memcpy(mem + at + 1, &rel, 4);
    return mem;
}
// Los cuatro reciben (raster, extension, banderas) por cdecl.
static void* thunk4CB7C0() { return make_thunk({0x57, 0x8B, 0x7C, 0x24, 0x0C, 0x8B, 0x44, 0x24, 0x08, 0x50}, 0x4CB7C0, {0x83, 0xC4, 0x04, 0x5F, 0xC3}); } // push edi; mov edi,[esp+12]; mov eax,[esp+8]; push eax; call; add esp,4; pop edi; ret
static void* thunk4CB9C0() { return make_thunk({0x56, 0x8B, 0x74, 0x24, 0x0C, 0x8B, 0x44, 0x24, 0x08, 0x50}, 0x4CB9C0, {0x83, 0xC4, 0x04, 0x5E, 0xC3}); } // igual con ESI
static void* thunk4CC5C0() { return make_thunk({0x57, 0x8B, 0x7C, 0x24, 0x08, 0x8B, 0x44, 0x24, 0x10}, 0x4CC5C0, {0x5F, 0xC3}); }                         // push edi; mov edi,[esp+8]; mov eax,[esp+16]; call; pop edi; ret
static void* thunk4CCD70() { return make_thunk({0x56, 0x57, 0x8B, 0x7C, 0x24, 0x0C, 0x8B, 0x74, 0x24, 0x10}, 0x4CCD70, {0x5F, 0x5E, 0xC3}); }             // push esi; push edi; mov edi,[esp+12]; mov esi,[esp+16]; call; pop edi; pop esi; ret

static int run_helpers(uint64_t n, uint64_t seed) {
    typedef int32_t(__cdecl* HFn)(RwRaster*, uint8_t*, uint32_t);
    HFn exeFn[4] = {(HFn)thunk4CB7C0(), (HFn)thunk4CB9C0(), (HFn)thunk4CC5C0(), (HFn)thunk4CCD70()};
    const char* names[4] = {"0x4CB7C0 textura", "0x4CB9C0 destino de render", "0x4CC5C0 formato", "0x4CCD70 profundidad"};
    uint64_t fails[4] = {}, counts[4] = {}, okRet[4] = {};
    std::map<uint32_t, uint64_t> coverage;
    for (uint64_t it = 0; it < n; it++) {
        const int k = (int)(it % 4);
        const uint64_t cs = seed * 7777777 + it;
        Case c = setup(cs);
        uint8_t* ext = g_raster + 0x34;
        RwRaster* r = (RwRaster*)g_raster;
        const uint32_t variant = (uint32_t)(std::mt19937_64(cs ^ 0x5555)());
        auto prep = [&] {
            ext[9] = (variant & 1) ? (uint8_t)(1 + (variant >> 8) % 15) : 0;
            ext[0xA] = (variant & 2) ? (uint8_t)((variant >> 12) & 0xFF) : 0;
            ext[4] = ext[5] = ext[6] = ext[7] = 0;
            r->cType = (uint8_t)(c.flags & 7);
            r->cFormat = (uint8_t)((variant >> 16) & 0xFF);
            if (variant & 4) r->cFormat &= 0x9F;
            if (k == 0) {
                if (variant & 8) r->cFormat = (uint8_t)(((variant >> 20) & 1) ? 0x25 : 0x26) | (uint8_t)(((variant >> 21) & 1) ? 0x80 : 0); // paletas 0x500 / 0x600
                r->cType = (uint8_t)((variant & 16) ? 4 : 0);
            }
            if (k == 1) r->cType = 5;
        };
        prep();
        const int32_t a = exeFn[k](r, ext, c.flags);
        const std::vector<uint32_t> oa = observe(a);
        setup(cs);
        prep();
        int32_t b = 0;
        if (k == 0) b = RasterCreation::RasterCreateTexture(r, ext);
        else if (k == 1) b = RasterCreation::RasterCreateRenderTargetTexture(r, ext);
        else if (k == 2) b = RasterCreation::RasterCheckFormat(r, c.flags);
        else b = RasterCreation::RasterCreateDepthSurface(r, ext);
        const std::vector<uint32_t> ob = observe(b);
        counts[k]++;
        if (a) okRet[k]++;
        for (uint32_t tag : {0xD23u, 0xD25u, 0xD29u, 0xD05u, 0xE1u, 0xE3u, 0xE4u}) {
            if (std::find(g_log.begin(), g_log.end(), tag) != g_log.end()) coverage[tag + 0x1000u * k]++;
        }
        if (oa != ob) {
            if (fails[k]++ < 4) {
                printf("DIFERENCIA ayudante %s caso=%llu flags=%05x: exe ret=%d ours ret=%d\n", names[k], (unsigned long long)cs, c.flags, a, b);
                for (size_t i = 0; i < std::min(oa.size(), ob.size()); i++) {
                    if (oa[i] != ob[i]) { printf("  primer valor distinto en [%zu]: exe=%08x ours=%08x\n", i, oa[i], ob[i]); break; }
                }
            }
        }
    }
    uint64_t total = 0;
    for (int k = 0; k < 4; k++) {
        printf("ayudante %s: %llu casos (devuelven != 0: %llu), %llu diferencias\n", names[k], (unsigned long long)counts[k], (unsigned long long)okRet[k], (unsigned long long)fails[k]);
        total += fails[k];
    }
    printf("  cobertura (ayudante * 0x1000 + marca; marcas 0xD23 CreateTexture, 0xD25 cubo, 0xD29 profundidad, 0xD05 Evict, 0xE1 malloc, 0xE3 realloc, 0xE4 FreeListAlloc):");
    for (auto& kv : coverage) printf(" %x=%llu", kv.first, (unsigned long long)kv.second);
    printf("\n");
    return (int)total;
}
