<?php

!function_exists('readover') && exit('Forbidden');

// 后续增加的一些代码转换,所有楼层帖子、论坛签名档、日志页面都用到这个转换
function convertPlus(&$message, &$postdate = 0)
{
    return ifconvert(&$message);
}

// 是否进行敏感关键字替换
function ifconvert(&$message, $ifreplace=true)
{
    if (!extension_loaded('badwords')) {
        global $replace;
        isset($replace) || include(D_P.'data/bbscache/wordsfb.php');
        return strtr(&$message, &$replace);
    }

    /* -- USE BADWORDS EXTENSION -- */
    global $badwords;

    if (!isset($badwords)) {
	$wordfile = D_P.'data/bbscache/wordsfb.php';
	$triebin = '/dev/shm/com.hoopchina.bbs-wordsfb.bin';
	$persistkey = 'badwords::com.hoopchina.bbs::wordsfb';
		
	$wmtime = filemtime($wordfile);
	$tmtime = filemtime($triebin);

	if ($tmtime === FALSE || $tmtime !== $wmtime && mt_rand(0, 99) < 5) {
		include($wordfile);
		$compiler = badwords_compiler_create(BADWORDS_ENCODING_GBK, True);
		badwords_compiler_append($compiler, &$replace);
		unset($replace);

		$trie = &badwords_compiler_compile($compiler);
		unset($compiler);

		if ($trie) {
			$triebin_tmp = $triebin.'-'.getmypid();
			file_put_contents($triebin_tmp, &$trie);
			touch($triebin_tmp, $wmtime);
			rename($triebin_tmp, $triebin);
			unset($trie);
		}
	}

	$badwords = badwords_create($triebin, $persistkey);
    }

    return badwords_replace($badwords, &$message);
}
