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
 
#ifndef __HILDON_WIZARD_DIALOG_H__
#define __HILDON_WIZARD_DIALOG_H__

#include<gtk/gtkwidget.h>
#include<gtk/gtkwindow.h>
#include<gtk/gtknotebook.h>
#include<gtk/gtkdialog.h>

G_BEGIN_DECLS
#define HILDON_TYPE_WIZARD_DIALOG ( hildon_wizard_dialog_get_type() )
#define HILDON_WIZARD_DIALOG(obj) (GTK_CHECK_CAST (obj,\
        HILDON_TYPE_WIZARD_DIALOG, HildonWizardDialog))
#define HILDON_WIZARD_DIALOG_CLASS(klass) (GTK_CHECK_CLASS_CAST ((klass),\
        HILDON_TYPE_WIZARD_DIALOG, HildonWizardDialogClass))
#define HILDON_IS_WIZARD_DIALOG(obj) (GTK_CHECK_TYPE (obj,\
        HILDON_TYPE_WIZARD_DIALOG))
#define HILDON_IS_WIZARD_DIALOG_CLASS(klass) \
        (GTK_CHECK_CLASS_TYPE ((klass), HILDON_TYPE_WIZARD_DIALOG))
typedef struct _HildonWizardDialog HildonWizardDialog;
typedef struct _HildonWizardDialogClass HildonWizardDialogClass;
typedef struct _HildonWizardDialogPrivate HildonWizardDialogPrivate;


/* button response IDs */
enum {
    HILDON_WIZARD_DIALOG_CANCEL,
    HILDON_WIZARD_DIALOG_PREVIOUS,
    HILDON_WIZARD_DIALOG_NEXT,
    HILDON_WIZARD_DIALOG_FINISH
};

struct _HildonWizardDialog {
    GtkDialog parent;
    HildonWizardDialogPrivate *priv;
};

struct _HildonWizardDialogClass {
    GtkDialogClass parent_class;
    void (*_gtk_reserved1) (void);
    void (*_gtk_reserved2) (void);
    void (*_gtk_reserved3) (void);
    void (*_gtk_reserved4) (void);
};


GType hildon_wizard_dialog_get_type(void) G_GNUC_CONST;

GtkWidget *hildon_wizard_dialog_new(GtkWindow * parent,
                                    const char *wizard_name,
                                    GtkNotebook * notebook);

G_END_DECLS
#endif
