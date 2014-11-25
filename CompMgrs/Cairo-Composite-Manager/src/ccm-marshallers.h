
#ifndef __ccm_cclosure_marshal_MARSHAL_H__
#define __ccm_cclosure_marshal_MARSHAL_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* VOID:INT,INT (ccm-marshallers.list:1) */
extern void ccm_cclosure_marshal_VOID__INT_INT (GClosure     *closure,
                                                GValue       *return_value,
                                                guint         n_param_values,
                                                const GValue *param_values,
                                                gpointer      invocation_hint,
                                                gpointer      marshal_data);

/* STRING:POINTER (ccm-marshallers.list:2) */
extern void ccm_cclosure_marshal_STRING__POINTER (GClosure     *closure,
                                                  GValue       *return_value,
                                                  guint         n_param_values,
                                                  const GValue *param_values,
                                                  gpointer      invocation_hint,
                                                  gpointer      marshal_data);

/* BOOLEAN:VOID (ccm-marshallers.list:3) */
extern void ccm_cclosure_marshal_BOOLEAN__VOID (GClosure     *closure,
                                                GValue       *return_value,
                                                guint         n_param_values,
                                                const GValue *param_values,
                                                gpointer      invocation_hint,
                                                gpointer      marshal_data);

/* VOID:STRING,BOOLEAN (ccm-marshallers.list:4) */
extern void ccm_cclosure_marshal_VOID__STRING_BOOLEAN (GClosure     *closure,
                                                       GValue       *return_value,
                                                       guint         n_param_values,
                                                       const GValue *param_values,
                                                       gpointer      invocation_hint,
                                                       gpointer      marshal_data);

/* VOID:OBJECT,OBJECT,LONG,LONG,LONG (ccm-marshallers.list:5) */
extern void ccm_cclosure_marshal_VOID__OBJECT_OBJECT_LONG_LONG_LONG (GClosure     *closure,
                                                                     GValue       *return_value,
                                                                     guint         n_param_values,
                                                                     const GValue *param_values,
                                                                     gpointer      invocation_hint,
                                                                     gpointer      marshal_data);

/* VOID:POINTER,POINTER (ccm-marshallers.list:6) */
extern void ccm_cclosure_marshal_VOID__POINTER_POINTER (GClosure     *closure,
                                                        GValue       *return_value,
                                                        guint         n_param_values,
                                                        const GValue *param_values,
                                                        gpointer      invocation_hint,
                                                        gpointer      marshal_data);

G_END_DECLS

#endif /* __ccm_cclosure_marshal_MARSHAL_H__ */

