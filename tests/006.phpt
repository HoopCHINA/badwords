--TEST--
Do replacement with persisted trie
--SKIPIF--
<?php if (!extension_loaded("badwords")) print "skip"; ?>
--FILE--
<?php 
$r = badwords_compiler_create(BADWORDS_ENCODING_UTF8, True);
$f = '/tmp/badwords-trie';

$a = badwords_compiler_append($r, 'word', 'replace');
$b = badwords_compiler_append($r, array('www'=>'xxx', 'ttt'=>'vvv'));
$t = badwords_compiler_compile($r);
file_put_contents($f, $t);

$t2 = badwords_create($f, 'trie1');
$c = badwords_replace($t2, 'My Word with wWw to TTT.');
print $c;

@unlink($f);
?>
--EXPECT--
My replace with xxx to vvv.
