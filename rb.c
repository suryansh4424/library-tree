#include "rb.h"

static inline enum rb_color get_color(const struct rbtree_node *node)
{
    return node->red_color ? RB_RED : RB_BLACK;
}

static inline void set_color(enum rb_color color, struct rbtree_node *node)
{
    node->red_color = (color == RB_BLACK) ? 0 : 1;
}

static inline struct rbtree_node *get_parent(const struct rbtree_node *node)
{
    return node->parent;
}

static inline void set_parent(struct rbtree_node *parent, struct rbtree_node *node)
{
    node->parent = parent;
}


static inline int is_root(struct rbtree_node *node)
{
    return get_parent(node) == NULL;
}

static inline int is_black(struct rbtree_node *node)
{
    return get_color(node) == RB_BLACK;
}

static inline int is_red(struct rbtree_node *node)
{
    return !is_black(node);
}

static inline void INIT_NODE(struct rbtree_node *node, struct rbtree *tree)
{
    node->left = NULL;
    node->right = NULL;
    node->tree = tree;
    set_color(RB_RED, node);
}

/*
 * Iterators
 */
static inline struct rbtree_node *get_first(struct rbtree_node *node)
{
    while (node->left)
        node = node->left;
    return node;
}

static inline struct rbtree_node *get_last(struct rbtree_node *node)
{
    while (node->right)
        node = node->right;
    return node;
}

struct rbtree_node *rbtree_first(const struct rbtree *tree)
{
    return tree->first;
}

struct rbtree_node *rbtree_last(const struct rbtree *tree)
{
    return tree->last;
}

struct rbtree_node *rbtree_next(const struct rbtree_node *node)
{
    struct rbtree_node *parent;

    if (node->right)
        return get_first(node->right);

    while ((parent = get_parent(node)) && parent->right == node)
        node = parent;
    return parent;
}

struct rbtree_node *rbtree_prev(const struct rbtree_node *node)
{
    struct rbtree_node *parent;

    if (node->left)
        return get_last(node->left);

    while ((parent = get_parent(node)) && parent->left == node)
        node = parent;
    return parent;
}

static inline struct rbtree_node *do_lookup(const struct rbtree_node *key, const struct rbtree *tree, struct rbtree_node **pparent, int *is_left)
{
    struct rbtree_node *node = tree->root;

    *pparent = NULL;
    *is_left = 0;

    while (node) {
        int res = tree->cmp_fn(node, key);
        if (res == 0)
            return node;
        *pparent = node;
        if ((*is_left = res > 0))
            node = node->left;
        else
            node = node->right;
    }
    return NULL;
}


static void rotate_left(struct rbtree_node *node, struct rbtree *tree)
{
    struct rbtree_node *p = node;
    struct rbtree_node *q = node->right; /* can't be NULL */
    struct rbtree_node *parent = get_parent(p);

    if (!is_root(p)) {
        if (parent->left == p)
            parent->left = q;
        else
            parent->right = q;
    } else
        tree->root = q;
    set_parent(parent, q);
    set_parent(q, p);

    p->right = q->left;
    if (p->right)
        set_parent(p, p->right);
    q->left = p;
}

static void rotate_right(struct rbtree_node *node, struct rbtree *tree)
{
    struct rbtree_node *p = node;
    struct rbtree_node *q = node->left; /* can't be NULL */
    struct rbtree_node *parent = get_parent(p);

    if (!is_root(p)) {
        if (parent->left == p)
            parent->left = q;
        else
            parent->right = q;
    } else
        tree->root = q;
    set_parent(parent, q);
    set_parent(q, p);

    p->left = q->right;
    if (p->left)
        set_parent(p, p->left);
    q->right = p;
}

struct rbtree_node *rbtree_lookup(const struct rbtree_node *key, const struct rbtree *tree)
{
    struct rbtree_node *parent;
    int is_left;

    return do_lookup(key, tree, &parent, &is_left);
}

static void set_child(struct rbtree_node *child, struct rbtree_node *node, int left)
{
    if (left)
        node->left = child;
    else
        node->right = child;
}

struct rbtree_node *rbtree_insert(struct rbtree_node *node, struct rbtree *tree)
{
    struct rbtree_node *key, *parent;
    int is_left;

    key = do_lookup(node, tree, &parent, &is_left);
    if (key)
        return key;

    ++tree->size;

    INIT_NODE(node, tree);

    set_parent(parent, node);

    if (parent) {
        if (is_left) {
            if (parent == tree->first)
                tree->first = node;
        } else {
            if (parent == tree->last)
                tree->last = node;
        }
        set_child(node, parent, is_left);
    } else {
        tree->root = node;
        tree->first = node;
        tree->last = node;
    }

    while ((parent = get_parent(node)) && is_red(parent)) {
        struct rbtree_node *grandpa = get_parent(parent);

        if (parent == grandpa->left) {
            struct rbtree_node *uncle = grandpa->right;

            if (uncle && is_red(uncle)) {
                set_color(RB_BLACK, parent);
                set_color(RB_BLACK, uncle);
                set_color(RB_RED, grandpa);
                node = grandpa;
            } else {
                if (node == parent->right) {
                    rotate_left(parent, tree);
                    node = parent;
                    parent = get_parent(node);
                }
                set_color(RB_BLACK, parent);
                set_color(RB_RED, grandpa);
                rotate_right(grandpa, tree);
            }
        } else {
            struct rbtree_node *uncle = grandpa->left;

            if (uncle && is_red(uncle)) {
                set_color(RB_BLACK, parent);
                set_color(RB_BLACK, uncle);
                set_color(RB_RED, grandpa);
                node = grandpa;
            } else {
                if (node == parent->left) {
                    rotate_right(parent, tree);
                    node = parent;
                    parent = get_parent(node);
                }
                set_color(RB_BLACK, parent);
                set_color(RB_RED, grandpa);
                rotate_left(grandpa, tree);
            }
        }
    }
    set_color(RB_BLACK, tree->root);
    return NULL;
}

