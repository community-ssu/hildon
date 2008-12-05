/*
 * This file is a part of hildon
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *   Fixes: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
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
 
#ifndef                                         __HILDON_WIZARD_DIALOG_H__
#define                                         __HILDON_WIZARD_DIALOG_H__

#include                                        <gtk/gtk.h>

G_BEGIN_DECLS

#define                                         HILDON_TYPE_WIZARD_DIALOG \
                                                (hildon_wizard_dialog_get_type())

#define                                         HILDON_WIZARD_DIALOG(obj) (GTK_CHECK_CAST ((obj), \
                                                HILDON_TYPE_WIZARD_DIALOG, HildonWizardDialog))

#define                                         HILDON_WIZARD_DIALOG_CLASS(klass) \
                                                (GTK_CHECK_CLASS_CAST ((klass), \
                                                HILDON_TYPE_WIZARD_DIALOG, HildonWizardDialogClass))

#define                                         HILDON_IS_WIZARD_DIALOG(obj) \
                                                (GTK_CHECK_TYPE ((obj), \
                                                HILDON_TYPE_WIZARD_DIALOG))

#define                                         HILDON_IS_WIZARD_DIALOG_CLASS(klass) \
                                                (GTK_CHECK_CLASS_TYPE ((klass), \
                                                HILDON_TYPE_WIZARD_DIALOG))

typedef struct                                  _HildonWizardDialog HildonWizardDialog;

typedef struct                                  _HildonWizardDialogClass HildonWizardDialogClass;

/* button response IDs */

typedef enum 
{
  /* HILDON_WIZARD_DIALOG_CANCEL should be marked as deprecated */
    HILDON_WIZARD_DIALOG_CANCEL = GTK_RESPONSE_CANCEL,
    HILDON_WIZARD_DIALOG_PREVIOUS = 0,
    HILDON_WIZARD_DIALOG_NEXT,
    HILDON_WIZARD_DIALOG_FINISH
}                                               HildonWizardDialogResponse;

struct                                          _HildonWizardDialog 
{
    GtkDialog parent;
};

struct                                          _HildonWizardDialogClass 
{
    GtkDialogClass parent_class;
};

typedef gboolean (*HildonWizardDialogPageFunc) (GtkNotebook *notebook, gint current_page, gpointer data);

GType G_GNUC_CONST
hildon_wizard_dialog_get_type                   (void);

GtkWidget* 
hildon_wizard_dialog_new                        (GtkWindow *parent,
                                                 const char *wizard_name,
                                                 GtkNotebook *notebook);

void
hildon_wizard_dialog_set_forward_page_func      (HildonWizardDialog *dialog,
                                                 HildonWizardDialogPageFunc pagefunc,
                                                 gpointer data,
                                                 GDestroyNotify destroy);

G_END_DECLS

#endif                                          /* __HILDON_WIZARD_DIALOG_H__ */

