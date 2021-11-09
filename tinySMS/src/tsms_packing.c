/*
* Copyright (C) 2009 Mamadou Diop.
*
* Contact: Mamadou Diop <diopmamadou [at) doubango (DOT) org>
*
* This file is part of Open Source Doubango Framework.
*
* DOUBANGO is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* DOUBANGO is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*
*/

/**@file tsms_packing.c
 * @brief SMS Packing (3GPP TS 23.038 subclause 6.1.2).
 *
 * @author Mamadou Diop <diopmamadou [at) doubango (DOT) org>
 *

 */
#include "tinysms/tsms_packing.h"

#include "tinysms/tsms_etsi_gsm_03_38.h"

#include "tsk_memory.h"
#include "tsk_string.h"
#include "tsk_debug.h"

#include <string.h> /* strlen() */
#include <stdbool.h> /* true/false */
#include <stdint.h> /* uint16_t and etc... */

 /**@defgroup tsms_packing_group SMS Packing
 */

 /**@ingroup tsms_packing_group
 * Converts ASCII string to GSM 7-bit characters as per 3GPP TS 23.038 v911 section 6.1.2.1.
 * @param ascii Null-Terminated ascii string to convert.
 * @retval Buffer containing GSM 7-bit characters if succeed and @a Null otherwise.
 * @sa @ref tsms_pack_from_7bit
 */
tsk_buffer_t* tsms_pack_to_7bit(const char* ascii)
{
    /* 3GPP TS 23.038 - 6.1.2.1.1	Packing of 7-bit characters
        If a character number $ is noted in the following way:
            b7	b6	b5	b4	b3	b2	b1
            $a	$b	$c	$d	$e	$f	$g
        The packing of the 7-bitscharacters in octets is done by completing the octets with zeros on the left.
        For examples, packing: $
        -	one character in one octet:
        -	bits number:
            7	6	5	4	3	2	1	0
            0	1a	1b	1c	1d	1e	1f	1g

        -	two characters in two octets:
        -	bits number:
            7	6	5	4	3	2	1	0
            2g	1a	1b	1c	1d	1e	1f	1g
            0	0	2a	2b	2c	2d	2e	2f

        -	three characters in three octets:
        -	bits number:
            7	6	5	4	3	2	1	0
            2g	1a	1b	1c	1d	1e	1f	1g
            3f	3g	2a	2b	2c	2d	2e	2f
            0	0	0	3a	3b	3c	3d	3e

        -	seven characters in seven octets:
        -	bits number:
            7	6	5	4	3	2	1	0
            2g	1a	1b	1c	1d	1e	1f	1g
            3f	3g	2a	2b	2c	2d	2e	2f
            4e	4f	4g	3a	3b	3c	3d	3e
            5d	5e	5f	5g	4a	4b	4c	4d
            6c	6d	6e	6f	6g	5a	5b	5c
            7b	7c	7d	7e	7f	7g	6a	6b
            0	0	0	0	0	0	0	7a

        -	eight characters in seven octets:
        -	bits number:
            7	6	5	4	3	2	1	0
            2g	1a	1b	1c	1d	1e	1f	1g
            3f	3g	2a	2b	2c	2d	2e	2f
            4e	4f	4g	3a	3b	3c	3d	3e
            5d	5e	5f	5g	4a	4b	4c	4d
            6c	6d	6e	6f	6g	5a	5b	5c
            7b	7c	7d	7e	7f	7g	6a	6b
            8a	8b	8c	8d	8e	8f	8g	7a

        The bit number zero is always transmitted first.
        Therefore, in 140 octets, it is possible to pack (140x8)/7=160 characters.
    */
    tsk_buffer_t* ret = tsk_null;
    uint8_t* _ret = tsk_null;
    register tsk_size_t len, bcount = 1/*1-7*/, index = 0, retindex = 0, retlen = 0;
    const uint8_t* pascii = (const uint8_t*)ascii;

    if (!ascii || !(len = strlen(ascii))) {
        TSK_DEBUG_WARN("Null or Empty ascci string.");
        goto bail;
    }

    retlen = len - (len / 7) + 1;
    if (!(_ret = tsk_calloc(retlen, sizeof(uint8_t)))) {
        goto bail;
    }

    while (index < len) {
        if (index == 0) {
            _ret[retindex] |= (pascii[index] & (0xFF >> bcount)) |
                (pascii[index + 1] << (8 - bcount));
        }
        else {
            _ret[retindex] = ((pascii[index] >> (bcount - 1)) & (0xFF >> bcount))
                | (pascii[index + 1] << (8 - bcount));
        }

        /* how many bytes to use to complete on the left? */
        if (++bcount == 8) {
            bcount = 1, ++index;
        }
        /* next */
        retindex++;
        index++;
    }

    /* creat ret */
    ret = tsk_buffer_create(_ret, (retlen - 1));
bail:
    TSK_FREE(_ret);

    return ret;
}

