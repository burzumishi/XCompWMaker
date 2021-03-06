/* valareport.c generated by valac, the Vala compiler
 * generated from valareport.vala, do not modify */

/* valareport.vala
 *
 * Copyright (C) 2006-2010  Jürg Billeter
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define VALA_TYPE_REPORT (vala_report_get_type ())
#define VALA_REPORT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), VALA_TYPE_REPORT, ValaReport))
#define VALA_REPORT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), VALA_TYPE_REPORT, ValaReportClass))
#define VALA_IS_REPORT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VALA_TYPE_REPORT))
#define VALA_IS_REPORT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VALA_TYPE_REPORT))
#define VALA_REPORT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VALA_TYPE_REPORT, ValaReportClass))

typedef struct _ValaReport ValaReport;
typedef struct _ValaReportClass ValaReportClass;
typedef struct _ValaReportPrivate ValaReportPrivate;

#define VALA_TYPE_SOURCE_REFERENCE (vala_source_reference_get_type ())
#define VALA_SOURCE_REFERENCE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), VALA_TYPE_SOURCE_REFERENCE, ValaSourceReference))
#define VALA_SOURCE_REFERENCE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), VALA_TYPE_SOURCE_REFERENCE, ValaSourceReferenceClass))
#define VALA_IS_SOURCE_REFERENCE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VALA_TYPE_SOURCE_REFERENCE))
#define VALA_IS_SOURCE_REFERENCE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VALA_TYPE_SOURCE_REFERENCE))
#define VALA_SOURCE_REFERENCE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VALA_TYPE_SOURCE_REFERENCE, ValaSourceReferenceClass))

typedef struct _ValaSourceReference ValaSourceReference;
typedef struct _ValaSourceReferenceClass ValaSourceReferenceClass;

#define VALA_TYPE_SOURCE_LOCATION (vala_source_location_get_type ())
typedef struct _ValaSourceLocation ValaSourceLocation;

#define VALA_TYPE_SOURCE_FILE (vala_source_file_get_type ())
#define VALA_SOURCE_FILE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), VALA_TYPE_SOURCE_FILE, ValaSourceFile))
#define VALA_SOURCE_FILE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), VALA_TYPE_SOURCE_FILE, ValaSourceFileClass))
#define VALA_IS_SOURCE_FILE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VALA_TYPE_SOURCE_FILE))
#define VALA_IS_SOURCE_FILE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VALA_TYPE_SOURCE_FILE))
#define VALA_SOURCE_FILE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VALA_TYPE_SOURCE_FILE, ValaSourceFileClass))

typedef struct _ValaSourceFile ValaSourceFile;
typedef struct _ValaSourceFileClass ValaSourceFileClass;
#define _g_free0(var) (var = (g_free (var), NULL))

#define VALA_TYPE_CODE_CONTEXT (vala_code_context_get_type ())
#define VALA_CODE_CONTEXT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), VALA_TYPE_CODE_CONTEXT, ValaCodeContext))
#define VALA_CODE_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), VALA_TYPE_CODE_CONTEXT, ValaCodeContextClass))
#define VALA_IS_CODE_CONTEXT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VALA_TYPE_CODE_CONTEXT))
#define VALA_IS_CODE_CONTEXT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VALA_TYPE_CODE_CONTEXT))
#define VALA_CODE_CONTEXT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VALA_TYPE_CODE_CONTEXT, ValaCodeContextClass))

typedef struct _ValaCodeContext ValaCodeContext;
typedef struct _ValaCodeContextClass ValaCodeContextClass;
#define _vala_code_context_unref0(var) ((var == NULL) ? NULL : (var = (vala_code_context_unref (var), NULL)))

struct _ValaReport {
	GObject parent_instance;
	ValaReportPrivate * priv;
	gint warnings;
	gint errors;
};

struct _ValaReportClass {
	GObjectClass parent_class;
	void (*note) (ValaReport* self, ValaSourceReference* source, const gchar* message);
	void (*depr) (ValaReport* self, ValaSourceReference* source, const gchar* message);
	void (*warn) (ValaReport* self, ValaSourceReference* source, const gchar* message);
	void (*err) (ValaReport* self, ValaSourceReference* source, const gchar* message);
};

struct _ValaReportPrivate {
	gboolean verbose_errors;
	gboolean _enable_warnings;
};

struct _ValaSourceLocation {
	gchar* pos;
	gint line;
	gint column;
};


static gpointer vala_report_parent_class = NULL;

GType vala_report_get_type (void) G_GNUC_CONST;
gpointer vala_source_reference_ref (gpointer instance);
void vala_source_reference_unref (gpointer instance);
GParamSpec* vala_param_spec_source_reference (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
void vala_value_set_source_reference (GValue* value, gpointer v_object);
void vala_value_take_source_reference (GValue* value, gpointer v_object);
gpointer vala_value_get_source_reference (const GValue* value);
GType vala_source_reference_get_type (void) G_GNUC_CONST;
#define VALA_REPORT_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), VALA_TYPE_REPORT, ValaReportPrivate))
enum  {
	VALA_REPORT_DUMMY_PROPERTY,
	VALA_REPORT_ENABLE_WARNINGS
};
void vala_report_set_verbose_errors (ValaReport* self, gboolean verbose);
gint vala_report_get_warnings (ValaReport* self);
gint vala_report_get_errors (ValaReport* self);
static void vala_report_report_source (ValaSourceReference* source);
GType vala_source_location_get_type (void) G_GNUC_CONST;
ValaSourceLocation* vala_source_location_dup (const ValaSourceLocation* self);
void vala_source_location_free (ValaSourceLocation* self);
void vala_source_reference_get_begin (ValaSourceReference* self, ValaSourceLocation* result);
void vala_source_reference_get_end (ValaSourceReference* self, ValaSourceLocation* result);
gpointer vala_source_file_ref (gpointer instance);
void vala_source_file_unref (gpointer instance);
GParamSpec* vala_param_spec_source_file (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
void vala_value_set_source_file (GValue* value, gpointer v_object);
void vala_value_take_source_file (GValue* value, gpointer v_object);
gpointer vala_value_get_source_file (const GValue* value);
GType vala_source_file_get_type (void) G_GNUC_CONST;
ValaSourceFile* vala_source_reference_get_file (ValaSourceReference* self);
gchar* vala_source_file_get_source_line (ValaSourceFile* self, gint lineno);
void vala_report_note (ValaReport* self, ValaSourceReference* source, const gchar* message);
static void vala_report_real_note (ValaReport* self, ValaSourceReference* source, const gchar* message);
gboolean vala_report_get_enable_warnings (ValaReport* self);
gchar* vala_source_reference_to_string (ValaSourceReference* self);
void vala_report_depr (ValaReport* self, ValaSourceReference* source, const gchar* message);
static void vala_report_real_depr (ValaReport* self, ValaSourceReference* source, const gchar* message);
void vala_report_warn (ValaReport* self, ValaSourceReference* source, const gchar* message);
static void vala_report_real_warn (ValaReport* self, ValaSourceReference* source, const gchar* message);
void vala_report_err (ValaReport* self, ValaSourceReference* source, const gchar* message);
static void vala_report_real_err (ValaReport* self, ValaSourceReference* source, const gchar* message);
void vala_report_notice (ValaSourceReference* source, const gchar* message);
gpointer vala_code_context_ref (gpointer instance);
void vala_code_context_unref (gpointer instance);
GParamSpec* vala_param_spec_code_context (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
void vala_value_set_code_context (GValue* value, gpointer v_object);
void vala_value_take_code_context (GValue* value, gpointer v_object);
gpointer vala_value_get_code_context (const GValue* value);
GType vala_code_context_get_type (void) G_GNUC_CONST;
ValaCodeContext* vala_code_context_get (void);
ValaReport* vala_code_context_get_report (ValaCodeContext* self);
void vala_report_deprecated (ValaSourceReference* source, const gchar* message);
void vala_report_experimental (ValaSourceReference* source, const gchar* message);
void vala_report_warning (ValaSourceReference* source, const gchar* message);
void vala_report_error (ValaSourceReference* source, const gchar* message);
ValaReport* vala_report_new (void);
ValaReport* vala_report_construct (GType object_type);
void vala_report_set_enable_warnings (ValaReport* self, gboolean value);
static void vala_report_finalize (GObject* obj);
static void _vala_vala_report_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec);
static void _vala_vala_report_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec);


/**
 * Set the error verbosity.
 */
