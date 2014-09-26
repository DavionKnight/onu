/***********************************************************************/
/* This file contains unpublished documentation and software                                            */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure,                              */
/* in whole or in part, of the information in this file without a                                              */
/* written consent of an officer of Cortina Systems Incorporated is                                     */
/* strictly prohibited.                                                                                                             */
/* Copyright (c) 2002-2010 by Cortina Systems Incorporated.                                            */
/***********************************************************************/

#include "../include/gw_timer.h"
#include "../include/gw_os_common.h"
#include "../apps/gw_log.h"

gw_uint32 timer_thread_count = 0;
gw_uint32 sys_interval_in_tick = 1;
gw_timer_control_t gw_timers;
gw_uint32 timer_pool_id;
gw_int32 gw_timers_debug = 0;

gw_int32 gw_timer_log( const gw_int8* string, ...)
{
	return gw_log(GW_LOG_LEVEL_DEBUG, string);
}

static
gw_int32 gw_process_timer()
{
    gw_timer_t *pTimer = (gw_timer_t *)0;
    gw_timer_t *pExpireTimer = (gw_timer_t *)0;
    gw_uint64  begin_time = 0;
    gw_uint64  end_time = 0;
    gw_uint64  cb_begin_time=0;
    gw_uint64  cb_end_time=0;

    begin_time = gw_current_time();
    gw_mutex_lock(gw_timers.timer_mutex_id);
    gw_lst_init(&gw_timers.expire_timer_list , NULL);
    for (pTimer = (gw_timer_t *)gw_lst_first(&gw_timers.timer_list) ; pTimer ;) {
        if(pTimer->remain_interval < gw_timers.delta)
            pTimer->remain_interval = 0;
        else
            pTimer->remain_interval -= gw_timers.delta;

        if(pTimer->remain_interval > 0)
            pTimer->remain_interval -= sys_interval_in_tick;

        if (pTimer->remain_interval >= sys_interval_in_tick) {
            pTimer = (gw_timer_t *)gw_lst_next((gw_node *)pTimer);
            continue;
        }

        pExpireTimer = pTimer;
        pTimer = (gw_timer_t *)gw_lst_next((gw_node *)pTimer);
        
        gw_lst_delete(&gw_timers.timer_list , (gw_node *)pExpireTimer);
        gw_lst_add(&gw_timers.expire_timer_list , (gw_node *)pExpireTimer);
        gw_timers.total_timeout_timer++;
    }
    gw_mutex_unlock(gw_timers.timer_mutex_id);

    while((pExpireTimer = (gw_timer_t *)gw_lst_get(&gw_timers.expire_timer_list)) != NULL)
    {
        cb_begin_time = gw_current_time();
        if (pExpireTimer->callback) {
            pExpireTimer->callback(pExpireTimer);
        }
        cb_end_time = gw_current_time();
        if(cb_end_time - cb_begin_time > gw_timers.callback_peek_time)
        {
            gw_timers.callback_peek_time = cb_end_time - cb_begin_time;
            if(GW_ONCE_TIMER == pExpireTimer->timer_type)
            {
                gw_timers.callback_peek_ptr = (gw_uint32)pExpireTimer->callback;
            }
            else
            {
                // circle timer need to record real app's callback
                gw_timers.callback_peek_ptr = (gw_uint32)pExpireTimer->app_callback;
            }
        }
    }

    end_time = gw_current_time();
    gw_timers.delta = end_time - begin_time;
    gw_timers.total_timer_delta += gw_timers.delta;
    return GW_TIMER_OK;
}


