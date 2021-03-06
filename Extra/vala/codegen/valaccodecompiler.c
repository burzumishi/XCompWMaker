/* valaccodecompiler.c generated by valac, the Vala compiler
 * generated from valaccodecompiler.vala, do not modify */

/* valaccodecompiler.vala
 *
 * Copyright (C) 2007-2009  Jürg Billeter
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
#include <vala.h>
#include <valagee.h>
#include <stdio.h>
#include <glib/gstdio.h>
#include <gobject/gvaluecollector.h>


#define VALA_TYPE_CCODE_COMPILER (vala_ccode_compiler_get_type ())
#define VALA_CCODE_COMPILER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), VALA_TYPE_CCODE_COMPILER, ValaCCodeCompiler))
#define VALA_CCODE_COMPILER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), VALA_TYPE_CCODE_COMPILER, ValaCCodeCompilerClass))
#define VALA_IS_CCODE_COMPILER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), VALA_TYPE_CCODE_COMPILER))
#define VALA_IS_CCODE_COMPILER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), VALA_TYPE_CCODE_COMPILER))
#define VALA_CCODE_COMPILER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), VALA_TYPE_CCODE_COMPILER, ValaCCodeCompilerClass))

typedef struct _ValaCCodeCompiler ValaCCodeCompiler;
typedef struct _ValaCCodeCompilerClass ValaCCodeCompilerClass;
typedef struct _ValaCCodeCompilerPrivate ValaCCodeCompilerPrivate;
#define _g_free0(var) (var = (g_free (var), NULL))
#define _g_error_free0(var) ((var == NULL) ? NULL : (var = (g_error_free (var), NULL)))
#define _vala_iterable_unref0(var) ((var == NULL) ? NULL : (var = (vala_iterable_unref (var), NULL)))
#define _vala_source_file_unref0(var) ((var == NULL) ? NULL : (var = (vala_source_file_unref (var), NULL)))
typedef struct _ValaParamSpecCCodeCompiler ValaParamSpecCCodeCompiler;

struct _ValaCCodeCompiler {
	GTypeInstance parent_instance;
	volatile int ref_count;
	ValaCCodeCompilerPrivate * priv;
};

struct _ValaCCodeCompilerClass {
	GTypeClass parent_class;
	void (*finalize) (ValaCCodeCompiler *self);
};

struct _ValaParamSpecCCodeCompiler {
	GParamSpec parent_instance;
};


static gpointer vala_ccode_compiler_parent_class = NULL;

gpointer vala_ccode_compiler_ref (gpointer instance);
void vala_ccode_compiler_unref (gpointer instance);
GParamSpec* vala_param_spec_ccode_compiler (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags);
void vala_value_set_ccode_compiler (GValue* value, gpointer v_object);
void vala_value_take_ccode_compiler (GValue* value, gpointer v_object);
gpointer vala_value_get_ccode_compiler (const GValue* value);
GType vala_ccode_compiler_get_type (void) G_GNUC_CONST;
enum  {
	VALA_CCODE_COMPILER_DUMMY_PROPERTY
};
ValaCCodeCompiler* vala_ccode_compiler_new (void);
ValaCCodeCompiler* vala_ccode_compiler_construct (GType object_type);
static gboolean vala_ccode_compiler_package_exists (const gchar* package_name);
void vala_ccode_compiler_compile (ValaCCodeCompiler* self, ValaCodeContext* context, const gchar* cc_command, gchar** cc_options, int cc_options_length1);
static void vala_ccode_compiler_finalize (ValaCCodeCompiler* obj);


ValaCCodeCompiler* vala_ccode_compiler_construct (GType object_type) {
	ValaCCodeCompiler* self = NULL;
	self = (ValaCCodeCompiler*) g_type_create_instance (object_type);
	return self;
}


ValaCCodeCompiler* vala_ccode_compiler_new (void) {
	return vala_ccode_compiler_construct (VALA_TYPE_CCODE_COMPILER);
}


static gboolean vala_ccode_compiler_package_exists (const gchar* package_name) {
	gboolean result = FALSE;
	const gchar* _tmp0_;
	gchar* _tmp1_;
	gchar* pc;
	gint exit_status = 0;
	GError * _inner_error_ = NULL;
	g_return_val_if_fail (package_name != NULL, FALSE);
	_tmp0_ = package_name;
	_tmp1_ = g_strconcat ("pkg-config --exists ", _tmp0_, NULL);
	pc = _tmp1_;
	{
		gint _tmp2_ = 0;
		g_spawn_command_line_sync (pc, NULL, NULL, &_tmp2_, &_inner_error_);
		exit_status = _tmp2_;
		if (_inner_error_ != NULL) {
			if (_inner_error_->domain == G_SPAWN_ERROR) {
				goto __catch1_g_spawn_error;
			}
			_g_free0 (pc);
			g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
			g_clear_error (&_inner_error_);
			return FALSE;
		}
		result = 0 == exit_status;
		_g_free0 (pc);
		return result;
	}
	goto __finally1;
	__catch1_g_spawn_error:
	{
		GError* e = NULL;
		GError* _tmp3_;
		const gchar* _tmp4_;
		e = _inner_error_;
		_inner_error_ = NULL;
		_tmp3_ = e;
		_tmp4_ = _tmp3_->message;
		vala_report_error (NULL, _tmp4_);
		result = FALSE;
		_g_error_free0 (e);
		_g_free0 (pc);
		return result;
	}
	__finally1:
	_g_free0 (pc);
	g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
	g_clear_error (&_inner_error_);
	return FALSE;
}


/**
 * Compile generated C code to object code and optionally link object
 * files.
 *
 * @param context a code context
 */