void vala_report_set_verbose_errors (ValaReport* self, gboolean verbose) {
	gboolean _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp0_ = verbose;
	self->priv->verbose_errors = _tmp0_;
}


/**
 * Returns the total number of warnings reported.
 */
gint vala_report_get_warnings (ValaReport* self) {
	gint result = 0;
	gint _tmp0_;
	g_return_val_if_fail (self != NULL, 0);
	_tmp0_ = self->warnings;
	result = _tmp0_;
	return result;
}


/**
 * Returns the total number of errors reported.
 */
gint vala_report_get_errors (ValaReport* self) {
	gint result = 0;
	gint _tmp0_;
	g_return_val_if_fail (self != NULL, 0);
	_tmp0_ = self->errors;
	result = _tmp0_;
	return result;
}


/**
 * Pretty-print the actual line of offending code if possible.
 */
static gchar string_get (const gchar* self, glong index) {
	gchar result = '\0';
	glong _tmp0_;
	gchar _tmp1_;
	g_return_val_if_fail (self != NULL, '\0');
	_tmp0_ = index;
	_tmp1_ = ((gchar*) self)[_tmp0_];
	result = _tmp1_;
	return result;
}


static void vala_report_report_source (ValaSourceReference* source) {
	ValaSourceReference* _tmp0_;
	ValaSourceLocation _tmp1_;
	ValaSourceLocation _tmp2_;
	gint _tmp3_;
	ValaSourceReference* _tmp4_;
	ValaSourceLocation _tmp5_;
	ValaSourceLocation _tmp6_;
	gint _tmp7_;
	ValaSourceReference* _tmp8_;
	ValaSourceFile* _tmp9_;
	ValaSourceFile* _tmp10_;
	ValaSourceReference* _tmp11_;
	ValaSourceLocation _tmp12_;
	ValaSourceLocation _tmp13_;
	gint _tmp14_;
	gchar* _tmp15_ = NULL;
	gchar* offending_line;
	const gchar* _tmp16_;
	g_return_if_fail (source != NULL);
	_tmp0_ = source;
	vala_source_reference_get_begin (_tmp0_, &_tmp1_);
	_tmp2_ = _tmp1_;
	_tmp3_ = _tmp2_.line;
	_tmp4_ = source;
	vala_source_reference_get_end (_tmp4_, &_tmp5_);
	_tmp6_ = _tmp5_;
	_tmp7_ = _tmp6_.line;
	if (_tmp3_ != _tmp7_) {
		return;
	}
	_tmp8_ = source;
	_tmp9_ = vala_source_reference_get_file (_tmp8_);
	_tmp10_ = _tmp9_;
	_tmp11_ = source;
	vala_source_reference_get_begin (_tmp11_, &_tmp12_);
	_tmp13_ = _tmp12_;
	_tmp14_ = _tmp13_.line;
	_tmp15_ = vala_source_file_get_source_line (_tmp10_, _tmp14_);
	offending_line = _tmp15_;
	_tmp16_ = offending_line;
	if (_tmp16_ != NULL) {
		FILE* _tmp17_;
		const gchar* _tmp18_;
		gint idx = 0;
		FILE* _tmp49_;
		_tmp17_ = stderr;
		_tmp18_ = offending_line;
		fprintf (_tmp17_, "%s\n", _tmp18_);
		{
			gboolean _tmp19_;
			idx = 1;
			_tmp19_ = TRUE;
			while (TRUE) {
				gboolean _tmp20_;
				gint _tmp22_;
				ValaSourceReference* _tmp23_;
				ValaSourceLocation _tmp24_;
				ValaSourceLocation _tmp25_;
				gint _tmp26_;
				const gchar* _tmp27_;
				gint _tmp28_;
				gchar _tmp29_ = '\0';
				_tmp20_ = _tmp19_;
				if (!_tmp20_) {
					gint _tmp21_;
					_tmp21_ = idx;
					idx = _tmp21_ + 1;
				}
				_tmp19_ = FALSE;
				_tmp22_ = idx;
				_tmp23_ = source;
				vala_source_reference_get_begin (_tmp23_, &_tmp24_);
				_tmp25_ = _tmp24_;
				_tmp26_ = _tmp25_.column;
				if (!(_tmp22_ < _tmp26_)) {
					break;
				}
				_tmp27_ = offending_line;
				_tmp28_ = idx;
				_tmp29_ = string_get (_tmp27_, (glong) (_tmp28_ - 1));
				if (_tmp29_ == '\t') {
					FILE* _tmp30_;
					_tmp30_ = stderr;
					fprintf (_tmp30_, "\t");
				} else {
					FILE* _tmp31_;
					_tmp31_ = stderr;
					fprintf (_tmp31_, " ");
				}
			}
		}
		{
			ValaSourceReference* _tmp32_;
			ValaSourceLocation _tmp33_;
			ValaSourceLocation _tmp34_;
			gint _tmp35_;
			gboolean _tmp36_;
			_tmp32_ = source;
			vala_source_reference_get_begin (_tmp32_, &_tmp33_);
			_tmp34_ = _tmp33_;
			_tmp35_ = _tmp34_.column;
			idx = _tmp35_;
			_tmp36_ = TRUE;
			while (TRUE) {
				gboolean _tmp37_;
				gint _tmp39_;
				ValaSourceReference* _tmp40_;
				ValaSourceLocation _tmp41_;
				ValaSourceLocation _tmp42_;
				gint _tmp43_;
				const gchar* _tmp44_;
				gint _tmp45_;
				gchar _tmp46_ = '\0';
				_tmp37_ = _tmp36_;
				if (!_tmp37_) {
					gint _tmp38_;
					_tmp38_ = idx;
					idx = _tmp38_ + 1;
				}
				_tmp36_ = FALSE;
				_tmp39_ = idx;
				_tmp40_ = source;
				vala_source_reference_get_end (_tmp40_, &_tmp41_);
				_tmp42_ = _tmp41_;
				_tmp43_ = _tmp42_.column;
				if (!(_tmp39_ <= _tmp43_)) {
					break;
				}
				_tmp44_ = offending_line;
				_tmp45_ = idx;
				_tmp46_ = string_get (_tmp44_, (glong) (_tmp45_ - 1));
				if (_tmp46_ == '\t') {
					FILE* _tmp47_;
					_tmp47_ = stderr;
					fprintf (_tmp47_, "\t");
				} else {
					FILE* _tmp48_;
					_tmp48_ = stderr;
					fprintf (_tmp48_, "^");
				}
			}
		}
		_tmp49_ = stderr;
		fprintf (_tmp49_, "\n");
	}
	_g_free0 (offending_line);
}


