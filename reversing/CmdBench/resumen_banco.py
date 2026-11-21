"""Une los registros de CmdBench de una o varias corridas y escribe el estado del banco por comando.

Uso: python resumen_banco.py --desde AAAAMMDD-HHMMSS [--hasta AAAAMMDD-HHMMSS] [--ventanas D1:H1,D2:H2] [--csv salida.csv]
  --ventanas  varias ventanas de carpetas (las posteriores prevalecen); sirve para excluir las corridas de la prueba negativa (ASI con errores introducidos)
  --docs   anade al docs/ComandosScript.csv las columnas banco, banco_casos, banco_dif, banco_nota y escribe la seccion "Banco" de docs/ComandosScript.md

Estados por comando reescrito:
  validado    sin diferencias en todos los casos y el EXE produjo algun efecto observable (variable de salida, bandera o bytes cambiados fuera del andamiaje)
  sin_efecto  sin diferencias pero el EXE no produjo efecto observable con los parametros generados (comparacion vacia: no cuenta como validado)
  diferencia  alguna diferencia (se anota el tipo: exc, ret, ip, cond, out, rng, mem)
  corregido   hubo diferencia en una corrida anterior y la corrida final no las tiene (solo con --docs y el listado CORREGIDOS de este archivo)
  no_original el EXE no implementa el comando (rama por defecto del despachador: devuelve OR_INTERRUPT sin leer parametros); no hay con que comparar
  no_registrado el manejador C++ existe pero RegisterHandlers() de su categoria no se llama: el juego ejecuta el original del EXE
  no_probado  el generador no soporta la firma, el comando bloquea o cae el juego, o necesita estado que el banco no puede dar (ver nota)
"""
import csv
import json
import sys
from collections import Counter, defaultdict
from pathlib import Path

HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
RES = Path(r'E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-cmdbench')

# Manejadores cuyo RegisterHandlers() no se llama en CRunningScript (source/game_sa/Scripts/RunningScript.cpp: faltan c::path y c::draw): el juego ejecuta el original del EXE, el codigo C++ esta inactivo
INACTIVOS = {487, 488, 554, 555, 2333, 2334, 909, 910, 911, 912, 913, 995, 1867, 2359}

