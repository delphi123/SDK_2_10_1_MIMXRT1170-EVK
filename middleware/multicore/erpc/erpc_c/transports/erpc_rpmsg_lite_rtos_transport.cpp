/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2020 NXP
 * Copyright 2021 ACRIOS Systems s.r.o.
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "erpc_rpmsg_lite_rtos_transport.h"

#include "erpc_config_internal.h"

#include "rpmsg_ns.h"

#include <cassert>

using namespace erpc;

////////////////////////////////////////////////////////////////////////////////
// Variables
////////////////////////////////////////////////////////////////////////////////
uint8_t RPMsgBaseTransport::s_initialized = 0U;
struct rpmsg_lite_instance *RPMsgBaseTransport::s_rpmsg;

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

RPMsgRTOSTransport::RPMsgRTOSTransport(void)
: RPMsgBaseTransport()
, m_rdev(NULL)
, m_app_rp_chnl(NULL)
, m_dst_addr(0)
, m_rpmsg_queue(NULL)
, m_rpmsg_ept(NULL)
{
}

RPMsgRTOSTransport::~RPMsgRTOSTransport(void)
{
    bool skip = false;
    if (s_rpmsg != RL_NULL)
    {
        if (m_rpmsg_ept != RL_NULL)
        {
            if (RL_SUCCESS != rpmsg_lite_destroy_ept(s_rpmsg, m_rpmsg_ept))
            {
                skip = true;
            }
        }

        if (!skip && (m_rpmsg_queue != RL_NULL))
        {
            if (RL_SUCCESS != rpmsg_queue_destroy(s_rpmsg, m_rpmsg_queue))
            {
                skip = true;
            }
        }

        if (!skip && (RL_SUCCESS != rpmsg_lite_deinit(s_rpmsg)))
        {
            skip = true;
        }

        if (!skip)
        {
            s_initialized = 0U;
        }
    }
}

erpc_status_t RPMsgRTOSTransport::init(uint32_t src_addr, uint32_t dst_addr, void *base_address, uint32_t length,
                                       uint32_t rpmsg_link_id)
{
    erpc_status_t status = kErpcStatus_Success;

    if (0U == s_initialized)
    {
        s_rpmsg = rpmsg_lite_master_init(base_address, length, rpmsg_link_id, RL_NO_FLAGS);
        if (s_rpmsg == RL_NULL)
        {
            status = kErpcStatus_InitFailed;
        }

        if (status == kErpcStatus_Success)
        {
            m_rpmsg_queue = rpmsg_queue_create(s_rpmsg);
            if (m_rpmsg_queue == RL_NULL)
            {
                status = kErpcStatus_InitFailed;
            }
        }

        if (status == kErpcStatus_Success)
        {
            m_rpmsg_ept = rpmsg_lite_create_ept(s_rpmsg, src_addr, rpmsg_queue_rx_cb, m_rpmsg_queue);
            if (m_rpmsg_ept == RL_NULL)
            {
                status = kErpcStatus_InitFailed;
            }
            else
            {
                m_dst_addr = dst_addr;
                s_initialized = 1U;
            }
        }

        if (status != kErpcStatus_Success)
        {
            if (m_rpmsg_queue != RL_NULL)
            {
                if (RL_SUCCESS == rpmsg_queue_destroy(s_rpmsg, m_rpmsg_queue))
                {
                    m_rpmsg_queue = NULL;
                }
            }

            if (s_rpmsg != RL_NULL)
            {
                if (RL_SUCCESS == rpmsg_lite_deinit(s_rpmsg))
                {
                    s_rpmsg = NULL;
                }
            }
        }
    }

    return status;
}

erpc_status_t RPMsgRTOSTransport::init(uint32_t src_addr, uint32_t dst_addr, void *base_address, uint32_t rpmsg_link_id,
                                       void (*ready_cb)(void), char *nameservice_name)
{
    erpc_status_t status = kErpcStatus_Success;

    if (0U == s_initialized)
    {
        s_rpmsg = rpmsg_lite_remote_init(base_address, rpmsg_link_id, RL_NO_FLAGS);
        if (s_rpmsg == RL_NULL)
        {
            status = kErpcStatus_InitFailed;
        }

        if (status == kErpcStatus_Success)
        {
            /* Signal the other core we are ready */
            if (ready_cb != NULL)
            {
                ready_cb();
            }

            while (0 == rpmsg_lite_is_link_up(s_rpmsg))
            {
            }

            m_rpmsg_queue = rpmsg_queue_create(s_rpmsg);
            if (m_rpmsg_queue == RL_NULL)
            {
                status = kErpcStatus_InitFailed;
            }
        }

        if (status == kErpcStatus_Success)
        {
            m_rpmsg_ept = rpmsg_lite_create_ept(s_rpmsg, src_addr, rpmsg_queue_rx_cb, m_rpmsg_queue);
            if (m_rpmsg_ept == RL_NULL)
            {
                status = kErpcStatus_InitFailed;
            }
        }

        if (status == kErpcStatus_Success)
        {
            if (NULL != nameservice_name)
            {
                if (RL_SUCCESS != rpmsg_ns_announce(s_rpmsg, m_rpmsg_ept, nameservice_name, (uint32_t)RL_NS_CREATE))
                {
                    status = kErpcStatus_InitFailed;
                }
            }
        }

        if (status == kErpcStatus_Success)
        {
            m_dst_addr = dst_addr;
            s_initialized = 1U;
        }
        else
        {
            if (m_rpmsg_ept != RL_NULL)
            {
                if (RL_SUCCESS == rpmsg_lite_destroy_ept(s_rpmsg, m_rpmsg_ept))
                {
                    m_rpmsg_ept = NULL;
                }
            }

            if (m_rpmsg_queue != RL_NULL)
            {
                if (RL_SUCCESS == rpmsg_queue_destroy(s_rpmsg, m_rpmsg_queue))
                {
                    m_rpmsg_queue = NULL;
                }
            }

            if (s_rpmsg != RL_NULL)
            {
                if (RL_SUCCESS == rpmsg_lite_deinit(s_rpmsg))
                {
                    s_rpmsg = NULL;
                }
            }
        }
    }

    return status;
}

erpc_status_t RPMsgRTOSTransport::receive(MessageBuffer *message)
{
    char *buf = NULL;
    uint32_t length = 0;
    int32_t ret_val;

    ret_val = rpmsg_queue_recv_nocopy(s_rpmsg, m_rpmsg_queue, &m_dst_addr, &buf, &length, RL_BLOCK);
    assert(buf);
    message->set((uint8_t *)buf, length);
    message->setUsed(length);

    return (ret_val != RL_SUCCESS) ? kErpcStatus_ReceiveFailed : kErpcStatus_Success;
}

erpc_status_t RPMsgRTOSTransport::send(MessageBuffer *message)
{
    erpc_status_t status = kErpcStatus_Success;
    uint8_t *buf = message->get();
    uint32_t length = message->getLength();
    uint32_t used = message->getUsed();
    int32_t ret_val;

    message->set(NULL, 0);
    ret_val = rpmsg_lite_send_nocopy(s_rpmsg, m_rpmsg_ept, m_dst_addr, buf, used);
    if (ret_val == RL_SUCCESS)
    {
        status = kErpcStatus_Success;
    }
    else
    {
        message->set(buf, length);
        message->setUsed(used);
        status = kErpcStatus_SendFailed;
    }

    return status;
}