/**
 * Reports the specified message as note.
 *
 * @param source  reference to source code
 * @param message note message
 */
static void vala_report_real_note (ValaReport* self, ValaSourceReference* source, const gchar* message) {
	gboolean _tmp0_;
	ValaSourceReference* _tmp1_;
	g_return_if_fail (message != NULL);
	_tmp0_ = self->priv->_enable_warnings;
	if (!_tmp0_) {
		return;
	}
	_tmp1_ = source;
	if (_tmp1_ == NULL) {
		FILE* _tmp2_;
		const gchar* _tmp3_;
		_tmp2_ = stderr;
		_tmp3_ = message;
		fprintf (_tmp2_, "note: %s\n", _tmp3_);
	} else {
		FILE* _tmp4_;
		ValaSourceReference* _tmp5_;
		gchar* _tmp6_ = NULL;
		gchar* _tmp7_;
		const gchar* _tmp8_;
		gboolean _tmp9_;
		_tmp4_ = stderr;
		_tmp5_ = source;
		_tmp6_ = vala_source_reference_to_string (_tmp5_);
		_tmp7_ = _tmp6_;
		_tmp8_ = message;
		fprintf (_tmp4_, "%s: note: %s\n", _tmp7_, _tmp8_);
		_g_free0 (_tmp7_);
		_tmp9_ = self->priv->verbose_errors;
		if (_tmp9_) {
			ValaSourceReference* _tmp10_;
			_tmp10_ = source;
			vala_report_report_source (_tmp10_);
		}
	}
}


