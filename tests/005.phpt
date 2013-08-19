--TEST--
Do case-insensitive replacement
--SKIPIF--
<?php if (!extension_loaded("badwords")) print "skip"; ?>
--FILE--
<?php 
$r = badwords_compiler_create(BADWORDS_ENCODING_UTF8, True);
$a = badwords_compiler_append($r, 'word', 'replace');
$b = badwords_compiler_append($r, array('www'=>'xxx', 'ttt'=>'vvv'));
$t = badwords_compiler_compile($r);
$c = badwords_replace($t, 'My Word with wWw to TTT.');
print $c;
?>
--EXPECT--
My replace with xxx to vvv.
