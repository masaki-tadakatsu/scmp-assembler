#include <iostream>
#include <string>
#include "error.hpp"

void Error::filename(std::string fn)        
{
    file = fn;
}

void Error::info(int line_no, std::string source)        
{
    this->line_no = line_no;
    this->source = source;
}

void Error::message(ERR_CODE error, std::string identifer)
{
    std::cout << file << ":" << line_no << ": ";
    
    if (error < ERR_WARNING){
        std::cout << "\033[31merror:\033[m ";
        error_count++;
    }
    else {
        std::cout << "\033[33mwarning:\033[m ";
        warning_count++;
    }    

    std::cout << error_msg_table.at(error);

    if (!identifer.empty()){
        std::cout << ": '" << identifer << "'";
    }
    if (error >= ERR_SYNTAX){
        std::cout << " :" << source;
    }
    std::cout << std::endl;
}

void Error::end()
{
    std::cout << warning_count << " warning(s)  " << error_count << " error(s)";
}