void rbtree_remove(struct rbtree_node *node, struct rbtree *tree)
{
    struct rbtree_node *parent = get_parent(node);
    struct rbtree_node *left = node->left;
    struct rbtree_node *right = node->right;
    struct rbtree_node *next;
    enum rb_color color;

    if (tree && (node->tree != tree))
        return;

    --tree->size;

    if (node == tree->first)
        tree->first = rbtree_next(node);
    if (node == tree->last)
        tree->last = rbtree_prev(node);

    if (!left)
        next = right;
    else if (!right)
        next = left;
    else
        next = get_first(right);

    if (parent)
        set_child(next, parent, parent->left == node);
    else
        tree->root = next;

    if (left && right) {
        color = get_color(next);
        set_color(get_color(node), next);

        next->left = left;
        set_parent(next, left);

        if (next != right) {
            parent = get_parent(next);
            set_parent(get_parent(node), next);

            node = next->right;
            parent->left = node;

            next->right = right;
            set_parent(next, right);
        } else {
            set_parent(parent, next);
            parent = next;
            node = next->right;
        }
    } else {
        color = get_color(node);
        node = next;
    }
    
    if (node)
        set_parent(parent, node);

    
    if (color == RB_RED)
        return;
    if (node && is_red(node)) {
        set_color(RB_BLACK, node);
        return;
    }

    do {
        if (node == tree->root)
            break;

        if (node == parent->left) {
            struct rbtree_node *sibling = parent->right;

            if (is_red(sibling)) {
                set_color(RB_BLACK, sibling);
                set_color(RB_RED, parent);
                rotate_left(parent, tree);
                sibling = parent->right;
            }
            if ((!sibling->left  || is_black(sibling->left)) &&
                (!sibling->right || is_black(sibling->right))) {
                set_color(RB_RED, sibling);
                node = parent;
                parent = get_parent(parent);
                continue;
            }
            if (!sibling->right || is_black(sibling->right)) {
                set_color(RB_BLACK, sibling->left);
                set_color(RB_RED, sibling);
                rotate_right(sibling, tree);
                sibling = parent->right;
            }
            set_color(get_color(parent), sibling);
            set_color(RB_BLACK, parent);
            set_color(RB_BLACK, sibling->right);
            rotate_left(parent, tree);
            node = tree->root;
            break;
        } else {
            struct rbtree_node *sibling = parent->left;

            if (is_red(sibling)) {
                set_color(RB_BLACK, sibling);
                set_color(RB_RED, parent);
                rotate_right(parent, tree);
                sibling = parent->left;
            }
            if ((!sibling->left  || is_black(sibling->left)) &&
                (!sibling->right || is_black(sibling->right))) {
                set_color(RB_RED, sibling);
                node = parent;
                parent = get_parent(parent);
                continue;
            }
            if (!sibling->left || is_black(sibling->left)) {
                set_color(RB_BLACK, sibling->right);
                set_color(RB_RED, sibling);
                rotate_left(sibling, tree);
                sibling = parent->left;
            }
            set_color(get_color(parent), sibling);
            set_color(RB_BLACK, parent);
            set_color(RB_BLACK, sibling->left);
            rotate_right(parent, tree);
            node = tree->root;
            break;
        }
    } while (is_black(node));

    if (node)
        set_color(RB_BLACK, node);
}

void rbtree_replace(struct rbtree_node *old, struct rbtree_node *node, struct rbtree *tree)
{
    struct rbtree_node *parent = get_parent(old);

    if (parent)
        set_child(node, parent, parent->left == old);
    else
        tree->root = node;

    if (old->left)
        set_parent(node, old->left);
    if (old->right)
        set_parent(node, old->right);

    if (tree->first == old)
        tree->first = node;
    if (tree->last == old)
        tree->last = node;

    *node = *old;
}

int rbtree_init(struct rbtree *tree, rbtree_cmp_fn_t fn)
{
    tree->cmp_fn = fn;
    tree->size = 0;
    tree->root = NULL;
    tree->first = NULL;
    tree->last = NULL;
    return 0;
}

void rbtree_clean(struct rbtree *tree)
{
    struct rbtree_node *i;
    for (i = rbtree_first(tree); i; i = rbtree_next(i))
        i->tree = NULL;
    rbtree_init(tree, tree->cmp_fn);
}

void rbtree_foreach(struct rbtree *tree, rbtree_call_fn_t call)
{
    struct rbtree_node * i;
    struct rbtree_node * n;
    for (i = rbtree_first(tree); i; )
    {
        n = rbtree_next(i);
        call(i);
        i = n;
    }
}

void rbtree_foreach_backward(struct rbtree *tree, rbtree_call_fn_t call)
{
    struct rbtree_node * i;
    struct rbtree_node * n;
    for (i = rbtree_last(tree); i; )
    {
        n = rbtree_prev(i);
        call(i);
        i = n;
    }
}
