#include <cstring>
#include "parser.h"

Parser::Parser()
{
    // left blank
}

Parser::Parser(string command)
{
    const bool debug = false;

    build_keywords_map();
    build_command_machine();

    // process command to get tokens:
    STokenizer command_tokenizer;
    char command_string[256];
    strcpy(command_string, command.c_str());
    command_tokenizer.set_string(command_string);

    Token token;
    queue<Token> command_queue;
    while(command_tokenizer.more()){
        command_tokenizer >> token;
        if(token.token_str() != " ")    // ignore spaces for now (only testing "select * from student" 2/20/20)
            command_queue.push(token);
    }

    if(debug) cout << "Parser::ctor: got a queue of tokens" << endl
                   << "Parser::ctor: generating parse tree" << endl;

    get_parse_tree(command_queue);

    if(debug) cout << "Parser::ctor: parse tree generated successfully?" << endl;
}

bool Parser::get_parse_tree(queue<Token> cmd)
{

    /* Takes a command in the form of a vector of tokens
     * and runs it through a command state machine
     * in order to generate a parse tree.
     */
    const bool debug = true;

    Token token;

    int start_state = 0;
    int new_state = start_state;    // row
    int column = -1;

    // THE BELOW BLOCK OF CODE IS CAUSING THE PROGRAM TO CRASH 2/21/20
    // 2/22/20 Update: the queue was being popped even once it was empty,
    // which causes undefined behavior and causes the program to crash.
    // Changed while loop condition to also make sure queue not empty before popping.

    while(!(cmd.empty()) && new_state != -1){
        token = cmd.front();                            // grab a token
        cmd.pop();                                      // pop it from the queue so that the next item can be accessed

        column = get_column(token);                     // convert token's keyword into a column number

        /* 5/11/2020 UPDATE: get_column() doesn't work if token is a single character, like a comma.
         * Should I update get_column() so that it deals with this kind of situation, or
         * should I figure out some way to determine if token is a keyword, and call
         * get_column() accordingly?
         * Let's try using an enum COMMA.
         */

        if(debug) cout << "current state: " << new_state << endl
             << "col: " << column << endl;

        new_state = command_machine[new_state][column]; // go to the next state

        if(debug) cout << "new state: " << new_state << endl;

        update_parse_tree(new_state, token);
        if(debug) cout << parse_tree << endl;
    }

    if(debug) cout << "Parser::get_parse_tree: exited state machine" << endl;

    if(new_state == -1){
        if(debug) cout << "Parser::get_parse_tree: fail state reached... resetting parse tree" << endl;
        parse_tree.clear();
        return false;
    }

    return true;
}

void Parser::build_keywords_map(){
    keywords_map["make"] = MAKE;
    keywords_map["table"] = TABLE;
    keywords_map["fields"] = FIELDS;
    keywords_map["select"] = SELECT;
    keywords_map["from"] = FROM;
    keywords_map[","] = COMMA;
//    keywords_map["where"] = WHERE;
    keywords_map["insert"] = INSERT;
    keywords_map["into"] = INTO;
    keywords_map["values"] = VALUES;
//    keywords_map["and"] = AND;
//    keywords_map["or"] = OR;
    // anything not on keywords_map is SYMBOL
}

void Parser::build_command_machine()
{
    /* 2/20/20 Note: For now, this state machine
     * only works with commands in the form of
     * "select [symbol] from [symbol]".
     */

    const bool debug = false;

    // init everything to -1:
    init_table(command_machine);

    // ----------------------------------------------------- SELECT v
    for(int i=0; i<5; i++){
        mark_fail(command_machine, i);
    }
    mark_success(command_machine, 5);

    mark_cell(0, command_machine, SELECT, 1);
    mark_cell(1, command_machine, SYMBOL, 2);
    mark_cell(2, command_machine, COMMA, 3);
    mark_cell(2, command_machine, FROM, 4);
    mark_cell(3, command_machine, SYMBOL, 2);
    mark_cell(4, command_machine, SYMBOL, 5);

    // ----------------------------------------------------- MAKE TABLE v
    for(int i = 10; i < 16; i++)
        mark_fail(command_machine, i);
    mark_success(command_machine, 14);

    mark_cell(0, command_machine, MAKE, 10);
    mark_cell(10, command_machine, TABLE, 11);
    mark_cell(11, command_machine, SYMBOL, 12);
    mark_cell(12, command_machine, FIELDS, 13);
    mark_cell(13, command_machine, SYMBOL, 14);
    mark_cell(14, command_machine, COMMA, 15);
    mark_cell(15, command_machine, SYMBOL, 14);

    // ------------------------------------------------------ INSERT INTO v
    for(int i = 20; i < 26; i++)
        mark_fail(command_machine, i);
    mark_success(command_machine, 24);

    mark_cell(0, command_machine, INSERT, 20);
    mark_cell(20, command_machine, INTO, 21);
    mark_cell(21, command_machine, SYMBOL, 22);
    mark_cell(22, command_machine, VALUES, 23);
    mark_cell(23, command_machine, SYMBOL, 24);
    mark_cell(24, command_machine, COMMA, 25);
    mark_cell(25, command_machine, SYMBOL, 24);

    if(debug){
        for(int i = 0; i < 10; i++){
            for(int j = 0; j < 10; j++){
                cout << setw(4) << command_machine[i][j];
            }
            cout << endl;
        }
    }
}

int Parser::get_column(Token token)
{

    /* Given a token, get the corresponding
     * column number from the keywords_map.
     *
     * Otherwise, return the SYMBOL column number.
     */

    //const bool debug = true;

    string tstring = token.token_str();

    if(tstring == "make" ||
            tstring == "table" ||
            tstring == "fields" ||
            tstring == "select" ||
            tstring == "from" ||
            tstring == "where" ||
            tstring == "insert" ||
            tstring == "into" ||
            tstring == "values" ||
            tstring == "and" ||
            tstring == "or" ||
            tstring == ","){

        return keywords_map[tstring];
    }

    return SYMBOL;
}

void Parser::update_parse_tree(int state, Token token)
{
    /* 2/20/20 Note: this function only deals
     * with commands in the form of
     * "select [symbol] from [symbol]".
     *
     * Based on what the state is,
     * that'll tell us if we got a command,
     * table name, field name, etc. and we
     * can add to the parse tree accordingly.
     *
     * Recall that when looking at a state machine
     * on paper, for example, when we get to
     * state n, that means we must have gotten
     * xyz token.
     */
    switch(state){
    case 1:
        // we got a SELECT
        parse_tree["command"] += token.token_str();
        break;
    case 2:
    case 14:
        // field name
        parse_tree["fields"] += token.token_str();
        break;
    case 3:
        // COMMA
        break;
    case 12:
    case 22:
        // table name
        parse_tree["table"] += token.token_str();
        break;
    case 10:
        // MAKE
        parse_tree["command"] += token.token_str();
        break;
    case 20:
        // INSERT
        parse_tree["command"] += token.token_str();
        break;
    case 24:
        // new record to insert
        parse_tree["values"] += token.token_str();
    case 5:
        // table name
        // select * from [table name] [no conditions]
        //parse_tree["condiitons"] += "none";
        parse_tree["table"] += token.token_str();
    }
}

MMap<string, string> Parser::get_ptree()
{
    // Accessor function that returns parse_tree.
    return parse_tree;
}
