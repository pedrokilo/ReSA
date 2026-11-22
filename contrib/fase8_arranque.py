"""Fase 8, parte P5 (lotes F1-F4) del plan de codigo vivo: mapa estatico del arranque del CRT del EXE.

Estatico (capstone sobre el EXE canonico, sin ejecutar el juego). Escribe en docs/CodigoVivoExe/:
  fase8-inicializadores.csv  los 1.673 inicializadores de _initterm (tabla C 0x8A5A14-0x8A5A30 y tabla C++ 0x8A4000-0x8A5A10): patron, destino,
                             operandos, llamadas y si el C++ del proyecto ya cita la direccion del destino (DatosExe u otro archivo)
  fase8-funclets-f4.csv      los 44 funclets Unwind@ sin dueno del lote F4 con su dueno real (tabla de desenrollado -> ehfuncinfo -> envoltura -> funcion)

Uso: python -X utf8 contrib/fase8_arranque.py [--exe RUTA] [--salida docs/CodigoVivoExe]
"""
import argparse
import collections
import csv
import re
import struct
import sys
from pathlib import Path

import capstone
from capstone.x86 import X86_OP_IMM, X86_OP_MEM

sys.path.insert(0, str(Path(__file__).resolve().parent))
from datos_exe import quitar_comentarios  # noqa: E402

EXE_DEFECTO = r'D:\Grand Theft Auto San Andreas\gta-sa.exe'
RAIZ = Path(__file__).resolve().parent.parent

TABLAS = [('C', 0x8A5A14, 0x8A5A30), ('C++', 0x8A4000, 0x8A5A10)]
ATEXIT = 0x821D1E
INICIO_DATOS = 0x858000

# Duenos que el informe original no resolvio (lote F4): dos despachadores de comandos de script (docs/ComandosScript.csv, columna despachador_exe) que Ghidra no separo como funcion
DESPACHADORES = {
    0x472310: ('despachador de comandos de script 2000-2099 (0x472310, 10 estados de desenrollado)', 'COMMAND 2000-2099'),
    0x490DB0: ('despachador de comandos de script 1500-1599 (0x490DB0, 34 estados de desenrollado)', 'COMMAND 1500-1599'),
}


class Imagen:
    def __init__(self, ruta):
        self.datos = Path(ruta).read_bytes()
        pe = struct.unpack_from('<I', self.datos, 0x3C)[0]
        n = struct.unpack_from('<H', self.datos, pe + 6)[0]
        tam_opt = struct.unpack_from('<H', self.datos, pe + 20)[0]
        self.base = struct.unpack_from('<I', self.datos, pe + 52)[0]
        self.secciones = []
        for i in range(n):
            o = pe + 24 + tam_opt + i * 40
            vs, va, rs, rp = struct.unpack_from('<IIII', self.datos, o + 8)
            self.secciones.append((va, vs, rp, rs))

    def leer(self, va, n):
        rva = va - self.base
        for sva, vs, rp, rs in self.secciones:
            if sva <= rva < sva + max(vs, rs):
                off = rp + rva - sva
                return self.datos[off:off + n] if rva - sva < rs else b'\0' * n
        return b''

    def buscar_dword(self, valor):
        pat = struct.pack('<I', valor)
        out = []
        for sva, vs, rp, rs in self.secciones:
            d = self.datos[rp:rp + rs]
            i = d.find(pat)
            while i >= 0:
                out.append(self.base + sva + i)
                i = d.find(pat, i + 1)
        return out


def literales_cpp():
    """-> {direccion: primer archivo} de las literales 0x4xxxxx-0xDxxxxx fuera de comentarios en source/."""
    rx = re.compile(r'0x0*([4-9A-Da-d][0-9A-Fa-f]{5})\b')
    out = {}
    for p in sorted((RAIZ / 'source').rglob('*')):
        if p.suffix not in ('.cpp', '.h', '.hpp', '.inc') or p.name == 'CrtStaticInitTable.inc':
            continue
        txt = quitar_comentarios(p.read_text(encoding='utf-8', errors='replace'))
        for m in rx.finditer(txt):
            out.setdefault(int(m.group(1), 16), p.relative_to(RAIZ).as_posix())
    return out


