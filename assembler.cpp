#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cctype>
#include <sstream>
#include <vector>
#include "common.h"
#include "memory.hpp"
#include "error.hpp"
#include "parser.hpp"
#include "assembler.hpp"
#include "util.hpp"

Assembler::Assembler(Memory &memory, Parser &parser, Error &error):  error(error), parser(parser), memory(memory){
}

bool Assembler::pass1(std::string filename, bool opt_v)
{
    std::ifstream file;
    std::string line, symbol, inst, operand;
    WORD size, code;
    int addr;
    int value;
    int line_no;
    bool ret = true;
    ERR_CODE err;
    bool end = false;

    file.open(filename);
    if (file.fail()){
        error.message(ERR_OPEN_FILE, filename);
        return (false);
    }

    addr = 0;
    line_no = 0;
    while (getline(file, line)) {  // 1行ずつ読み込む
        parser.parse(line, symbol, inst, operand);
        line_no++;
        error.info(line_no, line);

        inst_code(inst, code);
        if (code == OPE_ORG){
            if (parser.eval_expression(operand, addr) != ERR_OK){
                error.message(ERR_SYNTAX);
                ret = false;
            }
        }
        else if (code == OPE_END){
            end = true;
            break;
        }

        if (!symbol.empty()){
            if (inst == "=" || inst == "EQ"){
                if (symbol.back() == ':'){
                    symbol.pop_back();
                }
                if ((err = parser.eval_expression(operand, value)) == ERR_OK){
                    if (parser.insert_symbol(symbol, value) == ERR_OK){
                        if (opt_v){
                            std::cout << "  " << symbol << "=" << value << std::endl;
                        }
                        if (!operand.empty()){
                            error.message(ERR_SYNTAX);
                            ret = false;
                        }            
                    }
                    else {
                        ret = false;
                    }
                }
                else {
                    error.message(err);
                    ret = false;
                }
            }
            else {
                if (symbol.back() == ':'){
                    symbol.pop_back();
                    if (parser.insert_symbol(symbol, addr) != ERR_OK) {
                        ret = false;
                    }
                }
                else {
                    error.message(ERR_SYNTAX);
                    ret = false;
                }
            }
        }

        if (inst_bytes(inst, size) == true){
            addr += size;
        }
        else {
            error.message(ERR_SYNTAX);
            ret = false;
        }
    }
    file.close();

    if (!end){
        error.message(ERR_NO_END);
        ret = false;
    }
    return (ret);
}

bool Assembler::pass2(std::string sourcename, bool opt_v)
{
    std::ifstream source;
    std::string listname;
    std::ofstream list;
    std::string line, label, inst, operand;
    WORD size, code;
    int addr;
    int line_no;
    bool ret = true;
    std::stringstream ss;

    source.open(sourcename);
    if (source.fail()){
        error.message(ERR_OPEN_FILE, sourcename);
        return (false);
    }

    listname = sourcename.substr(0, sourcename.find_last_of('.')) + ".list";
    list.open(listname);
    if (list.fail()){
        error.message(ERR_OPEN_FILE, listname);
        return (false);
    }

    addr = 0;
    line_no = 0;
    while (getline(source, line)) {  // 1行ずつ読み込む
        parser.parse(line, label, inst, operand);
        line_no++;
        error.info(line_no, line);

        inst_code(inst, code);
        if (code == OPE_ORG){
            parser.eval_expression(operand, addr);
        }

        if (assemble(addr, inst, operand) == false){
std::cout << "asm error" << addr << ":" << inst  << ":" << operand << std::endl;
            ret = false;
        }

        // output list-file
        ss.str("");             // clear stringstream
        ss.clear(std::stringstream::goodbit);
        ss << Util::dec2str(line_no) << "/ " << Util::hex2str((WORD)addr) << ": ";

        inst_bytes(inst, size);
        for (int offset = 0; offset < size; offset++){
            ss << Util::hex2str(memory.read((WORD)(addr + offset))) << " ";
        }
        ss << std::string((7 - size) * 3, ' ');
        ss << "  |"  << line << std::endl;

        if (opt_v){
            std::cout << ss.str();
        }
        list << ss.str();

        addr += size;

        inst_code(inst, code);
        if (code == OPE_END){
            break;
        }
    }
    source.close();

    // sort syambols
    std::vector<std::pair<std::string, int>> symbol;
    parser.symbol2vector(symbol);
    sort(symbol.begin(), symbol.end());

    // Symbol Table output
    ss.str("");             // clear stringstream
    ss.clear(std::stringstream::goodbit);

    ss << std::endl;
    ss << "Symbole Table:" << std::endl;

    int symbol_cnt = 0;
    for ( auto& itr: symbol){
        ss << std::setw(16) << std::left << itr.first;
        ss << ": ";
        ss << std::setw(5) << std::dec << (int)itr.second;
        ss << "(0x" << std::hex << itr.second << ")" << std::endl;

        symbol_cnt++;
    }
    ss << std::dec << symbol_cnt << " symbols" << std::endl;
    if (opt_v){
        std::cout << ss.str();
    }
    list << ss.str();

    list.close();

    if (!ret){
//        remove(listname.c_str());
        return (ret);
    }

    // create binary file
    std::string srecname = sourcename.substr(0, sourcename.find_last_of('.')) + ".srec";
    if ((ret = memory.save(srecname, 0, addr - 1)) == false){
        error.message(ERR_OPEN_FILE, srecname);
    }

    return (ret);
}


