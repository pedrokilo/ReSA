"""Prueba negativa del banco Fn230A (lote 230A): aplica (o revierte) una ronda de mutaciones a las fuentes de produccion (rpworld.cpp, FxEmitter.cpp, FxEmitterBP.cpp, FxInfo.cpp, FxInfoManager.cpp y
LibJpegCompressEncode.cpp).

    python -I mutate.py apply 1        guarda la copia buena en artifacts/good/ y aplica las mutaciones de la ronda 1
    python -I mutate.py restore        restaura las fuentes buenas y comprueba que son identicas (SHA256) a las copias
    python -I mutate.py check          comprueba que cada texto original aparece exactamente una vez en las fuentes actuales (sin tocar nada)
    python -I mutate.py sha            imprime el SHA256 de las fuentes de produccion

Cada mutacion es (archivo, texto original, texto mutado); el texto original debe aparecer exactamente una vez. Cada ronda cambia lo que el banco observa (valor devuelto, bytes cambiados, eventos de los
falsos) en las 25 funciones. Los hooks de la DLL estan activos en el juego real antes de que empiece el banco (y varias de estas funciones se ejecutan durante el arranque): cada mutacion se condiciona a algo
que el juego real no produce y el banco si: un objeto desalineado a 4 bytes (los reales estan alineados), un valor de un campo o de un retorno que el EXE nunca da (0x7FFFFFFF, 0xFFFFFF00...). Ninguna
mutacion cambia la convencion de llamada (una pila desbalanceada rompe la compuerta del banco antes de poder compararla).
"""

import hashlib
import json
import shutil
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = Path(__file__).resolve().parents[2]
GOOD = HERE / 'artifacts' / 'good'
RW = 'source/game_sa/RenderWare/rw/rpworld.cpp'
FE = 'source/game_sa/Fx/FxEmitter.cpp'
FB = 'source/game_sa/Fx/FxEmitterBP.cpp'
FI = 'source/game_sa/Fx/Blueprints/FxInfo.cpp'
FM = 'source/game_sa/Fx/FxInfoManager.cpp'
LJ = 'source/game_sa/Plugins/JPegCompressPlugin/LibJpegCompressEncode.cpp'
SOURCES = [RW, FE, FB, FI, FM, LJ]
NL = chr(10)


def U(name):
    return '(reinterpret_cast<uintptr_t>(' + name + ') & 3)'


def AT(field, off):   # lectura de un dword de cinfo (por desplazamiento) para condicionar una mutacion
    return 'At<uint32>(cinfo, ' + hex(off) + ')'


