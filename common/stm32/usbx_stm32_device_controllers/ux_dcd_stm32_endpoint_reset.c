/***************************************************************************
 * Copyright (c) 2024 Microsoft Corporation 
 * 
 * This program and the accompanying materials are made available under the
 * terms of the MIT License which is available at
 * https://opensource.org/licenses/MIT.
 * 
 * SPDX-License-Identifier: MIT
 **************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** USBX Component                                                        */
/**                                                                       */
/**   STM32 Controller Driver                                             */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

#define UX_SOURCE_CODE
#define UX_DCD_STM32_SOURCE_CODE


/* Include necessary system files.  */

#include "ux_api.h"
#include "ux_dcd_stm32.h"
#include "ux_device_stack.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_dcd_stm32_endpoint_reset                        PORTABLE C      */
/*                                                           6.1.10       */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This function will reset a physical endpoint.                       */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    dcd_stm32                             Pointer to device controller  */
/*    endpoint                              Pointer to endpoint container */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*    Completion Status                                                   */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    HAL_PCD_EP_ClrStall                   Clear STALL                   */
/*                                                                        */
/*  CALLED BY                                                             */
/*                                                                        */
/*    STM32 Controller Driver                                             */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s), used ST  */
/*                                            HAL library to drive the    */
/*                                            controller,                 */
/*                                            resulting in version 6.1    */
/*  01-31-2022     Chaoqiong Xiao           Modified comment(s),          */
/*                                            added standalone support,   */
/*                                            resulting in version 6.1.10 */
/*                                                                        */
/**************************************************************************/
UINT  _ux_dcd_stm32_endpoint_reset(UX_DCD_STM32 *dcd_stm32, UX_SLAVE_ENDPOINT *endpoint)
{

UX_INTERRUPT_SAVE_AREA
UX_DCD_STM32_ED     *ed;


    /* Get the physical endpoint address in the endpoint container.  */
    ed =  (UX_DCD_STM32_ED *) endpoint -> ux_slave_endpoint_ed;

    UX_DISABLE

    /* Set the status of the endpoint to not stalled.  */
    ed -> ux_dcd_stm32_ed_status &= ~(UX_DCD_STM32_ED_STATUS_STALLED |
                                      UX_DCD_STM32_ED_STATUS_DONE |
                                      UX_DCD_STM32_ED_STATUS_SETUP);

    /* Set the state of the endpoint to IDLE.  */
    ed -> ux_dcd_stm32_ed_state =  UX_DCD_STM32_ED_STATE_IDLE;

    /* Clear STALL condition.  */
    HAL_PCD_EP_ClrStall(dcd_stm32 -> pcd_handle, endpoint -> ux_slave_endpoint_descriptor.bEndpointAddress);

    /* Flush buffer.  */
    HAL_PCD_EP_Flush(dcd_stm32 -> pcd_handle, endpoint->ux_slave_endpoint_descriptor.bEndpointAddress);

#ifndef UX_DEVICE_STANDALONE

    /* Wakeup pending thread.  */
    if (endpoint -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_semaphore.tx_semaphore_suspended_count)
        _ux_utility_semaphore_put(&endpoint -> ux_slave_endpoint_transfer_request.ux_slave_transfer_request_semaphore);
#endif

    UX_RESTORE

    /* This function never fails.  */
    return(UX_SUCCESS);
}
