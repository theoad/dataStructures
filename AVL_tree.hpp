//  AVL_tree.hpp
//  AVL_tree
//
//  Created on 30/04/2018.
//  by Theo Adrai
//
/*
 Generic AVL tree needed operators for class T : <,== (no need for copy c'tor)

 INTERFACE :

 n is the size of the tree, h its height


 c'tors :
 AVL_tree (Binary_node* r=NULL); .............  O(1)
 
 AVL_tree (const AVL_tree & t); ..............  O(n)
    throws std::bad_alloc


 operators :
 AVL_tree & operator=(const AVL_tree & t); ...  O(n)
    throws std::bad_alloc
 
 AVL_tree & operator+(AVL_tree & t); .........  O(n)
    throws std::bad_alloc


 elements actions:
 void balanced_insert(const T & val); ........  O(log n)
    throws AVL_tree::key_already_exists, std::bad_alloc
 
 void balanced_delete (const T & val); .......  O(log n)
    throws AVL_tree::key_not_found, std::bad_alloc
 
 bool is_empty () const; .....................  O(1)
 
 T& get (const T & val); .....................  O(log n)
    throws AVL_tree::key_nod_found


 iterators :
 inorder_iterator in_begin() const; .......... O(1)
 inorder_iterator in_end() const; ............ O(1)
 inorder_iterator & operator++(); ............ O(h)=O(log n)

 postorder_iterator post_begin() const; ...... O(1)
 postorder_iterator post_end() const; ........ O(1)
 postorder_iterator & operator++(); .......... O(h)=O(log n)

 */
#ifndef AVL_tree_hpp
#define AVL_tree_hpp
#include <stdio.h>
#include <cassert>
#include <new>
using namespace std;
template <class T>
/*================================AVL tree====================================*/
class AVL_tree {

    /*------------------------AVL tree Binary node----------------------------*/
    class Binary_node {
    public:
        T _data;
        int _height;
        Binary_node* _left;
        Binary_node* _right;
        Binary_node* _parent;
        Binary_node (const T & val, int h=0, Binary_node* l=NULL, Binary_node* r=NULL, Binary_node* p=NULL)
                : _data(val), _height(h), _left(l), _right(r), _parent(p) {}
        ~Binary_node() {
            if(_left) _left->_parent=NULL;
            if(_right) _right->_parent=NULL;
            if(_parent) {
                if(_parent->_left==this)
                    _parent->_left=NULL;
                else {
                    assert(_parent->_right==this);
                    _parent->_right=NULL;
                }
            }
        }
        Binary_node (const Binary_node & b) :
                _data(b._data), _height(b._height), _left(b._left), _right(b._right), _parent(b._parent){}
        Binary_node & operator=(const Binary_node &) = delete;

        int BF() {
            int h_l = _left ? _left->_height : -1;
            int h_r = _right ? _right->_height : -1;
            return h_l-h_r;
        }
        int H() {
            int h_l = _left ? _left->_height : -1;
            int h_r = _right ? _right->_height : -1;
            return h_l>h_r ? h_l+1 : h_r+1;
        }
    };
    /*------------------------------------------------------------------------*/

    Binary_node* root;



public:
    /*Exceptions*/
    class Error {};
    class key_not_found : public Error {};
    class key_already_exists : public Error {};

    /*---------------------------inorder iterator-----------------------------*/
    class inorder_iterator {
        Binary_node* _ptr;
    public:
        inorder_iterator (Binary_node* r) : _ptr(r) {}

        Binary_node* get() const {return _ptr;}
        T& get_data() const {return _ptr->_data;}

        bool operator==(const inorder_iterator & i) const {
            return _ptr==i._ptr;
        }
        bool operator!=(const inorder_iterator & i) const {
            return _ptr!=i._ptr;
        }

