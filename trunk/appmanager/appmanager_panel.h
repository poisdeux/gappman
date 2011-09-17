#include <gtk/gtk.h>
#include <gm_generic.h>

/**
* \brief starts the elements in the panel
* \param *panel pointer to the menu_elements structures holding the panel elementts
*/            
void appmanager_start_panel(gm_menu *panel);

/** 
* \brief stops the elements in the panel 
* \param *panel pointer to the menu_elements structures holding the panel elementts 
*/ 
void appmanager_stop_panel(gm_menu *panel);
 
/**
* \brief sets up the modules in the panel and calls gm_layout_menu_create afterwards
* \param panel the gm_menu holding the panel elements
* \return GtkWidget pointer to the container holding the menu (as setup by gm_layout_menu_create)
*/
GtkWidget *appmanager_panel_create(gm_menu *panel);
