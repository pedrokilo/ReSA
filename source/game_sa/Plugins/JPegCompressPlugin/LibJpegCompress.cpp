#include "StdInc.h"

#include <intrin.h>

#include "LibJpegCompress.h"
#include "X87Math.h"

// Lote 231A (sin validar): reconstruccion en C++ puro de la parte de compresion de la libjpeg 6b embebida en el EXE (jcmarker.c, jcmaster.c, jcapimin.c y
// jcomapi.c, 0x5C55D0..0x5C6BD0). El listado las atribuia a FxEmitterBP_c; la clase real sale del desensamblado (todas manejan un jpeg_compress_struct: cinfo+0x14C
// = marker, +0x13C = master, error 0x18 = JERR_CANT_SUSPEND...). Revisado contra el desensamblado instruccion por instruccion; no hay banco. Sin x87 ni SEH.
//
// ABI del EXE (las funciones salieron de un compilador con optimizacion de programa completo):
//  - Las que reciben cinfo en pila (cdecl) se enganchan directamente: WriteFileHeader, WriteFrameHeader, WriteFileTrailer, JinitMarkerWriter, PrepareForPass,
//    PassStartup, JinitCMasterControl, JpegAbort, JpegDestroy, JpegAllocQuantTable y JpegAllocHuffTable.
//  - EmitDqt, EmitDri, EmitSos, EmitJfifApp0 y EmitAdobeApp14 reciben cinfo en EAX (EmitDqt ademas el indice en pila), EmitSof recibe cinfo en ECX y el marcador en
//    AL de EAX, y EmitDht cinfo en ECX, AL = es AC y el indice en pila (todas salen con `ret`, el llamador limpia la pila). Se enganchan con envolturas que leen EAX con
//    EntryEaxCpp (X87Math; tiene que ser lo primero que se ejecuta, sin comprobaciones de ejecucion ni cookie de /GS: lo vigila contrib/audit_register_residue.py, objeto
//    LibJpegCompress), ECX como primer parametro __fastcall y la pila con _AddressOfReturnAddress. Los llamadores del EXE que quedan (0x5C61E0, 0x5C62F0 y los `jmp` de
//    0x5C61E0) no leen ECX, EDX ni EAX tras la llamada.
//  - InitialSetup (0x5C6430), SelectScanParameters (0x5C6600) y PerScanSetup (0x5C6680) reciben cinfo en ESI, que C++ no puede leer, y solo los llaman PrepareForPass y
//    JinitCMasterControl (xrefs del EXE: 0x5C6AC5, 0x5C6884/0x5C68C4/0x5C6933 y 0x5C6889/0x5C68C9/0x5C6938): ayudantes locales sin hook.
//  - Los ayudantes de bajo nivel emit_byte (0x5C54B0), emit_marker (0x5C54F0) y emit_2bytes (0x5C5560) (ESI = cinfo, EBX = valor) se reproducen en linea (EmitByte,
//    EmitMarker, Emit2Bytes); no se enganchan.

namespace {
constexpr int32 JERR_CANT_SUSPEND = 0x18;

constexpr uintptr_t kJpegNaturalOrder = 0x86B818; // jpeg_natural_order: 64 int32 (el EXE lo pone a 0x86B918 como fin del bucle)

// ERREXIT / ERREXIT1 / ERREXIT2: msg_code y parametros se escriben antes de llamar a error_exit; el original sigue ejecutando si error_exit vuelve.
void ErrExit(ExeJpegCompress* cinfo, int32 code) {
    cinfo->err->msg_code = code;
    cinfo->err->error_exit(cinfo);
}

void ErrExit1(ExeJpegCompress* cinfo, int32 code, int32 p0) {
    cinfo->err->msg_code = code;
    cinfo->err->msg_parm_i[0] = p0;
    cinfo->err->error_exit(cinfo);
}

void ErrExit2(ExeJpegCompress* cinfo, int32 code, int32 p0, int32 p1) {
    cinfo->err->msg_code = code;
    cinfo->err->msg_parm_i[0] = p0;
    cinfo->err->msg_parm_i[1] = p1;
    cinfo->err->error_exit(cinfo);
}

// 0x5C54B0 (ESI = cinfo, un byte en pila): *next_output_byte++ = value; si free_in_buffer llega a 0 y empty_output_buffer devuelve 0 -> JERR_CANT_SUSPEND.
void EmitByte(ExeJpegCompress* cinfo, int32 value) {
    ExeJpegDestinationMgr* dest = cinfo->dest;
    *dest->next_output_byte = static_cast<uint8>(value);
    uint8* next = dest->next_output_byte + 1;
    size_t left = dest->free_in_buffer - 1;
    dest->next_output_byte = next;
    dest->free_in_buffer = left;
    if (left == 0) {
        if (!dest->empty_output_buffer(cinfo)) {
            ErrExit(cinfo, JERR_CANT_SUSPEND);
        }
    }
}

// 0x5C54F0 (ESI = cinfo, un byte en pila): 0xFF y el marcador.
void EmitMarker(ExeJpegCompress* cinfo, int32 mark) {
    EmitByte(cinfo, 0xFF);
    EmitByte(cinfo, mark);
}

// 0x5C5560 (ESI = cinfo, EBX = valor): byte alto (value >> 8, solo el byte bajo se escribe) y byte bajo.
void Emit2Bytes(ExeJpegCompress* cinfo, int32 value) {
    EmitByte(cinfo, value >> 8);
    EmitByte(cinfo, value);
}

// 0x5CF890 (cdecl, a y b en pila): (a + b - 1) / b con idiv con signo. No esta reconstruida: se llama por direccion fija.
int32 JdivRoundUp(int32 a, int32 b) {
    return plugin::CallAndReturn<int32, 0x5CF890, int32, int32>(a, b);
}
} // namespace

