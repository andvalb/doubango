/*
* Copyright (C) 2009 Mamadou Diop.
*
* Contact: Mamadou Diop <diopmamadou@yahoo.fr>
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
* GNU Lesser General Public License for more details.
*	
* You should have received a copy of the GNU General Public License
* along with DOUBANGO.
*
*/

/**@file tnet_turn_attribute.c
 * @brief New STUN Attributes as per draft-ietf-behave-turn-16 subclause 14.
 *
 * @author Mamadou Diop <diopmamadou(at)yahoo.fr>
 *
 * @date Created: Sat Nov 8 16:54:58 2009 mdiop
 */
#include "tnet_turn_attribute.h"

#include "../stun/tnet_stun.h"

#include "tsk_debug.h"


tnet_stun_attribute_t* tnet_turn_attribute_deserialize(tnet_stun_attribute_type_t type, uint16_t length, const void* payload, size_t payload_size)
{
	tnet_stun_attribute_t *attribute = 0;
	const uint8_t* dataPtr = payload;

	/* Attribute Value
	*/
	
	switch(type)
	{
	/*	draft-ietf-behave-turn-16 - 14.1.  CHANNEL-NUMBER */
	case stun_channel_number:
		{
			TSK_DEBUG_ERROR("==> NOT IMPLEMENTED");
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.2.  LIFETIME */
	case stun_lifetime:
		{
			uint32_t lifetime = ntohl(*((uint32_t*)dataPtr));
			attribute = TNET_TURN_ATTRIBUTE_LIFETIME_CREATE(lifetime);
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.3.  XOR-PEER-ADDRESS */
	case stun_xor_peer_address:
		{
			TSK_DEBUG_ERROR("==> NOT IMPLEMENTED");
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.4.  DATA */
	case stun_data:
		{
			TSK_DEBUG_ERROR("==> NOT IMPLEMENTED");
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.5.  XOR-RELAYED-ADDRESS */
	case stun_xor_relayed_address:
		{
			attribute = TNET_TURN_ATTRIBUTE_XRELAYED_ADDR_CREATE(dataPtr, length);
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.6.  EVEN-PORT */
	case stun_even_port:
		{
			TSK_DEBUG_ERROR("==> NOT IMPLEMENTED");
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.7.  REQUESTED-TRANSPORT */
	case stun_requested_transport:
		{
			TSK_DEBUG_ERROR("==> NOT IMPLEMENTED");
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.8.  DONT-FRAGMENT */
	case stun_dont_fragment:
		{
			TSK_DEBUG_ERROR("==> NOT IMPLEMENTED");
			break;
		}
	
	/*	draft-ietf-behave-turn-16 - 14.9.  RESERVATION-TOKEN */
	case stun_reservation_token:
		{
			TSK_DEBUG_ERROR("==> NOT IMPLEMENTED");
			break;
		}

	default:
		{
			TSK_DEBUG_ERROR("==> NOT IMPLEMENTED");
			break;
		}
	}

	return attribute;
}


int tnet_turn_attribute_serialize(const tnet_stun_attribute_t* attribute, tsk_buffer_t *output)
{
	if(!attribute || !output)
	{
		return -1;
	}

	/* Attribute Value
	*/

	switch(attribute->type)
	{
	/*	draft-ietf-behave-turn-16 - 14.1.  CHANNEL-NUMBER */
	case stun_channel_number:
		{
			TSK_DEBUG_ERROR("SERIALIZE:CHANNEL-NUMBER ==> NOT IMPLEMENTED");
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.2.  LIFETIME */
	case stun_lifetime:
		{
			tnet_turn_attribute_lifetime_t *lifetime = (tnet_turn_attribute_lifetime_t*)attribute;
			tsk_buffer_append(output, &(lifetime->value), 4);
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.3.  XOR-PEER-ADDRESS */
	case stun_xor_peer_address:
		{
			TSK_DEBUG_ERROR("SERIALIZE:XOR-PEER-ADDRESS ==> NOT IMPLEMENTED");
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.4.  DATA */
	case stun_data:
		{
			TSK_DEBUG_ERROR("SERIALIZE:DATA ==> NOT IMPLEMENTED");
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.5.  XOR-RELAYED-ADDRESS */
	case stun_xor_relayed_address:
		{
			TSK_DEBUG_ERROR("SERIALIZE:XOR-RELAYED-ADDRESS ==> NOT IMPLEMENTED");
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.6.  EVEN-PORT */
	case stun_even_port:
		{
			tnet_turn_attribute_even_port_t *even_port = (tnet_turn_attribute_even_port_t*)attribute;
			uint8_t value = (even_port->R << 7);
			tsk_buffer_append(output, &(value), 1);
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.7.  REQUESTED-TRANSPORT */
	case stun_requested_transport:
		{
			tnet_turn_attribute_reqtrans_t *reqtrans = (tnet_turn_attribute_reqtrans_t*)attribute;
			tsk_buffer_append(output, &(reqtrans->protocol), 1);
			tsk_buffer_append(output, &(reqtrans->rffu), 3);
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.8.  DONT-FRAGMENT */
	case stun_dont_fragment:
		{
			TSK_DEBUG_ERROR("SERIALIZE:DONT-FRAGMENT ==> NOT IMPLEMENTED");
			break;
		}

	/*	draft-ietf-behave-turn-16 - 14.9.  RESERVATION-TOKEN */
	case stun_reservation_token:
		{
			TSK_DEBUG_ERROR("SERIALIZE:TOKEN ==> NOT IMPLEMENTED");
			break;
		}
	}

	return 0;
}








//========================================================
//	[[draft-ietf-behave-turn-16 - 14.1.  CHANNEL-NUMBER]] object definition
//
static void* tnet_turn_attribute_channelnum_create(void * self, va_list * app)
{
	tnet_turn_attribute_channelnum_t *attribute = self;
	if(attribute)
	{
		const void *payload = va_arg(*app, const void*);
		size_t payload_size = va_arg(*app, size_t);

		TNET_STUN_ATTRIBUTE(attribute)->type = stun_channel_number;
	}
	return self;
}

static void* tnet_turn_attribute_channelnum_destroy(void * self) 
{ 
	tnet_turn_attribute_channelnum_t *attribute = self;
	if(attribute)
	{
		
	}
	return self;
}

static const tsk_object_def_t tnet_turn_attribute_channelnum_def_s =
{
	sizeof(tnet_turn_attribute_channelnum_t),
	tnet_turn_attribute_channelnum_create,
	tnet_turn_attribute_channelnum_destroy,
	0,
};
const void *tnet_turn_attribute_channelnum_def_t = &tnet_turn_attribute_channelnum_def_s;



//========================================================
//	[[draft-ietf-behave-turn-16 - 14.2.  LIFETIME]] object definition
//
static void* tnet_turn_attribute_lifetime_create(void * self, va_list * app)
{
	tnet_turn_attribute_lifetime_t *attribute = self;
	if(attribute)
	{
		attribute->value = /*htonl*/(va_arg(*app, uint32_t));
		TNET_STUN_ATTRIBUTE(attribute)->type = stun_lifetime;
		TNET_STUN_ATTRIBUTE(attribute)->length = 4;
	}
	return self;
}

static void* tnet_turn_attribute_lifetime_destroy(void * self) 
{ 
	tnet_turn_attribute_lifetime_t *attribute = self;
	if(attribute)
	{
		
	}
	return self;
}

static const tsk_object_def_t tnet_turn_attribute_lifetime_def_s =
{
	sizeof(tnet_turn_attribute_lifetime_t),
	tnet_turn_attribute_lifetime_create,
	tnet_turn_attribute_lifetime_destroy,
	0,
};
const void *tnet_turn_attribute_lifetime_def_t = &tnet_turn_attribute_lifetime_def_s;




//========================================================
//	[[draft-ietf-behave-turn-16 - 14.3.  XOR-PEER-ADDRESS]] object definition
//
static void* tnet_turn_attribute_xpeer_addr_create(void * self, va_list * app)
{
	tnet_turn_attribute_xpeer_addr_t *attribute = self;
	if(attribute)
	{
		const void *payload = va_arg(*app, const void*);
		size_t payload_size = va_arg(*app, size_t);

		TNET_STUN_ATTRIBUTE(attribute)->type = stun_xor_peer_address;
	}
	return self;
}

static void* tnet_turn_attribute_xpeer_addr_destroy(void * self) 
{ 
	tnet_turn_attribute_xpeer_addr_t *attribute = self;
	if(attribute)
	{
		
	}
	return self;
}

static const tsk_object_def_t tnet_turn_attribute_xpeer_addr_def_s =
{
	sizeof(tnet_turn_attribute_xpeer_addr_t),
	tnet_turn_attribute_xpeer_addr_create,
	tnet_turn_attribute_xpeer_addr_destroy,
	0,
};
const void *tnet_turn_attribute_xpeer_addr_def_t = &tnet_turn_attribute_xpeer_addr_def_s;


//========================================================
//	[[draft-ietf-behave-turn-16 - 14.4.  DATA]] object definition
//
static void* tnet_turn_attribute_data_create(void * self, va_list * app)
{
	tnet_turn_attribute_data_t *attribute = self;
	if(attribute)
	{
		const void *payload = va_arg(*app, const void*);
		size_t payload_size = va_arg(*app, size_t);

		TNET_STUN_ATTRIBUTE(attribute)->type = stun_data;
	}
	return self;
}

static void* tnet_turn_attribute_data_destroy(void * self) 
{ 
	tnet_turn_attribute_data_t *attribute = self;
	if(attribute)
	{
		
	}
	return self;
}

static const tsk_object_def_t tnet_turn_attribute_data_def_s =
{
	sizeof(tnet_turn_attribute_data_t),
	tnet_turn_attribute_data_create,
	tnet_turn_attribute_data_destroy,
	0,
};
const void *tnet_turn_attribute_data_def_t = &tnet_turn_attribute_data_def_s;

//========================================================
//	[[draft-ietf-behave-turn-16 - 14.5.  XOR-RELAYED-ADDRESS]] object definition
//
static void* tnet_turn_attribute_xrelayed_addr_create(void * self, va_list * app)
{
	tnet_turn_attribute_xrelayed_addr_t *attribute = self;
	if(attribute)
	{
		const void *payload = va_arg(*app, const void*);
		size_t payload_size = va_arg(*app, size_t);

		const uint8_t *payloadPtr = (const uint8_t*)payload;
		payloadPtr += 1; /* Ignore first 8bits */

		TNET_STUN_ATTRIBUTE(attribute)->type = stun_xor_relayed_address;
		TNET_STUN_ATTRIBUTE(attribute)->length = payload_size;
		
		attribute->family = (tnet_stun_addr_family_t)(*(payloadPtr++));

		attribute->xport = ntohs(*((uint16_t*)payloadPtr));
		attribute->xport ^= 0x2112;
		payloadPtr+=2;
		

		if(attribute->family == stun_ipv4)
		{
			uint32_t addr = ntohl(*((uint32_t*)payloadPtr));
			addr ^= TNET_STUN_MAGIC_COOKIE;
			memcpy(attribute->xaddress, &addr, 4);
			payloadPtr+=4;
		}
		else if(attribute->family == stun_ipv6)
		{
			TSK_DEBUG_ERROR("IPv6 not supported yet.");
		}
		else
		{
			TSK_DEBUG_ERROR("UNKNOWN FAMILY.");
		}
	}
	return self;
}

static void* tnet_turn_attribute_xrelayed_addr_destroy(void * self) 
{ 
	tnet_turn_attribute_xrelayed_addr_t *attribute = self;
	if(attribute)
	{
		
	}
	return self;
}

static const tsk_object_def_t tnet_turn_attribute_xrelayed_addr_def_s =
{
	sizeof(tnet_turn_attribute_xrelayed_addr_t),
	tnet_turn_attribute_xrelayed_addr_create,
	tnet_turn_attribute_xrelayed_addr_destroy,
	0,
};
const void *tnet_turn_attribute_xrelayed_addr_def_t = &tnet_turn_attribute_xrelayed_addr_def_s;


//========================================================
//	[[draft-ietf-behave-turn-16 - 14.6.  EVEN-PORT]] object definition
//
static void* tnet_turn_attribute_even_port_create(void * self, va_list * app)
{
	tnet_turn_attribute_even_port_t *attribute = self;
	if(attribute)
	{
		attribute->R = va_arg(*app, unsigned);

		TNET_STUN_ATTRIBUTE(attribute)->type = stun_even_port;
		TNET_STUN_ATTRIBUTE(attribute)->length = 1;
	}
	return self;
}

static void* tnet_turn_attribute_even_port_destroy(void * self) 
{ 
	tnet_turn_attribute_even_port_t *attribute = self;
	if(attribute)
	{
		
	}
	return self;
}

static const tsk_object_def_t tnet_turn_attribute_even_port_def_s =
{
	sizeof(tnet_turn_attribute_even_port_t),
	tnet_turn_attribute_even_port_create,
	tnet_turn_attribute_even_port_destroy,
	0,
};
const void *tnet_turn_attribute_even_port_def_t = &tnet_turn_attribute_even_port_def_s;


//========================================================
//	[[draft-ietf-behave-turn-16 - 14.7.  REQUESTED-TRANSPORT]] object definition
//
static void* tnet_turn_attribute_reqtrans_create(void * self, va_list * app)
{
	tnet_turn_attribute_reqtrans_t *attribute = self;
	if(attribute)
	{
		attribute->protocol = va_arg(*app, tnet_proto_t);

		TNET_STUN_ATTRIBUTE(attribute)->type = stun_requested_transport;
		TNET_STUN_ATTRIBUTE(attribute)->length = 4;
	}
	return self;
}

static void* tnet_turn_attribute_reqtrans_destroy(void * self) 
{ 
	tnet_turn_attribute_reqtrans_t *attribute = self;
	if(attribute)
	{
		
	}
	return self;
}

static const tsk_object_def_t tnet_turn_attribute_reqtrans_def_s =
{
	sizeof(tnet_turn_attribute_reqtrans_t),
	tnet_turn_attribute_reqtrans_create,
	tnet_turn_attribute_reqtrans_destroy,
	0,
};
const void *tnet_turn_attribute_reqtrans_def_t = &tnet_turn_attribute_reqtrans_def_s;


//========================================================
//	[[draft-ietf-behave-turn-16 - 14.8.  DONT-FRAGMENT]] object definition
//
static void* tnet_turn_attribute_dontfrag_create(void * self, va_list * app)
{
	tnet_turn_attribute_dontfrag_t *attribute = self;
	if(attribute)
	{
		//const void *payload = va_arg(*app, const void*);
		//size_t payload_size = va_arg(*app, size_t);

		TNET_STUN_ATTRIBUTE(attribute)->type = stun_dont_fragment;
	}
	return self;
}

static void* tnet_turn_attribute_dontfrag_destroy(void * self) 
{ 
	tnet_turn_attribute_dontfrag_t *attribute = self;
	if(attribute)
	{
		
	}
	return self;
}

static const tsk_object_def_t tnet_turn_attribute_dontfrag_def_s =
{
	sizeof(tnet_turn_attribute_dontfrag_t),
	tnet_turn_attribute_dontfrag_create,
	tnet_turn_attribute_dontfrag_destroy,
	0,
};
const void *tnet_turn_attribute_dontfrag_def_t = &tnet_turn_attribute_dontfrag_def_s;



//========================================================
//	[[draft-ietf-behave-turn-16 - 14.9.  RESERVATION-TOKEN]] object definition
//
static void* tnet_turn_attribute_restoken_create(void * self, va_list * app)
{
	tnet_turn_attribute_restoken_t *attribute = self;
	if(attribute)
	{
		const void *payload = va_arg(*app, const void*);
		size_t payload_size = va_arg(*app, size_t);

		TNET_STUN_ATTRIBUTE(attribute)->type = stun_reservation_token;
	}
	return self;
}

static void* tnet_turn_attribute_restoken_destroy(void * self) 
{ 
	tnet_turn_attribute_restoken_t *attribute = self;
	if(attribute)
	{
		
	}
	return self;
}

static const tsk_object_def_t tnet_turn_attribute_restoken_def_s =
{
	sizeof(tnet_turn_attribute_restoken_t),
	tnet_turn_attribute_restoken_create,
	tnet_turn_attribute_restoken_destroy,
	0,
};
const void *tnet_turn_attribute_restoken_def_t = &tnet_turn_attribute_restoken_def_s;