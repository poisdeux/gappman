#include <appmanager_buttonmenu.h>

GtkWidget* appmanager_buttonmenu_create(gm_menu *menu, 
						void (*processevent) (GtkWidget *, GdkEvent *, 
										gm_menu_element *))
{
	gint i;
	gint width;
	gint height;
	GtkWidget *button;
	GtkWidget *buttonbox;

	gm_layout_calculate_sizes(menu);

	width = menu->widget_width;	
	height = menu->widget_height;	

	for(i = 0; i < menu->amount_of_elements; i++)
	{
		button = gm_layout_create_button(menu->elts[i], width, 
												height, processevent);		
		gm_menu_element_set_widget(button, menu->elts[i]);
	}

	buttonbox = gm_layout_create_menu(menu, processevent);

	return buttonbox;
}
