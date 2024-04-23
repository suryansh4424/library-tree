#ifndef ANYTREE__AVL__INCLUDED
#define ANYTREE__AVL__INCLUDED

#include <stdint.h>
#include <stddef.h>


#ifdef __GNUC__
#  define avltree_container_of(node, type, member) ({      \
    const struct avltree_node *__mptr = (node);            \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#else
#  define avltree_container_of(node, type, member)         \
    ((type *)((char *)(node) - offsetof(type, member)))
#endif 

struct avltree;

struct avltree_node {
    struct avltree *tree;
    struct avltree_node *left, *right;
    struct avltree_node *parent;
    signed balance:3;      
};

typedef int (*avltree_cmp_fn_t)(const struct avltree_node *, const struct avltree_node *);

struct avltree {
    avltree_cmp_fn_t cmp_fn;
    unsigned size;

    struct avltree_node *root;
    struct avltree_node *first, *last;

    int height;
};

struct avltree_node *avltree_first(const struct avltree *tree);
struct avltree_node *avltree_last(const struct avltree *tree);
struct avltree_node *avltree_next(const struct avltree_node *node);
struct avltree_node *avltree_prev(const struct avltree_node *node);

struct avltree_node *avltree_lookup(const struct avltree_node *key, const struct avltree *tree);
struct avltree_node *avltree_insert(struct avltree_node *node, struct avltree *tree);
void avltree_remove(struct avltree_node *node, struct avltree *tree);
void avltree_replace(struct avltree_node *old, struct avltree_node *node, struct avltree *tree);

#define avltree_is_empty(TREE) (TREE->size == 0)
#define avltree_size(TREE) (TREE->size)

int avltree_init(struct avltree *tree, avltree_cmp_fn_t cmp);
void avltree_clean(struct avltree *tree);

typedef void (*avltree_call_fn_t)(const struct avltree_node *);
void avltree_foreach(struct avltree *tree, avltree_call_fn_t call);
void avltree_foreach_backward(struct avltree *tree, avltree_call_fn_t call);

#endif 
