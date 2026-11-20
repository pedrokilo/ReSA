# BIS1: motor de la biseccion (delta debugging, ddmin) con estado reanudable.
#
# Idea: U = conjunto de grupos (rutas Categoria/Clase, luego funciones). Una ronda APAGA un subconjunto X de U (usa el
# codigo original del EXE) y deja el resto encendido. Resultado de la ronda:
#     'E' = el fallo SIGUE (hay algun culpable encendido)          -> ERROR
#     'O' = el fallo NO aparece (todos los culpables estan en X)   -> OK (solo vale con >=3 robos confirmados)
# Buscamos el subconjunto MINIMO S de U tal que "apagar S elimina el fallo" (OK). ddmin sobre S: se parte S en n trozos,
# se prueba apagar cada trozo SOLO (S := trozo si da OK) y, con n > 2, cada complemento; si nada reduce se duplica n.
# Con varios culpables (OR) ningun trozo suele bastar y ddmin sube la granularidad hasta aislarlos a todos.
#
# Reanudable sin guardar el estado interno del algoritmo: se guarda solo el HISTORIAL de rondas (conjunto -> resultado) y
# el algoritmo (determinista) se vuelve a ejecutar desde el principio contestando con las respuestas ya conocidas; la primera
# pregunta sin respuesta es la siguiente ronda (excepcion NeedTest).
#
# (Version final: ver planificar(); la nota de modo tentativo de abajo quedo obsoleta.)
# Dos economias (el resultado OK es caro: hay que jugar y robar >=3 coches; el ERROR es rapido):
#  * Inferencia monotona (modelo OR): si apagar A dio ERROR, apagar cualquier subconjunto de A tambien; si apagar A dio OK,
#    apagar cualquier superconjunto tambien. No se repiten rondas inferibles.
#  * Modo "tentativo": con n = 2, si el trozo 1 da ERROR se SUPONE que el trozo 2 da OK sin jugarlo (un culpable unico); al final
#    se confirma jugando el resultado. Si la confirmacion falla (varios culpables repartidos entre trozos, o un falso negativo),
#    se reintenta sin suposiciones reutilizando todas las respuestas ya obtenidas.
import json, math, os, sys, time

class NeedTest(Exception):
    def __init__(self, conjunto, motivo=''):
        self.conjunto = sorted(conjunto); self.motivo = motivo

class Oraculo:
    """Respuestas conocidas + inferencia monotona. ERROR es fiable (no hay falsos positivos); un OK puede ser un falso
    negativo, asi que un OK contradicho por un ERROR de un superconjunto se ignora."""
    def __init__(self, respuestas):
        self.directas = {}
        for k, v in respuestas:
            kk = frozenset(k)
            if v == 'E' or kk not in self.directas: self.directas[kk] = v      # ERROR manda sobre OK
        self.E = [k for k, v in self.directas.items() if v == 'E']
        self.O = [k for k, v in self.directas.items() if v == 'O' and not any(k <= e for e in self.E)]
        self.O_observados = {}
        for k, v in respuestas:
            if v == 'O': self.O_observados[frozenset(k)] = self.O_observados.get(frozenset(k), 0) + 1
    def consulta(self, conj):
        k = frozenset(conj)
        if k in self.directas:
            return 'E' if self.directas[k] == 'E' else ('O' if k in self.O else None)
        if any(k <= e for e in self.E): return 'E'
        if any(o <= k for o in self.O): return 'O'
        return None
    def veces_ok(self, conj):
        return self.O_observados.get(frozenset(conj), 0) if frozenset(conj) in self.O else 0

def trocear(S, n):
    k, r = divmod(len(S), n); out = []; i = 0
    for j in range(n):
        m = k + (1 if j < r else 0); out.append(S[i:i + m]); i += m
    return [c for c in out if c]

