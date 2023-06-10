#include "munit/munit.h"
#include "munit_vectors/munit_vectors.h"
#include <stdio.h>
#include <hmac_sha1.h>

struct t_hmac_sha1_vector {
	uint8_t *key;
	uint32_t key_len;
	uint8_t *msg;
	uint32_t msg_len;
	uint8_t mac[HMAC_SHA1_MAC_LEN];
};

void hexDump (
    const char * desc,
    const void * addr,
    const int len
) {
    int i;
    const unsigned char * pc = (const unsigned char *)addr;

    // Output description if given.
    if (desc != NULL) printf ("%s: ", desc);

    // Length checks.
    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }

    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);
    }
}

/*
 * Unit test: compute the HMAC signature of the specified string with the
 * specified key and compare it to the expected result.
 */
static MunitResult
test_hmac_sha1_vector(MUNIT_UNUSED const MunitParameter params[], void* user_data)
{
	uint8_t mac[HMAC_SHA1_MAC_LEN];
	const struct t_hmac_sha1_vector *vector = (struct t_hmac_sha1_vector *)user_data;
	
	hmac_sha1_complete(mac, vector->key, vector->key_len,
			(const uint8_t *)vector->msg, vector->msg_len);

	munit_assert_memory_equal(HMAC_SHA1_MAC_LEN, vector->mac, mac);
	return MUNIT_OK;
}

static MunitParameterEnum test_params[] = {
	{ "vector", NULL },
	{ NULL, NULL },
};

static MunitTest test_suite_tests[] = {
	{ (char*) "complete", test_hmac_sha1_vector, setup_get_vector, NULL, MUNIT_TEST_OPTION_NONE, test_params },
	{ NULL, NULL, NULL, NULL, MUNIT_TEST_OPTION_NONE, NULL }
};

static const MunitSuite hmac_sha1_test_suite = {
	(char*) "hmac_sha1/",
	test_suite_tests,
	NULL,	// don't include other suites
	1, 	// # of iterations
	MUNIT_SUITE_OPTION_NONE
};

void vectorfile_parse_hmac(const char* filename, size_t *vectors_len, MunitVector** dyn_vectors, const char*** dyn_vectors_names) {
	const size_t filename_len = strlen(filename);
	char path[100];
	build_vectorfile_path(path, sizeof(path), filename);

	char* line_buf = NULL;
	ssize_t line_len;
	size_t line_size = 0;

	FILE* fid = fopen(path, "r");
	munit_assert(fid != NULL);

	MunitVector new_vector_temp;
	struct t_hmac_sha1_vector new_vector_temp_data;
	memset(&new_vector_temp_data, 0, sizeof(new_vector_temp_data));

	while ((line_len = getline(&line_buf, &line_size, fid)) > 0) {
		//first, trim newline
		if (line_buf[line_len-1] == '\n') {
			line_buf[line_len-1] = '\0';
			line_len -= 1;
		}
		
		int done = parse_line(
			&new_vector_temp_data.msg_len, &new_vector_temp.description,
			&new_vector_temp_data.msg, NULL,
			&new_vector_temp_data.key, &new_vector_temp_data.key_len,
			new_vector_temp_data.mac, sizeof(new_vector_temp_data.mac),
			filename, filename_len,
			line_buf, line_len);

		if (done) {
			struct t_hmac_sha1_vector* new_vector_data = malloc(sizeof(struct t_hmac_sha1_vector));
			memcpy(new_vector_data, &new_vector_temp_data, sizeof(struct t_hmac_sha1_vector));
			munit_vectors_add(vectors_len, dyn_vectors, dyn_vectors_names, new_vector_temp.description, new_vector_data);
			memset(&new_vector_temp_data, 0, sizeof(new_vector_temp_data));
		}
	}

	free(line_buf);
	fclose(fid);
}

int hmac_sha1_main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
	size_t vectors_len = 0;
	const char** dyn_vectors_names = NULL;
	MunitVector* dyn_vectors = NULL;

	vectorfile_parse_hmac("hmac_sha1_pyca", &vectors_len, &dyn_vectors, &dyn_vectors_names);

	test_params[0].values = dyn_vectors_names;

	return munit_suite_main(&hmac_sha1_test_suite, (void*)dyn_vectors, argc, argv);
}
