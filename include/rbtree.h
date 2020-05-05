#ifndef _RBTREE_H
#define _RBTREE_H

#define NULL ((void*)0)
#define BLACK 0
#define RED 1

struct rb_node{
    unsigned long  __rb_parent_color;
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
#define RB_ROOT_CACHED (struct rb_root_cached) { {NULL, }, NULL }

struct rb_root {
	struct rb_node *rb_node;
};

 struct rb_root_cached {
	struct rb_root rb_root;
	struct rb_node *rb_leftmost;
};


static inline void rb_link_node(struct rb_node *node, struct rb_node *parent,
				struct rb_node **rb_link)
{
	node->__rb_parent_color = (unsigned long)parent;
	node->left = node->right = NULL;

	*rb_link = node;
}


static inline void rb_insert_color_cached(struct rb_node *node,
					  struct rb_root_cached *root,
					  int leftmost)
{
	if (leftmost)
		root->rb_leftmost = node;
	insert_case1(node);
}

#define rb_first_cached(root) (root)->rb_leftmost
#define	rb_entry(ptr, type, member) container_of(ptr, type, member)

#endif