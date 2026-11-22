#include <cstdio>
#include <cstring>
#include <cstdint>
typedef uint8_t RwUInt8; typedef uint32_t RwUInt32; typedef int32_t RwInt32;
#include "core.inc"
static char buf[0x4000];
int main(){
    RwUInt32 table[64]={0}; table[1]=2;table[2]=2;table[6]=2;table[7]=2;table[9]=2;table[3]=4;table[10]=4;table[11]=6;
    unsigned b0,b1,b2,b3,ver;
    while(scanf("%x %x %x %x %u",&b0,&b1,&b2,&b3,&ver)==5){
        RwUInt8 d[4]={(RwUInt8)b0,(RwUInt8)b1,(RwUInt8)b2,(RwUInt8)b3}; RwUInt8 out[8];
        memset(buf,0,sizeof buf);
        int n=BuildVertexShaderSource(d,out,buf,ver,256,table,(VsSprintf)sprintf);
        printf("D %02x%02x%02x%02x v%u n=%d out=",b0,b1,b2,b3,ver,n);
        for(int i=0;i<8;i++)printf("%02x",out[i]);
        printf("\n");
        if(n>=0) fwrite(buf,1,n,stdout);
        printf("@@\n");
    }
}
