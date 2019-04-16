#ifndef SKIPLIST_H
#define SKIPLIST_H

typedef struct skiplist_node {
        void *value;
        unsigned height;
        struct skiplist_node *links[];
} s_skiplist_node;

s_skiplist_node * new_skiplist_node (void *value, int height);
void           delete_skiplist_node (s_skiplist_node *n);
#define               skiplist_node_next(n, height) ((n)->links[height])
void                  skiplist_node_insert (s_skiplist_node *n,
                                            s_skiplist_node *pred);

typedef struct skiplist {
        s_skiplist_node *head;
        int (*compare) (void *value1, void *value2);
        unsigned long length;
        unsigned max_height;
        long height_table[];
} s_skiplist;

s_skiplist *  new_skiplist (int max_height, double spacing);
int               skiplist_compare_ptr (void *a, void *b);
s_skiplist_node * skiplist_pred (s_skiplist *sl, void *value);
s_skiplist_node * skiplist_insert (s_skiplist *sl, void *value);
void              skiplist_delete (s_skiplist *sl, s_skiplist_node *n);
s_skiplist_node * skiplist_find (s_skiplist *sl, void *value);
void       delete_skiplist (s_skiplist *sl);

#endif