void vala_report_note (ValaReport* self, ValaSourceReference* source, const gchar* message) {
	g_return_if_fail (self != NULL);
	VALA_REPORT_GET_CLASS (self)->note (self, source, message);
}


/**
 * Reports the specified message as deprecation warning.
 *
 * @param source  reference to source code
 * @param message warning message
 */
static void vala_report_real_depr (ValaReport* self, ValaSourceReference* source, const gchar* message) {
	gboolean _tmp0_;
	gint _tmp1_;
	ValaSourceReference* _tmp2_;
	g_return_if_fail (message != NULL);
	_tmp0_ = self->priv->_enable_warnings;
	if (!_tmp0_) {
		return;
	}
	_tmp1_ = self->warnings;
	self->warnings = _tmp1_ + 1;
	_tmp2_ = source;
	if (_tmp2_ == NULL) {
		FILE* _tmp3_;
		const gchar* _tmp4_;
		_tmp3_ = stderr;
		_tmp4_ = message;
		fprintf (_tmp3_, "warning: %s\n", _tmp4_);
	} else {
		FILE* _tmp5_;
		ValaSourceReference* _tmp6_;
		gchar* _tmp7_ = NULL;
		gchar* _tmp8_;
		const gchar* _tmp9_;
		_tmp5_ = stderr;
		_tmp6_ = source;
		_tmp7_ = vala_source_reference_to_string (_tmp6_);
		_tmp8_ = _tmp7_;
		_tmp9_ = message;
		fprintf (_tmp5_, "%s: warning: %s\n", _tmp8_, _tmp9_);
		_g_free0 (_tmp8_);
	}
}