def round1():
    out = []
    # 0 RpWorldPluginAttach: con el OR de estados igual a 0x7FFFFFFF devuelve FALSE
    out.append((RW, '    if (status < 0) {' + NL + '        return FALSE;' + NL + '    }' + NL + '    if (!_rpTieMeshPluginAttach()) {', '    if (status < 0 || status == 0x7FFFFFFF) {' + NL + '        return FALSE;' + NL + '    }' + NL + '    if (!_rpTieMeshPluginAttach()) {'))
    # 1 _rpLightTieDestroy: libera un nodo nulo con un nodo desalineado
    out.append((RW, '    FreeListFree(TieGlobals().lightTieFreeList, tie);' + NL + '    return TRUE;', '    FreeListFree(TieGlobals().lightTieFreeList, ' + U('tie') + ' ? nullptr : tie);' + NL + '    return TRUE;'))
    # 2 _rpTieDestroy: no libera con un nodo desalineado
    out.append((RW, '    if (tie->apAtom && tie->worldSector) {' + NL + '        TieUnlinkBoth(&tie->lAtomicInWorldSector, &tie->lWorldSectorInAtomic);' + NL + '        FreeListFree(TieGlobals().tieFreeList, tie);', '    if (tie->apAtom && tie->worldSector) {' + NL + '        TieUnlinkBoth(&tie->lAtomicInWorldSector, &tie->lWorldSectorInAtomic);' + NL + '        if (!' + U('tie') + ') FreeListFree(TieGlobals().tieFreeList, tie);'))
    # 3 FxEmitterBP_c cuerpo del destructor: la vtable cambia con un objeto desalineado
    out.append((FB, '    *reinterpret_cast<uint32*>(self) = 0x85A788;', '    *reinterpret_cast<uint32*>(self) = ' + U('self') + ' ? 0x85A789 : 0x85A788;'))
    # 4 FxEmitterBP_c operator new: la alineacion cambia con un tamano de 0x7FFFFFFF
    out.append((FB, '    return g_fxMan.GetMemPool().GetMem(size, 4);', '    return g_fxMan.GetMemPool().GetMem(size, size == 0x7FFFFFFF ? 8 : 4);'))
    # 5 FxEmitter_c cuerpo del constructor: la vtable cambia con un objeto desalineado
    out.append((FE, '    *reinterpret_cast<uint32*>(self) = 0x85A7A4;', '    *reinterpret_cast<uint32*>(self) = ' + U('self') + ' ? 0x85A7A5 : 0x85A7A4;'))
    # 6 FxEmitterBP_c::IsFxInfoPresent: el recorrido no ve la ultima entrada si el objeto esta desalineado
    out.append((FB, '    const int32 count = m_FxInfoManager.m_nNumInfos;' + NL + '    if (count <= 0)' + NL + '        return false;', '    const int32 count = m_FxInfoManager.m_nNumInfos;' + NL + '    if (count <= 0 || ' + U('this') + ')' + NL + '        return false;'))
    # 7 FxEmitter_c::Init: no pone la intensidad a cero con un objeto desalineado
    out.append((FE, '    m_PrimBP             = primBP;' + NL + '    m_System             = system;' + NL + '    m_fEmissionIntensity = 0.0f;', '    m_PrimBP             = primBP;' + NL + '    m_System             = system;' + NL + '    if (!' + U('this') + ') m_fEmissionIntensity = 0.0f;'))
    # 8 FxEmitter_c::Reset: pone 1.0 con un objeto desalineado
    out.append((FE, 'void FxEmitter_c::Reset() {' + NL + '    m_fEmissionIntensity = 0.0f;', 'void FxEmitter_c::Reset() {' + NL + '    m_fEmissionIntensity = ' + U('this') + ' ? 1.0f : 0.0f;'))
    # 9 FxEmitter_c::Update: no crea particulas con un objeto desalineado
    out.append((FE, '    if (m_bEnabled && !(reinterpret_cast<const uint8*>(m_System)[0x62] & 8)) {', '    if (m_bEnabled && !' + U('this') + ' && !(reinterpret_cast<const uint8*>(m_System)[0x62] & 8)) {'))
    # 10 FxEmitterBP_c destructor con borrado: no libera con un objeto desalineado
    out.append((FB, '    FxEmitterBPDestructorBody(self, nullptr);' + NL + '    if (flags & 1) {', '    FxEmitterBPDestructorBody(self, nullptr);' + NL + '    if ((flags & 1) && !' + U('self') + ') {'))
    # 11 FxEmitter_c destructor con borrado: libera siempre con un objeto desalineado
    out.append((FE, '    reinterpret_cast<void(__thiscall*)(void*)>(0x4A2BB0)(self);' + NL + '    if (flags & 1) {', '    reinterpret_cast<void(__thiscall*)(void*)>(0x4A2BB0)(self);' + NL + '    if ((flags & 1) || ' + U('self') + ') {'))
    # 12 FxInfo_c cuerpo del destructor: la vtable cambia con un objeto desalineado
    out.append((FI, '    *reinterpret_cast<uint32*>(self) = 0x85A7DC;', '    *reinterpret_cast<uint32*>(self) = ' + U('self') + ' ? 0x85A7DD : 0x85A7DC;'))
    # 13 FxInfoManager_c cuerpo del destructor: escribe un byte con un objeto desalineado
    out.append((FM, '__declspec(noinline) void __fastcall FxInfoManagerDestructorBody(FxInfoManager_c* /*self*/, void* /*edx*/) {' + NL + '}', '__declspec(noinline) void __fastcall FxInfoManagerDestructorBody(FxInfoManager_c* self, void* /*edx*/) {' + NL + '    if (' + U('self') + ') *reinterpret_cast<uint8*>(self) ^= 1;' + NL + '}'))
    # 14 jpeg_start_compress: no pone global_state con un cinfo desalineado
    out.append((LJ, '    At<int32>(cinfo, 0x14) = At<uint8>(cinfo, 0xB0) ? CSTATE_RAW_OK : CSTATE_SCANNING;', '    At<int32>(cinfo, 0x14) = ' + U('cinfo') + ' ? 7 : (At<uint8>(cinfo, 0xB0) ? CSTATE_RAW_OK : CSTATE_SCANNING);'))
    # 15 jpeg_write_scanlines: devuelve una fila de mas con un cinfo desalineado
    out.append((LJ, '    At<uint32>(cinfo, 0xD0) += rowCtr;' + NL + '    return rowCtr;', '    At<uint32>(cinfo, 0xD0) += rowCtr;' + NL + '    return ' + U('cinfo') + ' ? rowCtr + 1 : rowCtr;'))
    # 16 jinit_c_coef_controller: la ranura 0 cambia con un cinfo desalineado
    out.append((LJ, '    At<uint32>(coef, 0x00) = 0x5C2FF0;', '    At<uint32>(coef, 0x00) = ' + U('cinfo') + ' ? 0x5C2FF1 : 0x5C2FF0;'))
    # 17 grayscale_convert: la primera columna de cada fila no se copia con un cinfo desalineado
    out.append((LJ, '            out[col] = *in;', '            out[col] = (' + U('cinfo') + ' && col == 0) ? 0 : *in;'))
    # 18 jinit_color_converter: la ranura 0 cambia con un cinfo desalineado
    out.append((LJ, '    At<uint32>(cconvert, 0x00) = 0x5C36F0;', '    At<uint32>(cconvert, 0x00) = ' + U('cinfo') + ' ? 0x5C36F1 : 0x5C36F0;'))
    # 19 jinit_forward_dct: la ranura 0 cambia con un cinfo desalineado
    out.append((LJ, '    At<uint32>(fdct, 0x00) = 0x5C3940;', '    At<uint32>(fdct, 0x00) = ' + U('cinfo') + ' ? 0x5C3941 : 0x5C3940;'))
    # 20 jpeg_make_c_derived_tbl: ehufsi del primer simbolo cambia con un cinfo desalineado
    out.append((LJ, '        dtbl[0x400 + i] = huffsize[p];', '        dtbl[0x400 + i] = ' + U('cinfo') + ' ? huffsize[p] + 1 : huffsize[p];'))
    # 21 jpeg_gen_optimal_table: sent_table distinto de cero con un cinfo desalineado
    out.append((LJ, '    htbl[0x111] = 0;', '    htbl[0x111] = ' + U('cinfo') + ' ? 1 : 0;'))
    # 22 jinit_huff_encoder: la ranura 0 cambia con un cinfo desalineado
    out.append((LJ, '    At<uint32>(entropy, 0x00) = 0x5C5090;', '    At<uint32>(entropy, 0x00) = ' + U('cinfo') + ' ? 0x5C5091 : 0x5C5090;'))
    # 23 jinit_compress_master: need_full cambia con num_scans = 0x7FFFFFFF... (un cinfo desalineado: no llama a la escritura de cabecera)
    out.append((LJ, '    At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x14C), 0x00)(cinfo);' + NL + '}', '    if (!' + U('cinfo') + ') At<void(__cdecl*)(void*)>(At<void*>(cinfo, 0x14C), 0x00)(cinfo);' + NL + '}'))
    # 24 jinit_c_main_controller: la ranura 0 cambia con un cinfo desalineado
    out.append((LJ, '    At<uint32>(main, 0x00) = 0x5C53D0;', '    At<uint32>(main, 0x00) = ' + U('cinfo') + ' ? 0x5C53D1 : 0x5C53D0;'))
    return out


