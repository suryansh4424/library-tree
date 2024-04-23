#ifndef ANYTREE__BS__INCLUDED
#define ANYTREE__BS__INCLUDED

#include <stdint.h>
#include <stddef.h>

#ifdef __GNUC__
#  define bstree_container_of(node, type, member) ({      \
    const struct bstree_node *__mptr = (node);            \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#else
#  define bstree_container_of(node, type, member)         \
    ((type *)((char *)(node) - offsetof(type, member)))
#endif   

struct bstree;

struct bstree_node {
    struct bstree *tree;
    struct bstree_node *left, *right;
    unsigned left_is_thread:1;
    unsigned right_is_thread:1;
};

typedef int (*bstree_cmp_fn_t)(const struct bstree_node *, const struct bstree_node *);

struct bstree {
    bstree_cmp_fn_t cmp_fn;
    unsigned size;

    struct bstree_node *root;
    struct bstree_node *first, *last;
};

struct bstree_node *bstree_first(const struct bstree *tree);
struct bstree_node *bstree_last(const struct bstree *tree);
struct bstree_node *bstree_next(const struct bstree_node *node);
struct bstree_node *bstree_prev(const struct bstree_node *node);

struct bstree_node *bstree_lookup(const struct bstree_node *key, const struct bstree *tree);
struct bstree_node *bstree_insert(struct bstree_node *node, struct bstree *tree);
void bstree_remove(struct bstree_node *node, struct bstree *tree);
void bstree_replace(struct bstree_node *old, struct bstree_node *node, struct bstree *tree);

#define bstree_is_empty(TREE) (TREE->size == 0)
#define bstree_size(TREE) (TREE->size)

int bstree_init(struct bstree *tree, bstree_cmp_fn_t cmp);
void bstree_clean(struct bstree *tree);

typedef void (*bstree_call_fn_t)(const struct bstree_node *);
void bstree_foreach(struct bstree *tree, bstree_call_fn_t call);
void bstree_foreach_backward(struct bstree *tree, bstree_call_fn_t call);

#endif 