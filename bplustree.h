#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <iostream>
#include <iomanip> // setw
#include <stack>
#include "btree_array_functions.h"

using namespace std;

// PROTOTYPES -------------------------------------------------------------------------------------------------------

template <class T>
class BPlusTree
{
public:
    class Iterator{
    public:
        friend class BPlusTree;
        Iterator(BPlusTree<T>* _it=NULL, int _key_ptr = 0):it(_it), key_ptr(_key_ptr){
            // left blank
        }

        T operator *(){
            assert(key_ptr<it->data_count);
            return it->data[key_ptr]; // returns T at index of the node being pointed to by pointer
        }

        Iterator operator++(int un_used){
            // un_used is just a junk variable
            // used for overloading purposes.
            // it++
            // returns a copy of the current value
            // of it, then changes the actual value.
        }

        Iterator operator++(){
            // ++it
            // easier
            // changes it and returns reference.

            // if you've reached the end of the array,
            // point _it to the next node and
            // reset key_ptr to 0 since we're starting
            // at a new array.
            key_ptr++;
            if(key_ptr == it->data_count){
                it = it->next;
                key_ptr = 0;
            }
            return *this;
        }
        friend bool operator ==(const Iterator& lhs, const Iterator& rhs){
            return (lhs.it == rhs.it);
        }

        friend bool operator !=(const Iterator& lhs, const Iterator& rhs){
            return (lhs.it != rhs.it);
        }
        void print_Iterator(){
            cout << it->data[key_ptr] << endl;
        }
        bool is_null(){return !it;}
    private:
        BPlusTree<T>* it;
        int key_ptr;
    };

    BPlusTree(bool dups = false);
    //big three:
    BPlusTree(const BPlusTree<T>& other);
    ~BPlusTree();
    BPlusTree<T>& operator =(const BPlusTree<T>& RHS);

    void insert(const T& entry);                //insert entry into the tree
    void remove(const T& entry);                //remove entry from the tree

    void clear_tree();                          //clear this object (delete all nodes etc.)
    void copy_tree(const BPlusTree<T>& other, BPlusTree<T>* &last_leaf);  //copy other into this object
    void copy_tree(const BPlusTree<T>& other, stack<BPlusTree<T>*>& s);
    bool contains(const T& entry) const;        //true if entry can be found in the array

    T& get(const T& entry);                     //return a reference to entry in the tree
    const T& get(const T& entry)const;          //return a reference to entry in the tree
    T& get_existing(const T& entry);            //return a reference to entry in the tree

    Iterator find(const T& entry);              //return an iterator to this key. NULL if not there.
    Iterator lower_bound(const T& entry);       // return first that goes NOT BEFORE
                                                // key entry or next if does not
                                                // exist >= entry
    Iterator upper_bound(const T& entry);       // return first that goes AFTER key
                                                // exist or not, the next entry > entry

    int size() const;                           //count the number of elements in the tree
    bool empty() const;                         //true if the tree is empty

    void print_tree(int level = 0, ostream &outs=cout) const; //print a readable version of the tree
    friend ostream& operator<<(ostream& outs, const BPlusTree<T>& print_me){
        print_me.print_tree(0, outs);
        return outs;
    }
    bool is_valid();

    Iterator begin();
    Iterator end();


//private:
    static const int MINIMUM = 1;
    static const int MAXIMUM = 2 * MINIMUM;

    bool dups_ok;                       //true if duplicate keys may be inserted
    int data_count;                     //number of data elements
    T data[MAXIMUM + 1];                //holds the keys
    int child_count;                    //number of children
    BPlusTree* subset[MAXIMUM + 2];     //subtrees
    BPlusTree* next;
    bool is_leaf() const
        {return child_count==0;}        //true if this is a leaf node

    T* find_ptr(const T& entry);        //return a pointer to this key. NULL if not there.

    //insert element functions
    void loose_insert(const T& entry);  //allows MAXIMUM+1 data elements in the root
    void fix_excess(int i);             //fix excess of data elements in child i

