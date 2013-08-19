--TEST--
Add words to compiler
--SKIPIF--
<?php if (!extension_loaded("badwords")) print "skip"; ?>
--FILE--
<?php 
$r = badwords_compiler_create(BADWORDS_ENCODING_UTF8, True);
$a = badwords_compiler_append($r, 'word', 'replace');
$b = badwords_compiler_append($r, '', '');
$c = badwords_compiler_append($r, array('www'=>'xxx', 'ttt'=>'vvv'));
$d = badwords_compiler_append($r, array('zzz'=>'xxx'), 'foo');
print "$a,$b,$c,$d";
?>
--EXPECT--
4,0,6,3
