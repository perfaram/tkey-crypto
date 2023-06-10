#include "munit/munit.h"
#include "munit_vectors/munit_vectors.h"
#include <stdio.h>
#include <sha1.h>
#include <string.h>

struct t_sha1_vector {
	uint8_t *msg;
	uint32_t msg_len;
	uint32_t repetitions;
	uint8_t hash[SHA1_DIGEST_LEN];
};

static MunitResult
test_sha1_vector(MUNIT_UNUSED const MunitParameter params[], void* user_data)
{
	uint8_t hash[SHA1_DIGEST_LEN];
	const struct t_sha1_vector *vector = (struct t_sha1_vector *)user_data;

	size_t msg_len = vector->msg_len;
	if (msg_len == 0) {
		const char* msg_str = (char*)vector->msg;
		msg_len = strlen(msg_str);
	}
	
	if (vector->repetitions == 0) {
		sha1_complete(hash, (const uint8_t *)vector->msg, msg_len);
	}
	else {
		sha1_ctx cx[1];
		sha1_init(cx);
		for (int i = 0; i < vector->repetitions; i++) {
			sha1_update(cx, (const uint8_t *)vector->msg, msg_len);
		}
		sha1_final(cx, hash);
	}
	
	munit_assert_memory_equal(SHA1_DIGEST_LEN, vector->hash, hash);
	return MUNIT_OK;
}

static MunitParameterEnum test_params[] = {
	{ "vector", NULL }, // actual vectors will be filled by +main
	{ NULL, NULL },
};

static MunitTest test_suite_tests[] = {
	{ (char*) "light", test_sha1_vector, setup_get_vector, NULL, MUNIT_TEST_OPTION_NONE, test_params },
	{ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite sha1_test_suite = {
	(char*) "sha1/",
	test_suite_tests,
	NULL, // don't include other suites
	1,  // # of iterations
	MUNIT_SUITE_OPTION_NONE
};

void vectorfile_parse(const char* filename, size_t *vectors_len, MunitVector** dyn_vectors, const char*** dyn_vectors_names) {
	const size_t filename_len = strlen(filename);
	char path[100];
	build_vectorfile_path(path, sizeof(path), filename);

	char* line_buf = NULL;
	ssize_t line_len;
	size_t line_size = 0;

	FILE* fid = fopen(path, "r");
	munit_assert(fid != NULL);

	MunitVector new_vector_temp;
	struct t_sha1_vector new_vector_temp_data;

	while ((line_len = getline(&line_buf, &line_size, fid)) > 0) {
		//first, trim newline
		if (line_buf[line_len-1] == '\n') {
			line_buf[line_len-1] = '\0';
			line_len -= 1;
		}
		
		int done = parse_line(
			&new_vector_temp_data.msg_len, &new_vector_temp.description,
			&new_vector_temp_data.msg, &new_vector_temp_data.repetitions,
			NULL, NULL,
			new_vector_temp_data.hash, sizeof(new_vector_temp_data.hash),
			filename, filename_len,
			line_buf, line_len);
		if (done) {
			struct t_sha1_vector* new_vector_data = malloc(sizeof(struct t_sha1_vector));
			memcpy(new_vector_data, &new_vector_temp_data, sizeof(struct t_sha1_vector));
			munit_vectors_add(vectors_len, dyn_vectors, dyn_vectors_names, new_vector_temp.description, new_vector_data);
			memset(&new_vector_temp_data, 0, sizeof(new_vector_temp_data));
		}
	}

	free(line_buf);
	fclose(fid);
}

int sha1_main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
	size_t vectors_len = 0;
	const char** dyn_vectors_names = NULL;
	MunitVector* dyn_vectors = NULL;

	vectorfile_parse("sha1_rfc", &vectors_len, &dyn_vectors, &dyn_vectors_names);
	vectorfile_parse("sha1_nist_shortmsg", &vectors_len, &dyn_vectors, &dyn_vectors_names);

	test_params[0].values = dyn_vectors_names;

	return munit_suite_main(&sha1_test_suite, (void*)dyn_vectors, argc, argv);
}