/***********************************************************************
* NAME : gw_create_timer
* DESC :
*             Only allocate a memory block and initialize the memory according to these argu-
*             ments you provided . It is still not active unless you activate it. The callback function
*             will be involved when the timer is timeout. When you start the timer , perhaps the
*             timer will not run immediately.If the value of tiggle is zero,the timer will run immediately
*             when you start it.
*             For Example:
*                gw_create_timer(cb_func , 10 , 300)
*                if you start this timer , the timer will be activated after 10 ms, and will be timeout
*                after 310 ms . When the timer is timeout, cb_func is involved.
* ARGUMENT:
*             callback - pointer of callback function
*             triggle -
*             timeout -
* RETURN :
*             If create successfully , return the pointer of the timer , else return NULL
***********************************************************************/
static
gw_timer_t *gw_create_timer(
    gw_int32 (*callback)(gw_timer_t *pTimer) ,
    gw_int32 triggle ,
    gw_int32 timeout)
{
    gw_timer_t *pTimer = (gw_timer_t *)0;

    if (callback == NULL || triggle < 0 || timeout <= 0) {
        gw_timer_log("\r\n gw_create_timer failed , cause some parameter is null");
        return (gw_timer_t *)pTimer;
    }

//    pTimer = (gw_timer_t *)gw_mem_malloc(timer_pool_id);
    pTimer = (gw_timer_t*)malloc(sizeof(gw_timer_t));
    if (pTimer == (gw_timer_t *)0) {
        gw_timer_log("\r\n timer memory alloc failed");
        return (gw_timer_t *)pTimer;
    }

    memset((void *)pTimer , 0 , sizeof(gw_timer_t));
    pTimer->interval = pTimer->remain_interval  = (gw_int32)gw_milli_to_ticks(timeout);
    pTimer->callback = callback;
    pTimer->queue_id = GW_TIMER_NO_QUEUE;

    return (gw_timer_t *)pTimer;
}

static
gw_int32 gw_lookup_timer(gw_timer_t *pTimer)
{
    gw_timer_t *p = NULL;

    if(pTimer == NULL) {
        return GW_TIMER_ERROR;
    }

    gw_mutex_lock(gw_timers.timer_mutex_id);
    gw_lst_scan(&gw_timers.timer_list, p, gw_timer_t *)
    {
        if(p == pTimer) {
            gw_mutex_unlock(gw_timers.timer_mutex_id);
            return GW_TIMER_OK;
        }
    }
    gw_mutex_unlock(gw_timers.timer_mutex_id);

    return GW_TIMER_ERROR;
}


/***********************************************************************
* NAME : gw_count_timer
* DESC :
*            Get the count of timers in system timer-list
* ARGUMENT:
*             N/A
* RETURN :
*             count of timers
***********************************************************************/
gw_uint32 gw_count_timer()
{
    return gw_lst_count(&gw_timers.timer_list);
}

/***********************************************************************
* NAME : gw_destroy_timer
* DESC :
*            Destory a timer and free the memory. If the timer is active, you can not deotry it
*            successfully. If you have to destory it , you must stop if firstly.
* ARGUMENT:
*             pTimer - pointer of the timer you want to destroy
* RETURN :
*             If destroy it successfully , return GW_TIMER_OK , else return GW_TIMER_ERROR
***********************************************************************/
static
gw_int32 gw_destroy_timer(gw_timer_t *pTimer)
{
    if (pTimer == (gw_timer_t *)0) {
        gw_timer_log("\r\n You want to destroy a NULL timer?");
        return GW_TIMER_OK;
    }

    pTimer->stop_flag = GW_TIMER_DESTROYED;
    if(GW_TIMER_OK == gw_lookup_timer(pTimer)) {
        gw_timer_log("\r\n The timer is active , pls stop it firstly.");
        return GW_TIMER_OK;
    }

//    gw_mem_free((gw_uint8 *)pTimer);
    free(pTimer);

    return GW_TIMER_OK;
}

