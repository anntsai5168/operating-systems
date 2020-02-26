# include <iostream>
# include <fstream>
# include <cstring>
# include <string>
# include <map>
# include <set>
# include <iterator>
# include <array>
# include <iomanip>
# include "tokenizer.h"


void parseError(int &line_num, int &line_offset_begin ,int err_code) {
            const char* err_str[] = {
                    "NUM_EXPECTED",             // Number expect
                    "SYM_EXPECTED",             // Symbol Expected
                    "ADDR_EXPECTED",            // Addressing Expected which is A/E/I/R
                    "SYM_TOO_LONG",             // Symbol Name is too long
                    "TOO_MANY_DEF_IN_MODULE",   // >16
                    "TOO_MANY_USE_IN_MODULE",   // >16
                    "TOO_MANY_INSTR"            // total num_instr exceeds memory size (512)
            };
            std::cout << "Parse Error line " << line_num 
                      << " offset "<< line_offset_begin + 1  // offsets in the line start with 1
                      << ": " << err_str[err_code] << std::endl;
        }

void errorMessage(std::string &symbol_name, int err_code) {

    switch(err_code){
        case 0: {
            std::cout<<std::endl; // for new line
            break; 
        }
        case 1: {
            std::cout << " Error: Absolute address exceeds machine size; zero used" << std::endl;
            break; 
        }
        case 2: {
            std::cout << " Error: Relative address exceeds module size; zero used" << std::endl;
            break;
        }
        case 3: {
            std::cout << " Error: External address exceeds length of uselist; treated as immediate" << std::endl;
            break;
        }
        case 4: {
            std::cout << " Error: " << symbol_name << " is not defined; zero used" << std::endl;
            break;
        }
        case 5: {
            std::cout << " Error: This variable is multiple times defined; first value used" << std::endl;
            break;
        }
        case 6: {
            std::cout << " Error: Illegal immediate value; treated as 9999" << std::endl;
            break;
        }
        case 7: {
            std::cout << " Error: Illegal opcode; treated as 9999" << std::endl;
            break;
        }
        default : {
            std::cout << " Error: Others" << std::endl;
        }
    }
}

////////////////////////////////////////////////////////////////////
//////////////////////////// PASS ONE //////////////////////////////
////////////////////////////////////////////////////////////////////

// syntax and early error checking
// create symbol table


void passOne(Tokenizer &TK, std::map<std::string, int> &symbol_table){
    int module_addr = 0; // 0-based [recording base addr]
    int module_cnt = 1;  // 1-based
    std::map<std::string, int> symbol_error_map;

    if (TK.input_file){

        try{
            while (!TK.input_file.eof()){

                std::map<std::string, int> symbol_table_temp;
                
                // def lst
                int def_cnt = TK.readInt();

                if (def_cnt == -1000){  // when reach eof, token == "-1000"
                    continue;
                }
                else if (def_cnt > 16){
                    throw 4; // TOO_MANY_DEF_IN_MODULE
                }

                for (int i = 0; i < def_cnt; i++){
                    std::string symbol = TK.readSymbol();

                    if (symbol.length() > 16) {
                        throw 3; //SYM_TOO_LONG
                    }

                    int r_addr = TK.readInt();

                    if (symbol_table_temp.count(symbol) == 0 && symbol_table.count(symbol) == 0){
                        symbol_table_temp.insert({symbol, r_addr});
                        symbol_error_map.insert({symbol, 0});
                    }
                    else {
                        symbol_error_map.at(symbol) = 5; // Error: This variable is multiple times defined 
                    }
                }

                // use lst
                int use_cnt = TK.readInt();

                if (use_cnt > 16){
                    throw 5; // TOO_MANY_USE_IN_MODULE
                }
                for (int i = 0; i < use_cnt; i++){
                    std::string symbol = TK.readSymbol();
                    if (symbol.length() > 16) {
                        throw 3; // SYM_TOO_LONG
                    }
                }

                // program text
                int code_cnt = TK.readInt();

                if (code_cnt < 0){
                    continue;
                }
                if (module_addr + code_cnt - 512 > 0) {
                    throw 6; // TOO_MANY_INSTR
                }
                for (int i = 0; i < code_cnt; i++) {
                    TK.readIEAR();
                    TK.readInt();
                }

                // symbol table temp -> symbol table
                std::map<std::string, int>::iterator it;
                for (it = symbol_table_temp.begin(); it != symbol_table_temp.end(); it++){
                    // rule 5 : r addr > module size, and treat r addr as 0
                    if((*it).second > code_cnt){
                        printf("Warning: Module %d: %s too big %d (max=%d) assume zero relative\n", module_cnt, (*it).first.c_str(), (*it).second, code_cnt - 1);
                        (*it).second = 0;
                    }
                    else {
                        (*it).second += module_addr; // absolute addr
                    }
                    symbol_table.insert(*it);
                }

                module_cnt ++;
                module_addr += code_cnt;
            }
        }catch(int e){
            parseError(TK.line_num, TK.line_offset_begin, e);
            exit(1);
        }
    }
    else {
        std::cout << "input failed" << std::endl;
        exit(1);
    }

    // cout symbol table
    std::cout << "Symbol Table" << std::endl;

    std::map<std::string, int>::iterator it;
    for (it = symbol_table.begin(); it != symbol_table.end(); it++){
        std::cout << (*it).first << "=" << (*it).second;

        // cout error message
        int err_code = symbol_error_map.at((*it).first);
        std::string *p_err_symbol; // no need to point to anything, cuz here no rule 3 error
        errorMessage(*p_err_symbol, err_code); 
    }
    std::cout << std::endl;

    TK.input_file.close(); // close and reopen the file in pass two
}

