#ifndef __NBUS_H_c113e79c1f574a37bca726deef8b44ae__
#define __NBUS_H_c113e79c1f574a37bca726deef8b44ae__
/******************************************************************************/

struct nbus_object;
struct nbus_context;

typedef int (*nbus_handler_t)(struct nbus_context *ctx, struct nbus_object *obj,
			      struct ubus_request_data *req,
			      const char *method, struct blob *blob);

struct nbus_method {
	const char *name;
	nbus_handler_t handler;

	const struct blob_rule *rules;
	int n_rules;
};

struct nbus_service {
	uint32 id;
	const char *name;

	const struct nbus_method *methods;
	int n_methods;
};

extern struct nbus_context *
nbus_create(void);

extern int /* service id */
nbus_register_service(struct nbus_context *ctx, struct nbus_service *service);

extern int
nbus_unregister_service(struct nbus_context *ctx, int service_id);

extern int /* service id */
nbus_lookup_service(struct nbus_context *ctx, const char *name);

/******************************************************************************/
#endif /* __NBUS_H_c113e79c1f574a37bca726deef8b44ae__ */