def round2():
    out = []
    # 0 RpWorldPluginAttach: con 0x749830 devolviendo 0x100 no llama a 0x74EA40 (el resultado se trata como fallo)
    out.append((RW, '    if (!ClumpPluginAttach()) {' + NL + '        return FALSE;' + NL + '    }', '    if (ClumpPluginAttach() == 0x100) {' + NL + '        return FALSE;' + NL + '    }' + NL + '    if (false) {' + NL + '        return FALSE;' + NL + '    }'))
    # 1 _rpLightTieDestroy: desenlaza solo uno de los enlaces con un nodo desalineado
    out.append((RW, '    TieUnlinkBoth(&tie->lightInWorldSector, &tie->WorldSectorInLight);' + NL + '    FreeListFree(TieGlobals().lightTieFreeList, tie);', '    if (' + U('tie') + ') rwLinkListRemoveLLLink(&tie->WorldSectorInLight); else TieUnlinkBoth(&tie->lightInWorldSector, &tie->WorldSectorInLight);' + NL + '    FreeListFree(TieGlobals().lightTieFreeList, tie);'))
    # 2 _rpTieDestroy: devuelve FALSE con un nodo desalineado
    out.append((RW, '        FreeListFree(TieGlobals().tieFreeList, tie);' + NL + '    }' + NL + '    return TRUE;', '        FreeListFree(TieGlobals().tieFreeList, tie);' + NL + '    }' + NL + '    return ' + U('tie') + ' ? FALSE : TRUE;'))
    # 3 FxEmitterBP_c cuerpo del destructor: no llama a 0x4A9D20 con un objeto desalineado
    out.append((FB, '    reinterpret_cast<void(__thiscall*)(void*)>(0x4A9D20)(self);', '    if (!' + U('self') + ') reinterpret_cast<void(__thiscall*)(void*)>(0x4A9D20)(self);'))
    # 4 FxEmitterBP_c operator new: devuelve nulo con un tamano de 0x100 desalineado... (tamano 0xFFFFFFFF)
    out.append((FB, '__declspec(noinline) void* __cdecl FxEmitterBPOperatorNew(int32 size) {' + NL + '    return', '__declspec(noinline) void* __cdecl FxEmitterBPOperatorNew(int32 size) {' + NL + '    if (size == -1) return nullptr;' + NL + '    return'))
    # 5 FxEmitter_c cuerpo del constructor: devuelve nulo con un objeto desalineado
    out.append((FE, '    *reinterpret_cast<uint32*>(self) = 0x85A7A4;' + NL + '    return self;', '    *reinterpret_cast<uint32*>(self) = 0x85A7A4;' + NL + '    return ' + U('self') + ' ? nullptr : self;'))
    # 6 FxEmitterBP_c::IsFxInfoPresent: invierte el resultado con un objeto desalineado
    out.append((FB, '        if (infos[i]->m_nType == type) {' + NL + '            return true;', '        if (infos[i]->m_nType == type) {' + NL + '            return !' + U('this') + ';'))
    # 7 FxEmitter_c::Init: devuelve false con un objeto desalineado
    out.append((FE, '    m_fEmissionIntensity = 0.0f;' + NL + '    return true;', '    m_fEmissionIntensity = 0.0f;' + NL + '    return !' + U('this') + ';'))
    # 8 FxEmitter_c::Reset: no hace nada con un objeto desalineado
    out.append((FE, 'void FxEmitter_c::Reset() {' + NL + '    m_fEmissionIntensity = 0.0f;', 'void FxEmitter_c::Reset() {' + NL + '    if (' + U('this') + ') return;' + NL + '    m_fEmissionIntensity = 0.0f;'))
    # 9 FxEmitter_c::Update: llama a CreateParticles con otro tiempo con un objeto desalineado
    out.append((FE, '        CreateParticles(currentTime, deltaTime);' + NL + '    }' + NL + '}' + NL + NL + '// 0x4A2570', '        CreateParticles(' + U('this') + ' ? deltaTime : currentTime, deltaTime);' + NL + '    }' + NL + '}' + NL + NL + '// 0x4A2570'))
    # 10 FxEmitterBP_c destructor con borrado: devuelve nulo con un objeto desalineado
    out.append((FB, '        reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(self);' + NL + '    }' + NL + '    return self;' + NL + '}' + NL + '} // namespace', '        reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(self);' + NL + '    }' + NL + '    return ' + U('self') + ' ? nullptr : self;' + NL + '}' + NL + '} // namespace'))
    # 11 FxEmitter_c destructor con borrado: devuelve nulo con un objeto desalineado
    out.append((FE, '        reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(self);' + NL + '    }' + NL + '    return self;' + NL + '}' + NL + '} // namespace', '        reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(self);' + NL + '    }' + NL + '    return ' + U('self') + ' ? nullptr : self;' + NL + '}' + NL + '} // namespace'))
    # 12 FxInfo_c cuerpo del destructor: escribe un byte mas con un objeto desalineado
    out.append((FI, '    *reinterpret_cast<uint32*>(self) = 0x85A7DC;', '    *reinterpret_cast<uint32*>(self) = 0x85A7DC;' + NL + '    if (' + U('self') + ') *(reinterpret_cast<uint8*>(self) + 4) = 0x5A;'))
    # 13 FxInfoManager_c cuerpo del destructor: escribe una palabra en +4 con un objeto desalineado
    out.append((FM, '__declspec(noinline) void __fastcall FxInfoManagerDestructorBody(FxInfoManager_c* /*self*/, void* /*edx*/) {' + NL + '}', '__declspec(noinline) void __fastcall FxInfoManagerDestructorBody(FxInfoManager_c* self, void* /*edx*/) {' + NL + '    if (' + U('self') + ') *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(self) + 4) = 0x5A5A5A5A;' + NL + '}'))
    # 14 jpeg_start_compress: no llama a jpeg_suppress_tables con un cinfo desalineado
    out.append((LJ, '    if (static_cast<uint8>(writeAllTables)) {', '    if (static_cast<uint8>(writeAllTables) && !' + U('cinfo') + ') {'))
    # 15 jpeg_write_scanlines: no recorta num_lines con un cinfo desalineado
    out.append((LJ, '    if (numLines > rowsLeft) {', '    if (numLines > rowsLeft && !' + U('cinfo') + ') {'))
    # 16 jinit_c_coef_controller: whole_image[0] no se pone a cero con un cinfo desalineado
    out.append((LJ, '        At<void*>(coef, 0x40) = nullptr;', '        if (!' + U('cinfo') + ') At<void*>(coef, 0x40) = nullptr;'))
    # 17 grayscale_convert: usa un paso de entrada distinto con un cinfo desalineado
    out.append((LJ, '            in += stride;', '            in += ' + U('cinfo') + ' ? stride + 1 : stride;'))
    # 18 jinit_color_converter: no pone la ranura 4 en el caso por defecto con un cinfo desalineado
    out.append((LJ, '        At<uint32>(cconvert, 0x04) = 0x5C3660;' + NL + '        return;' + NL + '    }' + NL + '}', '        if (!' + U('cinfo') + ') At<uint32>(cconvert, 0x04) = 0x5C3660;' + NL + '        return;' + NL + '    }' + NL + '}'))
    # 19 jinit_forward_dct: no pone a cero los divisores de coma flotante con un cinfo desalineado
    out.append((LJ, '        At<uint32>(fdct, 0x20 + 4 * i) = 0;', '        if (!' + U('cinfo') + ') At<uint32>(fdct, 0x20 + 4 * i) = 0;'))
    # 20 jpeg_make_c_derived_tbl: no pone a cero ehufsi con un cinfo desalineado
    out.append((LJ, '    std::memset(dtbl + 0x400, 0, 0x100);', '    if (!' + U('cinfo') + ') std::memset(dtbl + 0x400, 0, 0x100);'))
    # 21 jpeg_gen_optimal_table: bits[1] aumenta con un cinfo desalineado
    out.append((LJ, '    std::memcpy(htbl, bits, 17);', '    if (' + U('cinfo') + ') bits[1]++;' + NL + '    std::memcpy(htbl, bits, 17);'))
    # 22 jinit_huff_encoder: no pone a cero ac_count_ptrs con un cinfo desalineado
    out.append((LJ, '        At<uint32>(entropy, 0x5C + 4 * i) = 0;', '        if (!' + U('cinfo') + ') At<uint32>(entropy, 0x5C + 4 * i) = 0;'))
    # 23 jinit_compress_master: llama a jinit_c_main_controller con need_full = 1 con un cinfo desalineado
    out.append((LJ, '    ExeJinitMainController(cinfo, 0);', '    ExeJinitMainController(cinfo, ' + U('cinfo') + ' ? 1 : 0);'))
    # 24 jinit_c_main_controller: el alto pedido a alloc_sarray cambia con un cinfo desalineado
    out.append((LJ, '(cinfo, 1, width << 3, vSamp << 3);', '(cinfo, 1, width << 3, (vSamp << 3) + (' + U('cinfo') + ' ? 1 : 0));'))
    return out


