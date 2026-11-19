# zlib 1.1.3 (1995-1998, Jean-loup Gailly y Mark Adler): la version que lleva embebida gta_sa.exe.
# Fuente oficial sin cambios (ver libs/zlib-1.1.3/README); solo se compila como biblioteca estatica.
# Lote LIB1A. Se usa unicamente con la opcion GTASA_WITH_ZLIB_LIBPNG (por defecto apagada).
cmake_minimum_required(VERSION 3.20)
# El proyecto anade /ZI (Editar y continuar) y /bigobj de forma global; aqui no se heredan: /ZI no admite /O2.
set_directory_properties(PROPERTIES COMPILE_OPTIONS "")
if (POLICY CMP0184)
    cmake_policy(SET CMP0184 NEW)
endif()
add_library(zlib113 STATIC
    adler32.c compress.c crc32.c deflate.c gzio.c infblock.c infcodes.c inffast.c
    inflate.c inftrees.c infutil.c trees.c uncompr.c zutil.c)
# huft_build es `local` (static) en inftrees.c y el EXE la tiene como funcion propia (0x7EAAB0): para poder sustituirla se compila SOLO inftrees.c con -Dlocal=
# (sin tocar la fuente); las tablas cplens/cplext/cpdist/cpdext pasan a ser simbolos externos, sin colision.
set_source_files_properties(inftrees.c PROPERTIES COMPILE_DEFINITIONS "local=")
target_include_directories(zlib113 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(zlib113 PROPERTIES C_STANDARD 90 C_EXTENSIONS ON)
if (MSVC)
    set_target_properties(zlib113 PROPERTIES MSVC_RUNTIME_CHECKS "" MSVC_DEBUG_INFORMATION_FORMAT Embedded)
    # Mismo runtime que el proyecto (/MT en Release, /MTd en Debug: CMAKE_MSVC_RUNTIME_LIBRARY global).
    # /arch:IA32: sin SSE/SSE2 (el EXE original, MSVC 7.1, solo usa x87 e IA-32 base). /Gd: __cdecl.
    # Debug del proyecto usa /Od: para comparar con el EXE (optimizado) se fuerza /O2 tambien en Debug.
    target_compile_options(zlib113 PRIVATE /W0 /Gd /arch:IA32 /O2 /Ob2 /Oy- /GS- /Zp8 /utf-8
        /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_WARNINGS)
endif()
