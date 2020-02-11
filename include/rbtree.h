#ifndef _RBTREE_H
#define _RBTREE_H

#define NULL ((void*)0)
#define BLACK 0
#define RED 1

struct rb_node{
    struct rb_node* left;
    struct rb_node* right;
    struct rb_node* parent;
    int color;
    };

 void rotate_left(struct rb_node *n);
 void rotate_right(struct rb_node *n);
 struct rb_node *grandparent(struct rb_node * n);
 struct rb_node *uncle(struct rb_node* n);
 void insert_case1(struct rb_node* n);
 void insert_case2(struct rb_node* n);
 void insert_case3(struct rb_node* n);
 void insert_case4(struct rb_node* n);
 void insert_case5(struct rb_node *n);
 struct rb_node *sibling(struct rb_node * n);
 int  is_leaf(struct rb_node*n);
 void replace_node(struct rb_node* n, struct rb_node* child);
 void delete_one_child(struct rb_node * n);
 void delete_case1(struct rb_node *n);
 void delete_case2(struct rb_node *n);
 void delete_case3(struct rb_node *n);
 void delete_case4(struct rb_node *n);
 void delete_case5(struct rb_node *n);
 void delete_case6(struct rb_node *n);

#endif