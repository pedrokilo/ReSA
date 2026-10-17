void FlcRouteRotor(FlcFrame& f) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    alignas(16) uint8 stk[0xB00] = {};                      // pila x86 del EXE (el marco [F+x] empieza en stk + 0x800)
    uint32 eax = 0, ecx = 0, edx = 0, ebx = (uint32)(uintptr_t)f.pad, esi = (uint32)(uintptr_t)f.v, edi = 0, ebp = 0;
    uint32 esp = (uint32)(uintptr_t)stk + 0x800;            // F
    uint32 fA = 0, fB = 0, fR = 0;
    M32(esp + 0x264) = (uint32)f.model; memcpy(stk + 0x800 + 0x268, &f.skid, 4); memcpy(stk + 0x800 + 0x26C, &f.upDown, 4);
    memcpy(stk + 0x800 + 0x270, &f.sideways, 4); memcpy(stk + 0x800 + 0x274, &f.accel, 4);
    M32(esp + 0x14) = ebx;                                  // mando del conductor
    memcpy(stk + 0x800 + 0x40, f.vel, 12);                  // velocidad + deriva del viento
    memcpy(stk + 0x800 + 0x50, f.com, 12);                  // centro de masas en el mundo
    M32(esp + 0x4C) = (uint32)(uintptr_t)&v.m_vecMoveSpeed;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d986e: mov ecx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 0x3c)));   // 6d9874: fld dword ptr [ecx + 0x3c]
    fp.Fld(CTimer::ms_fTimeStep);   // 6d9877: fld dword ptr [0xb7cb5c]
    FlcCrtPow(fp);   // 6d987d: call 0x822130
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d9882: mov eax, dword ptr [esp + 0x4c]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x264));   // 6d9886: mov ebp, dword ptr [esp + 0x264]
    fA = ebp; fB = 0x8u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d988d: cmp ebp, 8
    fp.FldSt(0);   // 6d9890: fld st(0)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 * m   // 6d9892: fmul dword ptr [eax]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(eax)));   // 6d9894: fstp dword ptr [eax]
    fp.FldSt(0);   // 6d9896: fld st(0)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x48)));   // ST0 = ST0 * m   // 6d9898: fmul dword ptr [esi + 0x48]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esi + 0x48)));   // 6d989b: fstp dword ptr [esi + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x4c)));   // ST0 = ST0 * m   // 6d989e: fmul dword ptr [esi + 0x4c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esi + 0x4c)));   // 6d98a1: fstp dword ptr [esi + 0x4c]
    if (fR != 0) goto L_6d9b9d;   // 6d98a4: jne 0x6d9b9d
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d98aa: mov eax, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d98ad: fld dword ptr [esp + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // ST0 = ST0 * m   // 6d98b1: fmul dword ptr [eax + 0x18]
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d98b4: add eax, 0x10
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d98b7: fld dword ptr [esp + 0x44]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 * m   // 6d98bb: fmul dword ptr [eax + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d98be: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d98c0: fld dword ptr [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 * m   // 6d98c4: fmul dword ptr [eax]
    fp.Arith('+', 1, 1, 0, true);   // 6d98c6: faddp st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d98c8: fstp dword ptr [esp + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d98cc: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), false);   // 6d98d3: fcom dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d98d9: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d98db: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d9912;   // 6d98de: jp 0x6d9912
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d98e0: test ebx, ebx
    fp.FstpSt(0);   // 6d98e2: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d98e4: fld dword ptr [0x858b50]
    if (fR == 0) goto L_6d9912;   // 6d98ea: je 0x6d9912
    ecx = ebx;   // 6d98ec: mov ecx, ebx
    fp.FstpSt(0);   // 6d98ee: fstp st(0)
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetAccelerate();   // 6d98f0: call 0x5403f0
    ecx = ebx;   // 6d98f5: mov ecx, ebx
    edi = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d98f7: movsx edi, ax
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetBrake();   // 6d98fa: call 0x540080
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d98ff: movsx edx, ax
    fA = edi; fB = edx; fR = (fA - fB) & 0xFFFFFFFFu; edi = fR;   // 6d9902: sub edi, edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edi);   // 6d9904: mov dword ptr [esp + 0x18], edi
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9908: fild dword ptr [esp + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x859a3c)));   // ST0 = ST0 * m   // 6d990c: fmul dword ptr [0x859a3c]
L_6d9912:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9912: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9916: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d991c: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d991e: test ah, 0x41
    if (fR == 0) goto L_6d9965;   // 6d9921: je 0x6d9965
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), false);   // 6d9923: fcom dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9929: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d992b: test ah, 0x41
    if (fR == 0) goto L_6d9965;   // 6d992e: je 0x6d9965
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9930: mov ecx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9936: fld dword ptr [esp + 0x10]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6d993a: fmul dword ptr [ecx + 8]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x859000)));   // ST0 = ST0 * m   // 6d993d: fmul dword ptr [0x859000]
    fp.Arith('-', 1, 1, 0, true);   // 6d9943: fsubp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d9945: fld dword ptr [0x858b50]
    fp.FcomSt(1, true);   // 6d994b: fcomp st(1)
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d994d: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d994f: test ah, 5
    if (FlcParityEven(fR)) goto L_6d995c;   // 6d9952: jp 0x6d995c
    fp.FstpSt(0);   // 6d9954: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d9956: fld dword ptr [0x858b50]
L_6d995c:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d995c: fmul dword ptr [ecx + 4]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d995f: fstp dword ptr [esp + 0x18]
    goto L_6d997d;   // 6d9963: jmp 0x6d997d
L_6d9965:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9965: mov eax, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d996b: fld dword ptr [esp + 0x10]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 * m   // 6d996f: fmul dword ptr [eax + 8]
    fp.Arith('-', 0, 1, 0, false);   // 6d9972: fsubr st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 * m   // 6d9974: fmul dword ptr [eax + 4]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9977: fstp dword ptr [esp + 0x18]
    fp.FstpSt(0);   // 6d997b: fstp st(0)
L_6d997d:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d997d: mov eax, dword ptr [0xb7cb5c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9982: mov ecx, dword ptr [esi + 0x8c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x18));   // 6d9988: mov edx, dword ptr [esp + 0x18]
    esp -= 4; M32(esp) = eax;   // 6d998c: push eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d998d: mov eax, dword ptr [esi + 0x14]
    esp -= 4; M32(esp) = ecx;   // 6d9990: push ecx
    esp -= 4; M32(esp) = edx;   // 6d9991: push edx
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d9992: add eax, 0x10
    esp -= 4; M32(esp) = eax;   // 6d9995: push eax
    ecx = (uint32)(esp + 0x240);   // 6d9996: lea ecx, [esp + 0x240]
    esp -= 4; M32(esp) = ecx;   // 6d999d: push ecx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d999e: call 0x40fec0
    esp += 0xCu;   // 6d99a3: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d99a6: push eax
    edx = (uint32)(esp + 0x14c);   // 6d99a7: lea edx, [esp + 0x14c]
    esp -= 4; M32(esp) = edx;   // 6d99ae: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d99af: call 0x40fec0
    esp += 0xCu;   // 6d99b4: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d99b7: push eax
    eax = (uint32)(esp + 0x1d8);   // 6d99b8: lea eax, [esp + 0x1d8]
    esp -= 4; M32(esp) = eax;   // 6d99bf: push eax
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d99c0: call 0x40fec0
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d99c5: mov edx, dword ptr [eax]
    ecx = esp;   // 6d99c7: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d99c9: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d99cb: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d99ce: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d99d1: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d99d4: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6d99d7: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d99d9: call 0x5429f0
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d99de: mov edi, dword ptr [esi + 0x14]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0x8d34e8));   // 6d99e1: mov edx, dword ptr [0x8d34e8]
    ecx = (uint32)(edi + 0x10);   // 6d99e7: lea ecx, [edi + 0x10]
    esp -= 4; M32(esp) = ecx;   // 6d99ea: push ecx
    esp -= 4; M32(esp) = edx;   // 6d99eb: push edx
    eax = (uint32)(esp + 0x160);   // 6d99ec: lea eax, [esp + 0x160]
    esp -= 4; M32(esp) = eax;   // 6d99f3: push eax
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d99f4: call 0x40fe90
    esp -= 4; M32(esp) = eax;   // 6d99f9: push eax
    fA = edi; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6d99fa: add edi, 0x20
    ecx = (uint32)(esp + 0x2c);   // 6d99fd: lea ecx, [esp + 0x2c]
    esp -= 4; M32(esp) = edi;   // 6d9a01: push edi
    esp -= 4; M32(esp) = ecx;   // 6d9a02: push ecx
    FlcVecSub(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9a03: call 0x40fe60
    esp += 0x18u;   // 6d9a08: add esp, 0x18
    ecx = (uint32)(esp + 0x1c);   // 6d9a0b: lea ecx, [esp + 0x1c]
    FlcVecNormalise(fp, (float*)(uintptr_t)ecx);   // 6d9a0f: call 0x59c910
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9a14: fld dword ptr [esp + 0x1c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // ST0 = ST0 * m   // 6d9a18: fmul dword ptr [esp + 0x40]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x590));   // 6d9a1c: mov eax, dword ptr [esi + 0x590]
    fA = eax; fB = eax; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9a22: test eax, eax
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6d9a24: fld dword ptr [esp + 0x24]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // ST0 = ST0 * m   // 6d9a28: fmul dword ptr [esp + 0x48]
    fp.Arith('+', 1, 1, 0, true);   // 6d9a2c: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9a2e: fld dword ptr [esp + 0x20]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // ST0 = ST0 * m   // 6d9a32: fmul dword ptr [esp + 0x44]
    fp.Arith('+', 1, 1, 0, true);   // 6d9a36: faddp st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9a38: fstp dword ptr [esp + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x8717ac)));   // 6d9a3c: fld dword ptr [0x8717ac]
    if (fR != 0) goto L_6d9b04;   // 6d9a42: jne 0x6d9b04
    fp.FstpSt(0);   // 6d9a48: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x8d34d8)));   // 6d9a4a: fld dword ptr [0x8d34d8]
    fp.Fchs();   // 6d9a50: fchs 
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x10)), false);   // 6d9a52: fcom dword ptr [esp + 0x10]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9a56: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9a58: test ah, 0x41
    if (fR != 0) goto L_6d9a61;   // 6d9a5b: jne 0x6d9a61
    fp.FldSt(0);   // 6d9a5d: fld st(0)
    goto L_6d9a65;   // 6d9a5f: jmp 0x6d9a65
