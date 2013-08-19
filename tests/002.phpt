--TEST--
Create a compiler resource
--SKIPIF--
<?php if (!extension_loaded("badwords")) print "skip"; ?>
--FILE--
<?php 
$r = badwords_compiler_create(BADWORDS_ENCODING_UTF8, True);;
var_dump($r);
?>
--EXPECTF--
resource(%d) of type (Badwords_Compiler)
