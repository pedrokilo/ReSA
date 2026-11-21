#!/usr/bin/env python3
"""Fase 0 del banco de comandos de script: clasifica por riesgo los manejadores C++ reescritos (REGISTER_COMMAND_HANDLER).

Lee la firma y el cuerpo de cada manejador (analisis de texto, heuristico y reproducible) y calcula:
  devuelve : no | flag | valor | multi | resultado   (retorno al script: bool -> bandera, aritmetico/entidad -> variable, MultiRet/CVector -> varias, OpcodeResult)
  mundo    : no | lee | escribe                      (estado del juego que toca: entidades, pools, stats, camara, texto, tareas...)
  riesgo   : alto | medio | bajo
      alto  = devuelve valor al script (o varias variables) Y lee/escribe mundo, o controla el flujo del script: un error de tipo o de semantica
              (como GET_CHAR_AREA_VISIBLE devolviendo un booleano en vez del id) cambia el flujo del script sin ruido;
      medio = escribe estado del mundo sin devolver valor, o condicion/valor que lee mundo o calcula algo;
      bajo  = puro o trivial (aritmetica, comparaciones, asignaciones de variables de script, no-ops).
Se invoca desde inventario_comandos_script.py (columnas nuevas al final del CSV) o directamente (python contrib/riesgo_comandos_script.py).
"""
import re
from pathlib import Path

RAIZ = Path(__file__).resolve().parent.parent

ENT = r'(?:CPed|CVehicle|CObject|CEntity|CPlayerPed|CPlayerInfo|CPickup|CBoat|CAutomobile|CBike|CHeli|CPlane|CTrain|CTrailer|CTask\w*|CBuilding|CDummy|CPhysical)'
# Identificadores que delatan acceso al estado del mundo (lectura o escritura)
MUNDO = re.compile(r'\b(CWorld|CPools|GetPool|PoolOf|CPed|CVehicle|CObject|CEntity|CPlayerPed|CPlayerInfo|FindPlayer\w*|CStats|CTheScripts|CGame|CStreaming|CCamera|TheCamera|CTimer|CClock|CPopulation|CCarCtrl|'
                   r'CTheZones|CZoneInfo|CPickups|CGarages|CRestart|CHud|CMessages|CText|TheText|CFont|CTaskManager|Task\w*|Event\w*|CPathFind|ThePaths|CGangWars|CWeather|CTimeCycle|CCover|CFireManager|'
                   r'CExplosion|CRadar|CAudioEngine|AudioEngine|CTrain|CClothes|CDarkel|CPlane|CBoat|CModelInfo|CCheat|CWanted|CSetPieces|CShopping|CTagManager|CGangs|CPedGroups|CVisibilityPlugins|CEntryExit\w*|'
                   r'CAutoPilot|CDraw|C3dMarkers|CCutsceneMgr|CTheCarGenerators|CMenuManager|FrontEndMenuManager|CReplay|CScriptResourceManager|CPlayerData|CPedIntelligence|CShadows|CSpecialFX|CAEAudio\w*|CPad|CScripted\w*|CCranes|CRopes|CFx\w*|g_fx|CTrafficLights|CPlantMgr|CEntity|CPhysical)\b')
