/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *   Fixes: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License.
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
 
#ifndef                                         __HILDON_WIZARD_DIALOG_PRIVATE_H__
#define                                         __HILDON_WIZARD_DIALOG_PRIVATE_H__

G_BEGIN_DECLS

#define                                         HILDON_WIZARD_DIALOG_GET_PRIVATE(obj)\
                                                (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
                                                HILDON_TYPE_WIZARD_DIALOG, \
                                                HildonWizardDialogPrivate));

typedef struct                                  _HildonWizardDialogPrivate HildonWizardDialogPrivate;

struct                                          _HildonWizardDialogPrivate 
{
    gchar *wizard_name;
    GtkNotebook *notebook;
    GtkBox *box;
    GtkWidget *image;
    gboolean autotitle;
};

G_END_DECLS

#endif                                          /* __HILDON_WIZARD_DIALOG_PRIVATE_H__ */

