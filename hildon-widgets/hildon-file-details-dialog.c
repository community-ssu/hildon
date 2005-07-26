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

/* @file hildon-file-details-dialog.c 
 * 
 * This file contains API for Hildon File Details dialog.
 * 
 */

#include <gtk/gtkcheckbutton.h>
#include <gtk/gtklabel.h>
#include <gtk/gtknotebook.h>
#include <gtk/gtkhbox.h>
#include <gtk/gtkvbox.h>
#include <gtk/gtkimage.h>
#include <gtk/gtkscrolledwindow.h>
#include <time.h>
#include <libintl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

#include <hildon-widgets/hildon-caption.h>
#include <hildon-widgets/hildon-file-system-model.h>
#include <hildon-widgets/gtk-infoprint.h>
#include <hildon-widgets/hildon-defines.h>
#include "hildon-file-details-dialog.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define _(String) dgettext(PACKAGE, String)

enum
{
  PROP_SHOW_TABS = 1,
  PROP_ADDITIONAL_TAB,
  PROP_ADDITIONAL_TAB_LABEL,
  PROP_MODEL
};

struct _HildonFileDetailsDialogPrivate {
    GtkNotebook *notebook;

    GtkWidget *file_location, *file_name;
    GtkWidget *file_type, *file_size;
    GtkWidget *file_date, *file_time;
    GtkWidget *file_readonly, *file_device;
    GtkWidget *file_location_image, *file_device_image;
    GtkWidget *ok_button;

    GtkTreeRowReference *active_file;
    gboolean checkbox_original_state;
    gulong toggle_handler;

    /* Properties */
    HildonFileSystemModel *model;
    GtkWidget *tab_label;
};

static void
hildon_file_details_dialog_class_init(HildonFileDetailsDialogClass *
                                      klass);
static void hildon_file_details_dialog_init(HildonFileDetailsDialog *
                                            filedetailsdialog);
static void hildon_file_details_dialog_finalize(GObject * object);

static void
hildon_file_details_dialog_set_property( GObject *object, guint param_id,
			                                   const GValue *value,
                                         GParamSpec *pspec );
static void
hildon_file_details_dialog_get_property( GObject *object, guint param_id,
			                                   GValue *value, GParamSpec *pspec );
static void 
hildon_file_details_dialog_response(GtkDialog *dialog, gint response_id);

static GtkDialogClass *file_details_dialog_parent_class = NULL;

GType hildon_file_details_dialog_get_type(void)
{
    static GType file_details_dialog_type = 0;

    if (!file_details_dialog_type) {
        static const GTypeInfo file_details_dialog_info = {
            sizeof(HildonFileDetailsDialogClass),
            NULL,       /* base_init */
            NULL,       /* base_finalize */
            (GClassInitFunc) hildon_file_details_dialog_class_init,
            NULL,       /* class_finalize */
            NULL,       /* class_data */
            sizeof(HildonFileDetailsDialog),
            0,  /* n_preallocs */
            (GInstanceInitFunc) hildon_file_details_dialog_init,
        };

        file_details_dialog_type =
            g_type_register_static(GTK_TYPE_DIALOG,
                                   "HildonFileDetailsDialog",
                                   &file_details_dialog_info,
                                   0);
    }

    return file_details_dialog_type;
}

static void change_state(HildonFileDetailsDialog *self, gboolean readonly)
{
  GtkTreeIter iter;

  g_return_if_fail(HILDON_IS_FILE_DETAILS_DIALOG(self));

  if (hildon_file_details_dialog_get_file_iter(self, &iter))
  {  
    gchar *path;
    struct stat buf;

    gtk_tree_model_get(GTK_TREE_MODEL(self->priv->model), &iter, 
      HILDON_FILE_SYSTEM_MODEL_COLUMN_LOCAL_PATH, &path, -1);    

    /* Here we should set the read only state */
    if (stat(path, &buf) == 0)
    {
      mode_t perm = (readonly ? 
                        buf.st_mode & ~(S_IWUSR | S_IWGRP | S_IWOTH) : 
                        buf.st_mode | (S_IWUSR | S_IWGRP));

      (void) chmod(path, perm);
    }

    /* No errors are defined in the specs, but the previous operations can still fail */
    if (errno)
      gtk_infoprint(GTK_WINDOW(self), g_strerror(errno));
    
    g_free(path);
  }
  else
    g_assert_not_reached();
}