bool Assembler::inst_code(std::string inst, WORD &code)
{
    inst = Util::toupper_str(inst);

    code = -1;
    if (inst.empty()){
        return (true);
    }
    if (inst_table.count(inst) == 0){
        return (false);
    }
    code = inst_table.at(inst).opcode;

    return (true);
}

bool Assembler::inst_bytes(std::string inst, WORD &bytes)
{
    inst = Util::toupper_str(inst);
    bytes = 0;
    if (inst == ""){
        return (true);
    }
    if (inst_table.count(inst) == 0) {
        return (false);
    }
    bytes = inst_table.at(inst).bytes;

    return (true);
}

bool Assembler::inst_type(std::string Assembler, int &type)
{
    Assembler = Util::toupper_str(Assembler);
    if (Assembler == ""){
        return (true);
    }
    if (inst_table.count(Assembler) == 0) {
        return (false);
    }
    type = inst_table.at(Assembler).type;

    return (true);
}

bool Assembler::assemble(WORD addr, std::string inst, std::string operand)
{
    int type;
    WORD code;
    BYTE opcode;
    BYTE disp;
    bool ret = true;

    inst_code(inst, code);
    opcode = code & 0xff;
    inst_type(inst, type);
    switch (type){
    case 0:
        // nothing to do
        break;

    case 1:         // type 1:オペランドなし
        if (inst_type1(operand) == false){
            ret = false;
        }
        memory.write(addr, opcode);
        break;

    case 2:         // type 2:PRのみ
        if (inst_type2(opcode, operand) == false){
            ret = false;
        }
        memory.write(addr, opcode);
        break;

    case 3:         // type 3:Immediate
        if (inst_type3(opcode, disp, operand) == false){
            ret = false;
        }
        memory.write(addr, opcode, disp);
        break;

    case 4:         // type 4:PRとdisp
        if (inst_type4(opcode, disp, operand) == false){
            ret = false;
        }
        memory.write(addr, opcode, disp);
        break;

    case 5:         // type 5:Jump Address
        if (inst_type5(addr, disp, operand) == false){
            ret = false;
        }
        memory.write(addr, opcode, disp);
        break;

    case 6:         // type 6:addressing
        if (inst_type6(opcode, disp, operand) == false){
            ret = false;
        }
        memory.write(addr, opcode, disp);
        break;

    case 7:         // DB pseudo-instruction
        if (inst_type3(opcode, disp, operand) == false){
            ret = false;
        }
        memory.write(addr, disp);
        break;

    case 8:
        if (inst_type8(addr, operand) == false){
            ret = false;
        }
        break;

    case 9:
        if (inst_type9(addr, operand) == false){
            ret = false;
        }
        break;

    }

    return (ret);
}

