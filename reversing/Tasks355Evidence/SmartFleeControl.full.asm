0065C1E0: push esi
0065C1E1: mov esi,ecx
0065C1E3: push edi
0065C1E4: mov edi,dword ptr [esi+8]
0065C1E7: mov eax,dword ptr [edi]
0065C1E9: mov ecx,edi
0065C1EB: call dword ptr [eax+10h]
0065C1EE: cmp eax,390h
0065C1F3: jne 0065C390
0065C1F9: mov al,byte ptr [esi+24h]
0065C1FC: test al,al
0065C1FE: push ebx
0065C1FF: mov ebx,dword ptr [esp+10h]
0065C203: je 0065C21E
0065C205: push 0
0065C207: push 0
0065C209: push 0
0065C20B: push 3DCCCCCDh
0065C210: push 0
0065C212: push 15Bh
0065C217: mov ecx,ebx
0065C219: call 005EFFE0
0065C21E: mov ecx,dword ptr [esi+30h]
0065C221: push ebp
0065C222: mov ebp,dword ptr [esi+8]
0065C225: mov dword ptr [ebp+0Ch],ecx
0065C228: mov al,byte ptr [esi+41h]
0065C22B: test al,al
0065C22D: mov al,byte ptr [esi+3Ch]
0065C230: je 0065C2CB
0065C236: test al,al
0065C238: mov byte ptr [esi+41h],0
0065C23C: je 0065C251
0065C23E: mov eax,dword ptr [esi+28h]
0065C241: mov edx,dword ptr ds:[00B7CB84h]
0065C247: mov dword ptr [esi+34h],edx
0065C24A: mov dword ptr [esi+38h],eax
0065C24D: mov byte ptr [esi+3Ch],1
0065C251: push ebx
0065C252: mov ecx,esi
0065C254: call 0065BE40
0065C259: cmp byte ptr [esi+40h],al
0065C25C: je 0065C26C
0065C25E: mov byte ptr [esi+40h],al
0065C261: movzx eax,al
0065C264: push eax
0065C265: mov ecx,ebp
0065C267: call 00669D30
0065C26C: mov edi,dword ptr [esi+8]
0065C26F: cmp dword ptr [esi+30h],6
0065C273: pop ebp
0065C274: jne 0065C2C3
0065C276: push ebx
0065C277: mov ecx,0C15448h
0065C27C: call 006181A0
0065C281: test al,al
0065C283: jne 0065C2C3
0065C285: push 64h
0065C287: push 0
0065C289: call 00407180
0065C28E: add esp,8
0065C291: cmp eax,5
0065C294: jg 0065C2C3
0065C296: push 0
0065C298: push 3
0065C29A: push 1F4h
0065C29F: push 3E800000h
0065C2A4: push 0
0065C2A6: add esi,18h
0065C2A9: push esi
0065C2AA: push 0FFFFFFFFh
0065C2AC: push 7D0h
0065C2B1: push 0
0065C2B3: push ebx
0065C2B4: push 86F770h
0065C2B9: mov ecx,0C15448h
0065C2BE: call 00618970
0065C2C3: pop ebx
0065C2C4: mov eax,edi
0065C2C6: pop edi
0065C2C7: pop esi
0065C2C8: ret 4
0065C2CB: test al,al
0065C2CD: je 0065C2EC
0065C2CF: lea ecx,[esi+34h]
0065C2D2: call 00420E30
0065C2D7: test al,al
0065C2D9: je 0065C2EC
0065C2DB: push ebx
0065C2DC: push 0CBh
0065C2E1: mov ecx,esi
0065C2E3: call 0065BE80
0065C2E8: mov edi,eax
0065C2EA: jmp 0065C26F
0065C2EC: mov ecx,dword ptr [ebx+14h]
0065C2EF: test ecx,ecx
0065C2F1: lea eax,[ecx+30h]
0065C2F4: jne 0065C2F9
0065C2F6: lea eax,[ebx+4]
0065C2F9: fld dword ptr [esi+18h]
0065C2FC: fsub dword ptr [eax]
0065C2FE: fld dword ptr [esi+1Ch]
0065C301: fsub dword ptr [eax+4]
0065C304: fld dword ptr [esi+20h]
0065C307: fsub dword ptr [eax+8]
0065C30A: fld dword ptr [esi+2Ch]
0065C30D: fld st(0)
0065C30F: fmul st,st(1)
0065C311: fstp dword ptr [esp+14h]
0065C315: fstp st(0)
0065C317: fld st(0)
0065C319: fmul st,st(1)
0065C31B: fld st(2)
0065C31D: fmul st,st(3)
0065C31F: faddp st(1),st
0065C321: fld st(3)
0065C323: fmul st,st(4)
0065C325: faddp st(1),st
0065C327: fcomp dword ptr [esp+14h]
0065C32B: fstp st(0)
0065C32D: fnstsw ax
0065C32F: fstp st(0)
0065C331: test ah,41h
0065C334: fstp st(0)
0065C336: jne 0065C26C
0065C33C: test ecx,ecx
0065C33E: lea eax,[ecx+30h]
0065C341: jne 0065C346
0065C343: lea eax,[ebx+4]
0065C346: fld dword ptr [esi+0Ch]
0065C349: fsub dword ptr [eax]
0065C34B: fld dword ptr [esi+10h]
0065C34E: fsub dword ptr [eax+4]
0065C351: fld dword ptr [esi+14h]
0065C354: fsub dword ptr [eax+8]
0065C357: fld st(0)
0065C359: fmul st,st(1)
0065C35B: fld st(2)
0065C35D: fmul st,st(3)
0065C35F: faddp st(1),st
0065C361: fld st(3)
0065C363: fmul st,st(4)
0065C365: faddp st(1),st
0065C367: fcomp dword ptr [esp+14h]
0065C36B: fstp st(0)
0065C36D: fnstsw ax
0065C36F: fstp st(0)
0065C371: test ah,41h
0065C374: fstp st(0)
0065C376: jne 0065C26F
0065C37C: push ebx
0065C37D: push 0CBh
0065C382: mov ecx,esi
0065C384: call 0065BE80
0065C389: mov edi,eax
0065C38B: jmp 0065C26F
0065C390: mov edi,dword ptr [esp+0Ch]
0065C394: push edi
0065C395: mov ecx,0C15448h
0065C39A: call 006181A0
0065C39F: test al,al
0065C3A1: je 0065C3B3
0065C3A3: push 0FAh
0065C3A8: push edi
0065C3A9: mov ecx,0C15448h
 0065C3AE: E8
0065C3AF: int 0BEh