/* Cancel changes if read-only is changed */
static void 
hildon_file_details_dialog_response(GtkDialog *dialog, gint response_id)
{
    if (response_id == GTK_RESPONSE_CANCEL)
    {
      HildonFileDetailsDialog *self;
      gboolean state;

      self = HILDON_FILE_DETAILS_DIALOG(dialog);  
      state = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self->priv->file_readonly));

      if (state != self->priv->checkbox_original_state)
        change_state(self, self->priv->checkbox_original_state);
    }
}

static void
hildon_file_details_dialog_read_only_toggled(GtkWidget *widget, gpointer data)
{
  change_state(HILDON_FILE_DETAILS_DIALOG(data), 
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}

static void 
hildon_file_details_dialog_map(GtkWidget *widget)
{
  HildonFileDetailsDialogPrivate *priv;

  priv = HILDON_FILE_DETAILS_DIALOG(widget)->priv;

  GTK_WIDGET_CLASS(file_details_dialog_parent_class)->map(widget);

  if (gtk_notebook_get_show_tabs(priv->notebook))
  {
    gtk_notebook_set_current_page(priv->notebook, 0);
    gtk_widget_grab_focus(GTK_WIDGET(priv->notebook));
  }
  else
    gtk_widget_grab_focus(priv->ok_button);
}

static void
hildon_file_details_dialog_class_init(HildonFileDetailsDialogClass * klass)
{
    GObjectClass *gobject_class;

    file_details_dialog_parent_class = g_type_class_peek_parent(klass);
    gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = hildon_file_details_dialog_finalize;
    gobject_class->get_property = hildon_file_details_dialog_get_property;
    gobject_class->set_property = hildon_file_details_dialog_set_property;
    GTK_WIDGET_CLASS(klass)->map = hildon_file_details_dialog_map;
    GTK_DIALOG_CLASS(klass)->response = hildon_file_details_dialog_response;

    g_type_class_add_private(klass, sizeof(HildonFileDetailsDialogPrivate));

  /**
   * HildonFileDetailsDialog:additional_tab:
   *
   * This is a place for an additional tab.
   */
  g_object_class_install_property( gobject_class, PROP_ADDITIONAL_TAB,
                                   g_param_spec_object("additional-tab",
                                   "Additional tab",
                                   "Tab to show additinal information",
						       GTK_TYPE_WIDGET, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  /**
   * HildonFileDetailsDialog:show_tabs:
   *
   * Do we want to show the tab labels.
   */
  g_object_class_install_property( gobject_class, PROP_SHOW_TABS,
                                   g_param_spec_boolean("show-tabs",
                                   "Show tab labels",
                                   "Do we want to show the tab label.",
                                   FALSE, G_PARAM_READWRITE | G_PARAM_CONSTRUCT));
  /**
   * HildonFileDetailsDialog:additional_tab_label:
   *
   * 
   */
  g_object_class_install_property( gobject_class, PROP_ADDITIONAL_TAB_LABEL,
                                   g_param_spec_string("additional-tab-label",
                                   "Additional tab label",
                                   "Label to the additional tab",
                                   NULL, G_PARAM_READWRITE));

  g_object_class_install_property( gobject_class, PROP_MODEL,
                 g_param_spec_object("model", "Model", 
                 "HildonFileSystemModel to use when fetching information",
                 HILDON_TYPE_FILE_SYSTEM_MODEL, G_PARAM_READWRITE));
}

/* This handler is needed to correctly position the scrollbar down. 
   It doesn't happen automatically... */
static gboolean 
handle_focus(GtkWidget *widget, GtkDirectionType *dir, gpointer data)
{
  if (!GTK_WIDGET_HAS_FOCUS(widget))
  {
    GtkAdjustment *adj;
    adj = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(data));
    gtk_adjustment_set_value(adj, adj->upper - adj->page_size);
  }

  return FALSE;
}

static void
hildon_file_details_dialog_init(HildonFileDetailsDialog *self)
{
    GtkWidget *caption_location, *caption_name, *caption_type;
    GtkWidget *caption_date, *caption_size, *caption_time;
    GtkWidget *caption_read, *caption_device;
    GtkWidget *hbox_location, *hbox_device;
    GtkWidget *vbox;
    GtkWidget *scroll;
    GtkSizeGroup *group;
    GdkGeometry geometry;

    HildonFileDetailsDialogPrivate *priv;

    self->priv = priv =
      G_TYPE_INSTANCE_GET_PRIVATE(self, \
          HILDON_TYPE_FILE_DETAILS_DIALOG, HildonFileDetailsDialogPrivate);

    priv->notebook = GTK_NOTEBOOK(gtk_notebook_new());
    scroll = gtk_scrolled_window_new(NULL, NULL);
    vbox = gtk_vbox_new(FALSE, 0);
    group = gtk_size_group_new(GTK_SIZE_GROUP_BOTH);
    
    priv->tab_label = gtk_label_new(_("sfil_ti_notebook_file"));
    g_object_ref(priv->tab_label);
    gtk_object_sink(GTK_OBJECT(priv->tab_label));
    gtk_widget_show(priv->tab_label);

    priv->file_device = g_object_new(GTK_TYPE_LABEL, "xalign", 0.0f, NULL);
    priv->file_location = g_object_new(GTK_TYPE_LABEL, "xalign", 0.0f, NULL);
    priv->file_name = g_object_new(GTK_TYPE_LABEL, "xalign", 0.0f, NULL);
    priv->file_type = g_object_new(GTK_TYPE_LABEL, "xalign", 0.0f, NULL);
    priv->file_size = g_object_new(GTK_TYPE_LABEL, "xalign", 0.0f, NULL);
    priv->file_date = g_object_new(GTK_TYPE_LABEL,"xalign", 0.0f, NULL);
    priv->file_time = g_object_new(GTK_TYPE_LABEL, "xalign", 0.0f, NULL);
    priv->file_readonly = gtk_check_button_new();

    hbox_location = gtk_hbox_new(FALSE, HILDON_MARGIN_DEFAULT);
    hbox_device = gtk_hbox_new(FALSE, HILDON_MARGIN_DEFAULT);

    priv->file_location_image = gtk_image_new();
    priv->file_device_image = gtk_image_new();

    gtk_box_pack_start(GTK_BOX(hbox_location), priv->file_location_image, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_location), priv->file_location, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_device), priv->file_device_image, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox_device), priv->file_device, TRUE, TRUE, 0);

    /* Tab one */
    caption_name = hildon_caption_new(group, _("ckdg_fi_properties_name_prompt"), 
      priv->file_name, NULL, HILDON_CAPTION_OPTIONAL);
    caption_type = hildon_caption_new(group, _("ckdg_fi_properties_type_prompt"), 
      priv->file_type, NULL, HILDON_CAPTION_OPTIONAL);
    caption_location = hildon_caption_new(group, _("sfil_fi_properties_location_prompt"), 
      hbox_location, NULL, HILDON_CAPTION_OPTIONAL);
    caption_device = hildon_caption_new(group, _("sfil_fi_properties_device_prompt"), 
      hbox_device, NULL, HILDON_CAPTION_OPTIONAL);
    caption_date = hildon_caption_new(group, _("ckdg_fi_properties_date_prompt"), 
      priv->file_date, NULL, HILDON_CAPTION_OPTIONAL);
    caption_time = hildon_caption_new(group, _("ckdg_fi_properties_time_prompt"), 
      priv->file_time, NULL, HILDON_CAPTION_OPTIONAL);
    caption_size = hildon_caption_new(group, _("ckdg_fi_properties_size_prompt"), 
      priv->file_size, NULL, HILDON_CAPTION_OPTIONAL);
    caption_read = hildon_caption_new(group, _("ckdg_fi_properties_read_only"), 
      priv->file_readonly, NULL, HILDON_CAPTION_OPTIONAL);

    hildon_caption_set_separator(HILDON_CAPTION(caption_name), "");
    hildon_caption_set_separator(HILDON_CAPTION(caption_type), "");
    hildon_caption_set_separator(HILDON_CAPTION(caption_location), "");
    hildon_caption_set_separator(HILDON_CAPTION(caption_device), "");
    hildon_caption_set_separator(HILDON_CAPTION(caption_date), "");
    hildon_caption_set_separator(HILDON_CAPTION(caption_time), "");
    hildon_caption_set_separator(HILDON_CAPTION(caption_size), "");
    hildon_caption_set_separator(HILDON_CAPTION(caption_read), "");

    g_object_unref(group);

    gtk_box_pack_start(GTK_BOX(vbox), caption_name, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), caption_type, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), caption_location, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), caption_device, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), caption_date, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), caption_time, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), caption_size, FALSE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), caption_read, FALSE, TRUE, 0);

    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll), vbox);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll), 
        GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_set_border_width(GTK_CONTAINER(scroll), 
        HILDON_MARGIN_DEFAULT);
    /* Both scrolled window and viewport have separate shadows... */
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scroll),
        GTK_SHADOW_NONE);
    gtk_viewport_set_shadow_type(
        GTK_VIEWPORT(gtk_bin_get_child(GTK_BIN(scroll))),
        GTK_SHADOW_NONE);

    /* Populate dialog */
    gtk_notebook_append_page(priv->notebook, scroll,
			     gtk_label_new(_("sfil_ti_notebook_common")));

    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(self)->vbox),
                       GTK_WIDGET(priv->notebook), TRUE, TRUE, 0);

    /* From widget specs, generic dialog size */
    geometry.min_width = 133;
    geometry.max_width = 602;
    /* Scrolled windows do not ask space for whole contents in size_request.
       So, we must force the dialog to have larger than minimum size */
    geometry.min_height = 240 + (2 * HILDON_MARGIN_DEFAULT);
    geometry.max_height = 240 + (2 * HILDON_MARGIN_DEFAULT);

    gtk_window_set_geometry_hints(GTK_WINDOW(self),
				  GTK_WIDGET(priv->notebook), &geometry,
				  GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE);

    gtk_widget_show_all(GTK_WIDGET(priv->notebook));
    priv->ok_button = gtk_dialog_add_button(GTK_DIALOG(self),
                          _("sfil_bd_filetype_details_dialog_ok"),
                          GTK_RESPONSE_OK);
    gtk_dialog_add_button(GTK_DIALOG(self),
                          _("sfil_bd_filetype_details_dialog_cancel"),
                          GTK_RESPONSE_CANCEL);
    gtk_dialog_set_default_response(GTK_DIALOG(self),
                          GTK_RESPONSE_OK);

    priv->toggle_handler = g_signal_connect(priv->file_readonly, "toggled", 
      G_CALLBACK(hildon_file_details_dialog_read_only_toggled), 
      self);
    g_signal_connect(priv->file_readonly, "focus",
      G_CALLBACK(handle_focus), scroll);
}

