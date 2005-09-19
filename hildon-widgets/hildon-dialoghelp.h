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

#ifndef __HILDON_DIALOG_HELP_H__
#define __HILDON_DIALOG_HELP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*#include <gtk/gtk.h>*/
#include <gtk/gtkdialog.h>

void gtk_dialog_help_enable(GtkDialog * dialog);
void gtk_dialog_help_disable(GtkDialog * dialog);

#ifdef __cplusplus
}
#endif /* __cplusplus */
#endif /* __DIALOG_HELP_H__ */
