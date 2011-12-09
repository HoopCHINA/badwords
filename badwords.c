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
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ext/standard/php_string.h"
#include "ext/standard/php_var.h"
#include "ext/standard/php_smart_str.h"
#include "_mbsupport.h"
#include "badwords.h"

static void
bw_match_text(struct bw_trie_header_t *header, zval *return_value,
				uint8_t *text, uint8_t *text_to_walk, int c)
{
    struct bw_node_t *root = (struct bw_node_t *)(header+1), *node, *gotnode;
	uint8_t *end = text_to_walk + c, *watch, *gotwatch;
	uint8_t *_Rep_base = (uint8_t *)(root + header->node_count);

	smart_str result = {0};

    while (text_to_walk < end) {
		/* MATCH */
		if (root->next[*text_to_walk] != 0) {
			watch = text_to_walk;
			node = root;
			gotnode = NULL;
			
			do {
				node = root + node->next[*watch];
				if (node->is_fragment) {
					if ((end-watch) > node->fragment.len
						&& memcmp(watch+1, node->fragment.byte, node->fragment.len) == 0) {
						gotnode = node;
						gotwatch = watch + node->fragment.len;
					}
					break;
				}
				if (node->is_terminal) {
					gotnode = node;
					gotwatch = watch;
				}
				watch++;
			}
			while (watch < end && node->next[*watch] != 0);

			if (gotnode) {
				smart_str_appendl(&result, text, (gotwatch - text_to_walk + 1));
				break;
            }
		}
		
		watch = bw_mb_skip_char(text_to_walk, end, header->trie_encoding);
		text += watch - text_to_walk;
		text_to_walk = watch;
    }

	smart_str_0(&result);
	
	RETURN_STRINGL(result.c, result.len, 0)
}

void
bw_trie_match(zval *trie, zval *return_value, uint8_t *text, int c)
{
	struct bw_trie_header_t *header = (struct bw_trie_header_t *) Z_STRVAL_P(trie);
	
	if (header->magic_num != BW_TRIE_MAGIC
		|| header->version != BW_TRIE_VERSION) RETURN_FALSE;
	
	if (!header->case_insensitive) {
		bw_match_text(header, return_value, text, text, c);
	}
	else {
		uint8_t *text_to_walk = estrndup(text, c);
		if (!text_to_walk) {
			RETURN_FALSE;
		} else {
			bw_mb_strtolower(text_to_walk, text_to_walk+c, header->trie_encoding);
			bw_match_text(header, return_value, text, text_to_walk, c);
			efree(text_to_walk);
		}
	}
}

static void
bw_replace_text(struct bw_trie_header_t *header, zval *return_value,
				uint8_t *text, uint8_t *text_to_walk, int c)
{
    struct bw_node_t *root = (struct bw_node_t *)(header+1), *node, *gotnode;
	uint8_t *end = text_to_walk + c, *watch, *gotwatch;
	uint8_t *_Rep_base = (uint8_t *)(root + header->node_count);

	smart_str result = {0};

    while (text_to_walk < end) {
		/* REPLACE */
		if (root->next[*text_to_walk] != 0) {
			watch = text_to_walk;
			node = root;
			gotnode = NULL;
			
			do {
				node = root + node->next[*watch];
				if (node->is_fragment) {
					if ((end-watch) > node->fragment.len
						&& memcmp(watch+1, node->fragment.byte, node->fragment.len) == 0) {
						gotnode = node;
						gotwatch = watch + node->fragment.len;
					}
					break;
				}
				if (node->is_terminal) {
					gotnode = node;
					gotwatch = watch;
				}
				watch++;
			}
			while (watch < end && node->next[*watch] != 0);

			if (gotnode) {
				struct bw_string_t *replace = (struct bw_string_t *)(_Rep_base + gotnode->replace);
				smart_str_appendl(&result, replace->byte, replace->len);
				text += gotwatch - text_to_walk + 1;
				text_to_walk = gotwatch + 1;
				continue;
            }
		}
		
		watch = bw_mb_skip_char(text_to_walk, end, header->trie_encoding);
		smart_str_appendl(&result, text, watch-text_to_walk);
		text += watch - text_to_walk;
		text_to_walk = watch;
    }

	smart_str_0(&result);
	
	RETURN_STRINGL(result.c, result.len, 0)
}

void
bw_trie_replace(zval *trie, zval *return_value, uint8_t *text, int c)
{
	struct bw_trie_header_t *header = (struct bw_trie_header_t *) Z_STRVAL_P(trie);
	
	if (header->magic_num != BW_TRIE_MAGIC
		|| header->version != BW_TRIE_VERSION) RETURN_FALSE;
	
	if (!header->case_insensitive) {
		bw_replace_text(header, return_value, text, text, c);
	}
	else {
		uint8_t *text_to_walk = estrndup(text, c);
		if (!text_to_walk) {
			RETURN_FALSE;
		} else {
			bw_mb_strtolower(text_to_walk, text_to_walk+c, header->trie_encoding);
			bw_replace_text(header, return_value, text, text_to_walk, c);
			efree(text_to_walk);
		}
	}
}
