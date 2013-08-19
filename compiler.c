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
#include "_mbsupport.h"
#include "compiler.h"

/**
 * Create a new badwords trie compiler.
 */
struct bw_trie_compiler_t *
bw_trie_compiler_create(int encoding, int case_insensitive)
{
    struct bw_trie_compiler_t *compiler;
    struct bw_node_t *root;

    compiler = (struct bw_trie_compiler_t *) malloc(sizeof(struct bw_trie_compiler_t));
    if (!compiler) return NULL;

    compiler->trie_encoding = encoding;
    compiler->case_insensitive = case_insensitive;
    compiler->node_count = 1;
    compiler->replace_len = 0;

    root = compiler->nodes;
    memset(root, 0, sizeof(struct bw_node_t));
    
    return compiler;
}

static uint16_t
bw_node_create(struct bw_trie_compiler_t *compiler, uint8_t *fragment, int c, unsigned replace)
{
    if (compiler->node_count >= BW_NODE_MAX) return 0;

    struct bw_node_t *node = &compiler->nodes[compiler->node_count];
    node->is_terminal = 1;
    node->replace = replace;

    if (c == 0) {
        node->is_fragment = 0;
        memset(node->next, 0, sizeof(node->next));
    }
    else {
        node->is_fragment = 1;
        bw_string_cpy(&node->fragment, fragment, c);
    }

    return compiler->node_count++;
}

static uint16_t
bw_node_fork(struct bw_trie_compiler_t *compiler, struct bw_node_t *node)
{
    assert (node->is_terminal != 0);
    assert (node->is_fragment != 0);

    uint16_t forknode =
        bw_node_create(compiler, node->fragment.byte+1, node->fragment.len-1, node->replace);

    if (forknode) {
        uint8_t forkbyte = node->fragment.byte[0];
        memset(node, 0, sizeof(struct bw_node_t));
        node->next[forkbyte] = forknode;
    }
    
    return forknode;
}

static inline struct bw_node_t *
bw_node_ptr(struct bw_trie_compiler_t *compiler, uint16_t index)
{
    return compiler->nodes + index;
}

static uint32_t
bw_replace_create(struct bw_trie_compiler_t *compiler, uint8_t *replace, int rc)
{
    assert (rc <= BW_WORD_MAX);
    
    struct bw_string_t *bstr = (struct bw_string_t *)(compiler->replaces + compiler->replace_len);
    bw_string_cpy(bstr, replace, rc);

    uint32_t loc = compiler->replace_len;
    compiler->replace_len += bw_string_siz(bstr);
    return loc;
}

static int
bw_trie_compiler_add_bytes(struct bw_trie_compiler_t *compiler,
                            uint8_t *bytes, int c,
                            uint8_t *replace, int rc)
{
    assert (c <= BW_WORD_MAX);
    assert (rc <= BW_WORD_MAX);

    struct bw_node_t *root = compiler->nodes;
    struct bw_node_t *node = root;
    uint8_t *end = bytes + c;

    /* Ignore empty string */
    if (bytes == end) return 0;

    /* Bytes adding */
    while (bytes < end) {
        if (node->is_fragment) {
            if ((end-bytes) == node->fragment.len
                && memcmp(bytes, node->fragment.byte, node->fragment.len) == 0)
                return 0;
            if (!bw_node_fork(compiler, node))
                return -1;
        }
        if (node->next[*bytes] == 0) {
            uint16_t newnode = bw_node_create(compiler, bytes+1, end-bytes-1, 0);
            if (newnode) {
                root[newnode].replace = bw_replace_create(compiler, replace, rc);
                node->next[*bytes] = newnode;
                return c;
            } else {
                return -1;
            }
        }
        /* NEXT... */
        node = root + node->next[*bytes++];
    }
    
    if (node->is_fragment && !bw_node_fork(compiler, node))
        return -1;
    if (node->is_terminal)
        return 0;

    node->is_terminal = 1;
    node->replace = bw_replace_create(compiler, replace, rc);

    return c;
}

int
bw_trie_compiler_add_word(struct bw_trie_compiler_t *compiler,
                            uint8_t *word, int c,
                            uint8_t *replace, int rc)
{
    if (c > BW_WORD_MAX || rc > BW_WORD_MAX)
        return -1;

    if (!compiler->case_insensitive) {
        return bw_trie_compiler_add_bytes(compiler, word, c, replace, rc);
    }
    else {
        uint8_t buf[BW_WORD_MAX];
        memcpy(buf, word, c);
        bw_mb_strtolower(buf, buf+c, compiler->trie_encoding);
        return bw_trie_compiler_add_bytes(compiler, buf, c, replace, rc);
    }
}

void
bw_trie_compiler_compile(struct bw_trie_compiler_t *compiler, zval *return_value)
{
    uint32_t hlen = sizeof(struct bw_trie_header_t);
    uint32_t nlen = compiler->node_count * sizeof(struct bw_node_t);
    uint32_t rlen = compiler->replace_len;
    uint32_t tlen = hlen + nlen + rlen;

    uint8_t *trie = emalloc(tlen);

    if (trie) {
        struct bw_trie_header_t *header = (struct bw_trie_header_t *) trie;
        header->magic_num = BW_TRIE_MAGIC;
        header->version = BW_TRIE_VERSION;
        header->trie_encoding = compiler->trie_encoding;
        header->case_insensitive = compiler->case_insensitive;
        header->node_count = compiler->node_count;
        
        memcpy(trie+hlen, compiler->nodes, nlen);
        memcpy(trie+hlen+nlen, compiler->replaces, rlen);
        
        RETURN_STRINGL(trie, tlen, 0);
    }

    RETURN_FALSE;
}

void bw_trie_compiler_free(struct bw_trie_compiler_t *compiler)
{
    if (compiler)
        free(compiler);
}