L_6d9a61:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9a61: fld dword ptr [esp + 0x10]
L_6d9a65:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d9a65: fld dword ptr [0x858b50]
    fp.FcomSt(1, true);   // 6d9a6b: fcomp st(1)
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9a6d: fnstsw ax
    fp.FstpSt(0);   // 6d9a6f: fstp st(0)
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d9a71: test ah, 5
    if (FlcParityEven(fR)) goto L_6d9ad6;   // 6d9a74: jp 0x6d9ad6
    *(uint32*)(uintptr_t)(uint32)(esp + 0x10) = (uint32)(0x0u);   // 6d9a76: mov dword ptr [esp + 0x10], 0
L_6d9a7e:;
    fp.FstpSt(0);   // 6d9a7e: fstp st(0)
L_6d9a80:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9a80: fld dword ptr [esp + 0x10]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d9a84: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x8d34d4)));   // ST0 = ST0 * m   // 6d9a8a: fmul dword ptr [0x8d34d4]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(esi + 0x84c)));   // ST0 = m - ST0   // 6d9a90: fsubr dword ptr [esi + 0x84c]
    fp.Fst((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9a96: fst dword ptr [esp + 0x18]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esi + 0x84c)));   // 6d9a9a: fstp dword ptr [esi + 0x84c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x8d34dc)));   // 6d9aa0: fld dword ptr [0x8d34dc]
    fp.Fld(CTimer::ms_fTimeStep);   // 6d9aa6: fld dword ptr [0xb7cb5c]
    FlcCrtPow(fp);   // 6d9aac: call 0x822130
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = ST0 * m   // 6d9ab1: fmul dword ptr [esp + 0x18]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858ee8)), false);   // 6d9ab5: fcom dword ptr [0x858ee8]
    fp.Fst((*(float*)(uintptr_t)(uint32)(esi + 0x84c)));   // 6d9abb: fst dword ptr [esi + 0x84c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9ac1: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9ac3: test ah, 0x41
    if (fR != 0) goto L_6d9ae7;   // 6d9ac6: jne 0x6d9ae7
    fp.FstpSt(0);   // 6d9ac8: fstp st(0)
    *(uint32*)(uintptr_t)(uint32)(esi + 0x84c) = (uint32)(0x3ECCCCCDu);   // 6d9aca: mov dword ptr [esi + 0x84c], 0x3ecccccd
    goto L_6d9afe;   // 6d9ad4: jmp 0x6d9afe
L_6d9ad6:;
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x10)), false);   // 6d9ad6: fcom dword ptr [esp + 0x10]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9ada: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9adc: test ah, 0x41
    if (fR != 0) goto L_6d9a7e;   // 6d9adf: jne 0x6d9a7e
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9ae1: fstp dword ptr [esp + 0x10]
    goto L_6d9a80;   // 6d9ae5: jmp 0x6d9a80
L_6d9ae7:;
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x859018)), true);   // 6d9ae7: fcomp dword ptr [0x859018]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9aed: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d9aef: test ah, 5
    if (FlcParityEven(fR)) goto L_6d9afe;   // 6d9af2: jp 0x6d9afe
    *(uint32*)(uintptr_t)(uint32)(esi + 0x84c) = (uint32)(0x3DA3D70Au);   // 6d9af4: mov dword ptr [esi + 0x84c], 0x3da3d70a
L_6d9afe:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x84c)));   // 6d9afe: fld dword ptr [esi + 0x84c]
L_6d9b04:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9b04: fld dword ptr [esp + 0x1c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d9b08: mov edx, dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9b0e: fmul dword ptr [0x863984]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9b14: mov eax, dword ptr [esi + 0x8c]
    esp -= 4; M32(esp) = edx;   // 6d9b1a: push edx
    esp -= 4; M32(esp) = eax;   // 6d9b1b: push eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d9b1c: fstp dword ptr [esp + 0x3c]
    esp -= 4; M32(esp) = ecx;   // 6d9b20: push ecx
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9b21: fld dword ptr [esp + 0x2c]
    ecx = (uint32)(esp + 0x40);   // 6d9b25: lea ecx, [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9b29: fmul dword ptr [0x863984]
    edx = (uint32)(esp + 0x224);   // 6d9b2f: lea edx, [esp + 0x224]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d9b36: fstp dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9b3a: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9b3e: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9b44: fstp dword ptr [esp + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x8d34e0)));   // ST0 = ST0 * m   // 6d9b48: fmul dword ptr [0x8d34e0]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9b4e: fld dword ptr [esp + 0x1c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x8d34e4)));   // ST0 = ST0 * m   // 6d9b52: fmul dword ptr [0x8d34e4]
    fp.Arith('-', 1, 1, 0, true);   // 6d9b58: fsubp st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9b5a: fstp dword ptr [esp]
    esp -= 4; M32(esp) = ecx;   // 6d9b5d: push ecx
    esp -= 4; M32(esp) = edx;   // 6d9b5e: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9b5f: call 0x40fec0
    esp += 0xCu;   // 6d9b64: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9b67: push eax
    eax = (uint32)(esp + 0x17c);   // 6d9b68: lea eax, [esp + 0x17c]
    esp -= 4; M32(esp) = eax;   // 6d9b6f: push eax
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9b70: call 0x40fec0
    esp += 0xCu;   // 6d9b75: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9b78: push eax
    ecx = (uint32)(esp + 0xa0);   // 6d9b79: lea ecx, [esp + 0xa0]
    esp -= 4; M32(esp) = ecx;   // 6d9b80: push ecx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9b81: call 0x40fec0
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9b86: mov ecx, dword ptr [eax]
    edx = esp;   // 6d9b88: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6d9b8a: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9b8c: mov ecx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9b8f: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6d9b92: mov dword ptr [edx + 4], ecx
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d9b95: mov dword ptr [edx + 8], eax
    goto L_6d9ef2;   // 6d9b98: jmp 0x6d9ef2
L_6d9b9d:;
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9b9d: mov edi, dword ptr [esi + 0x14]
    ecx = (uint32)(edi + 0x20);   // 6d9ba0: lea ecx, [edi + 0x20]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx));   // 6d9ba3: mov edx, dword ptr [ecx]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 4));   // 6d9ba5: mov eax, dword ptr [ecx + 4]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 8));   // 6d9ba8: mov ecx, dword ptr [ecx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x20) = (uint32)(eax);   // 6d9bab: mov dword ptr [esp + 0x20], eax
    fA = (uint32)(*(uint8*)(uintptr_t)(uint32)(esi + 0x42b)); fB = 0x10u; fR = (fA & fB) & 0xFFu;   // 6d9baf: test byte ptr [esi + 0x42b], 0x10
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(edx);   // 6d9bb6: mov dword ptr [esp + 0x1c], edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x24) = (uint32)(ecx);   // 6d9bba: mov dword ptr [esp + 0x24], ecx
    if (fR == 0) goto L_6d9c04;   // 6d9bbe: je 0x6d9c04
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9bc0: fld dword ptr [esp + 0x1c]
    FlcCrtAsin(fp);   // 6d9bc4: call 0x821e70
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b90)));   // ST0 = ST0 * m   // 6d9bc9: fmul dword ptr [0x858b90]
    fp.Fsin();   // 6d9bcf: fsin 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9bd1: fstp dword ptr [esp + 0x1c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9bd5: fld dword ptr [esp + 0x20]
    FlcCrtAsin(fp);   // 6d9bd9: call 0x821e70
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b90)));   // ST0 = ST0 * m   // 6d9bde: fmul dword ptr [0x858b90]
    fp.Fsin();   // 6d9be4: fsin 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9be6: fstp dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6d9bea: fld dword ptr [esp + 0x24]
    FlcCrtAcos(fp);   // 6d9bee: call 0x822380
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b90)));   // ST0 = ST0 * m   // 6d9bf3: fmul dword ptr [0x858b90]
    fp.Fcos();   // 6d9bf9: fcos 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6d9bfb: fstp dword ptr [esp + 0x24]
    goto L_6d9c95;   // 6d9bff: jmp 0x6d9c95
