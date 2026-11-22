#pragma once

// Arranque del CRT estatico del EXE (MSVC 7.1) reconstruido en C++, lotes F1 y E2-estado/monton de la parte P5 del plan de codigo vivo (docs/CodigoVivoExe/fase8-arranque.md).
// SOLO RECONSTRUIDAS, SIN GANCHO (como Ioinit/Initstdio/Ioterm): corren una sola vez antes de que la DLL exista (la DLL se carga cuando el EXE ya ejecuto `entry` 0x824570), asi que un
// gancho nunca se ejecutaria. Las usa el arranque del ejecutable independiente (ExeCrt::StartupEarly, compilada siempre y llamada solo con GTASA_STANDALONE_STARTUP).
// El estado se deja en las direcciones del EXE (DatosExe las aporta en el ejecutable propio).
//
//   entry 0x824570 (WinMainCRTStartup, MSVC 7.1) en orden:  GetVersionExA -> [0xC9AC08] plataforma, [0xC9AC14] mayor, [0xC9AC18] menor, [0xC9AC0C] compilacion, [0xC9AC10] mayor<<8+menor
//     __heap_init(1) 0x82C471 (fallo: _amsg_exit(0x1C))   __mtinit 0x827D25 (fallo: _amsg_exit(0x10))   __RTC_Initialize 0x82BEDA (rango vacio 0x88E240..0x88E240: no hace nada)
//     __ioinit 0x82C0B9 (CrtLowIo, fallo: 0x1B)   GetCommandLineA   __crtGetEnvironmentStringsA 0x82DBC8   __setargv 0x82DB26 (fallo: 8)   __setenvp 0x82D8F3 (fallo: 9)
//     __cinit 0x823B76 (0 = bien)   GetStartupInfoA   __wincmdln 0x82D88A   WinMain 0x748710   exit 0x823CAE
//   __cinit 0x823B76 ejecuta la tabla C 0x8A5A14..0x8A5A30 (0x826A72, __onexitinit 0x821CBE, __initstdio 0x823EE5, 0x8289F5, 0x833494, 0x82AC54, __security_init_cookie 0x8339CA),
//   registra atexit(0x82BF1E) y ejecuta los 1.668 inicializadores de C++ 0x8A4000..0x8A5A10 (ver docs/CodigoVivoExe/fase8-inicializadores.csv).
//
// Lo reconstruido aqui (todas cdecl, sin __asm):
//   __heap_select 0x82C457 (1 = monton del sistema desde Windows 2000)   __heap_init 0x82C471 (el camino del monton de bloques pequenos, __active_heap == 3, NO se reproduce: devuelve fallo)
//   __mtinitlocks 0x82AC74   __mtdeletelocks 0x82ACBD   __mtinit 0x827D25   __mtterm 0x827B0C   __onexitinit 0x821CBE

namespace ExeCrt {
void __cdecl OsVersionInit();    // prologo de `entry` 0x824570 (hasta el GetVersionExA): rellena las variables de version
int  __cdecl HeapSelect();       // 0x82C457
int  __cdecl HeapInit(int mode); // 0x82C471: 1 = bien, 0 = fallo
int  __cdecl Mtinitlocks();      // 0x82AC74: 1 = bien, 0 = fallo
void __cdecl Mtdeletelocks();    // 0x82ACBD
int  __cdecl Mtinit();           // 0x827D25: 1 = bien, 0 = fallo
void __cdecl Mtterm();           // 0x827B0C
int  __cdecl Onexitinit();       // 0x821CBE: 0 = bien, 0x18 si no hay memoria

// Parte de `entry` que reconstruyen las funciones de arriba mas __ioinit: devuelve 0 si todo fue bien o el codigo de _amsg_exit (0x1C monton, 0x10 hilos, 0x1B E/S).
int  __cdecl StartupEarly();

// Arranque completo del estado del CRT del EXE para el ejecutable independiente (GTASA_STANDALONE_STARTUP): StartupEarly + __onexitinit + __initstdio + las constantes x87 de CrtStaticInit.
// Devuelve 0 o el codigo de _amsg_exit. Los 140 constructores estaticos con atexit y las 38 asignaciones de datos de _initterm los aporta DatosExe (ver docs/CodigoVivoExe/fase8-arranque.md).
int  __cdecl StandaloneStartup();
} // namespace ExeCrt
