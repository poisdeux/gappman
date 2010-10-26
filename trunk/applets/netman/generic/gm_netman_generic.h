/**
 * \file gm_netmand.c
 *
 *
 *
 * GPL v2
 *
 * \author Martijn Brekhof <m.brekhof@gmail.com>
 */

#define GM_NETMAND_ERROR g_quark_from_static_string ("gm-netmand-error-quark")

typedef enum
{
	GM_NETMAND_EXEC_FAILED	 //< failed to execute command
} GMError;
