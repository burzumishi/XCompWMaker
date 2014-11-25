/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ccm-snapshot-dialog.c
 * Copyright (C) Nicolas Bruguier 2007-2011 <gandalfn@club-internet.fr>
 *
 * cairo-compmgr is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * cairo-compmgr is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <X11/Xatom.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#include "ccm-debug.h"
#include "ccm-cairo-utils.h"
#include "ccm-snapshot-dialog.h"

G_DEFINE_TYPE (CCMSnapshotDialog, ccm_snapshot_dialog, G_TYPE_OBJECT);

struct _CCMSnapshotDialogPrivate
{
    cairo_surface_t *surface;
    GtkBuilder *builder;
};

#define CCM_SNAPSHOT_DIALOG_GET_PRIVATE(o)  \
(G_TYPE_INSTANCE_GET_PRIVATE ((o), CCM_TYPE_SNAPSHOT_DIALOG, CCMSnapshotDialogPrivate))

static void
ccm_snapshot_dialog_init (CCMSnapshotDialog * self)
{
    self->priv = CCM_SNAPSHOT_DIALOG_GET_PRIVATE (self);
    self->priv->surface = NULL;
    self->priv->builder = NULL;
    CCM_SNAPSHOT_DIALOG_GET_CLASS (self)->nb++;
}

static void
ccm_snapshot_dialog_finalize (GObject * object)
{
    CCMSnapshotDialog *self = CCM_SNAPSHOT_DIALOG (object);

    if (self->priv->surface)
        cairo_surface_destroy (self->priv->surface);
    if (self->priv->builder)
        g_object_unref (self->priv->builder);
    CCM_SNAPSHOT_DIALOG_GET_CLASS (self)->nb--;

    G_OBJECT_CLASS (ccm_snapshot_dialog_parent_class)->finalize (object);
}

static void
ccm_snapshot_dialog_class_init (CCMSnapshotDialogClass * klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    klass->nb = 0;

    g_type_class_add_private (klass, sizeof (CCMSnapshotDialogPrivate));

    object_class->finalize = ccm_snapshot_dialog_finalize;
}


static void
ccm_snapshot_dialog_on_close (CCMSnapshotDialog * self, GtkWidget * widget)
{
    g_return_if_fail (self != NULL);

    if (self->priv->surface)
        cairo_surface_destroy (self->priv->surface);

    self->priv->surface = NULL;
    g_object_unref (self);
    gtk_widget_destroy (widget);
}

static void
ccm_snapshot_dialog_on_response (CCMSnapshotDialog * self, int response,
                                 GtkWidget * widget)
{
    g_return_if_fail (self != NULL);

    if (response == GTK_RESPONSE_OK)
    {
        GtkWidget *path = GTK_WIDGET (gtk_builder_get_object (self->priv->builder,
                                                              "path"));
        GtkWidget *name = GTK_WIDGET (gtk_builder_get_object (self->priv->builder,
                                                              "name"));

        gchar *dir = gtk_file_chooser_get_current_folder (GTK_FILE_CHOOSER (path));
        const gchar *file = gtk_entry_get_text (GTK_ENTRY (name));

        if (dir && file && strlen (file))
        {
            gchar *filename = g_strdup_printf ("%s/%s", dir, file);
            cairo_surface_write_to_png (self->priv->surface, filename);
            g_free (filename);
            g_free (dir);
        }
    }
    if (self->priv->surface)
        cairo_surface_destroy (self->priv->surface);

    self->priv->surface = NULL;
    g_object_unref (self);
    gtk_widget_destroy (widget);
}

static void
ccm_snapshot_dialog_on_realize (CCMSnapshotDialog * self, GtkWidget * widget)
{
    GtkWidget *snapshot, *table;
    int width, height;

    width = cairo_image_surface_get_width (self->priv->surface);
    height = cairo_image_surface_get_height (self->priv->surface);

    snapshot = GTK_WIDGET (gtk_builder_get_object (self->priv->builder, "snapshot_image"));
    table = GTK_WIDGET (gtk_builder_get_object (self->priv->builder, "table"));

    if (width >= height)
        gtk_widget_set_size_request (snapshot, table->allocation.width,
                                     table->allocation.width * height / width);
    else
        gtk_widget_set_size_request (snapshot,
                                     table->allocation.width * width / height,
                                     table->allocation.width);
}

static void
ccm_snapshot_dialog_paint_snapshot (CCMSnapshotDialog * self, cairo_t * ctx)
{
    GtkWidget *snapshot;
    int width, height;

    width = cairo_image_surface_get_width (self->priv->surface);
    height = cairo_image_surface_get_height (self->priv->surface);

    snapshot = GTK_WIDGET (gtk_builder_get_object (self->priv->builder, "snapshot_image"));
    cairo_save (ctx);
    cairo_set_operator (ctx, CAIRO_OPERATOR_OVER);
    cairo_translate (ctx, snapshot->allocation.x, snapshot->allocation.y);
    cairo_scale (ctx, (double) snapshot->allocation.width / (double) width,
                 (double) snapshot->allocation.height / (double) height);
    gdk_cairo_set_source_color (ctx, &snapshot->style->bg[GTK_STATE_NORMAL]);
    cairo_rectangle (ctx, 0, 0, width, height);
    cairo_fill (ctx);
    cairo_set_source_surface (ctx, self->priv->surface, 0, 0);
    cairo_paint (ctx);
    cairo_restore (ctx);
}

