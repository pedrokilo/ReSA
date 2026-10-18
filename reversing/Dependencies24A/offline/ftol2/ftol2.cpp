#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <vector>
#include <random>
#include "ftol_bytes.h"
#include "X87Math.h"
using u32=uint32_t; using u8=uint8_t;
alignas(16) static u8 gPrime[108];
static float k1000=1000.0f; static int gN;
static void MakePrime(u32 fsw){std::memset(gPrime,0,108);gPrime[0]=0x7F;uint16_t s=uint16_t(fsw);std::memcpy(gPrime+4,&s,2);
 for(int i=0;i<8;++i){u8* r=gPrime+28+10*i;for(int k=0;k<8;++k)r[k]=u8(0x11*(i+1)+k);r[7]|=0x80;r[8]=u8(i);r[9]=0x40;}}
struct Out{u32 eax;alignas(16) u8 img[108];};
static void* gFtol;
static void RunExe(int n,Out& o){
  u8* img=o.img;const u8* pr=gPrime;u32 e=0;void* f=gFtol;gN=n;
  __asm{
    mov eax,pr
    frstor [eax]
    fstp st(0)
    fstp st(0)
    fstp st(0)
    fstp st(0)
    fstp st(0)
    fstp st(0)
    fstp st(0)
    fstp st(0)
    fild dword ptr [gN]
    fdivr dword ptr [k1000]
    mov eax,f
    call eax
    mov e,eax
    mov eax,img
    fnsave [eax]
    frstor [eax]
  }
  o.eax=e;}
static void RunDll(int n,Out& o){
  u8* img=o.img;const u8* pr=gPrime;u32 e=0;
  __asm{
    mov eax,pr
    frstor [eax]
    fstp st(0)
    fstp st(0)
    fstp st(0)
    fstp st(0)
    fstp st(0)
    fstp st(0)
    fstp st(0)
    fstp st(0)
  }
  e=WndProcTimerPeriodCpp(n);
  __asm{
    mov eax,img
    fnsave [eax]
    frstor [eax]
  }
  o.eax=e;}
int main(int argc,char**argv){
  void* p=VirtualAlloc(nullptr,0x1000,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);std::memcpy(p,kFtol2,sizeof(kFtol2));gFtol=p;
  std::vector<int> ns{0,1,-1,2,-2,3,5,7,10,15,16,24,25,30,31,32,33,40,50,60,64,75,100,125,128,200,250,500,999,1000,1001,1024,2000,100000,16777215,16777216,16777217,33554433,0x7FFFFFFF,int(0x80000000),int(0x80000001),-1000,-1001,-16,-3};
  std::mt19937 rng(12345);for(int i=0;i<30000;++i){int v=int(rng());if(i%3==0)v=int(rng()%2000)-100;if(i%7==0)v>>=int(rng()%31);ns.push_back(v);}
  const u32 fsws[]{0x0000,0x0020,0x003F,0x0001,0x0004,0x0010,0x0002,0x0008,0x4100,0x0100,0x0400,0x4000,0x0200};
  long diffs=0,total=0;
  for(int n:ns)for(u32 fsw:fsws){MakePrime(fsw);Out a{},b{};RunExe(n,a);MakePrime(fsw);RunDll(n,b);
    std::memset(a.img+12,0,4);std::memset(b.img+12,0,4);++total;
    if(a.eax!=b.eax||std::memcmp(a.img,b.img,108)){++diffs;if(diffs<=12){std::printf("DIFF n=%d fsw=%04x eax %08x %08x\n",n,fsw,a.eax,b.eax);for(int i=0;i<108;++i)if(a.img[i]!=b.img[i])std::printf("  byte %d: %02x %02x\n",i,a.img[i],b.img[i]);}}}
  std::printf("total %ld diffs %ld\n",total,diffs);return diffs!=0;}