// Envolturas con los registros del EXE. /RTC1 anade __RTC_CheckEsp tras las llamadas y la cookie de /GS tambien: de ahi runtime_checks off y safebuffers (lo vigila
// contrib/audit_register_residue.py, objeto LibJpegCompress). EntryEaxCpp es lo primero que se ejecuta en cada una que lo usa.
namespace {
#pragma runtime_checks("", off)
// 0x5C55D0: EAX = cinfo, indice en pila, devuelve la precision en EAX.
__declspec(safebuffers) int32 __cdecl EmitDqtExe(int32 index) {
    auto* const cinfo = reinterpret_cast<ExeJpegCompress*>(EntryEaxCpp());
    return LibJpegCompress::EmitDqt(cinfo, index);
}

// 0x5C5780: ECX = cinfo (primer parametro __fastcall; EDX no se usa), AL = es AC, indice en la primera posicion de pila (no se declara como parametro: la funcion
// __fastcall la limpiaria con `ret 4`, y el EXE sale con `ret` y limpia el llamador).
__declspec(safebuffers) void __fastcall EmitDhtExe(ExeJpegCompress* cinfo, uint32 /*edx*/) {
    const bool isAc = static_cast<uint8>(EntryEaxCpp()) != 0;
    const int32 index = static_cast<const int32*>(_AddressOfReturnAddress())[1];
    LibJpegCompress::EmitDht(cinfo, index, isAc);
}

// 0x5C5910: EAX = cinfo.
__declspec(safebuffers) void __cdecl EmitDriExe() {
    LibJpegCompress::EmitDri(reinterpret_cast<ExeJpegCompress*>(EntryEaxCpp()));
}

// 0x5C5940: ECX = cinfo, AL = marcador.
__declspec(safebuffers) void __fastcall EmitSofExe(ExeJpegCompress* cinfo, uint32 /*edx*/) {
    const uint8 code = static_cast<uint8>(EntryEaxCpp());
    LibJpegCompress::EmitSof(cinfo, code);
}

// 0x5C5AC0: EAX = cinfo.
__declspec(safebuffers) void __cdecl EmitSosExe() {
    LibJpegCompress::EmitSos(reinterpret_cast<ExeJpegCompress*>(EntryEaxCpp()));
}

// 0x5C5CA0: EAX = cinfo.
__declspec(safebuffers) void __cdecl EmitJfifApp0Exe() {
    LibJpegCompress::EmitJfifApp0(reinterpret_cast<ExeJpegCompress*>(EntryEaxCpp()));
}

// 0x5C5EC0: EAX = cinfo.
__declspec(safebuffers) void __cdecl EmitAdobeApp14Exe() {
    LibJpegCompress::EmitAdobeApp14(reinterpret_cast<ExeJpegCompress*>(EntryEaxCpp()));
}
#pragma runtime_checks("", restore)
} // namespace

