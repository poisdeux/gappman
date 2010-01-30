/***
 * \file parseconf.c
 *
 * 
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#include "gm_parseconf.h"
#include <string.h>

static int numberElts;
static char *program_menu_alignment;
static menu_elements* programs = NULL;
static menu_elements* actions = NULL;
static menu_elements* panel_elts;
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
        printf("width: %s, ", xmlTextReaderGetAttribute (reader, "width"));
        printf("height: %s, ", xmlTextReaderGetAttribute (reader, "height"));
        printf("orientation: %s\n", xmlTextReaderGetAttribute (reader, "orientation"));
      }
}

struct menu_element* createMenuElement()
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
    str_length->value = atoi (strndup(length, strspn( length,  "0123456789")));
    if ( strstr( length,  "%") != NULL )
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

char* getCachelocation()
{
	return cache_location;
}

char* getProgramname()
{
	return program_name;
}

xmlChar* getAlignment()
{
  return program_menu_alignment;
}

/**
* \brief process an program element from the XML configuration file.
* \param reader the XMLtext reader pointing to the configuration file.
* \param *elt menu_element structure that will contain the program configuration values
*/
static void
processMenuElement(xmlTextReaderPtr reader, menu_elements *elt, const char* element_name ) {
    xmlChar *name, *value;
    int ret = 1;
    
    if (name == NULL)
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
        if( strcmp((char *) name, "name") == 0 )
        {
          elt->name = value;
        }
        else if( strcmp((char *) name, "printlabel") == 0 )
        {
          elt->printlabel = atoi(value);
        }
        else if( strcmp((char *) name, "exec") == 0 )
        {
          elt->exec = value;
        }
        else if( strcmp((char *) name, "logo") == 0 )
        {
          elt->logo = value;
        }
        else if( strcmp((char *) name, "arg") == 0 )
        {
          addArgument(elt, (char *) value);
        }
        else if( strcmp((char *) name, "autostart") == 0 )
        {
          elt->autostart = atoi(value);
        }
        else if( strcmp((char *) name, "resolution") == 0 )
        {
          if(sscanf (value, "%dx%d", &elt->app_width, &elt->app_height) != 2)
          {
            fprintf(stderr, "Error: could not parse resolution value: %s", value);
          }
        }
        else if( strcmp((char *) name, "objectfile") == 0 )
				{
					elt->module = value;
				}
			  else if( strcmp((char *) name, "conffile") == 0 )
				{
					elt->module_conffile = value;
				}	
      }

      if( strcmp((char *) xmlTextReaderName(reader), element_name) == 0 && xmlTextReaderNodeType(reader) == 15 )
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
* \brief prints the elements of a menu_element structure
* \param *elt menu_element structure that should be printed
*/
void printMenuElements( menu_elements *elt )
{
  menu_elements *next;
  int i ;
	char *result = NULL;
	char **orient = NULL;

  while(elt != NULL)
  {
    printf("Element\t\tValue\n");
    printf("-----\n");
    printf("name:\t\t%s\n", elt->name);
    printf("logo:\t\t%s\n", elt->logo);
    printf("menuwidth:\t\t%d\n", elt->menu_width->value);
    printf("menuheight:\t\t%d\n", elt->menu_height->value);
    printf("app_width:\t\t%d\n", elt->app_width);
    printf("app_height:\t\t%d\n", elt->app_height);
		orient = elt->orientation;	
	
		i = 0;
		while ( elt->orientation[i] != NULL)
		{
			printf("orientation:\t\t%s\n", elt->orientation[i++]);
		}		
		printf("autostart:\t\t%d\n", elt->autostart);
		printf("printlabel:\t\t%d\n", elt->printlabel);
    printf("exec:\t\t%s\n", elt->exec);
    printf("numArguments:\t\t%d\n", elt->numArguments);
    printf("args:\t\t");
    for (i = 0; i < elt->numArguments; i++)
    {
      printf("%s ", elt->args[i]);
    }
    printf("\n");
    next = elt->next;
    elt = next;
  }
}

/**
* \brief returns the total number of menu_elements
*/
int getNumberOfElements()
{
  return numberElts;
}

