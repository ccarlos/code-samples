
#include "listmap.h"

comparex <string> compare;

template <typename key_t, typename value_t>
listmap <key_t, value_t>::node::node (const mappairx &pair):
            pair(pair), prev(NULL), next(NULL) {
}

template <typename key_t, typename value_t>
listmap <key_t, value_t>::listmap (): head(NULL), tail (NULL) {
}

template <typename key_t, typename value_t>
listmap <key_t, value_t>::~listmap () {
   TRACE ('l', (void*) this);
   //iterator itor = begin ();
   //iterator iend = end ();
   //while (itor != iend) itor.erase ();
   listmap <key_t, value_t>::iterator itor = this->begin();
   listmap <key_t, value_t>::iterator end  = this->end();
   while( itor != end ) {
      itor.erase();
      TRACE('e',"Eraser()");
   }
}

template <typename key_t, typename value_t>
pairx <key_t, value_t> &
listmap <key_t, value_t>::iterator::operator* () {
   TRACE ('l', where->pair);
   return where->pair;
}

template <typename key_t, typename value_t>
pairx <key_t, value_t> *
listmap <key_t, value_t>::iterator::operator-> () {
   TRACE ('l', where->pair);
   return &(where->pair);
}

template <typename key_t, typename value_t>
typename listmap <key_t, value_t>::iterator &
listmap <key_t, value_t>::iterator::operator++ () {
   TRACE ('l', "First: " << map << ", " << where);
   TRACE ('l', "Second: " << map->head << ", " << map->tail);
   //test to see if we are pointing at phantom element
   if(where==NULL)
      throw logic_error("Cannot increment on phantom elment");
   where = where->next;
   return *this;
}

template <typename key_t, typename value_t>
typename listmap <key_t, value_t>::iterator &
listmap <key_t, value_t>::iterator::operator-- () {
   // Have to comment out since it cause a seg fault
   // Works perfect with itor's Since we only implemented
   // Operator == and !=
   // if(where==map->head)
   //   throw logic_error("Cannot decrement on head element");
   where = where->prev;
   return *this;
}

template <typename key_t, typename value_t>
bool listmap <key_t, value_t>::iterator::operator==
            (const iterator &that) const {
   return this->where == that.where;
}

template <typename key_t, typename value_t>
bool listmap <key_t, value_t>::iterator::operator!=
            (const iterator &that) const {
   return this->where != that.where;
}

template <typename key_t, typename value_t>
listmap <key_t, value_t>::iterator::iterator ():
            map (NULL), where (NULL){
}

template <typename key_t, typename value_t>
listmap <key_t, value_t>::iterator::iterator (listmap *map,
            node *where): map (map), where (where){
}

template <typename key_t, typename value_t>
typename listmap <key_t, value_t>::iterator
listmap <key_t, value_t>::find(const key_t &key) {
   listmap <key_t, value_t>::iterator itor = this->begin();
   listmap <key_t, value_t>::iterator end  = this->end();
   
   for(; itor != end; ++itor){
      if(key==itor->first) return iterator(this,itor.where); 
   }
   return iterator(NULL,NULL);
}

template <typename key_t, typename value_t>
typename listmap <key_t, value_t>::iterator
listmap <key_t, value_t>::begin_min() {
   return iterator (this,NULL);
}

template <typename key_t, typename value_t>
typename listmap <key_t, value_t>::iterator
listmap <key_t, value_t>::begin () {
   return iterator (this, head);
}

template <typename key_t, typename value_t>
typename listmap <key_t, value_t>::iterator
listmap <key_t, value_t>::null_itor () {
   return iterator (NULL, NULL);
}

template <typename key_t, typename value_t>
typename listmap <key_t, value_t>::iterator
listmap <key_t, value_t>::end () {
   return iterator (this, NULL);
}

template <typename key_t, typename value_t>
typename listmap <key_t, value_t>::iterator
listmap <key_t, value_t>::end_minus () {
   return iterator (this, tail);
}

template <typename key_t, typename value_t>
bool listmap <key_t, value_t>::empty () const {
   return head == NULL;
}

template <typename key_t, typename value_t>
void listmap <key_t, value_t>::iterator::erase () {
   if(where==NULL) return;
   //case if only one element
   if(where==map->head && where==map->tail){  
      delete where;
      map->head=NULL;
      map->tail=NULL;
      where = NULL;
   }
   //case if *node is pointing at head
   else if(where==map->head){
      node *next_tmp = where->next;
      next_tmp->prev = NULL;
      delete where;
      map->head = next_tmp;
      where = next_tmp;//NULL
   }
   //case if *node is pointing at tail
   else if(where==map->tail){
      node *prev_tmp = map->tail->prev;
      prev_tmp->next = NULL;
      delete where;
      map->tail = prev_tmp;
      where = prev_tmp;//NULL
   }
   //case if *node is in middle of list
   else{
      node *next_tmp = where->next;
      node *prev_tmp = where->prev;
      next_tmp->prev = prev_tmp;
      prev_tmp->next = next_tmp;
      delete where;
      where = prev_tmp;//NULL
   }
}

template <typename key_t, typename value_t>
void listmap<key_t, value_t>::insert
            (const pairx <key_t, value_t> &pair) {
   node *tmp = new node (pair);
   if (empty ()) {
      head = tail = tmp;
   }else {  
      //here we will add based on lexicographical order
      //we have a case for insert before HEAD and a case
      //for insert before elements between HEAD and TAIL
      for(node *curr=head; curr!=NULL; curr=curr->next) {
         int cmp = compare (curr->pair.first, pair.first);
         if(cmp==1 && curr==head){
            tmp->next = head;
            tmp->prev = NULL;
            head->prev = tmp;
            head = tmp;
            return;
         }else if(cmp==1){
            tmp->next = curr;
            node *curr_prev = curr->prev;
            curr->prev = tmp;
            tmp->prev = curr_prev;
            curr_prev->next = tmp;
            return;
         }
      }
      //item was greater than those elments in list
      //so we default add it to the back
      tail->next = tmp;
      tmp->prev = tail;
      tail = tmp;
   }
   TRACE ('l', &pair << "->" << pair);
}

#include "listmap.ccti"

//RCSC(__listmap_cc__,
//"$Id: listmap.cc,v 1.15 2010-03-04 03:03:37-08 - - $")

