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

#include "parseconf.h"
#include <string.h>

static int numberElts;
static char *program_menu_alignment;
static menu_elements* programs;
static menu_elements* actions;
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
  elt->autostart = 0;
  elt->printlabel = 0;
  elt->app_height = -1;
  elt->app_width = -1;
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

/**
* \brief Get the path of the cache location on disk
* \return string
*/
char* getCachelocation()
{
	return cache_location;
}

/**
* \brief Get the name of the program as specified in the configuration file
* \return string 
*/
char* getProgramname()
{
	return program_name;
}

/**
* \brief Get the orientation of the program menu area
* \param orientation Orientation top, bottom, left or right
*/
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
	//printf("DEBUG: Attribute: %s\n", name);
      }
      else if ( xmlTextReaderNodeType(reader) == 3 )
      {
        value = xmlTextReaderValue(reader);
        if( strcmp((char *) name, "name") == 0 )
        {
          //printf("DEBUG: processProgram: %s\n", value);
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

	//free shared elements
	printf("Freeing shared elements\n");
	fflush(stdout);
  printf("menu_element free: menu_width\n");
  fflush(stdout);
  next = elt->next;
  free(elt->menu_width);
  printf("menu_element free: menu_height\n");
  fflush(stdout);
  free(elt->menu_height);
  printf("menu_element free: orientation\n");
	i = 0;
	while(elt->orientation[i] != NULL)
	{
		printf("Length: %d String: %s\n", strlen(elt->orientation[i]), elt->orientation[i]);
  	fflush(stdout);		
  	free(elt->orientation[i]);
		i=i+1;
	}
	printf("Freeing individual element\n");
	fflush(stdout);
	while(elt != NULL)
  {
    printf("menu_element free: name %s\n", elt->name);
    fflush(stdout);
    free((xmlChar *) elt->name);
    printf("menu_element free: exec %s\n", elt->exec);
    fflush(stdout);
    free((xmlChar *) elt->exec);
    printf("menu_element free: logo %s\n", elt->logo);
    fflush(stdout);
    free((xmlChar *) elt->logo);
    for (i = 0; i < elt->numArguments; i++)
    {
    printf("menu_element free: args[%d]\n", i);
    fflush(stdout);
    free(elt->args[i]);
    }
    printf("menu_element free: args\n");
    fflush(stdout);
    free(elt->args);
    printf("menu_element free: elt\n");
    fflush(stdout);
    free(elt);
    printf("menu_element free: next\n");
    fflush(stdout);
    elt = next;
  }
}

/**
* \brief returns the menu_elements structure that contains the programs
* \return pointer to menu_elements structure
*/
menu_elements* getPrograms()
{
  return programs;
}

/**
* \brief returns the menu_elements structure that contains the actions
* \return pointer to menu_elements structure
*/
menu_elements* getActions()
{
  return actions;
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
  menu_elements *prev;
  struct length *menu_width;
  struct length *menu_height;
  char** orient;
  prev = NULL;

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
      //printf("DEBUG: processMenuElements: %s\n", name);
      processMenuElement(reader, *elts, element_name);
      numberElts++;
    }
   
		//parse global parameters 
    if( strcmp((char *) name, group_element_name) == 0 && xmlTextReaderNodeType(reader) == 15)
    {
      //printf("DEBUG: processMenuElements: %s\n", name);
      if (xmlTextReaderHasAttributes(reader))
      {
        parseLength(xmlTextReaderGetAttribute (reader, "width"), menu_width);
        parseLength(xmlTextReaderGetAttribute (reader, "height"), menu_height);
        printf("DEBUG: menu_width %d, menu_height %d\n", menu_width->value, menu_height->value);
        align = xmlTextReaderGetAttribute (reader, "align");
				i = 0;
     		result = strtok(align, ",");
				while(result != NULL)
				{
						//printf("DEBUG: orient: %s\n", result);
						orient[i] = (char*) malloc ( strlen(result) * sizeof(char) );
						orient[i] = strcpy(orient[i], result);
						i=i+1;
     				result = strtok(NULL, ",");
				}
				orient[i] = NULL;
      }
      ret = 0;
    }
  }
}

/**
* \brief load the configuration file and parses it to create the menu_elements structures for each program.
* \param  *filename the name of the configuration file with the path
*/
void loadConf(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;
    xmlChar *name;
    numberElts = 0;

    //Initialize
    programs = NULL;
    actions = NULL;
    cache_location = NULL;
    program_name = NULL;

    reader = xmlReaderForFile(filename, NULL, 0);
    if (reader != NULL) {

        ret = xmlTextReaderRead(reader);

        // first attribute must be the name of the program 
	program_name = xmlTextReaderName(reader);
	//printf("DEBUG: Program_name: %s\n", program_name);

        while (ret == 1) {
          name = xmlTextReaderName(reader);
          if( strcmp((char *) name, "programs") == 0 && xmlTextReaderNodeType(reader) == 1)
          {
            processMenuElements("program", "programs", reader, &programs);
          }
          else if( strcmp((char *) name, "actions") == 0 && xmlTextReaderNodeType(reader) == 1)
          {
            processMenuElements("action", "actions", reader, &actions);
          }
	  if( strcmp((char *) name, "cachelocation") == 0 && xmlTextReaderNodeType(reader) == 1) 
	  { 
            ret = xmlTextReaderRead(reader);
	    cache_location = xmlTextReaderValue(reader);
	    //printf("DEBUG: Cache_location: %s\n",cache_location);
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
        if (ret != 0) {
            fprintf(stderr, "%s : failed to parse\n", filename);
        }

    } else {
        fprintf(stderr, "Unable to open %s\n", filename);
    }
    /**
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();

}
