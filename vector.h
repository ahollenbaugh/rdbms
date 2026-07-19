#ifndef VECTOR_H
#define VECTOR_H

#include <iostream>
#include <iomanip>
#include <cassert>
#include "vector_lower_level_functions.h"

using namespace std;

template <class T>
class Vector
{
public:

    // constructor:
    Vector(unsigned int capacity = 100);

    // big three:
    ~Vector(); // destructor
    Vector(const Vector &other); // copy constructor
    void operator =(const Vector &rhs); // assignment operator

    // accessor functions:
    const T operator [](const unsigned int index) const;
    T& operator [](const unsigned int index);

    T& at(int index); // return reference to item at position index
    const T at(int index) const; // return a const item at position index

    T& front() const; // return item at position 0
    T& back() const; // return item at last position

    // push and pop functions:
    Vector& operator +=(const T& item); // push back
    void push_back(const T& item); // append to the end
    T pop_back(); // remove the last item and return it

    // insert and erase:
    void insert(int insert_here, const T& insert_this); // insert at pos
    void erase(int erase_index); // erase item at position
    int index_of(const T& item); // search for item and return index

    // size and capacity:
    void set_size(int size); // enlarge the vector to this size
    void set_capacity(int capacity); // allocate this space
    int size() const{return _how_many;} // return _size
    int capacity() const{return _capacity;} // return _capacity
    bool empty() const; // return true if vector is empty

    // output:
    template<class U>
    friend ostream& operator <<(ostream& outs, const Vector<U>& a);

private:
    T* _storage; // points to array
    int _how_many; // number of elements in the array
    int _capacity; // number of elements the array can hold
};

// DEFINITIONS ------------------------------------------------------------------------------

template <class T>
Vector<T>::Vector(unsigned int capacity){
    const bool debug = false;
    _how_many = 0;
    _capacity = capacity;
    _storage = allocate<T>(capacity);
    if(debug){
        cout << endl;
        cout << "New vector created" << endl
             << "SIZE:     " << _how_many << endl
             << "CAPACITY: " << _capacity << endl;
        cout << endl;
    }
}

//template <class T>
//Vector<T>::Vector(T* data_array, unsigned int capacity){
//    _capacity = capacity;
//    _storage = allocate<T>(capacity); // or _capacity? does it matter?

//}

template <class T>
Vector<T>::~Vector(){
    // destructor
    const bool debug = false;
    delete [] _storage;
    if(debug){
        cout << endl;
        cout << "Vector destroyed." << endl;
        cout << endl;
    }
}

template <class T>
Vector<T>::Vector(const Vector<T> &other){
    // copy constructor
    // new object being created
    const bool debug = false;
    if(debug)
        cout << "New vector created using copy ctor." << endl;
    _capacity = other.capacity();
    _how_many = other.size();
    _storage = allocate<T>(_capacity);
    copy_list<T>(_storage, other._storage, _how_many);
        // should I make _storage public or make accessor function?
}

template <class T>
void Vector<T>::operator =(const Vector<T> &rhs){
    // assignment operator
    // object already exists

    // since this object already exists,
    // if the size of its array
    // is not large enough for the contents of
    // the array of rhs, then we will need to
    // delete the original array and reallocate
    // memory.
    const bool debug = false;
    // check for self-assignment:
    if(this == &rhs)
        return;
    if(rhs.size() > _capacity){
        delete [] _storage;
        _capacity = rhs.capacity();
        _storage = allocate<T>(_capacity);
    }
    // now copy the values over:
    copy_list<T>(_storage, rhs._storage, rhs.size());
    _how_many = rhs.size();

    if(debug) cout << "Vector set equal to another vector using assignment operator" << endl;

    // capacity is already set
    // We can only change it by deleting
    // the original array and reallocating
    // memory for it so that it can
    // accomodate the size of rhs.

    // otherwise, this is identical to the copy ctor.
}

template <class T>
const T Vector<T>::operator [](const unsigned int index) const{
    const bool debug = false;
    if(debug) cout << "const []" << endl;
    return at(index);
}

template <class T>
T& Vector<T>::operator [](const unsigned int index){
    const bool debug = false;
    if(debug) cout << "non-const []" << endl;
    return at(index);
}

template <class T>
T& Vector<T>::at(int index){
    // assert(index >= 0 && index < size());
    return _storage[index];
}

template <class T>
const T Vector<T>::at(int index) const{
    assert(index >= 0 && index < size());
    return _storage[index];
}

template <class T>
T& Vector<T>::front() const{
    // what if size == 0? what should I return?
    return _storage[0];
}

template <class T>
T& Vector<T>::back() const{
    // what if size == 0? what should I return?
    return _storage[_how_many-1];
}

template <class T>
Vector<T>& Vector<T>::operator +=(const T& item){
    push_back(item);
    return *this;
}

template <class T>
void Vector<T>::push_back(const T& item){
    const bool debug = false;
    _storage = add_entry(_storage, item, _how_many, _capacity);
    if(debug){
        cout << endl;
        cout << "push_back" << endl
             << "SIZE:     " << _how_many << endl
             << "CAPACITY: " << _capacity << endl
             << "_storage: ";
        print_list(_storage, _how_many);
        cout << endl;
    }
}

template <class T>
T Vector<T>::pop_back(){
    const bool debug = false;
    T *item = remove_entry(_storage, _how_many, _capacity);
//    _storage = remove_entry(_storage, _how_many, _capacity);
    if(debug){
        cout << endl;
        cout << "pop_back" << endl
             << "SIZE:     " << _how_many << endl
             << "CAPACITY: " << _capacity << endl
             << "_storage: ";
        print_list(_storage, _how_many);
        cout << endl;
    }
    return *item;
}

template <class T>
void Vector<T>::insert(int insert_here, const T& insert_this){
    // there's some shift right involved here... deal with this later
}

template <class T>
void Vector<T>::erase(int erase_index){
    // there's some shift left involved here... deal with this later
}

template <class T>
int Vector<T>::index_of(const T& item){
    for(int i = 0; i < _how_many; i++){
        if(_storage[i] == item)
            return i;
    }
    return -1; // not in list
}

template <class T>
void Vector<T>::set_size(int size){
    _how_many = size;
}

template <class T>
void Vector<T>::set_capacity(int capacity){
    _capacity = capacity;
}

template <class T>
bool Vector<T>::empty() const{
    if(_how_many == 0)
        return true;
    return false;
}

template<class U>
ostream& operator <<(ostream& outs, const Vector<U>& a){
    for(int i = 0; i < a.size(); i++){
        outs << setw(6) << a._storage[i];
    }
    outs << endl;
    return outs;
}

#endif // VECTOR_H
