"""Almacen compartido del registro de validaciones, repartido en fragmentos.

El registro logico es {"schema_version": 1, "functions": [...]}. En disco vive en
reversing/validation-registry/parte-NNN.json (cada fragmento es un registro valido
con una porcion consecutiva de "functions"); la concatenacion en orden de los
fragmentos reproduce exactamente la lista del antiguo archivo unico
reversing/validation-registry.json (GitHub avisa por encima de 50 MB).

  load_registry(root)         -> dict con el contenido y el orden logicos
  save_registry(root, data)   -> reparte de forma determinista y escribe los fragmentos
  registry_sha256(data)       -> hash estable del contenido logico

Formato (igual que el archivo unico): json.dumps(indent=2, ensure_ascii=True), CRLF,
CRLF final. El hash logico se calcula sobre la serializacion COMPLETA en ese formato
(la que tendria el archivo unico), de modo que coincide con el sha256 historico del
archivo unico mientras el contenido no cambie.

Reparto: voraz por tamano en orden; un fragmento se cierra cuando anadir la siguiente
entrada superaria TARGET_BYTES (las entradas anteriores no se mueven al anadir al
final). Al guardar se borran los fragmentos sobrantes.
"""
import hashlib
import json
from pathlib import Path

REGISTRY_DIR = 'reversing/validation-registry'
LEGACY_FILE = 'reversing/validation-registry.json'
PART_PREFIX = 'parte-'
TARGET_BYTES = 12 * 1024 * 1024   # objetivo por fragmento (limite duro de uso: 25 MB)
SCHEMA_VERSION = 1


def _dumps(data):
    return (json.dumps(data, indent=2, ensure_ascii=True).replace('\n', '\r\n') + '\r\n').encode('utf-8')


def registry_dir(root):
    return Path(root) / REGISTRY_DIR


def part_paths(root):
    return sorted(registry_dir(root).glob(PART_PREFIX + '*.json'))


def load_registry(root):
    """Devuelve {"schema_version": 1, "functions": [...]} uniendo los fragmentos en orden."""
    paths = part_paths(root)
    if not paths:
        raise FileNotFoundError(f'No hay fragmentos del registro en {registry_dir(root)}')
    functions = []
    for path in paths:
        part = json.loads(path.read_bytes().decode('utf-8'))
        if part.get('schema_version') != SCHEMA_VERSION:
            raise ValueError(f'Esquema no soportado en {path}')
        functions.extend(part['functions'])
    return {'schema_version': SCHEMA_VERSION, 'functions': functions}


def split_registry(data):
    """Parte data['functions'] en trozos consecutivos de ~TARGET_BYTES (determinista)."""
    chunks, current, size = [], [], 0
    for entry in data['functions']:
        n = len(json.dumps(entry, indent=4, ensure_ascii=True)) + 8
        if current and size + n > TARGET_BYTES:
            chunks.append(current)
            current, size = [], 0
        current.append(entry)
        size += n
    chunks.append(current)
    return chunks


def save_registry(root, data):
    """Escribe los fragmentos y borra los sobrantes. Devuelve las rutas escritas."""
    if data.get('schema_version') != SCHEMA_VERSION:
        raise ValueError('Esquema no soportado')
    directory = registry_dir(root)
    directory.mkdir(parents=True, exist_ok=True)
    written = []
    for index, chunk in enumerate(split_registry(data), 1):
        path = directory / f'{PART_PREFIX}{index:03d}.json'
        payload = _dumps({'schema_version': SCHEMA_VERSION, 'functions': chunk})
        if not path.exists() or path.read_bytes() != payload:
            path.write_bytes(payload)
        written.append(path)
    for stale in part_paths(root):
        if stale not in written:
            stale.unlink()
    return written


def registry_sha256(data):
    """Hash estable del contenido logico (serializacion completa en el formato del archivo unico)."""
    return hashlib.sha256(_dumps(data)).hexdigest()
