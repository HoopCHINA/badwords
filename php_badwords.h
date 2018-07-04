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
#ifndef PHP_BADWORDS_H
#define PHP_BADWORDS_H

#include "php57_include.h"

/**
 * Compiler Example:
 *  $compiler = badwords_compiler_create(BADWORDS_ENCODING_UTF8, True);
 *  badwords_compiler_append($compiler, $word, $replace);
 *  badwords_compiler_append($compiler, array('a'=>'b', 'c'=>'d'));
 *  $badwords = badwords_compiler_compile($compiler);
 *  unset($compiler);
 *
 * --
 * Badwords Example:
 *  $badwords = badwords_create($filename, $persistKey);
 *  badwords_match($badwords, $text);
 *  badwords_replace($badwords, $text);
 *  unset($badwords);
 *  $badwords = badwords_create($filename);
 *  badwords_match($badwords, $text2);
 *  badwords_replace($badwords, $text2);
 *  unset($badwords);
 */

extern zend_module_entry badwords_module_entry;
#define phpext_badwords_ptr &badwords_module_entry


#ifdef PHP_WIN32
#     define PHP_BADWORDS_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#     define PHP_BADWORDS_API __attribute__ ((visibility("default")))
#else
#     define PHP_BADWORDS_API
#endif


#ifdef ZTS
#include "TSRM.h"
#endif

#include "badwords.h"
#include "compiler.h"

#define PHP_BADWORDS_TRIE_RES_NAME      "Badwords"
#define PHP_BADWORDS_COMPILER_RES_NAME  "Badwords_Compiler"

PHP_MINIT_FUNCTION(badwords);
PHP_MSHUTDOWN_FUNCTION(badwords);
PHP_MINFO_FUNCTION(badwords);
PHP_RINIT_FUNCTION(badwords);
PHP_RSHUTDOWN_FUNCTION(badwords);

#if PHP_MAJOR_VERSION >= 7
# define BADWORDS_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(badwords, v)
# if defined(ZTS) && defined(COMPILE_DL_BADWORDS)
  ZEND_TSRMLS_CACHE_EXTERN()
# endif
#endif

PHP_FUNCTION(badwords_compiler_create);
PHP_FUNCTION(badwords_compiler_append);
PHP_FUNCTION(badwords_compiler_compile);
PHP_FUNCTION(badwords_create);
PHP_FUNCTION(badwords_match);
PHP_FUNCTION(badwords_replace);
PHP_FUNCTION(badwords_version);

#define PHP_BADWORDS_VERSION "1.1.4"

struct bw_trie_mmap_t {
    int32_t  refcount;
    dev_t    trie_dev;
    ino_t    trie_ino;
    time_t   trie_tim;
    uint8_t *trie;
    size_t   mlen;
};

#endif  /* PHP_BADWORDS_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