void LibJpegCompress::InjectHooks() {
    RH_ScopedClass(LibJpegCompress);
    RH_ScopedCategory("Plugins");

    RH_ScopedNamedGlobalInstall(EmitDqtExe, "EmitDqt", 0x5C55D0);           // lote 231A (sin validar)
    RH_ScopedNamedGlobalInstall(EmitDhtExe, "EmitDht", 0x5C5780);           // lote 231A (sin validar)
    RH_ScopedNamedGlobalInstall(EmitDriExe, "EmitDri", 0x5C5910);           // lote 231A (sin validar)
    RH_ScopedNamedGlobalInstall(EmitSofExe, "EmitSof", 0x5C5940);           // lote 231A (sin validar)
    RH_ScopedNamedGlobalInstall(EmitSosExe, "EmitSos", 0x5C5AC0);           // lote 231A (sin validar)
    RH_ScopedNamedGlobalInstall(EmitJfifApp0Exe, "EmitJfifApp0", 0x5C5CA0); // lote 231A (sin validar)
    RH_ScopedNamedGlobalInstall(EmitAdobeApp14Exe, "EmitAdobeApp14", 0x5C5EC0); // lote 231A (sin validar)
    RH_ScopedInstall(WriteFileHeader, 0x5C60B0);     // lote 231A (sin validar)
    RH_ScopedInstall(WriteFrameHeader, 0x5C6100);    // lote 231A (sin validar)
    RH_ScopedInstall(WriteFileTrailer, 0x5C62D0);    // lote 231A (sin validar)
    RH_ScopedInstall(JinitMarkerWriter, 0x5C63D0);   // lote 231A (sin validar)
    RH_ScopedInstall(PrepareForPass, 0x5C6850);      // lote 231A (sin validar)
    RH_ScopedInstall(PassStartup, 0x5C69E0);         // lote 231A (sin validar)
    RH_ScopedInstall(JinitCMasterControl, 0x5C6A90); // lote 231A (sin validar)
    RH_ScopedInstall(JpegAbort, 0x5C6B50);           // lote 231A (sin validar)
    RH_ScopedInstall(JpegDestroy, 0x5C6B90);         // lote 231A (sin validar)
    RH_ScopedInstall(JpegAllocQuantTable, 0x5C6BB0); // lote 231A (sin validar)
    RH_ScopedInstall(JpegAllocHuffTable, 0x5C6BD0);  // lote 231A (sin validar)
}

// 0x5C55D0 (lote 231A, sin validar; EAX = cinfo, indice en pila, `ret`, 419 bytes): emit_dqt. Devuelve la precision (0 u 1, EAX); sent_table = 1 al terminar.
int32 LibJpegCompress::EmitDqt(ExeJpegCompress* cinfo, int32 index) {
    ExeJpegQuantTable* qtbl = cinfo->quant_tbl_ptrs[index];
    if (!qtbl) {
        ErrExit1(cinfo, 0x34, index); // JERR_NO_QUANT_TABLE
    }

    int32 prec = 0;
    for (int32 i = 0; i < 64; i++) {
        if (qtbl->quantval[i] > 255) {
            prec = 1;
        }
    }

    if (!qtbl->sent_table) {
        EmitMarker(cinfo, 0xDB); // DQT
        Emit2Bytes(cinfo, prec ? 64 * 2 + 1 + 2 : 64 + 1 + 2); // neg/sbb/and 0x40 + 0x43: 0x83 o 0x43
        EmitByte(cinfo, index + (prec << 4));

        const auto* const naturalOrder = reinterpret_cast<const int32*>(kJpegNaturalOrder);
        for (int32 i = 0; i < 64; i++) {
            const uint32 qval = qtbl->quantval[naturalOrder[i]]; // movzx word
            if (prec) {
                EmitByte(cinfo, static_cast<int32>(qval >> 8));
            }
            EmitByte(cinfo, static_cast<int32>(qval));
        }

        qtbl->sent_table = 1;
    }
    return prec;
}

// 0x5C5780 (lote 231A, sin validar; ECX = cinfo, AL = es AC, indice en pila, `ret`, 373 bytes): emit_dht. Con AC el indice se desplaza +0x10 (tambien en el mensaje de error).
void LibJpegCompress::EmitDht(ExeJpegCompress* cinfo, int32 index, bool isAc) {
    ExeJpegHuffTable* htbl;
    if (isAc) {
        htbl = cinfo->ac_huff_tbl_ptrs[index];
        index += 0x10;
    } else {
        htbl = cinfo->dc_huff_tbl_ptrs[index];
    }
    if (!htbl) {
        ErrExit1(cinfo, 0x32, index); // JERR_NO_HUFF_TABLE
    }

    if (!htbl->sent_table) {
        EmitMarker(cinfo, 0xC4); // DHT

        int32 length = 0;
        for (int32 i = 1; i <= 16; i++) {
            length += htbl->bits[i];
        }

        Emit2Bytes(cinfo, length + 2 + 1 + 16);
        EmitByte(cinfo, index);

        for (int32 i = 1; i <= 16; i++) {
            EmitByte(cinfo, htbl->bits[i]);
        }
        for (int32 i = 0; i < length; i++) {
            EmitByte(cinfo, htbl->huffval[i]);
        }

        htbl->sent_table = 1;
    }
}

// 0x5C5910 (lote 231A, sin validar; EAX = cinfo, `ret`, 41 bytes): emit_dri. Sin llamadores en el EXE (el compilador lo dejo suelto).
void LibJpegCompress::EmitDri(ExeJpegCompress* cinfo) {
    EmitMarker(cinfo, 0xDD); // DRI
    Emit2Bytes(cinfo, 4);
    Emit2Bytes(cinfo, static_cast<int32>(cinfo->restart_interval));
}