/**
* \brief relinguishes the memory occupied by menu_element structures
* \param *elt first menu_element structure
*/
void freeMenuElements( menu_elements *elt )
{
  menu_elements *next;
  int i;
	char** orient;

	if (elt != NULL)
	{
		free(elt->amount_of_elements);
	  free(elt->menu_width);
 	 	free(elt->menu_height);
		i = 0;
		while(elt->orientation[i] != NULL)
		{
	 	 	free(elt->orientation[i]);
			i=i+1;
		}
		while(elt != NULL)
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

menu_elements* getPrograms()
{
  return programs;
}

menu_elements* getActions()
{
  return actions;
}

menu_elements* getPanel()
{
  return panel_elts;
}


/**
* \brief creates the menu_elements structures
* \param element_name name of the element being processed
* \param group_element_name name of the group the element belongs to. E.g programs or actions.
* \param reader the XML reader from libxml
* \param **elts pointer to the linked list of menu element structures.
* \return nothing. Use the **elts call by reference to retrieve the menu elements.
*/
void static processMenuElements(const char* element_name, const char* group_element_name, xmlTextReaderPtr reader, menu_elements **elts)
{
  int ret = 1;
	int i;
  xmlChar *name;
	char* result;
	char* align;
  int width;
  int height;
  int *number_elts;
  menu_elements *prev;
  struct length *menu_width;
  struct length *menu_height;
  char** orient;
  prev = NULL;

	number_elts = (int*) malloc (sizeof(int));
  menu_width = (struct length *) malloc (sizeof(struct length));
  menu_height = (struct length *) malloc (sizeof(struct length));
  orient = (char **) malloc (sizeof(char));
  
  //Initial default values 100%
  menu_width->value = 100;
  menu_width->type = PERCENTAGE;
  menu_height->value = 100;
  menu_height->type = PERCENTAGE;

  while (ret)
  {
    ret = xmlTextReaderRead(reader);
    name = xmlTextReaderName(reader);

		// Parse new program or action and create a new menu_element for it.
    if( strcmp((char *) name, element_name) == 0 && xmlTextReaderNodeType(reader) == 1)
    {
      *elts = createMenuElement();
      (*elts)->numArguments = 0;
      (*elts)->next = prev;
      (*elts)->menu_width = menu_width;
      (*elts)->menu_height = menu_height;
      (*elts)->orientation = orient;
      
      prev = *elts;
      processMenuElement(reader, *elts, element_name);
			(*number_elts)++;
    	(*elts)->amount_of_elements = number_elts;
    }
   
		//parse global parameters when endtag for groupelement is found 
    if( strcmp((char *) name, group_element_name) == 0 && xmlTextReaderNodeType(reader) == 15)
    {
      if (xmlTextReaderHasAttributes(reader))
      {
        parseLength(xmlTextReaderGetAttribute (reader, "width"), menu_width);
        parseLength(xmlTextReaderGetAttribute (reader, "height"), menu_height);
        align = xmlTextReaderGetAttribute (reader, "align");
				i = 0;
     		result = strtok(align, ",");
				while(result != NULL)
				{
						orient[i] = (char*) malloc ( strlen(result) * sizeof(char) );
						orient[i] = strcpy(orient[i], result);
						i=i+1;
     				result = strtok(NULL, ",");
				}
				orient[i] = NULL;
      }
			//this should end parsing this group of elements
      ret = 0;
    }
  }
}

int loadConf(const char *filename) {
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
				program_name = xmlTextReaderName(reader);

        while (ret == 1) 
				{
          name = xmlTextReaderName(reader);
          if( strcmp((char *) name, "programs") == 0 && xmlTextReaderNodeType(reader) == 1)
          {
            processMenuElements("program", "programs", reader, &programs);
          }
          else if( strcmp((char *) name, "actions") == 0 && xmlTextReaderNodeType(reader) == 1)
          {
            processMenuElements("action", "actions", reader, &actions);
          }
					else if( strcmp((char *) name, "panel") == 0 && xmlTextReaderNodeType(reader) == 1)
          {
            processMenuElements("applet", "panel", reader, &panel_elts);
          }
	  			if( strcmp((char *) name, "cachelocation") == 0 && xmlTextReaderNodeType(reader) == 1) 
	  			{ 
            ret = xmlTextReaderRead(reader);
	    			cache_location = xmlTextReaderValue(reader);
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
		return 0;
}