    //remove element functions:
    void loose_remove(const T& entry);  //allows MINIMUM-1 data elements in the root

    BPlusTree<T>* fix_shortage(int i);  //fix shortage of data elements in child i
                                        // and return the smallest key in this subtree
    BPlusTree<T>* get_smallest_node();
    void get_smallest(T& entry);        //entry := leftmost leaf
    void get_biggest(T& entry);         //entry := rightmost leaf
    void remove_biggest(T& entry);      //remove the biggest child of this tree->entry
    void transfer_left(int i);          //transfer one element LEFT from child i
    void transfer_right(int i);         //transfer one element RIGHT from child i
    BPlusTree<T> *merge_with_next_subset(int i);    //merge subset i with subset i+1

};

// DEFINITIONS -------------------------------------------------------------------------------------------------------

template <class T>
BPlusTree<T>::BPlusTree(bool dups){
    data_count = 0;
    child_count = 0;
    next = NULL;
}

template <class T>
BPlusTree<T>::BPlusTree(const BPlusTree<T>& other){
    BPlusTree<T> *last = NULL;
    copy_tree(other, last);
}

template <class T>
BPlusTree<T>::~BPlusTree(){
    clear_tree();
}

template <class T>
BPlusTree<T>& BPlusTree<T>::operator =(const BPlusTree<T>& RHS){
    if(this == &RHS)
        return *this;
    BPlusTree<T> *last = NULL;
    copy_tree(RHS, last);
    return *this;
}

template <class T>
void BPlusTree<T>::insert(const T& entry){

    /*

    Note: In order for this class to be able to keep track of the number of the keys,
    this function (and the functions it calls ) must return a success code.
    If we are to keep track of the number the keys (as opposed to key/values)
    then the success code must distinguish between inserting a new key, or
    adding a new key to the existing key. (for "dupes_ok")

    How this function works:
    loose_insert this entry into this root.
    loose_insert(entry) will insert entry into this tree. Once it returns,
    all the subtrees are valid btree subtrees EXCEPT this root may have
    one extra data item; in this case (we have excess in the root),
    create a new node, copy all the contents of this root into it,
    clear this root node, make the new node this root's only child
    (subset[0]). Then, call fix_excess on this only subset (subset[0]).

    */

    const bool debug = false;

    if(debug) cout << "insert fired" << endl;

    loose_insert(entry);

    if(data_count > MAXIMUM)                        // Fix root.
    {
        BPlusTree<T> *new_root = new BPlusTree<T>;
        BPlusTree<T> *last = NULL;

        new_root->copy_tree(*this, last);
        clear_tree();
        attach_item(subset, child_count, new_root);
        fix_excess(0);
    }

}

template <class T>
void BPlusTree<T>::remove(const T& entry){
    // To be written...
}

template <class T>
void BPlusTree<T>::clear_tree()
{

    /*

    Clear this object (delete all nodes etc.).

    1. If this node is not a leaf, then call this
    function recursively on the leftmost node.
    (if it IS a leaf, do nothing)

    2. After coming back up from the recursive call,
    in a for loop: increment i and recursively
    call this function on the next subtree.
    Do this while i < data_count.

    3. THEN, set your data_count and child_count to 0.

    */

    const bool debug = false;
    if(debug){
        cout << "clear_tree fired" << endl;
        print_tree(0);
    }


    int i = 0;
    if(!is_leaf())
    {
        if(debug)cout<<"deleting child "<<i<<endl;
        subset[i]->clear_tree();

        while(i < child_count)
        {
            if(debug)cout<<"deleting child "<<i<<endl;
            subset[i]->clear_tree();
            delete subset[i];
            i++;
        }

        child_count = 0;
        data_count = 0;
    }
    else
        data_count = 0;
}

