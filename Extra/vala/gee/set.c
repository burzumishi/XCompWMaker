/* set.c generated by valac, the Vala compiler
 * generated from set.vala, do not modify */

/* set.vala
 *
 * Copyright (C) 2007  Jürg Billeter
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 * Author:
 * 	Jürg Billeter <j@bitron.ch>
 */

#include <glib.h>
#include <glib-object.h>


#define VALA_TYPE_ITERABLE (vala_iterable_get_type ())
#define VALA_ITERABLE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), VALA_TYPE_ITERABLE, ValaIterable))
#define VALA_ITERABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), VALA_TYPE_ITERABLE, ValaIterableClass))
#define VALA_IS_ITERABLE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VALA_TYPE_ITERABLE))
#define VALA_IS_ITERABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VALA_TYPE_ITERABLE))
#define VALA_ITERABLE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VALA_TYPE_ITERABLE, ValaIterableClass))

typedef struct _ValaIterable ValaIterable;
typedef struct _ValaIterableClass ValaIterableClass;
typedef struct _ValaIterablePrivate ValaIterablePrivate;

#define VALA_TYPE_ITERATOR (vala_iterator_get_type ())
#define VALA_ITERATOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), VALA_TYPE_ITERATOR, ValaIterator))
#define VALA_ITERATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), VALA_TYPE_ITERATOR, ValaIteratorClass))
#define VALA_IS_ITERATOR(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VALA_TYPE_ITERATOR))
#define VALA_IS_ITERATOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VALA_TYPE_ITERATOR))
#define VALA_ITERATOR_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VALA_TYPE_ITERATOR, ValaIteratorClass))

typedef struct _ValaIterator ValaIterator;
typedef struct _ValaIteratorClass ValaIteratorClass;

#define VALA_TYPE_COLLECTION (vala_collection_get_type ())
#define VALA_COLLECTION(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), VALA_TYPE_COLLECTION, ValaCollection))
#define VALA_COLLECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), VALA_TYPE_COLLECTION, ValaCollectionClass))
#define VALA_IS_COLLECTION(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VALA_TYPE_COLLECTION))
#define VALA_IS_COLLECTION_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VALA_TYPE_COLLECTION))
#define VALA_COLLECTION_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VALA_TYPE_COLLECTION, ValaCollectionClass))

typedef struct _ValaCollection ValaCollection;
typedef struct _ValaCollectionClass ValaCollectionClass;
typedef struct _ValaCollectionPrivate ValaCollectionPrivate;

#define VALA_TYPE_SET (vala_set_get_type ())
#define VALA_SET(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), VALA_TYPE_SET, ValaSet))
#define VALA_SET_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), VALA_TYPE_SET, ValaSetClass))
#define VALA_IS_SET(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VALA_TYPE_SET))
#define VALA_IS_SET_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VALA_TYPE_SET))
#define VALA_SET_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VALA_TYPE_SET, ValaSetClass))

typedef struct _ValaSet ValaSet;
typedef struct _ValaSetClass ValaSetClass;
typedef struct _ValaSetPrivate ValaSetPrivate;

struct _ValaIterable {
	GTypeInstance parent_instance;
	volatile int ref_count;
	ValaIterablePrivate * priv;
};

struct _ValaIterableClass {
	GTypeClass parent_class;
	void (*finalize) (ValaIterable *self);
	GType (*get_element_type) (ValaIterable* self);
	ValaIterator* (*iterator) (ValaIterable* self);
};

struct _ValaCollection {
	ValaIterable parent_instance;
	ValaCollectionPrivate * priv;
};

struct _ValaCollectionClass {
	ValaIterableClass parent_class;
	gboolean (*contains) (ValaCollection* self, gconstpointer item);
	gboolean (*add) (ValaCollection* self, gconstpointer item);
	gboolean (*remove) (ValaCollection* self, gconstpointer item);
	void (*clear) (ValaCollection* self);
	gint (*get_size) (ValaCollection* self);
};

struct _ValaSet {
	ValaCollection parent_instance;
	ValaSetPrivate * priv;
};

struct _ValaSetClass {
	ValaCollectionClass parent_class;
};

struct _ValaSetPrivate {
	GType g_type;
	GBoxedCopyFunc g_dup_func;
	GDestroyNotify g_destroy_func;
};


static gpointer vala_set_parent_class = NULL;

gpointer vala_iterable_ref (gpointer instance);
void vala_iterable_unref (gpointer instance);
GParamSpec* vala_param_spec_iterable (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
void vala_value_set_iterable (GValue* value, gpointer v_object);
void vala_value_take_iterable (GValue* value, gpointer v_object);
gpointer vala_value_get_iterable (const GValue* value);
GType vala_iterable_get_type (void) G_GNUC_CONST;
gpointer vala_iterator_ref (gpointer instance);
void vala_iterator_unref (gpointer instance);
GParamSpec* vala_param_spec_iterator (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
void vala_value_set_iterator (GValue* value, gpointer v_object);
void vala_value_take_iterator (GValue* value, gpointer v_object);
gpointer vala_value_get_iterator (const GValue* value);
GType vala_iterator_get_type (void) G_GNUC_CONST;
GType vala_collection_get_type (void) G_GNUC_CONST;
GType vala_set_get_type (void) G_GNUC_CONST;
#define VALA_SET_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), VALA_TYPE_SET, ValaSetPrivate))
enum  {
	VALA_SET_DUMMY_PROPERTY
};
ValaSet* vala_set_construct (GType object_type, GType g_type, GBoxedCopyFunc g_dup_func, GDestroyNotify g_destroy_func);
ValaCollection* vala_collection_construct (GType object_type, GType g_type, GBoxedCopyFunc g_dup_func, GDestroyNotify g_destroy_func);


ValaSet* vala_set_construct (GType object_type, GType g_type, GBoxedCopyFunc g_dup_func, GDestroyNotify g_destroy_func) {
	ValaSet* self = NULL;
	self = (ValaSet*) vala_collection_construct (object_type, g_type, (GBoxedCopyFunc) g_dup_func, g_destroy_func);
	self->priv->g_type = g_type;
	self->priv->g_dup_func = g_dup_func;
	self->priv->g_destroy_func = g_destroy_func;
	return self;
}


static void vala_set_class_init (ValaSetClass * klass) {
	vala_set_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (ValaSetPrivate));
}


static void vala_set_instance_init (ValaSet * self) {
	self->priv = VALA_SET_GET_PRIVATE (self);
}


/**
 * A set is a collection without duplicates.
 */
GType vala_set_get_type (void) {
	static volatile gsize vala_set_type_id__volatile = 0;
	if (g_once_init_enter (&vala_set_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValaSetClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) vala_set_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValaSet), 0, (GInstanceInitFunc) vala_set_instance_init, NULL };
		GType vala_set_type_id;
		vala_set_type_id = g_type_register_static (VALA_TYPE_COLLECTION, "ValaSet", &g_define_type_info, G_TYPE_FLAG_ABSTRACT);
		g_once_init_leave (&vala_set_type_id__volatile, vala_set_type_id);
	}
	return vala_set_type_id__volatile;
}



