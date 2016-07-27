#include "gen_list.h"
#include "stdlib_bf.h"

/************************************************************************/
/*				list_unlink				*/
/* Primitive to unlink a node out of a circular double linked list	*/
/************************************************************************/

genlists_status list_unlink (DBL_LNK **list_head_ptr, DBL_LNK *node_ptr)
{
	/* If list checking is enabled and we get to here we have valid 	*/
	/* arguments and a valid list						*/

	if ((node_ptr -> next == *list_head_ptr) && 
		(node_ptr == *list_head_ptr))		/* only node in list?	*/
	{
		*list_head_ptr = NULL;
	}
	else 
	{					       
		if (node_ptr == *list_head_ptr)    	/* first node in a multi*/
			*list_head_ptr = node_ptr->next;		/* node list		*/
		(node_ptr->next)->prev = node_ptr->prev;	/* link cur next to prev*/
		(node_ptr->prev)->next = node_ptr->next;	/* link cur prev to next*/
	}
	return ListsTrue;
}


/************************************************************************/
/*				list_add_first				*/
/* Primitive to add a node as the first node of a circular double 	*/
/* linked list								*/
/************************************************************************/

genlists_status list_add_first (DBL_LNK **list_head_ptr, DBL_LNK *node_ptr)
{
	DBL_LNK *list_tail_ptr;

	/* If list checking is enabled and we get to here we have valid 	*/
	/* arguments 								*/

	if (*list_head_ptr == NULL) 	/* will this be the only node?	*/
	{
		node_ptr->next = node_ptr;
		node_ptr->prev = node_ptr;
	}
	else
	{
		list_tail_ptr = (*list_head_ptr)->prev;
		node_ptr->next = *list_head_ptr;
		node_ptr->prev = list_tail_ptr;
		list_tail_ptr->next = node_ptr;
		(*list_head_ptr)->prev = node_ptr;
	}
	*list_head_ptr = node_ptr;		/* assign the new head of list	*/
	return ListsTrue;

}

/************************************************************************/
/*				list_get_first				*/
/* Primitive to unlink the first node out of the list and return it's	*/
/* address								*/
/************************************************************************/

void *list_get_first (DBL_LNK **list_head_ptr)
{
	DBL_LNK *node_ptr;

	if (*list_head_ptr == NULL)
		return (NULL);

	node_ptr = *list_head_ptr;
	list_unlink (list_head_ptr, node_ptr);
	return (node_ptr);

}

/************************************************************************/
/*		           list_get_next				*/
/* Primitive to get the node pointed to by the next component of the	*/
/* current node.  This function returns NULL if the list_head_ptr is	*/
/* NULL, the address of the current node is NULL or the next node in the*/
/* list is the list_head_ptr(the list has wrapped around).  This 	*/
/* function is useful for traversing a doubly linked circular list	*/
/* from begining to the end as if the list were NULL terminated.	*/
/************************************************************************/

void *list_get_next (DBL_LNK *list_head_ptr, DBL_LNK *cur_node)
{
	DBL_LNK *next_node = NULL;

	if (list_head_ptr == NULL || cur_node == NULL )
	{
		return NULL;
	}

	else if (cur_node->next != list_head_ptr)
	{
		next_node = cur_node->next;
	}
	return (next_node);
}

/************************************************************************/
/*		           list_find_prev				*/
/* Primitive to find the node "before" the current node in the linked	*/
/* list. This function returns NULL if the list_head_ptr is		*/
/* NULL, the current node is NULL, or the current node			*/
/* is the head of the linked list (i.e. does not wrap around).		*/
/************************************************************************/

DBL_LNK* list_find_prev (DBL_LNK *list_head_ptr, DBL_LNK *cur_node)
{
	DBL_LNK *prev_node = NULL;

	if (list_head_ptr == NULL || cur_node == NULL )
	{
		return NULL;
	}
	
	else if (cur_node != list_head_ptr)
	{
		prev_node = cur_node->prev;
	}
	return (prev_node);
}

/************************************************************************/
/*				list_find_last				*/
/* Primitive to find the last node in the linked list and return it's	*/
/* address.								*/
/************************************************************************/

DBL_LNK* list_find_last (DBL_LNK *list_head_ptr)
{
	DBL_LNK *last_node;

	if (list_head_ptr)
		last_node = list_head_ptr->prev;
	else
	{	/* head is NULL (i.e. list is empty) so last is NULL too.	*/
		last_node = NULL;
	}
	return (last_node);
}