// 0x5C5940 (lote 231A, sin validar; ECX = cinfo, AL = marcador, `ret`, 370 bytes): emit_sof. Solo la llaman por `jmp` WriteFrameHeader (0x5C61B0, 0x5C61C7, 0x5C61DB).
void LibJpegCompress::EmitSof(ExeJpegCompress* cinfo, uint8 code) {
    EmitMarker(cinfo, code);
    Emit2Bytes(cinfo, cinfo->num_components * 3 + 2 + 5 + 1);

    // Comparaciones con signo del EXE (jg/jle sobre las dimensiones sin signo).
    if (static_cast<int32>(cinfo->image_height) > 65535 || static_cast<int32>(cinfo->image_width) > 65535) {
        ErrExit1(cinfo, 0x29, 65535); // JERR_IMAGE_TOO_BIG
    }

    EmitByte(cinfo, cinfo->data_precision);
    Emit2Bytes(cinfo, static_cast<int32>(cinfo->image_height));
    Emit2Bytes(cinfo, static_cast<int32>(cinfo->image_width));
    EmitByte(cinfo, cinfo->num_components);

    const ExeJpegComponentInfo* comp = cinfo->comp_info;
    for (int32 ci = 0; ci < cinfo->num_components; ci++, comp++) {
        EmitByte(cinfo, comp->component_id);
        EmitByte(cinfo, static_cast<uint8>((static_cast<uint8>(comp->h_samp_factor) << 4) + static_cast<uint8>(comp->v_samp_factor)));
        EmitByte(cinfo, comp->quant_tbl_no);
    }
}

// 0x5C5AC0 (lote 231A, sin validar; EAX = cinfo, `ret`, 468 bytes): emit_sos. Solo la llama por `jmp` el write_scan_header del EXE (0x5C62C1, no incluido en el lote).
void LibJpegCompress::EmitSos(ExeJpegCompress* cinfo) {
    EmitMarker(cinfo, 0xDA); // SOS
    Emit2Bytes(cinfo, 2 * cinfo->comps_in_scan + 2 + 1 + 3);
    EmitByte(cinfo, cinfo->comps_in_scan);

    for (int32 i = 0; i < cinfo->comps_in_scan; i++) {
        const ExeJpegComponentInfo* comp = cinfo->cur_comp_info[i];
        EmitByte(cinfo, comp->component_id);

        int32 td = comp->dc_tbl_no;
        int32 ta = comp->ac_tbl_no;
        if (cinfo->progressive_mode) {
            // Modo progresivo: en un barrido solo se usan tablas DC o AC, y el refinamiento DC con Huffman no usa tabla.
            if (cinfo->Ss == 0) {
                ta = 0;
                if (cinfo->Ah != 0 && !cinfo->arith_code) {
                    td = 0;
                }
            } else {
                td = 0;
            }
        }
        EmitByte(cinfo, static_cast<uint8>((static_cast<uint8>(td) << 4) + static_cast<uint8>(ta)));
    }

    EmitByte(cinfo, cinfo->Ss);
    EmitByte(cinfo, cinfo->Se);
    EmitByte(cinfo, static_cast<uint8>((static_cast<uint8>(cinfo->Ah) << 4) + static_cast<uint8>(cinfo->Al)));
}

// 0x5C5CA0 (lote 231A, sin validar; EAX = cinfo, `ret`, 536 bytes): emit_jfif_app0.
void LibJpegCompress::EmitJfifApp0(ExeJpegCompress* cinfo) {
    EmitMarker(cinfo, 0xE0); // APP0
    Emit2Bytes(cinfo, 2 + 4 + 1 + 2 + 1 + 2 + 2 + 1 + 1);
    EmitByte(cinfo, 0x4A); // 'J'
    EmitByte(cinfo, 0x46); // 'F'
    EmitByte(cinfo, 0x49); // 'I'
    EmitByte(cinfo, 0x46); // 'F'
    EmitByte(cinfo, 0);
    EmitByte(cinfo, cinfo->JFIF_major_version);
    EmitByte(cinfo, cinfo->JFIF_minor_version);
    EmitByte(cinfo, cinfo->density_unit);
    Emit2Bytes(cinfo, cinfo->X_density); // movzx word
    Emit2Bytes(cinfo, cinfo->Y_density);
    EmitByte(cinfo, 0); // sin miniatura
    EmitByte(cinfo, 0);
}

// 0x5C5EC0 (lote 231A, sin validar; EAX = cinfo, `ret`, 362 bytes): emit_adobe_app14. Solo la llama por `jmp` WriteFileHeader (0x5C60EF).
void LibJpegCompress::EmitAdobeApp14(ExeJpegCompress* cinfo) {
    EmitMarker(cinfo, 0xEE); // APP14
    Emit2Bytes(cinfo, 2 + 5 + 2 + 2 + 2 + 1);
    EmitByte(cinfo, 0x41); // 'A'
    EmitByte(cinfo, 0x64); // 'd'
    EmitByte(cinfo, 0x6F); // 'o'
    EmitByte(cinfo, 0x62); // 'b'
    EmitByte(cinfo, 0x65); // 'e'
    Emit2Bytes(cinfo, 100); // version
    Emit2Bytes(cinfo, 0);   // flags0
    Emit2Bytes(cinfo, 0);   // flags1
    switch (cinfo->jpeg_color_space) {
    case 3: // JCS_YCbCr
        EmitByte(cinfo, 1);
        break;
    case 5: // JCS_YCCK
        EmitByte(cinfo, 2);
        break;
    default:
        EmitByte(cinfo, 0);
        break;
    }
}