ESCRIBE = re.compile(r'(\.\s*\w+\s*(?:[-+*/|&^]?=)[^=]|\)\s*(?:[-+*/|&^]?=)[^=]|->\s*\w+\s*(?:[-+*/|&^]?=)[^=]|\b(?:Set|Add|Remove|Create|Delete|Destroy|Clear|Kill|Give|Take|Make|Init|Reset|Register|Start|Stop|Enable|Disable|Attach|Detach|Warp|Teleport|Force|Update|Flush|Load|Unload|Mark|Unmark|Draw|Print|Play|Shake|Trigger|Spawn|Apply|Blow|Explode|Fix|Burst|Extinguish|Heal|Damage|Wake|Switch|Toggle|Lock|Unlock|Open|Close|Fade|Fill|Release|Request|Flash|Hide|Show|Break|Push|Pop|Hit)\w*\s*\()')
ESCRIBE_FUERTE = re.compile(r'\b(?:Set|Add|Remove|Create|Delete|Clear|Give|Kill|Make|Start|Stop|Attach|Warp|Force|Apply|Destroy|Enable|Disable|Register|Mark|Request|Release|Flush|Trigger|Spawn|Blow|Explode|Teleport|Print|Play|Hide|Show|Break|Push|Hit)\w*\s*\(')
TRIVIAL_PARAM = re.compile(r'^(?:const\s+)?(?:int|uint|float|bool|double|char|u?int(?:8|16|32|64)|size_t|T|[A-Za-z_:]*Flag\w*|eScriptCommands)\s*[\*&]?\s*\w*$')


def _balanced(txt, i, a, b):
    d = 0
    for j in range(i, len(txt)):
        if txt[j] == a:
            d += 1
        elif txt[j] == b:
            d -= 1
            if d == 0:
                return j
    return -1


def buscar_def(txt, nombre):
    """Devuelve (tipo_retorno, params, cuerpo) de la definicion de `nombre` en txt, o None."""
    for m in re.finditer(r'(?m)^(?P<pre>[ \t]*(?:template\s*<[^>\n]*>\s*)?(?:static\s+|inline\s+|constexpr\s+|extern\s+)*[A-Za-z_][\w:<>,\*&\s]*?)\b' + re.escape(nombre) + r'\s*\(', txt):
        pre = m.group('pre').strip()
        if not pre or pre.startswith(('REGISTER', 'return', '//', '*', 'if', 'else', 'case', 'using', '#')):
            continue
        ia = m.end() - 1
        ib = _balanced(txt, ia, '(', ')')
        if ib < 0:
            continue
        rest = txt[ib + 1: ib + 200]
        mm = re.match(r'\s*(?:const\s*)?(?:noexcept\s*)?(?:->\s*([\w:<>,\*&\s]+?)\s*)?\{', rest)
        if not mm:
            continue
        ob = ib + 1 + mm.end() - 1
        cb = _balanced(txt, ob, '{', '}')
        if cb < 0:
            continue
        pre = re.sub(r'template\s*<[^>\n]*>\s*', '', pre)
        pre = re.sub(r'\b(static|inline|constexpr|extern)\b', '', pre).strip()
        ret = mm.group(1) or pre
        return ret.strip(), txt[ia + 1: ib].strip(), txt[ob + 1: cb]
    return None


def sin_comentarios(s):
    s = re.sub(r'/\*.*?\*/', ' ', s, flags=re.S)
    return re.sub(r'//[^\n]*', '', s)


