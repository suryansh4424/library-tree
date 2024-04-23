#ifndef ANYTREE__SPLAY__INCLUDED
#define ANYTREE__SPLAY__INCLUDED

#include <stdint.h>
#include <stddef.h>


#ifdef __GNUC__
#  define splaytree_container_of(node, type, member) ({      \
    const struct splaytree_node *__mptr = (node);            \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#else
#  define splaytree_container_of(node, type, member)         \
    ((type *)((char *)(node) - offsetof(type, member)))
#endif    


struct splaytree;

struct splaytree_node {
    struct splaytree *tree;
    struct splaytree_node *left, *right;
    unsigned left_is_thread:1;
    unsigned right_is_thread:1;
};

typedef int (*splaytree_cmp_fn_t)(const struct splaytree_node *, const struct splaytree_node *);

struct splaytree {
    splaytree_cmp_fn_t cmp_fn;
    unsigned size;

    struct splaytree_node *root;
    struct splaytree_node *first, *last;
};

struct splaytree_node *splaytree_first(const struct splaytree *tree);
struct splaytree_node *splaytree_last(const struct splaytree *tree);
struct splaytree_node *splaytree_next(const struct splaytree_node *node);
struct splaytree_node *splaytree_prev(const struct splaytree_node *node);

struct splaytree_node *splaytree_lookup(const struct splaytree_node *key, struct splaytree *tree);
struct splaytree_node *splaytree_insert( struct splaytree_node *node, struct splaytree *tree);
void splaytree_remove(struct splaytree_node *node, struct splaytree *tree);
void splaytree_replace(struct splaytree_node *old, struct splaytree_node *node, struct splaytree *tree);

#define splaytree_is_empty(TREE) (TREE->size == 0)
#define splaytree_size(TREE) (TREE->size)

int splaytree_init(struct splaytree *tree, splaytree_cmp_fn_t cmp);
void splaytree_clean(struct splaytree *tree);

typedef void (*splaytree_call_fn_t)(const struct splaytree_node *);
void splaytree_foreach(struct splaytree *tree, splaytree_call_fn_t call);
void splaytree_foreach_backward(struct splaytree *tree, splaytree_call_fn_t call);

#endif 
