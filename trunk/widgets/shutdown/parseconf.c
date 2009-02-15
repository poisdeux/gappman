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
static struct length widget_width;
static struct length widget_height;
static char *widget_alignment;
static buttons* programs;
static buttons* actions;

static void printXmlText(xmlTextReaderPtr reader)
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

struct button* createElement()
{
  struct button *elt;
  elt = (buttons *) malloc(sizeof(buttons));
  elt->imagefilename = NULL;
  elt->labeltext = NULL;
  elt->executable = NULL;
  elt->next = NULL;
  elt->args = NULL;
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
* \param *elt button structure which should have its argument list expanded
* \param *argument argument that should be added to *elt->args
*/
static void addArgument(buttons *elt, char *argument)
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
  return &widget_width;
}

/**
* \brief Get the height of the program menu area
* \param height Height in pixels or percentage
* \return pointer to length struct
*/
struct length* getHeight()
{
  return &widget_height;
}

/**
* \brief Get the orientation of the program menu area
* \param orientation Orientation top, bottom, left or right
*/
xmlChar* getAlignment()
{
  return widget_alignment;
}

/**
* \brief process an program element from the XML configuration file.
* \param reader the XMLtext reader pointing to the configuration file.
* \param *elt button structure that will contain the program configuration values
*/
static void
processElement(xmlTextReaderPtr reader, buttons *elt, const char* element_name ) {
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
          elt->labeltext = value;
        }
        else if( strcmp((char *) name, "exec") == 0 )
        {
          elt->executable = value;
        }
        else if( strcmp((char *) name, "logo") == 0 )
        {
          elt->imagefilename = value;
        }
        else if( strcmp((char *) name, "arg") == 0 )
        {
          addArgument(elt, (char *) value);
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
* \brief prints the elements of a button structure
* \param *elt button structure that should be printed
*/
void printElements( buttons *elt )
{
  buttons *next;
  int i ;

  while(elt != NULL)
  {
    printf("Element\t\tValue\n");
    printf("-----\n");
    printf("labeltext:\t\t%s\n", elt->labeltext);
    printf("exec:\t\t%s\n", elt->executable);
    printf("logo:\t\t%s\n", elt->imagefilename); 
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
* \brief returns the total number of buttons
*/
int getNumberOfElements()
{
  return numberElts;
}

/**
* \brief relinguishes the memory occupied by button structures
* \param *elt first button structure
*/
void freeElements( buttons *elt )
{
  buttons *next;
  int i;
  while(elt != NULL)
  {
    printf("TEST:\n");
    fflush(stdout);
    next = elt->next;
    free((xmlChar *) elt->labeltext);
    free((xmlChar *) elt->executable);
    free((xmlChar *) elt->imagefilename);
    for (i = 0; i < elt->numArguments; i++)
    {
      free(elt->args[i]);
    }
    free(elt->args);
    free(elt);
    elt = next;
  }
}

/**
* \brief returns the buttons structure that contains the programs
* \return pointer to buttons structure
*/
buttons* getPrograms()
{
  return programs;
}

/**
* \brief returns the buttons structure that contains the actions
* \return pointer to buttons structure
*/
buttons* getActions()
{
  return actions;
}

void static processElements(const char* element_name, const char* group_element_name, xmlTextReaderPtr reader, buttons **elts)
{
  int ret = 1;
  xmlChar *name;
  int width;
  int height;
  buttons *prev;
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
      *elts = createElement();
      (*elts)->numArguments = 0;
      (*elts)->next = prev;
      
      prev = *elts;
      printf("processElements: %s\n", name);
      processElement(reader, *elts, element_name);
      numberElts++;
    }
    
    if( strcmp((char *) name, group_element_name) == 0 && xmlTextReaderNodeType(reader) == 15)
    {
      printf("processElements: %s\n", name);
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
* \brief load the configuration file and parses it to create the buttons structures for each program.
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
            processElements("program", "programs", reader, &programs);
          }
          if( strcmp((char *) name, "actions") == 0 && xmlTextReaderNodeType(reader) == 1)
          {
            processElements("action", "actions", reader, &actions);
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