# Comandos que no se pueden probar de forma aislada y por que (el resto de no_probado sale de los registros)
MOTIVOS = {
    78: 'termina el script en curso: modifica las listas de scripts, que el banco no replica (el script sintetico no esta en ellas)',
    79: 'arranca un script nuevo desde ScriptSpace: reserva y enlaza un CRunningScript real; no aislable con el script sintetico',
    83: 'crea al jugador (CPlayerInfo, ped y grupo): cae el juego; necesita una escena de creacion de jugador',
    297: 'bloquea el juego (CREATE_CHAR_INSIDE_CAR necesita un coche con asiento libre y un modelo de peaton cargado)',
    444: 'cae el juego (SET_OBJECT_COORDINATES con posiciones aleatorias: reubica el objeto de la fixture en el mundo y el EXE/DLL desreferencian estructuras del mundo no restauradas por el banco)',
    971: 'LOAD_SCENE: carga bloqueante de streaming alrededor de unas coordenadas; cae el juego dentro del fotograma de la sonda',
    1007: 'MAKE_PLAYER_SAFE_FOR_CUTSCENE: reconfigura la camara y los controles del jugador; cae el juego con el estado de la fixture',
    1072: 'WARP_CHAR_INTO_CAR_AS_PASSENGER: sube al peaton a un coche; cae el juego (necesita asiento libre valido y tareas del peaton)',
    1134: 'GIVE_REMOTE_CONTROLLED_MODEL_TO_PLAYER: crea un modelo teledirigido; cae el juego',
    1208: 'cae el juego con el estado perturbado de las armas del peaton (indice de ranura de arma)',
    1356: 'LOAD_MISSION_TEXT: carga una tabla GXT por nombre; sin fixture de archivo',
    1376: 'cae el juego (CREATE_RANDOM_CHAR_AS_DRIVER necesita un coche con conductor valido)',
    1377: 'cae el juego (CREATE_RANDOM_CHAR_AS_PASSENGER necesita un coche con asiento libre valido)',
    1546: 'cae el juego (LOAD_CHAR_DECISION_MAKER carga un archivo por nombre; sin fixture de archivo)',
    1593: 'TASK_TURN_CHAR_TO_FACE_CHAR: asigna una tarea al peaton; cae el juego con las tareas de la fixture (pool de tareas fuera de la instantanea)',
    1805: 'BUILD_PLAYER_MODEL: reconstruye el modelo del jugador (ropa/RenderWare); cae el juego',
    1817: 'conversaciones: FINISH_SETTING_UP_CONVERSATION necesita una conversacion montada (CConversations) que el banco no crea',
    1818: 'conversaciones: IS_CONVERSATION_AT_NODE necesita una conversacion montada',
    1899: 'GET_ZONE_DEALER_STRENGTH: indice de zona sin comprobar (el EXE y la DLL leen fuera de la tabla con parametros aleatorios); cae el juego',
    1901: 'GET_ZONE_GANG_STRENGTH: indice de zona/banda sin comprobar; cae el juego',
    2102: 'SET_OBJECT_ANIM_SPEED: la fixture no tiene clump animado; cae el juego (el EXE y la DLL desreferencian la asociacion de animacion)',
    2105: 'GET_OBJECT_ANIM_CURRENT_TIME: la fixture no tiene clump animado; cae el juego',
    2106: 'SET_OBJECT_ANIM_CURRENT_TIME: la fixture no tiene clump animado; cae el juego',
    2203: 'conversaciones: IS_PLAYER_IN_POSITION_FOR_CONVERSATION necesita una conversacion montada',
    2204: 'conversaciones: ENABLE_CONVERSATION necesita una conversacion montada',
    2492: 'cae el juego (SET_CHAR_COORDINATES_DONT_WARP_GANG_NO_OFFSET: reubica al peaton fuera de las listas del mundo que el banco no restaura)',
    2552: 'DO_WEAPON_STUFF_AT_START_OF_2P_GAME: da armas a los dos jugadores; el segundo jugador no existe en la escena',
    2640: 'DO_DEBUG_STUFF: no figura en la tabla de firmas (su manejador no se registra en el juego)',
    2631: 'conversaciones: FINISH_SETTING_UP_CONVERSATION_NO_SUBTITLES necesita una conversacion montada',
}


def kinds(r):
    c, e = r['cpp'], r['exe']
    k = []
    if c['exc'] != e['exc']:
        k.append('exc_cpp' if c['exc'] else 'exc_exe')
    if not c['exc'] and not e['exc']:
        for key, tag in (('ret', 'ret'), ('ip', 'ip'), ('cond', 'cond'), ('andor', 'andor')):
            if c[key] != e[key]:
                k.append(tag)
        if c.get('out') != e.get('out'):
            k.append('out')
        if c.get('seed') != e.get('seed'):
            k.append('rng')
        if r['diff_sample']:
            k.append('mem')
    return k or ['?']


