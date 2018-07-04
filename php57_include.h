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
#ifndef _PHP57_INCLUDE_H_
#define _PHP57_INCLUDE_H_

// see doc. https://wiki.php.net/phpng-upgrading

#include <php.h>
#include <php_ini.h>
#include <php_version.h>

#include <ext/standard/php_string.h>
#include <ext/standard/php_var.h>
#include <ext/standard/info.h>

#if PHP_MAJOR_VERSION < 7
#include <ext/standard/php_smart_str.h>
#include "php5_include.h"
#else
#include <ext/standard/php_smart_string.h>
#include "php7_include.h"
#endif  

#endif	//	_PHP57_INCLUDE_H_  