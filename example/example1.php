<?php
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

define ('D_P', './');

// Get shared badwords resource
function get_shared_badwords__() {
    static $badwords;

    if (!empty($badwords)) {
    	return $badwords;
    }

    $wordfile = D_P.'words.php';
    $triebin = D_P.'com.foo.bar-words.bin';
    $persistkey = 'badwords::com.foo.bar1::words';

    $wmtime = filemtime($wordfile);
    $tmtime = filemtime($triebin);

    if ($tmtime === FALSE || $tmtime !== $wmtime && mt_rand(0, 99) < 5) {
    	include($wordfile);
    	$compiler = badwords_compiler_create(BADWORDS_ENCODING_UTF8, True);
    	badwords_compiler_append($compiler, $replace);
    	unset($replace);

    	$trie = badwords_compiler_compile($compiler);
    	unset($compiler);

    	if ($trie) {
            $triebin_tmp = $triebin.'-'.getmypid();
    	    file_put_contents($triebin_tmp, $trie);
    	    touch($triebin_tmp, $wmtime);
    	    rename($triebin_tmp, $triebin);
            chmod($triebin, 0755);
    	    unset($trie);
    	}
    }

    $badwords = badwords_create($triebin, $persistkey);

    return $badwords;
}

// Replace words
function do_replace($message)
{
    if (!function_exists('badwords_replace')) {
        global $replace;
        isset($replace) || include(D_P.'data/cache/words.php');
        return strtr($message, $replace);
    }

    /* -- USE BADWORDS EXTENSION -- */
    $badwords = get_shared_badwords__();
    return badwords_replace($badwords, $message);
}

// Match words
function do_match($message)
{
    if (!function_exists('badwords_match')) {
        global $replace;
        isset($replace) || include(D_P.'data/cache/words.php');
        foreach ($replace as $from=>$to) {
            if (stripos($message, (string)$from) !== FALSE) {
                return $from;
            }
        }
        return '';
    }

    /* -- USE BADWORDS EXTENSION -- */
    $badwords = get_shared_badwords__();
    return badwords_match($badwords, $message);
}

$message = "近日特朗普将携希拉里一同访问中国，北京欢迎你！520~";
$rlword = do_replace($message);
$mtword = do_match($message);

var_dump($rlword, $mtword);