static void
hildon_file_details_dialog_set_property( GObject *object, guint param_id,
			                                   const GValue *value,
                                         GParamSpec *pspec )
{
  HildonFileDetailsDialogPrivate *priv;
  GtkNotebook *notebook;
  GtkLabel *label;

  priv = HILDON_FILE_DETAILS_DIALOG(object)->priv;
  notebook = priv->notebook;
  label = GTK_LABEL(priv->tab_label);

  switch( param_id ) 
  {
    case PROP_SHOW_TABS:
    {
      gtk_notebook_set_show_tabs(notebook, g_value_get_boolean(value));
      gtk_notebook_set_show_border(notebook, g_value_get_boolean(value));
      break;
    }
    case PROP_ADDITIONAL_TAB:
    {
      GtkWidget *widget = g_value_get_object(value);
      GtkWidget *sw = gtk_scrolled_window_new(NULL, NULL);
      
      if (gtk_notebook_get_n_pages(notebook) == 2)
        gtk_notebook_remove_page(notebook, 1);

      if (widget == NULL)
      {
        widget = g_object_new(GTK_TYPE_LABEL, 
            "label", _("sfil_ia_filetype_no_details"), "yalign", 0.0f, NULL);
        gtk_widget_show(widget);
      }

      gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sw),
				     GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
      gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(sw), widget);
      gtk_viewport_set_shadow_type(GTK_VIEWPORT(GTK_BIN(sw)->child),
                                   GTK_SHADOW_NONE);
      gtk_widget_show_all(sw);
      gtk_notebook_append_page(notebook, sw, priv->tab_label);
      gtk_notebook_set_current_page(notebook, 0);
      break;
    }
    case PROP_ADDITIONAL_TAB_LABEL:
      gtk_label_set_text(label, g_value_get_string(value));
      break;
    case PROP_MODEL:
    {
      HildonFileSystemModel *new_model = g_value_get_object(value);
      if (new_model != priv->model)
      {
        if (G_IS_OBJECT(priv->model))
          g_object_unref(priv->model);
        priv->model = new_model;
        if (new_model)
          g_object_ref(new_model);
      }  
      break;
    }
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
  }
}