        inorder_iterator & operator++() {
            /*remarks: myself=the current node in this->_ptr*/

            //if I visited myself, the next node to visit
            //is the most left grand son of my right son
            if(_ptr->_right) {
                _ptr=_ptr->_right;
                while(_ptr->_left)
                    _ptr=_ptr->_left;
            }

                //check if we got to the end of the iteration
            else if (!_ptr->_parent)
                _ptr=NULL;

                /*According to the inorder iteration, If I got to myself,
                 I have already visited my left sub tree.
                 If we got there, I don't have right son.*/

                //If I'm left son of my father and I visited myself
                //(left sub tree done+no right child),
                //I know have to visit my father
            else if (_ptr->_parent->_left==_ptr) {
                assert(_ptr->_right==NULL);
                _ptr=_ptr->_parent;
            }
            else {
                //if we got here I am right son.
                assert(_ptr->_right==NULL && _ptr->_parent->_right==_ptr);
                while(_ptr->_parent!=NULL && _ptr->_parent->_right==_ptr)
                    _ptr=_ptr->_parent;
                if(!_ptr->_parent) _ptr=NULL;
                else {
                    assert(_ptr->_parent->_left==_ptr);
                    _ptr=_ptr->_parent;
                }
            }
            return *this;
        }
    };
    inorder_iterator in_begin() const {
        Binary_node* temp=root;
        if(temp) {
            while(temp->_left!=NULL)
                temp=temp->_left;
        }
        return inorder_iterator(temp);
    }
    inorder_iterator in_end() const {
        return inorder_iterator(NULL);
    }
    
    /*-------------------------postorder iterator-----------------------------*/
    class postorder_iterator {
        Binary_node* _ptr;
    public:
        postorder_iterator (Binary_node* r) : _ptr(r) {
            if(_ptr)
                while(_ptr->_left)
                    _ptr=_ptr->_left;
        }
	postorder_iterator & operator++() {
            if (!_ptr->_parent) _ptr=NULL;
            else if (_ptr->_parent->_right==_ptr) _ptr=_ptr->_parent;
            else {
                assert(_ptr->_parent->_left==_ptr);
                _ptr=_ptr->_parent;
                if(_ptr->_right) {
                    _ptr=_ptr->_right;
                    while(1) {
                        while(_ptr->_left)
                            _ptr=_ptr->_left;
                        
                        if(_ptr->_right) _ptr=_ptr->_right;
                        else {
                            break;
                        }
                    }
                }
            }
            return *this;
        }
        Binary_node* get() const {return _ptr;}
        bool operator==(const postorder_iterator & i) const {
            return _ptr==i._ptr;
        }
        bool operator!=(const postorder_iterator & i) const {
            return _ptr!=i._ptr;
        }
    };

    postorder_iterator post_begin() const {
        Binary_node* temp=root;
        if(temp) {
            while(1) {
                while(temp->_left)
                    temp=temp->_left;
                
                if(temp->_right) temp=temp->_right;
                else {
                    break;
                }
	    }
	}
        return postorder_iterator(temp);
    }
    postorder_iterator post_end() const {
        return postorder_iterator(NULL);
    }

