// Memoria que hace de "imagen del EXE" en el arnes aislado: una seccion RWX de 10 MB que el enlazador coloca la PRIMERA de la imagen (exemem.obj va el primero en la linea
// de comandos del enlazador, base 0x400000), es decir en 0x401000..0xE00FFF. Asi las direcciones del EXE usadas por las 20 funciones y por la DLL (0x758920..0x8E2300, 0xC97B24, 0xC9B8C8...)
// son memoria valida y escribible del propio proceso, sin depender de que el sistema deje libre esa zona del espacio de direcciones.
#pragma section(".exemem", read, write, execute)
extern "C" __declspec(allocate(".exemem")) unsigned char g_exeMem[0xA00000] = {1};

// Diagnostico: ante una excepcion no controlada imprime EIP/EAX/ECX/EDX/ESP y termina (se instala antes que cualquier inicializador de los objetos enlazados).
#include <windows.h>
#include <cstdio>
#pragma init_seg(compiler)
static LONG WINAPI ReportCrash(EXCEPTION_POINTERS* p) {
    std::fprintf(stderr, "EXCEPCION %08lx en EIP=%08lx EAX=%08lx ECX=%08lx EDX=%08lx ESP=%08lx\n", p->ExceptionRecord->ExceptionCode, p->ContextRecord->Eip,
                 p->ContextRecord->Eax, p->ContextRecord->Ecx, p->ContextRecord->Edx, p->ContextRecord->Esp);
    return EXCEPTION_EXECUTE_HANDLER;
}
static const struct InstallCrashReport { InstallCrashReport() { SetUnhandledExceptionFilter(ReportCrash); } } installCrashReport;
