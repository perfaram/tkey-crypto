#include "munit/munit.h"
#include "t_sha1.h"
#include "t_hmac_sha1.h"

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
	sha1_main(argc, argv);
	hmac_sha1_main(argc, argv);
}