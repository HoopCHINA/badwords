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
#ifndef _PHP7_INCLUDE_H_
#define _PHP7_INCLUDE_H_

#include <stdint.h>
#ifndef INT64_MAX
#define INT64_MAX	INT64_C( 9223372036854775807)
#endif
#ifndef INT64_MIN
#define INT64_MIN	(-INT64_C( 9223372036854775807)-1)
#endif

// 可以使用 zval_get_long(zval), zval_get_double(zval), zval_get_string(zval)等
// 函数获取 zval 的值，这样不会改变原始的 zval

#define COM57_Z_STRVAL_PP(zv_pp)	Z_STRVAL_P(*zv_pp)
#define COM57_Z_STRLEN_PP(zv_pp)	Z_STRLEN_P(*zv_pp)
#define COM57_Z_LVAL_PP(zv_pp)		Z_LVAL_P(*zv_pp)
#define COM57_Z_TYPE_PP(zv_pp)		Z_TYPE_P(*zv_pp)
#define COM57_Z_ISREF_PP(zv_pp)		Z_ISREF_P(*zv_pp)

// 字符串宏定义
#define COM57_RETURN_STRINGL(s, l, dup)			RETURN_STRINGL(s, l)
#define COM57_RETURN_STRING(s, dup)				RETURN_STRING(s)
#define COM57_ZVAL_STRINGL(z, s, l, dup)		ZVAL_STRINGL(z, s, l)
#define COM57_ZVAL_STRING(z, s, dup)			ZVAL_STRING(z, s)
#define COM57_RETVAL_STRINGL(s, l, dup)			RETVAL_STRINGL(s, l)
#define COM57_RETVAL_STRING(s, dup)				RETVAL_STRING(s)

// smart_string 字符串相关操作(仅定义项目使用)
#define COM57_SMART_STRING_T						smart_string
#define COM57_SMART_STRING_APPENDL(dest, src, len)	smart_string_appendl(dest, src, len)
#define COM57_SMART_STRING_0(s)						smart_string_0(s)
#define COM57_SMART_STRING_FREE(s)					smart_string_free(s)

// php7 zend_rsrc_list_entry should be replaced by zend_resource
#define COM57_ZEND_RESURCE_T	zend_resource

// resource register/fetch
#define COM57_ZEND_REGISTER_RESOURCE(return_value, rsrc_ptr, rsrc_type) \
	RETURN_RES(zend_register_resource(rsrc_ptr, rsrc_type))

#define COM57_ZEND_FETCH_RESOURCE(return_value, return_type, zv, i, rsrc_ptr, rsrc_type) \
	return_value = (return_type)zend_fetch_resource(Z_RES_P(*zv), rsrc_ptr, rsrc_type);

// zval* 获取 zend_string*
// zend_string* = Z_STR_P(zval*)

// zend_string* 获取 char*
// char* = ZSTR_VAL(zend_string*)

// zend_string* 获取 len
// char* = ZSTR_LEN(zend_string*)

// char* to zend_string* | release zend_string*
#define COM57_STRING_INIT(c_string) 	zend_string_init(c_string, strlen(c_string), 0)
#define COM57_STRING_RELEASE(z_string) 	zend_string_release(z_string)

// hash
#define COM57_ZEND_HASH_DEL(ht, key, len) \
({ \
	int ret = FAILURE; \
	zend_string *zstr = zend_string_init(key, strlen(key), 0); \
	if (!(zend_hash_del(ht, zstr))) { \
		ret = SUCCESS; \
	} \
	zend_string_release(zstr); \
	ret; \
})

#define COM57_ZEND_HASH_UPDATE(ht, key, len, zv, zlen, dest_ptr) \
({ \
	int ret = FAILURE; \
	zend_string *zstr = zend_string_init(key, strlen(key), 0); \
	if (!(zend_hash_update(ht, zstr, zv))) { \
		ret = SUCCESS; \
	} \
	zend_string_release(zstr); \
	ret; \
})

#endif	// _PHP7_INCLUDE_H_
