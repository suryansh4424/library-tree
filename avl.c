
#include <assert.h>

#include "avl.h"


static inline int is_root(struct avltree_node *node)
{
    return node->parent == NULL;
}

static inline void INIT_NODE(struct avltree_node *node, struct avltree *tree)
{
    node->left = NULL;
    node->right = NULL;
    node->tree = tree;
    node->parent = NULL;
    node->balance = 0;
}

static inline signed get_balance(struct avltree_node *node)
{
    return node->balance;
}

static inline void set_balance(int balance, struct avltree_node *node)
{
    node->balance = balance;
}

static inline int inc_balance(struct avltree_node *node)
{
    return ++node->balance;
}

static inline int dec_balance(struct avltree_node *node)
{
    return --node->balance;
}

static inline struct avltree_node *get_parent(const struct avltree_node *node)
{
    return node->parent;
}

static inline void set_parent(struct avltree_node *parent, struct avltree_node *node)
{
    node->parent = parent;
}


static inline struct avltree_node *get_first(struct avltree_node *node)
{
    while (node->left)
        node = node->left;
    return node;
}

static inline struct avltree_node *get_last(struct avltree_node *node)
{
    while (node->right)
        node = node->right;
    return node;
}

struct avltree_node *avltree_first(const struct avltree *tree)
{
    return tree->first;
}

struct avltree_node *avltree_last(const struct avltree *tree)
{
    return tree->last;
}

struct avltree_node *avltree_next(const struct avltree_node *node)
{
    struct avltree_node *parent;

    if (node->right)
        return get_first(node->right);

    while ((parent = get_parent(node)) && parent->right == node)
        node = parent;
    return parent;
}

struct avltree_node *avltree_prev(const struct avltree_node *node)
{
    struct avltree_node *parent;

    if (node->left)
        return get_last(node->left);

    while ((parent = get_parent(node)) && parent->left == node)
        node = parent;
    return parent;
}

