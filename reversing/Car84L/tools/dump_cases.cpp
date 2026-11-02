#include <cstdio>
#include "../Fixtures.hpp"
int main(int argc,char** argv){
    const unsigned from=argc>1?unsigned(atoi(argv[1])):0,to=argc>2?unsigned(atoi(argv[2])):C84LFix::kCases;
    for(unsigned n=from;n<to;++n){auto c=C84LFix::MakeC(n);std::printf("%s\n",C84LFix::JsonC(c).c_str());}
    return 0;
}
