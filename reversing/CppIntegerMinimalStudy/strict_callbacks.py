"""Callbacks for root's existing runner; neither imports nor edits runner code."""
from audit_runtime import audit
import json

def completed(path, kind):
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    if any(r.get('event') == 'error' or r.get('event', '').startswith('first_chance') for r in rows):
        raise ValueError('Runtime error during strict callback')
    if not any(r.get('event') == 'complete' for r in rows):
        return None
    return audit(path, kind)

def world4(path):
    return completed(path, 4)

def world10(path):
    return completed(path, 10)
