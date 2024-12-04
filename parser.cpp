#include <iostream>     // for std::cout
#include <string>
#include <cctype>
#include <algorithm>
#include <vector>
#include "common.h"
#include "error.hpp"
#include "util.hpp"
#include "parser.hpp"

Parser::Parser(Error &error): error(error)
{
}

void Parser::parse(std::string line, std::string &label,  std::string &Assembler, std::string &operand)
{  
    unsigned int pos;

    label = "";
    Assembler = "";
    operand = "";

    pos = 0;
    if (pos == line.length()){
        return;
    }
    // label or symbol
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
    // mnemonic
    while (line[pos] != ' '  && line[pos] != '\t'){
        if (line[pos] == ';'){
            return;
        }
        Assembler += line[pos++];
        if (pos == line.length()){
            return;
        }
    }
    if (skip_sp(line, pos) == false){
        return;
    }
    // operand
    bool flg_instr = false;
    while (pos < line.length()){
        if (line[pos] == '\''){
            flg_instr = !flg_instr;
        }
        if (flg_instr){
            operand += line[pos];
        }
        else {
            if (line[pos] == ';'){
                break;
            }
            if (line[pos] != ' '  && line[pos] != '\t'){
                operand += line[pos];
            }
        }
        pos++;
        if (pos == line.length()){
            return;
        }
    }
}

bool Parser::skip_sp(std::string str, unsigned int &pos)
{
    while (str[pos] == ' ' || str[pos] == '\t'){
        pos++;
        if (pos == str.length()){
            return (false);
        }
    }
    return (true);
}

ERR_CODE Parser::insert_symbol(std::string symbol, int value)
{
    symbol = Util::toupper_str(symbol);
    if (symbol_table.count(symbol) == 0) {
        symbol_table[symbol] = value;
        return (ERR_OK);
    }
    error.message(ERR_DUP_SYMBOL, symbol);
    return (ERR_DUP_SYMBOL);
}

void Parser::symbol2vector(std::vector<std::pair<std::string, int>> &symbol)
{
    for (auto &itr: symbol_table){
        symbol.push_back(std::make_pair(itr.first, itr.second));
    }
}

ERR_CODE Parser::eval_expression(std::string &expression, int &value)
{
    ERR_CODE ret;
    int r_value, l_value;

    if (expression.empty()){
        error.message(ERR_NO_EXPRESSION);
        ret = ERR_NO_EXPRESSION;
    }
    if ((ret = get_factor(expression, l_value)) == ERR_OK){
        while (1){
            if (expression.empty()){
                break;
            }
            if (expression.front() == '+'){
                expression.erase(0, 1);
                if ((ret = get_factor(expression, r_value)) == ERR_OK){
                    l_value += r_value;
                }
                else {
                    break;
                }
            }
            else if (expression.front() == '-'){
                expression.erase(0, 1);
                if ((ret = get_factor(expression, r_value)) == ERR_OK){
                    l_value -= r_value;
                }
                else {
                    break;
                }
            }
            else if (expression.front() == '|'){
                expression.erase(0, 1);
                if ((ret = get_factor(expression, r_value)) == ERR_OK){
                    l_value |= r_value;
                }
                else {
                    break;
                }
            }
            else {
                break;
            }
        }
    }

    value = l_value;

    return (ret);
}

ERR_CODE Parser::get_factor(std::string &expression, int &value)
{
    ERR_CODE ret;
    int r_value, l_value;

    if (expression.empty()){
        error.message(ERR_NO_EXPRESSION);
        ret = ERR_NO_EXPRESSION;
    }
    if ((ret = get_number(expression, l_value)) == ERR_OK){
        while (1){
            if (expression.empty()){
                break;
            }
            if (expression.front() == '*'){
                expression.erase(0, 1);
                if ((ret = get_number(expression, r_value)) == ERR_OK){
                    l_value *= r_value;
                }
                else {
                    break;
                }
            }
            else if (expression.front() == '/'){
                expression.erase(0, 1);
                if ((ret = get_number(expression, r_value)) == ERR_OK){
                    l_value /= r_value;
                }
                else {
                    break;
                }
            }
            else {
                break;
            }
        }
    }

    value = l_value;

    return (ret);
}

ERR_CODE Parser::get_number(std::string &number, int &value)
{
    ERR_CODE ret;

    if (number.empty()){
        error.message(ERR_SYNTAX);
        ret = ERR_SYNTAX;
    }

    if (number.front() == '('){
        number.erase(0, 1);
        if ((ret = eval_expression(number, value)) == ERR_OK){
            if (number.front() == ')'){
                number.erase(0, 1);
                ret = ERR_OK;
            }
            else {
                error.message(ERR_LEFT_PARENTHSIS);
                ret = ERR_LEFT_PARENTHSIS;
            }
        }
    }
    else if (number.size() >= 4 && std::toupper(number.front()) == 'H' && number[1] == '('){
        ret = eval_h_func(number, value);
        return (ret);
    }
    else if (number.size() >= 4 && std::toupper(number.front()) == 'L' && number[1] == '('){
        ret = eval_l_func(number, value);
        return (ret);
    }
    else if (number.front() == '\''){
        ret = get_char(number, value);
    }
    else if (isdigit(number.front()) || number.front() == '+' || number.front() == '-'){
        ret = get_num(number, value);
    }
    else {
        ret = get_symbol(number, value);
    }
    return (ret);
}

