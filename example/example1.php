<?php
function do_convert(&$message)
{
    if (!extension_loaded('badwords')) {
        global $replace;
        isset($replace) || include(D_P.'data/cache/words.php');
        return strtr(&$message, &$replace);
    }

    /* -- USE BADWORDS EXTENSION -- */
    global $badwords;

    if (!isset($badwords)) {
    $wordfile = D_P.'data/cache/words.php';
    $triebin = '/dev/shm/words.bin';
    $persistkey = 'badwords::words';
    
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

$message = &do_convert(&$message);