template <class T>
void BPlusTree<T>::copy_tree(const BPlusTree<T>& other, BPlusTree<T>* &last_leaf)
{

    /*

    1. Copy other's data into your data.
    2. In a loop, create a new node
    and recursively call this function on it
    with the argument being the corresponding
    tree in other. Do this child_count times.
    Else, point last_leaf's next to this node
    if last_leaf is not null.
    Set last_leaf to this node.

    */

    const bool debug = false;
    assert(empty());
    if(debug) cout << "copy_tree fired" << endl;

    copy_array(data, other.data, data_count, other.data_count);


    if(!other.is_leaf()){
        for(int i = 0; i < other.child_count; i++){
            BPlusTree<T> *tree = new BPlusTree<T>;
            attach_item(subset, child_count, tree);
            subset[i]->copy_tree(*other.subset[i], last_leaf);
        }
    }
    else{

        if(last_leaf)
            last_leaf->next = this;
        last_leaf = this;
    }
}

template <class T>
bool BPlusTree<T>::contains(const T& entry) const{
    int i = first_ge(data, data_count, entry);
    bool found = (i<data_count && data[i]==entry);
    if(is_leaf()){
        if(found)
            return true;
        return false;
    }
    else{
        if(found)
            return subset[i+1]->contains(entry);
        return subset[i]->contains(entry);
    }
}

template <class T>
T& BPlusTree<T>::get(const T& entry){
    //If entry is not in the tree, add it to the tree

    const bool debug = false;
    if (!contains(entry))
        insert(entry);

    return get_existing(entry);
}

template <class T>
const T& BPlusTree<T>::get(const T& entry)const{
    assert(contains(entry));
    const bool debug = false;
//    if (!contains(entry))
//        insert(entry);

    return get_existing(entry);
}

template <class T>
T& BPlusTree<T>::get_existing(const T& entry){
    /*
    * ---------------------------------------------------------------
    * This routing explains plainly how the BPlusTree is organized.
    * i = first_ge
    *       |      found         |    !found         |
    * ------|--------------------|-------------------|
    * leaf  |  you're done       | it's not there    |
    *       |                    |                   |
    * ------|--------------------|-------------------|
    * !leaf | subset[i+1]->get() |  subset[i]->get() |
    *       | (inner node)       |                   |
    * ------|--------------------|-------------------|
    *       |                    |                   |
    *
    * ---------------------------------------------------------------
    *
    */
    //assert that entry is not in the tree.
    //assert(contains(entry));

    const bool debug = false;
    int i = first_ge(data, data_count, entry);
    bool found = (i<data_count && data[i]==entry);
    if (is_leaf())
        if (found){
            return data[i];
        }
        else{
            if (debug) cout<<"get_existing was called with nonexistent entry"<<endl;
            assert(found);
        }
    if (found) //inner node
        return subset[i+1]->get(entry);
    //or just return true?
    else //not found yet...
        return subset[i]->get(entry);
}

template <class T>
typename BPlusTree<T>::Iterator BPlusTree<T>::find(const T& entry){
    //
}

template <class T>
typename BPlusTree<T>::Iterator BPlusTree<T>::lower_bound(const T& entry){
    //
}

template <class T>
typename BPlusTree<T>::Iterator BPlusTree<T>::upper_bound(const T& entry){
    //
}

template <class T>
int BPlusTree<T>::size() const{
    //
}

template <class T>
bool BPlusTree<T>::empty() const{
    return (data_count == 0);
}

template <class T>
void BPlusTree<T>::print_tree(int level, ostream &outs) const{
    // 1. print the last child (if any)
    // 2. print all the rest of the data and children

    // DETAILS
    // 1. Go all the way to the rightmost node
    // and print the data there.
    // 2. Coming back up from the recursive call,
    // print data[element] (element = data_count-1).
    // 3. Call this function on subset[element].
    // (Steps 2 and 3 are in a loop)
    const bool debug = false;
    if(next)
        outs << setw(level) << "^" << endl;
    if(!is_leaf()){
        subset[child_count-1]->print_tree(level+10, outs);
        for(int i = data_count-1; i >= 0; i--){
            outs << setw(level) << data[i] << endl;
            subset[i]->print_tree(level+10, outs);
        }
    }
    else{
        for(int i = data_count-1; i >= 0; i--){
            outs << setw(level) << data[i] << endl;
        }
    }
}

