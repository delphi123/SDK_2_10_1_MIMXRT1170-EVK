/*
 * Copyright 2018 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Generated by erpcgen 1.8.1 on Fri Apr 16 14:22:27 2021.
 *
 * AUTOGENERATED - DO NOT EDIT
 */


#include "erpc_two_way_rpc_Core0Interface_server.h"
#include <new>
#include "erpc_port.h"
#include "erpc_manually_constructed.h"

#if 10801 != ERPC_VERSION_NUMBER
#error "The generated shim code version is different to the rest of eRPC code."
#endif

using namespace erpc;
using namespace std;

#if ERPC_NESTED_CALLS_DETECTION
extern bool nestingDetection;
#endif

static ManuallyConstructed<Core0Interface_service> s_Core0Interface_service;


static const getNumberCallback_t _getNumberCallback_t[] = { getNumberFromCore1, getNumberFromCore0 };


// Call the correct server shim based on method unique ID.
erpc_status_t Core0Interface_service::handleInvocation(uint32_t methodId, uint32_t sequence, Codec * codec, MessageBufferFactory *messageFactory)
{
    erpc_status_t erpcStatus;
    switch (methodId)
    {
        case kCore0Interface_setGetNumberFunction_id:
        {
            erpcStatus = setGetNumberFunction_shim(codec, messageFactory, sequence);
            break;
        }

        case kCore0Interface_getGetNumberFunction_id:
        {
            erpcStatus = getGetNumberFunction_shim(codec, messageFactory, sequence);
            break;
        }

        case kCore0Interface_nestedCallGetNumber_id:
        {
            erpcStatus = nestedCallGetNumber_shim(codec, messageFactory, sequence);
            break;
        }

        case kCore0Interface_getNumberFromCore1_id:
        {
            erpcStatus = getNumberFromCore1_shim(codec, messageFactory, sequence);
            break;
        }

        default:
        {
            erpcStatus = kErpcStatus_InvalidArgument;
            break;
        }
    }

    return erpcStatus;
}

// Server shim for setGetNumberFunction of Core0Interface interface.
erpc_status_t Core0Interface_service::setGetNumberFunction_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    getNumberCallback_t getNumberCallbackParam = NULL;

    // startReadMessage() was already called before this shim was invoked.

    codec->readCallback((arrayOfFunPtr)(_getNumberCallback_t), 2, (funPtr *)(&getNumberCallbackParam));

    err = codec->getStatus();
    if (err == kErpcStatus_Success)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        setGetNumberFunction(getNumberCallbackParam);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif
    }

    return err;
}

// Server shim for getGetNumberFunction of Core0Interface interface.
erpc_status_t Core0Interface_service::getGetNumberFunction_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    getNumberCallback_t *getNumberCallbackParam = NULL;

    // startReadMessage() was already called before this shim was invoked.

    getNumberCallbackParam = (getNumberCallback_t *) erpc_malloc(sizeof(getNumberCallback_t));
    if (getNumberCallbackParam == NULL)
    {
        codec->updateStatus(kErpcStatus_MemoryError);
    }

    err = codec->getStatus();
    if (err == kErpcStatus_Success)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        getGetNumberFunction(getNumberCallbackParam);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (err == kErpcStatus_Success)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kCore0Interface_service_id, kCore0Interface_getGetNumberFunction_id, sequence);

        codec->writeCallback((arrayOfFunPtr)(_getNumberCallback_t), 2, (funPtr)(*getNumberCallbackParam));

        err = codec->getStatus();
    }

    if (getNumberCallbackParam)
    {
        erpc_free(getNumberCallbackParam);
    }

    return err;
}

// Server shim for nestedCallGetNumber of Core0Interface interface.
erpc_status_t Core0Interface_service::nestedCallGetNumber_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    getNumberCallback_t getNumberCallbackParam = NULL;

    // startReadMessage() was already called before this shim was invoked.

    codec->readCallback((arrayOfFunPtr)(_getNumberCallback_t), 2, (funPtr *)(&getNumberCallbackParam));

    err = codec->getStatus();
    if (err == kErpcStatus_Success)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        nestedCallGetNumber(getNumberCallbackParam);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (err == kErpcStatus_Success)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kCore0Interface_service_id, kCore0Interface_nestedCallGetNumber_id, sequence);

        err = codec->getStatus();
    }

    return err;
}

// Server shim for getNumberFromCore1 of Core0Interface interface.
erpc_status_t Core0Interface_service::getNumberFromCore1_shim(Codec * codec, MessageBufferFactory *messageFactory, uint32_t sequence)
{
    erpc_status_t err = kErpcStatus_Success;

    uint32_t number;

    // startReadMessage() was already called before this shim was invoked.

    err = codec->getStatus();
    if (err == kErpcStatus_Success)
    {
        // Invoke the actual served function.
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = true;
#endif
        getNumberFromCore1(&number);
#if ERPC_NESTED_CALLS_DETECTION
        nestingDetection = false;
#endif

        // preparing MessageBuffer for serializing data
        err = messageFactory->prepareServerBufferForSend(codec->getBuffer());
    }

    if (err == kErpcStatus_Success)
    {
        // preparing codec for serializing data
        codec->reset();

        // Build response message.
        codec->startWriteMessage(kReplyMessage, kCore0Interface_service_id, kCore0Interface_getNumberFromCore1_id, sequence);

        codec->write(number);

        err = codec->getStatus();
    }

    return err;
}

erpc_service_t create_Core0Interface_service()
{
    s_Core0Interface_service.construct();
    return s_Core0Interface_service.get();
}

void destroy_Core0Interface_service()
{
    s_Core0Interface_service.destroy();
}
