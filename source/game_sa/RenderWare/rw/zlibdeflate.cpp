// Lote LIB4A: compresion deflate de zlib 1.1.3 del EXE (deflate.c, trees.c) y adler32/crc32, sustituida por la biblioteca oficial (libs/zlib-1.1.3).
// Archivo GENERADO por reversing/FnLIB4A/gen_hooks.py desde reversing/FnLIB4A/confirmacion.csv; no editar a mano. Ver reversing/FnLIB4A/README.md.
// Con GTASA_WITH_ZLIB_LIBPNG cada cuerpo reenvia a la biblioteca oficial (libs/); sin la opcion no se engancha nada y el EXE original sigue ejecutando su codigo.
#include "StdInc.h"

#include "zlibdeflate.h"

#ifdef GTASA_WITH_ZLIB_LIBPNG
extern "C" {
    int32 deflateInit_(void* strm, int32 level, const char* version, int32 stream_size);
    int32 deflateInit2_(void* strm, int32 level, int32 method, int32 windowBits, int32 memLevel, int32 strategy, const char* version, int32 stream_size);
    int32 deflateSetDictionary(void* strm, const uint8* dictionary, uint32 dictLength);
    int32 deflateReset(void* strm);
    int32 deflateParams(void* strm, int32 level, int32 strategy);
    int32 deflate(void* strm, int32 flush);
    int32 deflateEnd(void* strm);
    int32 deflateCopy(void* dest, void* source);
    int32 deflate_stored(void* s, int32 flush);
    void fill_window(void* s);
    int32 deflate_fast(void* s, int32 flush);
    uint32 longest_match(void* s, uint32 cur_match);
    int32 deflate_slow(void* s, int32 flush);
    uint32 crc32(uint32 crc, const uint8* buf, uint32 len);
    void _tr_init(void* s);
    void _tr_stored_block(void* s, void* buf, uint32 stored_len, int32 eof);
    void _tr_align(void* s);
    void _tr_flush_block(void* s, void* buf, uint32 stored_len, int32 eof);
    void build_tree(void* s, void* desc);
    void pqdownheap(void* s, void* tree, int32 k);
    void scan_tree(void* s, void* tree, int32 max_code);
    void send_tree(void* s, void* tree, int32 max_code);
    void compress_block(void* s, void* ltree, void* dtree);
    void bi_windup(void* s);
    uint32 adler32(uint32 adler, const uint8* buf, uint32 len);
}

