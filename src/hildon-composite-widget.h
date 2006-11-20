/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
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

/*
 * @file
 *
 * Hildon composite widget includes all features which were not fitting into
 * the any current class. A new, separate widget was not created because of the 
 * amount of current features is low. Need for this kind of class was not known 
 * when the building of the class hierarchy began. When a new feature is added, 
 * need for a new class should be re-considered. To make this decision one 
 * should really consider all the common features which are needed in the 
 * hildon composite widgets. Class position (for hildon-composite-widget)
 * in the class hierarchy is between GtkContainer and any composite widget.
 *
 */

#ifndef __HILDON_COMPOSITE_WIDGET__
#define __HILDON_COMPOSITE_WIDGET__

#include <gtk/gtk.h>

G_BEGIN_DECLS

gboolean
hildon_composite_widget_focus( GtkWidget *widget, GtkDirectionType direction );

G_END_DECLS

#endif /*__HILDON_COMPOSITE_WIDGET__*/
