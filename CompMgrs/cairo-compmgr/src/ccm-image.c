/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * ccm-image.c
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

#include <X11/Xlib.h>
#include <X11/ImUtil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <stdlib.h>
#include <pixman.h>

#include "ccm-debug.h"
#include "ccm-image.h"

struct _CCMImage
{
    CCMDisplay *display;

    gboolean xshm;
    Visual *visual;
    cairo_format_t format;
    int depth;
    int width;
    int height;
    XImage *image;
    XShmSegmentInfo shminfo;

    pixman_image_t *pimage;
};


static pixman_format_code_t
ccm_image_get_pixman_format (cairo_format_t format)
{
    pixman_format_code_t ret;

    switch (format)
    {
        case CAIRO_FORMAT_A1:
            ret = PIXMAN_a1;
            break;
        case CAIRO_FORMAT_A8:
            ret = PIXMAN_a8;
            break;
        case CAIRO_FORMAT_RGB24:
            ret = PIXMAN_x8r8g8b8;
            break;
        case CAIRO_FORMAT_ARGB32:
        default:
            ret = PIXMAN_a8r8g8b8;
            break;
    }

    return ret;
}

CCMImage *
ccm_image_new (CCMDisplay * display, Visual * visual, cairo_format_t format,
               int width, int height, int depth)
{
    g_return_val_if_fail (display != NULL, NULL);
    g_return_val_if_fail (visual != NULL, NULL);

    CCMImage *image = g_new0 (CCMImage, 1);

    image->display = display;
    g_object_get (G_OBJECT (display), "use_xshm", &image->xshm, NULL);
    image->visual = visual;
    image->format = format;
    image->depth = depth;
    image->width = width;
    image->height = height;

    if (image->xshm)
    {
        pixman_format_code_t pformat;

        image->image =
            XShmCreateImage (CCM_DISPLAY_XDISPLAY (image->display),
                             image->visual, image->depth, ZPixmap, NULL,
                             &image->shminfo, width, height);
        if (!image->image)
            return NULL;

        image->shminfo.shmid =
            shmget (IPC_PRIVATE,
                    image->image->bytes_per_line * image->image->height,
                    IPC_CREAT | 0600);

        if (image->shminfo.shmid == -1)
        {
            XDestroyImage (image->image);
            image->image = NULL;
            g_free (image);
            return NULL;
        }

        image->shminfo.readOnly = False;
        image->shminfo.shmaddr = shmat (image->shminfo.shmid, 0, 0);
        if (image->shminfo.shmaddr == (gpointer) - 1)
        {
            XDestroyImage (image->image);
            image->image = NULL;
            g_free (image);
            return NULL;
        }

        image->image->data = image->shminfo.shmaddr;

        XShmAttach (CCM_DISPLAY_XDISPLAY (display), &image->shminfo);

        pformat = ccm_image_get_pixman_format (format);

        image->pimage =
            pixman_image_create_bits (pformat, width, height,
                                      (guint32 *) image->image->data,
                                      image->image->bytes_per_line);
    }

    return image;
}

void
ccm_image_destroy (CCMImage * image)
{
    g_return_if_fail (image != NULL);

    if (image->image)
    {
        pixman_image_unref (image->pimage);
        if (image->xshm)
        {
            XShmDetach (CCM_DISPLAY_XDISPLAY (image->display), &image->shminfo);
            shmdt (image->shminfo.shmaddr);
            shmctl (image->shminfo.shmid, IPC_RMID, 0);
        }
        XDestroyImage (image->image);
        image->image = NULL;
    }
    g_free (image);
}

