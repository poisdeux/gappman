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

static nm_elements* stati;
static nm_elements* actions;
static char *cache_location;

struct nm_element* create_nm_element()
{
  struct nm_element *elt;
  elt = (nm_elements *) malloc(sizeof(nm_elements));
  elt->numArguments = 0;
  elt->logosuccess = NULL;
  elt->logofail = NULL;
	elt->success = 0;
  elt->name = NULL;
  elt->exec = NULL;
  elt->next = NULL;
  elt->args = NULL;
	elt->pid = -1;
  return elt;
}

/**
* \brief add an argument to the argument list for elt
* \param *elt nm_element structure which should have its argument list expanded
* \param *argument argument that should be added to *elt->args
*/
static void add_argument(nm_elements *elt, char *argument)
{
  elt->numArguments++;
  elt->args = (char **) realloc(elt->args, ((elt->numArguments) * sizeof(char *)));
  elt->args[elt->numArguments - 1] = (char *) argument;
}

char* nm_get_cache_location()
{
	return cache_location;
}

/**
* \brief process an status element from the XML configuration file.
* \param reader the XMLtext reader pointing to the configuration file.
* \param *elt nm_element structure that will contain the status configuration values
*/
static void
process_nm_element(xmlTextReaderPtr reader, nm_elements *elt, const char* element_name ) {
    xmlChar *name, *value;
    int ret = 1;
    
    if (name == NULL)
	name = BAD_CAST "--";

    while ( ret == 1 )
    {

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
        else if( strcmp((char *) name, "exec") == 0 )
        {
          elt->exec = value;
        }
        else if( strcmp((char *) name, "logosuccess") == 0 )
        {
          elt->logosuccess = value;
        }
        else if( strcmp((char *) name, "logofail") == 0 )
        {
          elt->logofail = value;
        }
        else if( strcmp((char *) name, "arg") == 0 )
        {
          add_argument(elt, (char *) value);
        }
				else if( strcmp((char *) name, "success") == 0 )
        {
          elt->success = atoi(value);
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
* \brief relinguishes the memory occupied by nm_element structures
* \param *elt first nm_element structure
*/
void nm_free_elements( nm_elements *elt )
{
  nm_elements *next;
  int i;
	char** orient;

	if (elt != NULL)
	{
		free(elt->amount_of_elements);
		i = 0;
		while(elt != NULL)
		{
			free((xmlChar *) elt->name);
			free((xmlChar *) elt->exec);
			free((xmlChar *) elt->logosuccess);
			free((xmlChar *) elt->logofail);

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

nm_elements* nm_get_stati()
{
  return stati;
}

nm_elements* nm_get_actions()
{
  return actions;
}


/**
* \brief creates the nm_elements structures
* \param element_name name of the element being processed
* \param group_element_name name of the group the element belongs to. E.g stati or actions.
* \param reader the XML reader from libxml
* \param **elts pointer to the linked list of menu element structures.
* \return nothing. Use the **elts call by reference to retrieve the menu elements.
*/
void static process_nm_elements(const char* element_name, const char* group_element_name, xmlTextReaderPtr reader, nm_elements **elts)
{
  int ret = 1;
	int i;
  xmlChar *name;
	char* result;
	char* align;
  int *number_elts;
  nm_elements *prev;
  prev = NULL;

	number_elts = (int*) malloc (sizeof(int));
  
  while (ret)
  {
    ret = xmlTextReaderRead(reader);
    name = xmlTextReaderName(reader);

		// Parse new status or action and create a new nm_element for it.
    if( strcmp((char *) name, element_name) == 0 && xmlTextReaderNodeType(reader) == 1)
    {
      *elts = create_nm_element();
      (*elts)->numArguments = 0;
      (*elts)->next = prev;
      
      prev = *elts;
      process_nm_element(reader, *elts, element_name);
    	(*elts)->amount_of_elements = prev->amount_of_elements + 1;
    }
  }
}

int nm_load_conf(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;
    xmlChar *name;

    //Initialize
    stati = NULL;
    actions = NULL;
    cache_location = NULL;

    reader = xmlReaderForFile(filename, NULL, 0);
    if (reader != NULL) 
		{
        ret = xmlTextReaderRead(reader);

        while (ret == 1) 
				{
          name = xmlTextReaderName(reader);
          if( strcmp((char *) name, "stati") == 0 && xmlTextReaderNodeType(reader) == 1)
          {
            process_nm_elements("status", "stati", reader, &stati);
          }
          else if( strcmp((char *) name, "actions") == 0 && xmlTextReaderNodeType(reader) == 1)
          {
            process_nm_elements("action", "actions", reader, &actions);
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
            fprintf(stderr, "%s : failed to parse\n", filename);
        }
    } 
		else 
		{
        fprintf(stderr, "Unable to open %s\n", filename);
				return 1;
    }
 		 /**
     * Cleanup function for the XML library.
     */
    xmlCleanupParser();
		return 0;
}
