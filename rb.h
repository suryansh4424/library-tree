#ifndef ANYTREE__RB__INCLUDED
#define ANYTREE__RB__INCLUDED

#include <stdint.h>
#include <stddef.h>


#ifdef __GNUC__
#  define rbtree_container_of(node, type, member) ({      \
    const struct rbtree_node *__mptr = (node);            \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#else
#  define rbtree_container_of(node, type, member)         \
    ((type *)((char *)(node) - offsetof(type, member)))
#endif    /* __GNUC__ */


enum rb_color {
    RB_BLACK,
    RB_RED
};

struct rbtree;

struct rbtree_node {
    struct rbtree *tree;
    struct rbtree_node *left, *right;
    struct rbtree_node *parent;
    unsigned red_color:1;
};

typedef int (*rbtree_cmp_fn_t)(const struct rbtree_node *, const struct rbtree_node *);

struct rbtree {
    rbtree_cmp_fn_t cmp_fn;
    unsigned size;

    struct rbtree_node *root;
    struct rbtree_node *first, *last;
};

struct rbtree_node *rbtree_first(const struct rbtree *tree);
struct rbtree_node *rbtree_last(const struct rbtree *tree);
struct rbtree_node *rbtree_next(const struct rbtree_node *node);
struct rbtree_node *rbtree_prev(const struct rbtree_node *node);

struct rbtree_node *rbtree_lookup(const struct rbtree_node *key, const struct rbtree *tree);
struct rbtree_node *rbtree_insert(struct rbtree_node *node, struct rbtree *tree);
void rbtree_remove(struct rbtree_node *node, struct rbtree *tree);
void rbtree_replace(struct rbtree_node *old, struct rbtree_node *node, struct rbtree *tree);

#define rbtree_is_empty(TREE) (TREE->size == 0)
#define rbtree_size(TREE) (TREE->size)

int rbtree_init(struct rbtree *tree, rbtree_cmp_fn_t cmp);
void rbtree_clean(struct rbtree *tree);

typedef void (*rbtree_call_fn_t)(const struct rbtree_node *);
void rbtree_foreach(struct rbtree *tree, rbtree_call_fn_t call);
void rbtree_foreach_backward(struct rbtree *tree, rbtree_call_fn_t call);

#endif 
