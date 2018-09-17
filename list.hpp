//
//  list.hpp
//  wet2
//
//  Created by Théo Adraï on 07/06/2018.
//  Copyright © 2018 Théo Adraï. All rights reserved.
//

#ifndef list_hpp
#define list_hpp
#include <stdio.h>
#include <cassert>
template <class T>
class List {
    
public:
    class Node {
    public:
        T _data;
        Node* _next;
        Node() : _next(NULL) {} //suppose default c'tor for T
        Node(T val) : _data(val), _next(NULL) {} //suppose copy c'tor for T
    };
private:
    Node* _dummie;
    Node* _last;
public:
    //exceptions :
    class exceptions {};
    class already_exist : public exceptions {};
    class dont_exist : public exceptions {};
    class Empty : public exceptions {};
    List () {
        _dummie=new Node(); //can throw bad_alloc
        _last=_dummie;
    }
    
    ~List () {
        Node* ptr=_dummie;
        while(ptr) {
            Node* to_destroy=ptr;
            ptr=ptr->_next;
            delete to_destroy;
        }
    }
    
    void head_insert (const T & val) { //can throw bad_alloc
        Node* new_node=new Node(val);
        Node* ptr=_dummie->_next;
        _dummie->_next=new_node;
        new_node->_next=ptr;
        if(!new_node->_next) _last=new_node;
    }
    
    /*don't allow identical objects, can throw bad alloc, can throw already exists,
     suppose == operator for T*/
    void exclusive_insert (const T & val) {
        Node* new_node=new Node(val);
        Node* ptr=_dummie;
        while(ptr->_next) {
            if (ptr->_next->_data==val) {
                delete new_node;
                throw already_exist();
            }
            ptr=ptr->_next;
        }
        ptr->_next=new_node;
        _last=new_node;
    }
    
    Node* tail_insert (const T & val) {
        Node* new_node=new Node(val);
        _last->_next=new_node;
        _last = new_node;
        return _last;
    }
    
    void transfer_last (List & dst_list) { //*this, is the source list, non exclusive
        Node* ptr=_dummie;
        if(!_dummie->_next) throw Empty();
        Node* node_to_tranfer;
        //find the node one before the last
        while (ptr->_next->_next) ptr=ptr->_next;
        
        node_to_tranfer=ptr->_next;
        assert(node_to_tranfer==_last);
        ptr->_next=NULL;
        _last=ptr;
        ptr=dst_list._dummie;
        //add the node to the tail of the destination list
        while (ptr->_next) ptr=ptr->_next;
        
        ptr->_next=node_to_tranfer;
        dst_list._last=node_to_tranfer;
        assert(!(node_to_tranfer->_next));
    }
    
    T & get_data (const T & val) {
        Node* ptr=_dummie->_next;
        while(ptr) {
            if (ptr->_data==val) return ptr->_data;
            ptr=ptr->_next;
        }
        throw dont_exist();
    }
    
    T & get_last () {
        return _last->_data;
    }
    
    void delete_last () {
        Node* ptr=_dummie;
        while (ptr->_next->_next) ptr=ptr->_next;
        assert(ptr->_next==_last);
        delete ptr->_next;
        ptr->_next=NULL;
        _last=ptr;
    }
};
#endif /* list_hpp */
