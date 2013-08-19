--TEST--
Do case-sensitive and case-insensitive match
--SKIPIF--
<?php if (!extension_loaded("badwords")) print "skip"; ?>
--FILE--
<?php 
$r = badwords_compiler_create(BADWORDS_ENCODING_UTF8, False);
$a = badwords_compiler_append($r, 'word', 'replace');
$b = badwords_compiler_append($r, array('www'=>'xxx', 'ttt'=>'vvv'));
$t = badwords_compiler_compile($r);
$c = badwords_match($t, 'My WoRd with www to ttt.');

$r2 = badwords_compiler_create(BADWORDS_ENCODING_UTF8, True);
$a2 = badwords_compiler_append($r2, 'word', 'replace');
$b2 = badwords_compiler_append($r2, array('www'=>'xxx', 'ttt'=>'vvv'));
$t2 = badwords_compiler_compile($r2);
$c2 = badwords_match($t2, 'My WoRd with www to ttt.');

print "$c,$c2";
?>
--EXPECT--
www,WoRd
