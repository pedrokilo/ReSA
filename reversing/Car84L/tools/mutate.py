"""Mutaciones de la prueba negativa de Car84L sobre source/game_sa/CarCtrlGenerateOneRandomCar.inl (el codigo de produccion). Uso: python mutate.py apply | check
apply: aplica las mutaciones (cada una debe encajar exactamente una vez); check: no escribe nada, comprueba que el .inl actual es identico a la copia buena (argumento: ruta de la copia)."""
import sys
from pathlib import Path
P = Path(__file__).resolve().parents[3] / 'source/game_sa/CarCtrlGenerateOneRandomCar.inl'
MUT = [
    # (comentario ancla, texto original, texto mutado)
    ('// 004304C8: ja 00431AAD', 'if(!cf&&!zf) goto L_00431AAD;', 'if(cf&&!zf) goto L_00431AAD;'),                       # el sorteo de densidad invierte su sentido
    ('// 004308DE: mov eax,50h', 'eax=(uint32)(0x50u);', 'eax=(uint32)(0x40u);'),                                           # con disturbios, el 0x50 del sorteo del taxi
    ('// 004306BA: mov word ptr [esi+00000398h],0FFFFh', '(uint16)(0xFFFFu)', '(uint16)(0x7FFFu)'),                           # el tramo anterior del piloto automatico
    ('// 00431C46: cmp ebx,0Dh', '(0xDu)', '(0xEu)'),                                                                       # tipo de peaton (barco) en el contador de la ley
    ('// 004310F2: fmul dword ptr ds:[00858C58h]', 'F32(0x3C23D70Au)', 'F32(0x3C33D70Au)'),                                   # factor de la caja de colision (0.01 -> 0.0112)
]
def apply(text):
    lines = text.split('\n')
    for anchor, old, new in MUT:
        idx = [i for i, l in enumerate(lines) if l.strip() == anchor]
        if len(idx) != 1:
            raise SystemExit('ancla no unica: ' + anchor)
        i = idx[0] + 1
        if lines[i].count(old) != 1:
            raise SystemExit('texto original no encaja una vez tras ' + anchor + ': ' + lines[i][:120])
        lines[i] = lines[i].replace(old, new)
    return '\n'.join(lines)
if __name__ == '__main__':
    if sys.argv[1] == 'apply':
        P.write_bytes(apply(P.read_bytes().decode('utf-8')).encode('utf-8'))
        print('mutaciones aplicadas:', len(MUT))