template <class T>
bool BPlusTree<T>::is_valid()
{
    const bool debug = false;

    if(debug){
        cout << "is_valid fired" << endl
             << "--- you are here ----" << endl
             << *this << endl
             << "---------------------" << endl;
    }

    bool valid = true;

    //should check that every data[i] < data[i+1]
    for(int i = 0; i < data_count-1; i++){
        if(data[i] >= data[i+1]){
            valid = false;
            cout << "---------- ERROR ----------" << endl;
            cout << data[i] << " should be < " << data[i+1] << endl;
        }
    }

    //data[data_count-1] must be less than equal to
    //  every subset[child_count-1]->data[ ]
    if(!is_leaf()){
        T last_item = data[data_count-1];
        int last_child_data_count;
        last_child_data_count=subset[child_count-1]->data_count;
        for(int i=0; i<last_child_data_count; i++){
            if(last_item > subset[child_count-1]->data[i]){
                valid = false;
                cout << "---------- ERROR ----------" << endl;
                cout << last_item << " must be <= " << subset[child_count-1]->data[i] << endl;
            }
        }
    }

    //every data[i] is greater than every subset[i]->data[ ]
    if(!is_leaf()){
        for(int i=0; i<data_count; i++){
            for(int j=0; j<subset[i]->data_count; j++){
                if(data[i]<=subset[i]->data[j]){
                    valid = false;
                    cout << "---------- ERROR ----------" << endl;
                    cout << data[i] << " must be > " << subset[i]->data[j] << endl;
                }
            }
        }
    }

    //B+Tree: Every data[i] is equal to subset[i+1]->smallest
    if(!is_leaf()){
        T entry;
        for(int i=0; i<data_count; i++){
            subset[i+1]->get_smallest(entry);
            if(data[i] != entry){
                valid = false;
                cout << "---------- ERROR ----------" << endl;
                cout << data[i] << " must be equal to " << entry << endl;
            }
        }
    }

    //Recursively validate every subset[i]
    if(!is_leaf()){}
        for(int i=0; i<child_count; i++)
            valid = subset[i]->is_valid();
    return valid;
}

template <class T>
typename BPlusTree<T>::Iterator BPlusTree<T>::begin(){
    return Iterator(get_smallest_node());
}

template <class T>
typename BPlusTree<T>::Iterator BPlusTree<T>::end(){
    return Iterator(NULL);
}

template <class T>
T* BPlusTree<T>::find_ptr(const T& entry){
    T* find_me = NULL;
    int i = first_ge(data, data_count, entry);
    bool found = ((i < data_count) && (data[i] == entry));
    if(is_leaf()){
        if(found)
            find_me = &data[i];
        return find_me;
    }
    else{
        if(found)
            return subset[i+1]->find_ptr(entry);
        return subset[i]->find_ptr(entry);
    }
}

