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
#ifndef __BADWORDS_H__
#define __BADWORDS_H__

#define BW_TRIE_MAGIC   0xBAD1
#define BW_TRIE_VERSION 0x0100

#define BW_NODE_MAX  65535
#define BW_WORD_MAX  255

struct bw_string_t {
	uint8_t len;
	uint8_t byte[BW_WORD_MAX];
};

static inline void
bw_string_cpy(struct bw_string_t *dest, uint8_t *src, int c)
{
	dest->len = c;
	memcpy(dest->byte, src, c);
}

static inline int
bw_string_siz(struct bw_string_t *bstr)
{
	return bstr->len + 1;
}

struct bw_trie_header_t {
	uint16_t magic_num;
	uint16_t version;
    uint8_t  trie_encoding;
    uint8_t  case_insensitive;
	uint16_t node_count;
};

struct bw_node_t {
	unsigned is_terminal : 1;
	unsigned is_fragment : 1;
	unsigned reserved : 6;
	unsigned replace : 24;
	union {
		uint16_t next[256];
		struct bw_string_t fragment;
	};
};

void bw_trie_match(zval *trie, zval *return_value, uint8_t *text, int c);
void bw_trie_replace(zval *trie, zval *return_value, uint8_t *text, int c);

#endif /* __BADWORDS_H_ */