/**@ingroup tsms_packing_group
* Get length of utf8 encoded string in therms of characters.
* @param s Null-Terminated utf8 string.
* @retval string size in chracters.
* @sa @ref tsms_pack_from_utf-16
*/
size_t utf8len(uint8_t* s)
{
    size_t len = 0;
    for (; *s; ++s) if ((*s & 0xC0) != 0x80) ++len;
    return len;
}

/**@ingroup tsms_packing_group
* Converts ASCII (or utf-8) string to utf-16 characters.
* @param ascii or utf-8 Null-Terminated string to convert.
* @retval Buffer containing utf-16 characters if succeed and @a Null otherwise.
* @sa @ref tsms_pack_from_utf16
*/
tsk_buffer_t* tsms_pack_to_ucs2(const char* utf8ptr)
{
    uint8_t* utf8 = (uint8_t*)utf8ptr;
    size_t length = utf8len((uint8_t*)utf8);
    uint8_t utf8char = 0;
    uint16_t* utf16str = NULL; 
    tsk_buffer_t* result = NULL;
    size_t n = 0;
    size_t i = 0;

    //allocate for the worst case (4 byte utf16 char)
    utf16str = (uint16_t*)tsk_calloc(sizeof(uint16_t)*2, (length + 1));
    if(!utf16str)
    {
        TSK_DEBUG_WARN("Failed to allocate buffer.");
        goto error;
    }

    while (utf8char = utf8[i++])
    {
        if (!(utf8char & 0x80))
        {
            utf16str[n++] = utf8char;
            continue;
        }

        // Get byte length
        size_t extra = 0;
        struct { 
            uint8_t bits;
            uint8_t mask;
        } chars[] = { 
            {5,0x06},
            {4,0x0e},
            {3,0x1e},
            {2,0x3e},
            {1,0x7e},
        };
        for (int j = 0; j < sizeof(chars) / sizeof(*chars); j++)
        {
            extra = ((utf8char >> chars[j].bits) == chars[j].mask) ? j + 1 : 0;
            if (extra) break;
        }
        if (!extra)
        {
            continue;
        }
        uint32_t mask = (1 << (8 - extra - 1)) - 1;
        uint32_t res = utf8char & mask;
        uint32_t next;
        size_t count = 0;

        for (count = 0; count < extra; count++)
        {
            next = utf8[i++];
            if (next >> 6 != 0x02)
                break;
            res = (res << 6) | (next & 0x3f);
        }

        if (count != extra)
        {
            i--;
            continue;
        }

        if (res <= 0xffff)
        {
            utf16str[n++] = res;
            continue;
        }

        res -= 0x10000;
        utf16str[n++] = ((res >> 10) & 0x3ff) + 0xd800;
        utf16str[n++] = (res & 0x3ff) + 0xdc00;
    }
    utf16str[n++] = 0;

    result = (tsk_buffer_t*)tsk_buffer_create(utf16str, (n * sizeof(uint16_t)));
    TSK_FREE(utf16str);

error:
    return result;
}

