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
#ifndef _PHP5_INCLUDE_H_
#define _PHP5_INCLUDE_H_

#define COM57_Z_STRVAL_PP   Z_STRVAL_PP
#define COM57_Z_STRLEN_PP   Z_STRLEN_PP
#define COM57_Z_LVAL_PP     Z_LVAL_PP
#define COM57_Z_TYPE_PP     Z_TYPE_PP
#define COM57_Z_ISREF_PP    Z_ISREF_PP

// 字符串宏定义
#define COM57_RETURN_STRINGL    RETURN_STRINGL
#define COM57_RETURN_STRING     RETURN_STRING
#define COM57_ZVAL_STRINGL      ZVAL_STRINGL
#define COM57_ZVAL_STRING       ZVAL_STRING
#define COM57_RETVAL_STRINGL    RETVAL_STRINGL
#define COM57_RETVAL_STRING     RETVAL_STRING

// smart_str 字符串相关操作(仅定义项目使用)
#define COM57_SMART_STRING_T        smart_str
#define COM57_SMART_STRING_APPENDL  smart_str_appendl
#define COM57_SMART_STRING_0        smart_str_0
#define COM57_SMART_STRING_FREE     smart_str_free

#define COM57_ZEND_RESURCE_T  zend_rsrc_list_entry

// resource register/fetch
#define COM57_ZEND_REGISTER_RESOURCE  ZEND_REGISTER_RESOURCE

#define COM57_ZEND_FETCH_RESOURCE     ZEND_FETCH_RESOURCE

// php7结构 zend_string
#define COM57_STRING_INIT
#define COM57_STRING_RELEASE

// hash
#define COM57_ZEND_HASH_DEL(ht, key, len) \
   zend_hash_del(ht, key, len)

#define COM57_ZEND_HASH_UPDATE(ht, key, len, zv, zlen, dest_ptr) \
   zend_hash_update(ht, key, len, (void *)zv, zlen, dest_ptr)

#endif	//	_PHP5_INCLUDE_H_
