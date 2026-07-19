#include "sql.h"

SQL::SQL()
{
    // left blank 
}

void SQL::run(){
    const bool debug = false;

    string command;
    string c0 = "make table employee fields lname, fname, department, salary";
    string c1 = "select * from employee";
    string c2 = "select lname, fname, department, salary from employee";
    string c3 = "insert into employee values Hollenbaugh, Allison, CS, 100000";
    string c4 = "insert into employee values McGee, Peeps, Journalism, 50000";
    // Don't use c5 for now. Spaces aren't supported yet.
    // string c5 = "insert into employee values Van Gogh, Jim Bob, Art, 20000";
    string c6 = "select * from employee";

    Parser parser(c6);
    MMap<string, string> ptree = parser.get_ptree();

    // Create table
    if(ptree["command"][0]=="make"){
        if(debug){
            cout<<"SQL::run: command == make table"<<endl;
        }
        string table_name = ptree["table"][0];
        Table t(table_name, ptree["fields"]);
    }

    // Select
    if(ptree["command"][0]=="select"){
        string table_name = ptree["table"][0];
        if(debug) cout << "table name: " << table_name << endl;
        Table t(table_name);
        // if no conditions:
        if(t._where == false){
            ptree["conditions"] += "none";
        }
        if(ptree["conditions"][0]=="none"){
            // select all
            if(debug){
                cout<<"SQL::run: command == select all"<<endl;
            }
            t.select_all();
        }
        if(debug){
            cout<<"SQL::run: command == select with conditions"<<endl;
        }

        //Table t(table_name);
        /* 5/14/2020 UPDATE: If fields == * then we need a vector
         * with all fields.
         */
    }

    // Insert data
    if(ptree["command"][0]=="insert"){
        if(debug){
            cout<<"SQL::run: command == insert"<<endl;
        }
        string table_name = ptree["table"][0];
        // use "open table" ctor
        Table existingTable(table_name);
        Record newRecord(ptree["values"]);
        existingTable.insert_into(newRecord);
    }

//    cout << "Enter Q to quit." << endl;
//    string command;
//    while(true){
//        STokenizer command_tokenizer;           // Do NOT make this a member of SQL! Tokenizing won't work properly.
//        // 1. Prompt user for a command:
//        cout << "> ";
//        getline(cin, command);
//        if(command == "Q" || command == "q")
//            break;
//        Parser parser(command);

        // 2. Use string tokenizer to turn
        // string into a queue of tokens:
                                                // (Annoyingly, I have to copy command.c_str() to command_string
//        char command_string[256];               // because set_string won't accept a const char array.)
//        strcpy(command_string, command.c_str());

//        command_tokenizer.set_string(command_string);

//        Token token;
//        Vector<Token> command_vector;
//        while(command_tokenizer.more()){
//            command_tokenizer >> token;
//            command_vector += token;
//        }
//        if(debug){
//            cout << "Here's a queue of tokens: " << endl;
//            for(int i=0; i<command_vector.size(); i++){
//                cout << setw(10) << left << command_vector[i];
//            }
//        }
        // 3. Parse command to make sure it's valid:
        // 4. Generate parse tree
        // 5. Either have a SQL function that determines
        // which table function to call, or send parse
        // tree to table class so that it can decide:
//    }
}

void SQL::run_batch(string filename){
    //
}