L_6d9c04:;
    fA = (uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x22)); fB = 0x208u; fR = (fA - fB) & 0xFFFFu;   // 6d9c04: cmp word ptr [esi + 0x22], 0x208
    if (fR != 0) goto L_6d9c95;   // 6d9c0a: jne 0x6d9c95
    edx = (uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x86c));   // 6d9c10: movzx edx, word ptr [esi + 0x86c]
    eax = (uint32)(int32_t)(int16_t)((uint32)(*(uint16*)(uintptr_t)(uint32)(0x8d33c4)));   // 6d9c17: movsx eax, word ptr [0x8d33c4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edx);   // 6d9c1e: mov dword ptr [esp + 0x18], edx
    ecx = (uint32)(edi + 0x20);   // 6d9c22: lea ecx, [edi + 0x20]
    esp -= 4; M32(esp) = ecx;   // 6d9c25: push ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9c26: fild dword ptr [esp + 0x1c]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(eax);   // 6d9c2a: mov dword ptr [esp + 0x1c], eax
    esp -= 4; M32(esp) = ecx;   // 6d9c2e: push ecx
    edx = (uint32)(esp + 0xac);   // 6d9c2f: lea edx, [esp + 0xac]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858fe4)));   // ST0 = ST0 * m   // 6d9c36: fmul dword ptr [0x858fe4]
    fp.FidivI((*(int*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9c3c: fidiv dword ptr [esp + 0x20]
    fp.FldSt(0);   // 6d9c40: fld st(0)
    fp.Fcos();   // 6d9c42: fcos 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9c44: fstp dword ptr [esp + 0x20]
    fp.Fsin();   // 6d9c48: fsin 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9c4a: fstp dword ptr [esp]
    esp -= 4; M32(esp) = edx;   // 6d9c4d: push edx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9c4e: call 0x40fe90
    esp += 0xCu;   // 6d9c53: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9c56: push eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x1c));   // 6d9c57: mov eax, dword ptr [esp + 0x1c]
    fA = edi; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6d9c5b: add edi, 0x10
    esp -= 4; M32(esp) = edi;   // 6d9c5e: push edi
    esp -= 4; M32(esp) = eax;   // 6d9c5f: push eax
    ecx = (uint32)(esp + 0xc8);   // 6d9c60: lea ecx, [esp + 0xc8]
    esp -= 4; M32(esp) = ecx;   // 6d9c67: push ecx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9c68: call 0x40fe90
    esp += 0xCu;   // 6d9c6d: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9c70: push eax
    edx = (uint32)(esp + 0xdc);   // 6d9c71: lea edx, [esp + 0xdc]
    esp -= 4; M32(esp) = edx;   // 6d9c78: push edx
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9c79: call 0x40fe30
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9c7e: mov ecx, dword ptr [eax]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9c80: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9c83: mov eax, dword ptr [eax + 8]
    esp += 0xCu;   // 6d9c86: add esp, 0xc
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(ecx);   // 6d9c89: mov dword ptr [esp + 0x1c], ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x20) = (uint32)(edx);   // 6d9c8d: mov dword ptr [esp + 0x20], edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x24) = (uint32)(eax);   // 6d9c91: mov dword ptr [esp + 0x24], eax
L_6d9c95:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9c95: fld dword ptr [esp + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // ST0 = ST0 * m   // 6d9c99: fmul dword ptr [esp + 0x24]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d9c9d: fld dword ptr [esp + 0x44]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6d9ca1: fmul dword ptr [esp + 0x20]
    fp.Arith('+', 1, 1, 0, true);   // 6d9ca5: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9ca7: fld dword ptr [esp + 0x1c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // ST0 = ST0 * m   // 6d9cab: fmul dword ptr [esp + 0x40]
    fp.Arith('+', 1, 1, 0, true);   // 6d9caf: faddp st(1)
    fp.Fst((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9cb1: fst dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9cb5: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9cbb: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9cbd: test ah, 0x41
    if (fR != 0) goto L_6d9ccc;   // 6d9cc0: jne 0x6d9ccc
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9cc2: fld dword ptr [esp + 0x10]
    fp.Arith('+', 0, 0, 0, false);   // 6d9cc6: fadd st(0), st(0)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9cc8: fstp dword ptr [esp + 0x10]
L_6d9ccc:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d9ccc: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d9cd3: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9cd9: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d9cdb: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d9d5a;   // 6d9cde: jp 0x6d9d5a
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9ce0: test ebx, ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x274) = (uint32)(0x0u);   // 6d9ce2: mov dword ptr [esp + 0x274], 0
    if (fR == 0) goto L_6d9d5a;   // 6d9ced: je 0x6d9d5a
    ecx = ebx;   // 6d9cef: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetAccelerate();   // 6d9cf1: call 0x5403f0
    ecx = ebx;   // 6d9cf6: mov ecx, ebx
    edi = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d9cf8: movsx edi, ax
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetBrake();   // 6d9cfb: call 0x540080
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d9d00: movsx ecx, ax
    fA = edi; fB = ecx; fR = (fA - fB) & 0xFFFFFFFFu; edi = fR;   // 6d9d03: sub edi, ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edi);   // 6d9d05: mov dword ptr [esp + 0x18], edi
    ecx = ebx;   // 6d9d09: mov ecx, ebx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9d0b: fild dword ptr [esp + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x859a3c)));   // ST0 = ST0 * m   // 6d9d0f: fmul dword ptr [0x859a3c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d9d15: fstp dword ptr [esp + 0x274]
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();   // 6d9d1c: call 0x53fc10
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d9d21: movsx edx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edx);   // 6d9d24: mov dword ptr [esp + 0x18], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9d28: fild dword ptr [esp + 0x18]
    fp.Fabs();   // 6d9d2c: fabs 
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6d9d2e: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9d34: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9d36: test ah, 0x41
    if (fR != 0) goto L_6d9d5a;   // 6d9d39: jne 0x6d9d5a
    ecx = ebx;   // 6d9d3b: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();   // 6d9d3d: call 0x53fc10
    eax = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d9d42: movsx eax, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(eax);   // 6d9d45: mov dword ptr [esp + 0x18], eax
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9d49: fild dword ptr [esp + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6d9d4d: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d9d53: fstp dword ptr [esp + 0x274]
L_6d9d5a:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9d5a: mov ecx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d9d60: fld dword ptr [esp + 0x274]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d9d67: fmul dword ptr [ecx + 4]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d9d6a: mov edx, dword ptr [0xb7cb5c]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9d70: mov eax, dword ptr [esi + 0x8c]
    esp -= 4; M32(esp) = edx;   // 6d9d76: push edx
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x8631d0)));   // ST0 = ST0 + m   // 6d9d77: fadd dword ptr [0x8631d0]
    esp -= 4; M32(esp) = eax;   // 6d9d7d: push eax
    esp -= 4; M32(esp) = 0x3C03126Fu;   // 6d9d7e: push 0x3c03126f
    esp -= 4; M32(esp) = ecx;   // 6d9d83: push ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9d84: fstp dword ptr [esp + 0x28]
    ecx = (uint32)(esp + 0x44);   // 6d9d88: lea ecx, [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b8c)));   // 6d9d8c: fld dword ptr [0x858b8c]
    edx = (uint32)(esp + 0xfc);   // 6d9d92: lea edx, [esp + 0xfc]
    fp.Fsub((*(float*)(uintptr_t)(uint32)(esi + 0x4c)));   // ST0 = ST0 - m   // 6d9d99: fsub dword ptr [esi + 0x4c]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x44) = (uint32)(0x0u);   // 6d9d9c: mov dword ptr [esp + 0x44], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x48) = (uint32)(0x0u);   // 6d9da4: mov dword ptr [esp + 0x48], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x4c) = (uint32)(0x3F800000u);   // 6d9dac: mov dword ptr [esp + 0x4c], 0x3f800000
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9db4: fstp dword ptr [esp]
    esp -= 4; M32(esp) = ecx;   // 6d9db7: push ecx
    esp -= 4; M32(esp) = edx;   // 6d9db8: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9db9: call 0x40fec0
    esp += 0xCu;   // 6d9dbe: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9dc1: push eax
    eax = (uint32)(esp + 0x114);   // 6d9dc2: lea eax, [esp + 0x114]
    esp -= 4; M32(esp) = eax;   // 6d9dc9: push eax
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9dca: call 0x40fec0
    esp += 0xCu;   // 6d9dcf: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9dd2: push eax
    ecx = (uint32)(esp + 0x128);   // 6d9dd3: lea ecx, [esp + 0x128]
    esp -= 4; M32(esp) = ecx;   // 6d9dda: push ecx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9ddb: call 0x40fec0
    esp += 0xCu;   // 6d9de0: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9de3: push eax
    edx = (uint32)(esp + 0x13c);   // 6d9de4: lea edx, [esp + 0x13c]
    esp -= 4; M32(esp) = edx;   // 6d9deb: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9dec: call 0x40fec0
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9df1: mov edx, dword ptr [eax]
    ecx = esp;   // 6d9df3: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d9df5: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9df7: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9dfa: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d9dfd: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d9e00: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6d9e03: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d9e05: call 0x5429f0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9e0a: fld dword ptr [esp + 0x10]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9e0e: mov ecx, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6d9e14: fmul dword ptr [ecx + 8]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9e17: mov edx, dword ptr [esi + 0x14]
    fA = edx; fB = edx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9e1a: test edx, edx
    eax = (uint32)(edx + 0x30);   // 6d9e1c: lea eax, [edx + 0x30]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = m - ST0   // 6d9e1f: fsubr dword ptr [esp + 0x18]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9e23: fstp dword ptr [esp + 0x18]
    if (fR != 0) goto L_6d9e2c;   // 6d9e27: jne 0x6d9e2c
    eax = (uint32)(esi + 4);   // 6d9e29: lea eax, [esi + 4]
L_6d9e2c:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9e2c: mov eax, dword ptr [eax + 8]
    esp -= 4; M32(esp) = ebp;   // 6d9e2f: push ebp
    esp -= 4; M32(esp) = eax;   // 6d9e30: push eax
    ecx = esi;   // 6d9e31: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d9e33: call 0x6d2600
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9e38: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9e3e: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9e40: test ah, 0x41
    if (fR != 0) goto L_6d9e6f;   // 6d9e43: jne 0x6d9e6f
    fA = edx; fB = edx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9e45: test edx, edx
    eax = (uint32)(edx + 0x30);   // 6d9e47: lea eax, [edx + 0x30]
    if (fR != 0) goto L_6d9e4f;   // 6d9e4a: jne 0x6d9e4f
    eax = (uint32)(esi + 4);   // 6d9e4c: lea eax, [esi + 4]
L_6d9e4f:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9e4f: mov ecx, dword ptr [eax + 8]
    esp -= 4; M32(esp) = ebp;   // 6d9e52: push ebp
    esp -= 4; M32(esp) = ecx;   // 6d9e53: push ecx
    ecx = esi;   // 6d9e54: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d9e56: call 0x6d2600
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x85862c)));   // ST0 = ST0 + m   // 6d9e5b: fadd dword ptr [0x85862c]
    fp.Fdivr((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = m / ST0   // 6d9e61: fdivr dword ptr [esp + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x85862c)));   // ST0 = ST0 * m   // 6d9e65: fmul dword ptr [0x85862c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9e6b: fstp dword ptr [esp + 0x18]
L_6d9e6f:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9e6f: fld dword ptr [esp + 0x1c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d9e73: mov edx, dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9e79: fmul dword ptr [0x863984]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9e7f: mov eax, dword ptr [esi + 0x8c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x18));   // 6d9e85: mov ecx, dword ptr [esp + 0x18]
    esp -= 4; M32(esp) = edx;   // 6d9e89: push edx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d9e8a: fstp dword ptr [esp + 0x38]
    esp -= 4; M32(esp) = eax;   // 6d9e8e: push eax
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9e8f: fld dword ptr [esp + 0x28]
    esp -= 4; M32(esp) = ecx;   // 6d9e93: push ecx
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9e94: fmul dword ptr [0x863984]
    edx = (uint32)(esp + 0x40);   // 6d9e9a: lea edx, [esp + 0x40]
    esp -= 4; M32(esp) = edx;   // 6d9e9e: push edx
    eax = (uint32)(esp + 0x15c);   // 6d9e9f: lea eax, [esp + 0x15c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9ea6: fstp dword ptr [esp + 0x48]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d9eaa: fld dword ptr [esp + 0x34]
    esp -= 4; M32(esp) = eax;   // 6d9eae: push eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9eaf: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d9eb5: fstp dword ptr [esp + 0x50]
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9eb9: call 0x40fec0
    esp += 0xCu;   // 6d9ebe: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9ec1: push eax
    ecx = (uint32)(esp + 0x170);   // 6d9ec2: lea ecx, [esp + 0x170]
    esp -= 4; M32(esp) = ecx;   // 6d9ec9: push ecx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9eca: call 0x40fec0
    esp += 0xCu;   // 6d9ecf: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9ed2: push eax
    edx = (uint32)(esp + 0x184);   // 6d9ed3: lea edx, [esp + 0x184]
    esp -= 4; M32(esp) = edx;   // 6d9eda: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9edb: call 0x40fec0
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9ee0: mov edx, dword ptr [eax]
    ecx = esp;   // 6d9ee2: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d9ee4: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9ee6: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9ee9: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d9eec: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d9eef: mov dword ptr [ecx + 8], eax
L_6d9ef2:;
    ecx = esi;   // 6d9ef2: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d9ef4: call 0x5429f0
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9ef9: mov ebx, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebx + 0x28)));   // 6d9efc: fld dword ptr [ebx + 0x28]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9eff: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9f05: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9f07: test ah, 0x41
    if (fR != 0) goto L_6da142;   // 6d9f0a: jne 0x6da142
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9f10: mov ecx, dword ptr [esi + 0x388]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(0x0u);   // 6d9f16: mov dword ptr [esp + 0x28], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(0x0u);   // 6d9f1e: mov dword ptr [esp + 0x2c], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(0x3F800000u);   // 6d9f26: mov dword ptr [esp + 0x30], 0x3f800000
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 0x38));   // 6d9f2e: mov edx, dword ptr [ecx + 0x38]
    esp -= 4; M32(esp) = 0xC813E0u;   // 6d9f31: push 0xc813e0
    esp -= 4; M32(esp) = edx;   // 6d9f36: push edx
    eax = (uint32)(esp + 0x19c);   // 6d9f37: lea eax, [esp + 0x19c]
    esp -= 4; M32(esp) = eax;   // 6d9f3e: push eax
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9f3f: call 0x40fe90
    esp += 0xCu;   // 6d9f44: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9f47: push eax
    ecx = (uint32)(esp + 0x2c);   // 6d9f48: lea ecx, [esp + 0x2c]
    FlcVecAddTo(fp, (float*)(uintptr_t)ecx, (float*)(uintptr_t)M32(esp + 0)); eax = ecx; esp += 4;   // 6d9f4c: call 0x411a00
    ecx = (uint32)(esp + 0x28);   // 6d9f51: lea ecx, [esp + 0x28]
    FlcVecNormalise(fp, (float*)(uintptr_t)ecx);   // 6d9f55: call 0x59c910
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9f5a: mov ebp, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebp + 0x28)));   // 6d9f60: fld dword ptr [ebp + 0x28]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9f63: mov edi, dword ptr [esi + 0x14]
    fp.Fchs();   // 6d9f66: fchs 
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9f68: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9f6c: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9f6f: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d9f73: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9f76: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9f78: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9f7c: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9f7e: faddp st(1)
    fp.FldSt(1);   // 6d9f80: fld st(1)
    fp.Fcompp();   // 6d9f82: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9f84: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9f86: test ah, 0x41
    if (fR != 0) goto L_6d9f8f;   // 6d9f89: jne 0x6d9f8f
    fp.FldSt(0);   // 6d9f8b: fld st(0)
    goto L_6d9fa7;   // 6d9f8d: jmp 0x6d9fa7
