--TEST--
Replace on empty and null string
--SKIPIF--
<?php if (!extension_loaded("badwords")) print "skip"; ?>
--FILE--
<?php 
$r = badwords_compiler_create(BADWORDS_ENCODING_UTF8, True);
$a = badwords_compiler_append($r, 'word', 'replace');
$b = badwords_compiler_append($r, array('www'=>'xxx', 'ttt'=>'vvv'));
$t = badwords_compiler_compile($r);
$c = badwords_replace($t, '');
print 'Replace with empty'.$c;
$d = badwords_replace($t, NULL);
print ' and NULL.'.$d;
?>
--EXPECT--
Replace with empty and NULL.
