#ifndef ANYTREE__ANY__INCLUDED
#define ANYTREE__ANY__INCLUDED

#include "avl.h"
#include "bs.h"
#include "rb.h"
#include "splay.h"


#ifdef __GNUC__
#  define anytree_container_of(node, type, member) ({      \
    const struct anytree_node *__mptr = (node);            \
    (type *)( (char *)__mptr - offsetof(type,member) );})
#else
#  define anytree_container_of(node, type, member)         \
    ((type *)((char *)(node) - offsetof(type, member)))
#endif   

struct anytree;

struct anytree_node {
    union {
        struct anytree *tree;
        struct avltree_node avl;
        struct bstree_node bs;
        struct rbtree_node rb;
        struct splaytree_node splay;
    };
};

typedef int (*anytree_cmp_fn_t)(const struct anytree_node *, const struct anytree_node *);

typedef struct anytree_node * (*anytree_first_fn_t)(const struct anytree *tree);
typedef struct anytree_node * (*anytree_last_fn_t)(const struct anytree *tree);
typedef struct anytree_node * (*anytree_next_fn_t)(const struct anytree_node *node);
typedef struct anytree_node * (*anytree_prev_fn_t)(const struct anytree_node *node);

typedef struct anytree_node * (*anytree_lookup_fn_t)(const struct anytree_node *key, const struct anytree *tree);
typedef struct anytree_node * (*anytree_insert_fn_t)(struct anytree_node *node, struct anytree *tree);
typedef void (*anytree_remove_fn_t)(struct anytree_node *node, struct anytree *tree);
typedef void (*anytree_replace_fn_t)(struct anytree_node *old, struct anytree_node *node, struct anytree *tree);

typedef void (*anytree_clean_fn_t)(const struct anytree *tree);

struct anytree_functions {
    anytree_first_fn_t first_fn;
    anytree_last_fn_t last_fn;
    anytree_next_fn_t next_fn;
    anytree_prev_fn_t prev_fn;

    anytree_lookup_fn_t lookup_fn;
    anytree_insert_fn_t insert_fn;
    anytree_remove_fn_t remove_fn;
    anytree_replace_fn_t replace_fn;

    anytree_clean_fn_t clean_fn;
};

struct anytree_common {
    anytree_cmp_fn_t cmp_fn;
    unsigned size;
};

struct anytree {
    union {
        struct anytree_common common;
        struct avltree avl;
        struct bstree bs;
        struct rbtree rb;
        struct splaytree splay;
    };

    struct anytree_functions *functions;
};

#define anytree_first(TREE) (TREE->functions->first_fn(TREE))
#define anytree_last(TREE) (TREE->functions->last_fn(TREE))
#define anytree_next(NODE) (NODE->tree->functions->next_fn(NODE))
#define anytree_prev(NODE) (NODE->tree->functions->prev_fn(NODE))

#define anytree_lookup(KEY, TREE) (TREE->functions->lookup_fn(KEY, TREE))
#define anytree_insert(NODE, TREE) (TREE->functions->insert_fn(NODE, TREE))
#define anytree_remove(NODE) (NODE->tree->functions->remove_fn(NODE, NODE->tree))
#define anytree_replace(OLD, NODE) (OLD->tree->functions->replace_fn(OLD, NODE, OLD->tree))

#define anytree_is_empty(TREE) (TREE->common.size == 0)
#define anytree_size(TREE) (TREE->common.size)

#define anytree_clean(TREE) (TREE->functions->clean_fn(TREE))

typedef void (*anytree_call_fn_t)(const struct anytree_node *);
void anytree_foreach(struct anytree *tree, anytree_call_fn_t call);
void anytree_foreach_backward(struct anytree *tree, anytree_call_fn_t call);


enum anytree_type {
    ANYTREE_AVL,
    ANYTREE_BS,
    ANYTREE_RB,
    ANYTREE_SPLAY
};

struct anytree * anytree_init(enum anytree_type type, anytree_cmp_fn_t cmp);
void anytree_release(struct anytree *tree);

#endif