#include <gtk/gtk.h>
#include <gm_generic.h>

/**
* \brief sets up the buttons and calls gm_layout_menu_create afterwards
* \param menu pointer to gm_menu holding the buttons
* \param processevent callback function that should be called when one of the buttons is pressed
* \return GtkWidget pointer to the container holding the menu (as setup by gm_layout_menu_create)
*/
GtkWidget* appmanager_buttonmenu_create(gm_menu *menu, 
						void (*processevent) (GtkWidget *, GdkEvent *, 
										gm_menu_element *));
