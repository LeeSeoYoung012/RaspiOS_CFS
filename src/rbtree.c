#include "rbtree.h"

 void rotate_left(struct rb_node *n)
{
    struct rb_node *c = n->right;
    struct rb_node *p = n->parent;
    if(c->left!= NULL)
        c->left->parent = n;
    n->right = c->left;
    n->parent =c;
    c->left = n;
    c->parent =p;

    if(p!=NULL){
        if(p->left == n)
            p->left =c;
        else
        {
            p->right =c;
        }
        
    }
}

 void rotate_right(struct rb_node *n)
{
    struct rb_node *c = n->left;
    struct rb_node *p = n->parent;

    if(c->right !=NULL)
    { c->right->parent = n; }

    n->left = c-> right;
    n->parent = c;
    c->right =n;
    c->parent = p;

    if(p!=NULL)
    {
        if(p->right ==n)
            p->right =c;
        else
            p->left =c;
    }
}


struct rb_node *grandparent(struct rb_node * n)
{
    if((n!=NULL)&&(n->parent!=NULL))
        return n->parent->parent;
    else
        return NULL;
}

struct rb_node *uncle(struct rb_node* n)
{
    struct rb_node* g = grandparent(n);
    if(g==NULL)
        return NULL;
    if(n->parent == g->left)
          return g->right;
    else 
        return g->left;
}

void insert_case1(struct rb_node* n)
{
    if(n->parent == NULL)
        n->color = BLACK;
    else 
        insert_case2(n);
}

void insert_case2(struct rb_node * n){
    if(n->parent->color ==BLACK)
        return;
    else
        insert_case3(n);
}

void insert_case3(struct rb_node *n){
    struct rb_node * u = uncle(n);
    struct rb_node * g ;
    if((u!=NULL)&(u->color == RED)){
        n->parent->color =BLACK;
        u->color = BLACK;
        g = grandparent(n);
        g-> color = RED;
        insert_case1(g);
    }
    else{
        insert_case4(n);
    }
}

void insert_case4(struct rb_node*n){
    struct rb_node *g = grandparent(n);
    if((n==n->parent->right)&&(n->parent==g->left)){
        rotate_left(n->parent);
        n= n->left;
    } else if ((n == n->parent->left)&&(n->parent = g->right)){
          rotate_right(n->parent);
          n=n->right;
    }
    insert_case5(n);
}



void insert_case5(struct rb_node *n){
    struct rb_node *g = grandparent(n);
    n->parent->color = BLACK;
    g->color = RED;
    if(n==n->parent->left)
        rotate_right(g);
    else
        rotate_left(g);
}

struct rb_node *sibling(struct rb_node * n)
{
    if(n==n->parent->left)
        return n->parent->right;
    else
        return n->parent->left;
}

int is_leaf(struct rb_node*n){
    struct rb_node* LEAF;
    LEAF->left = NULL;
    LEAF->right = NULL;
    LEAF->parent = n->parent;
    LEAF->color = BLACK;
    return (n==LEAF) ? 1:0 ;
}

void replace_node(struct rb_node* n, struct rb_node* child)
{ 
    /*앞에서 n의 부모가 NULL이 되는 경우를 delete_case에 
    오지 않게 미리 처리해주면 된다.*/

    child->parent = n->parent;
    if(n->parent->left ==n) n->parent->left = child;
    else if (n->parent->right ==n) n->parent->right =child;
}

void delete_one_child(struct rb_node * n)
{
    struct rb_node * child = is_leaf(n->right) ? n->left: n->right;
    replace_node(n,child);
    if(n->color == BLACK){
        if(child->color==RED)
            child->color = BLACK;
        else
            delete_case1(child);
    }

}

void delete_case1(struct rb_node *n)
{
    if(n->parent != NULL)
        delete_case2(n);
}

void delete_case2(struct rb_node *n)//n의 형제와 n의부모 관계 파악후 rotate
{
    struct rb_node *s = sibling(n);
    if(s->color == RED){
         n->parent->color = RED;
         s->color = BLACK;
         if(n == n->parent->left)
            rotate_left(n->parent);
         else
            rotate_right(n->parent);
    }

    delete_case3(n);
}

void delete_case3(struct rb_node *n) //p,s,s의자식들이 다 검은색일떈 s를 빨간색
{
    struct rb_node *s = sibling(n);
    if((n->parent->color == BLACK)&&
     (s->color ==BLACK)&&
     (s->left->color ==BLACK)&&
     (s->right->color == BLACK)){
         s->color = RED;
         delete_case1(n->parent);
    }else
        delete_case4(n);
}

void delete_case4(struct rb_node *n)//p 빨간색,n,s,s의자식들이 검은색일때 s와 p를 바꿔준다. 
{
  struct rb_node *s = sibling(n);

  if((n->parent->color == RED)&&
      (s->color ==BLACK)&&
      (s->left->color ==BLACK)&&
      (s->right->color ==BLACK)){
          s->color = RED;
          n->parent->color =BLACK;
      }
    else
    {
        delete_case5(n);
    }
}

void delete_case5(struct rb_node *n)
{
    struct rb_node* s = sibling(n);
    if(s->color ==BLACK){
        if((n==n->parent->left)&&
           (s->right->color ==BLACK)&&
           (s->left->color == RED)){
               s->color = RED;
               s->left->color = BLACK;
               rotate_right(s);
           } else if((n == n->parent->right)&&
                (s->left->color ==BLACK)&&
                (s->right->color == RED)){
                    s->color =RED;
                    s->right->color =BLACK;
                    rotate_left(s);
                }
    }
    delete_case6(n);
}

void delete_case6(struct rb_node *n)
{
    struct rb_node *s = sibling(n);
    s->color = n->parent->color;
    n->parent->color =BLACK;

    if(n==n->parent->left){
        s->right->color =BLACK;
        rotate_left(n->parent);
    } else{
        s->left->color =BLACK;
        rotate_right(n->parent);
    }
}







    
