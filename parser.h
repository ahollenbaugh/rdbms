#ifndef PARSER_H
#define PARSER_H

#include <iostream>
#include <queue>
#include "stokenizer.h"
#include "vector.h"
#include "map.h"
#include "state_machine.h"
#include "mmap.h"

using namespace std;

class Parser
{
public:
    Parser();
    Parser(string command);
    bool get_parse_tree(queue<Token> cmd);
    void print_parse_tree();
    MMap<string, string> get_ptree();
private:
    Map<string, int> keywords_map;
    MMap<string, string> parse_tree;
    enum keywords {SUCCESS, SELECT, SYMBOL, FROM, MAKE,
                   TABLE, FIELDS, INSERT, INTO, VALUES, COMMA=','};
    int command_machine[MAX_ROWS][MAX_COLUMNS];

    void build_keywords_map();
    void build_command_machine();
    int get_column(Token token);
    void update_parse_tree(int state, Token token);
};

#endif // PARSER_H