static gboolean
ccm_snapshot_dialog_on_expose_event (GtkWidget * widget,
                                     GdkEventExpose * event,
                                     CCMSnapshotDialog * self)
{
    cairo_t *ctx = gdk_cairo_create (widget->window);
    GtkWidget *child = GTK_WIDGET (gtk_builder_get_object (self->priv->builder,
                                                           "dialog-vbox"));

    gdk_cairo_region (ctx, event->region);
    cairo_clip (ctx);

    ccm_snapshot_dialog_paint_snapshot (self, ctx);
    cairo_destroy (ctx);

    gtk_container_propagate_expose (GTK_CONTAINER (widget), child, event);

    return FALSE;
}

CCMSnapshotDialog *
ccm_snapshot_dialog_new (cairo_surface_t * surface, CCMScreen * screen)
{
    g_return_val_if_fail (surface != NULL, NULL);
    g_return_val_if_fail (screen != NULL, NULL);

    CCMSnapshotDialog *self = g_object_new (CCM_TYPE_SNAPSHOT_DIALOG, NULL);
    GtkWidget *dialog, *name, *path;
    GdkColormap *colormap;
    int width, height;
    gchar *str;
    GdkDisplay *display = gdk_display_get_default ();
    GdkScreen *gdk_screen = gdk_display_get_screen (display,
                                                    ccm_screen_get_number
                                                    (screen));

    self->priv->surface = surface;
    width = cairo_image_surface_get_width (surface);
    height = cairo_image_surface_get_height (surface);

    self->priv->builder = gtk_builder_new ();
    if (self->priv->builder == NULL)
    {
        g_warning ("Error on create snapshot dialog");
        g_object_unref (self);
        return NULL;
    }

    if (!gtk_builder_add_from_file
        (self->priv->builder, UI_DIR "/ccm-snapshot.ui", NULL))
    {
        g_warning ("Error on open snapshot dialog %s",
                   UI_DIR "/ccm-snapshot.ui");
        g_object_unref (self);
        return NULL;
    }

    dialog =
        GTK_WIDGET (gtk_builder_get_object (self->priv->builder, "dialog"));
    if (!dialog)
    {
        g_warning ("Error on get snapshot dialog widget");
        g_object_unref (self);
        return NULL;
    }
    colormap = gdk_screen_get_rgba_colormap (gdk_screen);
    gtk_widget_set_colormap (dialog, colormap);
    gtk_window_set_screen (GTK_WINDOW (dialog), gdk_screen);

    g_signal_connect_swapped (dialog, "close",
                              G_CALLBACK (ccm_snapshot_dialog_on_close), self);
    g_signal_connect_swapped (dialog, "response",
                              G_CALLBACK (ccm_snapshot_dialog_on_response),
                              self);
    g_signal_connect_swapped (dialog, "realize",
                              G_CALLBACK (ccm_snapshot_dialog_on_realize),
                              self);
    g_signal_connect_after (dialog, "expose-event",
                            G_CALLBACK (ccm_snapshot_dialog_on_expose_event),
                            self);

    name = GTK_WIDGET (gtk_builder_get_object (self->priv->builder, "name"));
    if (!name)
    {
        g_warning ("Error on get snapshot name widget");
        g_object_unref (self);
        return NULL;
    }
    if (CCM_SNAPSHOT_DIALOG_GET_CLASS (self)->nb > 1)
        str = g_strdup_printf ("snapshot-%i.png",
                               CCM_SNAPSHOT_DIALOG_GET_CLASS (self)->nb - 1);
    else
        str = g_strdup_printf ("snapshot.png");
    gtk_entry_set_text (GTK_ENTRY (name), str);
    gtk_editable_select_region (GTK_EDITABLE (name), 0, strlen (str) - 4);
    g_free (str);

    path = GTK_WIDGET (gtk_builder_get_object (self->priv->builder, "path"));
    if (!path)
    {
        g_warning ("Error on get snapshot path widget");
        g_object_unref (self);
        return NULL;
    }
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (path),
                                         g_get_user_special_dir (G_USER_DIRECTORY_DESKTOP));

    gtk_widget_show (dialog);
    gtk_window_set_keep_above (GTK_WINDOW (dialog), TRUE);
    gtk_window_set_focus_on_map (GTK_WINDOW (dialog), TRUE);
    gtk_window_present (GTK_WINDOW (dialog));

    return self;
}