ERR_CODE Parser::eval_h_func(std::string &func, int &value)
{
    ERR_CODE ret;

    func.erase(0, 2);

    if ((ret = eval_expression(func, value)) == ERR_OK){
        if (func.front() == ')'){
            value = (value & 0xffff) >> 8;

            func.erase(0, 1);
            ret = ERR_OK;
        }
        else {
            error.message(ERR_LEFT_PARENTHSIS);
            ret = ERR_LEFT_PARENTHSIS;
        }
    }

    return (ret);
}

ERR_CODE Parser::eval_l_func(std::string &func, int &value)
{
    ERR_CODE ret;

    func.erase(0, 2);       // remove "L("

    if ((ret = eval_expression(func, value)) == ERR_OK){
        if (func.front() == ')'){
            value = value & 0xff;

            func.erase(0, 1);  // remove ")"
            ret = ERR_OK;
        }
        else {
            error.message(ERR_LEFT_PARENTHSIS);
            ret = ERR_LEFT_PARENTHSIS;
        }
    }
    return (ret);
}

ERR_CODE Parser::get_num(std::string &num, int &value)
{
    int digit;
    char c;
    bool minus = false;

    if (num.empty()){
        error.message(ERR_SYNTAX);
        return (ERR_SYNTAX);
    }
    if (num.front() == '-'){
        num.erase(0, 1);
        minus = true;
    }
    if (num.front() == '+'){
        num.erase(0, 1);
    }

    if (num.empty()){
        error.message(ERR_SYNTAX);
        return (ERR_SYNTAX);
    }
    if (num.size() >= 2 && num.compare(0, 2, "0x") == 0){ // hexa-decimal
        value = 0;
        digit = 0;
        for (unsigned int i = 2; i < num.size(); i++){
            c = std::toupper(num[i]);
            if ('0' <= c && c <= '9'){
                digit++;
                value = value * 16 + (c - '0');
            }
            else if ('A' <= c && c <= 'F'){
                digit++;
                value = value * 16 + (c - 'A' + 10);
            }
            else {
                break;
            }
        }
        if (digit == 0){
            error.message(ERR_SYNTAX);
            return (ERR_SYNTAX);
        }
        num.erase(0, digit + 2);
    }
    else {      // decimal
        value = 0;
        digit = 0;
        for (unsigned int i = 0; i < num.size(); i++){
            if ('0' <= num[i] && num[i] <= '9'){
                digit++;
                value = value * 10 + (num[i] - '0');
            }
            else {
                break;
            }
        }
        if (digit == 0){
            error.message(ERR_SYNTAX);
            return (ERR_SYNTAX);
        }
        num.erase(0, digit);
    }
    if (minus){
        value *= -1;        
    }
    return (ERR_OK);
}

ERR_CODE Parser::get_symbol(std::string &symbol, int &value)
{
    std::string symbol_str;
    
    if (symbol.empty()){
        error.message(ERR_SYNTAX);
        return (ERR_SYNTAX);
    }
    for (unsigned int i = 0; i < symbol.size(); i++){
        if (std::isalnum(symbol[i]) || symbol[i] == '_'){
            symbol_str += symbol[i];
        }
        else {
            break;
        }
    }
    symbol_str = Util::toupper_str(symbol_str);
    if (symbol_str.size() == 0){
        error.message(ERR_SYNTAX);
        return (ERR_SYNTAX);
    }  
    if (symbol_table.count(symbol_str) != 0){
        value = symbol_table[symbol_str];
    }
    else {
        error.message(ERR_UNDEFINED_SYMBOL);
        return (ERR_UNDEFINED_SYMBOL);
    }

    symbol.erase(0, symbol_str.size());

    return (ERR_OK);
}

ERR_CODE Parser::get_char(std::string &char_str, int &value)
{
    std::string symbol_str;
    
    if (char_str.size() < 3){
        error.message(ERR_SYNTAX);
        return (ERR_SYNTAX);
    }
    if (char_str[2] != '\''){
        error.message(ERR_SYNTAX);
        return (ERR_SYNTAX);
    }
    value = char_str[1];

    char_str.erase(0, 3);

    return (ERR_OK);
}

ERR_CODE Parser::get_disp(std::string &operand, BYTE &disp)
{
    int value;
    ERR_CODE ret;

    if ((ret = eval_expression(operand, value)) == ERR_OK){
        if (-128 <= value && value <= 255){
            disp = value & 0xff;
        }
        else {
            ret = ERR_CONSTANT;
            error.message(ERR_CONSTANT);            
        }
    }

    return (ret);
}

ERR_CODE Parser::get_pr(std::string &operand, int &pr)
{
    std::string pr_str;
    ERR_CODE ret = ERR_OK;

    if (operand.empty()){
        pr = 0;
        return (ERR_OK);
    }

    if (operand.size() < 4){
        ret = ERR_SYNTAX;
        error.message(ERR_SYNTAX);            
    }

    operand = Util::toupper_str(operand);

    if (operand == "(P0)" || operand == "(PC)"){
        pr = 0;
    }
    else if (operand == "(P1)"){
        pr = 1;
    }
    else if (operand == "(P2)"){
        pr = 2;
    }
    else if (operand == "(P3)"){
        pr = 3;
    }
    else {
        ret = ERR_SYNTAX;
        error.message(ERR_SYNTAX);            
    }

    return (ret);
}
