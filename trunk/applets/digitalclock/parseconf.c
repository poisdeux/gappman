/**
 * \file applets/digitalclock/parseconf.c
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

struct element config;

static void
process_element(xmlTextReaderPtr reader, const char *element_name)
{
	xmlChar *name, *value;
	int ret = 1;

	name = NULL;

	while( TRUE )
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
			if (strcmp((char *)name, element_name) == 0)
			{
				config.show_date = atoi((const char *) value);
			}
		}

		if (strcmp((char *)xmlTextReaderName(reader), element_name) == 0
			&& xmlTextReaderNodeType(reader) == 15)
		{
			break;	
		}
		
		ret = xmlTextReaderRead(reader);
	}
}

int load_conf(const char *filename)
{
	xmlTextReaderPtr reader;
	int ret;
	xmlChar *name;

	reader = xmlReaderForFile(filename, NULL, 0);
	if (reader == NULL)
	{
		g_warning("Unable to open %s\n", filename);
		return 1;
	}
	
	while ( TRUE )
	{
		ret = xmlTextReaderRead(reader);
		if( ret != 1 )
			break;

		if (xmlTextReaderNodeType(reader) == 1)
		{
			name = xmlTextReaderName(reader);
			if (strcmp((char *)name, "digitalclock") == 0)
			{
				process_element(reader, "show_date");
			}
		}
	}

	xmlFreeTextReader(reader);
	xmlCleanupParser();
	return 0;
}

int get_show_date()
{
	return config.show_date;
}
