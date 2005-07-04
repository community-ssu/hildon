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

#ifndef __HILDON_INSERT_OBJECT_DIALOG_H__
#define __HILDON_INSERT_OBJECT_DIALOG_H__

#include <gtk/gtkdialog.h>

/*
 * Headers for GnomeVFS used for mime types
 *
 */
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-init.h>
#include <libgnomevfs/gnome-vfs-ops.h>
#include <libgnomevfs/gnome-vfs-mime-handlers.h>
#include <libgnomevfs/gnome-vfs-mime.h>
#include <libgnomevfs/gnome-vfs-utils.h>
#include <libgnomevfs/gnome-vfs-mime-info.h>

#define HILDON_RESPONSE_INSERT_EXISTING 12345


G_BEGIN_DECLS
#define HILDON_TYPE_INSERT_OBJECT_DIALOG \
  ( hildon_insert_object_dialog_get_type() )
#define HILDON_INSERT_OBJECT_DIALOG(obj) \
  (GTK_CHECK_CAST (obj, HILDON_TYPE_INSERT_OBJECT_DIALOG,\
   HildonInsertObjectDialog))
#define HILDON_INSERT_OBJECT_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_CAST ((klass), HILDON_TYPE_INSERT_OBJECT_DIALOG, \
  HildonInsertObjectDialogClass))
#define HILDON_IS_INSERT_OBJECT_DIALOG(obj) \
  (GTK_CHECK_TYPE (obj, HILDON_TYPE_INSERT_OBJECT_DIALOG))
#define HILDON_IS_INSERT_OBJECT_DIALOG_CLASS(klass) \
  (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_INSERT_OBJECT_DIALOG))
typedef struct _HildonInsertObjectDialog HildonInsertObjectDialog;
typedef struct _HildonInsertObjectDialogClass
    HildonInsertObjectDialogClass;

/**
 * HildonInsertObjectDialogPrivate:
 *
 * This structure contains just private data and should not be
 * accessed directly.
 */
typedef struct _HildonInsertObjectDialogPrivate
    HildonInsertObjectDialogPrivate;

struct _HildonInsertObjectDialog {
    GtkDialog parent;
    HildonInsertObjectDialogPrivate *priv;
};

struct _HildonInsertObjectDialogClass {
    GtkDialogClass parent_class;
};

GType hildon_insert_object_dialog_get_type(void);

GtkWidget *hildon_insert_object_dialog_new(GtkWindow * parent);

/* Use return value of gtk_dialog_run to decide
   which one of the following to call */
const gchar *hildon_insert_object_dialog_get_name(HildonInsertObjectDialog
                                                  * dialog);
const gchar
    *hildon_insert_object_dialog_get_mime_type(HildonInsertObjectDialog *
                                               dialog);

G_END_DECLS
#endif