L_6d9f8f:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9f8f: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9f93: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9f96: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d9f9a: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9f9d: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9f9f: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9fa3: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9fa5: faddp st(1)
L_6d9fa7:;
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(ebp + 0x28)), true);   // 6d9fa7: fcomp dword ptr [ebp + 0x28]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9faa: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9fac: test ah, 0x41
    if (fR != 0) goto L_6d9fbc;   // 6d9faf: jne 0x6d9fbc
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebp + 0x28));   // 6d9fb1: mov ecx, dword ptr [ebp + 0x28]
    fp.FstpSt(0);   // 6d9fb4: fstp st(0)
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(ecx);   // 6d9fb6: mov dword ptr [esp + 0x18], ecx
    goto L_6d9ffd;   // 6d9fba: jmp 0x6d9ffd
L_6d9fbc:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9fbc: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9fc0: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9fc3: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d9fc7: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9fca: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9fcc: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9fd0: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9fd2: faddp st(1)
    fp.FldSt(1);   // 6d9fd4: fld st(1)
    fp.Fcompp();   // 6d9fd6: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9fd8: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9fda: test ah, 0x41
    if (fR == 0) goto L_6d9ff9;   // 6d9fdd: je 0x6d9ff9
    fp.FstpSt(0);   // 6d9fdf: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9fe1: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9fe5: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9fe8: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d9fec: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9fef: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9ff1: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9ff5: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9ff7: faddp st(1)