// 0x5C60B0 (lote 231A, sin validar; cdecl, `ret`, 70 bytes): write_file_header.
void LibJpegCompress::WriteFileHeader(ExeJpegCompress* cinfo) {
    ExeJpegMarkerWriter* marker = cinfo->marker;
    EmitMarker(cinfo, 0xD8); // SOI
    marker->last_restart_interval = 0;

    if (cinfo->write_JFIF_header) {
        EmitJfifApp0(cinfo);
    }
    if (cinfo->write_Adobe_marker) {
        EmitAdobeApp14(cinfo);
    }
}

// 0x5C6100 (lote 231A, sin validar; cdecl, `ret`, 224 bytes): write_frame_header. Termina con `jmp` a EmitSof (ECX = cinfo, AL = marcador).
void LibJpegCompress::WriteFrameHeader(ExeJpegCompress* cinfo) {
    int32 prec = 0;
    for (int32 ci = 0; ci < cinfo->num_components; ci++) {
        prec += EmitDqt(cinfo, cinfo->comp_info[ci].quant_tbl_no);
    }

    bool isBaseline;
    if (!cinfo->arith_code && !cinfo->progressive_mode && cinfo->data_precision == 8) {
        isBaseline = true;
        for (int32 ci = 0; ci < cinfo->num_components; ci++) {
            const ExeJpegComponentInfo& comp = cinfo->comp_info[ci];
            if (comp.dc_tbl_no > 1 || comp.ac_tbl_no > 1) {
                isBaseline = false;
            }
        }
        if (prec && isBaseline) {
            isBaseline = false;
            cinfo->err->msg_code = 0x4B; // JTRC_16BIT_TABLES, TRACEMS(cinfo, 0, ...)
            cinfo->err->emit_message(cinfo, 0);
        }
    } else {
        isBaseline = false;
    }

    if (cinfo->arith_code) {
        EmitSof(cinfo, 0xC9);
    } else if (cinfo->progressive_mode) {
        EmitSof(cinfo, 0xC2);
    } else if (isBaseline) {
        EmitSof(cinfo, 0xC0);
    } else {
        EmitSof(cinfo, 0xC1);
    }
}

// 0x5C62D0 (lote 231A, sin validar; cdecl, `ret`, 20 bytes): write_file_trailer.
void LibJpegCompress::WriteFileTrailer(ExeJpegCompress* cinfo) {
    EmitMarker(cinfo, 0xD9); // EOI
}

// 0x5C63D0 (lote 231A, sin validar; cdecl, `ret`, 81 bytes): jinit_marker_writer. Los punteros que guarda son los del EXE (los enganchados saltan a los hooks; 0x5C61E0, 0x5C62F0,
// 0x5C6030 y 0x5C6070 siguen siendo codigo del EXE).
void LibJpegCompress::JinitMarkerWriter(ExeJpegCompress* cinfo) {
    auto* marker = static_cast<ExeJpegMarkerWriter*>(cinfo->mem->alloc_small(cinfo, 1, sizeof(ExeJpegMarkerWriter))); // JPOOL_IMAGE, 0x20
    cinfo->marker = marker;
    marker->write_file_header = reinterpret_cast<void*>(0x5C60B0);
    marker->write_frame_header = reinterpret_cast<void*>(0x5C6100);
    marker->write_scan_header = reinterpret_cast<void*>(0x5C61E0);
    marker->write_file_trailer = reinterpret_cast<void*>(0x5C62D0);
    marker->write_tables_only = reinterpret_cast<void*>(0x5C62F0);
    marker->write_marker_header = reinterpret_cast<void*>(0x5C6030);
    marker->write_marker_byte = reinterpret_cast<void*>(0x5C6070);
    marker->last_restart_interval = 0;
}

