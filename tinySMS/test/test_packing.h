/*
* Copyright (C) 2009 Mamadou Diop.
*
* Contact: Mamadou Diop <diopmamadou(at)doubango.org>
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
#ifndef _TEST_SMSPACKING_H
#define _TEST_SMSPACKING_H

#include <wchar.h>
#include <uchar.h>

typedef struct test_message_7bit_s {
    const char* ascii;
    const char* _7bit;
}
test_message_7bit_t;

test_message_7bit_t test_messages_7bit[] = {
    "a","\x61",
    "ab","\x61\x31",
    "abc","\x61\xF1\x18",
    "abcdefg","\x61\xF1\x98\x5C\x36\x9F\x01",
    "abcdefgh","\x61\xF1\x98\x5C\x36\x9F\xD1",
    "abcdefghi","\x61\xF1\x98\x5C\x36\x9F\xD1\x69",
    "abcdefgh12345678","\x61\xF1\x98\x5C\x36\x9F\xD1\x31\xD9\x8C\x56\xB3\xDD\x70",
    "abcdefgh123456789","\x61\xF1\x98\x5C\x36\x9F\xD1\x31\xD9\x8C\x56\xB3\xDD\x70\x39",
    "This is a test sms message","\x54\x74\x7A\x0E\x4A\xCF\x41\x61\x10\xBD\x3C\xA7\x83\xE6\xED\x39\xA8\x5D\x9E\xCF\xC3\xE7\x32",
    "salut","\xF3\x30\xBB\x4E\x07",
    "hellohello","\xE8\x32\x9B\xFD\x46\x97\xD9\xEC\x37",
};

typedef struct test_message_utf16_s {
    const char* utf8;
    const char* utf16;
}
test_message_utf16_t;

test_message_utf16_t test_messages_utf16[] = {
     u8"Ascii test string", "\x00\x41\x00\x73\x00\x63\x00\x69\x00\x69\x00\x20\x00\x74\x00\x65\x00\x73\x00\x74\x00\x20\x00\x73\x00\x74\x00\x72\x00\x69\x00\x6e\x00\x67",
     u8"Ещё одни самый новый тест 😅😂👍🏾🤷‍♂️👨‍💻🐁🙃💪🏽☺️👋🤘🏿👆👏","\x04\x15\x04\x49\x04\x51\x00\x20\x04\x3e\x04\x34\x04\x3d\x04\x38\x00\x20\x04\x41\x04\x30\x04\x3c\x04\x4b\x04\x39\x00\x20\x04\x3d\x04\x3e\x04\x32\x04\x4b\x04\x39\x00\x20\x04\x42\x04\x35\x04\x41\x04\x42\x00\x20\xd8\x3d\xde\x05\xd8\x3d\xde\x02\xd8\x3d\xdc\x4d\xd8\x3c\xdf\xfe\xd8\x3e\xdd\x37\x20\x0d\x26\x42\xfe\x0f\xd8\x3d\xdc\x68\x20\x0d\xd8\x3d\xdc\xbb\xd8\x3d\xdc\x01\xd8\x3d\xde\x43\xd8\x3d\xdc\xaa\xd8\x3c\xdf\xfd\x26\x3a\xfe\x0f\xd8\x3d\xdc\x4b\xd8\x3e\xdd\x18\xd8\x3c\xdf\xff\xd8\x3d\xdc\x46\xd8\x3d\xdc\x4f",
     u8"มนุษย์ทั้งหลายเกิดมามี😃😄😝😖", "\x0e\x21\x0e\x19\x0e\x38\x0e\x29\x0e\x22\x0e\x4c\x0e\x17\x0e\x31\x0e\x49\x0e\x07\x0e\x2b\x0e\x25\x0e\x32\x0e\x22\x0e\x40\x0e\x01\x0e\x34\x0e\x14\x0e\x21\x0e\x32\x0e\x21\x0e\x35\xd8\x3d\xde\x03\xd8\x3d\xde\x04\xd8\x3d\xde\x1d\xd8\x3d\xde\x16"
};

void test_7bit()
{
    tsk_size_t i;
    tsk_buffer_t* buffer;
    char* temp;

    /* To 7bit */
    for(i=0; i<sizeof(test_messages_7bit)/sizeof(test_message_7bit_t); i++) {
        if((buffer = tsms_pack_to_7bit(test_messages_7bit[i].ascii))) {
            /*if((temp = tohex(buffer->data, buffer->size))){
            	if(!tsk_striequals(temp, test_messages_7bit[i]._7bit)){
            		TSK_DEBUG_ERROR("tsms_pack_to_7bit(\"%s\") Failed.\n", test_messages_7bit[i].ascii);
            	}
            	TSK_FREE(temp);
            }*/
            if(!bin_equals(buffer->data, test_messages_7bit[i]._7bit, buffer->size)) {
                TSK_DEBUG_ERROR("tsms_pack_to_7bit(\"%s\") Failed.\n", test_messages_7bit[i].ascii);
            }
            TSK_OBJECT_SAFE_FREE(buffer);
        }
    }

    /* From 7bit */
    for(i=0; i<sizeof(test_messages_7bit)/sizeof(test_message_7bit_t); i++) {
        if((temp = tsms_pack_from_7bit(test_messages_7bit[i]._7bit, (tsk_size_t)tsk_strlen(test_messages_7bit[i]._7bit)))) {
            if(!tsk_striequals(temp, test_messages_7bit[i].ascii)) {
                TSK_DEBUG_ERROR("tsms_pack_from_7bit(\"%s\") Failed.\n", test_messages_7bit[i].ascii);
            }
            TSK_FREE(temp);
        }
    }
}

