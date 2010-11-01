/**
 * \file gm_parseconf.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#include <string.h>
#include "gm_parseconf.h"
#include <gm_generic.h>

static int numberElts;
static menu_elements* programs = NULL;
static menu_elements* actions = NULL;
static menu_elements* panel_elts = NULL;
static char *program_name;
static char *cache_location;

static void printElements(xmlTextReaderPtr reader)
{
    printf("%d %d %s %d %d",
           xmlTextReaderDepth(reader),
           xmlTextReaderNodeType(reader),
           xmlTextReaderName(reader),
           xmlTextReaderIsEmptyElement(reader),
           xmlTextReaderHasValue(reader));
    if (xmlTextReaderValue(reader) == NULL)
        printf("\n");
    else {
        if (xmlStrlen(xmlTextReaderValue(reader)) > 40)
            printf(" %.40s...\n", xmlTextReaderValue(reader));
        else
            printf(" %s\n", xmlTextReaderValue(reader));
    }

    if (xmlTextReaderHasAttributes(reader))
    {
        printf("width: %s, ", xmlTextReaderGetAttribute (reader, (const xmlChar*) "width"));
        printf("height: %s, ", xmlTextReaderGetAttribute (reader, (const xmlChar*) "height"));
    }
}

/**
* \brief creates and initializes a new menu_element struct
*/
static struct menu_element* createMenuElement()
{
    struct menu_element *elt;
    elt = (menu_elements *) malloc(sizeof(menu_elements));
    elt->numArguments = 0;
    elt->logo = NULL;
    elt->name = NULL;
    elt->exec = NULL;
    elt->next = NULL;
    elt->args = NULL;
    elt->module_conffile = NULL;
    elt->autostart = 0;
    elt->printlabel = 0;
    elt->app_height = -1;
    elt->app_width = -1;
    elt->pid = -1;
    elt->numArguments = 0;
    return elt;
}

