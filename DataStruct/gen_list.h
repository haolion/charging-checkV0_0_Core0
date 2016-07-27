
#ifndef GEN_LIST_INCLUDED
#define GEN_LIST_INCLUDED


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
	ListsFalse = 0,
	ListsTrue
}genlists_status;


/************************************************************************/
/* LINKED LIST MECHANISM						*/
/************************************************************************/
/* This following structure is used in all doubly linked circular lists */
/* as the first component in the structure.  This allows one set of list*/
/* manipulation primitives to be used with any linked structure		*/
/* containing it.							*/

typedef struct dbl_lnk
  {
  /* Note: adding/removing fields to/from this struct will make older	*/
  /* !!!   applications incompatible with new security DLLs.		*/
  struct dbl_lnk *next;
  struct dbl_lnk *prev;
  } DBL_LNK;


/* genlists.c so it will compile.					*/

void *list_get_first (DBL_LNK **);
void *list_get_next  (DBL_LNK *, DBL_LNK *);

DBL_LNK*  list_find_prev (DBL_LNK *list_head_ptr, DBL_LNK *cur_node);
DBL_LNK*  list_find_last (DBL_LNK *list_head_ptr);

/************************************************************************/
/* Primitive functions for generic queue handling			*/

genlists_status	list_unlink         (DBL_LNK **, DBL_LNK *);
genlists_status	list_add_first      (DBL_LNK **, DBL_LNK *);




#ifdef __cplusplus
}
#endif

#endif