// 0x5C6430 (lote 231A, sin validar; ESI = cinfo, `ret`, 460 bytes): initial_setup. Ayudante local sin hook (solo lo llama JinitCMasterControl, 0x5C6AC5).
void LibJpegCompress::InitialSetup(ExeJpegCompress* cinfo) {
    if (cinfo->image_height == 0 || cinfo->image_width == 0 || cinfo->num_components <= 0 || cinfo->input_components <= 0) {
        ErrExit(cinfo, 0x20); // JERR_EMPTY_IMAGE
    }

    // Comparaciones con signo del EXE (jg/jle).
    if (static_cast<int32>(cinfo->image_height) > 0xFFDC || static_cast<int32>(cinfo->image_width) > 0xFFDC) {
        ErrExit1(cinfo, 0x29, 0xFFDC); // JERR_IMAGE_TOO_BIG
    }

    if (cinfo->data_precision != 8) {
        ErrExit1(cinfo, 0x0F, cinfo->data_precision); // JERR_BAD_PRECISION
    }

    if (cinfo->num_components > 4) {
        ErrExit2(cinfo, 0x1A, cinfo->num_components, 4); // JERR_COMPONENT_COUNT
    }

    cinfo->max_h_samp_factor = 1;
    cinfo->max_v_samp_factor = 1;
    {
        ExeJpegComponentInfo* comp = cinfo->comp_info;
        for (int32 ci = 0; ci < cinfo->num_components; ci++, comp++) {
            if (comp->h_samp_factor <= 0 || comp->h_samp_factor > 4 || comp->v_samp_factor <= 0 || comp->v_samp_factor > 4) {
                ErrExit(cinfo, 0x12); // JERR_BAD_SAMPLING
            }
            // max con signo: se conserva el actual si es mayor (jg), si no el del componente.
            cinfo->max_h_samp_factor = cinfo->max_h_samp_factor > comp->h_samp_factor ? cinfo->max_h_samp_factor : comp->h_samp_factor;
            cinfo->max_v_samp_factor = cinfo->max_v_samp_factor > comp->v_samp_factor ? cinfo->max_v_samp_factor : comp->v_samp_factor;
        }
    }

    {
        ExeJpegComponentInfo* comp = cinfo->comp_info;
        for (int32 ci = 0; ci < cinfo->num_components; ci++, comp++) {
            comp->component_index = ci;
            comp->DCT_scaled_size = 8;
            comp->width_in_blocks = static_cast<uint32>(JdivRoundUp(static_cast<int32>(comp->h_samp_factor * cinfo->image_width), cinfo->max_h_samp_factor * 8));
            comp->height_in_blocks = static_cast<uint32>(JdivRoundUp(static_cast<int32>(comp->v_samp_factor * cinfo->image_height), cinfo->max_v_samp_factor * 8));
            comp->downsampled_width = static_cast<uint32>(JdivRoundUp(static_cast<int32>(comp->h_samp_factor * cinfo->image_width), cinfo->max_h_samp_factor));
            comp->downsampled_height = static_cast<uint32>(JdivRoundUp(static_cast<int32>(comp->v_samp_factor * cinfo->image_height), cinfo->max_v_samp_factor));
            comp->component_needed = 1;
        }
    }

    cinfo->total_iMCU_rows = static_cast<uint32>(JdivRoundUp(static_cast<int32>(cinfo->image_height), cinfo->max_v_samp_factor * 8));
}

// 0x5C6600 (lote 231A, sin validar; ESI = cinfo, `ret`, 127 bytes): select_scan_parameters (un solo barrido con todos los componentes). Ayudante local sin hook.
void LibJpegCompress::SelectScanParameters(ExeJpegCompress* cinfo) {
    if (cinfo->num_components > 4) {
        ErrExit2(cinfo, 0x1A, cinfo->num_components, 4); // JERR_COMPONENT_COUNT
    }

    cinfo->comps_in_scan = cinfo->num_components;
    for (int32 ci = 0; ci < cinfo->num_components; ci++) {
        cinfo->cur_comp_info[ci] = &cinfo->comp_info[ci];
    }

    cinfo->Ss = 0;
    cinfo->Se = 63;
    cinfo->Ah = 0;
    cinfo->Al = 0;
}