    /*----------roligns. return the new root. updates the _heights------------*/
    Binary_node* LL (Binary_node* B) {
        assert(B);
        Binary_node* A=B->_left;
        assert(A);

        if(!B->_parent) {
            A->_parent=NULL;
            root=A;
        }
        else {
            if(B->_parent->_left==B)
                B->_parent->_left=A;
            else
                B->_parent->_right=A;
            A->_parent=B->_parent;
        }
        B->_parent=A;
        B->_left=A->_right;
        if(A->_right) A->_right->_parent = B;
        A->_right=B;

        B->_height=B->H();
        A->_height=A->H();
        return A;
    }
    Binary_node* RR (Binary_node* B) {
        assert(B);
        Binary_node* A=B->_right;
        assert(A);

        if(!B->_parent) {
            A->_parent=NULL;
            root=A;
        }
        else {
            if(B->_parent->_left==B)
                B->_parent->_left=A;
            else
                B->_parent->_right=A;
            A->_parent=B->_parent;
        }
        B->_parent=A;
        B->_right=A->_left;
        if(A->_left) A->_left->_parent = B;
        A->_left=B;

        B->_height=B->H();
        A->_height=A->H();
        return A;
    }
    Binary_node* LR (Binary_node* C) {
        assert(C);
        Binary_node* B=C->_left;
        assert(B && C->BF()==2 && B->BF()==-1);
        Binary_node* A=B->_right;
        Binary_node* temp=RR(B);
        assert(A==temp);
        assert(A->_parent==C);
        LL(C);
        return C;
    }
    Binary_node* RL (Binary_node* C) {
        assert(C);
        Binary_node* B=C->_right;
        assert(B && C->BF()==-2 && B->BF()>=0);
        Binary_node* A=B->_left;
        Binary_node* temp=LL(B);
        assert(A==temp);
        assert(A->_parent==C);
        RR(C);
        return C;
    }
    void rolling (Binary_node* B) {
        assert(B);
        if(B->BF()==2) {
            Binary_node* A=B->_left;
            assert(A);
            if (A->BF()>=0)
                LL(B);
            else
                LR(B);
        }
        else {
            assert(B->BF()==-2);
            Binary_node* A=B->_right;
            assert(A);
            if(A->BF()<=0)
                RR(B);
            else
                RL(B);
        }
    }
    /*===============================Methodes=================================*/

    AVL_tree (Binary_node* r=NULL) : root(r) {}
    ~AVL_tree () {
        AVL_tree::postorder_iterator it=post_begin();
        while (it!=post_end()) {
            Binary_node* to_delet = it.get();
            ++it;
            delete to_delet;
        }
    }
    
