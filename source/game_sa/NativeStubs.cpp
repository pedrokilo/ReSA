// Compile separately: /O2 /Oi /Oy /arch:IA32 /fp:fast, ordinary /Zi and no RTC/PCH/unity.
// Funciones sueltas del EXE que no pertenecen a ninguna clase y cuyo cuerpo debe coincidir byte a byte
// con el original. Declaradas en CarCtrl.h (sólo alias de dirección; CCarCtrl es el dueño de la instrumentación).

// 0x424100: XOR AL,AL; RET. Conserva EAX alto, ECX/EDX y los registros no volátiles.
bool Native_00424100(float, float) { return false; }
