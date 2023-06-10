#include "munit/munit.h"
#include "munit_vectors.h"
#include <stdio.h>

void
munit_vectors_extract_names(const MunitVector vectors[], char** vector_names) {
	for (int i = 0; ; i++) {
		if (vectors[i].description == NULL) {
			vector_names[i] = NULL;
			break;
		}
		vector_names[i] = (char*)vectors[i].description;
	}
}

const void*
munit_vector_get(const MunitVector* vectors, const char* description) {
	const MunitVector* vector;
	
	for (vector = vectors ; vector != NULL && vector->description != NULL ; vector++) {
		if (strcmp(vector->description, description) == 0)
			return vector->content;
	}
	return NULL;
}

MunitResult
munit_vectors_add(
	size_t* vectors_size, MunitVector* vectors[MUNIT_ARRAY_PARAM(*vectors_size)],
	const char** vector_names[MUNIT_ARRAY_PARAM(*vectors_size)],
	char* description, void* content)
{
  *vectors = realloc(*vectors, sizeof(MunitVector) * (*vectors_size + 2));
  if (*vectors == NULL)
    return MUNIT_ERROR;

  *vector_names = realloc(*vector_names, sizeof(char*) * (*vectors_size + 2));
  if (*vector_names == NULL)
    return MUNIT_ERROR;

  (*vector_names)[*vectors_size] = description;
  (*vectors)[*vectors_size].description = description;
  (*vectors)[*vectors_size].content = content;
  (*vectors_size)++;
  (*vector_names)[*vectors_size] = NULL;
  (*vectors)[*vectors_size].description = NULL;
  (*vectors)[*vectors_size].content = NULL;

  return MUNIT_OK;
}

void*
setup_get_vector(const MunitParameter params[], void* user_data) {
	const MunitVector* vectors = (const MunitVector*)user_data;

	const char *vector_name = munit_parameters_get(params, "vector");
	const struct t_vector *vector = (struct t_vector *)munit_vector_get(vectors, vector_name);
	return (void*)vector;
}

errno_t build_vectorfile_path(char* path_buf, size_t path_buf_size, const char* filename) {
	const char folder[] = "vectors/";
	const size_t folder_len = strlen(folder);
	const char extension[] = ".rsp";
	const size_t extension_len = strlen(extension);

	const size_t filename_len = strlen(filename);
    int pos = 0;

    if ((folder_len+extension_len+filename_len+1) > path_buf_size) {
    	return (EOVERFLOW);
    } 

    strcpy(&path_buf[pos], folder);
    pos += folder_len;

	strcpy(&path_buf[pos], filename);
    pos += filename_len;

    strcpy(&path_buf[pos], extension);
    return 0;
}

size_t convert_hex(uint8_t *dest, size_t count, const char *src) {
    char buf[3];
    size_t i;
    for (i = 0; i < count && *src; i++) {
        buf[0] = *src++;
        buf[1] = '\0';
        if (*src) {
            buf[1] = *src++;
            buf[2] = '\0';
        }
        if (sscanf(buf, "%hhx", &dest[i]) != 1)
            break;
    }
    return i;
}

void parseif_name(char **description_ptr, const char *line_buf, size_t line_len)
{
	const char prefix[] = "Name = ";
	const size_t prefix_len = sizeof(prefix) - 1;

	if (prefix_len >= line_len)
		return;

	if (memcmp(line_buf, prefix, prefix_len) != 0)
		return;

	*description_ptr = strdup(&line_buf[prefix_len]);
}

void parseif_len_description(
	uint32_t *msg_len, char **description_ptr, 
	const char *filename, size_t filename_len, 
	const char *line_buf, size_t line_len)
{
	const char prefix[] = "Len = ";
	const size_t prefix_len = sizeof(prefix) - 1;

	if (prefix_len >= line_len)
		return;

	if (memcmp(line_buf, prefix, prefix_len) != 0)
		return;

	*msg_len = strtoumax(&line_buf[prefix_len], NULL, 10) / 8;

	size_t description_len = line_len - prefix_len + filename_len + 1;
	char* description = malloc(sizeof(char) * description_len);

	memcpy(description, filename, filename_len);
	description[filename_len] = '/';
	memcpy(&description[filename_len+1], &line_buf[prefix_len], line_len - 5);
	description[description_len] = '\0';

	*description_ptr = description;
}