    //helper function to swap 2 nodes (don't change data and don't use copy c'tor of T)
    void swap_nodes (Binary_node* node_1, Binary_node* n2) {
        Binary_node* node_1_parent=node_1->_parent;
        Binary_node* node_1_left=node_1->_left;
        Binary_node* node_1_right=node_1->_right;

        //node_1 parent
        if(n2->_parent==node_1)
            node_1->_parent=n2;
        else {
            node_1->_parent=n2->_parent;
            if(n2->_parent) {
                if(n2->_parent->_right==n2)
                    n2->_parent->_right=node_1;
                else {
                    assert(n2->_parent->_left==n2);
                    n2->_parent->_left=node_1;
                }
            }
            else {
                assert(root==n2);
                root=node_1;
            }
        }

        //node_1 left
        if(n2->_left==node_1)
            node_1->_left=n2;
        else {
            node_1->_left=n2->_left;
            if(n2->_left)
                n2->_left->_parent=node_1;
        }


        //node_1 right
        if(n2->_right==node_1)
            node_1->_right=n2;
        else {
            node_1->_right=n2->_right;
            if(n2->_right)
                n2->_right->_parent=node_1;
        }

        //n2 parent
        if(node_1_parent==n2)
            n2->_parent=node_1;
        else {
            n2->_parent=node_1_parent;
            if(node_1_parent) {
                if(node_1_parent->_right==node_1)
                    node_1_parent->_right=n2;
                else {
                    assert(node_1_parent->_left==node_1);
                    node_1_parent->_left=n2;
                }
            }
            else {
                assert(root==node_1);
                root=n2;
            }
        }

        //n2 left
        if(node_1->_left==n2)
            n2->_left=node_1;
        else {
            n2->_left=node_1_left;
            if(node_1_left)
                node_1_left->_parent=n2;
        }

        //n2 right
        if(node_1_right==n2)
            n2->_right=node_1;
        else {
            n2->_right=node_1_right;
            if(node_1_right)
                node_1_right->_parent=n2;
        }

        int node_1_height=node_1->_height;
        node_1->_height=n2->_height;
        n2->_height=node_1_height;


    }
    //don't update the parent height. return the inserted node. can return NULL.
    //can return 2 exceptions : std::bad_alloc and AVL_tree<T>::key_already_exist
    Binary_node* insert (const T & val) {
        Binary_node* node=new Binary_node(val);
        Binary_node* p=root;
        if(!p) {
            root=node;
            return node;
        }
        while (1) {
            if(p->_data<val) {
                if(!p->_right) {
                    p->_right=node;
                    node->_parent=p;
                    return node;
                }
                p=p->_right;
            }
            else if(p->_data==val) {
               	 delete node;
		 throw key_already_exists();
            }
            else {
                if(!p->_left) {
                    p->_left=node;
                    node->_parent=p;
                    return node;
                }
                p=p->_left;
            }
        }
    }
    //don't update heights. return the parent of the deleted node. can return NULL.
    //can return 1 exception
    Binary_node* delet (const T & val) {
        Binary_node* p=root;
        if(!p) {
            throw key_not_found();
        }

        while (1) {
            if(p->_data<val) {
                if(!p->_right)
                    throw key_not_found();
                p=p->_right;
            }
            else if(p->_data==val) { //We want to destroy p
                if (p->_left && p->_right) { //We want to find the next element after p
                    inorder_iterator it(p);
                    ++it; //inorder iteration return the next element (inorder visit = sorted visit)
                    swap_nodes(p, it.get());
                }
                assert((!p->_left || !p->_right) && p->H()<=1); //the searched node has less than 2 sons
                Binary_node* only_son;
                Binary_node* parent=p->_parent;

                if(p->_left || p->_right) { //if it has one son, connect the son to the father
                    only_son=p->_left;
                    if(p->_right) only_son=p->_right;
                    assert(only_son);
                    if(!parent) {
                        delete p;
                        root=only_son;
                        only_son->_parent=NULL;
                    }
                    else if(parent->_right==p) {
                        delete p;
                        parent->_right=only_son;
                        only_son->_parent=parent;
                    }
                    else {
                        assert(parent->_left==p);
                        delete p;
                        parent->_left=only_son;
                        only_son->_parent=parent;
                    }
                }
                else {
                    if(!parent) root=NULL;
                    delete p;
                }
                return parent;
            }
            else {
                if(!p->_left)
                    throw key_not_found();
                p=p->_left;
            }
        }
    }

    void balanced_insert(const T & val) {
        Binary_node* v=insert(val); //can throw exception
        while (v->_parent) {
            Binary_node* p=v->_parent;
            if(p->_height >= v->_height+1)
                break;
            p->_height=p->H();
            assert(p->_height==v->_height+1);
            if(p->BF()>1 || p->BF()<-1) {
                rolling(p);
                break;
            }
            v=p;
        }
    }

    void balanced_delete (const T & val) {
        Binary_node* v=delet(val); //can throw exception
        while (v) {
            v->_height=v->H();
            if(v->BF()>1 || v->BF()<-1)
                rolling(v);
            v=v->_parent;
        }
    }

