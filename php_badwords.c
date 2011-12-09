/** Copyright 2011 HoopCHINA, Co., Ltd.
 
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
 
 http://www.apache.org/licenses/LICENSE-2.0
 
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "ext/standard/php_string.h"
#include "ext/standard/php_var.h"
#include "ext/standard/php_smart_str.h"
#include "_mbsupport.h"
#include "php_badwords.h"

/* True global resources - no need for thread safety here */
static int le_badwords_compiler, le_badwords_trie;

/* {{{ badwords_functions[]
 *
 * Every user visible function must have an entry in badwords_functions[].
 */
zend_function_entry badwords_functions[] = {
	PHP_FE(badwords_compiler_create,	NULL)
	PHP_FE(badwords_compiler_append,	NULL)
	PHP_FE(badwords_compiler_compile,	NULL)
	PHP_FE(badwords_create,				NULL)
	PHP_FE(badwords_match,				NULL)
	PHP_FE(badwords_replace,			NULL)
	PHP_FE(badwords_version,			NULL)
	{NULL, NULL, NULL}	/* Must be the last line in badwords_functions[] */
};
/* }}} */

/* {{{ badwords_module_entry
 */
zend_module_entry badwords_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"badwords",
	badwords_functions,
	PHP_MINIT(badwords),
	PHP_MSHUTDOWN(badwords),
    NULL,
	NULL,
	PHP_MINFO(badwords),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_BADWORDS_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_BADWORDS
ZEND_GET_MODULE(badwords)
#endif

static void php_badwords_compiler_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    struct bw_trie_compiler_t *compiler = (struct bw_trie_compiler_t *) rsrc->ptr;
    bw_trie_compiler_free(compiler);
}

