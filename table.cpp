#include "table.h"

Table::Table()
{
    const bool debug = false;
    if(debug) cout << "Table ctor fired (default)" << endl;
    // Left blank
}

Table::Table(const string& name, const Vector<string> fields)
{
    // CREATE NEW TABLE

    const bool debug = true;
    if(debug) cout << "Table ctor fired (new table)" << endl;

    table_name = name;
    _fields = fields;


    //  Build _indices map and _field_order map:
    if(debug) cout << "Building _indices map and _field_order map..." << endl;
    string field_name;
    MMap<string, int> field_table;
    for(int i = 0; i < fields.size(); i++)  // Create a table for each field
    {
        // Build _indices map:
        field_name = _fields[i];

        _indices.push_back(field_table);

        // Build _field_order map:
        _field_order[field_name] = i;
    }

    if(debug){
        cout << "--- _indices vector ---" << endl;
        for(int i=0; i<_indices.size(); i++){
            cout << _fields[i] << endl;
            cout << _indices[i] << endl;
        }
        cout << "-----------------------" << endl;
    }

    // Create empty binary file with same name as this table:
    if(debug) cout << "Creating new binary file..." << endl;
    fstream table_file;
    bin_fname = table_name + ".bin";
    open_fileW(table_file, bin_fname.c_str());

    // Write field names to a text file:
    if(debug) cout << "Writing field names to text file..." << endl;
    fstream field_names_file;
    fields_fname = table_name + "_field_names.txt";
    open_fileW(field_names_file, fields_fname.c_str());
    for(int i = 0; i < _fields.size(); i++){
        field_names_file.write(_fields[i].c_str(), _fields[i].size());
        // perhaps have a new line or comma
        field_names_file.write("\n", 1);
    }
}

Table::Table(const string& name){

    // OPEN EXISTING TABLE

    const bool debug = true;
    if(debug) cout << "Table ctor fired (open existing table)" << endl;

    table_name = name;

    // Get field names from text file that stores them
    // (we'll need these to reindex):
    // open the file:
    fstream field_names_file;
    fields_fname = table_name + "_field_names.txt";
    field_names_file.open(fields_fname);

    // read in the field names:
    string field;
    int i = 0;
    while(getline(field_names_file, field)){
        // rebuild _fields vector:
        _fields.push_back(field);
        // rebuild _field_order map:
        _field_order[field] = i;
        i++;
    }
    if(debug){
        cout << "_fields vector:" << endl;
        for(int i = 0; i < _fields.size(); i++)
            cout << _fields[i] << endl;
        cout << "File read successful." << endl;
    }

    // ----------------------------------------------

    // Read everything in from the binary file
    // to rebuild _indices:

    // _indices is a vector of multimaps.
    // We'll need a multimap that corresponds
    // to each field, so let's go ahead and
    // push back multimaps to _indices:
    // NOTE: shouldn't reindex() take care of this???
    MMap<string, int> m;
    for(int i=0; i<_fields.size(); i++){
        _indices.push_back(m);
    }

    // open the binary file:
    bin_fname = table_name + ".bin";
    fstream table;
    open_fileRW(table, bin_fname.c_str());

    // start reading in records and indexing them:
    // -> uh what happened to the reindex() function????
    Record r;
    int record_number = 0;
    int gcount = r.read(table, record_number);
    // two things just happened:
    // 1. we attempted to read in a record (it may or may not exist)
    // 2. we got the number of characters in the stream
    // (if 0, there's nothing to do)
    while(gcount != 0){
        cout << "recno: " << record_number << endl;
        cout << r << endl;
        for(int i=0; i<_fields.size(); i++){
            _indices[i][r.record[i]] += record_number;
            // example: lname -> Hollenbaugh | 24
        }
        record_number++;
        // keep reading each consecutive record
        gcount = r.read(table, record_number);
    }

    if(debug){
        cout << "--- _indices ---" << endl;
        for(int i=0; i < _indices.size(); i++){
            cout << _indices[i] << endl;
        }
        cout << "----------------" << endl;
    }

    //
    // 1. open/build index structures
    // 2. open filestream
    // 3. read_info: read filenames from a text file into a vector
    // 4. call set_fields using the vector we just built
    // 5. reindex: rebuild index structures
}

void Table::insert_into(Record entry){
    const bool debug = true;
    if(debug){
        cout << "Table::insert_into fired" << endl;
    }
    // 1. Write record to binary file:
    fstream table;
    open_fileRW(table, bin_fname.c_str());
    long recordNumber = entry.write(table);
    // 2. update _indices:
    reindex(entry, recordNumber);
    if(debug){
        cout << "--- _indices ---" << endl;
        for(int i=0; i < _indices.size(); i++){
            cout << _indices[i] << endl;
        }
        cout << "----------------" << endl;
    }
}

Table Table::select(vector<string> command){
    const bool debug = true;
    if(debug){
        cout<<"Table:: select() fired" << endl;
    }
}

Table Table::select_all(){
    print_table();
    return *this;
}

void Table::set_fields(Vector<string> field_names){
    _fields = field_names;
}

void Table::get_fields(){
    //
}

void Table::reindex(Record r, long recNo){
    // Update _indices after adding/deleting a record
    const bool debug = true;
    if(debug){
        cout << "Table::reindex fired" << endl;
    }
    for(int i = 0; i < _fields.size(); i++){
        _indices[i][r.record[i]] += recNo;
    }
    if(debug){
        cout << "reindex successful" << endl;
    }
}

void Table::print_table(){
    // print the field names (column names)
    cout << setw(20) << left << "#";
    for(int i=0; i<_fields.size(); i++){
        cout << setw(20) << left << _fields[i];
    }
    cout << endl;
    for(int i=0; i<((_fields.size()+1)*20); i++){
        cout << "-";
    }
//    cout << "---------------------------------"
//            "---------------------------------"
//         << endl;
    cout << endl;
    // print everything in order:
    fstream binfile;
    open_fileRW(binfile, bin_fname.c_str());
    Record r;
    int i = 0;
    int gcount = r.read(binfile, i);
    while(gcount != 0){
        cout << setw(20) << left << i;
        for(int j=0; j<_fields.size(); j++){
            cout << setw(20) << left << r.record[j];
        }
        cout << endl;
        i++;
        gcount = r.read(binfile, i);
    }
}

ostream & operator << (ostream &out, const Table &t){
    //
}