/***********************************************************************
* NAME : gw_start_timer
* DESC :
*            Activate the timer , add it to the system timer-list.
* ARGUMENT:
*             pTimer - pointer of the timer
* RETURN :
*             If start it successfully , return GW_TIMER_OK , else return GW_TIMER_ERROR
***********************************************************************/
static
gw_int32 gw_start_timer(gw_timer_t *pTimer)
{
    gw_timer_t *pNode = NULL;
    gw_int32 flag=0;

    if (pTimer == (gw_timer_t *)0) {
        gw_timer_log("\r\n start a NULL timer");
        return GW_TIMER_ERROR;
    }

    pTimer->stop_flag = GW_TIMER_NO_STOPPED;
    if(GW_TIMER_OK == gw_lookup_timer(pTimer)) {
        gw_timer_log("\r\n The timer is active already, do not need start");
        return GW_TIMER_OK;
    }

    gw_mutex_lock(gw_timers.timer_mutex_id);
    pTimer->remain_interval = pTimer->interval;
    gw_lst_scan(&gw_timers.timer_list, pNode, gw_timer_t *)
    {
        if(pNode->remain_interval <= pTimer->remain_interval)
            continue;

        gw_lst_insert(&gw_timers.timer_list, gw_lst_prev((gw_node *)pNode), (gw_node *)pTimer);
        flag = 1;
        break;
    }

    if(!flag)
        gw_lst_add(&gw_timers.timer_list , (gw_node *)pTimer);

    if(gw_timers.timer_peek_value <= gw_count_timer())
        gw_timers.timer_peek_value = gw_count_timer();
    gw_mutex_unlock(gw_timers.timer_mutex_id);
    return GW_TIMER_OK;
}

/***********************************************************************
* NAME : gw_stop_timer
* DESC :
*            Stop the timer , delete it from the system timer-list
* ARGUMENT:
*             pTimer - pointer of the timer
* RETURN :
*             If stop it successfully , return GW_TIMER_OK , else return GW_TIMER_ERROR
***********************************************************************/
static
gw_int32 gw_stop_timer(gw_timer_t *pTimer)
{
    if (pTimer == (gw_timer_t *)0) {
        gw_timer_log("\r\n stop a NULL timer");
        return GW_TIMER_ERROR;
    }

    pTimer->stop_flag = GW_TIMER_STOPPED;
    if(GW_TIMER_ERROR == gw_lookup_timer(pTimer)) {
        gw_timer_log("\r\n the timer is non-active");
        return GW_TIMER_OK;
    }

    gw_mutex_lock(gw_timers.timer_mutex_id);
    gw_lst_delete(&gw_timers.timer_list , (gw_node *)pTimer);
    gw_mutex_unlock(gw_timers.timer_mutex_id);

    return GW_TIMER_OK;
}

/***********************************************************************
* NAME : gw_restart_timer
* DESC :
*            Restart the timer. Firstly we stop it , then start it again.
* ARGUMENT:
*             pTimer - pointer of the timer
* RETURN :
*             If restart it successfully , return GW_TIMER_OK , else return GW_TIMER_ERROR
***********************************************************************/
static
gw_int32 gw_restart_timer(gw_timer_t *pTimer)
{
    if (pTimer == (gw_timer_t *)0) {
        gw_timer_log("\r\n restart a NULL timer");
        return GW_TIMER_ERROR;
    }

    gw_stop_timer(pTimer);
    gw_start_timer(pTimer);

    return GW_TIMER_OK;
}

static
gw_int32 gw_retiming_timer(gw_timer_t *pTimer , gw_uint32 timeout)
{
    if(pTimer == NULL) {
        gw_timer_log("\r\n retiming failed cause some NULL parameter");
        return GW_TIMER_ERROR;
    }

    if(GW_TIMER_ERROR == gw_lookup_timer(pTimer)) {
        gw_timer_log("\r\n the timer is non-active , don't need retiming");
        return GW_TIMER_OK;
    }

    gw_mutex_lock(gw_timers.timer_mutex_id);
    pTimer->remain_interval = pTimer->interval = (gw_int32)gw_milli_to_ticks(timeout);
    gw_mutex_unlock(gw_timers.timer_mutex_id);

    return GW_TIMER_OK;
}

/* public interfaces */
void gw_timer_thread()
{
    while (1) {
        timer_thread_count++;
        gw_process_timer();
        gw_thread_delay(sys_interval_in_tick * 10); // delay in mili sec
    }
    return;
}