static gpointer _vala_iterable_ref0 (gpointer self) {
	return self ? vala_iterable_ref (self) : NULL;
}


static gchar* string_strip (const gchar* self) {
	gchar* result = NULL;
	gchar* _tmp0_ = NULL;
	gchar* _result_;
	const gchar* _tmp1_;
	g_return_val_if_fail (self != NULL, NULL);
	_tmp0_ = g_strdup (self);
	_result_ = _tmp0_;
	_tmp1_ = _result_;
	g_strstrip (_tmp1_);
	result = _result_;
	return result;
}


void vala_ccode_compiler_compile (ValaCCodeCompiler* self, ValaCodeContext* context, const gchar* cc_command, gchar** cc_options, int cc_options_length1) {
	gboolean use_pkgconfig;
	gchar* _tmp0_;
	gchar* pc;
	ValaCodeContext* _tmp1_;
	gboolean _tmp2_;
	gboolean _tmp3_;
	const gchar* _tmp6_;
	gchar* _tmp7_;
	ValaCodeContext* _tmp8_;
	gboolean _tmp9_;
	gboolean _tmp10_;
	gchar* _tmp31_;
	gchar* pkgflags;
	gboolean _tmp32_;
	const gchar* _tmp42_;
	const gchar* _tmp43_;
	gchar* _tmp44_;
	gchar* cmdline;
	ValaCodeContext* _tmp45_;
	gboolean _tmp46_;
	gboolean _tmp47_;
	ValaCodeContext* _tmp50_;
	gboolean _tmp51_;
	gboolean _tmp52_;
	ValaCodeContext* _tmp90_;
	ValaList* _tmp91_ = NULL;
	ValaList* source_files;
	ValaCodeContext* _tmp115_;
	ValaList* _tmp116_ = NULL;
	ValaList* c_source_files;
	const gchar* _tmp135_;
	const gchar* _tmp136_;
	gchar* _tmp137_ = NULL;
	gchar* _tmp138_;
	gchar* _tmp139_;
	gchar* _tmp140_;
	gchar* _tmp141_;
	gchar** _tmp142_;
	gint _tmp142__length1;
	ValaCodeContext* _tmp151_;
	gboolean _tmp152_;
	gboolean _tmp153_;
	GError * _inner_error_ = NULL;
	g_return_if_fail (self != NULL);
	g_return_if_fail (context != NULL);
	use_pkgconfig = FALSE;
	_tmp0_ = g_strdup ("pkg-config --cflags");
	pc = _tmp0_;
	_tmp1_ = context;
	_tmp2_ = vala_code_context_get_compile_only (_tmp1_);
	_tmp3_ = _tmp2_;
	if (!_tmp3_) {
		const gchar* _tmp4_;
		gchar* _tmp5_;
		_tmp4_ = pc;
		_tmp5_ = g_strconcat (_tmp4_, " --libs", NULL);
		_g_free0 (pc);
		pc = _tmp5_;
	}
	use_pkgconfig = TRUE;
	_tmp6_ = pc;
	_tmp7_ = g_strconcat (_tmp6_, " gobject-2.0", NULL);
	_g_free0 (pc);
	pc = _tmp7_;
	_tmp8_ = context;
	_tmp9_ = vala_code_context_get_thread (_tmp8_);
	_tmp10_ = _tmp9_;
	if (_tmp10_) {
		const gchar* _tmp11_;
		gchar* _tmp12_;
		_tmp11_ = pc;
		_tmp12_ = g_strconcat (_tmp11_, " gthread-2.0", NULL);
		_g_free0 (pc);
		pc = _tmp12_;
	}
	{
		ValaCodeContext* _tmp13_;
		ValaList* _tmp14_ = NULL;
		ValaList* _pkg_list;
		ValaList* _tmp15_;
		gint _tmp16_;
		gint _tmp17_;
		gint _pkg_size;
		gint _pkg_index;
		_tmp13_ = context;
		_tmp14_ = vala_code_context_get_packages (_tmp13_);
		_pkg_list = _tmp14_;
		_tmp15_ = _pkg_list;
		_tmp16_ = vala_collection_get_size ((ValaCollection*) _tmp15_);
		_tmp17_ = _tmp16_;
		_pkg_size = _tmp17_;
		_pkg_index = -1;
		while (TRUE) {
			gint _tmp18_;
			gint _tmp19_;
			gint _tmp20_;
			ValaList* _tmp21_;
			gint _tmp22_;
			gpointer _tmp23_ = NULL;
			gchar* pkg;
			const gchar* _tmp24_;
			gboolean _tmp25_ = FALSE;
			_tmp18_ = _pkg_index;
			_pkg_index = _tmp18_ + 1;
			_tmp19_ = _pkg_index;
			_tmp20_ = _pkg_size;
			if (!(_tmp19_ < _tmp20_)) {
				break;
			}
			_tmp21_ = _pkg_list;
			_tmp22_ = _pkg_index;
			_tmp23_ = vala_list_get (_tmp21_, _tmp22_);
			pkg = (gchar*) _tmp23_;
			_tmp24_ = pkg;
			_tmp25_ = vala_ccode_compiler_package_exists (_tmp24_);
			if (_tmp25_) {
				const gchar* _tmp26_;
				const gchar* _tmp27_;
				gchar* _tmp28_;
				gchar* _tmp29_;
				gchar* _tmp30_;
				use_pkgconfig = TRUE;
				_tmp26_ = pc;
				_tmp27_ = pkg;
				_tmp28_ = g_strconcat (" ", _tmp27_, NULL);
				_tmp29_ = _tmp28_;
				_tmp30_ = g_strconcat (_tmp26_, _tmp29_, NULL);
				_g_free0 (pc);
				pc = _tmp30_;
				_g_free0 (_tmp29_);
			}
			_g_free0 (pkg);
		}
		_vala_iterable_unref0 (_pkg_list);
	}
	_tmp31_ = g_strdup ("");
	pkgflags = _tmp31_;
	_tmp32_ = use_pkgconfig;
	if (_tmp32_) {
		{
			gint exit_status = 0;
			const gchar* _tmp33_;
			gchar* _tmp34_ = NULL;
			gint _tmp35_ = 0;
			gint _tmp36_;
			_tmp33_ = pc;
			g_spawn_command_line_sync (_tmp33_, &_tmp34_, NULL, &_tmp35_, &_inner_error_);
			_g_free0 (pkgflags);
			pkgflags = _tmp34_;
			exit_status = _tmp35_;
			if (_inner_error_ != NULL) {
				if (_inner_error_->domain == G_SPAWN_ERROR) {
					goto __catch2_g_spawn_error;
				}
				_g_free0 (pkgflags);
				_g_free0 (pc);
				g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
				g_clear_error (&_inner_error_);
				return;
			}
			_tmp36_ = exit_status;
			if (_tmp36_ != 0) {
				gint _tmp37_;
				gchar* _tmp38_ = NULL;
				gchar* _tmp39_;
				_tmp37_ = exit_status;
				_tmp38_ = g_strdup_printf ("pkg-config exited with status %d", _tmp37_);
				_tmp39_ = _tmp38_;
				vala_report_error (NULL, _tmp39_);
				_g_free0 (_tmp39_);
				_g_free0 (pkgflags);
				_g_free0 (pc);
				return;
			}
		}
		goto __finally2;
		__catch2_g_spawn_error:
		{
			GError* e = NULL;
			GError* _tmp40_;
			const gchar* _tmp41_;
			e = _inner_error_;
			_inner_error_ = NULL;
			_tmp40_ = e;
			_tmp41_ = _tmp40_->message;
			vala_report_error (NULL, _tmp41_);
			_g_error_free0 (e);
			_g_free0 (pkgflags);
			_g_free0 (pc);
			return;
		}
		__finally2:
		if (_inner_error_ != NULL) {
			_g_free0 (pkgflags);
			_g_free0 (pc);
			g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
			g_clear_error (&_inner_error_);
			return;
		}
	}
	_tmp42_ = cc_command;
	if (_tmp42_ == NULL) {
		cc_command = "cc";
	}
	_tmp43_ = cc_command;
	_tmp44_ = g_strdup (_tmp43_);
	cmdline = _tmp44_;
	_tmp45_ = context;
	_tmp46_ = vala_code_context_get_debug (_tmp45_);
	_tmp47_ = _tmp46_;
	if (_tmp47_) {
		const gchar* _tmp48_;
		gchar* _tmp49_;
		_tmp48_ = cmdline;
		_tmp49_ = g_strconcat (_tmp48_, " -g", NULL);
		_g_free0 (cmdline);
		cmdline = _tmp49_;
	}
	_tmp50_ = context;
	_tmp51_ = vala_code_context_get_compile_only (_tmp50_);
	_tmp52_ = _tmp51_;
	if (_tmp52_) {
		const gchar* _tmp53_;
		gchar* _tmp54_;
		_tmp53_ = cmdline;
		_tmp54_ = g_strconcat (_tmp53_, " -c", NULL);
		_g_free0 (cmdline);
		cmdline = _tmp54_;
	} else {
		ValaCodeContext* _tmp55_;
		const gchar* _tmp56_;
		const gchar* _tmp57_;
		_tmp55_ = context;
		_tmp56_ = vala_code_context_get_output (_tmp55_);
		_tmp57_ = _tmp56_;
		if (_tmp57_ != NULL) {
			ValaCodeContext* _tmp58_;
			const gchar* _tmp59_;
			const gchar* _tmp60_;
			gchar* _tmp61_;
			gchar* output;
			gboolean _tmp62_ = FALSE;
			gboolean _tmp63_ = FALSE;
			ValaCodeContext* _tmp64_;
			const gchar* _tmp65_;
			const gchar* _tmp66_;
			gboolean _tmp70_;
			gboolean _tmp75_;
			const gchar* _tmp83_;
			const gchar* _tmp84_;
			gchar* _tmp85_ = NULL;
			gchar* _tmp86_;
			gchar* _tmp87_;
			gchar* _tmp88_;
			gchar* _tmp89_;
			_tmp58_ = context;
			_tmp59_ = vala_code_context_get_output (_tmp58_);
			_tmp60_ = _tmp59_;
			_tmp61_ = g_strdup (_tmp60_);
			output = _tmp61_;
			_tmp64_ = context;
			_tmp65_ = vala_code_context_get_directory (_tmp64_);
			_tmp66_ = _tmp65_;
			if (_tmp66_ != NULL) {
				ValaCodeContext* _tmp67_;
				const gchar* _tmp68_;
				const gchar* _tmp69_;
				_tmp67_ = context;
				_tmp68_ = vala_code_context_get_directory (_tmp67_);
				_tmp69_ = _tmp68_;
				_tmp63_ = g_strcmp0 (_tmp69_, "") != 0;
			} else {
				_tmp63_ = FALSE;
			}
			_tmp70_ = _tmp63_;
			if (_tmp70_) {
				ValaCodeContext* _tmp71_;
				const gchar* _tmp72_;
				const gchar* _tmp73_;
				gboolean _tmp74_ = FALSE;
				_tmp71_ = context;
				_tmp72_ = vala_code_context_get_output (_tmp71_);
				_tmp73_ = _tmp72_;
				_tmp74_ = g_path_is_absolute (_tmp73_);
				_tmp62_ = !_tmp74_;
			} else {
				_tmp62_ = FALSE;
			}
			_tmp75_ = _tmp62_;
			if (_tmp75_) {
				ValaCodeContext* _tmp76_;
				const gchar* _tmp77_;
				const gchar* _tmp78_;
				ValaCodeContext* _tmp79_;
				const gchar* _tmp80_;
				const gchar* _tmp81_;
				gchar* _tmp82_ = NULL;
				_tmp76_ = context;
				_tmp77_ = vala_code_context_get_directory (_tmp76_);
				_tmp78_ = _tmp77_;
				_tmp79_ = context;
				_tmp80_ = vala_code_context_get_output (_tmp79_);
				_tmp81_ = _tmp80_;
				_tmp82_ = g_strdup_printf ("%s%c%s", _tmp78_, (gint) G_DIR_SEPARATOR, _tmp81_);
				_g_free0 (output);
				output = _tmp82_;
			}
			_tmp83_ = cmdline;
			_tmp84_ = output;
			_tmp85_ = g_shell_quote (_tmp84_);
			_tmp86_ = _tmp85_;
			_tmp87_ = g_strconcat (" -o ", _tmp86_, NULL);
			_tmp88_ = _tmp87_;
			_tmp89_ = g_strconcat (_tmp83_, _tmp88_, NULL);
			_g_free0 (cmdline);
			cmdline = _tmp89_;
			_g_free0 (_tmp88_);
			_g_free0 (_tmp86_);
			_g_free0 (output);
		}
	}
	_tmp90_ = context;
	_tmp91_ = vala_code_context_get_source_files (_tmp90_);
	source_files = _tmp91_;
	{
		ValaList* _tmp92_;
		ValaList* _tmp93_;
		ValaList* _file_list;
		ValaList* _tmp94_;
		gint _tmp95_;
		gint _tmp96_;
		gint _file_size;
		gint _file_index;
		_tmp92_ = source_files;
		_tmp93_ = _vala_iterable_ref0 (_tmp92_);
		_file_list = _tmp93_;
		_tmp94_ = _file_list;
		_tmp95_ = vala_collection_get_size ((ValaCollection*) _tmp94_);
		_tmp96_ = _tmp95_;
		_file_size = _tmp96_;
		_file_index = -1;
		while (TRUE) {
			gint _tmp97_;
			gint _tmp98_;
			gint _tmp99_;
			ValaList* _tmp100_;
			gint _tmp101_;
			gpointer _tmp102_ = NULL;
			ValaSourceFile* file;
			ValaSourceFile* _tmp103_;
			ValaSourceFileType _tmp104_;
			ValaSourceFileType _tmp105_;
			_tmp97_ = _file_index;
			_file_index = _tmp97_ + 1;
			_tmp98_ = _file_index;
			_tmp99_ = _file_size;
			if (!(_tmp98_ < _tmp99_)) {
				break;
			}
			_tmp100_ = _file_list;
			_tmp101_ = _file_index;
			_tmp102_ = vala_list_get (_tmp100_, _tmp101_);
			file = (ValaSourceFile*) _tmp102_;
			_tmp103_ = file;
			_tmp104_ = vala_source_file_get_file_type (_tmp103_);
			_tmp105_ = _tmp104_;
			if (_tmp105_ == VALA_SOURCE_FILE_TYPE_SOURCE) {
				const gchar* _tmp106_;
				ValaSourceFile* _tmp107_;
				gchar* _tmp108_ = NULL;
				gchar* _tmp109_;
				gchar* _tmp110_ = NULL;
				gchar* _tmp111_;
				gchar* _tmp112_;
				gchar* _tmp113_;
				gchar* _tmp114_;
				_tmp106_ = cmdline;
				_tmp107_ = file;
				_tmp108_ = vala_source_file_get_csource_filename (_tmp107_);
				_tmp109_ = _tmp108_;
				_tmp110_ = g_shell_quote (_tmp109_);
				_tmp111_ = _tmp110_;
				_tmp112_ = g_strconcat (" ", _tmp111_, NULL);
				_tmp113_ = _tmp112_;
				_tmp114_ = g_strconcat (_tmp106_, _tmp113_, NULL);
				_g_free0 (cmdline);
				cmdline = _tmp114_;
				_g_free0 (_tmp113_);
				_g_free0 (_tmp111_);
				_g_free0 (_tmp109_);
			}
			_vala_source_file_unref0 (file);
		}
		_vala_iterable_unref0 (_file_list);
	}
	_tmp115_ = context;
	_tmp116_ = vala_code_context_get_c_source_files (_tmp115_);
	c_source_files = _tmp116_;
	{
		ValaList* _tmp117_;
		ValaList* _tmp118_;
		ValaList* _file_list;
		ValaList* _tmp119_;
		gint _tmp120_;
		gint _tmp121_;
		gint _file_size;
		gint _file_index;
		_tmp117_ = c_source_files;
		_tmp118_ = _vala_iterable_ref0 (_tmp117_);
		_file_list = _tmp118_;
		_tmp119_ = _file_list;
		_tmp120_ = vala_collection_get_size ((ValaCollection*) _tmp119_);
		_tmp121_ = _tmp120_;
		_file_size = _tmp121_;
		_file_index = -1;
		while (TRUE) {
			gint _tmp122_;
			gint _tmp123_;
			gint _tmp124_;
			ValaList* _tmp125_;
			gint _tmp126_;
			gpointer _tmp127_ = NULL;
			gchar* file;
			const gchar* _tmp128_;
			const gchar* _tmp129_;
			gchar* _tmp130_ = NULL;
			gchar* _tmp131_;
			gchar* _tmp132_;
			gchar* _tmp133_;
			gchar* _tmp134_;
			_tmp122_ = _file_index;
			_file_index = _tmp122_ + 1;
			_tmp123_ = _file_index;
			_tmp124_ = _file_size;
			if (!(_tmp123_ < _tmp124_)) {
				break;
			}
			_tmp125_ = _file_list;
			_tmp126_ = _file_index;
			_tmp127_ = vala_list_get (_tmp125_, _tmp126_);
			file = (gchar*) _tmp127_;
			_tmp128_ = cmdline;
			_tmp129_ = file;
			_tmp130_ = g_shell_quote (_tmp129_);
			_tmp131_ = _tmp130_;
			_tmp132_ = g_strconcat (" ", _tmp131_, NULL);
			_tmp133_ = _tmp132_;
			_tmp134_ = g_strconcat (_tmp128_, _tmp133_, NULL);
			_g_free0 (cmdline);
			cmdline = _tmp134_;
			_g_free0 (_tmp133_);
			_g_free0 (_tmp131_);
			_g_free0 (file);
		}
		_vala_iterable_unref0 (_file_list);
	}
	_tmp135_ = cmdline;
	_tmp136_ = pkgflags;
	_tmp137_ = string_strip (_tmp136_);
	_tmp138_ = _tmp137_;
	_tmp139_ = g_strconcat (" ", _tmp138_, NULL);
	_tmp140_ = _tmp139_;
	_tmp141_ = g_strconcat (_tmp135_, _tmp140_, NULL);
	_g_free0 (cmdline);
	cmdline = _tmp141_;
	_g_free0 (_tmp140_);
	_g_free0 (_tmp138_);
	_tmp142_ = cc_options;
	_tmp142__length1 = cc_options_length1;
	{
		gchar** cc_option_collection = NULL;
		gint cc_option_collection_length1 = 0;
		gint _cc_option_collection_size_ = 0;
		gint cc_option_it = 0;
		cc_option_collection = _tmp142_;
		cc_option_collection_length1 = _tmp142__length1;
		for (cc_option_it = 0; cc_option_it < _tmp142__length1; cc_option_it = cc_option_it + 1) {
			gchar* _tmp143_;
			gchar* cc_option = NULL;
			_tmp143_ = g_strdup (cc_option_collection[cc_option_it]);
			cc_option = _tmp143_;
			{
				const gchar* _tmp144_;
				const gchar* _tmp145_;
				gchar* _tmp146_ = NULL;
				gchar* _tmp147_;
				gchar* _tmp148_;
				gchar* _tmp149_;
				gchar* _tmp150_;
				_tmp144_ = cmdline;
				_tmp145_ = cc_option;
				_tmp146_ = g_shell_quote (_tmp145_);
				_tmp147_ = _tmp146_;
				_tmp148_ = g_strconcat (" ", _tmp147_, NULL);
				_tmp149_ = _tmp148_;
				_tmp150_ = g_strconcat (_tmp144_, _tmp149_, NULL);
				_g_free0 (cmdline);
				cmdline = _tmp150_;
				_g_free0 (_tmp149_);
				_g_free0 (_tmp147_);
				_g_free0 (cc_option);
			}
		}
	}
	_tmp151_ = context;
	_tmp152_ = vala_code_context_get_verbose_mode (_tmp151_);
	_tmp153_ = _tmp152_;
	if (_tmp153_) {
		FILE* _tmp154_;
		const gchar* _tmp155_;
		_tmp154_ = stdout;
		_tmp155_ = cmdline;
		fprintf (_tmp154_, "%s\n", _tmp155_);
	}
	{
		gint exit_status = 0;
		const gchar* _tmp156_;
		gint _tmp157_ = 0;
		gint _tmp158_;
		_tmp156_ = cmdline;
		g_spawn_command_line_sync (_tmp156_, NULL, NULL, &_tmp157_, &_inner_error_);
		exit_status = _tmp157_;
		if (_inner_error_ != NULL) {
			if (_inner_error_->domain == G_SPAWN_ERROR) {
				goto __catch3_g_spawn_error;
			}
			_vala_iterable_unref0 (c_source_files);
			_vala_iterable_unref0 (source_files);
			_g_free0 (cmdline);
			_g_free0 (pkgflags);
			_g_free0 (pc);
			g_critical ("file %s: line %d: unexpected error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
			g_clear_error (&_inner_error_);
			return;
		}
		_tmp158_ = exit_status;
		if (_tmp158_ != 0) {
			gint _tmp159_;
			gchar* _tmp160_ = NULL;
			gchar* _tmp161_;
			_tmp159_ = exit_status;
			_tmp160_ = g_strdup_printf ("cc exited with status %d", _tmp159_);
			_tmp161_ = _tmp160_;
			vala_report_error (NULL, _tmp161_);
			_g_free0 (_tmp161_);
		}
	}
	goto __finally3;
	__catch3_g_spawn_error:
	{
		GError* e = NULL;
		GError* _tmp162_;
		const gchar* _tmp163_;
		e = _inner_error_;
		_inner_error_ = NULL;
		_tmp162_ = e;
		_tmp163_ = _tmp162_->message;
		vala_report_error (NULL, _tmp163_);
		_g_error_free0 (e);
	}
	__finally3:
	if (_inner_error_ != NULL) {
		_vala_iterable_unref0 (c_source_files);
		_vala_iterable_unref0 (source_files);
		_g_free0 (cmdline);
		_g_free0 (pkgflags);
		_g_free0 (pc);
		g_critical ("file %s: line %d: uncaught error: %s (%s, %d)", __FILE__, __LINE__, _inner_error_->message, g_quark_to_string (_inner_error_->domain), _inner_error_->code);
		g_clear_error (&_inner_error_);
		return;
	}
	{
		ValaList* _tmp164_;
		ValaList* _tmp165_;
		ValaList* _file_list;
		ValaList* _tmp166_;
		gint _tmp167_;
		gint _tmp168_;
		gint _file_size;
		gint _file_index;
		_tmp164_ = source_files;
		_tmp165_ = _vala_iterable_ref0 (_tmp164_);
		_file_list = _tmp165_;
		_tmp166_ = _file_list;
		_tmp167_ = vala_collection_get_size ((ValaCollection*) _tmp166_);
		_tmp168_ = _tmp167_;
		_file_size = _tmp168_;
		_file_index = -1;
		while (TRUE) {
			gint _tmp169_;
			gint _tmp170_;
			gint _tmp171_;
			ValaList* _tmp172_;
			gint _tmp173_;
			gpointer _tmp174_ = NULL;
			ValaSourceFile* file;
			ValaSourceFile* _tmp175_;
			ValaSourceFileType _tmp176_;
			ValaSourceFileType _tmp177_;
			_tmp169_ = _file_index;
			_file_index = _tmp169_ + 1;
			_tmp170_ = _file_index;
			_tmp171_ = _file_size;
			if (!(_tmp170_ < _tmp171_)) {
				break;
			}
			_tmp172_ = _file_list;
			_tmp173_ = _file_index;
			_tmp174_ = vala_list_get (_tmp172_, _tmp173_);
			file = (ValaSourceFile*) _tmp174_;
			_tmp175_ = file;
			_tmp176_ = vala_source_file_get_file_type (_tmp175_);
			_tmp177_ = _tmp176_;
			if (_tmp177_ == VALA_SOURCE_FILE_TYPE_SOURCE) {
				ValaCodeContext* _tmp178_;
				gboolean _tmp179_;
				gboolean _tmp180_;
				_tmp178_ = context;
				_tmp179_ = vala_code_context_get_save_csources (_tmp178_);
				_tmp180_ = _tmp179_;
				if (!_tmp180_) {
					ValaSourceFile* _tmp181_;
					gchar* _tmp182_ = NULL;
					gchar* _tmp183_;
					_tmp181_ = file;
					_tmp182_ = vala_source_file_get_csource_filename (_tmp181_);
					_tmp183_ = _tmp182_;
					g_unlink (_tmp183_);
					_g_free0 (_tmp183_);
				}
			}
			_vala_source_file_unref0 (file);
		}
		_vala_iterable_unref0 (_file_list);
	}
	_vala_iterable_unref0 (c_source_files);
	_vala_iterable_unref0 (source_files);
	_g_free0 (cmdline);
	_g_free0 (pkgflags);
	_g_free0 (pc);
}


static void vala_value_ccode_compiler_init (GValue* value) {
	value->data[0].v_pointer = NULL;
}


static void vala_value_ccode_compiler_free_value (GValue* value) {
	if (value->data[0].v_pointer) {
		vala_ccode_compiler_unref (value->data[0].v_pointer);
	}
}


static void vala_value_ccode_compiler_copy_value (const GValue* src_value, GValue* dest_value) {
	if (src_value->data[0].v_pointer) {
		dest_value->data[0].v_pointer = vala_ccode_compiler_ref (src_value->data[0].v_pointer);
	} else {
		dest_value->data[0].v_pointer = NULL;
	}
}


static gpointer vala_value_ccode_compiler_peek_pointer (const GValue* value) {
	return value->data[0].v_pointer;
}


static gchar* vala_value_ccode_compiler_collect_value (GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	if (collect_values[0].v_pointer) {
		ValaCCodeCompiler* object;
		object = collect_values[0].v_pointer;
		if (object->parent_instance.g_class == NULL) {
			return g_strconcat ("invalid unclassed object pointer for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		} else if (!g_value_type_compatible (G_TYPE_FROM_INSTANCE (object), G_VALUE_TYPE (value))) {
			return g_strconcat ("invalid object type `", g_type_name (G_TYPE_FROM_INSTANCE (object)), "' for value type `", G_VALUE_TYPE_NAME (value), "'", NULL);
		}
		value->data[0].v_pointer = vala_ccode_compiler_ref (object);
	} else {
		value->data[0].v_pointer = NULL;
	}
	return NULL;
}


static gchar* vala_value_ccode_compiler_lcopy_value (const GValue* value, guint n_collect_values, GTypeCValue* collect_values, guint collect_flags) {
	ValaCCodeCompiler** object_p;
	object_p = collect_values[0].v_pointer;
	if (!object_p) {
		return g_strdup_printf ("value location for `%s' passed as NULL", G_VALUE_TYPE_NAME (value));
	}
	if (!value->data[0].v_pointer) {
		*object_p = NULL;
	} else if (collect_flags & G_VALUE_NOCOPY_CONTENTS) {
		*object_p = value->data[0].v_pointer;
	} else {
		*object_p = vala_ccode_compiler_ref (value->data[0].v_pointer);
	}
	return NULL;
}


GParamSpec* vala_param_spec_ccode_compiler (const gchar* name, const gchar* nick, const gchar* blurb, GType object_type, GParamFlags flags) {
	ValaParamSpecCCodeCompiler* spec;
	g_return_val_if_fail (g_type_is_a (object_type, VALA_TYPE_CCODE_COMPILER), NULL);
	spec = g_param_spec_internal (G_TYPE_PARAM_OBJECT, name, nick, blurb, flags);
	G_PARAM_SPEC (spec)->value_type = object_type;
	return G_PARAM_SPEC (spec);
}


gpointer vala_value_get_ccode_compiler (const GValue* value) {
	g_return_val_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, VALA_TYPE_CCODE_COMPILER), NULL);
	return value->data[0].v_pointer;
}


