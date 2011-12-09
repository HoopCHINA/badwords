Badwords PHP extension
======================

Badwords is a PHP extension for fast replacement of multiple words in a piece of text. It supports case-sensitive and case-insensitive matching. It requires that the text it operates on be encoded as UTF-8 and/or GBK.

Using Badwords
--------------

>>> Getting Started

There are four steps to one-time doing a multiple-word-replacement with Badwords:

1. Create a new Badwords compiler resource;
2. Add the words to the resource that you want to replace;
3. Compile and generate a binary form replace database;
4. Process the text in which you want to do the replacements thru the database.

For example:

    <?php
    $compiler = badwords_compiler_create(BADWORDS_ENCODING_UTF8, True);
    badwords_compiler_append($compiler, $word, $replace);
    badwords_compiler_append($compiler, array('a'=>'b', 'c'=>'d'));
    
    $trie = badwords_compiler_compile($compiler);
    badwords_match($trie, $text);
    badwords_replace($trie, $text);

or you can persist the database and use later:

1. Persist the binary form database to file;
2. Create a new Badwords resource and mmap in the database;
3. Process the text in which you want to do the replacements thru the resource.

For example:

    <?php
    // Persist
    file_put_contents('/tmp/badwords-trie', &$trie);
    
    // mmap in
    $trie = badwords_create('/tmp/badwords-trie', 'trie1');
    badwords_match($trie, $text);
    badwords_replace($trie, $text);

>>> Case Sensitivity

The second argument to `badwords_compiler_create()` controls the case-sensitivity of the matching for replacement. Pass `true` to make matches case-insensitive, `false` to make them case-sensitive. The default (if no argument is provided) is to match case-sensitive.

>>> Multibyte Characters

Badwords understands `US-ASCII`, `UTF-8` and `GBK`.

Reference
---------

>>> Encoding Consts

* `const BADWORDS_ENCODING_UTF8`
* `const BADWORDS_ENCODING_GBK`

>>> Badwords Compiler Functions

* `resource badwords_compiler_create([encoding = BADWORDS_ENCODING_UTF8, [case_insensitive = False]]);`
* `int badwords_compiler_append(resource compiler, string from, string to);`
* `int badwords_compiler_append(resource compiler, array replace);`
* `string badwords_compiler_compile(resource compiler);`

>>> Badwords Functions

* `resource badwords_create(string filename, [string php_persist_object_key]);`
* `string badwords_match(resource trie, string text);`
* `string badwords_match(string trie, string text);`
* `string badwords_replace(resource trie, string text);`
* `string badwords_replace(string trie, string text);`
* `string badwords_version();`

Internals
---------

The Badwords resource contains a trie which holds the list of words to replace. For example, if you add "one", "onto", and "bob" to the trie, then it looks like this:

    root -> o -> n -> e
         |         \-> to
         \-> bob

If matching is to be done case-insensitively, the words are folded to lowercase before they're added to the trie. 

`badwords_replace()`, walks through the text to replace byte by byte. If it sees a byte that is one of the bytes the root of the trie points too, then it attempts to traverse the trie, looking for children that correspond to subsequent bytes in the text to replace. 

This traversal ends in one of two possibilities. If the code gets to a point in the trie where it reaches a leaf node (one with no children), then it's found a correspondance between the text to replace and a word in the trie. So it replaces all but the first character of the word with the replacement byte.

If the code instead gets to a point where it reaches a non-leaf node that does not have a child corresponding to the next byte in the text, then what has appeared to be a match isn't. (E.g. "one" is on the list but the text to replace contains "onyx" -- the o and the n match but then the "n" node only has an "e" child, not a "y" child. At this point, the code goes back to the character after the initial match (e.g. the "n") and starts scanning again.

If matching is to be case insensitive, then the text walking happens against a copy of the text that has been folded to lowercase rather than the original text.

Multibyte characters are a little more complicated. Even though the comparisons can still happen byte-by-byte, replacements need to take into account character length. The structure of UTF-8/GBK makes it fast to determine the length of character sequences and how to move forward and back in the strings.

`badwords_match()` is the simplied version of `badwords_replace()` which only returns the _first_ matched word. If can not match
any word, it returns _empty_ string.

Future Directions
-----------------

* Need more friendly reference...
* Need add unit tests...

License
-------

>>> Badwords is Copyright 2011 HoopCHINA, Co., Ltd.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
       http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

>>> Boxwood is Copyright 2010, Ning, Inc.

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at
    
       http://www.apache.org/licenses/LICENSE-2.0
    
    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.

Credit
------

* Thanks to David Sklar, it makes the excellent [Boxwood](https://github.com/ning/boxwood) extension which Badwords based on.
* Thanks to [HoopCHINA.com](http://www.hoopchina.com) for sponsor resources to make this extension happen.