bool Assembler::inst_type1(std::string operand)
{
    if (!operand.empty()){
        error.message(ERR_SYNTAX);
        return (false);
    }
    return (true);
}

bool Assembler::inst_type2(BYTE &opcode, std::string operand)
{
    bool ret = true;

    operand = Util::toupper_str(operand);
    if (operand == "1" || operand == "P1"){
        opcode |= 1;
    }
    else if (operand == "2" || operand == "P2"){
        opcode |= 2;
    }
    else if (operand == "3" || operand == "P3"){
        opcode |= 3;
    }
    else {
        error.message(ERR_SYNTAX);
        ret = false;
    }
    return (ret);
}

bool Assembler::inst_type3(BYTE &opcode, BYTE &disp, std::string operand)
{
    bool ret = false;

    if (parser.get_disp(operand, disp) == ERR_OK){
        if (operand.empty()){
            ret = true;
        }
        else {
            error.message(ERR_SYNTAX);            
        }
    }

    return (ret);
}

bool Assembler::inst_type4(BYTE &opcode, BYTE &disp, std::string operand)
{
    bool ret = false;
    int pr;

    if (operand.front() == '('){
        disp = 0;
        if (parser.get_pr(operand, pr) == ERR_OK){
            opcode |= pr;
            ret = true;
        }        
    }
    else if (parser.get_disp(operand, disp) == ERR_OK){
        if (parser.get_pr(operand, pr) == ERR_OK){
            opcode |= pr;
            ret = true;
        }
    }

    return (ret);
}

bool Assembler::inst_type5(WORD addr, BYTE &disp, std::string operand)
{
    bool ret = false;
    int jump;

    if (parser.eval_expression(operand, jump) == ERR_OK){;
        jump -= (addr + 2);
        if (-127 <= jump && jump <= 128){
            disp = jump & 0xff;
            ret = true;
        }
        else {
            error.message(ERR_TOO_FAR_JUMP);            
        }
    }

    return (ret);
}

bool Assembler::inst_type6(BYTE &opcode, BYTE &disp, std::string operand)
{
    bool ret;

    if (operand.empty()){
        error.message(ERR_SYNTAX);
        return (false);
    }
    if (operand.front() == '@'){
        opcode |= 4;
        operand.erase(0, 1);
    }

    ret = inst_type4(opcode, disp, operand);

    return (ret);
}

bool Assembler::inst_type8(WORD addr, std::string operand)
{
    bool ret = true;
    std::string operand1, operand2;

    if (operand.size() < 4){
        error.message(ERR_SYNTAX);
        return (false);
    }
    operand1 = operand.substr(0, 2);
    operand2 = operand.substr(3);

    if (assemble(addr,     "LDI", "H(" + operand2 + ")") == false){
        ret = false;
    }
    if (assemble(addr + 2, "XPAH", operand1) == false){
        ret = false;
    }
    if (assemble(addr + 3, "LDI", "L(" + operand2 + ")") == false){
        ret = false;
    }
    if (assemble(addr + 5, "XPAL", operand1) == false){
        ret = false;
    }

    return (ret);
}

bool Assembler::inst_type9(WORD addr, std::string operand)
{
    bool ret = true;
    std::string operand1, operand2;

    if (operand.size() < 4){
        error.message(ERR_SYNTAX);
        return (false);
    }
    operand1 = operand.substr(0, 2);
    operand2 = operand.substr(3);

    if (assemble(addr,     "LDI", "H(" + operand2 + "-1)") == false){
        ret = false;
    }
    if (assemble(addr + 2, "XPAH", operand1) == false){
        ret = false;
    }
    if (assemble(addr + 3, "LDI", "L(" + operand2 + "-1)") == false){
        ret = false;
    }
    if (assemble(addr + 5, "XPAL", operand1) == false){
        ret = false;
    }
    if (assemble(addr + 6, "XPPC", operand1) == false){
        ret = false;
    }

    return (ret);
}