L_6d9ff9:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9ff9: fstp dword ptr [esp + 0x18]
L_6d9ffd:;
    edx = (uint32)(esp + 0x50);   // 6d9ffd: lea edx, [esp + 0x50]
    esp -= 4; M32(esp) = edx;   // 6da001: push edx
    eax = (uint32)(esp + 0x1b0);   // 6da002: lea eax, [esp + 0x1b0]
    esp -= 4; M32(esp) = edi;   // 6da009: push edi
    esp -= 4; M32(esp) = eax;   // 6da00a: push eax
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da00b: call 0x40fe30
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da010: fld dword ptr [esp + 0x24]
    fp.Fchs();   // 6da014: fchs 
    esp += 0xCu;   // 6da016: add esp, 0xc
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ebp + 0x2c)));   // ST0 = ST0 * m   // 6da019: fmul dword ptr [ebp + 0x2c]
    fA = edi; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6da01c: add edi, 0x20
    esp -= 4; M32(esp) = edi;   // 6da01f: push edi
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da020: fmul dword ptr [0xb7cb5c]
    esp -= 4; M32(esp) = ecx;   // 6da026: push ecx
    ecx = (uint32)(esp + 0x1cc);   // 6da027: lea ecx, [esp + 0x1cc]
    ebx = eax;   // 6da02e: mov ebx, eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6da030: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6da036: fstp dword ptr [esp]
    esp -= 4; M32(esp) = ecx;   // 6da039: push ecx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da03a: call 0x40fe90
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebx));   // 6da03f: mov ecx, dword ptr [ebx]
    edx = esp;   // 6da041: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6da043: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebx + 4));   // 6da045: mov ecx, dword ptr [ebx + 4]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da048: mov dword ptr [edx + 4], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebx + 8));   // 6da04b: mov ecx, dword ptr [ebx + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(ecx);   // 6da04e: mov dword ptr [edx + 8], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6da051: mov ecx, dword ptr [eax]
    esp -= 0xCu;   // 6da053: sub esp, 0xc
    edx = esp;   // 6da056: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6da058: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6da05a: mov ecx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6da05d: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da060: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6da063: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da065: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da068: call 0x542a50
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da06d: mov ebx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebx + 0x28)));   // 6da073: fld dword ptr [ebx + 0x28]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da076: mov ebp, dword ptr [esi + 0x14]
    fp.Fchs();   // 6da079: fchs 
    ecx = (uint32)(ebp + 0x10);   // 6da07b: lea ecx, [ebp + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da07e: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6da082: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da085: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6da089: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6da08c: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da08e: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6da092: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6da094: faddp st(1)
    fp.FldSt(1);   // 6da096: fld st(1)
    fp.Fcompp();   // 6da098: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da09a: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6da09c: test ah, 0x41
    if (fR != 0) goto L_6da0a5;   // 6da09f: jne 0x6da0a5
    fp.FldSt(0);   // 6da0a1: fld st(0)
    goto L_6da0bd;   // 6da0a3: jmp 0x6da0bd
L_6da0a5:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da0a5: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6da0a9: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da0ac: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6da0b0: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6da0b3: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da0b5: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6da0b9: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6da0bb: faddp st(1)
L_6da0bd:;
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(ebx + 0x28)), true);   // 6da0bd: fcomp dword ptr [ebx + 0x28]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da0c0: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6da0c2: test ah, 0x41
    if (fR != 0) goto L_6da0d2;   // 6da0c5: jne 0x6da0d2
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebx + 0x28));   // 6da0c7: mov edx, dword ptr [ebx + 0x28]
    fp.FstpSt(0);   // 6da0ca: fstp st(0)
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edx);   // 6da0cc: mov dword ptr [esp + 0x18], edx
    goto L_6da113;   // 6da0d0: jmp 0x6da113
L_6da0d2:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da0d2: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6da0d6: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da0d9: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6da0dd: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6da0e0: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da0e2: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6da0e6: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6da0e8: faddp st(1)
    fp.FldSt(1);   // 6da0ea: fld st(1)
    fp.Fcompp();   // 6da0ec: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da0ee: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6da0f0: test ah, 0x41
    if (fR == 0) goto L_6da10f;   // 6da0f3: je 0x6da10f
    fp.FstpSt(0);   // 6da0f5: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da0f7: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6da0fb: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da0fe: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6da102: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6da105: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da107: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6da10b: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6da10d: faddp st(1)
L_6da10f:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da10f: fstp dword ptr [esp + 0x18]
L_6da113:;
    eax = (uint32)(esp + 0x50);   // 6da113: lea eax, [esp + 0x50]
    esp -= 4; M32(esp) = eax;   // 6da117: push eax
    esp -= 4; M32(esp) = ecx;   // 6da118: push ecx
    ecx = (uint32)(esp + 0x1e4);   // 6da119: lea ecx, [esp + 0x1e4]
    esp -= 4; M32(esp) = ecx;   // 6da120: push ecx
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da121: call 0x40fe30
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da126: fld dword ptr [esp + 0x24]
    esp += 0xCu;   // 6da12a: add esp, 0xc
    fp.Fchs();   // 6da12d: fchs 
    fA = ebp; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; ebp = fR;   // 6da12f: add ebp, 0x20
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ebx + 0x2c)));   // ST0 = ST0 * m   // 6da132: fmul dword ptr [ebx + 0x2c]
    esp -= 4; M32(esp) = ebp;   // 6da135: push ebp
    edx = (uint32)(esp + 0x1f8);   // 6da136: lea edx, [esp + 0x1f8]
    goto L_6da22e;   // 6da13d: jmp 0x6da22e
L_6da142:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebx + 8)));   // 6da142: fld dword ptr [ebx + 8]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da145: mov ebp, dword ptr [esi + 0x388]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6da14b: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da151: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6da153: test ah, 5
    if (FlcParityEven(fR)) goto L_6da161;   // 6da156: jp 0x6da161
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebp + 0x28));   // 6da158: mov ecx, dword ptr [ebp + 0x28]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(ecx);   // 6da15b: mov dword ptr [esp + 0x18], ecx
    goto L_6da16a;   // 6da15f: jmp 0x6da16a
L_6da161:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebp + 0x28)));   // 6da161: fld dword ptr [ebp + 0x28]
    fp.Fchs();   // 6da164: fchs 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da166: fstp dword ptr [esp + 0x18]
L_6da16a:;
    edx = (uint32)(esp + 0x50);   // 6da16a: lea edx, [esp + 0x50]
    esp -= 4; M32(esp) = edx;   // 6da16e: push edx
    eax = (uint32)(esp + 0x210);   // 6da16f: lea eax, [esp + 0x210]
    esp -= 4; M32(esp) = ebx;   // 6da176: push ebx
    esp -= 4; M32(esp) = eax;   // 6da177: push eax
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da178: call 0x40fe30
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da17d: fld dword ptr [esp + 0x24]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ebp + 0x2c)));   // ST0 = ST0 * m   // 6da181: fmul dword ptr [ebp + 0x2c]
    esp += 0xCu;   // 6da184: add esp, 0xc
    fA = ebx; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; ebx = fR;   // 6da187: add ebx, 0x20
    esp -= 4; M32(esp) = ebx;   // 6da18a: push ebx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da18b: fmul dword ptr [0xb7cb5c]
    esp -= 4; M32(esp) = ecx;   // 6da191: push ecx
    ecx = (uint32)(esp + 0x22c);   // 6da192: lea ecx, [esp + 0x22c]
    edi = eax;   // 6da199: mov edi, eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6da19b: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6da1a1: fstp dword ptr [esp]
    esp -= 4; M32(esp) = ecx;   // 6da1a4: push ecx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da1a5: call 0x40fe90
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi));   // 6da1aa: mov ecx, dword ptr [edi]
    edx = esp;   // 6da1ac: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6da1ae: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi + 4));   // 6da1b0: mov ecx, dword ptr [edi + 4]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da1b3: mov dword ptr [edx + 4], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi + 8));   // 6da1b6: mov ecx, dword ptr [edi + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(ecx);   // 6da1b9: mov dword ptr [edx + 8], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6da1bc: mov ecx, dword ptr [eax]
    esp -= 0xCu;   // 6da1be: sub esp, 0xc
    edx = esp;   // 6da1c1: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6da1c3: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6da1c5: mov ecx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6da1c8: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da1cb: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6da1ce: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da1d0: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da1d3: call 0x542a50
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da1d8: mov ebx, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebx + 0x18)));   // 6da1db: fld dword ptr [ebx + 0x18]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da1de: mov ebp, dword ptr [esi + 0x388]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6da1e4: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da1ea: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6da1ec: test ah, 5
    if (FlcParityEven(fR)) goto L_6da1fa;   // 6da1ef: jp 0x6da1fa
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebp + 0x28));   // 6da1f1: mov ecx, dword ptr [ebp + 0x28]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(ecx);   // 6da1f4: mov dword ptr [esp + 0x18], ecx
    goto L_6da203;   // 6da1f8: jmp 0x6da203
L_6da1fa:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebp + 0x28)));   // 6da1fa: fld dword ptr [ebp + 0x28]
    fp.Fchs();   // 6da1fd: fchs 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da1ff: fstp dword ptr [esp + 0x18]
