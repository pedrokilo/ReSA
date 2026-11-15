#pragma once

/**
 * Compresor de la libjpeg 6b EMBEBIDA en el EXE (0x5C55D0..0x5C6BD0: jcmarker.c, jcmaster.c, jcapimin.c y jcomapi.c). El listado las atribuia a
 * FxEmitterBP_c; no pertenecen a ninguna clase del juego. Lote 231A (sin validar).
 *
 * No es la libjpeg-turbo de la DLL (la que usa JPegCompress.cpp): la disposicion de jpeg_compress_struct del EXE es la de la 6b, asi que aqui
 * se declaran sus campos con los desplazamientos reales del EXE (comprobados con static_assert).
 */

struct ExeJpegCompress;

// jpeg_error_mgr (6b): error_exit(0), emit_message(4), output_message(8), format_message(0xC), reset_error_mgr(0x10), msg_code(0x14), msg_parm(0x18).
struct ExeJpegErrorMgr {
    void(__cdecl* error_exit)(ExeJpegCompress*);
    void(__cdecl* emit_message)(ExeJpegCompress*, int32 level);
    void* output_message;
    void* format_message;
    void* reset_error_mgr;
    int32 msg_code;
    int32 msg_parm_i[8];
};

// jpeg_memory_mgr (6b): alloc_small(0), alloc_large(4), alloc_sarray(8), alloc_barray(0xC), request_virt_sarray(0x10), request_virt_barray(0x14),
// realize_virt_arrays(0x18), access_virt_sarray(0x1C), access_virt_barray(0x20), free_pool(0x24), self_destruct(0x28).
struct ExeJpegMemoryMgr {
    void*(__cdecl* alloc_small)(ExeJpegCompress*, int32 pool, size_t size);
    void* alloc_large;
    void* alloc_sarray;
    void* alloc_barray;
    void* request_virt_sarray;
    void* request_virt_barray;
    void* realize_virt_arrays;
    void* access_virt_sarray;
    void* access_virt_barray;
    void(__cdecl* free_pool)(ExeJpegCompress*, int32 pool);
    void(__cdecl* self_destruct)(ExeJpegCompress*);
};

// jpeg_progress_mgr (6b).
struct ExeJpegProgressMgr {
    void* progress_monitor;
    int32 pass_counter;
    int32 pass_limit;
    int32 completed_passes;
    int32 total_passes;
};

// jpeg_destination_mgr (6b): el 0xC devuelve boolean (AL).
struct ExeJpegDestinationMgr {
    uint8* next_output_byte;
    size_t free_in_buffer;
    void* init_destination;
    uint8(__cdecl* empty_output_buffer)(ExeJpegCompress*);
    void* term_destination;
};

// jpeg_component_info (6b), 0x54 bytes.
struct ExeJpegComponentInfo {
    int32 component_id;         // +0x00
    int32 component_index;      // +0x04
    int32 h_samp_factor;        // +0x08
    int32 v_samp_factor;        // +0x0C
    int32 quant_tbl_no;         // +0x10
    int32 dc_tbl_no;            // +0x14
    int32 ac_tbl_no;            // +0x18
    uint32 width_in_blocks;     // +0x1C
    uint32 height_in_blocks;    // +0x20
    int32 DCT_scaled_size;      // +0x24
    uint32 downsampled_width;   // +0x28
    uint32 downsampled_height;  // +0x2C
    uint8 component_needed;     // +0x30 (boolean)
    uint8 pad31[3];
    int32 MCU_width;            // +0x34
    int32 MCU_height;           // +0x38
    int32 MCU_blocks;           // +0x3C
    int32 MCU_sample_width;     // +0x40
    int32 last_col_width;       // +0x44
    int32 last_row_height;      // +0x48
    void* quant_table;          // +0x4C
    void* dct_table;            // +0x50
};
static_assert(sizeof(ExeJpegComponentInfo) == 0x54 && offsetof(ExeJpegComponentInfo, component_needed) == 0x30 && offsetof(ExeJpegComponentInfo, last_row_height) == 0x48);

// JQUANT_TBL (6b): 64 words y sent_table (boolean, byte) en +0x80; 0x82 bytes.
struct ExeJpegQuantTable {
    uint16 quantval[64];
    uint8 sent_table;
    uint8 pad81;
};
static_assert(sizeof(ExeJpegQuantTable) == 0x82);

// JHUFF_TBL (6b): bits[17] (el 0 no se usa), huffval[256] en +0x11 y sent_table en +0x111; 0x112 bytes.
struct ExeJpegHuffTable {
    uint8 bits[17];
    uint8 huffval[256];
    uint8 sent_table;
};
static_assert(sizeof(ExeJpegHuffTable) == 0x112 && offsetof(ExeJpegHuffTable, sent_table) == 0x111);

