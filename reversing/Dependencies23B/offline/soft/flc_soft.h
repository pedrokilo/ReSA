// ============================================================================================================================================================
// g23b: maquina x87 de software (CVehicle::FlyingControl, 0x6D85F0).
// Reproduce, solo con aritmetica entera, las instrucciones x87 que ejecuta el EXE a precision de 24 bits (palabra de control 0x7F del juego): valores de 64 bits de mantisa,
// reglas de NaN del x87, banderas acumuladas (IE DE ZE OE UE PE), C0..C3 y C1, y los registros de la pila con el residuo que el EXE deja en los que quedan por debajo de la cima.
// No ejecuta ninguna instruccion de coma flotante (ni x87 ni SSE) al calcular: el hardware solo interviene en FlcX87::Commit (deja el estado final) y en fsin/fcos/fpatan/pow/asin/acos.
// ============================================================================================================================================================
#pragma once

struct FlcExt {
    unsigned long long sig;   // mantisa con bit entero (bit 63 = 1 en normales y NaN; en NaN el bit 62 es el de "silenciado"); 0 para cero
    int                exp;   // exponente sin sesgo: valor = sig * 2^(exp - 63)
    unsigned char      kind;  // 0 cero, 1 normal, 2 infinito, 3 NaN
    unsigned char      neg;
};
enum { FLC_ZERO = 0, FLC_NORMAL = 1, FLC_INF = 2, FLC_NAN = 3 };

// Banderas acumuladas (posiciones de la palabra de estado) y codigos de condicion.
enum { FLC_IE = 0x01, FLC_DE = 0x02, FLC_ZE = 0x04, FLC_OE = 0x08, FLC_UE = 0x10, FLC_PE = 0x20 };
enum { FLC_C0 = 0x0100, FLC_C1 = 0x0200, FLC_C2 = 0x0400, FLC_C3 = 0x4000 };

// Conversiones (los operandos de memoria float/double/int32 que carga el EXE). `flags` recibe DE/IE de la carga.
FlcExt FlcFromFloat(unsigned int bits, unsigned int* flags);
FlcExt FlcFromDouble(unsigned long long bits, unsigned int* flags);
FlcExt FlcFromInt(int value);
// fstp dword: devuelve los bits del float; flags recibe OE/UE/PE; *roundedUp = C1.
unsigned int FlcToFloat(const FlcExt& v, unsigned int* flags, bool* roundedUp);
unsigned long long FlcToDouble(const FlcExt& v);
unsigned long long FlcToDoubleFlags(const FlcExt& v, unsigned int* flags, bool* roundedUp);
FlcExt FlcFromDoubleExact(double d);   // sin banderas (valores que ya estaban en un registro)
double FlcToDoubleValue(const FlcExt& v);

// Aritmetica a 24 bits (resultado redondeado al par). `state` = banderas acumuladas; *c1 = el resultado se redondeo hacia arriba (magnitud mayor).
FlcExt FlcAdd(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1);
FlcExt FlcMul(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1);
FlcExt FlcDiv(const FlcExt& a, const FlcExt& b, unsigned int* state, bool* c1);
FlcExt FlcSqrt(const FlcExt& a, unsigned int* state, bool* c1);
// Comparacion ordenada: devuelve C0|C2|C3 (0x0100, 0x0400, 0x4000) como fcom; marca IE si hay un NaN.
unsigned int FlcCompare(const FlcExt& a, const FlcExt& b, unsigned int* state, bool unorderedSignals);
bool FlcSameBits(const FlcExt& a, const FlcExt& b);

// ---------------------------------------------------------------------------------------------------------------------------------------------------------
// La maquina: pila de 8 niveles. reg[d] = registro del nivel d (d = 0 es el primero que se apila tras la entrada, es decir T-1; la cima es reg[depth - 1]).
// ---------------------------------------------------------------------------------------------------------------------------------------------------------
struct FlcX87 {
    FlcExt        reg[8];
    bool          written[8];
    int           depth;
    int           maxDepth;
    unsigned int  state;       // banderas acumuladas de esta maquina (FLC_IE ... FLC_PE)
    unsigned int  cc;          // C0 / C2 / C3 de la ultima operacion que los escribe
    bool          ccValid;     // alguna operacion ha escrito C0/C2/C3
    bool          c1;
    int           lastClass;   // 0 ninguna, 1 fcomp m32, 2 fstp m32, 3 otra (clase de la ultima instruccion x87 del EXE; la reproduce Commit)
    float         lastFloat;   // (reservado)
    unsigned int  pendingDE;   // DE del operando de memoria, que solo cuenta si ningun operando es NaN