void vala_report_depr (ValaReport* self, ValaSourceReference* source, const gchar* message) {
	g_return_if_fail (self != NULL);
	VALA_REPORT_GET_CLASS (self)->depr (self, source, message);
}


/**
 * Reports the specified message as warning.
 *
 * @param source  reference to source code
 * @param message warning message
 */
static void vala_report_real_warn (ValaReport* self, ValaSourceReference* source, const gchar* message) {
	gboolean _tmp0_;
	gint _tmp1_;
	ValaSourceReference* _tmp2_;
	g_return_if_fail (message != NULL);
	_tmp0_ = self->priv->_enable_warnings;
	if (!_tmp0_) {
		return;
	}
	_tmp1_ = self->warnings;
	self->warnings = _tmp1_ + 1;
	_tmp2_ = source;
	if (_tmp2_ == NULL) {
		FILE* _tmp3_;
		const gchar* _tmp4_;
		_tmp3_ = stderr;
		_tmp4_ = message;
		fprintf (_tmp3_, "warning: %s\n", _tmp4_);
	} else {
		FILE* _tmp5_;
		ValaSourceReference* _tmp6_;
		gchar* _tmp7_ = NULL;
		gchar* _tmp8_;
		const gchar* _tmp9_;
		gboolean _tmp10_;
		_tmp5_ = stderr;
		_tmp6_ = source;
		_tmp7_ = vala_source_reference_to_string (_tmp6_);
		_tmp8_ = _tmp7_;
		_tmp9_ = message;
		fprintf (_tmp5_, "%s: warning: %s\n", _tmp8_, _tmp9_);
		_g_free0 (_tmp8_);
		_tmp10_ = self->priv->verbose_errors;
		if (_tmp10_) {
			ValaSourceReference* _tmp11_;
			_tmp11_ = source;
			vala_report_report_source (_tmp11_);
		}
	}
}


void vala_report_warn (ValaReport* self, ValaSourceReference* source, const gchar* message) {
	g_return_if_fail (self != NULL);
	VALA_REPORT_GET_CLASS (self)->warn (self, source, message);
}


/**
 * Reports the specified message as error.
 *
 * @param source  reference to source code
 * @param message error message
 */
static void vala_report_real_err (ValaReport* self, ValaSourceReference* source, const gchar* message) {
	gint _tmp0_;
	ValaSourceReference* _tmp1_;
	g_return_if_fail (message != NULL);
	_tmp0_ = self->errors;
	self->errors = _tmp0_ + 1;
	_tmp1_ = source;
	if (_tmp1_ == NULL) {
		FILE* _tmp2_;
		const gchar* _tmp3_;
		_tmp2_ = stderr;
		_tmp3_ = message;
		fprintf (_tmp2_, "error: %s\n", _tmp3_);
	} else {
		FILE* _tmp4_;
		ValaSourceReference* _tmp5_;
		gchar* _tmp6_ = NULL;
		gchar* _tmp7_;
		const gchar* _tmp8_;
		gboolean _tmp9_;
		_tmp4_ = stderr;
		_tmp5_ = source;
		_tmp6_ = vala_source_reference_to_string (_tmp5_);
		_tmp7_ = _tmp6_;
		_tmp8_ = message;
		fprintf (_tmp4_, "%s: error: %s\n", _tmp7_, _tmp8_);
		_g_free0 (_tmp7_);
		_tmp9_ = self->priv->verbose_errors;
		if (_tmp9_) {
			ValaSourceReference* _tmp10_;
			_tmp10_ = source;
			vala_report_report_source (_tmp10_);
		}
	}
}