static void
hildon_file_details_dialog_get_property( GObject *object, guint param_id,
			                                   GValue *value, GParamSpec *pspec )
{
  HildonFileDetailsDialogPrivate *priv;

  priv = HILDON_FILE_DETAILS_DIALOG(object)->priv;

  switch (param_id) 
  {
    case PROP_SHOW_TABS:
      g_value_set_boolean(value, gtk_notebook_get_show_tabs(priv->notebook));
      break;
    case PROP_ADDITIONAL_TAB:
      g_assert(gtk_notebook_get_n_pages(priv->notebook) == 2);        
      g_value_set_object(value, gtk_notebook_get_nth_page(priv->notebook, 1));
      break;
    case PROP_ADDITIONAL_TAB_LABEL:
      g_value_set_string(value, gtk_label_get_text(GTK_LABEL(priv->tab_label)));
      break;
    case PROP_MODEL:
      g_value_set_object(value, priv->model);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, param_id, pspec);
      break;
  }
}

static void hildon_file_details_dialog_finalize(GObject * object)
{
  HildonFileDetailsDialogPrivate *priv;

  g_return_if_fail(HILDON_IS_FILE_DETAILS_DIALOG(object));

  priv = HILDON_FILE_DETAILS_DIALOG(object)->priv;
  if (G_IS_OBJECT(priv->model))
    g_object_unref(priv->model);
  if (G_IS_OBJECT(priv->tab_label))
    g_object_unref(priv->tab_label);
  if (priv->active_file)
    gtk_tree_row_reference_free(priv->active_file);
    
  G_OBJECT_CLASS(file_details_dialog_parent_class)->finalize(object);
}