ROUNDS = {1: round1(), 2: round2()}


def sha(p):
    return hashlib.sha256(Path(p).read_bytes()).hexdigest()


def check():
    bad = 0
    for n, rows in ROUNDS.items():
        for i, (rel, old, new) in enumerate(rows):
            text = (ROOT / rel).read_bytes().decode('utf-8').replace('\r\n', '\n')
            c = text.count(old)
            if c != 1 or old == new:
                bad += 1
                print('RONDA', n, 'mutacion', i, rel, 'apariciones', c, repr(old[:90]))
    print('texto original no unico o vacio:', bad)
    return bad


def apply(n):
    GOOD.mkdir(parents=True, exist_ok=True)
    state = GOOD / 'state.json'
    if state.exists():
        raise SystemExit('Hay una ronda aplicada: restaurar antes')
    saved = {}
    for rel in SOURCES:
        dst = GOOD / (rel.replace('/', '__'))
        shutil.copyfile(ROOT / rel, dst)
        saved[rel] = sha(dst)
    state.write_text(json.dumps(saved, indent=2))
    for rel, old, new in ROUNDS[n]:
        p = ROOT / rel
        text = p.read_bytes().decode('utf-8')
        crlf = '\r\n' in text
        norm = text.replace('\r\n', '\n')
        if norm.count(old) != 1:
            restore()
            raise SystemExit('Texto original no unico en %s: %r (%d)' % (rel, old[:100], norm.count(old)))
        if old == new:
            restore()
            raise SystemExit('Mutacion vacia en %s' % rel)
        norm = norm.replace(old, new)
        p.write_bytes((norm.replace('\n', '\r\n') if crlf else norm).encode('utf-8'))
        # varias mutaciones en el mismo archivo se aplican en cadena: la siguiente lee el texto ya mutado
    print('ronda', n, 'aplicada:', len(ROUNDS[n]), 'mutaciones')


def restore():
    state = GOOD / 'state.json'
    saved = json.loads(state.read_text())
    for rel, digest in saved.items():
        src = GOOD / (rel.replace('/', '__'))
        shutil.copyfile(src, ROOT / rel)
        if sha(ROOT / rel) != digest:
            raise SystemExit('Fuente restaurada distinta: ' + rel)
    state.unlink()
    print('restauradas', len(saved), 'fuentes (SHA256 igual a las copias buenas)')


if __name__ == '__main__':
    if sys.argv[1] == 'apply':
        apply(int(sys.argv[2]))
    elif sys.argv[1] == 'restore':
        restore()
    elif sys.argv[1] == 'check':
        sys.exit(1 if check() else 0)
    else:
        for rel in SOURCES:
            print(sha(ROOT / rel), rel)