void vala_value_set_ccode_compiler (GValue* value, gpointer v_object) {
	ValaCCodeCompiler* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, VALA_TYPE_CCODE_COMPILER));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, VALA_TYPE_CCODE_COMPILER));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
		vala_ccode_compiler_ref (value->data[0].v_pointer);
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		vala_ccode_compiler_unref (old);
	}
}


void vala_value_take_ccode_compiler (GValue* value, gpointer v_object) {
	ValaCCodeCompiler* old;
	g_return_if_fail (G_TYPE_CHECK_VALUE_TYPE (value, VALA_TYPE_CCODE_COMPILER));
	old = value->data[0].v_pointer;
	if (v_object) {
		g_return_if_fail (G_TYPE_CHECK_INSTANCE_TYPE (v_object, VALA_TYPE_CCODE_COMPILER));
		g_return_if_fail (g_value_type_compatible (G_TYPE_FROM_INSTANCE (v_object), G_VALUE_TYPE (value)));
		value->data[0].v_pointer = v_object;
	} else {
		value->data[0].v_pointer = NULL;
	}
	if (old) {
		vala_ccode_compiler_unref (old);
	}
}


static void vala_ccode_compiler_class_init (ValaCCodeCompilerClass * klass) {
	vala_ccode_compiler_parent_class = g_type_class_peek_parent (klass);
	VALA_CCODE_COMPILER_CLASS (klass)->finalize = vala_ccode_compiler_finalize;
}