void vala_report_err (ValaReport* self, ValaSourceReference* source, const gchar* message) {
	g_return_if_fail (self != NULL);
	VALA_REPORT_GET_CLASS (self)->err (self, source, message);
}


void vala_report_notice (ValaSourceReference* source, const gchar* message) {
	ValaCodeContext* _tmp0_ = NULL;
	ValaCodeContext* _tmp1_;
	ValaReport* _tmp2_;
	ValaReport* _tmp3_;
	ValaSourceReference* _tmp4_;
	const gchar* _tmp5_;
	g_return_if_fail (message != NULL);
	_tmp0_ = vala_code_context_get ();
	_tmp1_ = _tmp0_;
	_tmp2_ = vala_code_context_get_report (_tmp1_);
	_tmp3_ = _tmp2_;
	_tmp4_ = source;
	_tmp5_ = message;
	vala_report_note (_tmp3_, _tmp4_, _tmp5_);
	_vala_code_context_unref0 (_tmp1_);
}


void vala_report_deprecated (ValaSourceReference* source, const gchar* message) {
	ValaCodeContext* _tmp0_ = NULL;
	ValaCodeContext* _tmp1_;
	ValaReport* _tmp2_;
	ValaReport* _tmp3_;
	ValaSourceReference* _tmp4_;
	const gchar* _tmp5_;
	g_return_if_fail (message != NULL);
	_tmp0_ = vala_code_context_get ();
	_tmp1_ = _tmp0_;
	_tmp2_ = vala_code_context_get_report (_tmp1_);
	_tmp3_ = _tmp2_;
	_tmp4_ = source;
	_tmp5_ = message;
	vala_report_depr (_tmp3_, _tmp4_, _tmp5_);
	_vala_code_context_unref0 (_tmp1_);
}


void vala_report_experimental (ValaSourceReference* source, const gchar* message) {
	ValaCodeContext* _tmp0_ = NULL;
	ValaCodeContext* _tmp1_;
	ValaReport* _tmp2_;
	ValaReport* _tmp3_;
	ValaSourceReference* _tmp4_;
	const gchar* _tmp5_;
	g_return_if_fail (message != NULL);
	_tmp0_ = vala_code_context_get ();
	_tmp1_ = _tmp0_;
	_tmp2_ = vala_code_context_get_report (_tmp1_);
	_tmp3_ = _tmp2_;
	_tmp4_ = source;
	_tmp5_ = message;
	vala_report_depr (_tmp3_, _tmp4_, _tmp5_);
	_vala_code_context_unref0 (_tmp1_);
}


void vala_report_warning (ValaSourceReference* source, const gchar* message) {
	ValaCodeContext* _tmp0_ = NULL;
	ValaCodeContext* _tmp1_;
	ValaReport* _tmp2_;
	ValaReport* _tmp3_;
	ValaSourceReference* _tmp4_;
	const gchar* _tmp5_;
	g_return_if_fail (message != NULL);
	_tmp0_ = vala_code_context_get ();
	_tmp1_ = _tmp0_;
	_tmp2_ = vala_code_context_get_report (_tmp1_);
	_tmp3_ = _tmp2_;
	_tmp4_ = source;
	_tmp5_ = message;
	vala_report_warn (_tmp3_, _tmp4_, _tmp5_);
	_vala_code_context_unref0 (_tmp1_);
}


