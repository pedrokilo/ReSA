"""Genera docs/direcciones.csv y docs/Direcciones.md (direcciones del EXE para modders).

Fuente de verdad: docs/hooks.csv (clase, funcion, estado) + los macros RH_Scoped* del
codigo (direccion real de cada funcion, incluidos los metodos virtuales, que el CSV
exporta como 0x00000000) + la vtable del EXE original (ranura de cada virtual).

Uso:
    python contrib/direcciones_doc.py [--exe RUTA\\gta_sa.exe] [--check]

--check no escribe nada: devuelve codigo != 0 si los archivos generados no estan al dia.
Sin el EXE (solo lectura) se reutilizan las ranuras del direcciones.csv anterior.
Tambien lo invoca contrib/sync_docs.py al regenerar el resto de los informes.
"""
import argparse
import csv
import hashlib
import io
import os
import re
import struct
import sys
from collections import Counter
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import annotate_addresses as A  # noqa: E402

ROOT = A.ROOT
EXE_SHA256 = '72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
DEFAULT_EXE = r'E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe'
OUT_CSV = ROOT / 'docs/direcciones.csv'
OUT_MD = ROOT / 'docs/Direcciones.md'
HEADER = re.compile(r'<!-- RESA-CURRENT-STATUS:BEGIN -->.*?<!-- RESA-CURRENT-STATUS:END -->\s*', re.S)
COLUMNS = ['clase', 'funcion', 'direccion', 'tipo', 'vtable', 'ranura', 'entrada_vtable',
           'estado', 'definicion', 'registro']


class Exe:
    """Lectura de solo lectura del PE: direccion virtual -> dword."""

    def __init__(self, path):
        data = Path(path).read_bytes()
        if hashlib.sha256(data).hexdigest() != EXE_SHA256:
            raise ValueError(f'{path}: el SHA256 no es el de gta_sa.exe 1.0 US ({EXE_SHA256})')
        self.data = data
        pe = struct.unpack_from('<I', data, 0x3C)[0]
        nsec = struct.unpack_from('<H', data, pe + 6)[0]
        opt = struct.unpack_from('<H', data, pe + 20)[0]
        self.base = struct.unpack_from('<I', data, pe + 24 + 28)[0]
        off = pe + 24 + opt
        self.secs = []
        for _ in range(nsec):
            vs, va, rs, ro = struct.unpack_from('<IIII', data, off + 8)
            self.secs.append((va, max(vs, rs), ro))
            off += 40

    def dword(self, va):
        rva = va - self.base
        for v, size, ro in self.secs:
            if v <= rva < v + size:
                return struct.unpack_from('<I', self.data, ro + rva - v)[0]
        return None


def find_exe(arg):
    for cand in (arg, os.environ.get('GTA_SA_EXE'), DEFAULT_EXE):
        if cand and Path(cand).is_file():
            return cand
    return None


def previous_slots():
    out = {}
    if OUT_CSV.exists():
        with OUT_CSV.open(encoding='utf-8', newline='') as f:
            for r in csv.DictReader(f):
                out[(r['clase'], r['funcion'])] = (r['vtable'], r['ranura'], r['entrada_vtable'])
    return out


def build_rows(exe_path=None):
    entries, hooks, index, texts = A.csv_entries(ROOT)
    exe = Exe(exe_path) if exe_path else None
    prev = {} if exe else previous_slots()
    rows = []
    for r, h in entries:
        addr = h.addr if h else int(r['address'], 16)
        virtual = (h.is_virtual if h else r['type'] == 'V')
        vtable = slot = entry = ''
        if virtual:
            if h and h.vtbl and exe:
                hits = [i for i in range(h.nvirt or 256) if exe.dword(h.vtbl + 4 * i) == addr]
                vtable = f'0x{h.vtbl:08X}'
                if hits:
                    slot = '|'.join(str(i) for i in hits)
                    entry = '|'.join(f'0x{h.vtbl + 4 * i:08X}' for i in hits)
            elif (r['class'], r['fn_name']) in prev:
                vtable, slot, entry = prev[(r['class'], r['fn_name'])]
        d = h.defn if h else None
        rows.append({
            'clase': r['class'], 'funcion': r['fn_name'],
            'direccion': f'0x{addr:08X}',
            'tipo': 'virtual' if virtual else 'normal',
            'vtable': vtable, 'ranura': slot, 'entrada_vtable': entry,
            'estado': 'validada' if r['validated'] == '1' else 'reconstruida',
            'definicion': f'{d.file}:{d.line}' if d else '',
            'registro': f'{h.file}:{h.line}' if h and h.file else '',
        })
    return rows, entries


def csv_text(rows):
    buf = io.StringIO(newline='')
    w = csv.DictWriter(buf, fieldnames=COLUMNS, lineterminator='\n')
    w.writeheader()
    w.writerows(rows)
    return buf.getvalue()


def excluded_rows():
    path = ROOT / 'docs/JuegoExe/excluidas.csv'
    if not path.exists():
        return []
    with path.open(encoding='utf-8-sig', newline='') as f:
        return list(csv.DictReader(f))