static void rotate_left(struct avltree_node *node, struct avltree *tree)
{
    struct avltree_node *p = node;
    struct avltree_node *q = node->right; 
    struct avltree_node *parent = get_parent(p);

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

static void rotate_right(struct avltree_node *node, struct avltree *tree)
{
    struct avltree_node *p = node;
    struct avltree_node *q = node->left;
    struct avltree_node *parent = get_parent(p);

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


static inline struct avltree_node *do_lookup(const struct avltree_node *key, const struct avltree *tree, struct avltree_node **pparent, struct avltree_node **unbalanced, int *is_left)
{
    struct avltree_node *node = tree->root;
    int res = 0;

    *pparent = NULL;
    *unbalanced = node;
    *is_left = 0;

    while (node) {
        if (get_balance(node) != 0)
            *unbalanced = node;

        res = tree->cmp_fn(node, key);
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

struct avltree_node *avltree_lookup(const struct avltree_node *key,
                    const struct avltree *tree)
{
    struct avltree_node *parent, *unbalanced;
    int is_left;

    return do_lookup(key, tree, &parent, &unbalanced, &is_left);
}

static void set_child(struct avltree_node *child, struct avltree_node *node, int left)
{
    if (left)
        node->left = child;
    else
        node->right = child;
}

struct avltree_node *avltree_insert(struct avltree_node *node, struct avltree *tree)
{
    struct avltree_node *key, *parent, *unbalanced;
    int is_left;

    key = do_lookup(node, tree, &parent, &unbalanced, &is_left);
    if (key)
        return key;

    ++tree->size;

    INIT_NODE(node, tree);

    if (!parent) {
        tree->root = node;
        tree->first = tree->last = node;
        tree->height++;
        return NULL;
    }
    if (is_left) {
        if (parent == tree->first)
            tree->first = node;
    } else {
        if (parent == tree->last)
            tree->last = node;
    }
    set_parent(parent, node);
    set_child(node, parent, is_left);

    for (;;) {
        if (parent->left == node)
            dec_balance(parent);
        else
            inc_balance(parent);

        if (parent == unbalanced)
            break;
        node = parent;
        parent = get_parent(parent);
    }

    switch (get_balance(unbalanced)) {
    case  1: case -1:
        tree->height++;
       
    case 0:
        break;
    case 2: {
        struct avltree_node *right = unbalanced->right;

        if (get_balance(right) == 1) {
            set_balance(0, unbalanced);
            set_balance(0, right);
        } else {
            switch (get_balance(right->left)) {
            case 1:
                set_balance(-1, unbalanced);
                set_balance( 0, right);
                break;
            case 0:
                set_balance(0, unbalanced);
                set_balance(0, right);
                break;
            case -1:
                set_balance(0, unbalanced);
                set_balance(1, right);
                break;
            }
            set_balance(0, right->left);

            rotate_right(right, tree);
        }
        rotate_left(unbalanced, tree);
        break;
    }
    case -2: {
        struct avltree_node *left = unbalanced->left;

        if (get_balance(left) == -1) {
            set_balance(0, unbalanced);
            set_balance(0, left);
        } else {
            switch (get_balance(left->right)) {
            case 1:
                set_balance( 0, unbalanced);
                set_balance(-1, left);
                break;
            case 0:
                set_balance(0, unbalanced);
                set_balance(0, left);
                break;
            case -1:
                set_balance(1, unbalanced);
                set_balance(0, left);
                break;
            }
            set_balance(0, left->right);

            rotate_left(left, tree);
        }
        rotate_right(unbalanced, tree);
        break;
    }
    }
    return NULL;
}

void avltree_remove(struct avltree_node *node, struct avltree *tree)
{
    struct avltree_node *parent = get_parent(node);
    struct avltree_node *left = node->left;
    struct avltree_node *right = node->right;
    struct avltree_node *next;
    int is_left = is_left;

    if (tree && (node->tree != tree))
        return;

    --tree->size;

    if (node == tree->first)
        tree->first = avltree_next(node);
    if (node == tree->last)
        tree->last = avltree_prev(node);

    if (!left)
        next = right;
    else if (!right)
        next = left;
    else
        next = get_first(right);

    if (parent) {
        is_left = parent->left == node;
        set_child(next, parent, is_left);
    } else
        tree->root = next;

    if (left && right) {
        set_balance(get_balance(node), next);

        next->left = left;
        set_parent(next, left);

        if (next != right) {
            parent = get_parent(next);
            set_parent(get_parent(node), next);

            node = next->right;
            parent->left = node;
            is_left = 1;

            next->right = right;
            set_parent(next, right);
        } else {
            set_parent(parent, next);
            parent = next;
            node = parent->right;
            is_left = 0;
        }
        assert(parent != NULL);
    } else
        node = next;

    if (node)
        set_parent(parent, node);

    while (parent) {
        int balance;
        node   = parent;
        parent = get_parent(parent);

        if (is_left) {
            is_left = parent && parent->left == node;

            balance = inc_balance(node);
            if (balance == 0)        /* case 1 */
                continue;
            if (balance == 1)        /* case 2 */
                return;
            right = node->right;        /* case 3 */
            switch (get_balance(right)) {
            case 0:                /* case 3.1 */
                set_balance( 1, node);
                set_balance(-1, right);
                rotate_left(node, tree);
                return;
            case 1:                /* case 3.2 */
                set_balance(0, node);
                set_balance(0, right);
                break;
            case -1:            /* case 3.3 */
                switch (get_balance(right->left)) {
                case 1:
                    set_balance(-1, node);
                    set_balance( 0, right);
                    break;
                case 0:
                    set_balance(0, node);
                    set_balance(0, right);
                    break;
                case -1:
                    set_balance(0, node);
                    set_balance(1, right);
                    break;
                }
                set_balance(0, right->left);

                rotate_right(right, tree);
            }
            rotate_left(node, tree);
        } else {
            is_left = parent && parent->left == node;

            balance = dec_balance(node);
            if (balance == 0)
                continue;
            if (balance == -1)
                return;
            left = node->left;
            switch (get_balance(left)) {
            case 0:
                set_balance(-1, node);
                set_balance(1, left);
                rotate_right(node, tree);
                return;
            case -1:
                set_balance(0, node);
                set_balance(0, left);
                break;
            case 1:
                switch (get_balance(left->right)) {
                case 1:
                    set_balance(0, node);
                    set_balance(-1, left);
                    break;
                case 0:
                    set_balance(0, node);
                    set_balance(0, left);
                    break;
                case -1:
                    set_balance(1, node);
                    set_balance(0, left);
                    break;
                }
                set_balance(0, left->right);

                rotate_left(left, tree);
            }
            rotate_right(node, tree);
        }
    }
    tree->height--;
}

void avltree_replace(struct avltree_node *old, struct avltree_node *node, struct avltree *tree)
{
    struct avltree_node *parent = get_parent(old);

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

int avltree_init(struct avltree *tree, avltree_cmp_fn_t cmp)
{
    tree->cmp_fn = cmp;
    tree->size = 0;
    tree->root = NULL;
    tree->first = NULL;
    tree->last = NULL;
    tree->height = -1;
    return 0;
}

void avltree_clean(struct avltree *tree)
{
    struct avltree_node *i;
    for (i = avltree_first(tree); i; i = avltree_next(i))
        i->tree = NULL;
    avltree_init(tree, tree->cmp_fn);
}

void avltree_foreach(struct avltree *tree, avltree_call_fn_t call)
{
    struct avltree_node * i;
    struct avltree_node * n;
    for (i = avltree_first(tree); i; )
    {
        n = avltree_next(i);
        call(i);
        i = n;
    }
}

void avltree_foreach_backward(struct avltree *tree, avltree_call_fn_t call)
{
    struct avltree_node * i;
    struct avltree_node * n;
    for (i = avltree_last(tree); i; )
    {
        n = avltree_prev(i);
        call(i);
        i = n;
    }
}
