#include <iostream>
#include <iomanip>
#include <cassert>
#include "state_machine.h"
#include <string.h>

using namespace std;

//Fill all cells of the array with -1
void init_table(int _table[][MAX_COLUMNS]){
    for(int i = 0; i < MAX_ROWS; i++){
        for(int j = 0; j < MAX_COLUMNS; j++){
            _table[i][j] = -1;
        }
    }
}

//Mark this state (row) with a 1 (success)
void mark_success(int _table[][MAX_COLUMNS], int state){
    // Each row represents a state.
    // The first column contains info as to
    // whether it's a fail or success state.
    // success = 1, fail = 0.
    _table[state][0] = 1;
}

//Mark this state (row) with a 0 (fail)
void mark_fail(int _table[][MAX_COLUMNS], int state){
    _table[state][0] = 0;
}

//true if state is a success state
bool is_success(int _table[][MAX_COLUMNS], int state){
    if(_table[state][0] == 0)
        return false;
    else if(_table[state][0] == 1)
        return true;
}

//Mark a range of cells in the array.
void mark_cells(int row, int _table[][MAX_COLUMNS], int from, int to, int state){
    // maybe use error enums for these functions?
    assert(row >= 0); // row cannot be negative.
    assert(row < MAX_ROWS); // row cannot exceed the last row.
    assert(from >= 0); // from cannot be negative.
    assert(to > 0); // to cannot be negative and must be greater than from.
    assert(from < MAX_COLUMNS - 1); // The last column is MAX_COLUMNS - 1.
                                    // from cannot be equal to the last column
                                    // because to would end up being equal
                                    // to a column number that's not in the array.
    assert(to < MAX_COLUMNS);       // The greatest value to can take on
                                    // is MAX_COLUMNS - 1.
    assert(from < to);              // Must go left to right.
    for(int col = from; col <= to; col++){
        _table[row][col] = state;
    }
}

//Mark columns represented by the string columns[] for this row
void mark_cells(int row, int _table[][MAX_COLUMNS], const char columns[], int state){
    // columns is an array of charaters.
    // Suppose we have const char DIGITS[] = "0123456789".
    // DIGITS[2] = '2' corresponds to ASCII value 50,
    // which is the column number we'll use to represent
    // the number 2.
    int i = 0;
    while(columns[i] != '\0'){
        mark_cell(row, _table, columns[i], state);
        i++;
    }
}

//Mark this row and column
void mark_cell(int row, int table[][MAX_COLUMNS], int column, int state){
    table[row][column] = state;
}

//This can realistically be used on a small table
void print_table(int _table[][MAX_COLUMNS]){
    for(int i = 0; i < MAX_ROWS; i++){
        for(int j = 0; j < MAX_COLUMNS; j++){
            cout << setw(4) << _table[i][j];
        }
        cout << endl;
    }
    cout << endl;
}

//show string s and mark this position on the string:
//hello world   pos: 7
//       ^
void show_string(char s[], int _pos){
    const bool debug = false;
    assert(_pos >= 0); // _pos cannot be negative
    if(debug){
        cout <<"state_machine::show_string: strlen(s): "
             << strlen(s) << endl;
    }
    cout << "|" << s << "|" << "    pos: " << _pos << endl;
    int i = 0;
    while(i <= strlen(s)){
        if(i == _pos)
            cout << "^";
        else
            cout << " ";
        i++;
    }
    cout << endl;
}
