--TEST--
Do replacement
--SKIPIF--
<?php if (!extension_loaded("badwords")) print "skip"; ?>
--FILE--
<?php 
$r = badwords_compiler_create(BADWORDS_ENCODING_UTF8, False);
$a = badwords_compiler_append($r, 'word', 'replace');
$b = badwords_compiler_append($r, array('www'=>'xxx', 'ttt'=>'vvv'));
$t = badwords_compiler_compile($r);
$c = badwords_replace($t, 'My word with www to ttt.');
print $c;
?>
--EXPECT--
My replace with xxx to vvv.
