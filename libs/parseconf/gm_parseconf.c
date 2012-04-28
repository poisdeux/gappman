/**
 * \file gm_parseconf.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 * 
 */

#include <string.h>
#include "gm_parseconf.h"
#include <libxml/xmlreader.h>
#include <gm_generic.h>

static gm_menu *programs = NULL;
static gm_menu *actions = NULL;
static gm_menu *panel = NULL;
static char *program_name = NULL;
static char *cache_location = NULL;
static char *popup_key = NULL;			//key that will bring GAppMan to top of the window stack

static void printElements(xmlTextReaderPtr reader)
{
	printf("%d %d %s %d %d",
		   xmlTextReaderDepth(reader),
		   xmlTextReaderNodeType(reader),
		   xmlTextReaderName(reader),
		   xmlTextReaderIsEmptyElement(reader), xmlTextReaderHasValue(reader));
	if (xmlTextReaderValue(reader) == NULL)
		printf("\n");
	else
	{
		if (xmlStrlen(xmlTextReaderValue(reader)) > 40)
			printf(" %.40s...\n", xmlTextReaderValue(reader));
		else
			printf(" %s\n", xmlTextReaderValue(reader));
	}

	if (xmlTextReaderHasAttributes(reader))
	{
		printf("width: %s, ",
			   xmlTextReaderGetAttribute(reader, (const xmlChar *)"width"));
		printf("height: %s, ",
			   xmlTextReaderGetAttribute(reader, (const xmlChar *)"height"));
	}
}

/**
* \brief parse the length string to determine the used metric and fill the length struct.
* \param *length string containing length value
* \param *str_length struct to hold the length value and its metric type
*/
static void parseLength(xmlChar * length, struct length *str_length)
{
	if (length != NULL)
	{
		str_length->value =
			atoi(strndup
				 ((const char *)length,
				  strspn((const char *)length, "0123456789")));
		if (strstr((const char *)length, "%") != NULL)
		{
			str_length->type = PERCENTAGE;
		}
		else
		{
			str_length->type = PIXELS;
		}
	}
}

/**
* \brief process a program element from the XML configuration file.
* \param reader the XMLtext reader pointing to the configuration file.
* \param *elt menu_element structure that will contain the program configuration values
* \param element_name name of the XML-element. Needed to determine when end of XML-block
*        is reached.
*/
static void
processMenuElement(xmlTextReaderPtr reader, gm_menu_element *elt,
				   const char *element_name)
{
	xmlChar *name = NULL;
	xmlChar *value = NULL;
	int ret = 1;


	name = BAD_CAST "--";

	while (ret == 1)
	{
		// printElements(reader);

		if (xmlTextReaderNodeType(reader) == 1)
		{
			name = xmlTextReaderName(reader);
		}
		else if (xmlTextReaderNodeType(reader) == 3)
		{
			value = xmlTextReaderValue(reader);
			if (strcmp((char *)name, "name") == 0)
			{
				elt->name = value;
			}
			else if (strcmp((char *)name, "printlabel") == 0)
			{
				elt->printlabel = atoi((const char *)value);
			}
			else if (strcmp((char *)name, "exec") == 0)
			{
				elt->exec = value;
			}
			else if (strcmp((char *)name, "logo") == 0)
			{
				elt->logo = value;
			}
			else if (strcmp((char *)name, "arg") == 0)
			{
				// \todo if gm_menu_element_add_argument fails we should remove the element from the menu
        // to prevent executing a program with incorrect parameters
				gm_menu_element_add_argument(value, elt);
			}
			else if (strcmp((char *)name, "autostart") == 0)
			{
				elt->autostart = atoi((const char *)value);
			}
			else if (strcmp((char *)name, "resolution") == 0)
			{
				if (sscanf
					((const char *)value, "%dx%d", &elt->app_width,
					 &elt->app_height) != 2)
				{
					fprintf(stderr,
							"Error: could not parse resolution value: %s",
							value);
				}
			}
			else if (strcmp((char *)name, "objectfile") == 0)
			{
				elt->module = value;
			}
			else if (strcmp((char *)name, "conffile") == 0)
			{
				elt->module_conffile = value;
			}
		}

		if (strcmp((char *)xmlTextReaderName(reader), element_name) == 0
			&& xmlTextReaderNodeType(reader) == 15)
		{
			ret = 0;
		}
		else
		{
			ret = xmlTextReaderRead(reader);
		}
	}
}

static void gm_parse_alignment(char *align, float *hor_align, int *vert_align)
{
	char *result;

	result = strtok(align, ",");
	while (result != NULL)
	{
		if (strcmp(result, "top") == 0)
		{
			*vert_align = 0;
		}
		else if (strcmp(result, "bottom") == 0)
		{
			*vert_align = 2;
		}
		else if (strcmp(result, "left") == 0)
		{
			*hor_align = 0.0;
		}
		else if (strcmp(result, "right") == 0)
		{
			*hor_align = 1.0;
		}
		else if (strcmp(result, "center") == 0)
		{
			*hor_align = 0.5;
		}
		result = strtok(NULL, ",");
	}
}