void vala_report_error (ValaSourceReference* source, const gchar* message) {
	ValaCodeContext* _tmp0_ = NULL;
	ValaCodeContext* _tmp1_;
	ValaReport* _tmp2_;
	ValaReport* _tmp3_;
	ValaSourceReference* _tmp4_;
	const gchar* _tmp5_;
	g_return_if_fail (message != NULL);
	_tmp0_ = vala_code_context_get ();
	_tmp1_ = _tmp0_;
	_tmp2_ = vala_code_context_get_report (_tmp1_);
	_tmp3_ = _tmp2_;
	_tmp4_ = source;
	_tmp5_ = message;
	vala_report_err (_tmp3_, _tmp4_, _tmp5_);
	_vala_code_context_unref0 (_tmp1_);
}


ValaReport* vala_report_construct (GType object_type) {
	ValaReport * self = NULL;
	self = (ValaReport*) g_object_new (object_type, NULL);
	return self;
}


ValaReport* vala_report_new (void) {
	return vala_report_construct (VALA_TYPE_REPORT);
}


gboolean vala_report_get_enable_warnings (ValaReport* self) {
	gboolean result;
	gboolean _tmp0_;
	g_return_val_if_fail (self != NULL, FALSE);
	_tmp0_ = self->priv->_enable_warnings;
	result = _tmp0_;
	return result;
}


void vala_report_set_enable_warnings (ValaReport* self, gboolean value) {
	gboolean _tmp0_;
	g_return_if_fail (self != NULL);
	_tmp0_ = value;
	self->priv->_enable_warnings = _tmp0_;
	g_object_notify ((GObject *) self, "enable-warnings");
}


static void vala_report_class_init (ValaReportClass * klass) {
	vala_report_parent_class = g_type_class_peek_parent (klass);
	g_type_class_add_private (klass, sizeof (ValaReportPrivate));
	VALA_REPORT_CLASS (klass)->note = vala_report_real_note;
	VALA_REPORT_CLASS (klass)->depr = vala_report_real_depr;
	VALA_REPORT_CLASS (klass)->warn = vala_report_real_warn;
	VALA_REPORT_CLASS (klass)->err = vala_report_real_err;
	G_OBJECT_CLASS (klass)->get_property = _vala_vala_report_get_property;
	G_OBJECT_CLASS (klass)->set_property = _vala_vala_report_set_property;
	G_OBJECT_CLASS (klass)->finalize = vala_report_finalize;
	g_object_class_install_property (G_OBJECT_CLASS (klass), VALA_REPORT_ENABLE_WARNINGS, g_param_spec_boolean ("enable-warnings", "enable-warnings", "enable-warnings", TRUE, G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK | G_PARAM_STATIC_BLURB | G_PARAM_READABLE | G_PARAM_WRITABLE));
}


static void vala_report_instance_init (ValaReport * self) {
	self->priv = VALA_REPORT_GET_PRIVATE (self);
	self->priv->_enable_warnings = TRUE;
}


static void vala_report_finalize (GObject* obj) {
	ValaReport * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, VALA_TYPE_REPORT, ValaReport);
	G_OBJECT_CLASS (vala_report_parent_class)->finalize (obj);
}


/**
 * Namespace to centralize reporting warnings and errors.
 */
GType vala_report_get_type (void) {
	static volatile gsize vala_report_type_id__volatile = 0;
	if (g_once_init_enter (&vala_report_type_id__volatile)) {
		static const GTypeInfo g_define_type_info = { sizeof (ValaReportClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) vala_report_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValaReport), 0, (GInstanceInitFunc) vala_report_instance_init, NULL };
		GType vala_report_type_id;
		vala_report_type_id = g_type_register_static (G_TYPE_OBJECT, "ValaReport", &g_define_type_info, 0);
		g_once_init_leave (&vala_report_type_id__volatile, vala_report_type_id);
	}
	return vala_report_type_id__volatile;
}


static void _vala_vala_report_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec) {
	ValaReport * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, VALA_TYPE_REPORT, ValaReport);
	switch (property_id) {
		case VALA_REPORT_ENABLE_WARNINGS:
		g_value_set_boolean (value, vala_report_get_enable_warnings (self));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}


static void _vala_vala_report_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec) {
	ValaReport * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (object, VALA_TYPE_REPORT, ValaReport);
	switch (property_id) {
		case VALA_REPORT_ENABLE_WARNINGS:
		vala_report_set_enable_warnings (self, g_value_get_boolean (value));
		break;
		default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
		break;
	}
}



