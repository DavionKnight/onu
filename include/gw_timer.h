/***********************************************************************/
/* This file contains unpublished documentation and software                                            */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure,                              */
/* in whole or in part, of the information in this file without a                                              */
/* written consent of an officer of Cortina Systems Incorporated is                                     */
/* strictly prohibited.                                                                                                             */
/* Copyright (c) 2002-2010 by Cortina Systems Incorporated.                                            */
/***********************************************************************/

#ifndef _GW_TIMER_H_
#define _GW_TIMER_H_

#include "gw_types.h"
#include "util_list.h"


#define GW_TIMERS_EX_MSG


typedef struct {
    gw_uint32   timer_thread_id;
    gw_uint32   timer_mutex_id;
    gw_uint32   delta;
    gw_list     timer_list;
    gw_list     expire_timer_list;
    gw_uint32   msg_timer_sendQ_error;
    gw_uint32   total_timeout_timer;
    gw_uint32   total_timer_delta;
    gw_uint32   timer_peek_value;
    gw_uint32   callback_peek_time;
    gw_uint32   callback_peek_ptr;
} gw_timer_control_t;

typedef struct iros_timer {
    gw_node         node;
    gw_int32        interval;
    gw_int32        remain_interval;
    gw_int32        (*callback)(struct iros_timer *);
    void            *data;
    void            (*app_callback)(void *data);
    gw_int16         timer_type;
    gw_int16         stop_flag; /*1: stopped  0: not stopped*/
    gw_uint32       queue_id; /* indentify timer is via msg or callback */
#ifdef GW_TIMERS_EX_MSG
    void            (*data_free_func)(void *data);
#endif
} gw_timer_t;

#define GW_TIMER_DESTROYED  2
#define GW_TIMER_STOPPED    1
#define GW_TIMER_NO_STOPPED 0

#define GW_TIMER_OK         0
#define GW_TIMER_ERROR      -1

#define GW_INVALID_TIMER    0
#define GW_TIMER_NO_QUEUE  (-1)

#ifdef GW_OSAL_MAX_TIMER
#define GW_MAX_TIMER_NUMBER  GW_OSAL_MAX_TIMER
#else
#define GW_MAX_TIMER_NUMBER  32
#endif

typedef enum
{
    GW_ONCE_TIMER = 0,
    GW_CIRCLE_TIMER = 1,
}TIMER_TYPE;

#define iros_timer_log if(gw_timers_debug) gw_printf

extern gw_uint32 timer_thread_count;

extern gw_int32 gw_timer_init();
extern gw_int32 gw_timer_add(gw_uint32 timeout, void (*callback)(void *), void *data);
extern gw_int32 gw_circle_timer_add(gw_uint32 timeout, void (*callback)(void *), void *data);
extern gw_int32 gw_timer_del(gw_uint32 timer_handle);
extern gw_int32 gw_timer_stop(gw_uint32 timer_handle);
extern gw_int32 gw_timer_start(gw_uint32 timer_handle);
extern gw_int32 gw_timer_restart(gw_uint32 timer_handle);
extern gw_uint32 gw_count_timer();
extern void gw_timer_show();
extern gw_int32 gw_timer_retiming(gw_uint32 timer_handle , gw_uint32 timeout);

#ifdef GW_TIMERS_EX_MSG

gw_int32 gw_msg_circle_timer_add(
                        gw_uint32 queue_id ,
                        gw_int32 timeout ,
                        void *data
                        );

gw_int32 gw_msg_timer_add(
                        gw_uint32 queue_id ,
                        gw_int32 timeout ,
                        void *data,
                        void (*data_free_func)(void *)
                        );
#endif

#endif