# Inicializadores del CRT que reconstruye source/game_sa/Crt (ExeCrt, sin gancho: corren antes de que exista la DLL). Parte CRT-B de la fase 8 (ejecucion-crt-fase8b.md).
INICIALIZADOR_CRT_HECHO = {
    0x826A72: 'ExeCrt::TableC826A72', 0x821CBE: 'ExeCrt::Onexitinit', 0x823EE5: 'ExeCrt::Initstdio', 0x8289F5: 'ExeCrt::TableCSseAvailableInit',
    0x833494: 'ExeCrt::InitMbcTable', 0x82AC54: 'ExeCrt::TableCUnhandledFilterInit', 0x8339CA: 'ExeCrt::InitSecurityCookie',
}


def estado_inicializador(fila, va):
    """(estado, nota) de un inicializador tras la parte CRT-B: hecho en ExeCrt::Cinit, constante x87 hecha, o pendiente de DatosExe (objetos del juego)."""
    if va in INICIALIZADOR_CRT_HECHO:
        return 'hecho', INICIALIZADOR_CRT_HECHO[va] + ' (ExeCrt::Cinit, en el orden del EXE)'
    clase = fila['clase']
    if clase.startswith('constante x87'):
        return 'hecho', 'constante x87 plegada en CrtStaticInitTable.inc (ExeCrt::RunX87StaticInitializers)'
    if clase.startswith('constructor'):
        return 'pendiente-DatosExe', 'constructor de un objeto estatico del juego (memoria .bss/.data de DatosExe) y destructor registrado con atexit: lo ejecutara DatosExe al definir el objeto'
    if clase.startswith('llamada'):
        return 'pendiente-DatosExe', 'llamada que rellena datos .bss/.data del juego (DatosExe)'
    cita = fila['destino_citado_en_cpp']
    if cita in ('si', 'parcial'):
        return 'pendiente-DatosExe', 'valor inicial de un global del juego que el C++ ya declara en ' + (fila['archivo_cpp'] or 'otro archivo') + ': lo define DatosExe'
    return 'pendiente-DatosExe', 'valor inicial de un global del juego (.bss/.data) sin declaracion en C++ todavia: lo define DatosExe'


