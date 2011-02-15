/**
 * \file nm_parseconf.c
 *
 *
 *
 * GPL v2
 *
 * Authors:
 *   Martijn Brekhof <m.brekhof@gmail.com>
 */

#include "nm_parseconf.h"
#include <string.h>

static nm_elements *stati;
static nm_elements *actions;
static const char *cache_location;
static const char *logounavail;

/**
* \brief creates a new nm_element and initializes it.
* \param prev pointer to the previous nm_element if it exist otherwise it should be NULL.
* \return pointer to the new nm_element 
*/

struct nm_element *create_nm_element(nm_elements * prev)
{
	struct nm_element *elt;
	elt = (nm_elements *) malloc(sizeof(nm_elements));
	elt->running = FALSE;
	elt->numArguments = 0;
	elt->logosuccess = NULL;
	elt->logofail = NULL;
	elt->success = 0;
	elt->name = NULL;
	elt->exec = NULL;
	elt->next = prev;
	elt->args = NULL;
	elt->argv = NULL;
	elt->status = 1;			// default fail
	elt->prev_status = 1;		// default fail
	elt->pid = -1;
	elt->image_success = NULL;
	elt->image_fail = NULL;
	if (prev == NULL)
	{
		elt->amount_of_elements = (int *)malloc(sizeof(int));
		*(elt->amount_of_elements) = 1;
	}
	else
	{
		elt->amount_of_elements = prev->amount_of_elements;
		*(elt->amount_of_elements) = *(elt->amount_of_elements) + 1;
	}
	return elt;
}

/**
* \brief add an argument to the argument list for elt
* \param *elt nm_element structure which should have its argument list expanded
* \param *argument argument that should be added to *elt->args
*/
static void add_argument(nm_elements * elt, char *argument)
{
	elt->numArguments++;
	elt->args =
		(char **)realloc(elt->args,
						 ((elt->numArguments + 1) * sizeof(char *)));
	elt->args[elt->numArguments - 1] = argument;
	elt->args[elt->numArguments] = NULL;
}

const char *nm_get_filename_logounavail()
{
	return logounavail;
}

const char *nm_get_cache_location()
{
	return cache_location;
}

/**
* \brief process an status element from the XML configuration file.
* \param reader the XMLtext reader pointing to the configuration file.
* \param *elt nm_element structure that will contain the status configuration values
* \param element_name name of the XML element we need to parse. Used to determine when
*        end of the xml block is reached
*/
static void
process_nm_element(xmlTextReaderPtr reader, nm_elements * elt,
				   const char *element_name)
{
	xmlChar *name, *value;
	int ret = 1;

	name = NULL;

	while (ret == 1)
	{
		if (name == NULL)
			name = BAD_CAST "--";

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
			else if (strcmp((char *)name, "exec") == 0)
			{
				elt->exec = value;
			}
			else if (strcmp((char *)name, "logosuccess") == 0)
			{
				elt->logosuccess = value;
			}
			else if (strcmp((char *)name, "logofail") == 0)
			{
				elt->logofail = value;
			}
			else if (strcmp((char *)name, "arg") == 0)
			{
				add_argument(elt, (char *)value);
			}
			else if (strcmp((char *)name, "success") == 0)
			{
				elt->success = atoi((const char *)value);
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
* \brief relinguishes the memory occupied by nm_element structures
* \param *elt first nm_element structure
*/
void nm_free_elements(nm_elements * elt)
{
	nm_elements *next;
	int i;

	if (elt != NULL)
	{
		free(elt->amount_of_elements);
		i = 0;
		while (elt != NULL)
		{
			free((xmlChar *) elt->name);
			free((xmlChar *) elt->exec);
			free((xmlChar *) elt->logosuccess);
			free((xmlChar *) elt->logofail);

			for (i = 0; elt->args[i] != NULL; i++)
			{
				free(elt->args[i]);
			}
			free(elt->args[i]);
			free(elt->args);

			for (i = 0; elt->argv[i] != NULL; i++)
			{
				free(elt->argv[i]);
			}

			next = elt->next;
			free(elt);
			elt = next;
		}
	}
}

nm_elements *nm_get_stati()
{
	return stati;
}

nm_elements *nm_get_actions()
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
static void process_nm_elements(const char *element_name,
								const char *group_element_name,
								xmlTextReaderPtr reader, nm_elements ** elts)
{
	int ret = 1;
	xmlChar *name;
	nm_elements *prev;
	prev = NULL;

	while (ret)
	{
		ret = xmlTextReaderRead(reader);
		if (xmlTextReaderNodeType(reader) == 1)
		{
			name = xmlTextReaderName(reader);

			// Parse new status or action and create a new nm_element for it.
			if (strcmp((char *)name, element_name) == 0)
			{
				*elts = create_nm_element(prev);
				prev = *elts;
				process_nm_element(reader, *elts, element_name);
			}
		}
		else if (xmlTextReaderNodeType(reader) == 3)
		{
			if (strcmp((char *)name, "logounavail") == 0)
			{
				logounavail = (const char *)xmlTextReaderValue(reader);
			}
		}
		else if (xmlTextReaderNodeType(reader) == 15)
		{
			name = xmlTextReaderName(reader);
			if (strcmp((const char *)name, group_element_name) == 0)
			{
				ret = 0;
			}
		}
	}
}

int nm_load_conf(const char *filename)
{
	xmlTextReaderPtr reader;
	int ret;
	xmlChar *name;

	// Initialize
	stati = NULL;
	actions = NULL;
	cache_location = NULL;
	logounavail = NULL;

	reader = xmlReaderForFile(filename, NULL, 0);
	if (reader != NULL)
	{
		ret = xmlTextReaderRead(reader);
		while (ret == 1)
		{
			if (xmlTextReaderNodeType(reader) == 1)
			{
				name = xmlTextReaderName(reader);
				if (strcmp((char *)name, "stati") == 0)
				{
					process_nm_elements("status", "stati", reader, &stati);
				}
				else if (strcmp((char *)name, "actions") == 0)
				{
					process_nm_elements("action", "actions", reader, &actions);
				}
			}
			else if (xmlTextReaderNodeType(reader) == 3)
			{
				if (strcmp((const char *)name, "cachelocation") == 0)
				{
					cache_location = (const char *)xmlTextReaderValue(reader);
				}
			}
			ret = xmlTextReaderRead(reader);
		}

		/**
         * Free up the reader
        */
		xmlFreeTextReader(reader);
		if (ret != 0)
		{
			g_warning("failed to parse file %s\n", filename);
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
