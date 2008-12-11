/*
 * This file is a part of hildon
 *
 * Copyright (C) 2007 Nokia Corporation, all rights reserved.
 *
 * Contact: Rodrigo Novo <rodrigo.novo@nokia.com>
 * Author: Xan Lopez <xan.lopez@nokia.com>
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

/**
 * SECTION:hildon-bread-crumb
 * @short_description: Interface for elements in a #HildonBreadCrumbTrail
 *
 * #HildonBreadCrumb is an interface for creating new types of items
 * for the #HildonBreadCrumbTrail widget.
 */

#undef HILDON_DISABLE_DEPRECATED

#include "hildon-bread-crumb.h"

static void hildon_bread_crumb_base_init (gpointer g_class);

GType
hildon_bread_crumb_get_type (void)
{
  static GType bread_crumb_type = 0;

  if (G_UNLIKELY (bread_crumb_type == 0))
    {
      const GTypeInfo bread_crumb_info =
        {
          sizeof (HildonBreadCrumbIface), /* class_size */
          hildon_bread_crumb_base_init, /* base_init */
          NULL, /* base_finalize */
          NULL,
          NULL, /* class_finalize */
          NULL, /* class_data */
          0,
          0,
          NULL
        };
      
      bread_crumb_type =
        g_type_register_static (G_TYPE_INTERFACE, "HildonBreadCrumb",
                                &bread_crumb_info, 0);

      g_type_interface_add_prerequisite (bread_crumb_type, GTK_TYPE_WIDGET);
    }

  return bread_crumb_type;
}

static void
hildon_bread_crumb_base_init (gpointer g_class)
{
  static gboolean initialized = FALSE;

  if (initialized == FALSE)
    {
      g_signal_new ("crumb-activated",
                    HILDON_TYPE_BREAD_CRUMB,
                    G_SIGNAL_RUN_LAST,
                    G_STRUCT_OFFSET (HildonBreadCrumbIface, crumb_activated),
                    NULL, NULL,
                    g_cclosure_marshal_VOID__VOID,
                    G_TYPE_NONE, 0);

      initialized = TRUE;
    }
}

/**
 * hildon_bread_crumb_get_natural_size:
 * @bread_crumb: A #HildonBreadCrumb
 * @width: location to store the natural width request of the @bread_crumb
 * @height: location to store the natural height request of the @bread_crumb
 *
 * Function to ask the @bread_crumb its preferred width and height requisition.
 * Natural size is different to size_request in that it's not the minimum space
 * the widget needs, but the ideal space it'd like to be allocated. For example,
 * in the case of a label the natural size would be the width and height needed
 * to show the full label without line breaks.
 *
 **/

void
hildon_bread_crumb_get_natural_size (HildonBreadCrumb *bread_crumb,
                                     gint *width,
                                     gint *height)
{
  g_return_if_fail (HILDON_IS_BREAD_CRUMB (bread_crumb));

  (* HILDON_BREAD_CRUMB_GET_IFACE (bread_crumb)->get_natural_size) (bread_crumb, width, height);
}

/**
 * hildon_bread_crumb_activated:
 * @bread_crumb: a #HildonBreadCrumb
 *
 * Emits the "crumb-activated" signal. The signal is meant to indicate that
 * the user has interacted with the bread crumb.
 **/

void
hildon_bread_crumb_activated (HildonBreadCrumb *bread_crumb)
{
  g_return_if_fail (HILDON_IS_BREAD_CRUMB (bread_crumb));

  g_signal_emit_by_name (bread_crumb, "crumb-activated");
}