/**@ingroup tsms_packing_group
* Converts ASCII string to 8-bit characters.
* @param ascii Null-terminated ascii string to convert.
* @retval Buffer containing 8-bit characters if succeed and @a Null otherwise.
* @sa @ref tsms_pack_from_8bit
*/
tsk_buffer_t* tsms_pack_to_8bit(const char* ascii)
{
    register tsk_size_t i, j;
    tsk_size_t len, retlen = 0;
    uint8_t* str = tsk_null;
    tsk_buffer_t* ret = tsk_null;

    if (!ascii || !(len = strlen(ascii))) {
        TSK_DEBUG_WARN("Null or Empty gsm7bit buffer.");
        goto bail;
    }

    if (!(str = tsk_calloc(len, sizeof(uint8_t)))) {
        goto bail;
    }

    /* very bas way to do conversion ==> to be fixed */
    for (i = 0; i < len; i++) {
        for (j = 0; j < TSMS_ETSI_GSM_03_38_COUNT; j++) {
            if ((uint8_t)ascii[i] == (uint8_t)(TSMS_ETSI_GSM_03_38[j][1] & 0xFF)) {
                *(str + retlen++) = (TSMS_ETSI_GSM_03_38[j][0] & 0xFF);
                continue;
            }
        }
    }

    /* create buffer */
    ret = tsk_buffer_create(str, retlen);

bail:
    TSK_FREE(str);
    return ret;
}

/**@ingroup tsms_packing_group
* Converts GSM 7-bit characters to ASCII string.
* @param gsm7bit Buffer containing GSM 7-bit characters to convert.
* @param size The size of the buffer.
* @retval Null-terminated ASCII string, ready to be shown to the screen.
* @sa @ref tsms_pack_to_7bit
*/
char* tsms_pack_from_7bit(const void* gsm7bit, tsk_size_t size)
{
    char* ret = tsk_null;
    register tsk_size_t bcount = 1/*1-7*/, index = 0, retindex = 0, retsize = 0;
    const uint8_t* pgsm7bit = gsm7bit;

    if (!gsm7bit || !size) {
        TSK_DEBUG_WARN("Null or Empty gsm7bit buffer.");
        goto bail;
    }

    // dup the gsm7bit buffer
    retsize = size + (size / 7) + 1; // variable used for debug
    if (!(ret = tsk_calloc(retsize, sizeof(uint8_t)))) {
        goto bail;
    }

    while (index < size) {
        ret[retindex] = (pgsm7bit[index] & (0xFF >> bcount));
        if (index) {
            ret[retindex] = (ret[retindex] << (bcount - 1))
                | (pgsm7bit[index - 1] >> (8 - bcount + 1));
        }

        /* how many bytes to use to complete on the right? */
        if (bcount++ == 8) {
            bcount = 1, --index;
        }
        /* next */
        index++;
        retindex++;
    }

    /* special one */
    if (!(size % 7)) {
        ret[retindex] = pgsm7bit[size - 1] >> 1;
    }

bail:

    return ret;
}

/**
* Get length of utf8 string in bytes including \0.
* @param utf16 Buffer containing UTF-16 characters to convert.
* @param len The size of the buffer in bytes.
* @retval size of the utf8 string in bytes including \0.
* @sa @ref tsms_pack_to_ucs2
*/
size_t utf16_to_utf8_length(uint16_t* utf16, size_t len)
{
    size_t size = 0;
    size_t i = 0;
    uint16_t utfchar;

    while ((2 * i < len) && (utfchar = utf16[i++]))
    {
        if (utfchar < 0x80)
        {
            size++;
        }
        else if (utfchar < 0x800)
        {
            size += 2;
        }
        else if ((utfchar < 0xd800) || (utfchar >= 0xe000))
        {
            size += 3;
        }
        else
        {
            size += 4;
        }
    }
    return size + 1;
}

