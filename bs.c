#include "bs.h"


static inline void INIT_NODE(struct bstree_node *node, struct bstree *tree)
{
    node->left = NULL;
    node->right = NULL;
    node->tree = tree;
    node->left_is_thread = 0;
    node->right_is_thread = 0;
}

static inline void set_left(struct bstree_node *l, struct bstree_node *n)
{
    n->left = l;
    n->left_is_thread = 0;
}

static inline void set_right(struct bstree_node *r, struct bstree_node *n)
{
    n->right = r;
    n->right_is_thread = 0;
}

static inline void set_prev(struct bstree_node *t, struct bstree_node *n)
{
    n->left = t;
    n->left_is_thread = 1;
}

static inline void set_next(struct bstree_node *t, struct bstree_node *n)
{
    n->right = t;
    n->right_is_thread = 1;
}

static inline struct bstree_node *get_left(const struct bstree_node *n)
{
    if (n->left_is_thread)
        return NULL;
    return n->left;
}

static inline struct bstree_node *get_right(const struct bstree_node *n)
{
    if (n->right_is_thread)
        return NULL;
    return n->right;
}

static inline struct bstree_node *get_prev(const struct bstree_node *n)
{
    if (!n->left_is_thread)
        return NULL;
    return n->left;
}

static inline struct bstree_node *get_next(const struct bstree_node *n)
{
    if (!n->right_is_thread)
        return NULL;
    return n->right;
}



static inline struct bstree_node *get_first(struct bstree_node *node)
{
    struct bstree_node *left;
    while ((left = get_left(node)))
        node = left;
    return node;
}

static inline struct bstree_node *get_last(struct bstree_node *node)
{
    struct bstree_node *right;
    while ((right = get_right(node)))
        node = right;
    return node;
}

struct bstree_node *bstree_first(const struct bstree *tree)
{
    if (tree->root)
        return tree->first;
    return NULL;
}

struct bstree_node *bstree_last(const struct bstree *tree)
{
    if (tree->root)
        return tree->last;
    return NULL;
}

struct bstree_node *bstree_next(const struct bstree_node *node)
{
    struct bstree_node *right = get_right(node);
    if (right)
        return get_first(right);
    return get_next(node);
}

struct bstree_node *bstree_prev(const struct bstree_node *node)
{
    struct bstree_node *left = get_left(node);
    if (left)
        return get_last(left);
    return get_prev(node);
}


static struct bstree_node *do_lookup(const struct bstree_node *key, const struct bstree *tree, struct bstree_node **pparent, int *is_left)
{
    struct bstree_node *node = tree->root;

    *pparent = NULL;
    *is_left = 0;

    while (node) {
        int res = tree->cmp_fn(node, key);
        if (res == 0)
            return node;
        *pparent = node;
        if ((*is_left = res > 0))
            node = get_left(node);
        else
            node = get_right(node);
    }
    return NULL;
}

struct bstree_node *bstree_lookup(const struct bstree_node *key, const struct bstree *tree)
{
    struct bstree_node *parent;
    int is_left;

    return do_lookup(key, tree, &parent, &is_left);
}

struct bstree_node *bstree_insert(struct bstree_node *node, struct bstree *tree)
{
    struct bstree_node *key, *parent;
    int is_left;

    key = do_lookup(node, tree, &parent, &is_left);
    if (key)
        return key;

    ++tree->size;

    INIT_NODE(node, tree);

    if (!parent) {
        tree->root = tree->first = tree->last = node;
        return NULL;
    }
    if (is_left) {
        if (parent == tree->first)
            tree->first = node;
        set_prev(get_prev(parent), node);
        set_next(parent, node);
        set_left(node, parent);
    } else {
        if (parent == tree->last)
            tree->last = node;
        set_prev(parent, node);
        set_next(get_next(parent), node);
        set_right(node, parent);
    }
    return NULL;
}

static void set_child(struct bstree_node *child, struct bstree_node *node, int left)
{
    if (left)
        set_left(child, node);
    else
        set_right(child, node);
}

void bstree_remove(struct bstree_node *node, struct bstree *tree)
{
    struct bstree_node *left, *right, *next;
    struct bstree_node fake_parent, *parent;
    int is_left;

    if (tree && (node->tree != tree))
        return;

    --tree->size;

    do_lookup(node, tree, &parent, &is_left);

    if (!parent) {
        INIT_NODE(&fake_parent, tree);
        parent = &fake_parent;
        is_left = 0;
    }
    left  = get_left(node);
    right = get_right(node);

    if (!left && !right) {
        if (is_left)
            set_prev(get_prev(node), parent);
        else
            set_next(get_next(node), parent);
        next = parent;
        goto update_first_last;
    }
    if (!left) {
        next = get_first(right);
        set_prev(get_prev(node), next);
        set_child(right, parent, is_left);
        goto update_first_last;
    }
    if (!right) {
        next = get_last(left);
        set_next(get_next(node), next);
        set_child(left, parent, is_left);
        goto update_first_last;
    }

    next = get_first(right);
    if (next != right) {
        struct bstree_node *m = get_next(get_last(next));

        if (get_right(next))
            set_left(get_right(next), m);
        else
            set_prev(next, m);

        set_right(right, next);
    }
    set_child(next, parent, is_left);
    set_left(left, next);
    set_next(next, get_last(left));
out:
    if (parent == &fake_parent)
        tree->root = get_right(parent);
    return;

update_first_last:
    if (node == tree->first)
        tree->first = next;
    if (node == tree->last)
        tree->last = next;
    goto out;
}

void bstree_replace(struct bstree_node *old, struct bstree_node *node, struct bstree *tree)
{
    struct bstree_node *parent;
    int is_left;

    do_lookup(old, tree, &parent, &is_left);
    if (parent)
        set_child(node, parent, is_left);
    else
        tree->root = node;

    if (tree->first == old)
        tree->first = node;
    if (tree->last == old)
        tree->last = node;

    *node = *old;
}

int bstree_init(struct bstree *tree, bstree_cmp_fn_t cmp)
{
    tree->cmp_fn = cmp;
    tree->size = 0;
    tree->root = NULL;
    return 0;
}

void bstree_clean(struct bstree *tree)
{
    struct bstree_node *i;
    for (i = bstree_first(tree); i; i = bstree_next(i))
        i->tree = NULL;
    bstree_init(tree, tree->cmp_fn);
}

void bstree_foreach(struct bstree *tree, bstree_call_fn_t call)
{
    struct bstree_node * i;
    struct bstree_node * n;
    for (i = bstree_first(tree); i; )
    {
        n = bstree_next(i);
        call(i);
        i = n;
    }
}

void bstree_foreach_backward(struct bstree *tree, bstree_call_fn_t call)
{
    struct bstree_node * i;
    struct bstree_node * n;
    for (i = bstree_last(tree); i; )
    {
        n = bstree_prev(i);
        call(i);
        i = n;
    }
}
