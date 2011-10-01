#include <appmanager_panel.h>
#include <gm_layout.h>

void appmanager_start_panel(gm_menu *panel)
{ 
  GThread *thread;
  int i;
    
  for(i = 0; i < panel->amount_of_elements; i++)
  {           
    if (panel->elts[i]->gm_module_start != NULL)
    {
      thread =
        g_thread_create((GThreadFunc) panel->elts[i]->gm_module_start, NULL,
                TRUE, NULL);
      if (!thread)
      {
        g_warning("Failed to create thread");
      }
    }
  } 
} 

void appmanager_stop_panel(gm_menu *panel) 
{ 
  int i; 
  for(i = 0; i < panel->amount_of_elements; i++) 
  { 
    if (panel->elts[i]->gm_module_stop != NULL) 
    { 
      panel->elts[i]->gm_module_stop(); 
    } 
  } 
} 

/**
* \todo move createpanelelement from gm_layout to appmanager.
* \brief Create a single button
* \param *elt pointer to menu_element struct that contains the logo image filename.
* \param width button width
* \param height button height
*/
static GmReturnCode setup_panel_element(gm_menu_element *menu_elt)
{
  GModule *module;

  module = g_module_open((const gchar *)menu_elt->module, G_MODULE_BIND_LAZY);

  if (!module)
  {
    g_warning("Could not load module %s\n%s", menu_elt->module,
          g_module_error());
    return GM_FAIL;
  }
  else
  {
    if (!g_module_symbol
      (module, "gm_module_start", (gpointer *) & (menu_elt->gm_module_start)))
    {
      menu_elt->gm_module_start = NULL;
      g_warning("Could not get function gm_module_start from %s\n%s",
            menu_elt->module, g_module_error());
    }

    if (!g_module_symbol
      (module, "gm_module_stop", (gpointer *) & (menu_elt->gm_module_stop)))
    {
      menu_elt->gm_module_stop = NULL;
      g_warning("Could not get function gm_module_stop from %s\n%s",
            menu_elt->module, g_module_error());
    }

    if (!g_module_symbol
      (module, "gm_module_init", (gpointer *) & (menu_elt->gm_module_init)))
    {
      menu_elt->gm_module_init = NULL;
      g_warning("Could not get function gm_module_init from %s\n%s",
            menu_elt->module, g_module_error());
    }
    if (!g_module_symbol
      (module, "gm_module_get_widget",
       (gpointer *) & (menu_elt->gm_module_get_widget)))
    {
      menu_elt->gm_module_get_widget = NULL;
      g_warning
        ("Could not get function gm_module_get_widget from %s\n%s",
         menu_elt->module, g_module_error());
    }
    if (menu_elt->module_conffile != NULL)
    {
      if (!g_module_symbol
        (module, "gm_module_set_conffile",
         (gpointer *) & (menu_elt->gm_module_set_conffile)))
      {
        menu_elt->gm_module_set_conffile = NULL;
        g_warning
          ("Could not get function gm_module_set_conffile from %s\n%s",
           menu_elt->module, g_module_error());
      }
      else
      {
        menu_elt->
          gm_module_set_conffile((const gchar *)
                       menu_elt->module_conffile);
      }
    }
    if (!g_module_symbol
      (module, "gm_module_set_icon_size",
       (gpointer *) & (menu_elt->gm_module_set_icon_size)))
    {
      menu_elt->gm_module_set_icon_size = NULL;
      g_warning
        ("Could not get function gm_module_set_icon_size from %s\n%s",
         menu_elt->module, g_module_error());
    }

  }

  return GM_SUCCESS;
}


GtkWidget *appmanager_panel_create(gm_menu *panel)
{
	int i;
	GtkWidget *widget;
	GtkWidget *buttonbox;
	gm_menu_element *menu_elt;

	gm_layout_calculate_sizes(panel);
	for( i = 0; i < panel->amount_of_elements; i++ )
	{
		menu_elt = gm_menu_get_menu_element(i, panel);
		if( setup_panel_element(menu_elt) != GM_SUCCESS )
			continue;

		if ( (menu_elt->gm_module_init != NULL ) && (menu_elt->gm_module_init() == GM_SUCCESS) )
		{
			gm_menu_element_set_widget(menu_elt->gm_module_get_widget(), menu_elt);

			if(menu_elt->gm_module_set_icon_size != NULL)
			{
				menu_elt->gm_module_set_icon_size(panel->widget_width, panel->widget_height);
			}
		}
		else
    {
			///< \todo we should remove the panel element if the init function fails
      g_warning("Failed to initialize module %s", menu_elt->module);
    }

	}

	buttonbox = gm_layout_create_menu(panel);	
	return buttonbox;
}