template <class T>
void BPlusTree<T>::loose_insert(const T& entry){
    const bool debug = false;

    if(debug){
        cout << "loose_insert fired" << endl;
    }

    int i = first_ge(data, data_count, entry);
    bool found = ((i < data_count) && (data[i] == entry));

    if(is_leaf()){
        if(!found){ // entry needs to be copied to a leaf
            insert_item(data, i, data_count, entry);
            if(debug) cout << entry << " inserted" << endl;
        }
        else{}
            // How do I deal with duplicates here?
    }
    else{
        if(found){ // "bread crumbs" of entry found, see if there's a copy in the leaves
            subset[i+1]->loose_insert(entry);
            if(subset[i+1]->data_count > MAXIMUM)
                fix_excess(i+1);
        }
        else{ // keep looking
            subset[i]->loose_insert(entry);
            if(subset[i]->data_count > MAXIMUM)
                fix_excess(i);
        }
    }
    /*
           int i = first_ge(data, data_count, entry);
           bool found = (i<data_count && data[i] == entry);

           three cases:
             found
             a. found/leaf: deal with duplicates: call +
             b. found/no leaf: subset[i+1]->loose_insert(entry)
                               fix_excess(i+1) if there is a need

             ! found:
             c. !found / leaf : insert entry in data at position i
             c. !found / !leaf: subset[i]->loose_insert(entry)
                                fix_excess(i) if there is a need

                |   found          |   !found         |
          ------|------------------|------------------|-------
          leaf  |a. dups? +        | c: insert entry  |
                |                  |    at data[i]    |
          ------|------------------|------------------|-------
                | b.               | d.               |
                |subset[i+1]->     | subset[i]->      |
          !leaf | loose_insert(i+1)|  loose_insert(i) |
                |fix_excess(i+1)   | fix_excess(i)    |
                |                  |                  |
          ------|------------------|------------------|-------
        */
}

template <class T>
void BPlusTree<T>::fix_excess(int i){
    const bool debug = false;
    if(debug) cout << "fix_excess(" << i << ") fired" << endl;

    //this node's child i has one too many items: 3 steps:
    //1a. add a new subset at location i+1 of this node
    BPlusTree<T> *new_child_ptr = new BPlusTree<T>;
    insert_item(subset, i+1, child_count, new_child_ptr);

    // 1b. Point the excess child's next to the new child:
    if(subset[i]->is_leaf())
        subset[i]->next = subset[i+1];

    //2. split subset[i] (both the subset array and the data array) and move half into
    //      subset[i+1] (this is the subset we created in step 1.)
    split(subset[i]->data, subset[i]->data_count, subset[i+1]->data, subset[i+1]->data_count);
    split(subset[i]->subset, subset[i]->child_count, subset[i+1]->subset, subset[i+1]->child_count);

    //3. detach the last data item of subset[i] and bring it and insert
    //      it into this node's data[]
    T entry;
    detach_item(subset[i]->data, subset[i]->data_count, entry);
    insert_item(data, i, data_count, entry);

    //  000 B_PLUS_TREE
    //  if (subset[i]->is_leaf())
    //  transfer the middle entry to the right and...
    //  Deal with next pointers. just like linked list insert
    if(subset[i+1]->is_leaf()){
        insert_item(subset[i+1]->data, 0, subset[i+1]->data_count, entry);
    }

    //Note that this last step may cause this node to have too many itesm.
    //  This is OK. This will be
    //  dealt with at the higher recursive level. (my parent will fix it!)
    //
    //NOTE: ODD/EVENNESS
    // when there is an excess, data_count will always be odd
    //  and child_count will always be even.
    //  so, splitting is always the same.


}

template <class T>
void BPlusTree<T>::loose_remove(const T& entry){
    //
}

template <class T>
BPlusTree<T>* BPlusTree<T>::fix_shortage(int i){
    //
}

template <class T>
BPlusTree<T>* BPlusTree<T>::get_smallest_node(){
    if(!is_leaf())
        return subset[0]->get_smallest_node();
    else
        return this;
}

template <class T>
void BPlusTree<T>::get_smallest(T& entry){
    if(!is_leaf())
        subset[0]->get_smallest(entry);
    else
        entry = data[0];
}

template <class T>
void BPlusTree<T>::get_biggest(T& entry){
    //
}

template <class T>
void BPlusTree<T>::remove_biggest(T& entry){
    //
}

template <class T>
void BPlusTree<T>::transfer_left(int i){
    //
}

template <class T>
void BPlusTree<T>::transfer_right(int i){
    //
}

template <class T>
BPlusTree<T>* BPlusTree<T>::merge_with_next_subset(int i){
    //
}


#endif // BPLUSTREE_H
