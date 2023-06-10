#include "munit/munit.h"
#include <errno_compat.h>

typedef struct {
	char *description;
	void *content;
} MunitVector;

void
munit_vectors_extract_names(const MunitVector vectors[], char** vector_names);

const void*
munit_vector_get(const MunitVector vectors[], const char* description);

MunitResult
munit_vectors_add(
	size_t* vectors_size, MunitVector* vectors[MUNIT_ARRAY_PARAM(*vectors_size)],
	const char** vector_names[MUNIT_ARRAY_PARAM(*vectors_size)],
	char* description, void* content);

void*
setup_get_vector(const MunitParameter params[], void* user_data);

errno_t build_vectorfile_path(char* path_buf, size_t path_buf_size, const char* filename);

void parseif_len_description(
	uint32_t *msg_len, char **description_ptr, 
	const char *filename, size_t filename_len, 
	const char *line_buf, size_t line_len);

int parseif_repetitions(
	uint32_t *repetitions, 
	const char *line_buf, size_t line_len);

int parseif_hex(
	uint8_t **msg, size_t msg_len,
	const char *line_buf, size_t line_len,
	const char *prefix, size_t prefix_len);

int parseif_fixed_hex(
	uint8_t buf[], size_t buf_len,
	const char *line_buf, size_t line_len,
	const char *prefix, size_t prefix_len);

int parse_line(
	uint32_t *msg_len, char **description_ptr,
	uint8_t **msg, uint32_t *repetitions,
	uint8_t **key, size_t *key_len,
	uint8_t hash_buf[], size_t hash_buf_size,
	const char *filename, size_t filename_len, 
	const char *line_buf, size_t line_len);