static void php_badwords_trie_dtor(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
	struct bw_trie_mmap_t *mmi = (struct bw_trie_mmap_t *) rsrc->ptr;
	if (mmi && --mmi->refcount <= 0) {
		munmap(mmi->trie, mmi->mlen);
		free(mmi);
	}
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(badwords)
{
    le_badwords_compiler = zend_register_list_destructors_ex(php_badwords_compiler_dtor, NULL, PHP_BADWORDS_COMPILER_RES_NAME, module_number);
    le_badwords_trie = zend_register_list_destructors_ex(php_badwords_trie_dtor, php_badwords_trie_dtor, PHP_BADWORDS_TRIE_RES_NAME, module_number);

	REGISTER_LONG_CONSTANT("BADWORDS_ENCODING_UTF8", BW_ENC_UTF8, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("BADWORDS_ENCODING_GBK", BW_ENC_GBK, CONST_CS | CONST_PERSISTENT);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(badwords)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(badwords)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "badwords support", "enabled");
	php_info_print_table_row(2, "extension version", PHP_BADWORDS_VERSION);
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto resource badwords_compiler_create([encoding = utf8, [case_insensitive = False]])
 */
PHP_FUNCTION(badwords_compiler_create)
{
	struct bw_trie_compiler_t *compiler;
	int       trie_encoding  = BW_ENC_UTF8;
    zend_bool case_sensitive = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|lb", &trie_encoding, &case_sensitive) == FAILURE) {
        return;
    }

    compiler = bw_trie_compiler_create(trie_encoding, case_sensitive);
	
    ZEND_REGISTER_RESOURCE(return_value, compiler, le_badwords_compiler);
}
/* }}} */

/* {{{ proto int badwords_compiler_append(resource compiler, string from, string to)
       proto int badwords_compiler_append(resource compiler, array replace)
 */
PHP_FUNCTION(badwords_compiler_append)
{
	struct bw_trie_compiler_t *compiler;
    zval *zcompiler;
	zval **from;
	char *to = NULL;
	int   to_len = 0;
	int   ac = ZEND_NUM_ARGS();
	long  added, total_added = 0;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rZ|s", &zcompiler, &from, &to, &to_len) == FAILURE) {
		return;
	}

	if (ac == 2 && Z_TYPE_PP(from) != IS_ARRAY) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "The second argument is not an array");
		RETURN_FALSE;
	}
    
    ZEND_FETCH_RESOURCE(compiler, struct bw_trie_compiler_t *, &zcompiler, -1, PHP_BADWORDS_COMPILER_RES_NAME, le_badwords_compiler);

	if (ac != 2) {
		convert_to_string_ex(from);
		added = bw_trie_compiler_add_word(compiler, Z_STRVAL_PP(from), Z_STRLEN_PP(from), to, to_len);
		if (added >= 0) {
			RETURN_LONG(added);
		} else {
			RETURN_FALSE;
		}
	}

	/* HASH */
	HashTable *hash = HASH_OF(*from);
	HashPosition hpos;
	zval **entry;
	int   key_len, elen;
	char *key, *eval;
	ulong num_key;
	int   keytype;
	zval  ktmp, etmp;

	zend_hash_internal_pointer_reset_ex(hash, &hpos);

	while (zend_hash_get_current_data_ex(hash, (void **)&entry, &hpos) == SUCCESS) {
		/* KEY */
		keytype = zend_hash_get_current_key_ex(hash, &key, &key_len, &num_key, 0, &hpos);
		if (keytype == HASH_KEY_IS_LONG) {
			ZVAL_LONG(&ktmp, num_key);
			convert_to_string(&ktmp);
			key = Z_STRVAL(ktmp);
			key_len = Z_STRLEN(ktmp);
		} else {
			key_len--;
		}

		/* VALUE */
		if (Z_TYPE_PP(entry) != IS_STRING) {
			etmp = **entry;
			zval_copy_ctor(&etmp);
			convert_to_string(&etmp);
			eval = Z_STRVAL(etmp);
			elen = Z_STRLEN(etmp);
		} else {
			eval = Z_STRVAL_PP(entry);
			elen = Z_STRLEN_PP(entry);
		}

		/* ADD... */
		added = bw_trie_compiler_add_word(compiler, key, key_len, eval, elen);

		if (Z_TYPE_PP(entry) != IS_STRING)
			zval_dtor(&etmp);
		if (keytype == HASH_KEY_IS_LONG)
			zval_dtor(&ktmp);

		/* CHECK... */
		if (added >= 0)
			total_added += added;
		// else
			// break;

		zend_hash_move_forward_ex(hash, &hpos);
	}
	
	RETURN_LONG(total_added);
}
/* }}} */

/* {{{ proto string badwords_compiler_compile(resource compiler)
 */
PHP_FUNCTION(badwords_compiler_compile)
{
	struct bw_trie_compiler_t *compiler;
    zval *zcompiler;
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zcompiler) == FAILURE) {
		return;
	}

    ZEND_FETCH_RESOURCE(compiler, struct bw_trie_compiler_t *, &zcompiler, -1, PHP_BADWORDS_COMPILER_RES_NAME, le_badwords_compiler);
	
	bw_trie_compiler_compile(compiler, return_value);
}
/* }}} */

/* {{{ proto resource badwords_create(string filename, [string persist_key])
 */
