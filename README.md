# SQL Project Documentation
Small relational database management system (work in progress).

`sql_main.cpp` is the entry point for the application, where an `SQL` object is instantiated and the `run` function is called.

# Classes

## SQL
### run
Runs the program in interactive mode where users can write queries. Currently capable of processing table creation, selecting data, and data insertion.
### run_batch (WIP)
Users must provide a batch file containing any queries they'd like to run.

## Parser
Queries are fed to a `Parser` object one at a time. Has a parse tree as a member of the class.
### Constructors
Can create a `Parser` and optionally initialize it with a query. When provided a query, it turns the query string into a queue of `Token` objects.
### get_parse_tree (WIP)
Given a query as a queue of `Token` objects, simulates parse tree creation, but does not mutate the actual `parse_tree` class member.
### get_ptree
Returns the Parser object's parse tree.
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
The `Parser` class also contains an array `command_machine` which is an adjacency matrix.
### get_column
### update_parse_tree

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
