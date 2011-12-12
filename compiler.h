/** Copyright 2011 HoopCHINA, Co., Ltd.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#ifndef __COMPILER_H__
#define __COMPILER_H__

#include "badwords.h"

#define BW_REPLACES_MAX  (BW_NODE_MAX * sizeof(struct bw_string_t))

struct bw_trie_compiler_t {
    uint8_t  trie_encoding;
    uint8_t  case_insensitive;
    uint16_t node_count;
    uint32_t replace_len;
    struct bw_node_t nodes[BW_NODE_MAX];
    uint8_t replaces[BW_REPLACES_MAX];
};

struct bw_trie_compiler_t *bw_trie_compiler_create(int encoding, int case_insensitive);
int bw_trie_compiler_add_word(struct bw_trie_compiler_t *compiler, uint8_t *word, int c, uint8_t *replace, int rc);
void bw_trie_compiler_compile(struct bw_trie_compiler_t *compiler, zval *return_value);
void bw_trie_compiler_free(struct bw_trie_compiler_t *compiler);

#endif /* __COMPILER_H__ */
