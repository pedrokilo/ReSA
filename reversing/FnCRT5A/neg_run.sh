#!/bin/sh
# Prueba negativa CRT5A: aplica una mutacion de una linea, recompila el banco y comprueba que detecta la diferencia.
# Las mutaciones corren en paralelo (cada una con su directorio de fuentes y su ejecutable); el resultado se junta al final en el orden de la lista.
ROOT=${ROOT:-/d/Original/wv-29}
ROOTW=${ROOTW:-'D:\Original\wv-29'}
EXE="E:\JUEGOS\GTA San Andreas - CVehicle Test\gta_sa.exe"
SRCDIR=$ROOT/source/game_sa/Crt
WORK=${WORK:-/d/Original/_crt5a_neg}
WORKW=${WORKW:-'D:\Original\_crt5a_neg'}
CASOS=${CASOS:-250}
rm -rf $WORK; mkdir -p $WORK
n=0
run() { # archivo, original, reemplazo, grupo
  n=$((n+1)); i=$n
  (
    T=$WORK/m$i; mkdir -p $T/src; cp $SRCDIR/*.cpp $SRCDIR/*.h $SRCDIR/*.inc $T/src/
    python - "$T/src/$1" "$2" "$3" <<'PY' || { echo "== mutacion $i: $1: no se pudo aplicar" > $WORK/r$i.txt; exit 0; }
import sys
p,a,b=sys.argv[1:4]; s=open(p,encoding='utf-8',newline='').read()
assert a in s,("no encontrado",a); open(p,'w',encoding='utf-8',newline='').write(s.replace(a,b,1))
PY
    cmd.exe //c "$ROOTW\reversing\FnCRT5A\build_bench.cmd $WORKW\m$i\out $WORKW\m$i\src" >/dev/null 2>&1
    { echo "== mutacion $i: $1: '$2' -> '$3'  (grupo $4)";
      BENCH_STOP=1 timeout 900 $T/out/bench_crt5a.exe "$EXE" --casos $CASOS --solo $4 2>&1 | grep -E "DIFERENCIA|RESULTADO" | head -2; } > $WORK/r$i.txt
    rm -rf $T/out $T/src
  ) &
  if [ $((n % 8)) -eq 0 ]; then wait; fi
}
run CrtLowIo.cpp "if (!(Io(fh)->osfile & 0x40)) {" "if (Io(fh)->osfile & 0x40) {" fd
run CrtLowIo.cpp "return written - lfCount;" "return written;" fd
run CrtLowIo.cpp "    Io(fh)->osfile &= 0xFD;
    return static_cast<long>(pos);" "    Io(fh)->osfile &= 0xFB;
    return static_cast<long>(pos);" fd
run CrtLowIo.cpp "if (!((~UmaskVal()) & pmode & 0x80)) {" "if (!((~UmaskVal()) & pmode & 0x100)) {" fd
run CrtLowIo.cpp "case 0x600: disposition = TRUNCATE_EXISTING; break;" "case 0x600: disposition = OPEN_EXISTING; break;" fd
run CrtLowIo.cpp "fileflags = 0x10;" "fileflags = 0x20;" fd
run CrtLowIo.cpp "ReadNolock(fh, &last, 1) == 0 && last == 0x1A" "ReadNolock(fh, &last, 1) == 0 && last == 0x1B" fd
run CrtLowIo.cpp "std::memset(zeros, 0, sizeof(zeros));" "std::memset(zeros, 1, sizeof(zeros));" fd
run CrtLowIo.cpp "const int old = (flags & 0x80) ? 0x4000 : 0x8000;" "const int old = (flags & 0x80) ? 0x8000 : 0x4000;" fd
run CrtLowIo.cpp "DosErrno() = err;" "DosErrno() = 0;" fd
run CrtLowIo.cpp "if (oscode >= 0x13 && oscode <= 0x24) {" "if (oscode >= 0x13 && oscode <= 0x23) {" unit
run CrtLowIo.cpp "NHandle() += 0x20;" "NHandle() += 0x10;" many
run CrtLowIo.cpp "    Io(fh)->osfile &= 0xFD;
    return static_cast<long long>" "    Io(fh)->osfile &= 0xFB;
    return static_cast<long long>" fd
run CrtLowIo.cpp "fileflags += 0x20;" "fileflags += 0x10;" fd
run CrtLowIo.cpp "SopenHelper(&unlockFlag, &fh, path, oflag, pmode, 0x40);" "SopenHelper(&unlockFlag, &fh, path, oflag, pmode, 0x10);" fd
run CrtLowIo.cpp "if (OsPlatform() != 1) {" "if (OsPlatform() != 2) {" unit
run CrtLowIo.cpp "SetStdHandle(STD_ERROR_HANDLE, reinterpret_cast<HANDLE>(static_cast<intptr_t>(osHandle)));" "SetStdHandle(STD_OUTPUT_HANDLE, reinterpret_cast<HANDLE>(static_cast<intptr_t>(osHandle)));" std
run CrtLowIo.cpp "io->osfile = 0x81;" "io->osfile = 0x80;" init
run CrtStdio.cpp "*f->base = static_cast<char>(ch);" "*f->base = static_cast<char>(ch + 1);" stdio
run CrtStdio.cpp "f->flag |= (n != 0) ? 0x20 : 0x10;" "f->flag |= (n != 0) ? 0x10 : 0x20;" stdio
run CrtStdio.cpp "toRead = remaining - remaining % bufSize;" "toRead = remaining;" stdio
run CrtStdio.cpp "if (static_cast<unsigned int>(w) < toWrite) {" "if (static_cast<unsigned int>(w) <= toWrite) {" stdio
run CrtStdio.cpp "++adjust;" "adjust += 2;" stdio
run CrtStdio.cpp "offset += FtellNolock(f);" "offset -= FtellNolock(f);" stdio
run CrtStdio.cpp "oflag = 0x109;" "oflag = 0x108;" stdio
run CrtStdio.cpp "++count;" "count += 2;" stdio
run CrtStdio.cpp "if (Close(f->file) < 0) {" "if (Close(f->file) <= 0) {" stdio
run CrtStdio.cpp "f->flag &= ~0x0408;" "f->flag &= ~0x0008;" stdio
run CrtStdio.cpp "                    f->flag &= ~2;" "                    f->flag &= ~1;" stdio
run CrtStdio.cpp "f->flag |= 0x2000;" "f->flag |= 0x1000;" stdio
run CrtStdio.cpp "n = 0x200;" "n = 0x100;" init
wait
for k in $(seq 1 $n); do cat $WORK/r$k.txt; done
