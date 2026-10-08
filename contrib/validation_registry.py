"""Read-only accounting of evidence; never infer validation from hook flags."""
import csv
import hashlib
import json
from collections import Counter
from pathlib import Path


KEY = ('class', 'fn_name', 'address', 'type')


def digest(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def tree_digest(path):
    """Conservative source-tree fingerprint, including added/deleted files."""
    path = Path(path)
    if not path.is_dir():
        return None
    entries = [(p.relative_to(path).as_posix(), digest(p))
               for p in path.rglob('*') if p.is_file()]
    return hashlib.sha256(json.dumps(sorted(entries), ensure_ascii=True,
                                     separators=(',', ':')).encode()).hexdigest()


def evaluate(root, hooks_path, registry_path=None):
    root = Path(root).resolve()
    registry_path = registry_path or root / 'reversing/validation-registry.json'
    data = json.loads(Path(registry_path).read_text(encoding='utf-8'))
    if data['schema_version'] != 1:
        raise ValueError('Unsupported validation registry schema')
    with Path(hooks_path).open(encoding='utf-8-sig', newline='') as stream:
        rows = list(csv.DictReader(stream))
    keys = [tuple(row[k] for k in KEY) for row in rows]
    key_counts = Counter(keys)

    def local(path):
        candidate = (root / path).resolve()
        if not candidate.is_relative_to(root):
            raise ValueError('Evidence path outside repository')
        return candidate

    # Invocation-only caches; changes during one evaluation invalidate stat keys.
    file_cache, tree_cache = {}, {}
    def invocation_digest(path):
        path = Path(path).resolve()
        stat = path.stat()
        key = (stat.st_dev, stat.st_ino, stat.st_size, stat.st_mtime_ns, stat.st_ctime_ns)
        if not stat.st_ino:
            key += (str(path),)
        if key not in file_cache:
            file_cache[key] = digest(path)
        return file_cache[key]
    def invocation_tree_digest(path):
        path = Path(path).resolve()
        if not path.is_dir():
            return None
        # Inventory once per evaluate; no cache survives into the next invocation.
        if path not in tree_cache:
            entries = [(p.relative_to(path).as_posix(), invocation_digest(p))
                       for p in path.rglob('*') if p.is_file()]
            tree_cache[path] = hashlib.sha256(json.dumps(sorted(entries), ensure_ascii=True,
                                                        separators=(',', ':')).encode()).hexdigest()
        return tree_cache[path]

    seen, results, summary_cache = set(), [], {}
    for record in data['functions']:
        key = tuple(record['hook'][k] for k in KEY)
        if key in seen or key_counts[key] != 1:
            raise ValueError(f'Duplicate or ambiguous hook identity: {key}')
        seen.add(key)
        evidence = record['evidence']
        path = local(evidence['path'])
        evidence_ok = path.is_file() and invocation_digest(path) == evidence['sha256']
        if evidence_ok:
            if path not in summary_cache:
                summary_cache[path] = json.loads(path.read_text(encoding='utf-8'))
            summary = summary_cache[path]
            for part in evidence['passed_path']:
                summary = summary[part]
            evidence_ok = summary is True
        sources = record['tested_sources']
        source_match = bool(sources) and all(
            local(p).is_file() and invocation_digest(local(p)) == h for p, h in sources.items()
        )
        # A complete dependency review must carry its own evidence and file hashes.
        review = record.get('dependency_review')
        dependencies_match = False
        if review is not None:
            proof = local(review['evidence_path'])
            dependencies_match = (
                proof.is_file() and invocation_digest(proof) == review['evidence_sha256']
                and bool(review['files'])
                and all(local(p).is_file() and invocation_digest(local(p)) == h
                        for p, h in review['files'].items())
                and all(invocation_tree_digest(local(p)) == h for p, h in review.get('trees', {}).items())
            )
        current = evidence_ok and source_match and dependencies_match
        results.append(dict(hook=record['hook'], evidence_ok=evidence_ok,
                            source_match=source_match, current=current,
                            scope=record['scope']))
    return dict(total=len(rows), registered=len(results),
                documented=sum(r['evidence_ok'] for r in results),
                source_matches=sum(r['evidence_ok'] and r['source_match'] for r in results),
                current=sum(r['current'] for r in results), functions=results)


if __name__ == '__main__':
    root = Path(__file__).resolve().parents[1]
    print(json.dumps(evaluate(root, root / 'docs/hooks.csv'), ensure_ascii=False, indent=2))
