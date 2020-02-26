# ifndef TOKENIZER_H
# define TOKENIZER_H

class Tokenizer {
    
public:

    std::ifstream input_file;
    std::string cur_line;

    char * p_token; // point to token
    char c_str[256]; // to get token [for strtok()]
    char cur_offset[256]; // store cur line [for calculating line_offset_begin]
    char token[512]; // store token we get
        
    int line_num; // for error reporting
    int line_offset_begin; // for error reporting

    // constructor
    Tokenizer(std::string input_filename){
        input_file.open(input_filename.c_str());
        line_num = 0;
        line_offset_begin = 0;
        p_token = NULL;
    }
    // destructor
    ~Tokenizer(){ 
        input_file.close(); 
    }

    void getToken() {

        p_token = strtok(NULL, " ,\t\n");

        // the whole new line
        while(!input_file.eof() && p_token == NULL){
            
            int cur_line_offset = cur_line.length(); // for errors happen at eof
            getline(input_file, cur_line);
            
            if (input_file.eof()) {
                line_offset_begin = cur_line_offset;
                strcpy(token, "-1000");// use token == "-1000" to replace the token, or it still keeps last token, then passOne won't stop
                return;
            }

            strcpy(c_str, cur_line.c_str());
            p_token = strtok(c_str, " ,\t\n");

            if (p_token != NULL) {
                strcpy(cur_offset, cur_line.c_str()); // to count the line offset for error reporting
                line_offset_begin = 0;
            }

            line_num ++;
        }

        // the original line
        if (p_token != NULL) {
            strcpy(token, p_token);

            char taget_token;
            taget_token = token[0];

            char * p_cur_line = cur_offset; 
            char * p_cur_token;

            p_cur_token = strchr(p_cur_line, taget_token);
            line_offset_begin = p_cur_token - cur_offset; // distance
        }
    }

    int readInt(){
        
        getToken();

        try {
            int num = std::stoi(token);
            return num;
        } catch (std::exception &e) {
            parseError(line_num, line_offset_begin, 0); // all we expect is a num
            exit(1); // Parse errors should abort processing.
        }
    }

    std::string readSymbol() {

        getToken();
        std::string token_str = token;
        
        // Symbols always begin with alpha characters 
        if (!isalpha(token_str[0])) {
            parseError(line_num, line_offset_begin, 1);
            exit(1);
        }
        // followed by optional alphanumerical characters
        for (int i = 1; i < token_str.length(); i++) {
            if (!isalnum(token_str[i])) {
                parseError(line_num, line_offset_begin, 1);
                exit(1);
            }
        }
        // check symbol length
        if (!token_str.length()) {
            parseError(line_num, line_offset_begin, 1);
            exit(1);
        }
        return token_str;
    }

    std::string readIEAR(){

        getToken();
        std::string IEAR = token;

        std::set<std::string> set = {"I", "E", "A", "R"}; 

        try {
            if (IEAR.length() != 1) { // check length
                throw 2;
            }
            if (set.find(IEAR) == set.end()) { // check content
                throw 2;
            }
            return IEAR;
        } catch (int err_code) {
            parseError(line_num, line_offset_begin, err_code);
            exit(1);
        }
    }

private:

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
};

#endif