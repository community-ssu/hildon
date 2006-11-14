
#ifndef ___HILDON_MARSHALERS_H__
#define ___HILDON_MARSHALERS_H__

#include	<glib-object.h>

G_BEGIN_DECLS

/* BOOLEAN:ENUM (hildon-marshalers.list:26) */
extern void _hildon_marshal_BOOLEAN__ENUM (GClosure     *closure,
                                           GValue       *return_value,
                                           guint         n_param_values,
                                           const GValue *param_values,
                                           gpointer      invocation_hint,
                                           gpointer      marshal_data);

/* BOOLEAN:INT,INT,INT (hildon-marshalers.list:27) */
extern void _hildon_marshal_BOOLEAN__INT_INT_INT (GClosure     *closure,
                                                  GValue       *return_value,
                                                  guint         n_param_values,
                                                  const GValue *param_values,
                                                  gpointer      invocation_hint,
                                                  gpointer      marshal_data);

/* VOID:OBJECT (hildon-marshalers.list:28) */
#define _hildon_marshal_VOID__OBJECT	g_cclosure_marshal_VOID__OBJECT

G_END_DECLS

#endif /* __HILDON_MARSHALERS_H__ */