/**
* \brief creates the menu_element structures
* \param element_name name of the element being processed
* \param group_element_name name of the group the element belongs to. E.g programs or actions.
* \param reader the XML reader from libxml
* \param **elts pointer to the linked list of menu element structures.
* \return nothing. Use the **elts call by reference to retrieve the menu elements.
*/
static void processMenuElements(const char *element_name,
								const char *group_element_name,
								xmlTextReaderPtr reader, gm_menu *menu)
{
	int ret = 1;
	xmlChar *name;
	xmlChar *attr;
  gm_menu_element *elt;

	while (ret)
	{
		ret = xmlTextReaderRead(reader);
		name = xmlTextReaderName(reader);

		// Parse new program or action and create a new menu_element for it.
		if (strcmp((char *)name, element_name) == 0
			&& xmlTextReaderNodeType(reader) == 1)
		{
			elt = gm_menu_element_create();
			if ( elt == NULL )
			{	
				g_warning("processMenuElements: failed to create menu_element");
				continue;
			}
			processMenuElement(reader, elt, element_name);

			if ( ! gm_menu_add_menu_element(elt, menu) )
			{
				g_warning("processMenuElements: failed to add menu_element to menu");
			}
		}

		// parse global parameters when endtag for groupelement is found
		if (strcmp((char *)name, group_element_name) == 0
			&& xmlTextReaderNodeType(reader) == 15)
		{
			if (xmlTextReaderHasAttributes(reader))
			{
				parseLength(xmlTextReaderGetAttribute
							(reader, (const xmlChar *)"width"), &(menu->menu_width));
				parseLength(xmlTextReaderGetAttribute
							(reader, (const xmlChar *)"height"), &(menu->menu_height));
				gm_parse_alignment((char *)xmlTextReaderGetAttribute(reader,
																	 (const
																	  xmlChar
																	  *)"align"),
								   &(menu->hor_alignment), &(menu->vert_alignment));
				attr = xmlTextReaderGetAttribute(reader, (const xmlChar *)"max_elts");
				if ( attr != NULL )
				{
					menu->max_elts_in_single_box = atoi(attr);		
				}	
			}
			// this should end parsing this group of elements
			ret = 0;
		}
	}
}

gchar *gm_parseconf_get_cache_location()
{
	return cache_location;
}

gchar *gm_parseconf_get_popupkey()
{
	return popup_key;
}

gchar *gm_parseconf_get_programname()
{
	return program_name;
}

gm_menu *gm_get_programs()
{
	return programs;
}

gm_menu *gm_get_actions()
{
	return actions;
}

gm_menu *gm_get_panel()
{
	return panel;
}

int gm_load_conf(const char *filename)
{
	xmlTextReaderPtr reader;
	int ret;
	xmlChar *name;

	// Initialize
	programs = gm_menu_create();
	actions = gm_menu_create();
	panel = gm_menu_create();
	cache_location = NULL;
	program_name = NULL;

	reader = xmlReaderForFile(filename, NULL, 0);
	if (reader != NULL)
	{
		ret = xmlTextReaderRead(reader);

		// first xml-element must be the name of the program
		program_name = (char *)xmlTextReaderName(reader);

		while (ret == 1)
		{
			name = xmlTextReaderName(reader);
			if (strcmp((char *)name, "programs") == 0
				&& xmlTextReaderNodeType(reader) == 1)
			{
#ifdef DEBUG
g_debug("gm_load_conf: processing %s", name);
#endif
				processMenuElements("program", "programs", reader, programs);
			}
			else if (strcmp((char *)name, "actions") == 0
					 && xmlTextReaderNodeType(reader) == 1)
			{
#ifdef DEBUG
g_debug("gm_load_conf: processing %s", name);
#endif
				processMenuElements("action", "actions", reader, actions);
			}
			else if (strcmp((char *)name, "panel") == 0
					 && xmlTextReaderNodeType(reader) == 1)
			{
#ifdef DEBUG
g_debug("gm_load_conf: processing %s", name);
#endif
				processMenuElements("applet", "panel", reader, panel);
			} 
			else if (strcmp((char *)name, "cachelocation") == 0
				&& xmlTextReaderNodeType(reader) == 1)
			{
				ret = xmlTextReaderRead(reader);
				cache_location = (char *)xmlTextReaderValue(reader);
#ifdef DEBUG
g_debug("gm_load_conf: cache_location=%s", cache_location);
#endif
			}
			else if (strcmp((char *)name, "popupkey") == 0
        && xmlTextReaderNodeType(reader) == 1)
      {
				ret = xmlTextReaderRead(reader);
				popup_key = (char *)xmlTextReaderValue(reader);
#ifdef DEBUG
g_debug("gm_load_conf: popup_key=%s", popup_key);
#endif
			}

			ret = xmlTextReaderRead(reader);
		}

		/**
         * Free up the reader
        */
		xmlFreeTextReader(reader);
		if (ret != 0)
		{
			g_warning("%s : failed to parse\n", filename);
		}
	}
	else
	{
		g_warning("Unable to open %s\n", filename);
		return 1;
	}
	/**
    * Cleanup function for the XML library.
    */
	xmlCleanupParser();
	return GM_SUCCESS;
}