////////////////////////////////////////////////////////////////////
//////////////////////////// PASS TWO //////////////////////////////
////////////////////////////////////////////////////////////////////

// additional error checking
// instruction transformation
// create memory map

void passTwo(Tokenizer &TK, std::map<std::string, int> &symbol_table){
    int module_addr = 0;   // 0-based [recording base addr]
    int module_cnt = 1;    // 1-based
    int instr_cnt = 0;     // for memory map 000: 001: 002: ......
    std::map<std::string, int> IEAR_to_int = {{"I", 0}, {"E", 1}, {"A", 2}, {"R", 3}};
    std::map<std::string, int> symbol_defined_module;   // record symbol defined in which module 
    std::map<std::string, int> def_lst_used_record;     // define lst symbol used or not

    // initializing symbol_defined_module
    std::map<std::string, int>::iterator it;
    for (it = symbol_table.begin(); it != symbol_table.end(); it++){
        symbol_defined_module.insert({(*it).first, 0});
        def_lst_used_record.insert({(*it).first, 0});
    }

    // parsing & print out Meomory Map
    if(TK.input_file){

        std::cout << "Memory Map\n";

        while (!TK.input_file.eof()){
            // def lst
            int def_cnt = TK.readInt();

            for (int i = 0; i < def_cnt; i++){
                std::string symbol = TK.readSymbol();
                TK.readInt();
                symbol_defined_module.at(symbol) = module_cnt;
            }

            // use lst
            std::array<std::string, 16> use_lst_match; // index and symbol match
            std::map<std::string, int> use_lst_used_record; // use lst symbol use or not
            
            int use_cnt = TK.readInt();

            for (int i = 0; i < use_cnt; i++){
                std::string symbol = TK.readSymbol();
                use_lst_match[i] = symbol;
                use_lst_used_record.insert({symbol, 0});
            }

            // program text & print out Meomory Map details
            int code_cnt = TK.readInt();

            for (int i = 0; i < code_cnt; i++){
                
                std::string IEAR = TK.readIEAR();
                int IEAR_int = IEAR_to_int.at(IEAR);

                int instr = TK.readInt();
                int opcode = instr / 1000;
                int operand = instr % 1000;
                int err_code = 0;    // clean last round value

                std::string *p_symbol; // point to error message symbol name
                
                switch (IEAR_int){
                    case 0:{ // I
                        if(instr >= 10000){
                            instr = 9999;
                            err_code = 6;
                        }
                        break;
                    }  
                    case 1:{ // E 
                        if (operand >= use_cnt){
                            err_code = 3; // treat as I
                        } 
                        else {
                            std::string use_symbol = use_lst_match[operand];
                            use_lst_used_record.at(use_symbol) = 1;

                            if (symbol_table.count(use_symbol) == 0){ //not defined but use
                                err_code = 4;
                                p_symbol = &use_symbol;
                                instr = instr - operand; // use absolute val = 0
                            }
                            else {
                                def_lst_used_record.at(use_symbol) = 1; 
                                instr = instr - operand + symbol_table.at(use_symbol); // absolute address
                            }
                        }
                        break;
                    }   
                    case 2:{ // A
                        if (operand >= 512){
                            err_code = 1;
                            instr -= operand;
                        }
                        break;
                    }
                    case 3:{ // R
                        if (operand > code_cnt){
                            err_code = 2;
                            instr = instr - operand + module_addr;
                        }
                        else {
                            instr += module_addr;
                        }
                        break;
                    }   
                }

                // illegal opcode : priority -> last [conflict with I]
                if (opcode >= 10 && err_code == 0){  // or we will overwrite rule 10 error
                    instr = 9999;
                    err_code = 7;   
                }

                // print memory map
                std::cout << std::setfill('0') << std::setw(3) << instr_cnt ++ << ": "
                          << std::setfill('0') << std::setw(4) << instr; // no need new line, cuz we had error code 0 -> print \n
                // error message print
                errorMessage(*p_symbol, err_code);  // no err -> err_code = 0
            }
            
            // Rule7 warning
            // no need a new line
            for (it = use_lst_used_record.begin(); it != use_lst_used_record.end(); it++){
                if ((*it).second == 0){
                    std::string use_symbol = (*it).first;
                    std::cout << "Warning: Module " << module_cnt << ": " << use_symbol << " appeared in the uselist but was not actually used\n";
                }
            }

            module_cnt ++;
            module_addr += code_cnt;
        }

        // Rule 4 warning
        // need a new line
        if (def_lst_used_record.size() > 0) {
            std::cout << std::endl;
        }
        for (it = def_lst_used_record.begin(); it != def_lst_used_record.end(); it++){
            if ((*it).second == false){
                    std::cout << "Warning: Module " << symbol_defined_module.at((*it).first) << ": " << (*it).first << " was defined but never used\n";
                }      
        }
    }
    else {
        std::cout << "input failed.\n";
        exit(1);
    }

}

////////////////////////////////////////////////////////////////////
//////////////////////////// MAIN //////////////////////////////////
////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {

    std::string input_filename;
    std::map<std::string, int> symbol_table;

    // take command line input as file name
    if (argc == 2){
        input_filename = argv[1];
    }
    else {
        std::cout << "Too many input files.";
        exit(1);
    }
    
    // pass one
    Tokenizer pass_one_source(input_filename);
    passOne(pass_one_source, symbol_table);
    // pass two
    Tokenizer pass_two_source(input_filename);
    passTwo(pass_two_source, symbol_table);

    return 0;
}

