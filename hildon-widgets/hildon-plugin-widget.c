/*
 * This file is part of hildon-libs
 *
 * Copyright (C) 2005, 2006 Nokia Corporation, all rights reserved.
 *
 * Author: Kuisma Salonen <kuisma.salonen@nokia.com>
 * Contact: Michael Dominic Kostrzewa <michael.kostrzewa@nokia.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; version 2.1 of
 * the License or any later version.
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
 * SECTION:hildon-plugin-widget
 * @short_description: A simple interface to load plugin based widgets. Not
 * compatible with GObject.
 *
 * #HildonPluginWidgetInfo is a struct containing information about loaded
 * module which contains code for a widget.
 */


#include <stdio.h>
#include <memory.h>
#include <string.h>

#include <gtk/gtk.h>


#include "hildon-plugin-widget.h"


#ifndef PLUGIN_DIR
#define PLUGIN_DIR      "/usr/lib/hildon-widgets"
#endif


struct HildonPluginWidgetInfo_ {
  GType base_type;

  gchar *filename;

  GModule *module;
  int refcount;

  GType (*get_type) ();
};


static gchar *hildon_plugin_filename(GType base_type, const gchar *name);

  /* WARNING: works properly only with ASCII */
static gchar *ascii_decapitalize_without_dashes(gchar *source);

static gchar *hildon_plugin_default_name(gchar *typename);

/**
 * hildon_plugin_info_initialize:
 * @base_type: a #GType representing parent type of object that will be
 * loaded.
 * @name: Name of child. To load default (or #GtkSettings defined), NULL
 * should be passed as name. To load specific child type, decapitalized name
 * should be passed here.
 * 
 * Creates a new #HildonPluginWidgetInfo structure and opens a module.
 *
 * The naming of child widgets (or objects) doesn't matter, but for plugins
 * the file names should be type
 * <decapitalized-parent-type-name-with-dashes>-<pluginname>.so where the
 * decapitalized type name would be for example for #GtkWidget gtk-widget.
 *
 * The name comes from name argument or from #GtkSettings where the variable
 * storing it is with name <ParentTypeName>-plugin, for #GtkWidget this would
 * be "GtkWidget-plugin". If nothing is defined in #GtkSettings, name
 * "default" is assumed. For this case there should be symlink to some child
 * type plugin named <parent-type-name>-default.so
 *
 * Returns: a #HildonPluginWidgetInfo struct pointer upon success, NULL if
 * failed.
 */
HildonPluginWidgetInfo *hildon_plugin_info_initialize(GType base_type, const gchar *name)
{
  HildonPluginWidgetInfo *ret;
  GModule *module;
  gchar *filename;


  if(!base_type) {
    return NULL;
  }


  filename = hildon_plugin_filename(base_type, name);
  g_return_val_if_fail (filename != NULL, NULL);


  module = g_module_open(filename, G_MODULE_BIND_LAZY | G_MODULE_BIND_LOCAL);
  if(!module) {
    g_warning ("Failed to load plugin for '%s' (filename: '%s')", name, filename);
    g_free(filename);
    return NULL;
  }


  ret = (HildonPluginWidgetInfo *)g_malloc0(sizeof(HildonPluginWidgetInfo) + strlen(filename) + 1);
  ret->filename = (gchar *)ret + sizeof(HildonPluginWidgetInfo);

  ret->base_type = base_type;

  ret->module = module;

  g_module_symbol(module, "export_type", (void **)&ret->get_type);

  memcpy(ret->filename, filename, strlen(filename));


  g_free(filename);


  return ret;
}

/**
 * hildon_plugin_info_construct_widget:
 * @info: pointer to a #HildonPluginWidgetInfo struct.
 * 
 * Creates instance of loaded type from module stored in
 * #HildonPluginWidgetInfo struct. Designed for loading types inherited from
 * GtkWidget, but could be basically any GTK+ type.
 *
 * Returns: a GtkWidget pointer to instance of loaded type.
 */
GtkWidget *hildon_plugin_info_construct_widget(HildonPluginWidgetInfo *info)
{
  g_return_val_if_fail (info != NULL, NULL);
  info->refcount++;


  return GTK_WIDGET(g_type_create_instance(info->get_type()));
}

/**
 * hildon_plugin_info_kill:
 * @info: a pointer to a #HildonPluginWidgetInfo struct that should be
 * destroyed.
 * 
 * Frees the plugin information structure and unloads the module.
 */
void hildon_plugin_info_kill(HildonPluginWidgetInfo *info)
{
  if(!info) {
    return;
  }


  g_module_close(info->module);


  g_free(info);
}


static gchar *hildon_plugin_filename(GType base_type, const gchar *name)
{
  gchar *ret, *name2, *plgbuf;
  gchar *typename = (gchar *)g_type_name(base_type);
  int retsize;


  plgbuf = ascii_decapitalize_without_dashes(typename);


  if(name) {
    name2 = g_strdup(name);
  } else {
    name2 = hildon_plugin_default_name(typename);
  }


  retsize = strlen(PLUGIN_DIR) + strlen(plgbuf) + strlen(name2) + 6;
  ret = (gchar *)g_malloc0(retsize);
  g_snprintf(ret, retsize, "%s/%s_%s.so", PLUGIN_DIR, plgbuf, name2);


  g_free(name2);
  g_free(plgbuf);


  return ret;
}

  /* possible speedup: pre-allocate more memory and ditch the first loop */
static gchar *ascii_decapitalize_without_dashes(gchar *source)
{
  gchar *ptr, *ret = g_strdup (source);


  for(ptr = ret; *ptr; ptr++) {
    if(*ptr >= 'A' && *ptr <= 'Z') {
      *ptr += 0x20;
    }
  }


  return ret;
}


static gchar *hildon_plugin_default_name(gchar *typename)
{
  GtkSettings *settings;
  gchar *ret, *val, *tmp;
  int tmplen;


  tmplen = strlen(typename) + strlen("-plugin") + 1;
  tmp = (gchar *)g_malloc0(tmplen);
  g_snprintf(tmp, tmplen, "%s-plugin", typename);


  gtk_settings_install_property(g_param_spec_string(tmp,
                                                    tmp,
                                                    "Plugin for this pecific widget",
                                                    NULL,
                                                    G_PARAM_READWRITE));

  settings = gtk_settings_get_default();

  g_object_get(G_OBJECT(settings), tmp, &val, NULL);

  g_free(tmp);


  if(val) {
    ret = (gchar *)g_malloc0(strlen(val)+1);
    memcpy(ret, val, strlen(val));
    g_free(val);
  } else {
    ret = (gchar *)g_malloc0(strlen("default")+1);


    g_snprintf(ret, strlen("default")+1, "default");
  }


  return ret;
}
