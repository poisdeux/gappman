/**
 * \file gm_generic.c
 * \brief generic stuff that is generally used by all gappman code
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#include <stdlib.h>
#include "gm_generic.h"

void gm_menu_element_free(gm_menu_element *elt)
{
	int i;

	if (elt == NULL)
		return;
	
	free(elt->name);
  free(elt->exec);
  free(elt->module);
  free(elt->module_conffile);
  free(elt->logo);
  for (i = 0; i < elt->numArguments; i++)
  {
    free(elt->args[i]);
  }
  free(elt->args);
}

void gm_menu_free(gm_menu *dish)
{
  int i;

  if (dish == NULL)
		return;
    
  for(i = 0; i < dish->amount_of_elements; i++)
  {
    gm_menu_element_free(&(dish->elts[i]));
  }
  free(dish->elts);
}

gm_menu_element *gm_menu_search_elt_by_name(gchar * name, gm_menu *dish)
{
  int i;
  if(dish != NULL)
  {
    for(i=0;i<dish->amount_of_elements;i++)
    {
      if (g_strcmp0(name, (const char *)(dish->elts[i]).name) == 0)
      {
        return &(dish->elts[i]);
      }
    }
  }
  return NULL;
}

gint gm_menu_get_amount_of_elements(gm_menu *dish)
{
  return dish->amount_of_elements;
}

gchar *gm_menu_element_get_name(gm_menu_element *elt)
{
  return elt->name;
}

gm_menu *gm_menu_create()
{
	return NULL;
}

gboolean gm_menu_add_menu_elt(gm_menu_element *elt, gm_menu *dish)
{
	return FALSE;
}

gm_menu_element *gm_menu_element_create()
{
	return NULL;
}

