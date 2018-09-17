//
//  min_heap.hpp
//  wet2
//
//  Created by Théo Adraï on 08/06/2018.
//  Copyright © 2018 Théo Adraï. All rights reserved.
//

#ifndef min_heap_hpp
#define min_heap_hpp
#include <stdio.h>
#include <new>
#include <cassert>
template <class T>
class Min_heap {
    int _next_free_index;
    int _array_size;
    
public:
    class Node {
    public:
        int _index;
        T _data;
        Node (int index, T val) : _index(index), _data(val) {} //c'tor for T
    };
    
private:
    Node** _array;
    
public:
    class Empty {};
    Min_heap (): _next_free_index(1), _array_size(10) {
        _array = _array = new Node*[_array_size];
        for (int i = 0; i < 10; i++) _array[i]=NULL;
    }
    Min_heap (int n, T* array, Node** node_pointers)
    {
        int closest_pow_of_2 = 1;
        while ( n >= closest_pow_of_2) closest_pow_of_2 *= 2;
        _array_size = closest_pow_of_2;
        _next_free_index = n+1;
        _array = new Node*[_array_size];
        for ( int i = 0; i < _array_size; i++) {
            if (i == 0 || i >= _next_free_index ) _array[i] = NULL;
            else {
                try {
                    _array[i] = new Node(i, array[i-1]);
                    node_pointers[i-1]=_array[i];
                }
                catch (std::bad_alloc &) {
                    for (int j = 1; j < i; j++) delete _array[j];
                    delete [] _array;
                    throw;
                }
            }
        }
        for ( int i = n/2; i > 0; i--)
            sift_down (i);
        
    }
    ~Min_heap () {
        for ( int i = 1; i < _next_free_index; i++)
            delete _array[i];
        
        delete [] _array;
    }
    
    int sift_down (int i) {
        assert( 2*i < _next_free_index );
        while ( 2*i < _next_free_index ) {
            Node* father = _array[i];
            Node* left = _array[2*i];
            assert (left);
            Node* right=_array[2*i+1];
            if ( father->_data < left->_data && (!right || father->_data < right->_data) ) break;
            if ( !right || left->_data < right->_data ) {
                _array[i] = left;
                left->_index = i;
                _array[2*i] = father;
                father->_index = 2*i;
                i=2*i;
            }
            else {
                assert(right);
                _array[i] = right;
                right->_index = i;
                _array[2*i+1] = father;
                father->_index = 2*i+1;
                i=2*i+1;
            }
        }
        return i;
    }
    
    int sift_up (int i) {
        assert( i < _next_free_index );
        while ( i > 1 ) {
            assert( i/2 >= 1 && i/2 < _next_free_index );
            Node* father = _array[i/2]; assert(father);
            Node* son = _array[i]; assert(son);
            
            if ( father->_data < son->_data ) break;
            _array[i/2] = son;
            son->_index = i/2;
            _array[i] = father;
            father->_index = i;
            i=i/2;
        }
        return i;
    }
    
    Node* insert (const T & val) {
        if ( _next_free_index < _array_size ) {
            _array[_next_free_index] = new Node(_next_free_index, val);
            return _array[sift_up(_next_free_index++)];
        }
        assert(_next_free_index >= _array_size && _next_free_index < 2*_array_size);
        Node** new_array = new Node*[2*_array_size];
        for (int i=0 ; i<_next_free_index; i++)
            new_array[i] = _array[i];
        try {
            new_array[_next_free_index] = new Node(_next_free_index, val);
        }
        catch ( std::bad_alloc & ) {
            delete [] new_array;
        }
        for ( int i = _next_free_index+1; i < 2*_array_size; i++)
            new_array[i] = NULL;
        delete [] _array;
        _array = new_array;
        _array_size = 2*_array_size;
        return _array[sift_up(_next_free_index++)];
    }
    
    void Dec_key ( int i, const T & val ) {
        assert(i < _next_free_index && i > 0);
        Node* vertex = _array[i];
        vertex->_data = val; //operator = for T
        sift_up(i);
    }
    
    const T & find_min () const {
        if(_next_free_index <= 1) throw Empty();
        return _array[1]->_data;
    }
    
    void Del_min () {
        if (_next_free_index == 1) throw Empty();
        delete _array[1];
        _array[1] = _array[_next_free_index-1];
        _array[--_next_free_index] = NULL;
        if(_array[1]) {
            _array[1]->_index = 1;
            if(_array[2]) sift_down(1);
        }
    }
    
};
#endif /* min_heap_hpp */
