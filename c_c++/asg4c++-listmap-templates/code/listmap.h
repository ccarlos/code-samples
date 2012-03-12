
#ifndef __LISTMAP_H__
#define __LISTMAP_H__

#include "comparex.h"
#include "pairx.h"

//class iterator;
template <typename key_t, typename value_t>
class listmap {
  // friend class iterator;
   private:
      struct node {
         pairx <key_t, value_t> pair;
         node *prev;
         node *next;
         node (const pairx <key_t, value_t> &);
      };
      node *head;
      node *tail;
   public:
      typedef pairx <key_t, value_t> mappairx;
      class iterator {
         friend class listmap;
         public:
            mappairx &operator* ();
            mappairx *operator-> ();
            iterator &operator++ ();
            iterator &operator-- ();
            bool operator== (const iterator &) const;
            bool operator!= (const iterator &) const;
            void erase ();
            iterator ();
         private:
            iterator (listmap *map, node *where);
            node *where;
            listmap *map;
      };
      listmap ();
      listmap (const listmap &);
      listmap &operator= (const listmap &);
      ~listmap ();
      void insert (const pairx <key_t, value_t> &);
      iterator find (const key_t &);
      iterator begin ();
      iterator begin_min();
      iterator null_itor();
      iterator end ();
      iterator end_minus();
      bool empty () const;
};

RCSH(__listmap_h__,
"$Id: listmap.h,v 1.9 2010-03-02 18:20:26-08 - - $")

#endif

