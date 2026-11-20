# libpng 1.0.12 (8-jun-2001): la version que lleva embebida gta_sa.exe (rtpng de RenderWare).
# Fuente oficial sin cambios; se compila como biblioteca estatica contra zlib 1.1.3.
# Lote LIB1A. Se usa unicamente con la opcion GTASA_WITH_ZLIB_LIBPNG (por defecto apagada).
cmake_minimum_required(VERSION 3.20)
# El proyecto anade /ZI (Editar y continuar) y /bigobj de forma global; aqui no se heredan: /ZI no admite /O2.
set_directory_properties(PROPERTIES COMPILE_OPTIONS "")
if (POLICY CMP0184)
    cmake_policy(SET CMP0184 NEW)
endif()
add_library(libpng1012 STATIC
    png.c pngerror.c pngget.c pngmem.c pngpread.c pngread.c pngrio.c pngrtran.c pngrutil.c
    pngset.c pngtrans.c pngwio.c pngwrite.c pngwtran.c pngwutil.c)
target_include_directories(libpng1012 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(libpng1012 PUBLIC zlib113)
# El EXE compila libpng con PNG_USER_MEM_SUPPORTED (png_create_read_struct_2, png_struct con mem_ptr/malloc_fn/free_fn) , SIN lectura progresiva y SIN stdio (png_struct de 0x1F4 bytes y png_info de 0x120, como en el EXE; ver reversing/FnLIB2A/README.md): LIB2A.
target_compile_definitions(libpng1012 PUBLIC PNG_USER_MEM_SUPPORTED PNG_NO_PROGRESSIVE_READ PNG_NO_STDIO)
set_target_properties(libpng1012 PROPERTIES C_STANDARD 90 C_EXTENSIONS ON)
if (MSVC)
    set_target_properties(libpng1012 PROPERTIES MSVC_RUNTIME_CHECKS "" MSVC_DEBUG_INFORMATION_FORMAT Embedded)
    target_compile_options(libpng1012 PRIVATE /W0 /Gd /arch:IA32 /O2 /Ob2 /Oy- /GS- /Zp8 /utf-8
        /D_CRT_SECURE_NO_WARNINGS /D_CRT_NONSTDC_NO_WARNINGS)
endif()
