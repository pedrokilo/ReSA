#pragma once
namespace RequestedPadDistanceTrainNopHex {
inline std::string HexBytes(const void* address,size_t size) {
    constexpr char digits[]="0123456789abcdef";
    const auto* bytes=static_cast<const uint8_t*>(address);std::string text(size*2,'0');
    for(size_t i=0;i<size;++i){text[i*2]=digits[bytes[i]>>4];text[i*2+1]=digits[bytes[i]&15];}
    return text;
}
inline std::vector<uint8_t> HexToBytes(const std::string& text) {
    if(text.size()%2)std::abort();std::vector<uint8_t> bytes(text.size()/2);
    auto nibble=[](char digit)->uint8_t{if(digit>='0'&&digit<='9')return uint8_t(digit-'0');if(digit>='a'&&digit<='f')return uint8_t(digit-'a'+10);std::abort();};
    for(size_t i=0;i<bytes.size();++i)bytes[i]=uint8_t((nibble(text[i*2])<<4)|nibble(text[i*2+1]));return bytes;
}
}
