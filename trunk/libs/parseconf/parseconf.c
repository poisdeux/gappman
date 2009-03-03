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
static struct length program_menu_width;
static struct length program_menu_height;
static char *program_menu_alignment;
static menu_elements* programs;
static menu_elements* actions;

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
* \brief Get the width of the program menu area
* \param width Width in pixels or percentage
* \return pointer to length struct
*/
struct length* getWidth()
{
  return &program_menu_width;
}

/**
* \brief Get the height of the program menu area
* \param height Height in pixels or percentage
* \return pointer to length struct
*/
struct length* getHeight()
{
  return &program_menu_height;
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
      }
      else if ( xmlTextReaderNodeType(reader) == 3 )
      {
        value = xmlTextReaderValue(reader);
        if( strcmp((char *) name, "name") == 0 )
        {
          printf("processProgram: %s\n", value);
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

  while(elt != NULL)
  {
    printf("Element\t\tValue\n");
    printf("-----\n");
    printf("name:\t\t%s\n", elt->name);
    printf("exec:\t\t%s\n", elt->exec);
    printf("logo:\t\t%s\n", elt->logo);
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
  while(elt != NULL)
  {
    //printf("menu_element free: menu_width\n");
    //fflush(stdout);
    next = elt->next;
    free(elt->menu_width);
    //printf("menu_element free: menu_height\n");
    //fflush(stdout);
    free(elt->menu_height);
    //printf("menu_element free: orientation\n");
    //fflush(stdout);
    free(elt->orientation);
    //printf("menu_element free: name\n");
    //fflush(stdout);
    free((xmlChar *) elt->name);
    //printf("menu_element free: exec\n");
    //fflush(stdout);
    free((xmlChar *) elt->exec);
    //printf("menu_element free: logo\n");
    //fflush(stdout);
    free((xmlChar *) elt->logo);
    for (i = 0; i < elt->numArguments; i++)
    {
    //printf("menu_element free: args[%d]\n", i);
    //fflush(stdout);
      free(elt->args[i]);
    }
    //printf("menu_element free: args\n");
    //fflush(stdout);
    free(elt->args);
    //printf("menu_element free: elt\n");
    //fflush(stdout);
    free(elt);
    //printf("menu_element free: next\n");
    //fflush(stdout);
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

void static processMenuElements(const char* element_name, const char* group_element_name, xmlTextReaderPtr reader, menu_elements **elts)
{
  int ret = 1;
  xmlChar *name;
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
  
  //Initial values 100%
  menu_width->value = 100;
  menu_width->type = PERCENTAGE;
  menu_height->value = 100;
  menu_height->type = PERCENTAGE;

  while (ret)
  {
    ret = xmlTextReaderRead(reader);
    name = xmlTextReaderName(reader);

    if( strcmp((char *) name, element_name) == 0 && xmlTextReaderNodeType(reader) == 1)
    {
      *elts = createMenuElement();
      (*elts)->numArguments = 0;
      (*elts)->next = prev;
      (*elts)->menu_width = menu_width;
      (*elts)->menu_height = menu_height;
      (*elts)->orientation = orient;
      
      prev = *elts;
      printf("processMenuElements: %s\n", name);
      processMenuElement(reader, *elts, element_name);
      numberElts++;
    }
    
    if( strcmp((char *) name, group_element_name) == 0 && xmlTextReaderNodeType(reader) == 15)
    {
      printf("processMenuElements: %s\n", name);
      if (xmlTextReaderHasAttributes(reader))
      {
        parseLength(xmlTextReaderGetAttribute (reader, "width"), menu_width);
        parseLength(xmlTextReaderGetAttribute (reader, "height"), menu_height);
        *orient = xmlTextReaderGetAttribute (reader, "align");
      
        printf("DEBUG: %d : %d : %s\n", menu_width->value, menu_height->value, *orient);
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

    reader = xmlReaderForFile(filename, NULL, 0);
    if (reader != NULL) {

        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            name = xmlTextReaderName(reader);
          if( strcmp((char *) name, "programs") == 0 && xmlTextReaderNodeType(reader) == 1)
          {
            processMenuElements("program", "programs", reader, &programs);
          }
          if( strcmp((char *) name, "actions") == 0 && xmlTextReaderNodeType(reader) == 1)
          {
            processMenuElements("action", "actions", reader, &actions);
          }
            ret = xmlTextReaderRead(reader);
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

//     return(programs);
}
