#include <stdio.h>
#include <stdlib.h>

#include <idna.h>
#include <jit/jit.h>

int main(int argc, char *argv[]) {
	char *out_string;
	jit_context_t ctx;
	jit_function_t fnc;
	jit_value_t input, buffer, buffer_ptr, flags, ext_args[3];
	jit_type_t sig, ext_sig, prm[] = {
		jit_type_void_ptr,
	}, ext_prm[] = {
		jit_type_void_ptr,
		jit_type_void_ptr,
		jit_type_sys_int
	};
	void *args[1];
	char *(*closure)(char *);

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <domain>\n", argv[0]);
		return EXIT_FAILURE;
	}
	args[0] = &argv[1];

	jit_init();

	ctx = jit_context_create();

	jit_context_build_start(ctx);
	sig = jit_type_create_signature(jit_abi_cdecl, jit_type_void_ptr, prm, 1, 1);
	fnc = jit_function_create(ctx, sig);

	flags = jit_value_create_nint_constant(fnc, jit_type_sys_int, 0);
	buffer = jit_value_create(fnc, jit_type_void_ptr);
	jit_value_set_addressable(buffer);
	buffer_ptr = jit_insn_address_of(fnc, buffer);
	input = jit_value_get_param(fnc, 0);

	ext_args[0] = input;
	ext_args[1] = buffer_ptr;
	ext_args[2] = flags;
	ext_sig = jit_type_create_signature(jit_abi_cdecl, jit_type_sys_int, ext_prm, 3, 1);
	(void)jit_insn_call_native(fnc, "idna_to_ascii_8z", &idna_to_ascii_8z, ext_sig, ext_args, 3, 0);

	jit_insn_return(fnc, buffer);
	jit_function_compile(fnc);
	jit_context_build_end(ctx);

	jit_function_apply(fnc, args, &out_string);
	printf("= %s\n", out_string);
	free(out_string);

	closure = jit_function_to_closure(fnc);
	out_string = closure(argv[1]);
	printf("= %s\n", out_string);
	free(out_string);

	jit_type_free(sig);
	jit_type_free(ext_sig);
	jit_context_destroy(ctx);

	return EXIT_SUCCESS;
}