def inicializadores(img, cpp):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    filas = []
    for nombre, a, b in TABLAS:
        punteros = struct.unpack('<%dI' % ((b - a) // 4), img.leer(a, b - a))
        for i, p in enumerate(punteros):
            if p == 0:
                continue
            mnem, llamadas, destinos, fuentes = [], [], [], []
            n = 0
            for ins in md.disasm(img.leer(p, 400), p):
                n = ins.address + ins.size - p
                mnem.append(ins.mnemonic)
                if ins.mnemonic == 'call' and ins.operands[0].type == X86_OP_IMM:
                    llamadas.append(ins.operands[0].imm)
                for k, op in enumerate(ins.operands):
                    if op.type == X86_OP_MEM and op.mem.base == 0 and op.mem.index == 0 and op.mem.disp >= INICIO_DATOS:
                        escribe = k == 0 and ins.mnemonic in ('mov', 'fstp', 'fst', 'fistp', 'and', 'or', 'movss', 'movsd')
                        (destinos if escribe else fuentes).append(op.mem.disp)
                if ins.mnemonic == 'ret' or (ins.mnemonic == 'jmp' and ins.operands[0].type == X86_OP_IMM and not p <= ins.operands[0].imm < p + 400):
                    break
            patron = ' '.join(mnem[:6])
            if not llamadas and mnem[:1] in (['fld'], ['fild']):
                clase = 'constante x87 (se pliega en C++)'
            elif ATEXIT in llamadas:
                clase = 'constructor + atexit (destructor estatico)'
            elif llamadas:
                clase = 'llamada a constructor/funcion'
            else:
                clase = 'asignacion de datos'
            cubiertos = sorted({cpp[d] for d in set(destinos) if d in cpp})
            filas.append({
                'tabla': nombre, 'indice': i, 'va': '0x%08X' % p, 'bytes': n, 'clase': clase, 'patron': patron,
                'destinos': ' '.join('0x%X' % d for d in sorted(set(destinos))),
                'fuentes': ' '.join('0x%X' % d for d in sorted(set(fuentes))),
                'llamadas': ' '.join('0x%X' % c for c in llamadas),
                'destino_citado_en_cpp': 'si' if destinos and all(d in cpp for d in set(destinos)) else ('parcial' if cubiertos else 'no'),
                'archivo_cpp': cubiertos[0] if cubiertos else '',
            })
            filas[-1]['estado'], filas[-1]['nota'] = estado_inicializador(filas[-1], p)
    return filas


def funclets_f4(img, filas_vivas):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    funclets = sorted(int(f['va'], 16) for f in filas_vivas if f['lote'] == 'F4')
    # ehfuncinfo (magic 0x19930520): {magic, maxState, pUnwindMap, ...}
    ehs = []
    for sva, vs, rp, rs in img.secciones:
        d = img.datos[rp:rp + rs]
        i = d.find(struct.pack('<I', 0x19930520))
        while i >= 0:
            va_i = img.base + sva + i
            if va_i % 4 == 0 and va_i >= INICIO_DATOS and i + 20 <= len(d):
                mx, tabla = struct.unpack_from('<II', d, i + 4)
                if 0 < mx < 4096 and tabla >= INICIO_DATOS:
                    ehs.append((va_i, mx, tabla))
            i = d.find(struct.pack('<I', 0x19930520), i + 1)
    filas = []
    for f in funclets:
        refs = img.buscar_dword(f)
        tabla = refs[0] if refs else 0
        eh = next((a for a, mx, t in ehs if mx and t <= tabla < t + 8 * mx), 0)
        envoltura = (img.buscar_dword(eh) or [0])[0] if eh else 0
        base = envoltura - 1 if envoltura else 0  # `mov eax, imm32` empieza un byte antes del imm
        usos = img.buscar_dword(base) if base else []
        dueno = 0
        if usos:
            # el `push offset envoltura` va a 6 bytes del `push -1; mov eax,fs:[0]` que abre la funcion: se busca el prologo hacia atras
            u = usos[0]
            ventana = img.leer(u - 0x20, 0x20)
            k = ventana.rfind(b'\x6a\xff\x64\xa1\x00\x00\x00\x00')
            dueno = u - 0x20 + k if k >= 0 else 0
        nombre, rango = DESPACHADORES.get(dueno, ('', ''))
        filas.append({
            'funclet': '0x%08X' % f, 'tabla_desenrollado': '0x%X' % tabla, 'ehfuncinfo': '0x%X' % eh,
            'envoltura': '0x%X' % base if base else '', 'push_offset_en': '0x%X' % usos[0] if usos else '',
            'funcion_duena': '0x%X' % dueno if dueno else '', 'dueno': nombre, 'alcance': rango,
        })
    return filas


FORMAS_X87 = {
    ('fld', 'fdiv', 'fstp'): 0,           # D = A / B
    ('fld', 'fmul', 'fstp'): 1,           # D = A * B
    ('fld', 'fmul', 'fdivr', 'fstp'): 2,  # D = C / (A * B)
    ('fld', 'fsub', 'fstp'): 3,           # D = A - B
}


def inicializadores_x87(img):
    """-> filas {forma, dest, a, b, c, tamanos} de los inicializadores de constantes x87 con las cuatro formas simples, en el orden de la tabla C++."""
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    a, b = TABLAS[1][1], TABLAS[1][2]
    filas = []
    for p in struct.unpack('<%dI' % ((b - a) // 4), img.leer(a, b - a)):
        if p == 0:
            continue
        ops = []
        for ins in md.disasm(img.leer(p, 64), p):
            if ins.mnemonic == 'ret':
                break
            if not ins.operands or ins.operands[0].type != X86_OP_MEM or ins.operands[0].mem.base or ins.operands[0].mem.index:
                ops = None
                break
            ops.append((ins.mnemonic, ins.operands[0].mem.disp, ins.operands[0].size))
        if not ops or tuple(o[0] for o in ops) not in FORMAS_X87:
            continue
        forma = FORMAS_X87[tuple(o[0] for o in ops)]
        dirs = [o[1] for o in ops]
        tam = [o[2] for o in ops]
        if any(s not in (4, 8) for s in tam):
            continue
        fila = {'forma': forma, 'dest': dirs[-1], 'a': dirs[0], 'b': dirs[1], 'c': dirs[2] if forma == 2 else 0}
        # bit0 A es double, bit1 B, bit2 C (formas con tres operandos), bit3 destino
        mask = (1 if tam[0] == 8 else 0) | (2 if tam[1] == 8 else 0)
        mask |= (4 if forma == 2 and tam[2] == 8 else 0) | (8 if tam[-1] == 8 else 0)
        fila['tamanos'] = mask
        fila['va'] = p
        filas.append(fila)
    return filas


# Resultado por funcion de los lotes E2-* (sin locale) y F1-F4. Direcciones de las rutinas que esta parte engancho o reconstruyo.
ENGANCHADAS_P4 = {
    0x8266F0: 'memcpy -> memmove del CRT de MSVC (CrtMem)', 0x822A20: 'memmove -> memmove del CRT de MSVC (CrtMem)', 0x82D4C0: 'memset -> memset del CRT de MSVC (CrtMem)',
    0x8213D0: 'strchr -> strchr del CRT de MSVC (CrtMem)', 0x82514C: '_strdup con el malloc del EXE (CrtMem)', 0x826071: 'strtod sobre CrtFloatIn (CrtStrtod)', 0x82BF62: '_fltin2 sobre CrtFloatIn (CrtStrtod)',
    0x828C4A: '_msize = HeapSize del montón del CRT (CrtHeap)',
    0x8211D6: 'destructor con borrado escalar (CrtExceptionDtor)', 0x8212B9: 'destructor con borrado escalar (CrtExceptionDtor)', 0x822F05: 'destructor con borrado escalar (CrtExceptionDtor)',
    0x822F40: 'destructor con borrado escalar (CrtExceptionDtor)', 0x822F5C: 'destructor con borrado escalar (CrtExceptionDtor)', 0x822F78: 'destructor con borrado escalar (CrtExceptionDtor)',
    0x822FBD: 'destructor con borrado escalar (CrtExceptionDtor)', 0x822E45: 'std::exception::what (CrtExceptionDtor)',
}
ARRANQUE_CRTB = {
    0x823B76: '__cinit (CrtStartup::Cinit)', 0x821397: '__fpmath / _FPinit (CrtStartup::FpMath)', 0x833494: '__initmbctable (CrtMbcs::InitMbcTable)', 0x826A72: 'tabla C 0x826A72 (CrtStartup::TableC826A72)',
    0x8289F5: '__sse2_available_init (CrtStartup::TableCSseAvailableInit)', 0x8289C0: 'prueba SSE2 bajo SEH (CrtStartup::Sse2ExecutionProbe)', 0x82AC54: 'tabla C 0x82AC54 (CrtStartup::TableCUnhandledFilterInit)',
    0x82AC0E: 'filtro de excepciones no manejadas (CrtStartup::CrtUnhandledExceptionFilter; terminate del CRT enlazado)', 0x832ECF: '_ValidateExecute (CrtStartup::IsValidCodePointer)',
    0x8339CA: '__security_init_cookie (CrtStartup::InitSecurityCookie)', 0x82BF1E: 'terminacion de RTC (CrtStartup::RtcTerminate)',
    0x834B6E: '_strncnt (CrtMbcs::Strncnt; usercall: reconstruida sin gancho)', 0x832F45: '__getmbcp lcid (absorbida en CrtMbcs::MbcpToLcid; usercall)',
}
ENGANCHADAS_CRTB = {
    0x833344: '_setmbcp (CrtMbcs::Setmbcp)', 0x83319E: 'setmbcp_nolock (CrtMbcs::SetmbcpCore)', 0x832F74: 'setSBCS (CrtMbcs::SetSbcs)', 0x832F9D: 'setSBUpLow (CrtMbcs::SetSbUpLow)',
    0x83312F: '__updatetmbcinfo (CrtMbcs::UpdateTMbcInfo)', 0x836697: '_mbschr (CrtMbcs::Mbschr)',
}
ARRANQUE_RECONSTRUIDO = {
    0x82C457: '__heap_select (CrtStartup::HeapSelect)', 0x82C471: '__heap_init (CrtStartup::HeapInit)', 0x82AC74: '__mtinitlocks (CrtStartup::Mtinitlocks)',
    0x82ACBD: '__mtdeletelocks (CrtStartup::Mtdeletelocks)', 0x827D25: '__mtinit (CrtStartup::Mtinit)', 0x827B0C: '__mtterm (CrtStartup::Mtterm)',
    0x821CBE: '__onexitinit (CrtStartup::Onexitinit)',
}
# Tablas de datos de los destructores con borrado escalar, cuerpos y envolturas de try/finally de rutinas ya en C++ (RAII): se descartan
DESCARTE_RAZON = {
    'estado por hilo y bloqueos': 'funclet try/finally (desbloqueo) de una rutina del CRT ya en C++ con RAII, o ayudante del manejador de excepciones',
    'E/S y stdio': 'funclet try/finally (_unlock_file) de una rutina de stdio ya en C++ con RAII',
}


def resultado_lote(fila):
    va = int(fila['va'], 16)
    lote, sub = fila['lote'], fila['subgrupo']
    if va in ENGANCHADAS_P4:
        return 'enganchada', ENGANCHADAS_P4[va]
    if va in ENGANCHADAS_CRTB:
        return 'enganchada', ENGANCHADAS_CRTB[va]
    if va in ARRANQUE_RECONSTRUIDO:
        return 'reconstruida-sin-gancho', ARRANQUE_RECONSTRUIDO[va]
    if va in ARRANQUE_CRTB:
        return 'reconstruida-sin-gancho', ARRANQUE_CRTB[va]
    if lote == 'F3':
        return 'enlazador', 'thunk de carga retardada / importacion: lo genera el enlazador (DelayLoadThunks.cpp ya reconstruye los envoltorios stdcall; los ayudantes jmp eax no se escriben en C++)'
    if lote == 'F4':
        return 'descartada-despachador-reescrito', 'funclet Unwind@ de un despachador de comandos de script del EXE (fase8-funclets-f4.csv): el despachador ya es C++ (RunningScriptCommands1500To1599.cpp / 2000To2099.cpp) y el funclet no se ejecuta'
    if lote == 'F2':
        return 'inicializador', 'inicializador de la tabla C de _initterm (0x8A5A14): ver fase8-inicializadores.csv'
    if lote == 'F1':
        return 'muere-con-entry', 'arranque del CRT de MSVC 7.1 (entry 0x824570 y su cadena): el ejecutable propio usa el arranque de su enlazador + ExeCrt::StandaloneStartup'
    if sub.startswith('excepciones'):
        return 'enlazar', 'runtime de excepciones/SEH/senales del CRT de MSVC: lo aporta el CRT del ejecutable propio; nada del C++ propio lo llama por direccion'
    for clave, razon in DESCARTE_RAZON.items():
        if sub.startswith(clave):
            return 'descartar', razon
    if sub.startswith('cadenas'):
        return 'enlazar', 'rutina de cadenas/memoria del CRT: la aporta el CRT de MSVC'
    if sub.startswith('montón') or sub.startswith('mont'):
        return 'descartar', 'ayudante del montón del CRT del EXE (funclet de tabla o cuerpo de std::exception) o rutina del montón de bloques pequeños sin uso con __active_heap == 1'
    return 'revisar', ''


LOTES_P45 = ('E2-excepciones-1', 'E2-excepciones-2', 'E2-montón-1', 'E2-E/S-1', 'E2-cadenas-1', 'E2-estado-1', 'E2-sin-1', 'F1', 'F2', 'F3', 'F4')


def lotes_p45(filas_vivas):
    out = []
    for f in filas_vivas:
        if f['lote'] not in LOTES_P45:
            continue
        res, motivo = resultado_lote(f)
        out.append({'lote': f['lote'], 'va': f['va'], 'bytes': f['bytes'], 'nombre_ghidra': f['nombre_ghidra'], 'subgrupo': f['subgrupo'], 'resultado': res, 'detalle': motivo})
    return out


def escribir_tabla_x87(ruta, filas):
    lineas = ['// Generado por contrib/fase8_arranque.py --tabla-x87 a partir del EXE canonico (no editar a mano).',
              '// {forma, destino, A, B, C, tamanos}: forma 0 D=A/B, 1 D=A*B, 2 D=C/(A*B), 3 D=A-B; tamanos: bit0 A double, bit1 B double, bit2 C double, bit3 destino double.',
              '// Una fila por inicializador estatico de constantes x87 del EXE (tabla C++ 0x8A4000-0x8A5A10), en el orden de ejecucion de _initterm.']
    for f in filas:
        lineas.append('{%d, 0x%X, 0x%X, 0x%X, 0x%X, %d}, // 0x%X' % (f['forma'], f['dest'], f['a'], f['b'], f['c'], f['tamanos'], f['va']))
    Path(ruta).write_text(chr(10).join(lineas) + chr(10), encoding='utf-8', newline='')


def escribir(ruta, filas):
    with open(ruta, 'w', encoding='utf-8', newline='') as s:
        w = csv.DictWriter(s, fieldnames=list(filas[0].keys()), lineterminator='\n')
        w.writeheader()
        w.writerows(filas)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--exe', default=EXE_DEFECTO)
    ap.add_argument('--salida', default=str(RAIZ / 'docs' / 'CodigoVivoExe'))
    ap.add_argument('--tabla-x87', default=str(RAIZ / 'source' / 'game_sa' / 'Crt' / 'CrtStaticInitTable.inc'), help='tabla generada para CrtStaticInit.cpp')
    args = ap.parse_args()
    img = Imagen(args.exe)
    salida = Path(args.salida)
    cpp = literales_cpp()
    ini = inicializadores(img, cpp)
    escribir(salida / 'fase8-inicializadores.csv', ini)
    with open(salida / 'funciones-vivas.csv', encoding='utf-8-sig', newline='') as s:
        vivas = list(csv.DictReader(s))
    f4 = funclets_f4(img, vivas)
    escribir(salida / 'fase8-funclets-f4.csv', f4)
    lotes = lotes_p45(vivas)
    escribir(salida / 'fase8-arranque-lotes.csv', lotes)
    print('lotes P4/P5:', collections.Counter((f['lote'].split('-')[0], f['resultado']) for f in lotes))
    x87 = inicializadores_x87(img)
    escribir_tabla_x87(args.tabla_x87, x87)
    print('constantes x87 en tabla:', len(x87), collections.Counter(f['forma'] for f in x87))
    por_clase = collections.Counter((f['tabla'], f['clase']) for f in ini)
    print('inicializadores:', len(ini))
    for k, v in sorted(por_clase.items()):
        print(' ', k, v)
    print('destino citado en C++:', collections.Counter(f['destino_citado_en_cpp'] for f in ini))
    print('funclets F4:', len(f4), collections.Counter(f['funcion_duena'] for f in f4))


if __name__ == '__main__':
    main()