int parseif_repetitions(
	uint32_t *repetitions, 
	const char *line_buf, size_t line_len)
{
	const char prefix[] = "Rep = ";
	const size_t prefix_len = sizeof(prefix) - 1;

	if (prefix_len >= line_len)
		return -1;

	if (memcmp(line_buf, prefix, prefix_len) != 0)
		return -1;

	*repetitions = strtoumax(&line_buf[prefix_len], NULL, 10);
	return 0;
}

int parseif_hex(
	uint8_t **msg, size_t msg_len,
	const char *line_buf, size_t line_len,
	const char *prefix, size_t prefix_len)
{
	if (prefix_len >= line_len)
		return -1;

	if (memcmp(line_buf, prefix, prefix_len) != 0)
		return -1;

	line_buf += prefix_len;
	line_len -= prefix_len;

	if (((msg_len*2) < line_len) 
		&& (msg_len != 0)) // NIST represents 0-sized vectors with a "00" message, not an actually void message
		return -1;
	
	*msg = malloc(msg_len);
	return convert_hex(*msg, msg_len, line_buf);
}

int parseif_fixed_hex(
	uint8_t buf[], size_t buf_len,
	const char *line_buf, size_t line_len,
	const char *prefix, size_t prefix_len)
{
	if (prefix_len >= line_len)
		return -1;

	if (memcmp(line_buf, prefix, prefix_len) != 0)
		return -1;

	line_buf += prefix_len;
	line_len -= prefix_len;

	size_t msg_len = buf_len;
	if ((msg_len*2) < line_len)
		return -1;
	
	msg_len = convert_hex(buf, msg_len, line_buf);

	return msg_len;
}

int parse_line(
	uint32_t *msg_len, char **description_ptr,
	uint8_t **msg, uint32_t *repetitions,
	uint8_t **key, size_t *key_len,
	uint8_t hash_buf[], size_t hash_buf_size,
	const char *filename, size_t filename_len, 
	const char *line_buf, size_t line_len)
{
	munit_assert(msg_len != NULL);
	munit_assert(description_ptr != NULL);
	munit_assert(msg != NULL);

	parseif_name(
		description_ptr,
		line_buf, line_len);
	parseif_len_description(
		msg_len, description_ptr,
		filename, filename_len,
		line_buf, line_len);

	const char prefix[] = "Msg = ";
	const size_t prefix_len = sizeof(prefix) - 1;
	int parsed_bytes = parseif_hex(
		msg, *msg_len,
		line_buf, line_len,
		prefix, prefix_len);
	if (parsed_bytes > 0) {
		munit_assert(parsed_bytes == *msg_len);
	}

	if ((key != NULL) && (key_len != NULL)) {
		const char prefix[] = "Key = ";
		const size_t prefix_len = sizeof(prefix) - 1;
		const uint32_t possible_key_len = (line_len-prefix_len)/2;

		parsed_bytes = parseif_hex(
			key, possible_key_len,
			line_buf, line_len,
			prefix, prefix_len);
		if (parsed_bytes > 0) {
			*key_len = possible_key_len;
			munit_assert(parsed_bytes == *key_len);
		}
	}

	if (repetitions != NULL) {
		parseif_repetitions(repetitions, line_buf, line_len);
	}

	const char md_prefix[] = "MD = ";
	parsed_bytes = parseif_fixed_hex(
		hash_buf, hash_buf_size,
		line_buf, line_len,
		md_prefix, sizeof(md_prefix)-1);

	if (parsed_bytes > 0) {
		munit_assert(parsed_bytes == hash_buf_size);

		return 1;
	}
	return 0;
}
