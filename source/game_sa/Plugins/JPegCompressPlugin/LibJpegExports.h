#pragma once

#include "LibJpegCompress.h"

// Valor de 32 bits de la direccion de una funcion C++ (los punteros a funcion de las estructuras de libjpeg del EXE ocupan un dword).
template<typename Fn>
inline uint32 LibJpegFnAddress(Fn function) {
    return static_cast<uint32>(reinterpret_cast<uintptr_t>(function));
}

/**
 * Entradas publicas de la libjpeg 6b reconstruida (todas con cinfo opaco) para que los archivos de este modulo se llamen entre si
 * sin pasar por las direcciones del EXE. Cada una es el cuerpo C++ de la funcion cuya direccion se cita; se definen al final del
 * archivo que contiene la implementacion (estas implementaciones viven en espacios de nombres anonimos).
 */
void LibJpegExportJinitInputController(void* cinfo);                  // jinit_input_controller (0x5CA840): LibJpegDecompress.cpp
void LibJpegExportJinitMarkerReader(void* cinfo);                     // jinit_marker_reader (0x5CC7D0): LibJpegDecompress.cpp
void LibJpegExportJinitMasterDecompress(void* cinfo);                 // jinit_master_decompress (0x5CCCC0): LibJpegDecompress.cpp
void LibJpegExportJinitPostController(void* cinfo, int32 needFull);   // jinit_d_post_controller (0x5CCDF0): LibJpegDecompressUpsample.cpp
void LibJpegExportJinitUpsampler(void* cinfo);                        // jinit_upsampler (0x5CD350): LibJpegDecompressUpsample.cpp
void LibJpegExportJinitMemoryMgr(void* cinfo);                        // jinit_memory_mgr (0x5CF6D0): LibJpegMemory.cpp
int32 LibJpegExportJdivRoundUp(int32 a, int32 b);                     // jdiv_round_up (0x5CF890): LibJpegMemory.cpp
int32 LibJpegExportJroundUp(int32 a, int32 b);                        // jround_up (0x5CF8A0): LibJpegMemory.cpp
void LibJpegExportJpegSuppressTables(void* cinfo, int32 suppress);    // jpeg_suppress_tables (0x5CFA50): LibJpegCompressApi.cpp
void LibJpegExportJinitCPrepController(void* cinfo, uint32 needFull); // jinit_c_prep_controller (0x5C7920): JPegCompress.cpp
void LibJpegExportJinitDownsampler(void* cinfo);                      // jinit_downsampler (0x5C7DF0): JPegCompress.cpp
void LibJpegExportJinitDCoefController(void* cinfo, uint32 needFull); // jinit_d_coef_controller (0x5C8E20): JPegCompress.cpp
void LibJpegExportJinitColorDeconverter(void* cinfo);                 // jinit_color_deconverter (0x5C9350): JPegCompress.cpp
void LibJpegExportJinitInverseDct(void* cinfo);                       // jinit_inverse_dct (0x5C9710): JPegCompress.cpp
