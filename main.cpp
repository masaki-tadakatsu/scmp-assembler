#include <iostream>     // for std::cout
#include <iomanip>      // for std::setw, std::setfill>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>


bool pass1(std::string filename);
void parse(std::string line, std::string &label,  std::string &mnemonic, std::string &operand);
bool skip_sp(std::string str, unsigned int &pos);

bool inst_code(std::string mnemonic, int &opcode);
bool inst_bytes(std::string mnemonic, int &bytes);


int main()
{
    std::string filename;

    std::cout << "Filename:";
    std::cin >> filename;

    pass1(filename);

    return (0);
}


void parse(std::string line, std::string &label,  std::string &mnemonic, std::string &operand)
{  
    unsigned int pos;

    label = "";
    mnemonic = "";
    operand = "";

    pos = 0;
    if (pos == line.length()){
        return;
    }
    if (line[pos] != ' ' && line[pos] != '\t'){
        while (line[pos] != ' '  && line[pos] != '\t'){
            if (line[pos] == ';'){
                return;
            }
            label += line[pos++];
            if (pos == line.length()){
                return;
            }
        }
    }
    if (skip_sp(line, pos) == false){
        return;
    }
    while (line[pos] != ' '  && line[pos] != '\t'){
        if (line[pos] == ';'){
            return;
        }
        mnemonic += line[pos++];
        if (pos == line.length()){
            return;
        }
    }
    if (skip_sp(line, pos) == false){
        return;
    }
    while (pos < line.length()){
        if (line[pos] == ';'){
            break;
        }
        if (line[pos] != ' '  && line[pos] != '\t'){
            operand += line[pos];
        }
        pos++;
        if (pos == line.length()){
            return;
        }
    }
}

bool skip_sp(std::string str, unsigned int &pos)
{
    while (str[pos] == ' ' || str[pos] == '\t'){
        pos++;
        if (pos >= str.length() - 1){
            return (false);
        }
    }
    return (true);
}

/* 

1. ERROR MISSING ARG.
This error indicates more arguments are required.

2. ERROR VALUE
This error indicates value out of range or exceeds
field size.

3. ERROR ADDRESS
This error indicates address out of range.

4. ERROR USAGE
This error indicates a rumber of possibilities including:
a. A .IF nesting error
b. Symbol not previously defined which would affect
location coonter
c. illegal expression, for example, two operators in
sequence

5. ERROR SYNTAX
Indicates an illegal character or improper statement
construction.

6. ERROR EXCESS ARG.
Indicates an existence of unprocessed arguments.

7. ERROR TBL OVERFLOW
Indicates the following:
a. If nesting level exceeds 10
b. Number of local regions exceeds 64
c. Symbol table overflow

8. ERROR UNDEFINED
Used to indicate either an undefined symbol or undefined
instruction/directive.

9. ERROR DUP. DEF.
Duplicate definition of the symbol.

10. ERROR POINTER
Indicates the following:
a. Pointer Register should have been specified but
was not
b. Pointer Register 0 (PC) not allowed

*/