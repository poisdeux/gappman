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
#include <gm_generic.h>

static struct menu *programs = NULL;
static struct menu *actions = NULL;
static struct menu *panel = NULL;
static char *program_name;
static char *cache_location;

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

static struct menu *createMenu()
{
	struct menu *dish;
	dish = (struct menu *) malloc(sizeof(struct menu));

	// Initial default values
	dish->menu_width.value = 100;
	dish->menu_width.type = PERCENTAGE;
	dish->menu_height.value = 100;
	dish->menu_height.type = PERCENTAGE;
	dish->hor_alignment = 0.5;			// <! default center
	dish->vert_alignment = 1;			// <! default center
	dish->amount_of_elements = 0;
	dish->elts = (struct menu_element*) malloc(5 * sizeof(struct menu_element));
	return dish;
}

/**
* \brief creates and initializes a new menu_element struct
*/
static struct menu_element createMenuElement()
{
	struct menu_element *elt;
	elt = (struct menu_element *) malloc(sizeof(struct menu_element));
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
	return *elt;
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
* \brief add an argument to the argument list for elt
* \param *elt menu_element structure which should have its argument list expanded
* \param *argument argument that should be added to *elt->args
*/
static void addArgument(struct menu_element *elt, char *argument)
{
	elt->numArguments++;
	elt->args =
		(char **)realloc(elt->args, ((elt->numArguments) * sizeof(char *)));
	elt->args[elt->numArguments - 1] = (char *)argument;
}

char *gm_get_cache_location()
{
	return cache_location;
}

char *gm_get_programname()
{
	return program_name;
}

/**
* \brief process a program element from the XML configuration file.
* \param reader the XMLtext reader pointing to the configuration file.
* \param *elt menu_element structure that will contain the program configuration values
* \param element_name name of the XML-element. Needed to determine when end of XML-block
*        is reached.
*/
static void
processMenuElement(xmlTextReaderPtr reader, struct menu_element *elt,
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
				addArgument(elt, (char *)value);
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


/**
* \brief relinguishes the memory occupied by menu_element structures
* \param *elt first menu_element structure
*/
void gm_free_menu(struct menu *dish)
{
	int i,j;

	if (dish != NULL)
	{
		for(i = 0; i < dish->amount_of_elements; i++)
		{
			free((xmlChar *) (dish->elts[i]).name);
			free((xmlChar *) (dish->elts[i]).exec);
			free((xmlChar *) (dish->elts[i]).module);
			free((xmlChar *) (dish->elts[i]).module_conffile);
			free((xmlChar *) (dish->elts[i]).logo);

			for (j = 0; j < (dish->elts[i]).numArguments; j++)
			{
				free((dish->elts[i]).args[j]);
			}
			free((dish->elts[i]).args);
		}
		free(dish->elts);
	}
}

struct menu *gm_get_programs()
{
	return programs;
}

struct menu *gm_get_actions()
{
	return actions;
}

struct menu *gm_get_panel()
{
	return panel;
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
								xmlTextReaderPtr reader, struct menu *dish)
{
	int ret = 1;
	xmlChar *name;
	xmlChar *attr;

	while (ret)
	{
		ret = xmlTextReaderRead(reader);
		name = xmlTextReaderName(reader);

		// Parse new program or action and create a new menu_element for it.
		if (strcmp((char *)name, element_name) == 0
			&& xmlTextReaderNodeType(reader) == 1)
		{
			dish->elts[dish->amount_of_elements] = createMenuElement();
			processMenuElement(reader, &(dish->elts[dish->amount_of_elements]), element_name);
			dish->amount_of_elements++;
			//check if we need to resize
			if((dish->amount_of_elements % 5) == 0)
			{
				dish->elts = (struct menu_element*) realloc(dish->elts, 
									(dish->amount_of_elements + 5) * sizeof(struct menu_element));
			}
		}

		// parse global parameters when endtag for groupelement is found
		if (strcmp((char *)name, group_element_name) == 0
			&& xmlTextReaderNodeType(reader) == 15)
		{
			if (xmlTextReaderHasAttributes(reader))
			{
				parseLength(xmlTextReaderGetAttribute
							(reader, (const xmlChar *)"width"), &(dish->menu_width));
				parseLength(xmlTextReaderGetAttribute
							(reader, (const xmlChar *)"height"), &(dish->menu_height));
				gm_parse_alignment((char *)xmlTextReaderGetAttribute(reader,
																	 (const
																	  xmlChar
																	  *)"align"),
								   &(dish->hor_alignment), &(dish->vert_alignment));
				attr = xmlTextReaderGetAttribute(reader, (const xmlChar *)"max_elts");
				if ( attr != NULL )
				{
					dish->max_elts_in_single_box = atoi(attr);		
				}	
				else
				{
					// set to maximum to disable creation of multiple boxes
					// in gm_create_buttonboxes
					dish->max_elts_in_single_box = dish->amount_of_elements;
				}
			}
			// this should end parsing this group of elements
			ret = 0;
		}
	}
}

int gm_load_conf(const char *filename)
{
	xmlTextReaderPtr reader;
	int ret;
	xmlChar *name;

	// Initialize
	programs = createMenu();
	actions = createMenu();
	panel = createMenu();
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
				processMenuElements("program", "programs", reader, programs);
			}
			else if (strcmp((char *)name, "actions") == 0
					 && xmlTextReaderNodeType(reader) == 1)
			{
				processMenuElements("action", "actions", reader, actions);
			}
			else if (strcmp((char *)name, "panel") == 0
					 && xmlTextReaderNodeType(reader) == 1)
			{
				processMenuElements("applet", "panel", reader, panel);
			}
			if (strcmp((char *)name, "cachelocation") == 0
				&& xmlTextReaderNodeType(reader) == 1)
			{
				ret = xmlTextReaderRead(reader);
				cache_location = (char *)xmlTextReaderValue(reader);
			}
			else
			{
				ret = xmlTextReaderRead(reader);
			}
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
	return GM_SUCCES;
}


struct menu_element *gm_search_elt_by_name(gchar * name, struct menu *dish)
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
