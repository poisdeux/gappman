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

void gm_menu_free(gm_menu *menu)
{
  int i;
	gm_menu_page *page;

  if (menu == NULL)
		return;
    
  for(i = 0; i < gm_menu_get_amount_of_elements(menu); i++)
  {
    gm_menu_element_free(menu->elts[i]);
  }
  free(menu->elts);

	page = menu->pages;
	while( page != NULL )
	{
		gm_menu_page_free(page);
		menu->pages = gm_menu_page_next(menu->pages);
		page = menu->pages;
	}
}

gm_menu_element *gm_menu_search_elt_by_name(gchar * name, gm_menu *menu)
{
  int i;
  if(menu != NULL)
  {
    for(i=0; i < gm_menu_get_amount_of_elements(menu); i++)
    {
      if (g_strcmp0(name, (const char *)(menu->elts[i]->name)) == 0) 
      {
        return menu->elts[i];
      }
    }
  }
  return NULL;
}

gint gm_menu_get_amount_of_elements(gm_menu *menu)
{
  return menu->amount_of_elements;
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
  menu->elts = NULL;
  menu->pages = NULL;
  return menu;
}

gboolean gm_menu_add_menu_element(gm_menu_element *elt, gm_menu *menu)
{
	if( menu == NULL )
		return FALSE;

	//Check if we need to resize the menu->elts array
	if ( ( menu->amount_of_elements % MENU_ELTS_ARRAY_INCREMENT ) == 0 )
		menu->elts = (gm_menu_element **) g_try_realloc(menu->elts, ( menu->amount_of_elements + MENU_ELTS_ARRAY_INCREMENT ) * sizeof(gm_menu_element *));
	
	if ( ( menu->elts == NULL )	|| ( elt == NULL ) )
		return FALSE;


	menu->elts[menu->amount_of_elements++] = elt;
	
	return TRUE;
}

GmReturnCode gm_menu_add_page(gm_menu_page *page, gm_menu *menu)
{
	if( ( menu == NULL ) || ( page == NULL ) )
    return GM_FAIL;

	if( menu->pages == NULL )
	{
		menu->pages = page;	
	}
	else
	{
  	menu->pages->next = page;
		page->prev = menu->pages;
	}

	return GM_SUCCESS;
}

void gm_menu_set_width(GmLengthType length_type, gint width, gm_menu *menu)
{
	if ( width < 0 )
		return;

	menu->menu_width.value = width;
	menu->menu_width.type = length_type;	
}

void gm_menu_set_height(GmLengthType length_type, gint height, gm_menu *menu)
{
	if ( height < 0 )
		return;

	menu->menu_width.value = height;
	menu->menu_width.type = length_type;	
}

void gm_menu_set_max_elts_in_single_box(gint amount, gm_menu *menu)
{
	if( amount < 0 )
		return;

	menu->max_elts_in_single_box = amount;
}

gm_menu_element *gm_menu_element_create()
{
  gm_menu_element *elt;

  elt = (gm_menu_element *) g_try_malloc(sizeof(gm_menu_element));
	if ( elt == NULL )
		return NULL;

  elt->amount_of_args = 0;
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
  for (i = 0; i < (gm_menu_element_get_amount_of_arguments(elt)); i++)
  {
    free(elt->args[i]);
  }
  free(elt->args);
}

gboolean gm_menu_element_add_argument(gchar *arg, gm_menu_element *elt)
{
	if ( ( elt == NULL ) || ( arg == NULL ) )
		return FALSE;

  elt->args =
    (gchar **)g_try_realloc(elt->args, (elt->amount_of_args + 1) * sizeof(gchar *));
	if ( elt->args == NULL )
		return FALSE;

  elt->args[elt->amount_of_args++] = arg;

	return TRUE;
}

void gm_menu_element_set_name(gchar *name, gm_menu_element *elt)
{
	elt->name = name;
}

void gm_menu_element_printlabel(gboolean bool, gm_menu_element *elt)
{
	if( bool == TRUE )
	{
		elt->printlabel = 1;
	}
	else
	{
		elt->printlabel = 0;
	}
}

void gm_menu_element_set_widget(GtkWidget *widget, gm_menu_element *elt)
{
	elt->widget = widget;
}

gint gm_menu_element_get_amount_of_arguments(gm_menu_element *elt)
{
	return elt->amount_of_args;
}

void gm_menu_element_set_pid(gint pid, gm_menu_element *elt)
{
	elt->pid = pid;
}

void gm_menu_element_set_data(gpointer data, gm_menu_element *elt)
{
	elt->data = data;
}

gm_menu_page *gm_menu_page_create(GtkWidget *box)
{
	gm_menu_page* new_page;

  new_page = (gm_menu_page *) malloc(sizeof(gm_menu_page));
  if ( new_page == NULL )
  {
    g_warning("gm_menu_page_create: could not allocate memory for new page");
    return NULL;
  }

  new_page->box = box;
  new_page->prev = NULL;
  new_page->next = NULL;
	
	return new_page;
}

void gm_menu_page_free(gm_menu_page *page)
{
	if( page != NULL )
		free(page);
}

gm_menu_page *gm_menu_page_next(gm_menu_page *page)
{
	if ( page == NULL )
		return NULL;

	return page->next;
}
 