// 0x5C6680 (lote 231A, sin validar; ESI = cinfo, `ret`, 453 bytes): per_scan_setup. Ayudante local sin hook.
void LibJpegCompress::PerScanSetup(ExeJpegCompress* cinfo) {
    if (cinfo->comps_in_scan == 1) {
        // Barrido no entrelazado.
        ExeJpegComponentInfo* comp = cinfo->cur_comp_info[0];
        cinfo->MCUs_per_row = comp->width_in_blocks;
        cinfo->MCU_rows_in_scan = comp->height_in_blocks;
        const uint32 vSamp = static_cast<uint32>(comp->v_samp_factor);
        const uint32 remainder = comp->height_in_blocks % vSamp; // div sin signo; con v_samp = 0 el EXE tambien falla
        comp->MCU_width = 1;
        comp->MCU_height = 1;
        comp->MCU_blocks = 1;
        comp->MCU_sample_width = 8;
        comp->last_col_width = 1;
        comp->last_row_height = static_cast<int32>(remainder == 0 ? vSamp : remainder);
        cinfo->blocks_in_MCU = 1;
        cinfo->MCU_membership[0] = 0;
    } else {
        // Barrido entrelazado.
        if (cinfo->comps_in_scan <= 0 || cinfo->comps_in_scan > 4) {
            ErrExit2(cinfo, 0x1A, cinfo->comps_in_scan, 4); // JERR_COMPONENT_COUNT
        }

        cinfo->MCUs_per_row = static_cast<uint32>(JdivRoundUp(static_cast<int32>(cinfo->image_width), cinfo->max_h_samp_factor * 8));
        cinfo->MCU_rows_in_scan = static_cast<uint32>(JdivRoundUp(static_cast<int32>(cinfo->image_height), cinfo->max_v_samp_factor * 8));
        cinfo->blocks_in_MCU = 0;

        for (int32 ci = 0; ci < cinfo->comps_in_scan; ci++) {
            ExeJpegComponentInfo* comp = cinfo->cur_comp_info[ci];
            const int32 hSamp = comp->h_samp_factor;
            const int32 vSamp = comp->v_samp_factor;
            comp->MCU_sample_width = hSamp * 8;
            const uint32 colRemainder = comp->width_in_blocks % static_cast<uint32>(hSamp);
            int32 mcuBlocks = vSamp * hSamp;
            comp->MCU_width = hSamp;
            comp->MCU_height = vSamp;
            comp->MCU_blocks = mcuBlocks;
            comp->last_col_width = static_cast<int32>(colRemainder == 0 ? static_cast<uint32>(hSamp) : colRemainder);
            const uint32 rowRemainder = comp->height_in_blocks % static_cast<uint32>(vSamp);
            comp->last_row_height = static_cast<int32>(rowRemainder == 0 ? static_cast<uint32>(vSamp) : rowRemainder);

            if (cinfo->blocks_in_MCU + mcuBlocks > 10) {
                ErrExit(cinfo, 0x0D); // JERR_BAD_MCU_SIZE
            }
            while (mcuBlocks > 0) {
                cinfo->MCU_membership[cinfo->blocks_in_MCU] = ci;
                cinfo->blocks_in_MCU++;
                mcuBlocks--;
            }
        }
    }

    if (cinfo->restart_in_rows > 0) {
        const int32 nominal = static_cast<int32>(cinfo->MCUs_per_row) * cinfo->restart_in_rows;
        cinfo->restart_interval = static_cast<uint32>(nominal < 0xFFFF ? nominal : 0xFFFF); // jl: con signo
    }
}

// 0x5C6850 (lote 231A, sin validar; cdecl, `ret`, 396 bytes): prepare_for_pass (jcmaster.c). pass_type: 0 main_pass, 1 huff_opt_pass, 2 output_pass.
void LibJpegCompress::PrepareForPass(ExeJpegCompress* cinfo) {
    ExeJpegMaster* master = cinfo->master;

    using StartPass1 = void(__cdecl*)(ExeJpegCompress*);
    using StartPass2 = void(__cdecl*)(ExeJpegCompress*, int32);
    const auto call1 = [cinfo](const ExeJpegSubmodule* module) { reinterpret_cast<StartPass1>(module->start_pass)(cinfo); };
    const auto call2 = [cinfo](const ExeJpegSubmodule* module, int32 mode) { reinterpret_cast<StartPass2>(module->start_pass)(cinfo, mode); };

    switch (master->pass_type) {
    case 0: // main_pass
        SelectScanParameters(cinfo);
        PerScanSetup(cinfo);
        if (!cinfo->raw_data_in) {
            call1(cinfo->cconvert);
            call1(cinfo->downsample);
            call2(cinfo->prep, 0); // JBUF_PASS_THRU
        }
        call1(cinfo->fdct);
        call2(cinfo->entropy, cinfo->optimize_coding);
        call2(cinfo->coef, master->total_passes > 1 ? 3 : 0); // JBUF_SAVE_AND_PASS / JBUF_PASS_THRU
        call2(cinfo->main, 0);                                // JBUF_PASS_THRU
        master->call_pass_startup = cinfo->optimize_coding == 0;
        break;
    case 1: // huff_opt_pass
        SelectScanParameters(cinfo);
        PerScanSetup(cinfo);
        if (cinfo->Ss != 0 || cinfo->Ah == 0 || cinfo->arith_code) {
            call2(cinfo->entropy, 1);
            call2(cinfo->coef, 2); // JBUF_CRANK_DEST
            master->call_pass_startup = false;
            break;
        }
        // Refinamiento DC con Huffman: no necesita tabla, se salta la pasada de optimizacion y se pasa a la de salida.
        master->pass_type = 2;
        master->pass_number++;
        [[fallthrough]];
    case 2: // output_pass
        if (!cinfo->optimize_coding) {
            SelectScanParameters(cinfo);
            PerScanSetup(cinfo);
        }
        call2(cinfo->entropy, 0);
        call2(cinfo->coef, 2); // JBUF_CONSUME_DATA
        if (master->scan_number == 0) {
            reinterpret_cast<void(__cdecl*)(ExeJpegCompress*)>(cinfo->marker->write_frame_header)(cinfo);
        }
        reinterpret_cast<void(__cdecl*)(ExeJpegCompress*)>(cinfo->marker->write_scan_header)(cinfo);
        master->call_pass_startup = false;
        break;
    default:
        ErrExit(cinfo, 0x30); // JERR_NOT_COMPILED
        break;
    }

    master->is_last_pass = master->pass_number == master->total_passes - 1;

    if (cinfo->progress) {
        cinfo->progress->completed_passes = master->pass_number;
        cinfo->progress->total_passes = master->total_passes;
    }
}