#define TIMER_THREAD_NAME "timer thread"
#define TIMER_THREAD_STACK_SIZE (4*1024)
#define  TIMER_THREAD_PRIORITY 8

gw_int32 gw_timer_init()
{
    gw_uint32 ret;

    memset((void *)&gw_timers , 0 , sizeof(gw_timer_control_t));
    gw_lst_init(&gw_timers.timer_list , NULL);

//    if(GW_E_OSAL_OK != gw_mempool_create(&timer_pool_id, "Timer-pool" , sizeof(gw_timer_t) , GW_MAX_TIMER_NUMBER))
//    {
//        gw_timer_log("\r\n create timer pool failed");
//        return GW_TIMER_ERROR;
//    }

    ret = gw_mutex_init(&gw_timers.timer_mutex_id, "timer_thread_mutex", 0);
//    if (ret != GW_E_OSAL_OK) {
//        gw_timer_log("\r\n gw_timer mutex create failed(%d)",ret);
//        gw_mempool_destroy(timer_pool_id);
//        return GW_TIMER_ERROR;
//    }

    ret = gw_thread_create(&gw_timers.timer_thread_id,
                             TIMER_THREAD_NAME,
                             (const void *)gw_timer_thread, (void *)0,
                             TIMER_THREAD_STACK_SIZE,
                             TIMER_THREAD_PRIORITY ,
                             0);

    if (ret != GW_E_OSAL_OK) {
        gw_timer_log("\r\n Iros-timer thread creat failed.");
        gw_mutex_destroy(gw_timers.timer_mutex_id);
//        gw_mempool_destroy(timer_pool_id);
        return GW_TIMER_ERROR;
    }

    return GW_TIMER_OK;
}



void gw_timer_show()
{
    gw_timer_t *pTimer = NULL;
    gw_int8 *circle_str = "circle";
    gw_int8 *once_str = "once";
    gw_int8 *yes_str = "yes";
    gw_int8 *no_str = "no";

    // gw_printf("\r\n========================================================================");
    gw_printf("\r\n%-10s   %-8s %-8s %-8s %-9s %-11s %-8s","Timer-ID","Remain","Triggle","O-Remain",
                                        "O-Triggle","Circle-Type","Msg-Type");
    //gw_printf("\r\n------------------------------------------------------------------------");
    gw_mutex_lock(gw_timers.timer_mutex_id);
    gw_lst_scan(&gw_timers.timer_list, pTimer, gw_timer_t *)
    {
        gw_printf("\r\n0x%08x   %-8d %-8d %-8d %-9d %-11s %-8s",(gw_uint32)pTimer,pTimer->remain_interval,
            0,pTimer->interval,0,
            (pTimer->timer_type == GW_CIRCLE_TIMER)?circle_str:once_str,
            (pTimer->queue_id != GW_TIMER_NO_QUEUE)?yes_str:no_str);
    }
    gw_mutex_unlock(gw_timers.timer_mutex_id);
    //gw_printf("\r\n========================================================================");
    gw_printf("\r\n Total Timer : %d", gw_count_timer());
    gw_printf("\r\n Total Delta : %d Ticks",gw_timers.total_timer_delta);
    gw_printf("\r\n Timer Peek  : %d Timers",gw_timers.timer_peek_value);
    gw_printf("\r\n Total Timeout Timer: %d ",gw_timers.total_timeout_timer);
    gw_printf("\r\n Callback Info      : %d ticks  cb=0x%08x",gw_timers.callback_peek_time,gw_timers.callback_peek_ptr);
    gw_printf("\r\n Msg Timer Error    : %d",gw_timers.msg_timer_sendQ_error);
    gw_printf("\r\n");
    return;
}