/*******************/
/* Public functions */
/*******************/

/**
 * hildon_file_details_dialog_new:
 * @parent: the parent window.
 * @filename: the filename.
 *
 * Creates a new #hildon_file_details_dialog AND new underlying 
 * HildonFileSystemModel. Be carefull with #filename
 * parameter: You don't get any notification if something fails.
 * THIS FUNCTION IS DEPRICATED AND PROVIDED ONLY FOR
 * BACKWARDS COMPABILITY.
 *
 * Returns: a new #HildonFileDetailsDialog.
 */
#ifndef HILDON_DISABLE_DEPRECATED
GtkWidget *hildon_file_details_dialog_new(GtkWindow * parent,
                                          const gchar * filename)
{
  HildonFileDetailsDialog *dialog;
  HildonFileSystemModel *model;
  GtkTreeIter iter;

  model = g_object_new(HILDON_TYPE_FILE_SYSTEM_MODEL, NULL);

  dialog =
        g_object_new(HILDON_TYPE_FILE_DETAILS_DIALOG, 
          "has-separator", FALSE, "title", _("sfil_ti_file_details"), 
          "model", model, NULL);

  if (filename && filename[0] && 
    hildon_file_system_model_load_local_path(dialog->priv->model, filename, &iter))
      hildon_file_details_dialog_set_file_iter(dialog, &iter);

  if (parent)
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

  return GTK_WIDGET(dialog);
}
#endif
/**
 * hildon_file_details_dialog_new_with_model:
 * @parent: the parent window.
 * @model: a #HildonFileSystemModel object used to fetch data.
 *
 * This is the preferred way to create #HildonFileDetailsDialog.
 * You can use a shared model structure to save loading times
 * (because you probably already have one at your disposal).
 *
 * Returns: a new #HildonFileDetailsDialog.
 */
