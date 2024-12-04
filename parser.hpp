#ifndef PARSER_HPP
#define PARSER_HPP

#include <string>
#include <map>
#include <vector>
#include "common.h"
#include "error.hpp"

class Parser {

public:
    Parser(Error &error);
    void parse(std::string line, std::string &label,  std::string &mnemonic, std::string &operand);
    ERR_CODE insert_symbol(std::string symbol, int value);
    void symbol2vector(std::vector<std::pair<std::string, int>> &symbol);
    ERR_CODE eval_expression(std::string &expression, int &value);
    ERR_CODE get_disp(std::string &disp, BYTE &value);
    ERR_CODE get_pr(std::string &operand, int &pr);

    std::map<std::string, int> symbol_table;

private:
    Error &error;

    bool skip_sp(std::string str, unsigned int &pos);
    ERR_CODE get_factor(std::string &factor, int &value);
    ERR_CODE get_number(std::string &number, int &value);
    ERR_CODE get_num(std::string &num, int &value);
    ERR_CODE get_symbol(std::string &symbol, int &value);
    ERR_CODE get_char(std::string &char_str, int &value);
    ERR_CODE eval_h_func(std::string &func, int &value);
    ERR_CODE eval_l_func(std::string &func, int &value);
};

#endif