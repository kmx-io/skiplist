
#include <assert.h>
#include <stdlib.h>
#include <strings.h>
#include "skiplist.h"

s_skiplist_node * new_skiplist_node (void *value, int height)
{
        s_skiplist_node *n = malloc(sizeof(s_skiplist_node) +
                                    height * sizeof(void*));
        if (n) {
                n->value = value;
                n->height = height;
                bzero(n->links, height * sizeof(void*));
        }
        return n;
}

void delete_skiplist_node (s_skiplist_node *n)
{
        free(n);
}

/*
  Random height
  -------------

  ∀ U ∈ ℕ : 1 < U
  ∀ n ∈ ℕ*
  ∀ r ∈ ℕ : r ≤ n
  ∀ random : ℕ* ⟶ ℕ
             random(n) ∈ [0..n-1]
             ∀ i ∈ [0..n-1], P(random(n) = i) = n⁻¹               (i)
  Qᵣ := random(Uⁿ) < Uⁿ⁻ʳ

  (i) ⇒        P(random(n) < v) = ∑ᵢ₌₀ᵛ⁻¹ P(random(n) = i)
      ⇒        P(random(n) < v) = v . n⁻¹                        (ii)

      ⇒    P(random(Uⁿ) < Uⁿ⁻ʳ) = Uⁿ⁻ʳ . (Uⁿ)⁻¹
      ⇔                   P(Qᵣ) = U⁻ʳ                           (iii)

  P(Qₙ) = P(random(Uⁿ) < U⁰)
        = P(random(Uⁿ) < 1)
        = P(random(Uⁿ) = 0)
        = U⁻ⁿ

  R := maxᵣ(Qᵣ)
     = maxᵣ(random(Uⁿ) < Uⁿ⁻ʳ)
     = maxᵣ(random(Uⁿ) + 1 ≤ Uⁿ⁻ʳ)
     = maxᵣ(logᵤ(random(Uⁿ) + 1) ≤ n - r)
     = maxᵣ(⌈logᵤ(random(Uⁿ) + 1)⌉ ≤ n - r)
     = maxᵣ(r ≤ n - ⌈logᵤ(random(Uⁿ) + 1)⌉)
     = n - ⌈logᵤ(random(Uⁿ) + 1)⌉                                (iv)

                       0 ≤ random(Uⁿ) < Uⁿ
   ⇔                   1 ≤ random(Uⁿ)+1 ≤ Uⁿ
   ⇔        logᵤ(1) ≤ logᵤ(random(Uⁿ)+1) ≤ logᵤ(Uⁿ)
   ⇔             0 ≤ ⌈logᵤ(random(Uⁿ)+1)⌉ ≤ n
   ⇔           -n ≤ -⌈logᵤ(random(Uⁿ)+1)⌉ ≤ 0
   ⇔         0 ≤ n - ⌈logᵤ(random(Uⁿ)+1)⌉ ≤ n
   ⇔                      0 ≤ R ≤ n                               (v)
*/

static void skiplist_height_table (s_skiplist *sl, double spacing)
{
        unsigned h;
        double w = spacing;
        double end = 1;
        for (h = 0; h < sl->max_height; h++) {
                w *= spacing;
                end += w;
                sl->height_table[h] = end;
        }
}

s_skiplist * new_skiplist (int max_height, double spacing)
{
        s_skiplist *sl = malloc(sizeof(s_skiplist) +
                                max_height * sizeof(long));
        if (sl) {
                sl->head = new_skiplist_node(NULL, max_height);
                sl->compare = skiplist_compare_ptr;
                sl->length = 0;
                sl->max_height = max_height;
                skiplist_height_table(sl, spacing);
        }
        return sl;
}

int skiplist_compare_ptr (void *a, void *b)
{
        if (a < b)
                return -1;
        if (a > b)
                return 1;
        return 0;
}

s_skiplist_node * skiplist_pred (s_skiplist *sl, void *value)
{
        int level = sl->max_height;
        s_skiplist_node *pred = new_skiplist_node(NULL,
                                                  sl->max_height);
        s_skiplist_node *node = sl->head;
        assert(pred);
        while (level--) {
                s_skiplist_node *n = node;
                while (n && sl->compare(n->value, value) < 0) {
                        node = n;
                        n = skiplist_node_next(n, level);
                }
                skiplist_node_next(pred, level) = node;
        }
        return pred;
}

void skiplist_node_insert (s_skiplist_node *n, s_skiplist_node *pred)
{
        unsigned level;
        for (level = 0; level < n->height; level++) {
                s_skiplist_node *p = skiplist_node_next(pred, level);
                skiplist_node_next(n, level) =
                        skiplist_node_next(p, level);
                skiplist_node_next(p, level) = n;
        }
}

unsigned skiplist_random_height (s_skiplist *sl)
{
        long max = sl->height_table[sl->max_height - 1];
        long k = random() % max;
        int i;
        for (i = 0; k < sl->height_table[i]; i++)
                ;
        return sl->max_height - i;
}

s_skiplist_node * skiplist_insert (s_skiplist *sl, void *value)
{
        s_skiplist_node *pred = skiplist_pred(sl, value);
        s_skiplist_node *next = skiplist_node_next(pred, 0);
        unsigned height;
        s_skiplist_node *n;
        if (next && sl->compare(value, next->value) == 0)
                return next;
        height = skiplist_random_height(sl);
        n = new_skiplist_node(value, height);
        skiplist_node_insert(n, pred);
        sl->length++;
        return n;
}

void * skiplist_delete (s_skiplist *sl, void *x)
{
        unsigned level;
        s_skiplist_node *pred = skiplist_pred(sl, x);
        s_skiplist_node *n = skiplist_node_next(pred, 0);
        void *value;
        if (!n || sl->compare(n->value, x) != 0)
                return NULL;
        for (level = 0; level < n->height; level++) {
                s_skiplist_node *p = skiplist_node_next(pred, level);
                skiplist_node_next(p, level) = skiplist_node_next(n, level);
        }
        value = n->value;
        delete_skiplist_node(pred);
        delete_skiplist_node(n);
        sl->length--;
        return value;
}

s_skiplist_node * skiplist_find (s_skiplist *sl, void *value)
{
        s_skiplist_node *node = sl->head;
        int level = node->height;
        while (level--) {
                s_skiplist_node *n = node;
                int c;
                while (n && (c = sl->compare(n->value, value)) < 0)
                        n = skiplist_node_next(n, level);
                if (c == 0)
                        return n;
        }
        return NULL;
}

void delete_skiplist (s_skiplist *sl)
{
        while (skiplist_node_next(sl->head, 0)) {
                s_skiplist_node *next =
                        skiplist_node_next(sl->head, 0);
                skiplist_delete(sl, next);
        }
        delete_skiplist_node(sl->head);
        free(sl);
}
