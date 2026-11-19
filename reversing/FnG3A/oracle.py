"""Oraculo independiente del separador 0x00746480 (WinMain), escrito desde el desensamblado.

Semantica (leida de las instrucciones, no del CRT):

  int** sep(const char* cmd /*EAX*/, int* argc /*pila*/)   // el argumento de pila lo limpia la llamadora (ret simple)
  n = strlen(cmd)
  Pasada 1 (cuenta): para i = 0..n (incluye el NUL final), c = cmd[i]:
      si c == '"'  -> inq = !inq
      si (c > 0x20 sin signo) o inq:
          si i != n: si !entoken: entoken = 1;  siguiente i
          (si i == n: cae al cierre)
      cierre: si entoken: entoken = 0; cuenta++
    => separadores = cualquier byte <= 0x20 fuera de comillas (espacio, tab, CR, LF, controles); los bytes >= 0x80 son
       caracteres normales; las comillas cuentan como inicio de token (una cadena solo de comillas es 1 token en esta pasada).
  Reserva: malloc(n + 1 + 4*cuenta); la tabla de punteros ocupa los primeros 4*cuenta bytes (SIN terminar con NULL)
           y detras va una copia de la cadena (n+1 bytes).
  Pasada 2 (llena): mismo recorrido sobre la cadena original con inq/entoken propios; en cada i < n con c > 0x20 o inq:
      si !entoken y c != '"': argv[k++] = copia + i; entoken = 1     (las comillas NO inician token en esta pasada)
    cierre (c <= 0x20 fuera de comillas, o i == n): si entoken: si copia[i-1] == '"' copia[i-1] = 0, si no copia[i] = 0;
      entoken = 0
    => las comillas iniciales se saltan, solo se borra UNA comilla final del token; las comillas intermedias se conservan;
       no hay tratamiento de barras invertidas ni de comillas dobles; una comilla sin cerrar abarca hasta el final.
  *argc = cuenta (de la pasada 1); devuelve el bloque. Si algun token solo tiene comillas (p. ej. "" o "), la pasada 2
  rellena menos punteros que `cuenta`: las ranuras finales quedan SIN INICIALIZAR (basura del heap) y argc las cuenta.
  El primer token (nombre del programa) no recibe ningun trato especial. Sin limite de longitud.
"""
GARBAGE = 0xCD


def orig_split(cmd: bytes, base: int = 0x10000000):
    """Devuelve (argc, bloque) con los punteros como base+desplazamiento y las ranuras sin rellenar a 0xCDCDCDCD."""
    n = len(cmd)
    s = cmd + b'\0'
    inq = entok = cnt = 0
    for i in range(n + 1):
        c = s[i]
        if c == 0x22:
            inq ^= 1
        if c > 0x20 or inq:
            if i != n:
                entok = 1
                continue
        if entok:
            entok = 0
            cnt += 1
    blk = bytearray([GARBAGE]) * (4 * cnt) + bytearray(s)
    cp = 4 * cnt
    k = 0
    inq = entok = 0
    for i in range(n + 1):
        c = s[i]
        if c == 0x22:
            inq ^= 1
        if c > 0x20 or inq:
            if i != n:
                if not entok and c != 0x22:
                    blk[4 * k:4 * k + 4] = (base + cp + i).to_bytes(4, 'little')
                    k += 1
                    entok = 1
                continue
        if entok:
            if blk[cp + i - 1] == 0x22:
                blk[cp + i - 1] = 0
            else:
                blk[cp + i] = 0
            entok = 0
    assert len(blk) == n + 1 + 4 * cnt
    return cnt, bytes(blk)


def argv_of(cnt: int, blk: bytes, base: int = 0x10000000):
    """Lista de bytes por ranura; None si la ranura quedo sin inicializar."""
    out = []
    for k in range(cnt):
        p = int.from_bytes(blk[4 * k:4 * k + 4], 'little')
        if p == 0xCDCDCDCD:
            out.append(None)
            continue
        off = p - base
        out.append(blk[off:blk.index(b'\0', off)])
    return out
