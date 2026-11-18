#!/bin/bash
cd /d/Original/wv-5/reversing
L="/e/JUEGOS/GTA San Andreas - CVehicle Test/reversed-tests.lock"
w() { for _ in $(seq 1 1080); do if [ ! -e "$L" ] && ! tasklist | grep -qi "^gta_sa.exe"; then return 0; fi; sleep 10; done; return 1; }
w || { echo "timeout espera" > Fn232A/go.out; exit 1; }
rm -f Fn232A/smoke_out.txt
bash Fn232A/smoke232.sh 512
cp Fn232A/smoke_out.txt Fn232A/go.out
if grep -q APROBADO Fn232A/smoke_out.txt; then
  w; KEEP_LOG= bash Fn232A/chain232.sh; echo "chain terminado" >> Fn232A/go.out
else echo "humo no aprobado" >> Fn232A/go.out; fi
