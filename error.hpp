#ifndef ERROR_HPP
#define ERROR_HPP

#include <string>
#include <map>

enum ERR_CODE {
    ERR_OK = 0,             // エラーなし
    ERR_OPEN_FILE = 100,    // 'filename' がオープンできません
    ERR_WRITE_FILE,         // ファイルの書き込みで I/O エラーが発生しました
    ERR_READ_FILE,          // ファイルの読み出しで I/O エラーが発生しました
    ERR_OPTION,             // 無効なコマンドラインオプションです
    ERR_NO_END,             // END ディレクティブがファイルの終りに必要です
    ERR_SYNTAX = 200,       // 構文の誤りです
    ERR_UNDEFINED_SYMBOL,   // シンボル 'identifier' は定義されていません
    ERR_UNDEFINED_LABEL,    // ラベル 'identifier' は定義されていません
    ERR_DUP_SYMBOL,         // シンボル 'identifier' は既に定義されています 
    ERR_DUP_LABEL,          // ラベル 'identifier' は既に定義されています
    ERR_IMMEDIATE,          // イミディエイトオペランドは許されません
    ERR_TOO_FAR_JUMP,       // 指定されたジャンプするディスティネーションが遠すぎます
    ERR_EXPRESSION,         // 式に構文の誤りがあります
    ERR_RIGNT_PARENTHSIS,   // 右括弧が間違っています
    ERR_LEFT_PARENTHSIS,    // 式に左括弧がありません
    ERR_NO_EXPRESSION,      // 式がありません
    ERR_NO_OPERATOR,        // 演算子がありません
    ERR_NO_CONSTANT,        // 定数がありません
    ERR_NUMBER,             // 数値の表現が間違っています
    ERR_CONSTANT,            // 定数値が範囲外です

    ERR_WARNING = 300
};

class Error {

public:
    void filename(std::string file);        
    void info(int line_no, std::string source);  
    void message(ERR_CODE error, std::string identifer="");
    void end();
    
private:

    const std::map<ERR_CODE, std::string> error_msg_table = {
        {ERR_OK,                "no error"},                                // エラーなし
        {ERR_OPEN_FILE,         "cannot open file"},                        // 'filename' がオープンできません
        {ERR_WRITE_FILE,        "I/O error writing file"},                  // ファイルの書き込みで I/O エラーが発生しました
        {ERR_READ_FILE,         "I/O error reading file"},                  // ファイルの読み出しで I/O エラーが発生しました
        {ERR_OPTION,            "invalid command-line option"},             // 無効なコマンドラインオプションです
        {ERR_NO_END,            "END directive required at end of file"},   // END ディレクティブがファイルの終りに必要です
        {ERR_SYNTAX,            "syntax error"},                            // 構文の誤りです
        {ERR_UNDEFINED_SYMBOL,  "undefined symbol"},                        // シンボル 'identifier' は定義されていません
        {ERR_UNDEFINED_LABEL,   "undefined label"},                         // ラベル 'identifier' は定義されていません
        {ERR_DUP_SYMBOL,        "Duplicate definition of the symbol"},      // シンボル 'identifier' は既に定義されています 
        {ERR_DUP_LABEL,         "Duplicate definition of the label"},       // ラベル 'identifier' は既に定義されています
        {ERR_IMMEDIATE,         "immediate operand not allowed"},           // イミディエイトオペランドは許されません
        {ERR_TOO_FAR_JUMP,      "jump destination too far"},                // 指定されたジャンプ先が遠すぎます
        {ERR_EXPRESSION,        "syntax error in expression"},              // 式に構文の誤りがあります
        {ERR_RIGNT_PARENTHSIS,  "missing right parenthesis in expression"}, // 右括弧が間違っています
        {ERR_LEFT_PARENTHSIS,   "missing left parenthesis in expression"},  // 式に左括弧がありません
        {ERR_NO_EXPRESSION,     "expression expected"},                     // 式がありません
        {ERR_NO_OPERATOR,       "operator expected"},                       // 演算子がありません
        {ERR_NO_CONSTANT,       "constant expected"},                       // 定数がありません
        {ERR_NUMBER,            "nondigit in number"},                      // 数値の表現が間違っています
        {ERR_CONSTANT,          "constant value out of range"},             // 定数値が範囲外です

        {ERR_WARNING,           "warning"},                                 // 警告
    };

    std::string file;
    int line_no;
    std::string source;

    int error_count = 0;
    int warning_count = 0;
};

#endif