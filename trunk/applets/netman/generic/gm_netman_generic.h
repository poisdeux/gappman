/**
 * \file gm_netman_generic.h
 *
 *
 *
 * GPL v2
 *
 * \author Martijn Brekhof <m.brekhof@gmail.com>
 */

/*
 * \brief quark needed to create a GError structure for gm_netmand
 */
#define GM_NETMAND_ERROR g_quark_from_static_string ("gm-netmand-error-quark") 

typedef enum
{
	GM_NETMAND_FAILED_EXEC,	 //< failed to fork or execute command
	GM_NETMAND_FAILED_WAIT	 //< failed while waiting for exitcode of child
} GMError; ///< enumeration for generic netman failure codes
