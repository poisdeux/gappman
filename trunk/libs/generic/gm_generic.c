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

#define MENU_ELTS_ARRAY_INCREMENT 5

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
    gm_menu_element_free(dish->elts[i]);
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
      if (g_strcmp0(name, (const char *)(dish->elts[i]->name)) == 0) 
      {
        return dish->elts[i];
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
  gm_menu *menu;

  menu = (gm_menu *) malloc(sizeof(gm_menu));
	if ( menu == NULL )
		return NULL;

  // Initial default values
  menu->menu_width.value = 100;	/// 100% 
  menu->menu_width.type = PERCENTAGE;
  menu->menu_height.value = 100; //// 100%
  menu->menu_height.type = PERCENTAGE;
  menu->hor_alignment = 0.5;      // <! default center
  menu->vert_alignment = 1;     // <! default center
  menu->amount_of_elements = 0;
  menu->elts = (gm_menu_element **) malloc(MENU_ELTS_ARRAY_INCREMENT * sizeof(gm_menu_element*));
  menu->boxes = NULL;
  return menu;
}

gm_menu_element *gm_menu_element_create()
{
  gm_menu_element *elt;

  elt = (gm_menu_element *) g_try_malloc(sizeof(gm_menu_element));
	if ( elt == NULL )
		return NULL;

  elt->numArguments = 0;
  elt->logo = NULL;
  elt->name = NULL;
  elt->exec = NULL;
  elt->args = NULL;
  elt->module = NULL;
  elt->module_conffile = NULL;
  elt->autostart = 0;
  elt->printlabel = 0;
  elt->app_height = -1;
  elt->app_width = -1;
  elt->pid = -1;

  return elt;
}

gboolean gm_menu_add_menu_element(gm_menu_element *elt, gm_menu *menu)
{
	if( menu == NULL )
		return FALSE;

	//Check if we need to resize the menu->elts array
	if ( ( menu->amount_of_elements % 5 ) == 0 )
		menu->elts = (gm_menu_element **) g_try_realloc(menu->elts, ( menu->amount_of_elements + MENU_ELTS_ARRAY_INCREMENT ) * sizeof(gm_menu_element *));
	
	if ( ( menu->elts == NULL )	|| ( elt == NULL ) )
		return FALSE;

	menu->elts[menu->amount_of_elements++] = elt;
	
	return TRUE;
}

gboolean gm_menu_element_add_argument(gchar *arg, gm_menu_element *elt)
{
	if ( ( elt == NULL ) || ( arg == NULL ) )
		return FALSE;

	elt->numArguments++;
  elt->args =
    (gchar **)g_try_realloc(elt->args, (elt->numArguments) * sizeof(gchar *));
	if ( elt->args == NULL )
		return FALSE;

  elt->args[elt->numArguments - 1] = arg;

	return TRUE;
}
