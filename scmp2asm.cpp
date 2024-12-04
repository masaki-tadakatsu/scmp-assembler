#include <iostream>
#include <string>
#include "assembler.hpp"
#include "parser.hpp"
#include "error.hpp"
#include "util.hpp"

int main(int argc, char* argv[])
{
    Memory memory;
    Error error;
    Parser parser(error);
    Assembler assembler(memory, parser, error);
    bool err = false;
    bool opt_v = false;
    std::string filename;

    if (argc <= 3){
        for (int i = 1; i < argc; i++){
            if (argv[i][0] == '-'){
                if (argv[i][1] == 'v'){
                    opt_v = true;
                }
                else {
                    err = true;
                }
            }
            else {
                filename = argv[i];
            }
        }
    }
    else {
        err = true;
    }

    if (err){
        std::cout <<"Usage: scmp2asm [option] [source file name]" << std::endl;
        std::cout <<"   option : -v  virbose" << std::endl;
        return (0);
    }

    if (filename.empty()){
        std::cout << "Filename:";
        std::cin >> filename;
    }

    error.filename(filename);

    std::cout << "pass1" << std::endl;
    if (!assembler.pass1(filename, opt_v)){
        goto end;
    }

    std::cout << "pass2" << std::endl;
    if (!assembler.pass2(filename, opt_v)){
        goto end;
    }

end:
    error.end();

    return (0);
}