// 0x5C69E0 (lote 231A, sin validar; cdecl, `ret`, 40 bytes): pass_startup.
void LibJpegCompress::PassStartup(ExeJpegCompress* cinfo) {
    cinfo->master->call_pass_startup = false;
    reinterpret_cast<void(__cdecl*)(ExeJpegCompress*)>(cinfo->marker->write_frame_header)(cinfo);
    reinterpret_cast<void(__cdecl*)(ExeJpegCompress*)>(cinfo->marker->write_scan_header)(cinfo);
}

// 0x5C6A90 (lote 231A, sin validar; cdecl (cinfo, transcode_only: solo se lee el byte bajo), `ret`, 186 bytes): jinit_c_master_control. Los punteros del maestro son los del EXE
// (0x5C6850 y 0x5C69E0 enganchadas; 0x5C6A10, finish_pass_master, sigue siendo del EXE).
void LibJpegCompress::JinitCMasterControl(ExeJpegCompress* cinfo, bool transcodeOnly) {
    auto* master = static_cast<ExeJpegMaster*>(cinfo->mem->alloc_small(cinfo, 1, sizeof(ExeJpegMaster))); // JPOOL_IMAGE, 0x20
    cinfo->master = master;
    master->prepare_for_pass = reinterpret_cast<void(__cdecl*)(ExeJpegCompress*)>(0x5C6850);
    master->pass_startup = reinterpret_cast<void(__cdecl*)(ExeJpegCompress*)>(0x5C69E0);
    master->finish_pass = reinterpret_cast<void(__cdecl*)(ExeJpegCompress*)>(0x5C6A10);
    master->is_last_pass = 0;

    InitialSetup(cinfo);

    if (cinfo->scan_info != nullptr) {
        ErrExit(cinfo, 0x30); // JERR_NOT_COMPILED (sin soporte de guiones de barridos)
    } else {
        cinfo->progressive_mode = 0;
        cinfo->num_scans = 1;
    }

    if (cinfo->progressive_mode) {
        cinfo->optimize_coding = 1;
    }

    if (transcodeOnly) {
        master->pass_type = cinfo->optimize_coding == 0 ? 2 : 1; // output_pass o huff_opt_pass
    } else {
        master->pass_type = 0; // main_pass
    }

    master->scan_number = 0;
    master->pass_number = 0;
    if (cinfo->optimize_coding) {
        master->total_passes = cinfo->num_scans * 2;
    } else {
        master->total_passes = cinfo->num_scans;
    }
}

// 0x5C6B50 (lote 231A, sin validar; cdecl, `ret`, 56 bytes): jpeg_abort. Solo actua con `mem` (si es 0 no toca nada). En descompresion 0x10C es marker_list (jpeg_decompress_struct 6b).
void LibJpegCompress::JpegAbort(ExeJpegCompress* cinfo) {
    if (cinfo->mem) {
        cinfo->mem->free_pool(cinfo, 1); // JPOOL_IMAGE
        if (cinfo->is_decompressor) {
            cinfo->global_state = 200; // DSTATE_START
            *reinterpret_cast<void**>(reinterpret_cast<uint8*>(cinfo) + 0x10C) = nullptr;
        } else {
            cinfo->global_state = 100; // CSTATE_START
        }
    }
}

// 0x5C6B90 (lote 231A, sin validar; cdecl, `ret`, 31 bytes): jpeg_destroy.
void LibJpegCompress::JpegDestroy(ExeJpegCompress* cinfo) {
    if (cinfo->mem != nullptr) {
        cinfo->mem->self_destruct(cinfo);
    }
    cinfo->mem = nullptr;
    cinfo->global_state = 0;
}

// 0x5C6BB0 (lote 231A, sin validar; cdecl, `ret`, 28 bytes): jpeg_alloc_quant_table. Reserva en el pool permanente (0) y deja sent_table = 0; devuelve la tabla.
ExeJpegQuantTable* LibJpegCompress::JpegAllocQuantTable(ExeJpegCompress* cinfo) {
    auto* table = static_cast<ExeJpegQuantTable*>(cinfo->mem->alloc_small(cinfo, 0, sizeof(ExeJpegQuantTable)));
    table->sent_table = 0;
    return table;
}

// 0x5C6BD0 (lote 231A, sin validar; cdecl, `ret`, 28 bytes): jpeg_alloc_huff_table.
ExeJpegHuffTable* LibJpegCompress::JpegAllocHuffTable(ExeJpegCompress* cinfo) {
    auto* table = static_cast<ExeJpegHuffTable*>(cinfo->mem->alloc_small(cinfo, 0, sizeof(ExeJpegHuffTable)));
    table->sent_table = 0;
    return table;
}
