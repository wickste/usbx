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


/* Include necessary system files.  */

#define UX_SOURCE_CODE
#define UX_HCD_STM32_SOURCE_CODE

#include "ux_api.h"
#include "ux_hcd_stm32.h"
#include "ux_host_stack.h"


/**************************************************************************/
/*                                                                        */
/*  FUNCTION                                               RELEASE        */
/*                                                                        */
/*    _ux_hcd_stm32_port_status_get                       PORTABLE C      */
/*                                                           6.0          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*     This function will return the status for each port attached to the */
/*     root HUB.                                                          */
/*                                                                        */
/*  INPUT                                                                 */
/*                                                                        */
/*    hcd_stm32                           Pointer to STM32 controller     */
/*    port_index                            Port index                    */
/*                                                                        */
/*  OUTPUT                                                                */
/*                                                                        */
/*  STM32 Port Status                                                     */
/*                                                                        */
/*    Where port status has the following format:                         */
/*                                                                        */
/*               bit 0         device connection status                   */
/*                             if 0 : no device connected                 */
/*                             if 1 : device connected to the port        */
/*               bit 1         port enable status                         */
/*                             if 0 : port disabled                       */
/*                             if 1 : port enabled                        */
/*               bit 2         port suspend status                        */
/*                             if 0 : port is not suspended               */
/*                             if 1 : port is suspended                   */
/*               bit 3         port overcurrent status                    */
/*                             if 0 : port has no overcurrent condition   */
/*                             if 1 : port has overcurrent condition      */
/*               bit 4         port reset status                          */
/*                             if 0 : port is not in reset                */
/*                             if 1 : port is in reset                    */
/*               bit 5         port power status                          */
/*                             if 0 : port power is off                   */
/*                             if 1 : port power is on                    */
/*               bit 6-7       device attached speed                      */
/*                             if 00 : low speed device attached          */
/*                             if 01 : full speed device attached         */
/*                             if 10 : high speed device attached         */
/*                                                                        */
/*  CALLS                                                                 */
/*                                                                        */
/*    HAL_HCD_GetCurrentSpeed             Get device speed                */
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
/*                                                                        */
/**************************************************************************/
ULONG  _ux_hcd_stm32_port_status_get(UX_HCD_STM32 *hcd_stm32, ULONG port_index)
{

ULONG       port_status;


    /* Check to see if this port is valid on this controller.  */
    if (UX_HCD_STM32_NB_ROOT_PORTS < port_index)
    {

        /* Error trap. */
        _ux_system_error_handler(UX_SYSTEM_LEVEL_THREAD, UX_SYSTEM_CONTEXT_HCD, UX_PORT_INDEX_UNKNOWN);

        /* If trace is enabled, insert this event into the trace buffer.  */
        UX_TRACE_IN_LINE_INSERT(UX_TRACE_ERROR, UX_PORT_INDEX_UNKNOWN, port_index, 0, 0, UX_TRACE_ERRORS, 0, 0)

        return(UX_PORT_INDEX_UNKNOWN);
    }

    /* The port is valid, build the status mask for this port. This function
       returns a controller agnostic bit field.  */
    port_status =  0;

    /* Device Connection Status.  */
    if (hcd_stm32 -> ux_hcd_stm32_controller_flag & UX_HCD_STM32_CONTROLLER_FLAG_DEVICE_ATTACHED)
        port_status |=  UX_PS_CCS;

    switch (HAL_HCD_GetCurrentSpeed(hcd_stm32 -> hcd_handle))
    {
    case 0:
        /* High Speed.  */
        port_status |=  UX_PS_DS_HS;
        break;

    case 1:
        /* Full Speed.  */
        port_status |=  UX_PS_DS_FS;
        break;

    case 2:
        /* Low Speed.  */
        port_status |=  UX_PS_DS_LS;
        break;

    default:
        /* Full Speed.  */
        port_status |=  UX_PS_DS_FS;
        break;
    }

    /* Return port status.  */
    return(port_status);
}