L_6da203:;
    edx = (uint32)(esp + 0x50);   // 6da203: lea edx, [esp + 0x50]
    esp -= 4; M32(esp) = edx;   // 6da207: push edx
    eax = (uint32)(ebx + 0x10);   // 6da208: lea eax, [ebx + 0x10]
    esp -= 4; M32(esp) = eax;   // 6da20b: push eax
    ecx = (uint32)(esp + 0x244);   // 6da20c: lea ecx, [esp + 0x244]
    esp -= 4; M32(esp) = ecx;   // 6da213: push ecx
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da214: call 0x40fe30
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da219: fld dword ptr [esp + 0x24]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ebp + 0x2c)));   // ST0 = ST0 * m   // 6da21d: fmul dword ptr [ebp + 0x2c]
    esp += 0xCu;   // 6da220: add esp, 0xc
    fA = ebx; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; ebx = fR;   // 6da223: add ebx, 0x20
    esp -= 4; M32(esp) = ebx;   // 6da226: push ebx
    edx = (uint32)(esp + 0x84);   // 6da227: lea edx, [esp + 0x84]
L_6da22e:;
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da22e: fmul dword ptr [0xb7cb5c]
    esp -= 4; M32(esp) = ecx;   // 6da234: push ecx
    edi = eax;   // 6da235: mov edi, eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6da237: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6da23d: fstp dword ptr [esp]
    esp -= 4; M32(esp) = edx;   // 6da240: push edx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da241: call 0x40fe90
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi));   // 6da246: mov edx, dword ptr [edi]
    ecx = esp;   // 6da248: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6da24a: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi + 4));   // 6da24c: mov edx, dword ptr [edi + 4]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6da24f: mov dword ptr [ecx + 4], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi + 8));   // 6da252: mov edx, dword ptr [edi + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6da255: mov dword ptr [ecx + 8], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6da258: mov edx, dword ptr [eax]
    esp -= 0xCu;   // 6da25a: sub esp, 0xc
    ecx = esp;   // 6da25d: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6da25f: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6da261: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6da264: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6da267: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6da26a: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6da26d: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da26f: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da274: fld dword ptr [esp + 0x26c]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da27b: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFFFF00u) | (((uint32)(*(uint8*)(uintptr_t)(uint32)(0xc1c970))) & 0xFFu);   // 6da281: mov al, byte ptr [0xc1c970]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x14));   // 6da286: mov edi, dword ptr [esp + 0x14]
    fA = (eax & 0xFFu); fB = (eax & 0xFFu); fR = (fA & fB) & 0xFFu;   // 6da28a: test al, al
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da28c: fnstsw ax
    if (fR != 0) goto L_6da3a1;   // 6da28e: jne 0x6da3a1
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da294: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da2c7;   // 6da297: jp 0x6da2c7
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da299: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x26c) = (uint32)(0x0u);   // 6da29b: mov dword ptr [esp + 0x26c], 0
    if (fR == 0) goto L_6da2c7;   // 6da2a6: je 0x6da2c7
    ecx = edi;   // 6da2a8: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringUpDown();   // 6da2aa: call 0x53fbd0
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da2af: movsx ecx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ecx);   // 6da2b2: mov dword ptr [esp + 0x14], ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da2b6: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6da2ba: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da2c0: fstp dword ptr [esp + 0x26c]
L_6da2c7:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da2c7: fld dword ptr [esp + 0x270]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da2ce: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da2d4: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da2d6: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da30b;   // 6da2d9: jp 0x6da30b
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da2db: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x270) = (uint32)(0x0u);   // 6da2dd: mov dword ptr [esp + 0x270], 0
    if (fR == 0) goto L_6da30b;   // 6da2e8: je 0x6da30b
    ecx = edi;   // 6da2ea: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringLeftRight();   // 6da2ec: call 0x53fb80
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da2f1: movsx edx, ax
    fR = (uint32)(0 - edx); edx = fR;   // 6da2f4: neg edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da2f6: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da2fa: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6da2fe: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da304: fstp dword ptr [esp + 0x270]
L_6da30b:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da30b: fld dword ptr [esp + 0x268]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da312: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da318: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da31a: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da468;   // 6da31d: jp 0x6da468
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da323: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x268) = (uint32)(0x0u);   // 6da325: mov dword ptr [esp + 0x268], 0
    if (fR == 0) goto L_6da468;   // 6da330: je 0x6da468
    ecx = edi;   // 6da336: mov ecx, edi
    eax = ((CPad*)(uintptr_t)ecx)->GetLookRight() ? 1u : 0u;   // 6da338: call 0x53fe10
    eax = (eax & 0xFFu);   // 6da33d: movzx eax, al
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(eax);   // 6da340: mov dword ptr [esp + 0x14], eax
    ecx = edi;   // 6da344: mov ecx, edi
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da346: fild dword ptr [esp + 0x14]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da34a: fstp dword ptr [esp + 0x268]
    eax = ((CPad*)(uintptr_t)ecx)->GetLookLeft() ? 1u : 0u;   // 6da351: call 0x53fdd0
    fA = (eax & 0xFFu); fB = (eax & 0xFFu); fR = (fA & fB) & 0xFFu;   // 6da356: test al, al
    if (fR == 0) goto L_6da365;   // 6da358: je 0x6da365
    *(uint32*)(uintptr_t)(uint32)(esp + 0x268) = (uint32)(0xBF800000u);   // 6da35a: mov dword ptr [esp + 0x268], 0xbf800000
L_6da365:;
    ecx = edi;   // 6da365: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunLeftRight();   // 6da367: call 0x53fc50
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da36c: movsx ecx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ecx);   // 6da36f: mov dword ptr [esp + 0x14], ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da373: fild dword ptr [esp + 0x14]
    fp.Fabs();   // 6da377: fabs 
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6da379: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da37f: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6da381: test ah, 0x41
    if (fR != 0) goto L_6da468;   // 6da384: jne 0x6da468
    ecx = edi;   // 6da38a: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunLeftRight();   // 6da38c: call 0x53fc50
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da391: movsx edx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da394: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da398: fild dword ptr [esp + 0x14]
    goto L_6da45b;   // 6da39c: jmp 0x6da45b
L_6da3a1:;
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da3a1: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da3d4;   // 6da3a4: jp 0x6da3d4
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da3a6: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x26c) = (uint32)(0x0u);   // 6da3a8: mov dword ptr [esp + 0x26c], 0
    if (fR == 0) goto L_6da3d4;   // 6da3b3: je 0x6da3d4
    ecx = edi;   // 6da3b5: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringUpDown();   // 6da3b7: call 0x53fbd0
    eax = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da3bc: movsx eax, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(eax);   // 6da3bf: mov dword ptr [esp + 0x14], eax
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da3c3: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6da3c7: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da3cd: fstp dword ptr [esp + 0x26c]
L_6da3d4:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da3d4: fld dword ptr [esp + 0x270]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da3db: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da3e1: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da3e3: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da410;   // 6da3e6: jp 0x6da410
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da3e8: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x270) = (uint32)(0x0u);   // 6da3ea: mov dword ptr [esp + 0x270], 0
    if (fR == 0) goto L_6da410;   // 6da3f5: je 0x6da410
    ecx = edi;   // 6da3f7: mov ecx, edi
    eax = ((CPad*)(uintptr_t)ecx)->GetLookLeft() ? 1u : 0u;   // 6da3f9: call 0x53fdd0
    ecx = (eax & 0xFFu);   // 6da3fe: movzx ecx, al
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ecx);   // 6da401: mov dword ptr [esp + 0x14], ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da405: fild dword ptr [esp + 0x14]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da409: fstp dword ptr [esp + 0x270]
L_6da410:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da410: fld dword ptr [esp + 0x268]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da417: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da41d: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da41f: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da468;   // 6da422: jp 0x6da468
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da424: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x268) = (uint32)(0x0u);   // 6da426: mov dword ptr [esp + 0x268], 0
    if (fR == 0) goto L_6da468;   // 6da431: je 0x6da468
    ecx = edi;   // 6da433: mov ecx, edi
    eax = ((CPad*)(uintptr_t)ecx)->GetLookRight() ? 1u : 0u;   // 6da435: call 0x53fe10
    fA = (eax & 0xFFu); fB = (eax & 0xFFu); fR = (fA & fB) & 0xFFu;   // 6da43a: test al, al
    if (fR == 0) goto L_6da449;   // 6da43c: je 0x6da449
    *(uint32*)(uintptr_t)(uint32)(esp + 0x270) = (uint32)(0xBF800000u);   // 6da43e: mov dword ptr [esp + 0x270], 0xbf800000