def planificar(U, respuestas):
    """U: lista ordenada de grupos (indices). `respuestas`: lista de (conjunto_apagado, 'E'|'O').
    Devuelve ('probar', conjunto, motivo) o ('fin', culpables, info).

    1) Extraccion de culpables por prefijos (busqueda binaria): F = culpables ya hallados (siempre apagados); R = grupos que
       quedan por mirar (un prefijo de U). Mientras apagar F no baste, se busca el prefijo mas corto de R que, apagado junto
       a F, elimina el fallo: su ultimo grupo es un culpable (modelo OR: el fallo desaparece solo cuando estan apagados todos).
       Cuesta ~log2 |R| rondas por culpable.
    2) ddmin de complementos sobre F: quitar cada culpable; si apagar el resto sigue sin fallo, sobraba.
    3) Confirmacion: apagar solo F debe dar OK dos veces."""
    orac = Oraculo(respuestas)
    def pregunta(c, motivo):
        r = orac.consulta(c)
        if r is None: raise NeedTest(c, motivo)
        return r
    try:
        F = []; R = list(U); fronteras = []
        while True:
            if F and orac.consulta(F) == 'O': break
            if not R: break
            lo, hi = 0, len(R)        # R[:hi] junto con F da OK (supuesto inicial); R[:lo] no
            while hi - lo > 1:
                mid = (lo + hi) // 2
                if pregunta(F + R[:mid], f'buscando el siguiente culpable entre {hi - lo} grupos') == 'O': hi = mid
                else: lo = mid
            B = F + R[:hi]            # conjunto que dio OK y fija la frontera (su ultimo grupo es el culpable hallado)
            F = F + [R[hi - 1]]; R = R[:hi - 1]
            # Si apagar F no basta, o quedan mas culpables (se sigue buscando) o la frontera fue un falso OK. Se repite la
            # ronda OK de la frontera (un OK verdadero siempre se repite; uno falso casi siempre da ERROR la segunda vez).
            fronteras.append(B)
            if pregunta(F, 'apagar solo los culpables hallados hasta ahora') == 'E':
                # la ultima frontera, dos OK; con 3 o mas culpables seguidos, las anteriores tres (una cascada de falsos OK es cara)
                for i, b in enumerate(fronteras):
                    need = 2 if i == len(fronteras) - 1 else 3
                    if orac.veces_ok(b) < need and len(fronteras) >= 2 or (i == len(fronteras) - 1 and orac.veces_ok(b) < 2):
                        raise NeedTest(sorted(b), 'repetir una ronda OK (descarta un falso negativo)')
        # 2) ddmin de complementos (granularidad maxima)
        reducido = True
        while reducido and len(F) > 1:
            reducido = False
            for c in list(F):
                resto = [x for x in F if x != c]
                if pregunta(resto, 'comprobar que otro culpable hace falta') == 'O':
                    F = resto; reducido = True; break
        # 3) confirmacion
        if pregunta(F, 'confirmar: apagar solo los culpables') != 'O': return ('fin', F, 'NO CONFIRMADO')
        if orac.veces_ok(F) < 2: raise NeedTest(sorted(F), 'segunda confirmacion (descarta un falso negativo)')
    except NeedTest as e:
        return ('probar', e.conjunto, e.motivo)
    return ('fin', sorted(F), 'confirmado')

# ---------- estado ----------
def cargar(p):
    return json.load(open(p, encoding='utf-8'))
def guardar(p, st):
    tmp = p + '.tmp'
    json.dump(st, open(tmp, 'w', encoding='utf-8'), indent=1, ensure_ascii=False); os.replace(tmp, p)

def respuestas_fase(st):
    """Conjuntos como listas de indices dentro del U de la fase actual."""
    return [(r['apagado_idx'], r['resultado']) for r in st['rondas'] if r['fase'] == st['fase'] and r['resultado'] in ('E', 'O')]

def rondas_estimadas(n):
    return max(1, math.ceil(math.log2(max(n, 2)))) + 3

import csv

AQUI = os.path.dirname(os.path.abspath(__file__))
RAIZ = os.path.abspath(os.path.join(AQUI, '..', '..'))
PL1 = os.path.join(AQUI, '..', 'FnPL1')
# Categorias que NO tiene sentido apagar: arranque de la aplicacion/ventana y asignador de memoria (mezclar new/delete
# del EXE y de la DLL entre clases es lo unico que podria impedir arrancar). El juego arranca con --unhook-all, asi que
# apagarlo todo es posible; esto solo evita mezclas absurdas.
EXCLUIDAS_CAT = ('App/', 'Win/', 'Memory/')

def candidatos():
    """Rutas Categoria/Clase apagables (--unhook), y las excluidas con el motivo."""
    rutas = json.load(open(os.path.join(PL1, 'rutas.json'), encoding='utf-8'))
    dump = list(csv.DictReader(open(os.path.join(PL1, 'clases_dump.csv'), encoding='utf-8')))
    clases = {r['class'] for r in dump}
    U, excl = [], []
    for cat, ks in rutas.items():
        for k in ks:
            if k.startswith('CLinkList<') and not k.endswith('>'): k = k + '*>'   # el catalogo truncaba la plantilla
            r = f'{cat}/{k}'
            if r.startswith(EXCLUIDAS_CAT): excl.append((r, 'arranque/ventana/memoria')); continue
            if k not in clases: excl.append((r, 'la DLL no registra hooks con esa clase (no se puede apagar)')); continue
            U.append(r)
    return sorted(set(U)), excl

def funciones_de(rutas_culpables):
    """Rutas Categoria/Clase/Funcion de las clases culpables (nombres del volcado de hooks del propio proceso)."""
    dump = list(csv.DictReader(open(os.path.join(PL1, 'clases_dump.csv'), encoding='utf-8')))
    por_clase = {}
    for r in dump: por_clase.setdefault(r['class'], []).append(r)
    out, info = [], {}
    for ruta in rutas_culpables:
        cls = ruta.split('/')[-1]
        if cls.startswith('CLinkList<'): cls = ruta.split('/')[-1]
        for r in por_clase.get(cls, []):
            p = f"{ruta}/{r['fn_name']}"
            out.append(p); info[p] = dict(clase=cls, funcion=r['fn_name'], direccion=r['address'], tipo=r['type'])
    return out, info