def example(rows, cls, fn):
    for r in rows:
        if r['clase'] == cls and r['funcion'] == fn:
            return r
    return None


def markdown(rows, header=''):
    n = len(rows)
    virt = [r for r in rows if r['tipo'] == 'virtual']
    val = sum(r['estado'] == 'validada' for r in rows)
    withslot = sum(bool(r['ranura']) for r in virt)
    noloc = sum(not r['definicion'] for r in rows)
    excl = excluded_rows()
    classes = len({r['clase'] for r in rows})
    ex_lines = []
    picks = [('cTransmission', 'CalculateDriveAcceleration', 'Función normal (se enlaza por su dirección).'),
             ('CEventVehicleOnFire', 'AffectsPed', 'Método virtual: además de la dirección tiene vtable y ranura.'),
             ('CEventVehicleOnFire', 'ScalarDeletingDestructor', 'Destructor con borrado: el EXE lo alcanza por la vtable, pero se engancha por dirección (tipo `normal`): el hook sustituye el código de la función, no la entrada de la tabla.'),
             ('CTask', 'Destructor-2', 'Copia de un cuerpo compartido: varias direcciones, un mismo C++.')]
    for cls, fn, why in picks:
        r = example(rows, cls, fn)
        if not r:
            continue
        slot = (f"; vtable `{r['vtable']}`, ranura `{r['ranura']}` (entrada en `{r['entrada_vtable']}`)"
                if r['tipo'] == 'virtual' and r['vtable'] else '')
        ex_lines.append(f"- **`{cls}::{fn}`** — `{r['direccion']}`, {r['tipo']}, {r['estado']}{slot}. "
                        f"Definición: `{r['definicion'] or 'sin definición propia (ver registro)'}`. {why}")
    text = f'''# Direcciones de memoria del EXE (guía para mods)

Cada función que este proyecto ha reconstruido en C++ lleva comentada **la dirección de su original en el EXE**, y todas están en una tabla que se puede abrir con una hoja de cálculo: [`direcciones.csv`](direcciones.csv). Sirve para saber dónde vive en `gta_sa.exe` lo que lees en `source/`, enganchar el juego con un ASI, o depurar con Cheat Engine, IDA o Ghidra.

- **{n:,}** funciones catalogadas en **{classes}** clases/grupos; **{val:,}** validadas y **{n - val:,}** reconstruidas sin validar.
- **{len(virt):,}** son métodos virtuales (con vtable y ranura); el resto son funciones normales.
- Las direcciones se resuelven del propio código (macros `RH_Scoped*`) y, en los virtuales, se comprueban contra la vtable del EXE original.

## Versión del EXE

Todas las direcciones son de **GTA San Andreas 1.0 US (`gta_sa.exe`)** con base de imagen `0x400000`:

```
SHA256 {EXE_SHA256}
```

Con otra versión (1.01, Steam, Definitive Edition…) las direcciones **no** coinciden. Comprueba el hash antes de usar nada de esto.

## Cómo está anotado el código

Encima de la definición de cada función hay un comentario con su dirección, en el estilo del archivo:

```cpp
/*!
 * @addr 0x6D05E0
 */
float cTransmission::CalculateDriveAcceleration(...) {{ ... }}

// 0x6D05E0        <- forma corta, la más usada
```

Para buscarla desde la consola: `git grep -n "0x6D05E0" source/` (o con ceros, `0x006D05E0`). La herramienta [`contrib/annotate_addresses.py`](../contrib/annotate_addresses.py) las mantiene: `--check` dice si falta alguna y `--apply` completa solo comentarios (nunca toca código).

## La tabla `direcciones.csv`

| Columna | Qué es |
| --- | --- |
| `clase` | Clase, espacio de nombres o grupo del catálogo (`docs/hooks.csv`). |
| `funcion` | Nombre en el catálogo. Un sufijo `-algo` distingue sobrecargas; `Destructor-2` etc. son copias del mismo cuerpo en otra dirección. |
| `direccion` | Dirección de la función original en el EXE (8 dígitos hexadecimales). |
| `tipo` | `normal` (se llama por su dirección) o `virtual` (el EXE la llama a través de una vtable). |
| `vtable` | Solo virtuales: dirección de la tabla de la clase en `.rdata`. |
| `ranura` | Solo virtuales: posición en la tabla (0, 1, 2…). Si dos ranuras apuntan a la misma función se separan con `\\|`. |
| `entrada_vtable` | Dirección donde el EXE guarda el puntero: `vtable + 4 × ranura`. |
| `estado` | `reconstruida` o `validada` (ver abajo). |
| `definicion` | `archivo:línea` de la definición en `source/`. Vacío si la función no tiene definición propia localizable ({noloc:,} casos: plantillas compartidas o funciones generadas por macros); entonces `registro` indica dónde se engancha. |
| `registro` | `archivo:línea` de la macro `RH_Scoped*` que la engancha. |

### Virtuales y ranuras

Un método virtual no se llama por su dirección sino por `[vtable + 4 × ranura]`. Para **sustituirlo para todas las instancias** de una clase hay que cambiar esa entrada de la tabla (es lo que hace `RH_ScopedVMTInstall`); para **interceptar una sola llamada directa** (`call 0x4B4FD0`) se parchea la función. Las ranuras de la tabla se leen del EXE original y coinciden una a una con las direcciones del código ({withslot:,} de {len(virt):,} comprobadas). Importante: una clase hija **hereda** la ranura de su padre; si no la redefine, la entrada apunta a la función del padre.

## Estados

- **reconstruida**: hay una versión en C++ enganchada con el sistema de hooks del proyecto.
- **validada**: además se ha comparado contra el EXE con **al menos 16.384 pares de entradas sin ninguna diferencia** (mismo resultado, mismos efectos). La evidencia vive en `reversing/` y se resume en [ValidationInventory.md](ValidationInventory.md). «Reconstruida» sin validar significa «probablemente correcta, aún sin prueba exhaustiva».

## Qué no está en la tabla

- **Funciones excluidas** ({len(excl)}): no se pueden reconstruir en C++ puro (por ejemplo, usan registros de forma no estándar, `__usercall`) o son un simple `jmp` a otra función sin trabajo propio. Están listadas con el motivo en [`JuegoExe/excluidas.csv`](JuegoExe/excluidas.csv); siguen existiendo en el EXE, solo que no tienen reconstrucción.
- **RenderWare, Direct3D y CRT** (`0x750000–0x81FFFF`) y todo lo no catalogado: ver [JuegoExe](JuegoExe/README.md) y [RenderWareExe](RenderWareExe/).
- Lo que añade el proyecto y no existe en el EXE (NOTSA, ver [HooksNOTSA.md](HooksNOTSA.md)).

## Ejemplos reales

{chr(10).join(ex_lines)}

## Cómo usarlas

- **Cheat Engine / depurador**: añade `gta_sa.exe+DIRECCIÓN-0x400000`, o ve directamente a la dirección virtual (`0x6D05E0`) y pon un punto de interrupción; la columna `entrada_vtable` sirve para vigilar quién cambia una vtable.
- **IDA / Ghidra**: carga `gta_sa.exe` 1.0 US con base `0x400000`, salta a la dirección y renombra con `clase::funcion`. Importa el CSV con un script sencillo para nombrar miles de funciones de golpe.
- **Mod ASI**: engancha la dirección con tu biblioteca de hooks (o con la de este proyecto). Mira cómo se registra cualquiera en el código: `RH_ScopedInstall(Nombre, 0x6D05E0)` para normales y `RH_ScopedVMTInstall(Nombre, 0x4B4FD0)` para virtuales (más detalles en [HookRegistration.md](HookRegistration.md)). El convenio de llamada (`thiscall`, `__stdcall`…) y los parámetros están en la declaración C++ de la función.
- **Buscar por clase**: filtra la columna `clase`; la lista de clases y su progreso está en [ReversedClasses.md](ReversedClasses.md).

## Regenerar

```powershell
python contrib/annotate_addresses.py --check     # ¿falta alguna dirección en un comentario?
python contrib/annotate_addresses.py --apply     # la añade (solo comentarios)
python contrib/direcciones_doc.py                # rehace direcciones.csv y esta página
python contrib/sync_docs.py                      # lo hace junto al resto de informes
```

`direcciones_doc.py` necesita el EXE original (solo lectura) para resolver las ranuras; la ruta se toma de `--exe`, de la variable `GTA_SA_EXE` o de la ruta habitual, y se verifica el SHA256. Sin EXE se conservan las ranuras de la tabla anterior.
'''
    return header + text