    T& get (const T & val) const {
        Binary_node* ptr=root;
        while (ptr!=NULL) {
            if(ptr->_data<val)
                ptr=ptr->_right;
            else if(ptr->_data==val)
                return ptr->_data;
            else
                ptr=ptr->_left;
        }
        throw key_not_found();
    }
    /*
    operator() : bonus tu use the operator + between 2 trees.
    if not relevant add to the T type :

        bool operator() const {
            reutrn true;
        }

    if relevant the () operator should return if an object should be added to the new tree

    the eaten tree t stays unchanged. At the end of the method,
     this contains all the elements in this and t that return true to the operator ().*/
    AVL_tree & operator+(AVL_tree & t) {

        int length1 = 0,length_to_delet=0, length2 = 0;

        for (AVL_tree::inorder_iterator it = this->in_begin(); it != this->in_end(); ++it) {
            if(it.get_data().operator()())
                length1++;
            else
                length_to_delet++;
        }

        for (AVL_tree::inorder_iterator it = t.in_begin(); it != t.in_end(); ++it)
            if(it.get_data().operator()())
                length2++;

        int length_dest=length1+length2;
        Binary_node **nodes_to_delet;
        Binary_node **to_merge_array1;
        Binary_node **to_merge_array2;
        Binary_node **dest_array;

        //memory allocation :
        //if allocation fails, we must liberate allocated memory
        nodes_to_delet= new Binary_node*[length_to_delet];
        try{
            to_merge_array1 = new Binary_node*[length1];
        }
        catch(std::bad_alloc&) {
            delete [] nodes_to_delet;
            throw std::bad_alloc();
        }
        try {
            to_merge_array2 = new Binary_node*[length2];
        }
        catch(std::bad_alloc&) {
            delete [] nodes_to_delet;
            delete [] to_merge_array1;
            throw std::bad_alloc();
        }
        try {
            dest_array = new Binary_node*[length_dest];
        }
        catch(std::bad_alloc&) {
            delete [] nodes_to_delet;
            delete [] to_merge_array1;
            delete [] to_merge_array2;
            throw std::bad_alloc();
        }

        int i=0;
        int i_=0;
        for (AVL_tree::inorder_iterator it = this->in_begin(); it != this->in_end(); ++it) {
            if(it.get_data().operator()()){
                assert(i<length1);
                to_merge_array1[i++] = it.get();
            }
            else
                nodes_to_delet[i_++]= it.get();
        }
        for (int i=0; i<length_to_delet; i++) {
            delete nodes_to_delet[i];
	    nodes_to_delet[i]=NULL;
        }
        delete [] nodes_to_delet;
        i=0;
        for (AVL_tree::inorder_iterator it = t.in_begin(); it != t.in_end(); ++it) {
            if(it.get_data().operator()()) {
                assert(i<length2);
                try {
                    to_merge_array2[i] = new Binary_node(it.get_data());
                }
                catch(std::bad_alloc&) {
                    for(int j=0; j<i; j++) {
                        delete to_merge_array2[j];
                    }
                    delete [] to_merge_array1;
                    delete [] to_merge_array2;
                    delete [] dest_array;
                    throw std::bad_alloc();
                }
                i++;
            }
        }

        merge(to_merge_array1, length1, to_merge_array2, length2, dest_array, length_dest);
        delete [] to_merge_array1;
        delete [] to_merge_array2;

        root = Create_avl(dest_array, length_dest);
        delete [] dest_array;
        return *this;
    }
    static Binary_node* Create_avl (Binary_node** a, int n) {
        if(n==1) {
            a[0]->_height=0;
            a[0]->_parent=NULL;
            a[0]->_left=NULL;
            a[0]->_right=NULL;
            return a[0];
        }
        if(n<=0) return NULL;

        int mid=n/2;
        Binary_node* root = a[mid];
        Binary_node* left_sub_tree = Create_avl(a, mid);
        Binary_node* right_sub_tree = Create_avl(a+mid+1, n-mid-1);

        root->_parent=NULL;
        root->_left=left_sub_tree;
        root->_right=right_sub_tree;
        if(left_sub_tree) left_sub_tree->_parent=root;
        if(right_sub_tree) right_sub_tree->_parent=root;

        root->_height=root->H();
        assert(root->BF()<=1 && root->BF()>=-1);
        return root;
    }
    static void merge(Binary_node** a,int na, Binary_node** b, int nb, Binary_node** c, int nc){
        int ia =0, ib=0, ic=0;
        while(ia<na && ib<nb) {
            if(a[ia]->_data < b[ib]->_data)
                c[ic++]=a[ia++];
            else
                c[ic++]=b[ib++];
        }
        while(ia<na) c[ic++]=a[ia++];
        while(ib<nb) c[ic++]=b[ib++];
    }
};

#endif /* AVL_tree_hpp */

