#include <iomanip>      // for std::setw, std::setfill>
#include <sstream>
#include <string>
#include <algorithm>
#include "common.h"
#include "util.hpp"

std::string Util::dec2str(int n)
{ 
    std::stringstream out; 

    out << std::setw(5) << std::right << n;

    return (out.str());
}

std::string Util::hex2str(BYTE n)
{ 
    std::stringstream out; 

    out << std::setfill('0') << std::setw(2) << std::right << std::hex << (UINT32)n;

    return (out.str());
}

std::string Util::hex2str(WORD n)
{ 
    std::stringstream out; 

    out << std::setfill('0') << std::setw(4) << std::right << std::hex << (UINT32)n;

    return (out.str());
}

std::string Util::hex2str_upper(BYTE n)
{ 
    std::stringstream out; 

    out << std::uppercase << std::setfill('0') << std::setw(2) << std::right << std::hex << (UINT32)n;

    return (out.str());
}

std::string Util::hex2str_upper(WORD n)
{ 
    std::stringstream out; 

    out << std::uppercase << std::setfill('0') << std::setw(4) << std::right << std::hex << (UINT32)n;

    return (out.str());
}

std::string Util::toupper_str(std::string str)
{
    transform(str.begin(), str.end(), str.begin(), ::toupper);   // 大文字に変換

    return (str);
}