// Control maestro de compresion (jpeg_comp_master + my_comp_master de jcmaster.c), 0x20 bytes reservados con alloc_small.
struct ExeJpegMaster {
    void(__cdecl* prepare_for_pass)(ExeJpegCompress*); // +0x00
    void(__cdecl* pass_startup)(ExeJpegCompress*);     // +0x04
    void(__cdecl* finish_pass)(ExeJpegCompress*);      // +0x08
    uint8 call_pass_startup;                           // +0x0C
    uint8 is_last_pass;                                // +0x0D
    uint8 pad0E[2];
    int32 pass_type;                                   // +0x10 (0 main_pass, 1 huff_opt_pass, 2 output_pass)
    int32 pass_number;                                 // +0x14
    int32 total_passes;                                // +0x18
    int32 scan_number;                                 // +0x1C
};
static_assert(sizeof(ExeJpegMaster) == 0x20);

// Escritor de marcadores (jpeg_marker_writer, jcmarker.c), 0x20 bytes: siete punteros a funcion y last_restart_interval.
struct ExeJpegMarkerWriter {
    void* write_file_header;     // +0x00
    void* write_frame_header;    // +0x04
    void* write_scan_header;     // +0x08
    void* write_file_trailer;    // +0x0C
    void* write_tables_only;     // +0x10
    void* write_marker_header;   // +0x14
    void* write_marker_byte;     // +0x18
    uint32 last_restart_interval; // +0x1C
};
static_assert(sizeof(ExeJpegMarkerWriter) == 0x20);

// Los submodulos que arrancan una pasada: start_pass es siempre el primer puntero de su tabla.
struct ExeJpegSubmodule {
    void* start_pass;
};

// jpeg_compress_struct (6b) hasta el campo `entropy`. Solo se accede por puntero (la reserva es del EXE).
struct ExeJpegCompress {
    ExeJpegErrorMgr* err;                 // +0x00
    ExeJpegMemoryMgr* mem;                // +0x04
    ExeJpegProgressMgr* progress;         // +0x08
    void* client_data;                    // +0x0C
    uint8 is_decompressor;                // +0x10 (boolean)
    uint8 pad11[3];
    int32 global_state;                   // +0x14
    ExeJpegDestinationMgr* dest;          // +0x18
    uint32 image_width;                   // +0x1C
    uint32 image_height;                  // +0x20
    int32 input_components;               // +0x24
    int32 in_color_space;                 // +0x28
    uint8 pad2C[4];
    double input_gamma;                   // +0x30
    int32 data_precision;                 // +0x38
    int32 num_components;                 // +0x3C
    int32 jpeg_color_space;               // +0x40
    ExeJpegComponentInfo* comp_info;      // +0x44
    ExeJpegQuantTable* quant_tbl_ptrs[4]; // +0x48
    ExeJpegHuffTable* dc_huff_tbl_ptrs[4]; // +0x58
    ExeJpegHuffTable* ac_huff_tbl_ptrs[4]; // +0x68
    uint8 arith_dc_L[16];                 // +0x78
    uint8 arith_dc_U[16];                 // +0x88
    uint8 arith_ac_K[16];                 // +0x98
    int32 num_scans;                      // +0xA8
    const void* scan_info;                // +0xAC
    uint8 raw_data_in;                    // +0xB0 (boolean)
    uint8 arith_code;                     // +0xB1
    uint8 optimize_coding;                // +0xB2
    uint8 CCIR601_sampling;               // +0xB3
    int32 smoothing_factor;               // +0xB4
    int32 dct_method;                     // +0xB8
    uint32 restart_interval;              // +0xBC
    int32 restart_in_rows;                // +0xC0
    uint8 write_JFIF_header;              // +0xC4
    uint8 JFIF_major_version;             // +0xC5
    uint8 JFIF_minor_version;             // +0xC6
    uint8 density_unit;                   // +0xC7
    uint16 X_density;                     // +0xC8
    uint16 Y_density;                     // +0xCA
    uint8 write_Adobe_marker;             // +0xCC
    uint8 padCD[3];
    uint32 next_scanline;                 // +0xD0
    uint8 progressive_mode;               // +0xD4
    uint8 padD5[3];
    int32 max_h_samp_factor;              // +0xD8
    int32 max_v_samp_factor;              // +0xDC
    uint32 total_iMCU_rows;               // +0xE0
    int32 comps_in_scan;                  // +0xE4
    ExeJpegComponentInfo* cur_comp_info[4]; // +0xE8
    uint32 MCUs_per_row;                  // +0xF8
    uint32 MCU_rows_in_scan;              // +0xFC
    int32 blocks_in_MCU;                  // +0x100
    int32 MCU_membership[10];             // +0x104
    int32 Ss;                             // +0x12C
    int32 Se;                             // +0x130
    int32 Ah;                             // +0x134
    int32 Al;                             // +0x138
    ExeJpegMaster* master;                // +0x13C
    ExeJpegSubmodule* main;               // +0x140
    ExeJpegSubmodule* prep;               // +0x144
    ExeJpegSubmodule* coef;               // +0x148
    ExeJpegMarkerWriter* marker;          // +0x14C
    ExeJpegSubmodule* cconvert;           // +0x150
    ExeJpegSubmodule* downsample;         // +0x154
    ExeJpegSubmodule* fdct;               // +0x158
    ExeJpegSubmodule* entropy;            // +0x15C
};
static_assert(offsetof(ExeJpegCompress, global_state) == 0x14 && offsetof(ExeJpegCompress, dest) == 0x18 && offsetof(ExeJpegCompress, image_height) == 0x20);
static_assert(offsetof(ExeJpegCompress, input_gamma) == 0x30 && offsetof(ExeJpegCompress, data_precision) == 0x38 && offsetof(ExeJpegCompress, comp_info) == 0x44);
static_assert(offsetof(ExeJpegCompress, quant_tbl_ptrs) == 0x48 && offsetof(ExeJpegCompress, ac_huff_tbl_ptrs) == 0x68 && offsetof(ExeJpegCompress, num_scans) == 0xA8);
static_assert(offsetof(ExeJpegCompress, raw_data_in) == 0xB0 && offsetof(ExeJpegCompress, restart_interval) == 0xBC && offsetof(ExeJpegCompress, write_JFIF_header) == 0xC4);
static_assert(offsetof(ExeJpegCompress, X_density) == 0xC8 && offsetof(ExeJpegCompress, write_Adobe_marker) == 0xCC && offsetof(ExeJpegCompress, progressive_mode) == 0xD4);
static_assert(offsetof(ExeJpegCompress, total_iMCU_rows) == 0xE0 && offsetof(ExeJpegCompress, cur_comp_info) == 0xE8 && offsetof(ExeJpegCompress, MCU_membership) == 0x104);
static_assert(offsetof(ExeJpegCompress, Ss) == 0x12C && offsetof(ExeJpegCompress, master) == 0x13C && offsetof(ExeJpegCompress, marker) == 0x14C && offsetof(ExeJpegCompress, entropy) == 0x15C);

