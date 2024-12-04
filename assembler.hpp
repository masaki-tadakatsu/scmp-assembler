#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <string>
#include <map>
#include "parser.hpp"
#include "memory.hpp"
#include "error.hpp"

const WORD OPE_ORG	= 0x100;
const WORD OPE_EQ	= 0x101;
const WORD OPE_END	= 0x102;
const WORD OPE_DB	= 0x103;
const WORD OPE_LDPI	= 0x200;
const WORD OPE_JS	= 0x201;

class Assembler {

public:
    Assembler(Memory &memory, Parser &parser, Error &error);
    bool pass1(std::string filename, bool opt_v);
    bool pass2(std::string filename, bool opt_v);

private:
    Error &error;
    Parser &parser;
    Memory &memory;

    void parse(std::string line, std::string &label,  std::string &mnemonic, std::string &operand);
    bool skip_sp(std::string str, unsigned int &pos);
    ERR_CODE eval_expression(std::string &expression, int &value);
    ERR_CODE get_factor(std::string &factor, int &value);
    ERR_CODE get_num(std::string &num, int &value);
    ERR_CODE get_symbol(std::string &symbol, int &value);
    ERR_CODE get_char(std::string &char_str, int &value);
    ERR_CODE eval_h_func(std::string &func, int &value);
    ERR_CODE eval_l_func(std::string &func, int &value);

    bool inst_type1(std::string operand);
    bool inst_type2(BYTE &opcode, std::string operand);
    bool inst_type3(BYTE &opcode, BYTE &disp, std::string operand);
    bool inst_type4(BYTE &opcode, BYTE &disp, std::string operand);
    bool inst_type5(WORD addr, BYTE &disp, std::string operand);
    bool inst_type6(BYTE &opcode, BYTE &disp, std::string operand);
    bool inst_type8(WORD addr, std::string operand);
    bool inst_type9(WORD addr, std::string operand);

    bool inst_code(std::string mnemonic, WORD &code);
    bool inst_bytes(std::string mnemonic, WORD &byte);
    bool inst_type(std::string mnemonic, int &type);
    bool assemble(WORD addr, std::string inst, std::string operand);


/*
type 1:オペランドなし
type 2:PRのみ
type 3:Immediate
type 4:PRとdisp
type 5:Jump Address
type 6:addressing
type 7:DB pseudo-instruction
type 8:LDPI macro
type 9:JS macro
*/
typedef struct {
    WORD opcode;
    int bytes;
    UINT16 type;
} ATTR;

const std::map<std::string, ATTR> inst_table = {
{"HALT",{0x00, 1, 1}},
{"XAE",	{0x01, 1, 1}},
{"CCL",	{0x02, 1, 1}},
{"SCL",	{0x03, 1, 1}},
{"DINT",{0x04, 1, 1}},
{"IEN",	{0x05, 1, 1}},
{"CSA",	{0x06, 1, 1}},
{"CAS",	{0x07, 1, 1}},
{"NOP",	{0x08, 1, 1}},
{"SIO",	{0x19, 1, 1}},
{"UND",	{0x1a, 1, 1}},
{"UND",	{0x1b, 1, 1}},
{"SR",	{0x1c, 1, 1}},
{"SRL",	{0x1d, 1, 1}},
{"RR",	{0x1e, 1, 1}},
{"RRL",	{0x1f, 1, 1}},
{"XPAL",{0x30, 1, 2}},
{"XPAH",{0x34, 1, 2}},
{"XPPC",{0x3c, 1, 2}},
{"LDE",	{0x40, 1, 1}},
{"ANE",	{0x50, 1, 1}},
{"ORE",	{0x58, 1, 1}},
{"XRE",	{0x60, 1, 1}},
{"DAE",	{0x68, 1, 1}},
{"ADE",	{0x70, 1, 1}},
{"CAE",	{0x78, 1, 1}},

{"PUTC",{0x20, 1, 1}},   /* for NIBL */
{"GETC",{0x21, 1, 1}},   /* for NIBL */

{"DLY",	{0x8f, 2, 3}},
{"JMP",	{0x90, 2, 5}},
{"JP",	{0x94, 2, 5}},
{"JZ",	{0x98, 2, 5}},
{"JNZ",	{0x9c, 2, 5}},
{"ILD",	{0xa8, 2, 4}},
{"DLD",	{0xb8, 2, 4}},
{"LD",	{0xc0, 2, 6}},
{"LDI",	{0xc4, 2, 3}},
{"ST",	{0xc8, 2, 6}},
{"AND",	{0xd0, 2, 6}},
{"ANI",	{0xd4, 2, 3}},
{"OR",	{0xd8, 2, 6}},
{"ORI",	{0xdc, 2, 3}},
{"XOR",	{0xe0, 2, 6}},
{"XRI",	{0xe4, 2, 3}},
{"DAD",	{0xe8, 2, 6}},
{"DAI",	{0xec, 2, 3}},
{"ADD",	{0xf0, 2, 6}},
{"ADI",	{0xf4, 2, 3}},
{"CAD",	{0xf8, 2, 6}},
{"CAI",	{0xfc, 2, 3}},

{"ORG",	{OPE_ORG, 0, 0}},
{"=",	{OPE_EQ, 0, 0}},
{"EQ",	{OPE_EQ, 0, 0}},
{"END", {OPE_END, 0, 0}},
{"DB",	{OPE_DB, 1, 7}},

{"LDPI",{OPE_LDPI, 6, 8}},  /* for NIBL */
{"JS",  {OPE_JS, 7, 9}}     /* for NIBL */
};

};

#endif