static void gw_timer_app_callback(gw_timer_t *pTimer)
{
    if(pTimer->timer_type == GW_CIRCLE_TIMER)
    {
        if(pTimer->stop_flag == GW_TIMER_DESTROYED)
        {
//            gw_mem_free((gw_uint8 *)pTimer);
        	free(pTimer);
            pTimer = NULL;
            return;
        }

        if(pTimer->stop_flag == GW_TIMER_STOPPED)
        {
            return;
        }
    }

    pTimer->stop_flag = GW_TIMER_NO_STOPPED;
    if(pTimer->app_callback) {
        pTimer->app_callback((void *)pTimer->data);
    }

    if(pTimer->timer_type == GW_ONCE_TIMER) {
        gw_destroy_timer(pTimer);
        pTimer = NULL;
    }
    else
    {
        gw_start_timer(pTimer);
    }

    return;
}


gw_int32 gw_timer_add(
                    gw_uint32 timeout ,
                    void (*callback)(void *) ,
                    void *data
                    )
{
    gw_timer_t *pTimer = (gw_timer_t *)NULL;

    if(callback == NULL) {
        gw_timer_log("\r\n create timer with null callback function");
        return GW_INVALID_TIMER;
    }

    pTimer = (gw_timer_t *)gw_create_timer((void *)gw_timer_app_callback, 0 , timeout);
    if(pTimer == NULL) {
        gw_timer_log("\r\n Create timer failed");
        return GW_INVALID_TIMER;
    }

    pTimer->queue_id = GW_TIMER_NO_QUEUE;
    pTimer->app_callback = callback;
    pTimer->data = data;
    pTimer->timer_type = GW_ONCE_TIMER;

    gw_start_timer(pTimer);

    return (gw_uint32)pTimer;
}

gw_int32 gw_circle_timer_add(gw_uint32 timeout, void (*callback)(void *), void *data)
{
    gw_timer_t *pTimer = (gw_timer_t *)NULL;

    if(callback == NULL) {
        gw_timer_log("\r\n create timer with null callback function");
        return GW_INVALID_TIMER;
    }

    pTimer = (gw_timer_t *)gw_create_timer((void *)gw_timer_app_callback, 0 , timeout);
    if(pTimer == NULL) {
        gw_timer_log("\r\n Create circle timer faileds");
        return GW_INVALID_TIMER;
    }

    pTimer->queue_id = GW_TIMER_NO_QUEUE;
    pTimer->app_callback = callback;
    pTimer->data = data;
    pTimer->timer_type = GW_CIRCLE_TIMER;

    gw_start_timer(pTimer);

    return (gw_uint32)pTimer;
}


gw_int32 gw_timer_del(gw_uint32 timer_handle)
{
    gw_timer_t *pTimer = (gw_timer_t *)timer_handle;

    if(pTimer == NULL) {
        gw_timer_log("\r\n Delete a NULL timer ?");
        return GW_TIMER_ERROR;
    }

    gw_stop_timer(pTimer);
    gw_destroy_timer(pTimer);

    return GW_TIMER_OK;
}

gw_int32 gw_timer_stop(gw_uint32 timer_handle)
{
    gw_timer_t *pTimer = (gw_timer_t *)timer_handle;

    if(pTimer == NULL) {
        gw_timer_log("\r\n stop a NULL timer");
        return GW_TIMER_ERROR;
    }

    gw_stop_timer(pTimer);

    return GW_TIMER_OK;
}

gw_int32 gw_timer_start(gw_uint32 timer_handle)
{
    gw_timer_t *pTimer = (gw_timer_t *)timer_handle;

    if(pTimer == NULL) {
        gw_timer_log("\r\n start a NULL timer.");
        return GW_TIMER_ERROR;
    }

    gw_start_timer(pTimer);

    return GW_TIMER_OK;
}

gw_int32 gw_timer_restart(gw_uint32 timer_handle)
{
    gw_timer_t *pTimer = (gw_timer_t *)timer_handle;

    if(pTimer == NULL) {
        gw_timer_log("\r\n restart a NULL timer");
        return GW_TIMER_ERROR;
    }

    gw_restart_timer(pTimer);

    return GW_TIMER_OK;
}