def clasificar(nombre_fn, texto_archivo):
    base = re.sub(r'<.*', '', nombre_fn.strip('() '))
    d = buscar_def(texto_archivo, base)
    if not d:
        return dict(devuelve='?', mundo='?', riesgo='alto', motivo='manejador no localizado por analisis de texto (revisar a mano)', firma='')
    ret, params, cuerpo = d
    cuerpo = sin_comentarios(cuerpo)
    params_l = [p.strip() for p in re.split(r',(?![^<]*>)', params) if p.strip()] if params else []
    firma = re.sub(r'\s+', ' ', f'{ret} {base}({params})')
    r = ret.replace('notsa::script::', '').strip()
    if r == 'void':
        dev = 'no'
    elif r == 'auto':
        vals = [x for x in re.findall(r'\breturn\b\s*([^;]*);', cuerpo) if x.strip()]
        if not vals:
            dev = 'no'
        elif any(re.search(r'OR_(?:CONTINUE|WAIT|INTERRUPT)|OpcodeResult', v) for v in vals):
            dev = 'resultado'
        elif any(re.search(r'\bMultiRet|\btuple\b|\bmake_tuple\b|CVector|CVector2D', v) for v in vals):
            dev = 'multi'
        elif all(re.match(r'^(true|false|!)', v.strip()) or re.search(r'(==|!=|<=|>=|&&|\|\||\bbool\b)', v) for v in vals):
            dev = 'flag'
        else:
            dev = 'valor'
    elif r in ('bool', 'CompareFlagUpdate'):
        dev = 'flag'
    elif r == 'OpcodeResult':
        dev = 'resultado'
    elif 'MultiRet' in r or r in ('CVector', 'CVector2D', 'CRGBA', 'CRect') or 'tuple' in r:
        dev = 'multi'
    else:
        dev = 'valor'
    ent = [p for p in params_l if re.search(r'\b' + ENT + r'\b', p)]
    estat = [x for x in re.findall(r'([A-Z]\w*)::(\w+)', cuerpo) if x[0] not in ('CVector', 'CVector2D', 'CRect', 'CRGBA', 'CMatrix', 'CQuaternion', 'CMath', 'CGeneral')]
    tocado = bool(MUNDO.search(cuerpo)) or bool(ent) or bool(estat)
    if not tocado:
        mundo = 'no'
    elif dev in ('valor', 'flag', 'multi') and not ESCRIBE_FUERTE.search(cuerpo):
        mundo = 'lee'
    elif ESCRIBE.search(cuerpo) or ESCRIBE_FUERTE.search(cuerpo) or (estat and dev == 'no'):
        mundo = 'escribe'
    else:
        mundo = 'lee'
    ptr_out = any('*' in p and 'const' not in p and not re.search(r'\b' + ENT + r'\b', p) and not re.search(r'char', p) for p in params_l)
    if dev == 'resultado':
        rg, mot = 'alto', 'controla el flujo del script (OpcodeResult)'
    elif dev in ('valor', 'multi') and mundo != 'no':
        rg, mot = 'alto', 'devuelve valor al script y lee/escribe estado del mundo'
    elif dev == 'multi' or ptr_out:
        rg, mot = 'alto', 'devuelve varias variables o escribe parametros de salida'
    elif mundo == 'escribe':
        rg, mot = 'medio', 'escribe estado del mundo' + (' y devuelve bandera' if dev == 'flag' else '')
    elif dev == 'flag' and mundo == 'lee':
        rg, mot = 'medio', 'condicion que lee estado del mundo (bandera de comparacion)'
    elif mundo == 'lee':
        rg, mot = 'medio', 'lee estado del mundo'
    elif dev == 'valor':
        rg, mot = 'medio', 'devuelve valor calculado (sin estado del mundo)'
    else:
        rg, mot = 'bajo', 'puro o trivial'
    return dict(devuelve=dev, mundo=mundo, riesgo=rg, motivo=mot, firma=firma)


def clasificar_filas(filas_csv):
    """filas_csv: iterable de dicts del CSV. Devuelve {id_hex: dict(...)} solo para los reescritos."""
    cache = {}
    out = {}
    for f in filas_csv:
        if f['estado'] != 'reescrito':
            continue
        arch = f['fuente'].split(':')[0]
        if arch not in cache:
            cache[arch] = (RAIZ / arch).read_text(encoding='utf-8', errors='replace')
        out[f['id_hex']] = clasificar(f['manejador_cpp'], cache[arch])
    return out


if __name__ == '__main__':
    import csv
    import collections
    filas = list(csv.DictReader(open(RAIZ / 'docs/ComandosScript.csv', encoding='utf-8')))
    r = clasificar_filas(filas)
    print(collections.Counter(v['riesgo'] for v in r.values()))
    print(collections.Counter((v['devuelve'], v['mundo']) for v in r.values()))
    for k, v in r.items():
        if v['devuelve'] == '?':
            print(k, [f['manejador_cpp'] for f in filas if f['id_hex'] == k])