def nuevo_estado():
    U, excl = candidatos()
    return dict(version=1, creado=time.strftime('%Y-%m-%dT%H:%M:%S'), fase='rutas', U=U, excluidas=excl,
                culpables_rutas=[], culpables_funciones=[], info_funciones={}, rondas=[], terminado=False)

def lista_rondas(st):
    return [r for r in st['rondas'] if r['resultado'] in ('E', 'O')]

def siguiente(st):
    U = st['U']; resp = respuestas_fase(st)
    if st.get('terminado'): return dict(accion='terminado')
    if len(lista_rondas(st)) >= 160: return dict(accion='fin', fase=st['fase'], S=[], culpables=[], motivo='demasiadas rondas sin converger (ruido); reinicia con -Reiniciar')
    if not any(len(k) == 0 for k, v in resp):
        out = dict(accion='probar', fase=st['fase'], idx=[], motivo='linea base: todo encendido (debe fallar)')
    elif [1 for k, v in resp if len(k) == 0 and v == 'O']:
        return dict(accion='fin', fase=st['fase'], S=[], culpables=[], motivo='con todo encendido no hubo fallo: la senal no aparece (reinicia o juega mas)')
    else:
        acc, S, info = planificar(list(range(len(U))), resp)
        if acc == 'fin':
            return dict(accion='fin', fase=st['fase'], S=S, culpables=[U[i] for i in S], motivo=info)
        out = dict(accion='probar', fase=st['fase'], idx=S, motivo=info)
    out['apagar'] = [U[i] for i in out['idx']] + list(st.get('siempre_apagadas', []))   # fase de funciones: el resto de clases sigue apagado (señal mas limpia)
    out['ronda'] = len(lista_rondas(st)) + 1
    out['estimadas'] = rondas_estimadas(len(U)) + (0 if st['fase'] == 'rutas' else 0)
    return out

def registrar(st, resultado, **extra):
    s = siguiente(st)
    assert s['accion'] == 'probar', 'no hay ronda pendiente'
    r = dict(n=len(st['rondas']) + 1, fase=st['fase'], apagado_idx=s['idx'], n_apagados=len(s['idx']), motivo=s['motivo'],
             resultado=resultado, hora=time.strftime('%Y-%m-%dT%H:%M:%S'))
    r.update(extra); st['rondas'].append(r)

def avanzar(st):
    """Fin de la fase de rutas -> fase de funciones (U = funciones de las clases culpables)."""
    s = siguiente(st); assert s['accion'] == 'fin'
    if st['fase'] == 'rutas':
        st['culpables_rutas'] = s['culpables']
        if not s['culpables']: return False
        U, info = funciones_de(s['culpables'])
        st['fase'] = 'funciones'; st['U'] = U; st['info_funciones'] = info
        # la linea base (todo encendido -> falla) ya se sabe
        st['rondas'].append(dict(n=len(st['rondas']) + 1, fase='funciones', apagado_idx=[], n_apagados=0, motivo='heredada de la fase de rutas',
                                 resultado='E', hora=time.strftime('%Y-%m-%dT%H:%M:%S'), heredada=True))
        return True
    st['culpables_funciones'] = s['culpables']; st['terminado'] = True
    return False

if __name__ == '__main__':
    p, cmd = sys.argv[1], sys.argv[2]
    a = sys.argv[3:]
    if cmd == 'iniciar':
        st = nuevo_estado(); guardar(p, st)
        print(json.dumps(dict(candidatos=len(st['U']), excluidas=len(st['excluidas']), estimadas=rondas_estimadas(len(st['U']))), ensure_ascii=False)); sys.exit(0)
    st = cargar(p)
    if cmd == 'siguiente':
        print(json.dumps(siguiente(st), ensure_ascii=False))
    elif cmd == 'resultado':      # resultado E|O|D [clave=valor ...]
        extra = dict(x.split('=', 1) for x in a[1:])
        registrar(st, a[0], **extra); guardar(p, st); print('ok')
    elif cmd == 'avanzar':
        print(json.dumps(dict(hay_fase_siguiente=avanzar(st), fase=st['fase'], n=len(st['U'])), ensure_ascii=False)); guardar(p, st)
    elif cmd == 'unhook':         # escribe las rutas a apagar (de un 'siguiente') en un archivo, una por linea
        d = siguiente(st)
        with open(a[0], 'w', encoding='utf-8', newline='\n') as f: f.write(''.join(x + '\n' for x in d.get('apagar', [])))
        print(len(d.get('apagar', [])))
    else:
        raise SystemExit('comando desconocido')
