//
//  hash_table.hpp
//  wet2
//
//  Created by Théo Adraï on 07/06/2018.
//  Copyright © 2018 Théo Adraï. All rights reserved.
//

#ifndef hash_table_hpp
#define hash_table_hpp
#include <stdio.h>
#include <cassert>
#include "list.hpp"

template <class T>
class Hash_table {
    int _size;
    int _insertions_num;
    List<T>* _array;
    
    class Func {
    public:
        int operator()(const T & val, int size) { //suppose operator () for T
            int i= val.operator()();
            return i%size;
        }
    };
    
    Func _f;
public :
    class exception {};
    class already_exist : public exception {};
    class dont_exist : public exception {};
    Hash_table (int size=10) : _size(size), _insertions_num(0) {
        if(size)
            _array=new List<T>[size];
    }
    
    ~Hash_table () {
        delete [] _array;
    }
    
    void quick_insert (const T & val) { //suppose that there is enough place, hence _size>_insertion_num
        assert(_insertions_num<=_size);
        int i=(_f)(val,_size);
        assert(i>=0 && i<_size);
        _array[i].List<T>::head_insert(val); //can throw bad alloc;
        _insertions_num++;
    }
    
    void resize () { //can throw bad alloc;
        List<T>* new_array=new List<T>[_insertions_num*2+2];
        int i;
        for (i=0; i<_size; i++) {
            while (1) {
                try {
                    int index=_f.operator()(_array[i].List<T>::get_last(), _insertions_num*2+2);
                    _array[i].List<T>::transfer_last( new_array[index] );
                }
                catch(typename List<T>::Empty &) {
                    break;
                }
            }
        }
        delete [] _array;
        _array=new_array;
        _size=_insertions_num*2+2;
        
    }
    
    void insert (const T & val) { //can throw bad alloc, already_exist;
        if (_insertions_num+1>_size) resize();
        
        assert(_insertions_num<_size);
        int i=(_f)(val,_size);
        assert(i>=0 && i<_size);
        try {
            _array[i].List<T>::exclusive_insert(val); //can throw bad alloc;
        }
        catch(typename List<T>::already_exist &) {
            throw already_exist();
        }
        _insertions_num++;
    }
    
    T & find (const T & val) {
        int i=(_f)(val,_size);
        try {
            return _array[i].List<T>::get_data(val);
        } catch (typename List<T>::dont_exist &) {
            throw dont_exist();
        }
    }
};
#endif /* hash_table_hpp */