gw_int32 gw_timer_retiming(gw_uint32 timer_handle , gw_uint32 timeout)
{
    return (gw_int32)gw_retiming_timer((gw_timer_t *)timer_handle, timeout);
}


#ifdef GW_TIMERS_EX_MSG

static void gw_msg_timer_app_callback(gw_timer_t *pTimer)
{
    gw_int32 queue_type = GW_NORMAL_QUEUE_TYPE;
    gw_int32 ret = GW_E_OSAL_ERR;
    gw_int32 max_pri=0;

    if(pTimer == NULL)
    {
        return ;
    }

    queue_type = gw_queue_type_get(pTimer->queue_id);
    if(queue_type == GW_NORMAL_QUEUE_TYPE)
        ret = gw_queue_put(pTimer->queue_id, (void *)&pTimer->data, sizeof(gw_uint32), GW_OSAL_NO_WAIT , 0);
    else {
        max_pri = gw_pri_queue_max_priority(pTimer->queue_id);
        ret = gw_pri_queue_put(pTimer->queue_id, (void *)&pTimer->data, sizeof(gw_uint32), GW_OSAL_NO_WAIT , max_pri);
    }

    if(GW_E_OSAL_OK != ret)
    {
        if(pTimer->timer_type == GW_ONCE_TIMER) {
            gw_destroy_timer(pTimer);
            pTimer = NULL;
        }
        else {
            if(pTimer->data_free_func) {
            pTimer->data_free_func((void *)pTimer->data);
            }
            gw_start_timer(pTimer);
        }
           
        gw_timers.msg_timer_sendQ_error++;
        return ;
    }

    if(pTimer->timer_type == GW_ONCE_TIMER) {
        gw_destroy_timer(pTimer);
        pTimer = NULL;
    }
    else
    {
        gw_start_timer(pTimer);
    }

    return;
}


gw_int32 gw_msg_timer_add(
                        gw_uint32 queue_id ,
                        gw_int32 timeout ,
                        void *data,
                        void (*data_free_func)(void *)
                        )
{
    gw_timer_t *pTimer = (gw_timer_t *)NULL;

    pTimer = (gw_timer_t *)gw_create_timer((void *)gw_msg_timer_app_callback, 0 , timeout);
    if(pTimer == NULL) {
        gw_timer_log("\r\n create msg timer failed");
        return GW_INVALID_TIMER;
    }

    pTimer->queue_id = queue_id;
    pTimer->data = data;
    pTimer->data_free_func = data_free_func;
    pTimer->timer_type = GW_ONCE_TIMER;

    gw_start_timer(pTimer);

    return (gw_uint32)pTimer;
}

gw_int32 gw_msg_circle_timer_add(
                        gw_uint32 queue_id ,
                        gw_int32 timeout ,
                        void *data
                        )
{
    gw_timer_t *pTimer = (gw_timer_t *)NULL;

    pTimer = (gw_timer_t *)gw_create_timer((void *)gw_msg_timer_app_callback, 0 , timeout);
    if(pTimer == NULL) {
        gw_timer_log("\r\n Create msg circle timer failed");
        return GW_INVALID_TIMER;
    }

    pTimer->queue_id = queue_id;
    pTimer->data = data;
    pTimer->data_free_func = NULL;
    pTimer->timer_type = GW_CIRCLE_TIMER;

    gw_start_timer(pTimer);

    return (gw_uint32)pTimer;
}

#endif


#define GW_TIMER_DEBUG
#ifdef GW_TIMER_DEBUG

void tm_cb(void* data)
{
    gw_printf("tm %d %lld\n", *((gw_uint8*)data), gw_current_time());
}

gw_uint8 tm_data_1 = 1;
gw_uint8 tm_data_2 = 2;


void timer_test()
{
    gw_circle_timer_add(2000, tm_cb, &tm_data_1); // 2 sec
    gw_timer_add(120*1000, tm_cb, &tm_data_2); // 2 min
    gw_printf("timer created\n");
}

#endif