namespace {
int32 __cdecl DeflateInit(void* strm, int32 level, const char* version, int32 stream_size) { return ::deflateInit_(strm, level, version, stream_size); } // 0x007DF480 (37 bytes)
int32 __cdecl DeflateInit2(void* strm, int32 level, int32 method, int32 windowBits, int32 memLevel, int32 strategy, const char* version, int32 stream_size) { return ::deflateInit2_(strm, level, method, windowBits, memLevel, strategy, version, stream_size); } // 0x007DF4B0 (753 bytes)
int32 __cdecl DeflateSetDictionary(void* strm, const uint8* dictionary, uint32 dictLength) { return ::deflateSetDictionary(strm, dictionary, dictLength); } // 0x007DF7B0 (247 bytes)
int32 __cdecl DeflateReset(void* strm) { return ::deflateReset(strm); } // 0x007DF8B0 (267 bytes)
int32 __cdecl DeflateParams(void* strm, int32 level, int32 strategy) { return ::deflateParams(strm, level, strategy); } // 0x007DF9C0 (202 bytes)
int32 __cdecl Deflate(void* strm, int32 flush) { return ::deflate(strm, flush); } // 0x007DFA90 (1121 bytes)
int32 __cdecl DeflateEnd(void* strm) { return ::deflateEnd(strm); } // 0x007DFF00 (170 bytes)
int32 __cdecl DeflateCopy(void* dest, void* source) { return ::deflateCopy(dest, source); } // 0x007DFFB0 (431 bytes)
int32 __cdecl DeflateStored(void* s, int32 flush) { return ::deflate_stored(s, flush); } // 0x007E0160 (651 bytes)
void __cdecl FillWindow(void* s) { ::fill_window(s); } // 0x007E03F0 (414 bytes)
int32 __cdecl DeflateFast(void* s, int32 flush) { return ::deflate_fast(s, flush); } // 0x007E0590 (1041 bytes)
uint32 __cdecl LongestMatch(void* s, uint32 cur_match) { return ::longest_match(s, cur_match); } // 0x007E09B0 (367 bytes)
int32 __cdecl DeflateSlow(void* s, int32 flush) { return ::deflate_slow(s, flush); } // 0x007E0B20 (1378 bytes)
uint32 __cdecl Crc32(uint32 crc, const uint8* buf, uint32 len) { return ::crc32(crc, buf, len); } // 0x007E10A0 (309 bytes)
void __cdecl TrInit(void* s) { ::_tr_init(s); } // 0x007E77F0 (181 bytes)
void __cdecl TrStoredBlock(void* s, void* buf, uint32 stored_len, int32 eof) { ::_tr_stored_block(s, buf, stored_len, eof); } // 0x007E78B0 (346 bytes)
void __cdecl TrAlign(void* s) { ::_tr_align(s); } // 0x007E7A10 (740 bytes)
void __cdecl TrFlushBlock(void* s, void* buf, uint32 stored_len, int32 eof) { ::_tr_flush_block(s, buf, stored_len, eof); } // 0x007E7D00 (1372 bytes)
void __cdecl BuildTree(void* s, void* desc) { ::build_tree(s, desc); } // 0x007E8260 (1124 bytes)
void __cdecl Pqdownheap(void* s, void* tree, int32 k) { ::pqdownheap(s, tree, k); } // 0x007E86D0 (209 bytes)
void __cdecl ScanTree(void* s, void* tree, int32 max_code) { ::scan_tree(s, tree, max_code); } // 0x007E87B0 (231 bytes)
void __cdecl SendTree(void* s, void* tree, int32 max_code) { ::send_tree(s, tree, max_code); } // 0x007E88A0 (1303 bytes)
void __cdecl CompressBlock(void* s, void* ltree, void* dtree) { ::compress_block(s, ltree, dtree); } // 0x007E8E70 (1037 bytes)
void __cdecl BiWindup(void* s) { ::bi_windup(s); } // 0x007E9280 (121 bytes)
uint32 __cdecl Adler32(uint32 adler, const uint8* buf, uint32 len) { return ::adler32(adler, buf, len); } // 0x007E9300 (302 bytes)
}

void ZlibDeflateInjectHooks() {
    RH_ScopedNamespaceName("ZlibDeflate");
    RH_ScopedCategory("Plugins");
    RH_ScopedGlobalInstall(DeflateInit, 0x7DF480);
    RH_ScopedGlobalInstall(DeflateInit2, 0x7DF4B0);
    RH_ScopedGlobalInstall(DeflateSetDictionary, 0x7DF7B0);
    RH_ScopedGlobalInstall(DeflateReset, 0x7DF8B0);
    RH_ScopedGlobalInstall(DeflateParams, 0x7DF9C0);
    RH_ScopedGlobalInstall(Deflate, 0x7DFA90);
    RH_ScopedGlobalInstall(DeflateEnd, 0x7DFF00);
    RH_ScopedGlobalInstall(DeflateCopy, 0x7DFFB0);
    RH_ScopedGlobalInstall(DeflateStored, 0x7E0160);
    RH_ScopedGlobalInstall(FillWindow, 0x7E03F0);
    RH_ScopedGlobalInstall(DeflateFast, 0x7E0590);
    RH_ScopedGlobalInstall(LongestMatch, 0x7E09B0);
    RH_ScopedGlobalInstall(DeflateSlow, 0x7E0B20);
    RH_ScopedGlobalInstall(Crc32, 0x7E10A0);
    RH_ScopedGlobalInstall(TrInit, 0x7E77F0);
    RH_ScopedGlobalInstall(TrStoredBlock, 0x7E78B0);
    RH_ScopedGlobalInstall(TrAlign, 0x7E7A10);
    RH_ScopedGlobalInstall(TrFlushBlock, 0x7E7D00);
    RH_ScopedGlobalInstall(BuildTree, 0x7E8260);
    RH_ScopedGlobalInstall(Pqdownheap, 0x7E86D0);
    RH_ScopedGlobalInstall(ScanTree, 0x7E87B0);
    RH_ScopedGlobalInstall(SendTree, 0x7E88A0);
    RH_ScopedGlobalInstall(CompressBlock, 0x7E8E70);
    RH_ScopedGlobalInstall(BiWindup, 0x7E9280);
    RH_ScopedGlobalInstall(Adler32, 0x7E9300);
}
#else
void ZlibDeflateInjectHooks() {}
#endif