gboolean
ccm_image_get_image (CCMImage * image, CCMPixmap * pixmap, int x, int y)
{
    g_return_val_if_fail (image != NULL, FALSE);
    g_return_val_if_fail (pixmap != NULL, FALSE);

    if (image->xshm)
    {
        return XShmGetImage (CCM_DISPLAY_XDISPLAY (image->display),
                             CCM_PIXMAP_XPIXMAP (pixmap), image->image, x, y,
                             AllPlanes);
    }
    else
    {
        pixman_format_code_t pformat;

        if (image->image)
            XDestroyImage (image->image);
        if (image->pimage)
        {
            pixman_image_unref (image->pimage);
            image->pimage = NULL;
        }

        pformat = ccm_image_get_pixman_format (image->format);

        image->image =
            XGetImage (CCM_DISPLAY_XDISPLAY (image->display),
                       CCM_PIXMAP_XPIXMAP (pixmap), x, y, image->width,
                       image->height, AllPlanes, ZPixmap);

        if (image->image)
            image->pimage =
            pixman_image_create_bits (pformat, image->width, image->height,
                                      (guint32 *) image->image->data,
                                      image->image->bytes_per_line);
        return image->image != NULL;
    }
}

gboolean
ccm_image_get_sub_image (CCMImage * image, CCMPixmap * pixmap, int x, int y,
                         int width, int height)
{
    g_return_val_if_fail (image != NULL, FALSE);
    g_return_val_if_fail (pixmap != NULL, FALSE);
    g_return_val_if_fail (width > 0 && height > 0, FALSE);

    gboolean ret = FALSE;
    cairo_format_t format = ccm_drawable_get_format (CCM_DRAWABLE (pixmap));
    CCMImage *sub_image = ccm_image_new (image->display, image->visual,
                                         format, width, height, image->depth);
    if (sub_image)
    {
        if (ccm_image_get_image (sub_image, pixmap, x, y))
        {
            pixman_image_composite (PIXMAN_OP_SRC, sub_image->pimage, NULL,
                                    image->pimage, 0, 0, 0, 0, x, y, width,
                                    height);
            ret = TRUE;
        }
        else
        {
            ccm_debug ("GET_SUB_IMAGE ERROR");
        }
        ccm_image_destroy (sub_image);
    }

    return ret;
}

gboolean
ccm_image_put_image (CCMImage * image, CCMPixmap * pixmap, int x_src, int y_src,
                     int x, int y, int width, int height)
{
    g_return_val_if_fail (image != NULL, FALSE);
    g_return_val_if_fail (pixmap != NULL, FALSE);

    CCMDisplay *display = ccm_drawable_get_display (CCM_DRAWABLE (pixmap));
    XGCValues gcv;
    GC gc;
    gboolean ret = FALSE;

    gcv.graphics_exposures = FALSE;
    gcv.subwindow_mode = IncludeInferiors;

    gc = XCreateGC (CCM_DISPLAY_XDISPLAY (display), CCM_PIXMAP_XPIXMAP (pixmap),
                    GCGraphicsExposures | GCSubwindowMode, &gcv);
    if (!gc)
        return ret;

    if (image->xshm)
    {
        ret = XShmPutImage (CCM_DISPLAY_XDISPLAY (image->display),
                            CCM_PIXMAP_XPIXMAP (pixmap), gc, image->image, x_src,
                            y_src, x, y, width, height, False);
    }
    else
    {
        ret = XPutImage (CCM_DISPLAY_XDISPLAY (image->display),
                         CCM_PIXMAP_XPIXMAP (pixmap), gc, image->image, x_src,
                         y_src, x, y, width, height);
    }

    ccm_display_sync (image->display);

    XFreeGC (CCM_DISPLAY_XDISPLAY (image->display), gc);

    if (!ret)
        ccm_debug ("ERROR ON FLUSH PIXMAP");
    return ret;
}

G_GNUC_PURE guchar *
ccm_image_get_data (CCMImage * image)
{
    g_return_val_if_fail (image != NULL, NULL);

    return image->image ? (guchar *) image->image->data : NULL;
}

G_GNUC_PURE gint
ccm_image_get_width (CCMImage * image)
{
    g_return_val_if_fail (image != NULL, 0);

    return image->width;
}

G_GNUC_PURE gint
ccm_image_get_height (CCMImage * image)
{
    g_return_val_if_fail (image != NULL, 0);

    return image->height;
}

G_GNUC_PURE gint
ccm_image_get_stride (CCMImage * image)
{
    g_return_val_if_fail (image != NULL, 0);

    return image->image ? image->image->bytes_per_line : 0;
}