/**
* \brief parse the length string to determine the used metric and fill the length struct.
* \param *length string containing length value
* \param *str_length struct to hold the length value and its metric type
*/
static void parseLength(xmlChar * length, struct length *str_length)
{
    if ( length != NULL )
    {
        str_length->value = atoi (strndup((const char*) length, strspn( (const char*) length,  "0123456789")));
        if ( strstr( (const char*) length,  "%") != NULL )
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
static void addArgument(menu_elements *elt, char *argument)
{
    elt->numArguments++;
    elt->args = (char **) realloc(elt->args, ((elt->numArguments) * sizeof(char *)));
    elt->args[elt->numArguments - 1] = (char *) argument;
}

char* gm_get_cache_location()
{
    return cache_location;
}

char* gm_get_programname()
{
    return program_name;
}

/**
* \brief process an program element from the XML configuration file.
* \param reader the XMLtext reader pointing to the configuration file.
* \param *elt menu_element structure that will contain the program configuration values
*/
static void
processMenuElement(xmlTextReaderPtr reader, menu_elements *elt, const char* element_name ) 
{
    xmlChar *name = NULL;
	xmlChar *value = NULL;
    int ret = 1;

   	name = BAD_CAST "--";

    while ( ret == 1 )
    {
//       printElements(reader);

        if ( xmlTextReaderNodeType(reader) == 1 )
        {
            name = xmlTextReaderName(reader);
        }
        else if ( xmlTextReaderNodeType(reader) == 3 )
        {
            value = xmlTextReaderValue(reader);
            if ( strcmp((char *) name, "name") == 0 )
            {
                elt->name = value;
            }
            else if ( strcmp((char *) name, "printlabel") == 0 )
            {
                elt->printlabel = atoi((const char*) value);
            }
            else if ( strcmp((char *) name, "exec") == 0 )
            {
                elt->exec = value;
            }
            else if ( strcmp((char *) name, "logo") == 0 )
            {
                elt->logo = value;
            }
            else if ( strcmp((char *) name, "arg") == 0 )
            {
                addArgument(elt, (char *) value);
            }
            else if ( strcmp((char *) name, "autostart") == 0 )
            {
                elt->autostart = atoi((const char*) value);
            }
            else if ( strcmp((char *) name, "resolution") == 0 )
            {
                if (sscanf ((const char*) value, "%dx%d", &elt->app_width, &elt->app_height) != 2)
                {
                    fprintf(stderr, "Error: could not parse resolution value: %s", value);
                }
            }
            else if ( strcmp((char *) name, "objectfile") == 0 )
            {
                elt->module = value;
            }
            else if ( strcmp((char *) name, "conffile") == 0 )
            {
                elt->module_conffile = value;
            }
        }

        if ( strcmp((char *) xmlTextReaderName(reader), element_name) == 0 && xmlTextReaderNodeType(reader) == 15 )
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
* \brief returns the total number of menu_elements
*/
int gm_get_number_of_elements()
{
    return numberElts;
}

/**
* \brief relinguishes the memory occupied by menu_element structures
* \param *elt first menu_element structure
*/
void gm_free_menu_elements( menu_elements *elt )
{
    menu_elements *next;
    int i;

    if (elt != NULL)
    {
        free(elt->amount_of_elements);
        free(elt->menu_width);
        free(elt->menu_height);
        free(elt->hor_alignment);
        free(elt->vert_alignment);

        while (elt != NULL)
        {
            free((xmlChar *) elt->name);
            free((xmlChar *) elt->exec);
            free((xmlChar *) elt->module);
            free((xmlChar *) elt->logo);

            for (i = 0; i < elt->numArguments; i++)
            {
                free(elt->args[i]);
            }
            free(elt->args);

            next = elt->next;
            free(elt);
            elt = next;
        }
    }
}

menu_elements* gm_get_programs()
{
    return programs;
}

menu_elements* gm_get_actions()
{
    return actions;
}

menu_elements* gm_get_panel()
{
    return panel_elts;
}

static void gm_parse_alignment(char* align, float *hor_align, int *vert_align)
{
    char* result;

    result = strtok(align, ",");
    while ( result != NULL )
    {
        if ( strcmp(result, "top") == 0 )
        {
            *vert_align = 0;
        }
        else if ( strcmp(result, "bottom") == 0 )
        {
            *vert_align = 2;
        }
        else if ( strcmp(result, "left") == 0 )
        {
            *hor_align = 0.0;
        }
        else if ( strcmp(result, "right") == 0 )
        {
            *hor_align = 1.0;
        }
        else if ( strcmp(result, "center") == 0 )
        {
            *hor_align = 0.5;
        }
        result = strtok(NULL, ",");
    }
}

/**
* \brief creates the menu_elements structures
* \param element_name name of the element being processed
* \param group_element_name name of the group the element belongs to. E.g programs or actions.
* \param reader the XML reader from libxml
* \param **elts pointer to the linked list of menu element structures.
* \return nothing. Use the **elts call by reference to retrieve the menu elements.
*/
static void processMenuElements(const char* element_name, const char* group_element_name, xmlTextReaderPtr reader, menu_elements **elts)
{
    int ret = 1;
    xmlChar *name;
    int *number_elts;
    float *hor_align;
    int *vert_align;
    menu_elements *prev;
    struct length *menu_width;
    struct length *menu_height;
    prev = NULL;

    number_elts = (int*) malloc (sizeof(int));
    hor_align = (float *) malloc (sizeof(float));
    vert_align = (int *) malloc (sizeof(int));
    menu_width = (struct length *) malloc (sizeof(struct length));
    menu_height = (struct length *) malloc (sizeof(struct length));

    //Initial default values 100%
    menu_width->value = 100;
    menu_width->type = PERCENTAGE;
    menu_height->value = 100;
    menu_height->type = PERCENTAGE;
    *hor_align = 0.5; //<! default center
    *vert_align = 1; //<! default center

    while (ret)
    {
        ret = xmlTextReaderRead(reader);
        name = xmlTextReaderName(reader);

        // Parse new program or action and create a new menu_element for it.
        if ( strcmp((char *) name, element_name) == 0 && xmlTextReaderNodeType(reader) == 1)
        {
            *elts = createMenuElement();
            (*elts)->next = prev;

            //the following struct items are shared among all elements of the same group
            (*elts)->menu_width = menu_width;
            (*elts)->menu_height = menu_height;
            (*elts)->hor_alignment = hor_align;
            (*elts)->vert_alignment = vert_align;
            (*number_elts)++;
            (*elts)->amount_of_elements = number_elts;

            prev = *elts;
            processMenuElement(reader, *elts, element_name);
        }

        //parse global parameters when endtag for groupelement is found
        if ( strcmp((char *) name, group_element_name) == 0 && xmlTextReaderNodeType(reader) == 15)
        {
            if (xmlTextReaderHasAttributes(reader))
            {
                parseLength(xmlTextReaderGetAttribute (reader, (const xmlChar*) "width"), menu_width);
                parseLength(xmlTextReaderGetAttribute (reader, (const xmlChar*) "height"), menu_height);
                gm_parse_alignment((char*) xmlTextReaderGetAttribute (reader, (const xmlChar*) "align"), hor_align, vert_align);
            }
            //this should end parsing this group of elements
            ret = 0;
        }
    }
}

int gm_load_conf(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;
    xmlChar *name;
    numberElts = 0;

    //Initialize
    programs = NULL;
    actions = NULL;
    panel_elts = NULL;
    cache_location = NULL;
    program_name = NULL;

    reader = xmlReaderForFile(filename, NULL, 0);
    if (reader != NULL)
    {
        ret = xmlTextReaderRead(reader);

        // first attribute must be the name of the program
        program_name = (char*) xmlTextReaderName(reader);

        while (ret == 1)
        {
            name = xmlTextReaderName(reader);
            if ( strcmp((char *) name, "programs") == 0 && xmlTextReaderNodeType(reader) == 1)
            {
                processMenuElements("program", "programs", reader, &programs);
            }
            else if ( strcmp((char *) name, "actions") == 0 && xmlTextReaderNodeType(reader) == 1)
            {
                processMenuElements("action", "actions", reader, &actions);
            }
            else if ( strcmp((char *) name, "panel") == 0 && xmlTextReaderNodeType(reader) == 1)
            {
                processMenuElements("applet", "panel", reader, &panel_elts);
            }
            if ( strcmp((char *) name, "cachelocation") == 0 && xmlTextReaderNodeType(reader) == 1)
            {
                ret = xmlTextReaderRead(reader);
                cache_location = (char*) xmlTextReaderValue(reader);
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


menu_elements* gm_search_elt_by_name(gchar* name, menu_elements* programs)
{
    while ( programs != NULL )
    {
        if ( g_strcmp0(name, (const char*) programs->name) == 0 )
        {
            return programs;
        }
        programs = programs->next;
    }
		return NULL;
}
