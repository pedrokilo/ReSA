BS=chr(92)
s=open('rwprobe.c',encoding='utf-8').read()
# hora del juego en la linea de estado
s=s.replace("DWORD gs = 0, fc = 0, tm = 0, ped = 0, mtx = 0; BYTE menu = 0; float x = 0, y = 0, z = 0;","DWORD gs = 0, fc = 0, tm = 0, ped = 0, mtx = 0; BYTE menu = 0, hh = 0, mm = 0; float x = 0, y = 0, z = 0;",1)
s=s.replace("rd(0xB7CD98, &ped, 4);\n            if (ped && rd(ped + 0x14, &mtx, 4)) {","rd(0xB70153, &hh, 1); rd(0xB70152, &mm, 1);\n            rd(0xB7CD98, &ped, 4);\n            if (ped && rd(ped + 0x14, &mtx, 4)) {",1)
s=s.replace('"{'+BS+'"t_ms'+BS+'":%lu,','"{'+BS+'"hora'+BS+'":'+BS+'"%02u:%02u'+BS+'",'+BS+'"t_ms'+BS+'":%lu,',1)
s=s.replace("(unsigned long)now, (unsigned long)gs, (unsigned long)fc,","hh, mm, (unsigned long)now, (unsigned long)gs, (unsigned long)fc,",1)
# fijar hora al armar
s=s.replace("            if (fpTrap && !armed && gs == 9 && fc > 0) {","""            if (!armed && gs == 9 && fc > 0 && getenv("RWPROBE_HOUR")) {
                BYTE h = (BYTE)atoi(getenv("RWPROBE_HOUR")), z0 = 0;
                DWORD old; VirtualProtect((void *)0xB70150, 8, PAGE_READWRITE, &old);
                *(BYTE *)0xB70153 = h; *(BYTE *)0xB70152 = z0;
            }
            if (fpTrap && !armed && gs == 9 && fc > 0) {""",1)
s=s.replace("                        armed = 1;\n","                        armed = 1;\n",1)
open('rwprobe.c','w',encoding='utf-8').write(s)
