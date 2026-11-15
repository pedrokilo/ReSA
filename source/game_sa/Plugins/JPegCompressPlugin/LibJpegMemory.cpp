#include "StdInc.h"

#include "LibJpegMemory.h"
#include "LibJpegExe.h"

// Lote 235A (sin validar): jmemmgr.c, jmemnobs.c y jutils.c de la libjpeg 6b del EXE (zona 0x5CEBC0..0x5CF960).
// my_memory_mgr del EXE: +0x00 alloc_small, +0x04 alloc_large, +0x08 alloc_sarray, +0x0C alloc_barray, +0x10 request_virt_sarray,
// +0x14 request_virt_barray, +0x18 realize_virt_arrays, +0x1C access_virt_sarray, +0x20 access_virt_barray, +0x24 free_pool,
// +0x28 self_destruct, +0x2C max_memory_to_use, +0x30 max_alloc_chunk, +0x34 small_list[2], +0x3C large_list[2],
// +0x44 virt_sarray_list, +0x48 virt_barray_list, +0x4C total_space_allocated, +0x50 last_rowsperchunk (0x54 bytes).
// small_pool_hdr / large_pool_hdr: +0 next, +4 bytes_in_use, +8 bytes_left (cabecera de 0x10).
// jvirt_sarray_control / jvirt_barray_control (0x78 bytes): +0x00 mem_buffer, +0x04 rows_in_array, +0x08 samplesperrow,
// +0x0C maxaccess, +0x10 rows_in_mem, +0x14 rowsperchunk, +0x18 cur_start_row, +0x1C first_undef_row, +0x20 pre_zero,
// +0x21 dirty, +0x22 b_s_open, +0x24 next_virt, +0x28 b_s_info (+0x30 close_backing_store).
namespace {
using namespace LibJpegExe;

constexpr uint32 EXE_MAX_ALLOC_CHUNK = 1000000000;       // 0x3B9ACA00
constexpr uint32 EXE_MAX_ALLOC_CHUNK_SMALL = 0x3B9AC9F0; // MAX_ALLOC_CHUNK - sizeof(pool_hdr)
constexpr uint32 EXE_MIN_SLOP = 50;

// 0x5CF800 (16 B): jpeg_get_small(cinfo, sizeofobject) de jmemnobs. malloc del EXE por direccion (size, 0), cdecl.
void* ExeJpegGetSmall(void* cinfo, uint32 sizeofobject) {
    return reinterpret_cast<void*(__cdecl*)(uint32, int32)>(0x72F420)(sizeofobject, 0);
}

// 0x5CF810 (10 B): jpeg_free_small(cinfo, object, sizeofobject). free del EXE por direccion.
void ExeJpegFreeSmall(void* cinfo, void* object, uint32 sizeofobject) {
    reinterpret_cast<void(__cdecl*)(void*)>(0x72F430)(object);
}

// 0x5CF820 (16 B): jpeg_get_large(cinfo, sizeofobject): igual que get_small.
void* ExeJpegGetLarge(void* cinfo, uint32 sizeofobject) {
    return reinterpret_cast<void*(__cdecl*)(uint32, int32)>(0x72F420)(sizeofobject, 0);
}

// 0x5CF830 (10 B): jpeg_free_large(cinfo, object, sizeofobject).
void ExeJpegFreeLarge(void* cinfo, void* object, uint32 sizeofobject) {
    reinterpret_cast<void(__cdecl*)(void*)>(0x72F430)(object);
}

// 0x5CF840 (5 B): jpeg_mem_available(cinfo, min_bytes_needed, max_bytes_needed, already_allocated): devuelve max_bytes_needed.
int32 ExeJpegMemAvailable(void* cinfo, int32 minBytesNeeded, int32 maxBytesNeeded, int32 alreadyAllocated) {
    return maxBytesNeeded;
}

// 0x5CF850 (20 B): jpeg_open_backing_store(cinfo, info, total_bytes_needed): ERREXIT(JERR_NO_BACKING_STORE) y sigue.
void ExeJpegOpenBackingStore(void* cinfo, void* info, int32 totalBytesNeeded) {
    ErrExit(cinfo, JERR_NO_BACKING_STORE);
}

// 0x5CF870 (3 B): jpeg_mem_init(cinfo): devuelve 0.
int32 ExeJpegMemInit(void* cinfo) {
    return 0;
}

// 0x5CF880 (1 B): jpeg_mem_term(cinfo): no hace nada.
void ExeJpegMemTerm(void* cinfo) {
}

// 0x5CEBC0 (331 B): alloc_small(cinfo, pool_id, sizeofobject) de jmemmgr. Tamano > 0x3B9AC9F0 (sin signo) -> ERREXIT1(0x36, 1) y SIGUE; se redondea a multiplo de 8;
// pool_id fuera de 0..1 -> ERREXIT1(0x0E) y SIGUE. Busca en small_list[pool] la primera cabecera con bytes_left >= tamano; si no hay, pide un bloque nuevo
// (min_request = tamano + 16, holgura de la tabla 0x86B7F8 -first_pool_slop- si es la primera cabecera o de 0x86B800 -extra_pool_slop- si no, recortada a MAX_ALLOC_CHUNK - min_request)
// y la reduce a la mitad mientras jpeg_get_small falle (< 50 -> ERREXIT1(0x36, 2) y SIGUE).
void* ExeAllocSmall(void* cinfo, int32 poolId, uint32 sizeofobject) {
    uint8* const mem = At<uint8*>(cinfo, 0x04);
    if (sizeofobject > EXE_MAX_ALLOC_CHUNK_SMALL) {
        ErrExit1(cinfo, JERR_OUT_OF_MEMORY, 1);
    }
    const uint32 oddBytes = sizeofobject & 7;
    if (oddBytes > 0) {
        sizeofobject += 8 - oddBytes;
    }
    if (poolId < 0 || poolId >= 2) {
        ErrExit1(cinfo, JERR_BAD_POOL_ID, poolId);
    }
    uint8* prevHdr = nullptr;
    uint8* hdr = At<uint8*>(mem, 0x34 + 4 * static_cast<uint32>(poolId));
    while (hdr != nullptr) {
        if (At<uint32>(hdr, 0x08) >= sizeofobject) {
            break;
        }
        prevHdr = hdr;
        hdr = At<uint8*>(hdr, 0x00);
    }
    if (hdr == nullptr) {
        const uint32 minRequest = sizeofobject + 0x10;
        uint32 slop;
        if (prevHdr == nullptr) {
            slop = reinterpret_cast<const uint32*>(0x86B7F8)[poolId]; // first_pool_slop
        } else {
            slop = reinterpret_cast<const uint32*>(0x86B800)[poolId]; // extra_pool_slop
        }
        const uint32 maxSlop = EXE_MAX_ALLOC_CHUNK - minRequest;
        if (slop > maxSlop) {
            slop = maxSlop;
        }
        for (;;) {
            hdr = static_cast<uint8*>(ExeJpegGetSmall(cinfo, minRequest + slop));
            if (hdr != nullptr) {
                break;
            }
            slop >>= 1;
            if (slop < EXE_MIN_SLOP) {
                ErrExit1(cinfo, JERR_OUT_OF_MEMORY, 2);
            }
        }
        At<uint32>(mem, 0x4C) += minRequest + slop;
        At<uint32>(hdr, 0x00) = 0;
        At<uint32>(hdr, 0x04) = 0;
        At<uint32>(hdr, 0x08) = slop + sizeofobject;
        if (prevHdr == nullptr) {
            At<uint8*>(mem, 0x34 + 4 * static_cast<uint32>(poolId)) = hdr;
        } else {
            At<uint8*>(prevHdr, 0x00) = hdr;
        }
    }
    uint8* const dataPtr = hdr + 0x10 + At<uint32>(hdr, 0x04);
    At<uint32>(hdr, 0x04) += sizeofobject;
    At<uint32>(hdr, 0x08) -= sizeofobject;
    return dataPtr;
}

// 0x5CED10 (188 B): alloc_large(cinfo, pool_id, sizeofobject) de jmemmgr. Mismos controles de tamano (ERREXIT1(0x36, 3)) y de pool; pide size + 16 a jpeg_get_large
// (nulo -> ERREXIT1(0x36, 4) y SIGUE), suma al total, encadena la cabecera por delante de large_list[pool] (bytes_in_use = tamano redondeado, bytes_left = 0).
void* ExeAllocLarge(void* cinfo, int32 poolId, uint32 sizeofobject) {
    uint8* const mem = At<uint8*>(cinfo, 0x04);
    if (sizeofobject > EXE_MAX_ALLOC_CHUNK_SMALL) {
        ErrExit1(cinfo, JERR_OUT_OF_MEMORY, 3);
    }
    const uint32 oddBytes = sizeofobject & 7;
    if (oddBytes > 0) {
        sizeofobject += 8 - oddBytes;
    }
    if (poolId < 0 || poolId >= 2) {
        ErrExit1(cinfo, JERR_BAD_POOL_ID, poolId);
    }
    uint8* const hdr = static_cast<uint8*>(ExeJpegGetLarge(cinfo, sizeofobject + 0x10));
    if (hdr == nullptr) {
        ErrExit1(cinfo, JERR_OUT_OF_MEMORY, 4);
    }
    At<uint32>(mem, 0x4C) += sizeofobject + 0x10;
    uint8* const next = At<uint8*>(mem, 0x3C + 4 * static_cast<uint32>(poolId));
    At<uint32>(hdr, 0x04) = sizeofobject;
    At<uint8*>(hdr, 0x00) = next;
    At<uint32>(hdr, 0x08) = 0;
    At<uint8*>(mem, 0x3C + 4 * static_cast<uint32>(poolId)) = hdr;
    return hdr + 0x10;
}

// 0x5CEDD0 (167 B): alloc_sarray(cinfo, pool_id, samplesperrow, numrows) de jmemmgr. rowsperchunk = 0x3B9AC9F0 / samplesperrow (sin signo); <= 0 -> ERREXIT(0x46) y SIGUE;
// se recorta a numrows (con signo) y se guarda en mem->last_rowsperchunk (+0x50). Vector de punteros con alloc_small(numrows * 4) y bloques con alloc_large
// (rowsperchunk * samplesperrow, rowsperchunk recortado SIN signo a numrows - currow); si numrows es 0 no entra en el bucle.
void** ExeAllocSarray(void* cinfo, int32 poolId, uint32 samplesperrow, uint32 numrows) {
    uint8* const mem = At<uint8*>(cinfo, 0x04);
    int32 rowsperchunk = static_cast<int32>(EXE_MAX_ALLOC_CHUNK_SMALL / samplesperrow);
    if (rowsperchunk <= 0) {
        ErrExit(cinfo, JERR_WIDTH_OVERFLOW);
    }
    if (!(rowsperchunk < static_cast<int32>(numrows))) {
        rowsperchunk = static_cast<int32>(numrows);
    }
    At<int32>(mem, 0x50) = rowsperchunk;
    void** const result = static_cast<void**>(ExeAllocSmall(cinfo, poolId, numrows * 4));
    uint32 currow = 0;
    if (numrows != 0) {
        do {
            uint32 thisRows = static_cast<uint32>(rowsperchunk);
            if (!(thisRows < numrows - currow)) {
                thisRows = numrows - currow;
            }
            rowsperchunk = static_cast<int32>(thisRows);
            uint8* workspace = static_cast<uint8*>(ExeAllocLarge(cinfo, poolId, thisRows * samplesperrow));
            if (thisRows > 0) {
                uint32 count = thisRows;
                do {
                    result[currow] = workspace;
                    ++currow;
                    workspace += samplesperrow;
                    --count;
                } while (count != 0);
            }
        } while (currow < numrows);
    }
    return result;
}

// 0x5CEE80 (167 B): alloc_barray(cinfo, pool_id, blocksperrow, numrows): igual que alloc_sarray con filas de blocksperrow * 128 bytes (JBLOCK).
void** ExeAllocBarray(void* cinfo, int32 poolId, uint32 blocksperrow, uint32 numrows) {
    uint8* const mem = At<uint8*>(cinfo, 0x04);
    const uint32 rowBytes = blocksperrow << 7;
    int32 rowsperchunk = static_cast<int32>(EXE_MAX_ALLOC_CHUNK_SMALL / rowBytes);
    if (rowsperchunk <= 0) {
        ErrExit(cinfo, JERR_WIDTH_OVERFLOW);
    }
    if (!(rowsperchunk < static_cast<int32>(numrows))) {
        rowsperchunk = static_cast<int32>(numrows);
    }
    At<int32>(mem, 0x50) = rowsperchunk;
    void** const result = static_cast<void**>(ExeAllocSmall(cinfo, poolId, numrows * 4));
    uint32 currow = 0;
    if (numrows != 0) {
        do {
            uint32 thisRows = static_cast<uint32>(rowsperchunk);
            if (!(thisRows < numrows - currow)) {
                thisRows = numrows - currow;
            }
            rowsperchunk = static_cast<int32>(thisRows);
            uint8* workspace = static_cast<uint8*>(ExeAllocLarge(cinfo, poolId, (thisRows * blocksperrow) << 7));
            if (thisRows > 0) {
                uint32 count = thisRows;
                do {
                    result[currow] = workspace;
                    ++currow;
                    workspace += rowBytes;
                    --count;
                } while (count != 0);
            }
        } while (currow < numrows);
    }
    return result;
}

// 0x5CEF30 (104 B): request_virt_sarray(cinfo, pool_id, pre_zero BYTE, samplesperrow, numrows, maxaccess) de jmemmgr. pool_id != 1 -> ERREXIT1(0x0E) y SIGUE.
// alloc_small(0x78): mem_buffer = 0, rows_in_array = numrows, samplesperrow, maxaccess, pre_zero (+0x20), b_s_open = 0 (+0x22), next_virt = mem->virt_sarray_list; la lista pasa a ser el nuevo.
void* ExeRequestVirtSarray(void* cinfo, int32 poolId, int32 preZero, uint32 samplesperrow, uint32 numrows, uint32 maxaccess) {
    uint8* const mem = At<uint8*>(cinfo, 0x04);
    if (poolId != 1) {
        ErrExit1(cinfo, JERR_BAD_POOL_ID, poolId);
    }
    uint8* const result = static_cast<uint8*>(ExeAllocSmall(cinfo, poolId, 0x78));
    At<uint32>(result, 0x04) = numrows;
    At<uint32>(result, 0x08) = samplesperrow;
    At<uint32>(result, 0x00) = 0;
    At<uint32>(result, 0x0C) = maxaccess;
    At<uint8>(result, 0x20) = static_cast<uint8>(preZero);
    At<uint8>(result, 0x22) = 0;
    At<uint32>(result, 0x24) = At<uint32>(mem, 0x44);
    At<uint8*>(mem, 0x44) = result;
    return result;
}

// 0x5CEFA0 (104 B): request_virt_barray(...): igual, con virt_barray_list (+0x48).
void* ExeRequestVirtBarray(void* cinfo, int32 poolId, int32 preZero, uint32 blocksperrow, uint32 numrows, uint32 maxaccess) {
    uint8* const mem = At<uint8*>(cinfo, 0x04);
    if (poolId != 1) {
        ErrExit1(cinfo, JERR_BAD_POOL_ID, poolId);
    }
    uint8* const result = static_cast<uint8*>(ExeAllocSmall(cinfo, poolId, 0x78));
    At<uint32>(result, 0x04) = numrows;
    At<uint32>(result, 0x08) = blocksperrow;
    At<uint32>(result, 0x00) = 0;
    At<uint32>(result, 0x0C) = maxaccess;
    At<uint8>(result, 0x20) = static_cast<uint8>(preZero);
    At<uint8>(result, 0x22) = 0;
    At<uint32>(result, 0x24) = At<uint32>(mem, 0x48);
    At<uint8*>(mem, 0x48) = result;
    return result;
}

// 0x5CF010 (403 B): realize_virt_arrays(cinfo) de jmemmgr. Suma por cada matriz sin mem_buffer maxaccess * samplesperrow (space_per_minheight) y rows_in_array * samplesperrow
// (maximum_space); las de bloques con << 7. space_per_minheight <= 0 (con signo) -> vuelve sin mas. jpeg_mem_available devuelve maximum_space (jmemnobs), asi que
// max_minheights = 1000000000. Por cada matriz: minheights = (rows - 1) / maxaccess + 1 (sin signo) comparado con signo; si cabe, rows_in_mem = rows_in_array; si no,
// rows_in_mem = maxaccess * max_minheights y jpeg_open_backing_store (ERREXIT y sigue) con b_s_open = 1. Luego mem_buffer = alloc_sarray/alloc_barray(pool 1, samplesperrow, rows_in_mem),
// rowsperchunk = mem->last_rowsperchunk, cur_start_row = first_undef_row = 0, dirty = 0.
void ExeRealizeVirtArrays(void* cinfo) {
    uint8* const mem = At<uint8*>(cinfo, 0x04);
    uint32 spacePerMinheight = 0;
    uint32 maximumSpace = 0;
    for (uint8* sptr = At<uint8*>(mem, 0x44); sptr != nullptr; sptr = At<uint8*>(sptr, 0x24)) {
        if (At<uint32>(sptr, 0x00) == 0) {
            const uint32 samplesperrow = At<uint32>(sptr, 0x08);
            spacePerMinheight += At<uint32>(sptr, 0x0C) * samplesperrow;
            maximumSpace += At<uint32>(sptr, 0x04) * samplesperrow;
        }
    }
    for (uint8* bptr = At<uint8*>(mem, 0x48); bptr != nullptr; bptr = At<uint8*>(bptr, 0x24)) {
        if (At<uint32>(bptr, 0x00) == 0) {
            const uint32 blocksperrow = At<uint32>(bptr, 0x08);
            spacePerMinheight += (At<uint32>(bptr, 0x0C) * blocksperrow) << 7;
            maximumSpace += (At<uint32>(bptr, 0x04) * blocksperrow) << 7;
        }
    }
    if (static_cast<int32>(spacePerMinheight) <= 0) {
        return;
    }
    const int32 availMem = ExeJpegMemAvailable(cinfo, static_cast<int32>(spacePerMinheight), static_cast<int32>(maximumSpace), static_cast<int32>(At<uint32>(mem, 0x4C)));
    int32 maxMinheights;
    if (availMem >= static_cast<int32>(maximumSpace)) {
        maxMinheights = static_cast<int32>(EXE_MAX_ALLOC_CHUNK);
    } else {
        maxMinheights = availMem / static_cast<int32>(spacePerMinheight);
        if (maxMinheights <= 0) {
            maxMinheights = 1;
        }
    }
    for (uint8* sptr = At<uint8*>(mem, 0x44); sptr != nullptr; sptr = At<uint8*>(sptr, 0x24)) {
        if (At<uint32>(sptr, 0x00) != 0) {
            continue;
        }
        const uint32 rowsInArray = At<uint32>(sptr, 0x04);
        const uint32 maxaccess = At<uint32>(sptr, 0x0C);
        const int32 minheights = static_cast<int32>((rowsInArray - 1) / maxaccess + 1);
        if (minheights <= maxMinheights) {
            At<uint32>(sptr, 0x10) = rowsInArray;
        } else {
            const uint32 rowsInMem = maxaccess * static_cast<uint32>(maxMinheights);
            const uint32 totalBytes = At<uint32>(sptr, 0x08) * rowsInArray;
            At<uint32>(sptr, 0x10) = rowsInMem;
            ExeJpegOpenBackingStore(cinfo, sptr + 0x28, static_cast<int32>(totalBytes));
            At<uint8>(sptr, 0x22) = 1;
        }
        At<void**>(sptr, 0x00) = ExeAllocSarray(cinfo, 1, At<uint32>(sptr, 0x08), At<uint32>(sptr, 0x10));
        At<uint32>(sptr, 0x14) = At<uint32>(mem, 0x50);
        At<uint32>(sptr, 0x18) = 0;
        At<uint32>(sptr, 0x1C) = 0;
        At<uint8>(sptr, 0x21) = 0;
    }
    for (uint8* bptr = At<uint8*>(mem, 0x48); bptr != nullptr; bptr = At<uint8*>(bptr, 0x24)) {
        if (At<uint32>(bptr, 0x00) != 0) {
            continue;
        }
        const uint32 rowsInArray = At<uint32>(bptr, 0x04);
        const uint32 maxaccess = At<uint32>(bptr, 0x0C);
        const int32 minheights = static_cast<int32>((rowsInArray - 1) / maxaccess + 1);
        if (minheights <= maxMinheights) {
            At<uint32>(bptr, 0x10) = rowsInArray;
        } else {
            const uint32 rowsInMem = maxaccess * static_cast<uint32>(maxMinheights);
            const uint32 totalBytes = (At<uint32>(bptr, 0x08) * rowsInArray) << 7;
            At<uint32>(bptr, 0x10) = rowsInMem;
            ExeJpegOpenBackingStore(cinfo, bptr + 0x28, static_cast<int32>(totalBytes));
            At<uint8>(bptr, 0x22) = 1;
        }
        At<void**>(bptr, 0x00) = ExeAllocBarray(cinfo, 1, At<uint32>(bptr, 0x08), At<uint32>(bptr, 0x10));
        At<uint32>(bptr, 0x14) = At<uint32>(mem, 0x50);
        At<uint32>(bptr, 0x18) = 0;
        At<uint32>(bptr, 0x1C) = 0;
        At<uint8>(bptr, 0x21) = 0;
    }
}

// 0x5CF580 (263 B): free_pool(cinfo, pool_id) de jmemmgr. pool_id fuera de 0..1 -> ERREXIT1(0x0E) y SIGUE. Con pool 1: cierra el almacen de respaldo de cada sarray y
// de cada barray abierto (b_s_open = 0 ANTES de llamar a close_backing_store(cinfo, &b_s_info) en +0x30), con virt_sarray_list = 0 tras el primer bucle y virt_barray_list = 0
// al final. Despues libera large_list[pool] (puesta a 0 antes) con jpeg_free_large y small_list[pool] con jpeg_free_small; tamano liberado = bytes_left + bytes_in_use + 16,
// que se resta de total_space_allocated tras cada liberacion.
void ExeFreePool(void* cinfo, int32 poolId) {
    uint8* const mem = At<uint8*>(cinfo, 0x04);
    if (poolId < 0 || poolId >= 2) {
        ErrExit1(cinfo, JERR_BAD_POOL_ID, poolId);
    }
    if (poolId == 1) {
        for (uint8* sptr = At<uint8*>(mem, 0x44); sptr != nullptr; sptr = At<uint8*>(sptr, 0x24)) {
            if (At<uint8>(sptr, 0x22) != 0) {
                At<uint8>(sptr, 0x22) = 0;
                At<void(__cdecl*)(void*, void*)>(sptr, 0x30)(cinfo, sptr + 0x28);
            }
        }
        uint8* bptr = At<uint8*>(mem, 0x48);
        At<uint32>(mem, 0x44) = 0;
        for (; bptr != nullptr; bptr = At<uint8*>(bptr, 0x24)) {
            if (At<uint8>(bptr, 0x22) != 0) {
                At<uint8>(bptr, 0x22) = 0;
                At<void(__cdecl*)(void*, void*)>(bptr, 0x30)(cinfo, bptr + 0x28);
            }
        }
        At<uint32>(mem, 0x48) = 0;
    }
    uint8* lhdr = At<uint8*>(mem, 0x3C + 4 * static_cast<uint32>(poolId));
    At<uint32>(mem, 0x3C + 4 * static_cast<uint32>(poolId)) = 0;
    while (lhdr != nullptr) {
        const uint32 bytesLeft = At<uint32>(lhdr, 0x08);
        const uint32 bytesInUse = At<uint32>(lhdr, 0x04);
        uint8* const next = At<uint8*>(lhdr, 0x00);
        const uint32 spaceFreed = bytesLeft + bytesInUse + 0x10;
        ExeJpegFreeLarge(cinfo, lhdr, spaceFreed);
        At<uint32>(mem, 0x4C) -= spaceFreed;
        lhdr = next;
    }
    uint8* shdr = At<uint8*>(mem, 0x34 + 4 * static_cast<uint32>(poolId));
    At<uint32>(mem, 0x34 + 4 * static_cast<uint32>(poolId)) = 0;
    while (shdr != nullptr) {
        const uint32 bytesLeft = At<uint32>(shdr, 0x08);
        const uint32 bytesInUse = At<uint32>(shdr, 0x04);
        uint8* const next = At<uint8*>(shdr, 0x00);
        const uint32 spaceFreed = bytesLeft + bytesInUse + 0x10;
        ExeJpegFreeSmall(cinfo, shdr, spaceFreed);
        At<uint32>(mem, 0x4C) -= spaceFreed;
        shdr = next;
    }
}

// 0x5CF6D0 (297 B): jinit_memory_mgr(cinfo) de jmemmgr. cinfo->mem = 0; jpeg_mem_init; jpeg_get_small(0x54): nulo -> jpeg_mem_term, ERREXIT1(0x36, 0) y SIGUE (escribe sobre nulo,
// como el EXE). Rellena la tabla de metodos con las direcciones del EXE (alloc_small..free_pool, access_virt_sarray 0x5CF2F0, access_virt_barray 0x5CF430, self_destruct 0x5CF690),
// max_alloc_chunk = 1000000000, max_memory_to_use = jpeg_mem_init() (0), listas a 0, total_space_allocated = 0x54 y cinfo->mem = mem. Luego JPEGMEM: getenv (0x8240C7, cadena 0x86B810) y
// sscanf (0x8220AD, formato "%ld%c" en 0x86B808): max_memory_to_use = n * 1000 (o n * 1000000 si el sufijo es 'm' o 'M').
void ExeJinitMemoryMgr(void* cinfo) {
    At<uint32>(cinfo, 0x04) = 0;
    int32 maxToUse = ExeJpegMemInit(cinfo);
    uint8* const mem = static_cast<uint8*>(ExeJpegGetSmall(cinfo, 0x54));
    if (mem == nullptr) {
        ExeJpegMemTerm(cinfo);
        ErrExit1(cinfo, JERR_OUT_OF_MEMORY, 0);
    }
    At<uint32>(mem, 0x00) = 0x5CEBC0;
    At<uint32>(mem, 0x04) = 0x5CED10;
    At<uint32>(mem, 0x08) = 0x5CEDD0;
    At<uint32>(mem, 0x0C) = 0x5CEE80;
    At<uint32>(mem, 0x10) = 0x5CEF30;
    At<uint32>(mem, 0x14) = 0x5CEFA0;
    At<uint32>(mem, 0x18) = 0x5CF010;
    At<uint32>(mem, 0x1C) = 0x5CF2F0;
    At<uint32>(mem, 0x20) = 0x5CF430;
    At<uint32>(mem, 0x24) = 0x5CF580;
    At<uint32>(mem, 0x28) = 0x5CF690;
    At<uint32>(mem, 0x30) = EXE_MAX_ALLOC_CHUNK;
    At<int32>(mem, 0x2C) = maxToUse;
    // Dos vueltas del EXE: (0x38, 0x40) y (0x34, 0x3C).
    At<uint32>(mem, 0x38) = 0;
    At<uint32>(mem, 0x40) = 0;
    At<uint32>(mem, 0x34) = 0;
    At<uint32>(mem, 0x3C) = 0;
    At<uint32>(mem, 0x44) = 0;
    At<uint32>(mem, 0x48) = 0;
    At<uint32>(mem, 0x4C) = 0x54;
    At<uint8*>(cinfo, 0x04) = mem;

    const char* const env = reinterpret_cast<const char*(__cdecl*)(const char*)>(0x8240C7)(reinterpret_cast<const char*>(0x86B810)); // getenv("JPEGMEM")
    if (env != nullptr) {
        char ch = 'x';
        const int32 matched = reinterpret_cast<int32(__cdecl*)(const char*, const char*, ...)>(0x8220AD)(env, reinterpret_cast<const char*>(0x86B808), &maxToUse, &ch); // sscanf(env, "%ld%c", &max_to_use, &ch)
        if (matched > 0) {
            if (ch == 'm' || ch == 'M') {
                At<int32>(mem, 0x2C) = maxToUse * 1000 * 1000;
            } else {
                At<int32>(mem, 0x2C) = maxToUse * 1000;
            }
        }
    }
}

// 0x5CF890 (16 B): jdiv_round_up(a, b) de jutils: (a + b - 1) / b con signo.
int32 ExeJdivRoundUp(int32 a, int32 b) {
    return (a + b - 1) / b;
}

// 0x5CF8A0 (24 B): jround_up(a, b) de jutils: a += b - 1; a - a % b con signo.
int32 ExeJroundUp(int32 a, int32 b) {
    const int32 sum = a + b - 1;
    return sum - sum % b;
}

// 0x5CF8C0 (70 B): jcopy_sample_rows(input_array, source_row, output_array, dest_row, num_rows, num_cols) de jutils. Por fila copia num_cols bytes hacia delante (palabras de 4 y luego
// el resto de bytes, como rep movs). num_rows <= 0 (con signo) no hace nada.
void ExeJcopySampleRows(uint8** inputArray, int32 sourceRow, uint8** outputArray, int32 destRow, int32 numRows, uint32 numCols) {
    uint8** inptr = inputArray + sourceRow;
    uint8** outptr = outputArray + destRow;
    if (numRows <= 0) {
        return;
    }
    do {
        const uint8* src = *inptr;
        uint8* dst = *outptr;
        for (uint32 i = 0; i < (numCols >> 2); ++i) {
            *reinterpret_cast<uint32*>(dst) = *reinterpret_cast<const uint32*>(src);
            dst += 4;
            src += 4;
        }
        for (uint32 i = 0; i < (numCols & 3); ++i) {
            *dst++ = *src++;
        }
        ++inptr;
        ++outptr;
        --numRows;
    } while (numRows != 0);
}

// 0x5CF940 (27 B): jzero_far(target, bytestozero) de jutils: pone a cero palabras de 4 y luego el resto de bytes.
void ExeJzeroFar(void* target, uint32 bytesToZero) {
    uint8* dst = static_cast<uint8*>(target);
    for (uint32 i = 0; i < (bytesToZero >> 2); ++i) {
        *reinterpret_cast<uint32*>(dst) = 0;
        dst += 4;
    }
    for (uint32 i = 0; i < (bytesToZero & 3); ++i) {
        *dst++ = 0;
    }
}
} // namespace

