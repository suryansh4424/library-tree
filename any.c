#include <malloc.h>

#include "any.h"
#include "avl.h"
#include "bs.h"
#include "rb.h"
#include "splay.h"


struct anytree_functions * get_avltree_functions(void)
{
    static struct anytree_functions avltree_functions;
    static int inited = 0;
    if (!inited)
    {
        inited = 1;
        avltree_functions.first_fn = (anytree_first_fn_t)avltree_first;
        avltree_functions.last_fn  = (anytree_last_fn_t)avltree_last;
        avltree_functions.next_fn  = (anytree_next_fn_t)avltree_next;
        avltree_functions.prev_fn  = (anytree_prev_fn_t)avltree_prev;
        avltree_functions.lookup_fn  = (anytree_lookup_fn_t)avltree_lookup;
        avltree_functions.insert_fn  = (anytree_insert_fn_t)avltree_insert;
        avltree_functions.remove_fn  = (anytree_remove_fn_t)avltree_remove;
        avltree_functions.replace_fn = (anytree_replace_fn_t)avltree_replace;
        avltree_functions.clean_fn = (anytree_clean_fn_t)avltree_clean;
    }
    return &avltree_functions;
}

struct anytree_functions * get_bstree_functions(void)
{
    static struct anytree_functions bstree_functions;
    static int inited = 0;
    if (!inited)
    {
        inited = 1;
        bstree_functions.first_fn = (anytree_first_fn_t)bstree_first;
        bstree_functions.last_fn  = (anytree_last_fn_t)bstree_last;
        bstree_functions.next_fn  = (anytree_next_fn_t)bstree_next;
        bstree_functions.prev_fn  = (anytree_prev_fn_t)bstree_prev;
        bstree_functions.lookup_fn  = (anytree_lookup_fn_t)bstree_lookup;
        bstree_functions.insert_fn  = (anytree_insert_fn_t)bstree_insert;
        bstree_functions.remove_fn  = (anytree_remove_fn_t)bstree_remove;
        bstree_functions.replace_fn = (anytree_replace_fn_t)bstree_replace;
        bstree_functions.clean_fn = (anytree_clean_fn_t)bstree_clean;
    }
    return &bstree_functions;
}

struct anytree_functions * get_rbtree_functions(void)
{
    static struct anytree_functions rbtree_functions;
    static int inited = 0;
    if (!inited)
    {
        inited = 1;
        rbtree_functions.first_fn = (anytree_first_fn_t)rbtree_first;
        rbtree_functions.last_fn  = (anytree_last_fn_t)rbtree_last;
        rbtree_functions.next_fn  = (anytree_next_fn_t)rbtree_next;
        rbtree_functions.prev_fn  = (anytree_prev_fn_t)rbtree_prev;
        rbtree_functions.lookup_fn  = (anytree_lookup_fn_t)rbtree_lookup;
        rbtree_functions.insert_fn  = (anytree_insert_fn_t)rbtree_insert;
        rbtree_functions.remove_fn  = (anytree_remove_fn_t)rbtree_remove;
        rbtree_functions.replace_fn = (anytree_replace_fn_t)rbtree_replace;
        rbtree_functions.clean_fn = (anytree_clean_fn_t)rbtree_clean;
    }
    return &rbtree_functions;
}

struct anytree_functions * get_splaytree_functions(void)
{
    static struct anytree_functions splaytree_functions;
    static int inited = 0;
    if (!inited)
    {
        inited = 1;
        splaytree_functions.first_fn = (anytree_first_fn_t)splaytree_first;
        splaytree_functions.last_fn  = (anytree_last_fn_t)splaytree_last;
        splaytree_functions.next_fn  = (anytree_next_fn_t)splaytree_next;
        splaytree_functions.prev_fn  = (anytree_prev_fn_t)splaytree_prev;
        splaytree_functions.lookup_fn  = (anytree_lookup_fn_t)splaytree_lookup;
        splaytree_functions.insert_fn  = (anytree_insert_fn_t)splaytree_insert;
        splaytree_functions.remove_fn  = (anytree_remove_fn_t)splaytree_remove;
        splaytree_functions.replace_fn = (anytree_replace_fn_t)splaytree_replace;
        splaytree_functions.clean_fn = (anytree_clean_fn_t)splaytree_clean;
    }
    return &splaytree_functions;
}

struct anytree * anytree_init(enum anytree_type type, anytree_cmp_fn_t cmp)
{
    struct anytree *tree = NULL;
    switch (type)
    {
    case ANYTREE_AVL:
        tree = (struct anytree *)malloc(sizeof(struct anytree));
        tree->functions = get_avltree_functions();
        if (avltree_init((struct avltree*)tree, (avltree_cmp_fn_t)cmp))
        {
            free((void*)tree);
            tree = NULL;
        }
        break;

    case ANYTREE_BS:
        tree = (struct anytree *)malloc(sizeof(struct anytree));
        tree->functions = get_bstree_functions();
        if (bstree_init((struct bstree*)tree, (bstree_cmp_fn_t)cmp))
        {
            free((void*)tree);
            tree = NULL;
        }
        break;

    case ANYTREE_RB:
        tree = (struct anytree *)malloc(sizeof(struct anytree));
        tree->functions = get_rbtree_functions();
        if (rbtree_init((struct rbtree*)tree, (rbtree_cmp_fn_t)cmp))
        {
            free((void*)tree);
            tree = NULL;
        }
        break;

    case ANYTREE_SPLAY:
        tree = (struct anytree *)malloc(sizeof(struct anytree));
        tree->functions = get_splaytree_functions();
        if (splaytree_init((struct splaytree*)tree, (splaytree_cmp_fn_t)cmp))
        {
            free((void*)tree);
            tree = NULL;
        }
        break;

    }
    return tree;
}

void anytree_release(struct anytree *tree)
{
    free((void*)tree);
}

void anytree_foreach(struct anytree *tree, anytree_call_fn_t call)
{
    struct anytree_node * i;
    struct anytree_node * n;
    for (i = anytree_first(tree); i; )
    {
        n = anytree_next(i);
        call(i);
        i = n;
    }
}

void anytree_foreach_backward(struct anytree *tree, anytree_call_fn_t call)
{
    struct anytree_node * i;
    struct anytree_node * n;
    for (i = anytree_last(tree); i; )
    {
        n = anytree_prev(i);
        call(i);
        i = n;
    }
}