L_6da449:;
    ecx = edi;   // 6da449: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringLeftRight();   // 6da44b: call 0x53fb80
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da450: movsx edx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da453: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da457: fild dword ptr [esp + 0x14]
L_6da45b:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6da45b: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da461: fstp dword ptr [esp + 0x268]
L_6da468:;
    fA = (uint32)(*(uint8*)(uintptr_t)(uint32)(esi + 0x42b)); fB = 0x10u; fR = (fA & fB) & 0xFFu;   // 6da468: test byte ptr [esi + 0x42b], 0x10
    if (fR == 0) goto L_6da4ae;   // 6da46f: je 0x6da4ae
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da471: mov edi, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // 6da477: fld dword ptr [edi + 0x20]
    fp.Fdiv((*(float*)(uintptr_t)(uint32)(edi + 0x2c)));   // ST0 = ST0 / m   // 6da47a: fdiv dword ptr [edi + 0x2c]
    FlcCrtAsin(fp);   // 6da47d: call 0x821e70
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c84)));   // ST0 = ST0 * m   // 6da482: fmul dword ptr [0x858c84]
    fp.Fsin();   // 6da488: fsin 
    fp.Fdiv((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // ST0 = ST0 / m   // 6da48a: fdiv dword ptr [edi + 0x20]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x2c)));   // ST0 = ST0 * m   // 6da48d: fmul dword ptr [edi + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da490: fld dword ptr [esp + 0x26c]
    fp.Arith('*', 0, 0, 1, false);   // 6da497: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da499: fstp dword ptr [esp + 0x26c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // ST0 = ST0 * m   // 6da4a0: fmul dword ptr [esp + 0x270]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da4a7: fstp dword ptr [esp + 0x270]
L_6da4ae:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da4ae: mov eax, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da4b1: fld dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // ST0 = ST0 + m   // 6da4b5: fadd dword ptr [eax + 0x10]
    esp -= 0xCu;   // 6da4b8: sub esp, 0xc
    edx = esp;   // 6da4bb: mov edx, esp
    esp -= 0xCu;   // 6da4bd: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6da4c0: fstp dword ptr [esp + 0x4c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // 6da4c4: fld dword ptr [esp + 0x6c]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 0x14)));   // ST0 = ST0 + m   // 6da4c8: fadd dword ptr [eax + 0x14]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da4cb: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6da4cf: fld dword ptr [esp + 0x70]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // ST0 = ST0 + m   // 6da4d3: fadd dword ptr [eax + 0x18]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da4d6: fstp dword ptr [esp + 0x54]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x284)));   // 6da4da: fld dword ptr [esp + 0x284]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x20)));   // ST0 = ST0 * m   // 6da4e1: fmul dword ptr [eax + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x284)));   // 6da4e4: fld dword ptr [esp + 0x284]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x24)));   // ST0 = ST0 * m   // 6da4eb: fmul dword ptr [eax + 0x24]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x284)));   // 6da4ee: fld dword ptr [esp + 0x284]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x28)));   // ST0 = ST0 * m   // 6da4f5: fmul dword ptr [eax + 0x28]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da4f8: mov eax, dword ptr [esi + 0x388]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 0x20));   // 6da4fe: mov ecx, dword ptr [eax + 0x20]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(ecx);   // 6da501: mov dword ptr [esp + 0x2c], ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da505: fstp dword ptr [esp + 0x7c]
    fp.Fxch(1);   // 6da509: fxch st(1)
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6da50b: mov eax, dword ptr [esp + 0x4c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da50f: fmul dword ptr [esp + 0x2c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6da513: mov ecx, dword ptr [esp + 0x50]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6da517: mov dword ptr [edx], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6da519: mov eax, dword ptr [esp + 0x54]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da51d: fstp dword ptr [esp + 0x34]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da521: mov dword ptr [edx + 4], ecx
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da524: mov dword ptr [edx + 8], eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da527: fmul dword ptr [esp + 0x2c]
    ecx = esp;   // 6da52b: mov ecx, esp
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da52d: fld dword ptr [esp + 0x7c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da531: fmul dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // 6da535: fld dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da53b: fld dword ptr [esp + 0x34]
    fp.Arith('*', 0, 0, 1, false);   // 6da53f: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da541: fstp dword ptr [esp + 0x40]
    fp.Fxch(2);   // 6da545: fxch st(2)
    fp.Arith('*', 0, 0, 2, false);   // 6da547: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da549: fstp dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6da54d: fmul st(1)
    fp.Fxch(1);   // 6da54f: fxch st(1)
    fp.FstpSt(0);   // 6da551: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da553: fld dword ptr [esp + 0x40]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da557: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da55d: fstp dword ptr [esp + 0x34]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6da561: mov edx, dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da565: fld dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6da569: mov dword ptr [ecx], edx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da56b: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6da571: fstp dword ptr [esp + 0x38]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6da575: mov eax, dword ptr [esp + 0x38]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da579: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6da57f: mov dword ptr [ecx + 4], eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6da582: fstp dword ptr [esp + 0x3c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6da586: mov edx, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6da58a: mov dword ptr [ecx + 8], edx
    ecx = esi;   // 6da58d: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da58f: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da594: fld dword ptr [esp + 0x50]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da598: mov eax, dword ptr [esi + 0x14]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 + m   // 6da59b: fadd dword ptr [eax]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da59d: fstp dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da5a1: fld dword ptr [esp + 0x54]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 + m   // 6da5a5: fadd dword ptr [eax + 4]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6da5a8: fstp dword ptr [esp + 0x38]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x58)));   // 6da5ac: fld dword ptr [esp + 0x58]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 + m   // 6da5b0: fadd dword ptr [eax + 8]
    esp -= 0xCu;   // 6da5b3: sub esp, 0xc
    edx = esp;   // 6da5b6: mov edx, esp
    esp -= 0xCu;   // 6da5b8: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da5bb: fstp dword ptr [esp + 0x54]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x288)));   // 6da5bf: fld dword ptr [esp + 0x288]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x20)));   // ST0 = ST0 * m   // 6da5c6: fmul dword ptr [eax + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x288)));   // 6da5c9: fld dword ptr [esp + 0x288]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x24)));   // ST0 = ST0 * m   // 6da5d0: fmul dword ptr [eax + 0x24]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x288)));   // 6da5d3: fld dword ptr [esp + 0x288]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x28)));   // ST0 = ST0 * m   // 6da5da: fmul dword ptr [eax + 0x28]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da5dd: mov eax, dword ptr [esi + 0x388]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 0x18));   // 6da5e3: mov ecx, dword ptr [eax + 0x18]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(ecx);   // 6da5e6: mov dword ptr [esp + 0x2c], ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da5ea: fstp dword ptr [esp + 0x7c]
    fp.Fxch(1);   // 6da5ee: fxch st(1)
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6da5f0: mov eax, dword ptr [esp + 0x4c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da5f4: fmul dword ptr [esp + 0x2c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6da5f8: mov ecx, dword ptr [esp + 0x50]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6da5fc: mov dword ptr [edx], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6da5fe: mov eax, dword ptr [esp + 0x54]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da602: fstp dword ptr [esp + 0x34]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da606: mov dword ptr [edx + 4], ecx
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da609: mov dword ptr [edx + 8], eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da60c: fmul dword ptr [esp + 0x2c]
    ecx = esp;   // 6da610: mov ecx, esp
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da612: fld dword ptr [esp + 0x7c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da616: fmul dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // 6da61a: fld dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da620: fld dword ptr [esp + 0x34]
    fp.Arith('*', 0, 0, 1, false);   // 6da624: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da626: fstp dword ptr [esp + 0x40]
    fp.Fxch(2);   // 6da62a: fxch st(2)
    fp.Arith('*', 0, 0, 2, false);   // 6da62c: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da62e: fstp dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6da632: fmul st(1)
    fp.Fxch(1);   // 6da634: fxch st(1)
    fp.FstpSt(0);   // 6da636: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da638: fld dword ptr [esp + 0x40]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da63c: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da642: fstp dword ptr [esp + 0x34]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6da646: mov edx, dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da64a: fld dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6da64e: mov dword ptr [ecx], edx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da650: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6da656: fstp dword ptr [esp + 0x38]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6da65a: mov eax, dword ptr [esp + 0x38]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da65e: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6da664: mov dword ptr [ecx + 4], eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6da667: fstp dword ptr [esp + 0x3c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6da66b: mov edx, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6da66f: mov dword ptr [ecx + 8], edx
    ecx = esi;   // 6da672: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da674: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6da679: fld dword ptr [esp + 0x48]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da67d: mov eax, dword ptr [esi + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 * m   // 6da680: fmul dword ptr [eax + 8]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6da683: mov ecx, dword ptr [eax]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da685: fld dword ptr [esp + 0x44]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6da689: mov edx, dword ptr [eax + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 * m   // 6da68c: fmul dword ptr [eax + 4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(ecx);   // 6da68f: mov dword ptr [esp + 0x1c], ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x20) = (uint32)(edx);   // 6da693: mov dword ptr [esp + 0x20], edx
    fp.Arith('+', 1, 1, 0, true);   // 6da697: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da699: fld dword ptr [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 * m   // 6da69d: fmul dword ptr [eax]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6da69f: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x24) = (uint32)(eax);   // 6da6a2: mov dword ptr [esp + 0x24], eax
    fp.Arith('+', 1, 1, 0, true);   // 6da6a6: faddp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6da6a8: fmul dword ptr [0x858c1c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da6ae: fstp dword ptr [esp + 0x18]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da6b2: fld dword ptr [esp + 0x18]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da6b6: mov ecx, dword ptr [esi + 0x388]
    fp.Fabs();   // 6da6bc: fabs 
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6da6be: mov edx, dword ptr [esi + 0x8c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6da6c4: fstp dword ptr [esp + 0x10]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da6c8: mov dword ptr [esp + 0x14], edx
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6da6cc: fld dword ptr [esp + 0x10]
    esp -= 0xCu;   // 6da6d0: sub esp, 0xc
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x14)));   // ST0 = ST0 * m   // 6da6d3: fmul dword ptr [ecx + 0x14]
    eax = esp;   // 6da6d6: mov eax, esp
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // ST0 = ST0 * m   // 6da6d8: fmul dword ptr [esp + 0x24]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da6dc: fld dword ptr [esp + 0x28]
    fp.Arith('*', 0, 0, 1, false);   // 6da6e0: fmul st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da6e2: fld dword ptr [esp + 0x2c]
    fp.Arith('*', 0, 0, 2, false);   // 6da6e6: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da6e8: fstp dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da6ec: fld dword ptr [esp + 0x30]
    fp.Arith('*', 0, 0, 2, false);   // 6da6f0: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da6f2: fstp dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6da6f6: fmul dword ptr [esp + 0x20]
    fp.FstpSt(1);   // 6da6fa: fstp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da6fc: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6da700: fmul dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da704: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6da708: fmul dword ptr [esp + 0x20]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6da70c: fstp dword ptr [esp + 0x70]
    fp.Fxch(1);   // 6da710: fxch st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da712: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da718: fstp dword ptr [esp + 0x40]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6da71c: mov ecx, dword ptr [esp + 0x40]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6da720: mov dword ptr [eax], ecx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da722: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da728: fstp dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6da72c: fld dword ptr [esp + 0x70]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6da730: mov edx, dword ptr [esp + 0x44]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da734: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6da73a: mov dword ptr [eax + 4], edx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6da73d: fstp dword ptr [esp + 0x48]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6da741: mov ecx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6da745: mov dword ptr [eax + 8], ecx
    ecx = esi;   // 6da748: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6da74a: call 0x5429f0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6da74f: fld dword ptr [esp + 0x10]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da753: mov eax, dword ptr [esi + 0x14]
    edx = eax;   // 6da756: mov edx, eax
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx));   // 6da758: mov ecx, dword ptr [edx]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(ecx);   // 6da75a: mov dword ptr [esp + 0x1c], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 4));   // 6da75e: mov ecx, dword ptr [edx + 4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x20) = (uint32)(ecx);   // 6da761: mov dword ptr [esp + 0x20], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da765: mov ecx, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x10)));   // ST0 = ST0 * m   // 6da76b: fmul dword ptr [ecx + 0x10]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 8));   // 6da76e: mov edx, dword ptr [edx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x24) = (uint32)(edx);   // 6da771: mov dword ptr [esp + 0x24], edx
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = ST0 * m   // 6da775: fmul dword ptr [esp + 0x18]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da779: fld dword ptr [esp + 0x268]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0xc)));   // ST0 = ST0 * m   // 6da780: fmul dword ptr [ecx + 0xc]
    fp.Arith('+', 1, 1, 0, true);   // 6da783: faddp st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da785: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6da78b: fmul dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da791: fld dword ptr [esp + 0x1c]
    fp.Arith('*', 0, 0, 1, false);   // 6da795: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da797: fstp dword ptr [esp + 0x1c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6da79b: fld dword ptr [esp + 0x20]
    fp.Arith('*', 0, 0, 1, false);   // 6da79f: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6da7a1: fstp dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da7a5: fld dword ptr [esp + 0x24]
    fp.Arith('*', 0, 0, 1, false);   // 6da7a9: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da7ab: fstp dword ptr [esp + 0x24]
    fp.FstpSt(0);   // 6da7af: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // 6da7b1: fld dword ptr [eax + 0x10]
    fp.Fchs();   // 6da7b4: fchs 
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 0x14)));   // 6da7b6: fld dword ptr [eax + 0x14]
    fp.Fchs();   // 6da7b9: fchs 
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // 6da7bb: fld dword ptr [eax + 0x18]
    fp.Fchs();   // 6da7be: fchs 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x64)));   // 6da7c0: fstp dword ptr [esp + 0x64]
    fp.Fxch(1);   // 6da7c4: fxch st(1)
    esp -= 0xCu;   // 6da7c6: sub esp, 0xc
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x5c)));   // ST0 = ST0 + m   // 6da7c9: fadd dword ptr [esp + 0x5c]
    eax = esp;   // 6da7cd: mov eax, esp
    esp -= 0xCu;   // 6da7cf: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6da7d2: fstp dword ptr [esp + 0x4c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6da7d6: mov ecx, dword ptr [esp + 0x4c]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6da7da: mov dword ptr [eax], ecx
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // ST0 = ST0 + m   // 6da7dc: fadd dword ptr [esp + 0x6c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da7e0: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da7e4: fld dword ptr [esp + 0x7c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6da7e8: mov edx, dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // ST0 = ST0 + m   // 6da7ec: fadd dword ptr [esp + 0x70]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6da7f0: mov dword ptr [eax + 4], edx
    edx = esp;   // 6da7f3: mov edx, esp
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da7f5: fstp dword ptr [esp + 0x54]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6da7f9: mov ecx, dword ptr [esp + 0x54]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6da7fd: mov dword ptr [eax + 8], ecx
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6da800: mov eax, dword ptr [esp + 0x34]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6da804: mov ecx, dword ptr [esp + 0x38]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6da808: mov dword ptr [edx], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6da80a: mov eax, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da80e: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6da811: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da813: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da816: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da81b: fld dword ptr [esp + 0x50]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da81f: mov eax, dword ptr [esi + 0x14]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 + m   // 6da822: fadd dword ptr [eax]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da824: mov ecx, dword ptr [esi + 0x388]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 0xc));   // 6da82a: mov edx, dword ptr [ecx + 0xc]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da82d: mov dword ptr [esp + 0x14], edx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da831: fstp dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da835: fld dword ptr [esp + 0x54]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 + m   // 6da839: fadd dword ptr [eax + 4]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6da83c: fstp dword ptr [esp + 0x38]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x58)));   // 6da840: fld dword ptr [esp + 0x58]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 + m   // 6da844: fadd dword ptr [eax + 8]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6da847: fstp dword ptr [esp + 0x3c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da84b: fld dword ptr [esp + 0x268]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // ST0 = ST0 * m   // 6da852: fmul dword ptr [eax + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da855: fld dword ptr [esp + 0x268]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x14)));   // ST0 = ST0 * m   // 6da85c: fmul dword ptr [eax + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da85f: fld dword ptr [esp + 0x268]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // ST0 = ST0 * m   // 6da866: fmul dword ptr [eax + 0x18]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x64)));   // 6da869: fstp dword ptr [esp + 0x64]
    fp.Fxch(1);   // 6da86d: fxch st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6da86f: fmul dword ptr [esp + 0x14]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da873: fstp dword ptr [esp + 0x1c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6da877: fmul dword ptr [esp + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x64)));   // 6da87b: fld dword ptr [esp + 0x64]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6da87f: fmul dword ptr [esp + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // 6da883: fld dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da889: fld dword ptr [esp + 0x1c]
    fp.Arith('*', 0, 0, 1, false);   // 6da88d: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da88f: fstp dword ptr [esp + 0x40]
    fp.Fxch(2);   // 6da893: fxch st(2)
    fp.Arith('*', 0, 0, 2, false);   // 6da895: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da897: fstp dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6da89b: fmul st(1)
    fp.Fxch(1);   // 6da89d: fxch st(1)
    fp.FstpSt(0);   // 6da89f: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da8a1: fld dword ptr [esp + 0x40]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da8a5: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da8ab: fstp dword ptr [esp + 0x1c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da8af: fld dword ptr [esp + 0x44]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da8b3: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6da8b9: fstp dword ptr [esp + 0x20]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6da8bd: mov ecx, dword ptr [esp + 0x34]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da8c1: fmul dword ptr [0xb7cb5c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6da8c7: mov edx, dword ptr [esp + 0x38]
    esp -= 0xCu;   // 6da8cb: sub esp, 0xc
    eax = esp;   // 6da8ce: mov eax, esp
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6da8d0: mov dword ptr [eax], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6da8d2: mov ecx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6da8d6: mov dword ptr [eax + 4], edx
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6da8d9: mov dword ptr [eax + 8], ecx
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x28));   // 6da8dc: mov eax, dword ptr [esp + 0x28]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x2c));   // 6da8e0: mov ecx, dword ptr [esp + 0x2c]
    esp -= 0xCu;   // 6da8e4: sub esp, 0xc
    edx = esp;   // 6da8e7: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6da8e9: mov dword ptr [edx], eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6da8eb: fstp dword ptr [esp + 0x3c]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6da8ef: mov eax, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da8f3: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6da8f6: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da8f8: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da8fb: call 0x542a50
}