GtkWidget *hildon_file_details_dialog_new_with_model(GtkWindow *parent,
  HildonFileSystemModel *model)
{
  GtkWidget *dialog;

  dialog = g_object_new(HILDON_TYPE_FILE_DETAILS_DIALOG,
    "has-separator", FALSE, "title", _("sfil_ti_file_details"), 
    "model", model, NULL);

  if (parent)
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);

  return dialog;
}

/**
 * hildon_file_details_dialog_set_file_iter:
 * @self: a #HildonFileDetailsDialog.
 * @iter: a #GtkTreeIter pointing to desired file.
 *
 * Sets the dialog to display information about a file defined by
 * given iterator.
 */
void hildon_file_details_dialog_set_file_iter(HildonFileDetailsDialog *self, GtkTreeIter *iter)
{
  GtkTreeModel *model;
  GtkTreePath *path;
  GtkTreeIter temp_iter, parent_iter;
  gchar *name, *mime, *local_path;
  const gchar *fmt;
  gint64 time_stamp, size;
  gchar buffer[256];
  struct tm *time_struct;
  time_t time_val;
  gint type;
  gboolean location_readonly = TRUE;

  g_return_if_fail(HILDON_IS_FILE_DETAILS_DIALOG(self));

  model = GTK_TREE_MODEL(self->priv->model);

  /* Save iterator to priv struct as row reference */
  gtk_tree_row_reference_free(self->priv->active_file);
  path = gtk_tree_model_get_path(model, iter);
  self->priv->active_file = gtk_tree_row_reference_new(model, path);
  gtk_tree_path_free(path);

  /* Setup the view */
  gtk_tree_model_get(model, iter,
    HILDON_FILE_SYSTEM_MODEL_COLUMN_DISPLAY_NAME, &name,
    HILDON_FILE_SYSTEM_MODEL_COLUMN_MIME_TYPE, &mime,
    HILDON_FILE_SYSTEM_MODEL_COLUMN_LOCAL_PATH, &local_path,
    HILDON_FILE_SYSTEM_MODEL_COLUMN_FILE_SIZE, &size,
    HILDON_FILE_SYSTEM_MODEL_COLUMN_FILE_TIME, &time_stamp,
    -1);

  g_object_set(self->priv->file_name, "label", name, NULL);
  g_object_set(self->priv->file_type, "label", _(mime), NULL);

  if (size < 1024)
    g_snprintf(buffer, sizeof(buffer),
               _("ckdg_va_properties_size_bytes"), (gint) size);
  else
    g_snprintf(buffer, sizeof(buffer),
               _("ckdg_va_properties_size_kb"), (gint) size / 1024);

  g_object_set(self->priv->file_size, "label", buffer, NULL);

  /* Too bad. We cannot use GDate function, because it doesn't handle
      time, just dates */
  time_val = (time_t) time_stamp;
  time_struct = localtime(&time_val);

  /* There are no more logical names for these. We are allowed
      to hardcode */
  strftime(buffer, sizeof(buffer), "%X", time_struct);
  g_object_set(self->priv->file_time, "label", buffer, NULL);
  
  /* If format is passed directly to strftime, gcc complains about
      that some locales use only 2 digit year numbers. Using
      a temporary disable this warning (from strftime man page) */
  fmt = "%x";
  strftime(buffer, sizeof(buffer), fmt, time_struct);
  g_object_set(self->priv->file_date, "label", buffer, NULL);

  /* Parent information */
  if (gtk_tree_model_iter_parent(model, &parent_iter, iter))
  {
    gchar *location_name, *parent_path;
    GdkPixbuf *location_icon;

    gtk_tree_model_get(model, &parent_iter,
      HILDON_FILE_SYSTEM_MODEL_COLUMN_DISPLAY_NAME, &location_name,
      HILDON_FILE_SYSTEM_MODEL_COLUMN_LOCAL_PATH, &parent_path,
      HILDON_FILE_SYSTEM_MODEL_COLUMN_ICON, &location_icon, -1);      

    if (parent_path)
      location_readonly = access(parent_path, W_OK)  != 0;

    gtk_label_set_text(GTK_LABEL(self->priv->file_location), location_name);
    gtk_image_set_from_pixbuf(GTK_IMAGE(self->priv->file_location_image), 
      location_icon);

    if (G_IS_OBJECT(location_icon))
      g_object_unref(location_icon);
    g_free(location_name);

    /* Go upwards in model until we find a device node */
    while (TRUE)
    {
      gtk_tree_model_get(model, &parent_iter,
        HILDON_FILE_SYSTEM_MODEL_COLUMN_TYPE, &type, -1);

      if (type >= HILDON_FILE_SYSTEM_MODEL_MMC)
        break;

      if (gtk_tree_model_iter_parent(model, &temp_iter, &parent_iter))
        parent_iter = temp_iter;
      else 
        break;
    }   

    gtk_tree_model_get(model, &parent_iter,
      HILDON_FILE_SYSTEM_MODEL_COLUMN_DISPLAY_NAME, &location_name, 
      HILDON_FILE_SYSTEM_MODEL_COLUMN_ICON, &location_icon, 
      -1);

    gtk_label_set_text(GTK_LABEL(self->priv->file_device), location_name);
    gtk_image_set_from_pixbuf(GTK_IMAGE(self->priv->file_device_image), 
      location_icon);

    if (G_IS_OBJECT(location_icon))
      g_object_unref(location_icon);
    g_free(location_name);
    g_free(parent_path);
  }
  else 
  {   /* We really should not come here */
      gtk_label_set_text(GTK_LABEL(self->priv->file_location), "");
      gtk_image_set_from_pixbuf(GTK_IMAGE(self->priv->file_location_image), NULL);
      gtk_label_set_text(GTK_LABEL(self->priv->file_device), "");
      gtk_image_set_from_pixbuf(GTK_IMAGE(self->priv->file_device_image), NULL);
  }

  /* We do not want initial setting to cause any action */
  g_signal_handler_block(self->priv->file_readonly, self->priv->toggle_handler);

  if (local_path)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->priv->file_readonly), 
      location_readonly || access(local_path, W_OK)  != 0);
    gtk_widget_set_sensitive(self->priv->file_readonly, !location_readonly);
  }
  else  
  {
      /* Backend do not support setting permissions, so we 
          cannot do anything for paths that are not local */
    gtk_toggle_button_set_inconsistent(
        GTK_TOGGLE_BUTTON(self->priv->file_readonly), TRUE);
    gtk_widget_set_sensitive(self->priv->file_readonly, FALSE);
  }

  self->priv->checkbox_original_state = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(self->priv->file_readonly));

  g_signal_handler_unblock(self->priv->file_readonly, self->priv->toggle_handler);

  g_free(local_path);
  g_free(name);
  g_free(mime);
}

/**
 * hildon_file_details_dialog_get_file_iter:
 * @self: a #HildonFileDetailsDialog.
 * @iter: a #GtkTreeIter to be filled.
 *
 * Gets an iterator pointing to displayed file.
 *
 * Returns: %TRUE, if dialog is displaying some information.
 */
gboolean 
hildon_file_details_dialog_get_file_iter(HildonFileDetailsDialog *self, GtkTreeIter *iter)
{
  GtkTreePath *path;
  gboolean result;

  g_return_val_if_fail(HILDON_IS_FILE_DETAILS_DIALOG(self), FALSE);

  if (!self->priv->active_file)
    return FALSE;
  path = gtk_tree_row_reference_get_path(self->priv->active_file);
  if (!path)
    return FALSE;
  
  result = gtk_tree_model_get_iter(GTK_TREE_MODEL(self->priv->model), iter, path);
  gtk_tree_path_free(path);  

  return result;
}