/**@ingroup tsms_packing_group
* Converts UTF-16 characters to utf-8 string.
* @param ptr Buffer containing UTF-16 characters to convert.
* @param size The size of the buffer (bytes).
* @retval Null-terminated utf-8 string, ready to be shown to the screen.
* @sa @ref tsms_pack_to_utf16
*/
char* tsms_pack_from_ucs2(const void* ptr, tsk_size_t size)
{
    const uint16_t* utf16 = (uint16_t*)ptr;
    char* utf8str = (char*)NULL;
    size_t n = 0;
    size_t i = 0;
    uint16_t utfchar = 0;

    if (!utf16 || !size) {
        TSK_DEBUG_WARN("Null or Empty gsm8bit buffer.");
        goto error;
    }

    if (!(utf8str = (char*)tsk_calloc(utf16_to_utf8_length((uint16_t*)utf16, size), sizeof(char)))) {
        TSK_DEBUG_WARN("Failed to allocate buffer.");
        goto error;
    }

    while (((2 * sizeof(char) * i) < size) && (utfchar = utf16[i++]))
    {
        if (utfchar < 0x80)
        {
            utf8str[n++] = (uint8_t)utfchar;
        }
        else if (utfchar < 0x800)
        {
            utf8str[n++] = (uint8_t)(0xc0 | (utfchar >> 6));
            utf8str[n++] = (uint8_t)(0x80 | (utfchar & 0x3f));
        }
        else if ((utfchar < 0xd800) || (utfchar >= 0xe000))
        {
            utf8str[n++] = (uint8_t)(0xe0 | (utfchar >> 12));
            utf8str[n++] = (uint8_t)(0x80 | ((utfchar >> 6) & 0x3f));
            utf8str[n++] = (uint8_t)(0x80 | (utfchar & 0x3f));
        }
        else
        {
            uint32_t utf4byteChar = 0x10000 + (((utfchar & 0x3ff) << 10) | (utf16[i++] & 0x3ff));
            utf8str[n++] = (uint8_t)(0xf0 | (utf4byteChar >> 18));
            utf8str[n++] = (uint8_t)(0x80 | ((utf4byteChar >> 12) & 0x3f));
            utf8str[n++] = (uint8_t)(0x80 | ((utf4byteChar >> 6) & 0x3f));
            utf8str[n++] = (uint8_t)(0x80 | (utf4byteChar & 0x3f));
        }
    }
    //C string terminator.
    utf8str[n] = 0;
error:
    return utf8str;
}

/**@ingroup tsms_packing_group
* Converts 8-bit characters to ASCII string.
* @param gsm8bit Buffer containing GSM 8-bit characters to convert.
* @param size The size of the buffer.
* @retval Null-terminated ASCII string, ready to be shown to the screen.
* @sa @ref tsms_pack_to_8bit
*/
char* tsms_pack_from_8bit(const void* gsm8bit, tsk_size_t size)
{
    register tsk_size_t i, j;
    char* ret = tsk_null;

    const uint8_t* pgsm8bit = gsm8bit;

    if (!gsm8bit || !size) {
        TSK_DEBUG_WARN("Null or Empty gsm8bit buffer.");
        goto bail;
    }

    if (!(ret = tsk_calloc(size + 1, sizeof(uint8_t)))) {
        goto bail;
    }

    /* Very bad way to do convertion ==> to be changed */
    for (i = 0; i < size; i++) {
        for (j = 0; j < TSMS_ETSI_GSM_03_38_COUNT; j++) {
            if (*(pgsm8bit + i) == (TSMS_ETSI_GSM_03_38[j][0] & 0xFF)) {
                *(ret + i) = (TSMS_ETSI_GSM_03_38[j][1] & 0xFF);
                continue;
            }
        }
    }

bail:
    return ret;
}