/**
 * Registra el gestor de memoria de la libjpeg del EXE (lote 235A, sin validar).
 */
void LibJpegMemory::InjectHooks() {
    RH_ScopedClass(LibJpegMemory);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(ExeAllocSmall, "alloc_small", 0x5CEBC0);                       // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeAllocLarge, "alloc_large", 0x5CED10);                       // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeAllocSarray, "alloc_sarray", 0x5CEDD0);                     // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeAllocBarray, "alloc_barray", 0x5CEE80);                     // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeRequestVirtSarray, "request_virt_sarray", 0x5CEF30);        // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeRequestVirtBarray, "request_virt_barray", 0x5CEFA0);        // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeRealizeVirtArrays, "realize_virt_arrays", 0x5CF010);        // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeFreePool, "free_pool", 0x5CF580);                           // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJinitMemoryMgr, "jinit_memory_mgr", 0x5CF6D0);              // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegGetSmall, "jpeg_get_small", 0x5CF800);                  // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegFreeSmall, "jpeg_free_small", 0x5CF810);                // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegGetLarge, "jpeg_get_large", 0x5CF820);                  // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegFreeLarge, "jpeg_free_large", 0x5CF830);                // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegMemAvailable, "jpeg_mem_available", 0x5CF840);          // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegOpenBackingStore, "jpeg_open_backing_store", 0x5CF850); // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegMemInit, "jpeg_mem_init", 0x5CF870);                    // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJpegMemTerm, "jpeg_mem_term", 0x5CF880);                    // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJdivRoundUp, "jdiv_round_up", 0x5CF890);                    // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJroundUp, "jround_up", 0x5CF8A0);                           // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJcopySampleRows, "jcopy_sample_rows", 0x5CF8C0);            // lote 235A (sin validar)
    RH_ScopedNamedGlobalInstall(ExeJzeroFar, "jzero_far", 0x5CF940);                           // lote 235A (sin validar)
}
