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
#ifndef ___MBSUPPORT_H__
#define ___MBSUPPORT_H__

#define BW_ENC_UTF8  0
#define BW_ENC_GBK   1

static inline uint8_t *
bw_mb_skip_char_utf8(uint8_t *str, uint8_t *end)
{
    while (++str < end)
        if ((*str & 0xC0) != 0x80)
            return str;
    return end;
}

static inline void
bw_mb_strtolower_utf8(uint8_t *str, uint8_t *end)
{
    while (str < end) {
        uint8_t ch = *str;
	    if (ch < 0x80)
            *str = tolower(ch);
        str++;
    }
}

static inline uint8_t *
bw_mb_skip_char_gbk(uint8_t *str, uint8_t *end)
{
    if (str < end) {
        uint8_t ch = *str++;
        if (ch < 0x81 || ch > 0xFE)
            return str;
    }
    if (str < end) {
        uint8_t ch = *str++;
        if (ch < 0x40 || ch > 0xFE || ch == 0x7F)
            return (str-1);
    }
    return str;
}

static inline void
bw_mb_strtolower_gbk(uint8_t *str, uint8_t *end)
{
    while (str < end) {
        uint8_t ch = *str;
        if (0x81 <= ch && ch < 0xFF)
            str += 2;
        else
            *str++ = tolower(ch);
    }
}

static inline uint8_t *
bw_mb_skip_char(uint8_t *str, uint8_t *end, int encoding)
{
    if (encoding == BW_ENC_GBK)
        return bw_mb_skip_char_gbk(str, end);
    else
        return bw_mb_skip_char_utf8(str, end);
}

static inline void
bw_mb_strtolower(uint8_t *str, uint8_t *end, int encoding)
{
    if (encoding == BW_ENC_GBK)
        return bw_mb_strtolower_gbk(str, end);
    else
        return bw_mb_strtolower_utf8(str, end);
}

#endif /* ___MBSUPPORT_H__ */

