# SQL Project Documentation
This is a simple relational database management system that accepts SQL-like querys. It features several data structures built from scratch: B+ trees, maps, multimaps, records, adjacency matrices, and vectors. Working on this project deepened my understanding of how said data structures work, and it gave me some of the foundational database knowledge I needed for my web development career.



# Classes

## SQL
### run
`sql_main.cpp` is the entry point for the application, where an `SQL` object is instantiated and the `run` function is called. It runs the program in interactive mode where users can write queries.

Supported operations:
- create table
- insert data
- select
  - all fields (*) or particular fields
  - with conditions (WIP)
### run_batch (WIP)
Users must provide a batch file containing any queries they'd like to run.

## Parser
Queries are fed to a `Parser` object one at a time. Has a parse tree as a member of the class.
### Constructors
Can create a `Parser` and optionally initialize it with a query. When provided a query, it turns the query string into a queue of `Token` objects.
### get_parse_tree (WIP)
Given a query as a queue of `Token` objects, updates the `Parser` object's `parse_tree` and returns `true`, or clears the parse tree and returns `false` if a fail state is reached.
### get_ptree
Returns the `Parser` object's `parse_tree` itself.
### build_keywords_map
The `Parser` class contains a `Map` object called `keywords_map`, along with a collection of `enum` constants called `keywords`. The `keywords_map` looks like this:

| string   | enum   |
|----------|--------|
| "make"   | MAKE   |
| "table"  | TABLE  |
| "fields" | FIELDS |
| "select" | SELECT |
| "from"   | FROM   |
| ","      | COMMA  |
| "insert" | INSERT |
| "into"   | INTO   |
| "values" | VALUES |

Anything that isn't part of the `keywords_map` is mapped to the enum `SYMBOL`.

### build_command_machine
The `Parser` class also contains an array `command_machine` which is an adjacency matrix. The array represents graphs for each database operation -- select, create table, and insert.
The diagram below is a simplified version of the graph for the select operation -- that is, with no conditions.

<img width="650" height="330" alt="SQL State Machine drawio" src="https://github.com/user-attachments/assets/ab1fff1c-81b8-4df7-9765-e4a490707ab6" />


Here's how it would look represented in an adjacency matrix or 2D array:

|   | SUCCESS | SELECT | SYMBOL | FROM | MAKE | TABLE | FIELDS | INSERT | INTO | VALUES |
|---|---------|--------|--------|------|------|-------|--------|--------|------|--------|
| 0 | 0       | 1      | -1     | -1   | 10   | -1    | -1     | 20     | -1   | -1     |
| 1 | 0       | -1     | 2      | -1   | -1   | -1    | -1     | -1     | -1   | -1     |
| 2 | 0       | -1     | -1     | 4    | -1   | -1    | -1     | -1     | -1   | -1     |
| 3 | 0       | -1     | 2      | -1   | -1   | -1    | -1     | -1     | -1   | -1     |
| 4 | 0       | -1     | 5      | -1   | -1   | -1    | -1     | -1     | -1   | -1     |
| 5 | 1       | -1     | -1     | -1   | -1   | -1    | -1     | -1     | -1   | -1     |


Each column is referred to by an enum constant. The `SUCCESS` column indicates whether a row represents a success state (1) or a fail state (0).

### get_column
Consults `keywords_map`, given a `Token` object containing a string, and returns the corresponding enum. For example, if a `Token` contains the string `"insert"`, it will return the enum `INSERT`.
### update_parse_tree
After a query string is converted to a queue of `Token`s, this function processes one `Token` at a time and (if applicable) adds it to the parse tree accordingly.

## Multimap
Used for parse trees, which break down queries into a format that's easier for the application to process. Suppose someone enters this query:

`select lname, fname from authors where lname = "Garcia Marquez" and country = "Colombia"` 

The parse tree would look like this:

| key        | value(s)                                          |
|------------|---------------------------------------------------|
| command    | select                                            |
| table      | authors                                           |
| fields     | lname, fname                                      |
| where      | yes                                               |
| conditions | lname = "Garcia Marquez" and country = "Colombia" |

The underlying data structure is a B+ tree. The B+ tree's nodes are Multipairs (key/value pairs where the values are lists, specifically vectors). Note that BPlusTree<MPair<K, V> > is referred to simply as `map_base`. The `MMap` class contains an `Iterator` subclass.

## Table
### Constructors
Can either create a new table (given a name and a list of fields), or open an existing one.
### insert_into
For adding data to a table.
### select (WIP)
Useful if you only want to view data for certain fields. Does not support conditions (yet).
### select_all (WIP)
View all fields. Does not support conditions (yet).
### set_fields
Sets field names upon table creation.
### get_fields
Retrieve a list of a table's fields.
### reindex
Updates the `_indices` class member when records are added or deleted. `_indices` is a vector of multimaps. Each index of the vector corresponds to a field name of a particular table. Each multimap consists of multipairs. Each multipair contains a string (field value) mapped to a list of integers (record numbers). For example:

| _indices for employees table | MMap #1                | MMap #2                    | ... |
|------------------------------|------------------------|----------------------------|-----|
| first                        | Bill -> 11, 37, 40     | Richard -> [73], 59, 63    | ... |
| last                         | Harris -> [73], 87, 92 | Johnson -> 18, 38          | ... |
| department                   | CS -> 22, 38, 54       | Humanities -> 51, [73], 80 | ... |
| ...                          | ...                    | ...                        | ... |