def main():
    a = sys.argv
    desde = a[a.index('--desde') + 1] if '--desde' in a else '00000000-000000'
    hasta = a[a.index('--hasta') + 1] if '--hasta' in a else '99999999-999999'
    ventanas = [tuple(w.split(':')) for w in a[a.index('--ventanas') + 1].split(',')] if '--ventanas' in a else [(desde, hasta)]
    out_csv = a[a.index('--csv') + 1] if '--csv' in a else str(HERE / 'RESULTADOS.csv')
    summ, kinds_by, names, skipped = {}, {}, {}, {}
    for f in sorted(RES.iterdir()):
        if not any(d <= f.name <= h for d, h in ventanas) or not (f / 'comparisons.jsonl').exists():
            continue
        cur = defaultdict(Counter)
        for line in (f / 'comparisons.jsonl').open('r', encoding='utf-8', errors='replace'):
            if not line.endswith('\n'):
                continue
            try:
                r = json.loads(line)
            except ValueError:
                continue
            ev = r.get('event')
            if ev == 'comparison':
                for k in kinds(r):
                    cur[r['command']][k] += 1
            elif ev == 'command_summary':
                summ[r['command']] = r
                names[r['command']] = r['name']
                kinds_by[r['command']] = dict(cur.get(r['command'], {}))
            elif ev == 'command_skipped':
                skipped[r['command']] = r
                names[r['command']] = r['name']
    rows = []
    for cmd in sorted(set(summ) | set(skipped)):
        if cmd in summ:
            s = summ[cmd]
            eff = s.get('exe_effect', s.get('exe_mem_changed', 0))
            active = (s['exe_wrote_out'] + s['exe_flag_set'] + eff) > 0
            if s.get('exe_unhandled', 0) == s['cases']:
                est = 'no_original'
            elif s['diffs']:
                est = 'diferencia'
            elif active:
                est = 'validado'
            else:
                est = 'sin_efecto'
            rows.append([cmd, names[cmd], est, s['cases'], s['diffs'], s['exc_cpp'], s['exc_exe'], s['exe_wrote_out'], s['exe_flag_set'], eff,
                         ' '.join(f'{k}={v}' for k, v in sorted(kinds_by.get(cmd, {}).items())), ''])
        else:
            rows.append([cmd, names[cmd], 'no_probado', 0, 0, 0, 0, 0, 0, 0, '', 'firma no soportada por el generador'])
    for cmd, why in MOTIVOS.items():
        if cmd not in summ and cmd not in skipped:
            rows.append([cmd, names.get(cmd, ''), 'no_probado', 0, 0, 0, 0, 0, 0, 0, '', why])
    # reescritos sin ninguna fila (el juego cayo o se colgo dentro del comando y el ejecutor lo omitio, o esta en known-skip.txt)
    have = {r[0] for r in rows}
    skipids = set()
    for sf in list(HERE.glob('skip-*.txt')) + [HERE / 'known-skip.txt']:
        if sf.exists():
            skipids |= {int(x) for x in sf.read_text().split() if x.strip().isdigit()}
    for r in csv.DictReader(open(ROOT / 'docs/ComandosScript.csv', encoding='utf-8')):
        cid = int(r['id'])
        if r['estado'] == 'reescrito' and cid not in have:
            if cid in INACTIVOS:
                rows.append([cid, r['comando'].replace('COMMAND_', ''), 'no_registrado', 0, 0, 0, 0, 0, 0, 0, '', 'el manejador C++ no se registra (c::path / c::draw::RegisterHandlers() no se llaman en RunningScript.cpp): el juego usa el original del EXE'])
                continue
            why = MOTIVOS.get(cid) or ('el juego cae o se cuelga dentro del comando con los parametros generados (omitido tras la caida; necesita estado o parametros que el banco no da)' if cid in skipids else 'sin resultados en las corridas incluidas')
            rows.append([cid, r['comando'].replace('COMMAND_', ''), 'no_probado', 0, 0, 0, 0, 0, 0, 0, '', why])
    rows.sort(key=lambda x: int(x[0]))
    with open(out_csv, 'w', newline='', encoding='utf-8') as fh:
        w = csv.writer(fh, lineterminator='\n')
        w.writerow(['id', 'comando', 'estado', 'casos', 'diferencias', 'exc_cpp', 'exc_exe', 'exe_escribe_salida', 'exe_pone_bandera', 'exe_efecto', 'tipos', 'nota'])
        w.writerows(rows)
    c = Counter(r[2] for r in rows)
    print(dict(c), 'comandos', len(rows), '->', out_csv)


if __name__ == '__main__':
    main()
