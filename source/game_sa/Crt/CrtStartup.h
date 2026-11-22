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

// Linea de comandos, entorno y cierre (sin gancho; el arranque corre antes de la DLL y _doexit lo usa el cierre propio):
char* __cdecl CrtGetEnvironmentStrings();                    // 0x82DBC8 __crtGetEnvironmentStringsA: copia ANSI (malloc) del bloque de entorno, null si falla
int   __cdecl Setargv();                                     // 0x82DB26: _pgmptr, __argc, __argv (0 = bien, -1 = sin memoria)
int   __cdecl Setenvp();                                     // 0x82D8F3: _environ desde _aenvptr (0 = bien, -1 = sin memoria)
int   __cdecl StartupArguments();                            // GetCommandLineA + entorno + Setargv + Setenvp: 0, 8 (argumentos) o 9 (entorno)
void  __cdecl CrtExitProcess(int code);                      // 0x823AFE __crtExitProcess: CorExitProcess si hay mscoree.dll y ExitProcess; no vuelve
void  __cdecl DoExit(int code, int quick, int retcaller);    // 0x823BDB _doexit: atexit/_onexit en orden inverso, terminadores y salida (exit = DoExit(code, 0, 0))

// Parte de `entry` que reconstruyen las funciones de arriba mas __ioinit: devuelve 0 si todo fue bien o el codigo de _amsg_exit (0x1C monton, 0x10 hilos, 0x1B E/S).
int  __cdecl StartupEarly();

// __cinit 0x823B76 (sin gancho): _FPinit = __fpmath 0x821397, tabla C completa en el orden del EXE (0x826A72, __onexitinit, __initstdio, 0x8289F5 deteccion SSE2, __initmbctable, 0x82AC54 filtro de
// excepciones), atexit(0x82BF1E = RtcTerminate), cookie de seguridad (0x8339CA, primer inicializador de C++) y las constantes x87. Devuelve 0 o el primer codigo distinto de cero de la tabla.
// Entradas de la tabla C de __cinit (todas devuelven 0; las ejecuta Cinit en este orden; publicas para el banco fuera del juego, reversing/FnCRTB8A):
int  __cdecl TableC826A72();                 // 0x826A72: ocho bytes a 0xFF en 0x8E37B4
int  __cdecl TableCSseAvailableInit();       // 0x8289F5: CPUID + prueba SSE2 -> 0xC9C400/0xC9C404 (con 0x8289C0 dentro)
int  __cdecl TableCUnhandledFilterInit();    // 0x82AC54: SetUnhandledExceptionFilter(0x82AC0E) y el filtro anterior en 0xC9AC74
long __stdcall CrtUnhandledExceptionFilter(void* exceptionPointers); // 0x82AC0E: terminate para excepciones de C++ sin capturar; si no, encadena con el filtro anterior (0xC9AC74)
void __cdecl FpMath();           // 0x821397 __fpmath (_FPinit): dispatch de intrinsecas, _adjust_fdiv, precision de 53 bits, fnclex
int  __cdecl Cinit();
void __cdecl RtcTerminate();       // 0x82BF1E
void __cdecl InitSecurityCookie(); // 0x8339CA

// Arranque completo del estado del CRT del EXE para el ejecutable independiente (GTASA_STANDALONE_STARTUP): StartupEarly + StartupArguments (con __initmbctable) + Cinit.
// Devuelve 0 o el codigo de _amsg_exit. Los 140 constructores estaticos con atexit y las asignaciones de datos del juego de _initterm los aporta DatosExe (ver docs/CodigoVivoExe/fase8-inicializadores.csv).
int  __cdecl StandaloneStartup();
bool __cdecl StandaloneStartupRan();   // true si ya corrio StandaloneStartup (ejecutable independiente: el codigo original no esta); en el ASI sobre el EXE es siempre false
int  __cdecl FpIntrinsicNonNegative(const double* value); // 0x826B26 (entrada 4 de la tabla de intrinsecas 0x8E30F0)
} // namespace ExeCrt
