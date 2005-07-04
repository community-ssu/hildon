/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005 Nokia Corporation.
 *
 * Contact: Luc Pionchon <luc.pionchon@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#ifndef __HILDON_FIND_REPLACE_DIALOG_H__
#define __HILDON_FIND_REPLACE_DIALOG_H__

#include <gtk/gtkdialog.h>

G_BEGIN_DECLS
#define HILDON_TYPE_FIND_REPLACE_DIALOG \
  ( hildon_find_replace_dialog_get_type() )
#define HILDON_FIND_REPLACE_DIALOG(obj) \
  (GTK_CHECK_CAST (obj, HILDON_TYPE_FIND_REPLACE_DIALOG,\
   HildonFindReplaceDialog))
#define HILDON_FIND_REPLACE_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_FIND_REPLACE_DIALOG, \
  HildonFindReplaceDialogClass))
#define HILDON_IS_FIND_REPLACE_DIALOG(obj) \
  (GTK_CHECK_TYPE (obj, HILDON_TYPE_FIND_REPLACE_DIALOG))
#define HILDON_IS_FIND_REPLACE_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_FIND_REPLACE_DIALOG))
typedef struct _HildonFindReplaceDialog HildonFindReplaceDialog;
typedef struct _HildonFindReplaceDialogClass HildonFindReplaceDialogClass;

/**
 * HildonReplaceType:
 * @HILDON_REPLACE_NORMAL: Replace mode is available.
 * @HILDON_REPLACE_READ_ONLY: Replace button is inactive. This is used when
 * content is read only.
 * @HILDON_REPLACE_IMMUTABLE: There is no replace button at all. This is
 * used (for example) for web pages.
 *
 * These values defines if replace button in dialog is available.
 */
typedef enum {
    HILDON_REPLACE_NORMAL = 0,
    HILDON_REPLACE_READ_ONLY,
    HILDON_REPLACE_IMMUTABLE
} HildonReplaceType;

struct _HildonFindReplaceDialog {
    GtkDialog parent;
};

struct _HildonFindReplaceDialogClass {
    GtkDialogClass parent_class;
};

GType hildon_find_replace_dialog_get_type(void);

GtkWidget *hildon_find_replace_dialog_new(GtkWindow * parent,
                                          HildonReplaceType type);
GtkWidget *hildon_find_replace_dialog_new_with_options(GtkWindow * parent,
                                                       GtkDialog * dialog,
                                                       HildonReplaceType type);

const gchar *hildon_find_replace_dialog_get_text(HildonFindReplaceDialog *
                                                 dialog);
const gchar
    *hildon_find_replace_dialog_get_new_text(HildonFindReplaceDialog *
                                             dialog);

void hildon_find_replace_dialog_set_text(HildonFindReplaceDialog * dialog,
                                         const gchar * text);
void hildon_find_replace_dialog_set_new_text(HildonFindReplaceDialog *
                                             dialog,
                                             const gchar * new_text);

G_END_DECLS
#endif