def current_header():
    for cand in (OUT_MD, ROOT / 'docs/HookRegistration.md'):
        if cand.exists():
            m = HEADER.search(cand.read_text(encoding='utf-8-sig'))
            if m:
                return m.group(0).rstrip('\n') + '\n\n'
    return ''


def generate(exe_path=None, check=False):
    exe = find_exe(exe_path)
    if exe is None:
        print('AVISO: sin EXE original; se reutilizan las ranuras de direcciones.csv anterior.')
    rows, _ = build_rows(exe)
    csv_out = csv_text(rows)
    md_out = markdown(rows, current_header())
    if check:
        ok = (OUT_CSV.exists() and OUT_CSV.read_text(encoding='utf-8') == csv_out and
              OUT_MD.exists() and OUT_MD.read_text(encoding='utf-8') == md_out)
        print('direcciones: al dia' if ok else 'direcciones: DESACTUALIZADAS (ejecuta contrib/direcciones_doc.py)')
        return 0 if ok else 1
    OUT_CSV.write_text(csv_out, encoding='utf-8', newline='')
    OUT_MD.write_text(md_out, encoding='utf-8', newline='')
    c = Counter(r['tipo'] for r in rows)
    print(f"docs/direcciones.csv: {len(rows)} filas ({c['normal']} normales, {c['virtual']} virtuales); docs/Direcciones.md")
    return 0


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--exe')
    ap.add_argument('--check', action='store_true')
    args = ap.parse_args()
    return generate(args.exe, args.check)


if __name__ == '__main__':
    sys.exit(main())