class LibJpegCompress {
public:
    static void InjectHooks();

    // Marcadores (jcmarker.c). En el EXE EmitDqt..EmitAdobeApp14 son usercall (EAX/ECX); se enganchan con envolturas (ver LibJpegCompress.cpp).
    static int32 EmitDqt(ExeJpegCompress* cinfo, int32 index);              // 0x5C55D0, devuelve la precision de la tabla (0 u 1)
    static void EmitDht(ExeJpegCompress* cinfo, int32 index, bool isAc);    // 0x5C5780
    static void EmitDri(ExeJpegCompress* cinfo);                            // 0x5C5910
    static void EmitSof(ExeJpegCompress* cinfo, uint8 code);                // 0x5C5940
    static void EmitSos(ExeJpegCompress* cinfo);                            // 0x5C5AC0
    static void EmitJfifApp0(ExeJpegCompress* cinfo);                       // 0x5C5CA0
    static void EmitAdobeApp14(ExeJpegCompress* cinfo);                     // 0x5C5EC0
    static void WriteFileHeader(ExeJpegCompress* cinfo);                    // 0x5C60B0
    static void WriteFrameHeader(ExeJpegCompress* cinfo);                   // 0x5C6100
    static void WriteFileTrailer(ExeJpegCompress* cinfo);                   // 0x5C62D0
    static void JinitMarkerWriter(ExeJpegCompress* cinfo);                  // 0x5C63D0

    // Control maestro (jcmaster.c).
    static void PrepareForPass(ExeJpegCompress* cinfo);                     // 0x5C6850
    static void PassStartup(ExeJpegCompress* cinfo);                        // 0x5C69E0
    static void JinitCMasterControl(ExeJpegCompress* cinfo, bool transcodeOnly); // 0x5C6A90

    // API comun (jcapimin.c / jcomapi.c).
    static void JpegAbort(ExeJpegCompress* cinfo);                          // 0x5C6B50
    static void JpegDestroy(ExeJpegCompress* cinfo);                        // 0x5C6B90
    static ExeJpegQuantTable* JpegAllocQuantTable(ExeJpegCompress* cinfo);  // 0x5C6BB0
    static ExeJpegHuffTable* JpegAllocHuffTable(ExeJpegCompress* cinfo);    // 0x5C6BD0

private:
    // Ayudantes locales sin hook: en el EXE reciben cinfo en ESI y solo los llaman PrepareForPass (0x5C6850) y JinitCMasterControl (0x5C6A90), reconstruidas aqui.
    static void InitialSetup(ExeJpegCompress* cinfo);            // 0x5C6430
    static void SelectScanParameters(ExeJpegCompress* cinfo);    // 0x5C6600
    static void PerScanSetup(ExeJpegCompress* cinfo);            // 0x5C6680
};
