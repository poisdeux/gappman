#include <stdlib.h>
#include "gm_generic.h"

void gm_menu_free(gm_menu *dish)
{
  int i,j;

  if (dish != NULL)
  {
    for(i = 0; i < dish->amount_of_elements; i++)
    {
      free((dish->elts[i]).name);
      free((dish->elts[i]).exec);
      free((dish->elts[i]).module);
      free((dish->elts[i]).module_conffile);
      free((dish->elts[i]).logo);

      for (j = 0; j < (dish->elts[i]).numArguments; j++)
      {
        free((dish->elts[i]).args[j]);
      }
      free((dish->elts[i]).args);
    }
    free(dish->elts);
  }
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

