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
* @sa @ref tsms_pack_from_ucs2
*/
size_t utf8len(char* s)
{
    size_t len = 0;
    for (; *s; ++s) if ((*s & 0xC0) != 0x80) ++len;
    return len;
}

/**@ingroup tsms_packing_group
* Converts ASCII string to UCS-2(2-byte Universal Character Set) characters.
* @param ascii Null-Terminated ascii string to convert.
* @retval Buffer containing UCS-2 characters if succeed and @a Null otherwise.
* @sa @ref tsms_pack_from_ucs2
*/
tsk_buffer_t* tsms_pack_to_ucs2(const char* mbStr)
{
    tsk_size_t len, size, retlen = 0;
    unsigned char* ucs2 = tsk_null;
    tsk_buffer_t* ret = tsk_null;
    unsigned char* ucs2begin = tsk_null;


    if (!mbStr || !(utf8len((char*)mbStr) > 0)) {
        TSK_DEBUG_WARN("Null or Empty gsm7bit buffer.");
        goto bail;
    }
    /* length in characters */
    len = utf8len((char*)mbStr);
    /* size in bytes without \0 */
    size = strlen(mbStr);
    /* each character always encoded as 2 bytes (this is ucs2) and + 1 for \0*/
    if (!(ucs2begin = ucs2 = (unsigned char*)tsk_calloc(len + 1, sizeof(uint8_t) * 2))) {
        goto bail;
    }

    for (tsk_size_t i = 0; i < size && size > 1; )
    {
        if ((*mbStr & 0x80) == 0)
        {
            *ucs2++ = 0;
            *ucs2++ = *mbStr++;
            i++;
        }
        else if ((*mbStr & 0xe0) == 0xc0)
        {
            *ucs2++ = (*mbStr >> 2) & 7;
            *ucs2++ = ((mbStr[0] & 3) << 6) | (mbStr[1] & 0x3f);
            mbStr += 2;
            i += 2;

        }
        else if ((*mbStr & 0xf0) == 0xe0)
        {
            *ucs2++ = (*mbStr << 4) | ((mbStr[1] >> 2) & 0x0f);
            *ucs2++ = (mbStr[1] << 6) | (mbStr[2] & 0x3f);
            mbStr += 3;
            i += 3;
        }
        else if ((*mbStr & 0xf8) == 0xf0)
        {
            mbStr += 4;
            i += 4;
        }
        else if ((*mbStr & 0xfc) == 0xf8)
        {
            mbStr += 5;
            i += 5;
        }
        else if ((*mbStr & 0xfe) == 0xfc)
        {
            mbStr += 6;
            i += 6;
        }
        else
        {
            mbStr++;
            i++;
        }
    }
    /* create buffer */
    ret = (tsk_buffer_t*)tsk_buffer_create(ucs2begin, (ucs2 - ucs2begin) + 1);
    TSK_FREE(ucs2begin);

bail:
    return ret;
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
* Get length of utf8 string in bytes.
* @param p Buffer containing UCS-2 characters to convert.
* @param len The size of the buffer.
* @param max The size limit of the buffer.
* @retval size of the utf8 string in bytes.
* @sa @ref tsms_pack_to_ucs2
*/
size_t ucs2_to_utf8_length(unsigned char* p, int len, int max)
{
    size_t s = 0;
    int i;

    len &= -2;
    for (i = 0; i < len && max > 3; i += 2)
    {
        if (p[i] == 0 && p[i + 1] <= 0x7f)
        {
            s += 1;
            max--;
        }
        else if ((p[i] & 0xf8) == 0)
        {
            s += 2;
        }
        else
        {
            s += 3;
        }
    }

    if (i < len)
    {
        if (p[i] == 0 && p[i + 1] <= 0x7f && max > 1)
        {
            s += 1;
            max--;
        }
        else if ((p[i] & 0xf8) == 0 && max > 2)
        {
            s += 2;
        }
    }
    return s;
}

/**@ingroup tsms_packing_group
* Converts UCS-2(2-byte Universal Character Set) characters to ASCII string.
* @param ucs2 Buffer containing UCS-2 characters to convert.
* @param size The size of the buffer.
* @retval Null-terminated ASCII string, ready to be shown to the screen.
* @sa @ref tsms_pack_to_ucs2
*/
char* tsms_pack_from_ucs2(const void* ucs2ptr, tsk_size_t size)
{
    register tsk_size_t i;
    char* utf8str = tsk_null;
    unsigned char* ucs2char = (unsigned char*)ucs2ptr;
    char* utf8begin = tsk_null;
    size_t max = (size * 3) + 1; //worst case

    size_t len = ucs2_to_utf8_length(ucs2char, size, max);

    if (!ucs2char || !size) {
        TSK_DEBUG_WARN("Null or Empty gsm8bit buffer.");
        goto bail;
    }

    if (!(utf8str = (char*)tsk_calloc(len + 1, sizeof(uint8_t)))) {
        goto bail;
    }
    utf8begin = utf8str;

    size &= -2;
    for (i = 0; i < size && max > 3; i += 2)
    {
        if (ucs2char[i] == 0 && ucs2char[i + 1] <= 0x7f)
        {
            *utf8str++ = ucs2char[i + 1];
            max--;
        }
        else if ((ucs2char[i] & 0xf8) == 0)
        {
            *utf8str++ = 0xc0 | (ucs2char[i] << 2) | (ucs2char[i + 1] >> 6);
            *utf8str++ = 0x80 | (ucs2char[i + 1] & 0x3f);
        }
        else
        {
            *utf8str++ = 0xe0 | (ucs2char[i] >> 4);
            *utf8str++ = 0x80 | ((ucs2char[i] & 0x0f) << 2) | (ucs2char[i + 1] >> 6);
            *utf8str++ = 0x80 | (ucs2char[i + 1] & 0x3f);
        }
    }

    if (i < size)
    {
        if (ucs2char[i] == 0 && ucs2char[i + 1] <= 0x7f && max > 1)
        {
            *utf8str++ = ucs2char[1];
            max--;
        }
        else if ((ucs2char[i] & 0xf8) == 0 && max > 2)
        {
            *utf8str++ = 0xc0 | (ucs2char[i] << 2) | (ucs2char[i + 1] >> 6);
            *utf8str++ = 0x80 | (ucs2char[i + 1] & 0x3f);
        }
    }

    *utf8str++ = 0;
bail:
    return utf8begin;
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
