/* bind.h
 * Copyright (C) 2008 Alex Graveley
 * Copyright (C) 2010 Ulrik Sverdrup <ulrik.sverdrup@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * \file gm_keybinder.h
 * \brief Provides global keybinding functionality
 * 
 * Original code adjusted to match GAppMan's namespace
 */

#ifndef __GM_KEY_BINDER_H__
#define __GM_KEY_BINDER_H__

#include <glib.h>

G_BEGIN_DECLS

typedef void (* KeybinderHandler) (const char *keystring, void *user_data);

void gm_keybinder_init (void);

gboolean gm_keybinder_bind (const char *keystring,
                         KeybinderHandler  handler,
                         void *user_data);

void gm_keybinder_unbind (const char *keystring,
                       KeybinderHandler  handler);

guint32 gm_keybinder_get_current_event_time (void);

G_END_DECLS

#endif /* __GM_KEY_BINDER_H__ */

