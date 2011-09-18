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
static GtkWidget *create_panel_element(gm_menu_element * elt)
{
  GModule *module;

  module = g_module_open((const gchar *)elt->module, G_MODULE_BIND_LAZY);

  if (!module)
  {
    g_warning("Could not load module %s\n%s", elt->module,
          g_module_error());
    return NULL;
  }
  else
  {
    if (!g_module_symbol
      (module, "gm_module_start", (gpointer *) & (elt->gm_module_start)))
    {
      elt->gm_module_start = NULL;
      g_warning("Could not get function gm_module_start from %s\n%s",
            elt->module, g_module_error());
    }

    if (!g_module_symbol
      (module, "gm_module_stop", (gpointer *) & (elt->gm_module_stop)))
    {
      elt->gm_module_stop = NULL;
      g_warning("Could not get function gm_module_stop from %s\n%s",
            elt->module, g_module_error());
    }

    if (!g_module_symbol
      (module, "gm_module_init", (gpointer *) & (elt->gm_module_init)))
    {
      elt->gm_module_init = NULL;
      g_warning("Could not get function gm_module_init from %s\n%s",
            elt->module, g_module_error());
    }
    if (!g_module_symbol
      (module, "gm_module_get_widget",
       (gpointer *) & (elt->gm_module_get_widget)))
    {
      elt->gm_module_get_widget = NULL;
      g_warning
        ("Could not get function gm_module_get_widget from %s\n%s",
         elt->module, g_module_error());
    }
    if (elt->module_conffile != NULL)
    {
      if (!g_module_symbol
        (module, "gm_module_set_conffile",
         (gpointer *) & (elt->gm_module_set_conffile)))
      {
        elt->gm_module_set_conffile = NULL;
        g_warning
          ("Could not get function gm_module_set_conffile from %s\n%s",
           elt->module, g_module_error());
      }
      else
      {
        elt->
          gm_module_set_conffile((const gchar *)
                       elt->module_conffile);
      }
    }
    if (!g_module_symbol
      (module, "gm_module_set_icon_size",
       (gpointer *) & (elt->gm_module_set_icon_size)))
    {
      elt->gm_module_set_icon_size = NULL;
      g_warning
        ("Could not get function gm_module_set_icon_size from %s\n%s",
         elt->module, g_module_error());
    }

    if (elt->gm_module_init() != GM_SUCCESS)
    {
      g_warning("Failed to initialize module %s", elt->module);
      return NULL;
    }
  }

  return elt->gm_module_get_widget();
}


GtkWidget *appmanager_panel_create(gm_menu *panel)
{
	int i;
	GtkWidget *widget;
	GtkWidget *buttonbox;

	for( i = 0; i < panel->amount_of_elements; i++ )
	{
		widget = create_panel_element(panel->elts[i]); 
		gm_menu_element_set_widget(widget, panel->elts[i]);
	}

	gm_layout_calculate_sizes(panel);
	buttonbox = gm_layout_create_menu(panel);	

	//gm_layout_create_menu determined widget size so we can now
  //let all widgets know the required size
	for( i = 0; i < panel->amount_of_elements; i++ )
  {
		if(panel->elts[i]->gm_module_set_icon_size != NULL)
		{
			panel->elts[i]->gm_module_set_icon_size(panel->widget_width, panel->widget_height);
		}
	}
	return buttonbox;
}