    FlcX87();

    // carga / almacenamiento
    void Fld(const float& m);              // fld dword [m]
    void FldD(const double& m);            // fld qword [m]
    void FldI(int value);                  // fild dword
    void FldSt(int i);                     // fld st(i)
    void FldZero();                        // fldz (no se usa en esta funcion, sirve a las pruebas)
    void FldExt(const FlcExt& v);          // empuja un valor ya calculado (retorno en ST0 de un llamado)
    void FldRet(const float& m);           // valor float que un llamado deja en ST0: la carga ya levanto sus banderas en el hardware, aqui solo se apila (senalizante -> silenciado)
    void Fstp(float& m);                   // fstp dword [m]
    void Fst(float& m);                    // fst dword [m]
    void FstpSt(int i);                    // fstp st(i)
    void FstSt(int i);
    void Fxch(int i);
    void Fchs();
    void Fabs();
    void Pop();                            // fstp st(0)
    // aritmetica ST0 op= m32 / m64 / int
    void Fadd(const float& m);   void Fsub(const float& m);   void Fsubr(const float& m);   void Fmul(const float& m);   void Fdiv(const float& m);   void Fdivr(const float& m);
    void FaddD(const double& m); void FsubD(const double& m); void FsubrD(const double& m); void FmulD(const double& m); void FdivD(const double& m); void FdivrD(const double& m);
    void FimulI(int v);  void FidivI(int v);  void FiaddI(int v);
    // aritmetica con registros: dest = (ST(a) op ST(b)); pop = true para las formas con "p"
    void Arith(char op, int dest, int srcA, int srcB, bool pop);   // op: '+', '-', '*', '/'; calcula ST(srcA) op ST(srcB) y lo escribe en ST(dest)
    // comparaciones: fcom/fcomp con m32 o ST(i)
    void FcomMem(const float& m, bool pop);
    void FcomSt(int i, bool pop);
    void Fcompp();
    void Fsqrt();
    void Fsin(); void Fcos(); void Fpatan();
    void FldOne();                         // fld1
    void FstpD(double& m);                 // fstp qword [m]
    void FstD(double& m);                  // fst qword [m]
    void FldI64(long long v);              // fild qword
    void Fistp64(long long& m);            // fistp qword [m] (redondeo al par)
    // lecturas de la ultima comparacion: AH de fnstsw (bits C0=1, C2=4, C3=0x40)
    unsigned int Ah() const;
    // valor de ST(i)
    const FlcExt& St(int i) const { return reg[depth - 1 - i]; }
    float  StFloat(int i);                 // el valor de ST(i) como float (sin tocar banderas)

    // Deja el estado final en el hardware (registros muertos T-1..T-maxDepth, banderas acumuladas y codigos de condicion). Implementado en X87Math.cpp.
    void Commit();

private:
    FlcExt& Top() { return reg[depth - 1]; }
    FlcExt& S(int i) { return reg[depth - 1 - i]; }
    void Push(const FlcExt& v);
    void Write(int i, const FlcExt& v);    // escribe ST(i) y marca el nivel
    FlcExt MemOperand(const float& m);
    FlcExt MemOperandD(const double& m);
    void ArithReg(char op, FlcExt a, FlcExt b, int dest, bool pop, bool memNaNOperand);
};

// Funciones x87 reales (X87Math.cpp, /arch:IA32): fsin, fcos y fpatan sobre doubles; el resultado de 64 bits sale como double.
double __cdecl FlcHwSin(double x);
double __cdecl FlcHwCos(double x);
double __cdecl FlcHwAtan2(double y, double x);
FlcExt FlcFromInt64(long long value);