static void vala_ccode_compiler_instance_init (ValaCCodeCompiler * self) {
	self->ref_count = 1;
}


static void vala_ccode_compiler_finalize (ValaCCodeCompiler* obj) {
	ValaCCodeCompiler * self;
	self = G_TYPE_CHECK_INSTANCE_CAST (obj, VALA_TYPE_CCODE_COMPILER, ValaCCodeCompiler);
}


/**
 * Interface to the C compiler.
 */
GType vala_ccode_compiler_get_type (void) {
	static volatile gsize vala_ccode_compiler_type_id__volatile = 0;
	if (g_once_init_enter (&vala_ccode_compiler_type_id__volatile)) {
		static const GTypeValueTable g_define_type_value_table = { vala_value_ccode_compiler_init, vala_value_ccode_compiler_free_value, vala_value_ccode_compiler_copy_value, vala_value_ccode_compiler_peek_pointer, "p", vala_value_ccode_compiler_collect_value, "p", vala_value_ccode_compiler_lcopy_value };
		static const GTypeInfo g_define_type_info = { sizeof (ValaCCodeCompilerClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) vala_ccode_compiler_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (ValaCCodeCompiler), 0, (GInstanceInitFunc) vala_ccode_compiler_instance_init, &g_define_type_value_table };
		static const GTypeFundamentalInfo g_define_type_fundamental_info = { (G_TYPE_FLAG_CLASSED | G_TYPE_FLAG_INSTANTIATABLE | G_TYPE_FLAG_DERIVABLE | G_TYPE_FLAG_DEEP_DERIVABLE) };
		GType vala_ccode_compiler_type_id;
		vala_ccode_compiler_type_id = g_type_register_fundamental (g_type_fundamental_next (), "ValaCCodeCompiler", &g_define_type_info, &g_define_type_fundamental_info, 0);
		g_once_init_leave (&vala_ccode_compiler_type_id__volatile, vala_ccode_compiler_type_id);
	}
	return vala_ccode_compiler_type_id__volatile;
}


gpointer vala_ccode_compiler_ref (gpointer instance) {
	ValaCCodeCompiler* self;
	self = instance;
	g_atomic_int_inc (&self->ref_count);
	return instance;
}


void vala_ccode_compiler_unref (gpointer instance) {
	ValaCCodeCompiler* self;
	self = instance;
	if (g_atomic_int_dec_and_test (&self->ref_count)) {
		VALA_CCODE_COMPILER_GET_CLASS (self)->finalize (self);
		g_type_free_instance ((GTypeInstance *) self);
	}
}