PHP_FUNCTION(badwords_create)
{
	char *filename, *persistkey = NULL;
	int  flen, klen = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|s", &filename, &flen, &persistkey, &klen) == FAILURE) {
		return;
	}
	
	int fd = open(filename, O_RDONLY);
	if (fd < 0) {
		if (persistkey) {
			zend_hash_del(&EG(persistent_list), persistkey, klen+1);
		}
		RETURN_FALSE;
	}
	
	struct stat stat;
	fstat(fd, &stat);

	if (persistkey) {
		struct bw_trie_mmap_t *existing_mmi;
		list_entry *existing_mmi_le;

		if (zend_hash_find(&EG(persistent_list), persistkey, klen+1, (void **)&existing_mmi_le) == SUCCESS) {
			existing_mmi = (struct bw_trie_mmap_t *) existing_mmi_le->ptr;
			if (existing_mmi->trie_tim == stat.st_mtime
				&& existing_mmi->trie_ino == stat.st_ino
				&& existing_mmi->trie_dev == stat.st_dev
				&& existing_mmi->mlen == stat.st_size) {
				existing_mmi->refcount++;
				close(fd);
				ZEND_REGISTER_RESOURCE(return_value, existing_mmi, le_badwords_trie);
				return;
			} else {
				zend_hash_del(&EG(persistent_list), persistkey, klen+1);
			}
		}
	}

	uint8_t *addr = mmap(NULL, stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	close(fd);
	
	/* MMAP_CHECK */
	if (addr == MAP_FAILED) RETURN_FALSE;
	
	struct bw_trie_mmap_t *mmi;
	mmi = (struct bw_trie_mmap_t *) malloc(sizeof(struct bw_trie_mmap_t));
	if (!mmi) {
		munmap(addr, stat.st_size);
		RETURN_FALSE;
	}

	mmi->refcount = 1;
	mmi->trie_dev = stat.st_dev;
	mmi->trie_ino = stat.st_ino;
	mmi->trie_tim = stat.st_mtime;
	mmi->trie = addr;
	mmi->mlen = stat.st_size;
	
	ZEND_REGISTER_RESOURCE(return_value, mmi, le_badwords_trie);

	if (persistkey) {
		list_entry le;
        le.type = le_badwords_trie;
        le.ptr = mmi;
        if (zend_hash_update(&EG(persistent_list), persistkey, klen+1, (void*)&le, sizeof(list_entry), NULL) == SUCCESS)
			mmi->refcount++;
	}
}
/* }}} */

/* {{{ proto string badwords_match(resource trie, string text)
       proto string badwords_match(string trie, string text)
 */
PHP_FUNCTION(badwords_match)
{
	zval **trie;
	char *text;
	int  text_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zs", &trie, &text, &text_len) == FAILURE) {
		return;
	}
	
	if (Z_TYPE_PP(trie) == IS_STRING) {
		bw_trie_match(*trie, return_value, text, text_len);
	}
	else if (Z_TYPE_PP(trie) == IS_RESOURCE) {
		struct bw_trie_mmap_t *mmi;
		zval trie_;
		ZEND_FETCH_RESOURCE(mmi, struct bw_trie_mmap_t *, trie, -1, PHP_BADWORDS_TRIE_RES_NAME, le_badwords_trie);
		ZVAL_STRINGL(&trie_, mmi->trie, mmi->mlen, 0);
		bw_trie_match(&trie_, return_value, text, text_len);
	}
}
/* }}} */

/* {{{ proto string badwords_replace(resource trie, string text)
       proto string badwords_replace(string trie, string text)
 */
PHP_FUNCTION(badwords_replace)
{
	zval **trie;
	char *text;
	int  text_len;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "Zs", &trie, &text, &text_len) == FAILURE) {
		return;
	}
	
	if (Z_TYPE_PP(trie) == IS_STRING) {
		bw_trie_replace(*trie, return_value, text, text_len);
	}
	else if (Z_TYPE_PP(trie) == IS_RESOURCE) {
		struct bw_trie_mmap_t *mmi;
		zval trie_;
		ZEND_FETCH_RESOURCE(mmi, struct bw_trie_mmap_t *, trie, -1, PHP_BADWORDS_TRIE_RES_NAME, le_badwords_trie);
		ZVAL_STRINGL(&trie_, mmi->trie, mmi->mlen, 0);
		bw_trie_replace(&trie_, return_value, text, text_len);
	}
}
/* }}} */

/* {{{ proto string badwords_version()
 */
PHP_FUNCTION(badwords_version)
{
	RETURN_STRING(PHP_BADWORDS_VERSION, 1);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
