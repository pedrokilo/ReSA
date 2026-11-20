# libjpeg 6b (Independent JPEG Group, 27-mar-1998): la version que lleva embebida gta_sa.exe. Fuente oficial SIN cambios (ver README, jpegsrc.v6b.tar.gz,
# SHA256 75c3ec241e9996504fe02a9ed4d12f16b74ade713972f3db9e65ce95cd27e35d); solo se compila como biblioteca estatica. Lote LIB3A.
# Se usa unicamente con la opcion GTASA_WITH_LIBJPEG (por defecto apagada). Cada .c entra por un envoltorio wrap/w_<archivo>.c que lo incluye entero y exporta las
# direcciones de sus funciones `static` (jpeg6b_export.h), porque los callbacks del EXE son METHODDEF (static) y se enlazan por puntero.
cmake_minimum_required(VERSION 3.20)
set_directory_properties(PROPERTIES COMPILE_OPTIONS "")
if (POLICY CMP0184)
    cmake_policy(SET CMP0184 NEW)
endif()
add_library(libjpeg6b STATIC
    wrap/w_jcapimin.c
    wrap/w_jcapistd.c
    wrap/w_jccoefct.c
    wrap/w_jccolor.c
    wrap/w_jcdctmgr.c
    wrap/w_jchuff.c
    wrap/w_jcinit.c
    wrap/w_jcmainct.c
    wrap/w_jcmarker.c
    wrap/w_jcmaster.c
    wrap/w_jcomapi.c
    wrap/w_jcparam.c
    wrap/w_jcphuff.c
    wrap/w_jcprepct.c
    wrap/w_jcsample.c
    wrap/w_jctrans.c
    wrap/w_jdapimin.c
    wrap/w_jdapistd.c
    wrap/w_jdatadst.c
    wrap/w_jdatasrc.c
    wrap/w_jdcoefct.c
    wrap/w_jdcolor.c
    wrap/w_jddctmgr.c
    wrap/w_jdhuff.c
    wrap/w_jdinput.c
    wrap/w_jdmainct.c
    wrap/w_jdmarker.c
    wrap/w_jdmaster.c
    wrap/w_jdmerge.c
    wrap/w_jdphuff.c
    wrap/w_jdpostct.c
    wrap/w_jdsample.c
    wrap/w_jdtrans.c
    wrap/w_jerror.c
    wrap/w_jfdctflt.c
    wrap/w_jfdctfst.c
    wrap/w_jfdctint.c
    wrap/w_jidctflt.c
    wrap/w_jidctfst.c
    wrap/w_jidctint.c
    wrap/w_jidctred.c
    wrap/w_jmemmgr.c
    wrap/w_jmemnobs.c
    wrap/w_jquant1.c
    wrap/w_jquant2.c
    wrap/w_jutils.c
    jpeg6b_exports.c)
target_include_directories(libjpeg6b PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(libjpeg6b PROPERTIES C_STANDARD 90 C_EXTENSIONS ON)
if (MSVC)
    set_target_properties(libjpeg6b PROPERTIES MSVC_RUNTIME_CHECKS "" MSVC_DEBUG_INFORMATION_FORMAT Embedded)
    # /arch:IA32: sin SSE/SSE2 (el EXE, MSVC 7.1, solo usa x87: la DCT/idct en coma flotante y el redondeo dependen de ello). Optimizada tambien en Debug (el EXE lo esta).
    # La precision del FPU (24 bits de mantisa) la fija el juego en el proceso, no la biblioteca.
    target_compile_options(libjpeg6b PRIVATE /W0 /Gd /arch:IA32 /O2 /Ob2 /Oy- /GS- /Zp8 /utf-8
        /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_WARNINGS)
endif()