void test_8bit()
{
    tsk_size_t i;
    tsk_buffer_t* buffer;
    char* temp;

    /* To 8bit */
    for(i=0; i<sizeof(test_messages_7bit)/sizeof(test_message_7bit_t); i++) {
        if((buffer = tsms_pack_to_8bit(test_messages_7bit[i].ascii))) {
            if(!tsk_strequals(buffer->data, test_messages_7bit[i].ascii)) {
                TSK_DEBUG_INFO("tsms_pack_to_8bit(%s) Failed", test_messages_7bit[i].ascii);
            }
            TSK_OBJECT_SAFE_FREE(buffer);
        }
    }

    /* From 8bit */
    for(i=0; i<sizeof(test_messages_7bit)/sizeof(test_message_7bit_t); i++) {
        if((temp = tsms_pack_from_8bit(test_messages_7bit[i].ascii, (tsk_size_t)tsk_strlen(test_messages_7bit[i].ascii)))) {
            if(!tsk_strequals(temp, test_messages_7bit[i].ascii)) {
                TSK_DEBUG_INFO("tsms_pack_from_8bit(%s) Failed", test_messages_7bit[i].ascii);
            }
            TSK_FREE(temp);
        }
    }
}

size_t U16bytes(const char16_t* str)
{
    size_t len = 0;
    char16_t* p = (char16_t*)str;
    while (*p != 0) p++;
    len = (char*)p - (char*)str;
    return len;
}

void test_utf16()
{
    tsk_size_t i;
    tsk_buffer_t* buffer;
    char* temp;

    /* To utf16 */
    for(i=0; i<sizeof(test_messages_utf16)/sizeof(test_message_utf16_t); i++) {

        if((buffer = tsms_pack_to_ucs2(test_messages_utf16[i].utf8))) {
            if(!tsk_strequals(buffer->data, test_messages_utf16[i].utf16)) {
                TSK_DEBUG_INFO("tsms_pack_to_utf16(%s) Failed", test_messages_utf16[i].utf8);
            }
            TSK_OBJECT_SAFE_FREE(buffer);
        }
    }

    /* From utf16 */
    for(i=0; i<sizeof(test_messages_utf16)/sizeof(test_message_utf16_t); i++) {
        if((temp = tsms_pack_from_ucs2(test_messages_utf16[i].utf16, (tsk_size_t)U16bytes((char16_t*)test_messages_utf16[i].utf16)))) {
            if(!tsk_strequals(temp, test_messages_utf16[i].utf8)) {
                TSK_DEBUG_INFO("tsms_pack_from_utf16(%s) Failed", test_messages_utf16[i].utf8);
            }
            TSK_FREE(temp);
        }
    }
}

void test_packing()
{
    test_utf16();
    test_8bit();
    test_7bit();
}

#endif /* _TEST_SMSPACKING_H */
