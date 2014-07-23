/***********************************************************************/
/* This file contains unpublished documentation and software                                            */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure,                              */
/* in whole or in part, of the information in this file without a                                              */
/* written consent of an officer of Cortina Systems Incorporated is                                     */
/* strictly prohibited.                                                                                                             */
/* Copyright (c) 2002-2010 by Cortina Systems Incorporated.                                            */
/***********************************************************************/

#include <unistd.h>
#include <stdio.h>

#ifdef CYG_LINUX

#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_arch.h>
#include <pkgconf/hal.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_io.h>
#include <pkgconf/system.h>
#include <pkgconf/memalloc.h>
#include <pkgconf/isoinfra.h>

#endif

#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "../cli_lib/cli_common.h"
#include "fcntl.h"
#include "pthread.h"
#include "semaphore.h"
#include "mqueue.h"

#ifndef CYG_LINUX
#include "signal.h"
#include <sys/prctl.h>
#endif

#include "../include/gw_os_common.h"
#include "../include/gw_os_api_core.h"
#include "../include/gw_types.h"
#include "../apps/gwdonuif_interval.h"

#ifdef CYG_LINUX
#define GW_OSAL_MAX_PRI         31
#else
#define GW_OSAL_MAX_PRI         250
#endif
#define GW_OSAL_UNINIT          0
#define OK	                    0

gw_uint32 gw_creator_find(void);


#ifdef CYG_LINUX

void usleep(unsigned int usecs)
{
	unsigned int msecs = 0;
	unsigned int ticks = 0;

	msecs = usecs/1000;
	ticks = msecs/10;
	if(ticks == 0)
		ticks = 1;

	cyg_thread_delay(ticks);
	return;
}

#endif

/*  tables for the properties of objects */

/* threads */
#if 1
typedef struct {
    gw_int32 free;
#ifdef CYG_LINUX
    cyg_handle_t id;
#else
    void (*func)(void*);
    gw_uint32 id;
    gw_uint32 ppid;
    gw_uint32 pid;
    void* param;
#endif
    gw_int8 name [GW_OSAL_MAX_API_NAME];
#ifdef CYG_LINUX
    cyg_handle_t creator;
#else
    gw_uint32 creator;
#endif
    gw_uint32 stack_size;
    gw_uint32 priority;
#ifdef CYG_LINUX
    cyg_thread thread_ctrl;
#endif
    gw_uint8 *stack_buf;
}osal_thread_record_t;
#else
typedef struct {
    int 			free;
    pthread_t 		id;
    char 			name[GW_OSAL_MAX_API_NAME];
    int 			creator;
    unsigned int 	stack_size;
    unsigned int 	priority;
}osal_thread_record_t;
#endif

/* Counting Semaphores */
#if 0
typedef struct {
    gw_int32 free;
    cyg_sem_t  id;                       /* a pointer to the id */
    gw_int8 name [GW_OSAL_MAX_API_NAME];
    cyg_handle_t creator;
}osal_count_sem_record_t;
#else
typedef struct {
    int 	free;
    sem_t 	id;
    char 	name[GW_OSAL_MAX_API_NAME];
    int 	creator;
}osal_count_sem_record_t;
#endif

/* Mutexes */
#if 0
typedef struct {
    gw_int32 free;
    cyg_mutex_t id;
    gw_int8 name [GW_OSAL_MAX_API_NAME];
    cyg_handle_t creator;
}osal_mut_record_t;
#else
typedef struct {
    int 			free;
    pthread_mutex_t id;
    char 			name[GW_OSAL_MAX_API_NAME];
    int 			creator;
}osal_mut_record_t;
#endif

/* queues */
#if 0
typedef struct {
    gw_int32 free;
    cyg_handle_t id;
    gw_int8 name [GW_OSAL_MAX_API_NAME];
    cyg_handle_t creator;
    cyg_mbox mbox;
    gw_uint32 mempool_id;
    gw_int32 depth;
    gw_int32 queue_size;
    gw_int32 queue_type;
    gw_int32    pri_num;
    gw_uint32   cnt_sem_id;
    gw_list         queue_list[GW_OSAL_MAX_QUEUE_PRI];
    gw_uint32 queue_mutex[GW_OSAL_MAX_QUEUE_PRI];
    gw_uint32   pri_queue_peek[GW_OSAL_MAX_QUEUE_PRI];

    gw_uint32 peek_value;
    gw_uint32 put_error;
    gw_uint32 put_full_error;
    gw_uint32 put_timeout_error;
    gw_uint32 put_data_error;
    gw_uint32 put_data_long;
    gw_uint32 get_error;
    gw_uint32 get_timeout_error;
    gw_uint32 get_data_error;
    gw_uint32 get_data_long;
}osal_queue_record_t;
#else
typedef struct {
    int 			free;
    mqd_t 			id;
    char 			name [GW_OSAL_MAX_API_NAME];
    int 			creator;
    unsigned int 	mempool_id;
    int				queue_size;
    int				depth;
    int				queue_type;
    int    			pri_num;
    unsigned int   	cnt_sem_id;
    gw_list     	queue_list[GW_OSAL_MAX_QUEUE_PRI];
    unsigned int 	queue_mutex[GW_OSAL_MAX_QUEUE_PRI];
    unsigned int	pri_queue_peek[GW_OSAL_MAX_QUEUE_PRI];
    unsigned int 	cur_num;
    unsigned int 	peek_value;
    unsigned int 	put_error;
    unsigned int 	put_full_error;
    unsigned int 	put_timeout_error;
    unsigned int 	put_data_error;
    unsigned int 	put_data_long;
    unsigned int 	get_error;
    unsigned int 	get_timeout_error;
    unsigned int 	get_data_error;
    unsigned int 	get_data_long;
}osal_queue_record_t;

#endif

/* Tables where the OS object information is stored */
osal_thread_record_t    gw_osal_thread_table      [GW_OSAL_MAX_THREAD];
osal_count_sem_record_t gw_osal_count_sem_table   [GW_OSAL_MAX_COUNT_SEM];
osal_mut_record_t       gw_osal_mut_table         [GW_OSAL_MAX_MUTEX];
osal_queue_record_t     gw_osal_queue_table       [GW_OSAL_MAX_QUEUE];

#ifdef CYG_LINUX
cyg_mutex_t gw_osal_task_table_mutex;
#else
pthread_mutex_t gw_osal_thread_table_mut;
#endif
#if 0
cyg_mutex_t osal_count_sem_table_mutex;
#else
pthread_mutex_t gw_osal_count_sem_table_mut;
#endif
#if 0
cyg_mutex_t osal_mut_table_mutex;
#else
pthread_mutex_t gw_osal_mut_table_mut;
#endif
#if 0
cyg_mutex_t osal_queue_table_mutex;
#else
pthread_mutex_t gw_os_queue_table_mut;
#endif
gw_int32 osal_debug=1;


/****************************************************************************************
                                INITIALIZATION FUNCTION
****************************************************************************************/

/*---------------------------------------------------------------------------------------
   Name: gw_osal_core_init

   Purpose: Initialize the tables that the OSAL API uses to keep track of information
            about objects

   returns: nothing
---------------------------------------------------------------------------------------*/
void gw_osal_core_init(void)
{
    gw_int32 i;

    /* Initialize Task Table */

	for (i = 0; i < GW_OSAL_MAX_THREAD; i++) {
        memset(&gw_osal_thread_table[i] , 0 , sizeof(osal_thread_record_t));
        gw_osal_thread_table[i].free = TRUE;
    }

    /* Initialize Counting Semaphore Table */
	for (i = 0; i < GW_OSAL_MAX_COUNT_SEM; i++) {
        memset(&gw_osal_count_sem_table[i] , 0 , sizeof(osal_count_sem_record_t));
        gw_osal_count_sem_table[i].free = TRUE;
    }

    /* Initialize Mutex Semaphore Table */
	for (i = 0; i < GW_OSAL_MAX_MUTEX; i++) {
        memset(&gw_osal_mut_table[i] , 0 , sizeof(osal_mut_record_t));
        gw_osal_mut_table[i].free = TRUE;
    }

    /* Initialize message queue Table */
	for (i = 0; i < GW_OSAL_MAX_QUEUE; i++) {
        memset(&gw_osal_queue_table[i] , 0 , sizeof(osal_queue_record_t));
        gw_osal_queue_table[i].free = TRUE;
    }

#ifdef CYG_LINUX
    cyg_mutex_init(&gw_osal_task_table_mutex);
	#else
	pthread_mutex_init((pthread_mutex_t *)&gw_osal_thread_table_mut, NULL);
	#endif
	#if 0
    cyg_mutex_init(&osal_count_sem_table_mutex);
	#else
	pthread_mutex_init((pthread_mutex_t *)&gw_osal_count_sem_table_mut, NULL);
	#endif
	#if 0
    cyg_mutex_init(&osal_mut_table_mutex);
	#else
	pthread_mutex_init((pthread_mutex_t *)&gw_osal_mut_table_mut, NULL);
	#endif
	#if 0
    cyg_mutex_init(&gw_os_queue_table_mutex);
	#else
	pthread_mutex_init((pthread_mutex_t *)&gw_os_queue_table_mut, NULL);
	#endif

    return;

}


/****************************************************************************************
                                    TASK API
****************************************************************************************/

/*---------------------------------------------------------------------------------------
   Name: gw_thread_create

   Purpose: Creates a task and starts running it.

   returns: GW_E_OSAL_INVALID_POINTER if any of the necessary pointers are NULL
            GW_E_OSAL_ERR_NAME_TOO_LONG if the name of the task is too long to be copied
            GW_E_OSAL_ERR_INVALID_PRIORITY if the priority is bad
            GW_E_OSAL_ERR_NO_FREE_IDS if there can be no more tasks created
            GW_E_OSAL_ERR_NAME_TAKEN if the name specified is already used by a task
            GW_E_OSAL_ERR if the operating system calls fail
            GW_E_OSAL_OK if success

    NOTES: task_id is passed back to the user as the ID. Flags are unused at this point.

    !!! the api gw_thread_exit must be called while exiting from the thread created by this api


---------------------------------------------------------------------------------------*/
#if 1
#if 0
gw_int32 gw_thread_create(gw_uint32 *thread_id,  const gw_int8 *thread_name,
                              const void *function_pointer, void *param , gw_uint32 stack_size,
                              gw_uint32 priority, gw_uint32 flags)
{
    gw_uint32 possible_taskid;
    gw_uint8 *stack_buf = NULL;

#ifndef CYG_LINUX
    pthread_t threadid;
    pthread_attr_t p_attr;
    struct sched_param attr_param;
#endif  

    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */

    /* Check for NULL pointers */

    if ((thread_name == NULL) || (function_pointer == NULL) || (thread_id == NULL)) {
        osal_printf("\r\n thread create failed , cause some parameter is NULL");
        return GW_E_OSAL_INVALID_POINTER;
    }

    if (strlen(thread_name) >= GW_OSAL_MAX_API_NAME) {
        osal_printf("\r\n thread name is too long");
        return GW_E_OSAL_ERR_NAME_TOO_LONG;
    }

    /* Check for bad priority */

    if (priority > GW_OSAL_MAX_PRI) {
        osal_printf("\r\n thread priority is out of range");
        return GW_E_OSAL_ERR_INVALID_PRIORITY;
    }

    /* Check Parameters */
//    stack_buf = (gw_uint8 *)iros_malloc(IROS_MID_OSAL , stack_size);
    stack_buf = (gw_uint8*)malloc(stack_size);
    if (stack_buf == NULL) {
        osal_printf("\r\n Allocate thread's stack space failed");
        return GW_E_OSAL_ERR;
    }

#ifdef CYG_LINUX
    cyg_mutex_lock(&gw_osal_task_table_mutex);
#else
    pthread_mutex_lock(&gw_osal_thread_table_mut);
#endif
    for (possible_taskid = 0; possible_taskid < GW_OSAL_MAX_THREAD; possible_taskid++) {
        if (gw_osal_thread_table[possible_taskid].free  == TRUE) {
            break;
        }
    }

    /* Check to see if the id is out of bounds */
    if (possible_taskid >= GW_OSAL_MAX_THREAD || gw_osal_thread_table[possible_taskid].free != TRUE) {
#ifdef CYG_LINUX
        cyg_mutex_unlock(&gw_osal_task_table_mutex);
#else
        pthread_mutex_unlock(&gw_osal_thread_table_mut);
#endif
//        iros_free(stack_buf);
        free(stack_buf);
        osal_printf("\r\n no free thread can be allocate");
        return GW_E_OSAL_ERR_NO_FREE_IDS;
    }



    gw_osal_thread_table[possible_taskid].free = FALSE;

#ifdef CYG_LINUX
    cyg_mutex_unlock(&gw_osal_task_table_mutex);
#else
    pthread_mutex_unlock(&gw_osal_thread_table_mut);
#endif
    /* Create VxWorks Task */

#ifdef CYG_LINUX
    cyg_thread_create(priority,
                      function_pointer,
                      (cyg_addrword_t)param,
                      (gw_int8*)thread_name,
                      stack_buf,
                      stack_size,
                      &gw_osal_thread_table[possible_taskid].id,
                      &gw_osal_thread_table[possible_taskid].thread_ctrl);
    cyg_thread_resume(gw_osal_thread_table[possible_taskid].id);
#else
    pthread_attr_init(&p_attr);

    pthread_attr_setstackaddr(&p_attr, stack_buf+stack_size);
    pthread_attr_setstacksize(&p_attr, stack_size);


	attr_param.sched_priority = (int)priority;	
	pthread_attr_setschedparam (&p_attr, &attr_param);
	
    pthread_create(&threadid, &p_attr, function_pointer, param);
    gw_osal_thread_table[possible_taskid].id = (gw_uint32)threadid;
#endif

    *thread_id = possible_taskid;

    strcpy(gw_osal_thread_table[*thread_id].name, thread_name);

    /* this Id no longer free */
#ifdef CYG_LINUX
    cyg_mutex_lock(&gw_osal_task_table_mutex);
#else
    pthread_mutex_lock(&gw_osal_thread_table_mut);
#endif
    gw_osal_thread_table[*thread_id].free = FALSE;

#ifdef CYG_LINUX
    gw_osal_thread_table[*thread_id].creator = (cyg_handle_t)gw_creator_find();
#else
    gw_osal_thread_table[*thread_id].creator = gw_creator_find();
#endif
    gw_osal_thread_table[*thread_id].stack_size = stack_size;
    gw_osal_thread_table[*thread_id].priority = priority;
    gw_osal_thread_table[*thread_id].stack_buf = stack_buf;

#ifdef CYG_LINUX
    cyg_mutex_unlock(&gw_osal_task_table_mutex);
#else
    pthread_mutex_unlock(&gw_osal_thread_table_mut);
#endif
    return GW_E_OSAL_OK;
}
#else
#ifndef CYG_LINUX
#if 0
gw_int32 Func_gwd_thread_info_show()
{
	int i_ret = 0;
	int i_count=0;

	for(i_count = 0; i_count < GW_OSAL_MAX_THREAD;i_count++)
	{
		if(gw_osal_thread_table[i_count].free == FALSE)
		{
			printf("%-16s %-15d %-9d %-9d %-9d %-9d\r\n", gw_osal_thread_table[i_count].name, gw_osal_thread_table[i_count].id,
					 	 	 	 	 	 	 	 	  gw_osal_thread_table[i_count].ppid, gw_osal_thread_table[i_count].pid,
					 	 	 	 	 	 	 	 	  gw_osal_thread_table[i_count].priority, gw_osal_thread_table[i_count].stack_size);
		}
	}
	return i_ret;
}
#endif
void Func_gwd_thread_name_set(char*threadname)
{
	if(threadname == NULL)
	{
		return;
	}
	prctl(PR_SET_NAME,(unsigned long )threadname,NULL,NULL,NULL);
}
#endif
static void* Func_gwd_thread_create(void* thread_info)
{
	osal_thread_record_t* st_threadinfo = thread_info;
	void (*func)(void*);
	void *param = NULL;

	if(thread_info == NULL)
	{
		return NULL;
	}
    st_threadinfo->id = pthread_self();
	pthread_detach(pthread_self());

	func = st_threadinfo->func;
	param = st_threadinfo->param;
	st_threadinfo->pid = getpid();
	st_threadinfo->ppid = getppid();
	st_threadinfo->creator = gw_creator_find();

	Func_gwd_thread_name_set(st_threadinfo->name);

	(*func)(param);
	gw_thread_delete(st_threadinfo->creator);

	return NULL;
}

gw_int32 gw_thread_create(gw_uint32 *thread_id,  const gw_int8 *thread_name,
                              const void *function_pointer, void *param , gw_uint32 stack_size,
                              gw_uint32 priority, gw_uint32 flags)
{
    gw_uint32 possible_taskid;
    gw_uint8 *stack_buf = NULL;

#ifndef CYG_LINUX
    pthread_t threadid;
    pthread_attr_t p_attr;
    struct sched_param attr_param;
#endif

    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */

    /* Check for NULL pointers */

    if ((thread_name == NULL) || (function_pointer == NULL) || (thread_id == NULL)) {
        osal_printf("\r\n thread create failed , cause some parameter is NULL");
        return GW_E_OSAL_INVALID_POINTER;
    }

    if (strlen(thread_name) >= GW_OSAL_MAX_API_NAME) {
        osal_printf("\r\n thread name is too long");
        return GW_E_OSAL_ERR_NAME_TOO_LONG;
    }

    /* Check for bad priority */

    if (priority > GW_OSAL_MAX_PRI) {
        osal_printf("\r\n thread priority is out of range");
        return GW_E_OSAL_ERR_INVALID_PRIORITY;
    }

    /* Check Parameters */
//    stack_buf = (gw_uint8 *)iros_malloc(IROS_MID_OSAL , stack_size);
    stack_buf = (gw_uint8*)malloc(stack_size);
    if (stack_buf == NULL) {
        osal_printf("\r\n Allocate thread's stack space failed");
        return GW_E_OSAL_ERR;
    }

#ifdef CYG_LINUX
    cyg_mutex_lock(&gw_osal_task_table_mutex);
#else
    pthread_mutex_lock(&gw_osal_thread_table_mut);
#endif
    for (possible_taskid = 0; possible_taskid < GW_OSAL_MAX_THREAD; possible_taskid++) {
        if (gw_osal_thread_table[possible_taskid].free  == TRUE) {
            break;
        }
    }

    /* Check to see if the id is out of bounds */
    if (possible_taskid >= GW_OSAL_MAX_THREAD || gw_osal_thread_table[possible_taskid].free != TRUE) {
#ifdef CYG_LINUX
        cyg_mutex_unlock(&gw_osal_task_table_mutex);
#else
        pthread_mutex_unlock(&gw_osal_thread_table_mut);
#endif
//        iros_free(stack_buf);
        free(stack_buf);
        osal_printf("\r\n no free thread can be allocate");
        return GW_E_OSAL_ERR_NO_FREE_IDS;
    }



    gw_osal_thread_table[possible_taskid].free = FALSE;
    gw_osal_thread_table[possible_taskid].func = function_pointer;
    gw_osal_thread_table[possible_taskid].stack_size = stack_size;
    gw_osal_thread_table[possible_taskid].priority = priority;
    gw_osal_thread_table[possible_taskid].stack_buf = stack_buf;
    gw_osal_thread_table[possible_taskid].param = param;
    strncpy(gw_osal_thread_table[possible_taskid].name, thread_name,GW_OSAL_MAX_API_NAME);

#ifdef CYG_LINUX
    cyg_mutex_unlock(&gw_osal_task_table_mutex);
#else
    pthread_mutex_unlock(&gw_osal_thread_table_mut);
#endif
    /* Create VxWorks Task */

#ifdef CYG_LINUX
    cyg_thread_create(priority,
                      function_pointer,
                      (cyg_addrword_t)param,
                      (gw_int8*)thread_name,
                      stack_buf,
                      stack_size,
                      &gw_osal_thread_table[possible_taskid].id,
                      &gw_osal_thread_table[possible_taskid].thread_ctrl);
    cyg_thread_resume(gw_osal_thread_table[possible_taskid].id);
#else
    pthread_attr_init(&p_attr);

    pthread_attr_setstackaddr(&p_attr, stack_buf+stack_size);
    pthread_attr_setstacksize(&p_attr, stack_size);


	attr_param.sched_priority = (int)priority;
	pthread_attr_setschedparam (&p_attr, &attr_param);

    pthread_create(&threadid, &p_attr, Func_gwd_thread_create,(void*)&gw_osal_thread_table[possible_taskid]);
#endif

    *thread_id = possible_taskid;

    /* this Id no longer free */
#ifdef CYG_LINUX
    cyg_mutex_lock(&gw_osal_task_table_mutex);
#endif

#ifdef CYG_LINUX
    gw_osal_thread_table[*thread_id].creator = (cyg_handle_t)gw_creator_find();
#endif

#ifdef CYG_LINUX
    cyg_mutex_unlock(&gw_osal_task_table_mutex);
#endif
    return GW_E_OSAL_OK;
}
#endif
gw_int32 gw_thread_delete(gw_uint32 thread_id)
{

#ifdef CYG_LINUX
    cyg_mutex_lock(&gw_osal_task_table_mutex);
#else
    pthread_mutex_lock(&gw_osal_thread_table_mut);
#endif


    if (thread_id >= GW_OSAL_MAX_THREAD || gw_osal_thread_table[thread_id].free != FALSE) {
#ifdef CYG_LINUX
    cyg_mutex_unlock(&gw_osal_task_table_mutex);
#else
    pthread_mutex_unlock(&gw_osal_thread_table_mut);
#endif
        return GW_E_OSAL_ERR_INVALID_ID;
    }

#ifdef CYG_LINUX
    cyg_thread_delete(gw_osal_thread_table[thread_id].id);
#else
    {
    	void * status = NULL;
    	pthread_cancel(gw_osal_thread_table[thread_id].id);
    	pthread_join(gw_osal_thread_table[thread_id].id, &status);
    }
#endif

    memset(gw_osal_thread_table[thread_id].name, 0, GW_OSAL_MAX_API_NAME);

    gw_osal_thread_table[thread_id].free = TRUE;
    gw_osal_thread_table[thread_id].creator = 0;
    gw_osal_thread_table[thread_id].stack_size = 0;
    gw_osal_thread_table[thread_id].priority = 0;
    gw_osal_thread_table[thread_id].func = NULL;
    gw_osal_thread_table[thread_id].param = NULL;
    gw_osal_thread_table[thread_id].pid = 0;
    gw_osal_thread_table[thread_id].ppid = 0;

    if(gw_osal_thread_table[thread_id].stack_buf)	//free stack buff allocated by creator
    {
    	free(gw_osal_thread_table[thread_id].stack_buf);
    	gw_osal_thread_table[thread_id].stack_buf = NULL;
    }

#ifdef CYG_LINUX
    cyg_mutex_unlock(&gw_osal_task_table_mutex);
#else
    pthread_mutex_unlock(&gw_osal_thread_table_mut);
#endif
	
    return GW_E_OSAL_OK;
}

gw_int32 gw_thread_delay(gw_uint32 milli_second)
{
#ifdef CYG_LINUX
    gw_uint32 sys_ticks;

    sys_ticks = gw_milli_to_ticks(milli_second);
    if(sys_ticks == 0)
        sys_ticks = 1;
    cyg_thread_delay(sys_ticks);
#else
    usleep(milli_second*1000);
#endif

    return GW_E_OSAL_OK;

}


gw_int32 gw_thread_exit()
{
    
	gw_uint32 thread_id = gw_creator_find();
#if OS_CYG_LINUX
    cyg_mutex_lock(&gw_osal_task_table_mutex);
#else
    pthread_mutex_lock(&gw_osal_thread_table_mut);
#endif

    if (thread_id >= GW_OSAL_MAX_THREAD || gw_osal_thread_table[thread_id].free != FALSE) {
#if OS_CYG_LINUX
    cyg_mutex_unlock(&gw_osal_task_table_mutex);
#else
    pthread_mutex_unlock(&gw_osal_thread_table_mut);
#endif
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    memset(gw_osal_thread_table[thread_id].name, 0, GW_OSAL_MAX_API_NAME);

    gw_osal_thread_table[thread_id].free = TRUE;
    gw_osal_thread_table[thread_id].creator = 0;
    gw_osal_thread_table[thread_id].stack_size = 0;
    gw_osal_thread_table[thread_id].priority = 0;

    if(gw_osal_thread_table[thread_id].stack_buf)	//free stack buff allocated by creator
    {
    	free(gw_osal_thread_table[thread_id].stack_buf);
    	gw_osal_thread_table[thread_id].stack_buf = NULL;
    }

#if OS_CYG_LINUX
    cyg_mutex_unlock(&gw_osal_task_table_mutex);
#else
    pthread_mutex_unlock(&gw_osal_thread_table_mut);
#endif

#if OS_CYG_LINUX
    cyg_thread_exit();
#else
    pthread_exit("done");
#endif

    return GW_E_OSAL_OK;
}



#ifdef CYG_LINUX

/*---------------------------------------------------------------------------------------
   Name: gw_thread_delay

   Purpose: Delay a task for specified amount of clock ticks have occurred

   returns: GW_E_OSAL_ERR if sleep fails
            GW_E_OSAL_OK if success

   Notes: VxWorks uses the system clock to handle task delays.  The system clock usually
            runs at 60Hz. This means that the resolution of the delay will be course.
            It rounds up.
---------------------------------------------------------------------------------------*/

gw_int32 gw_thread_delay(gw_uint32 milli_second)
{
    /* if successful, the execution of task will pend here until delay finishes */
    gw_uint32 sys_ticks;

    sys_ticks = gw_milli_to_ticks(milli_second);
    if(sys_ticks == 0)
        sys_ticks = 1;
    cyg_thread_delay(sys_ticks);

    return GW_E_OSAL_OK;

}



gw_uint32 gw_thread_number()
{
    gw_int32 i;
    gw_uint32 count=0;

    cyg_mutex_lock(&gw_osal_task_table_mutex);
    for(i = 0; i < GW_OSAL_MAX_THREAD; i++)
    {
        if(gw_osal_thread_table[i].free == FALSE)
            count++;
    }
    cyg_mutex_unlock(&gw_osal_task_table_mutex);

    return count;
}

typedef struct
{
    gw_node node;
    cyg_thread_info info;
}thread_display_t;

void gw_thread_show()
{
    cyg_handle_t thread=0;
    cyg_uint16 id=0;
    thread_display_t info[32];
    gw_uint32 thread_num = 0;
    gw_list display_list;
    thread_display_t *pNode = NULL;
    int addflag = 0;

    gw_lst_init(&display_list , NULL);
    memset(&info[0] , 0 , 32*sizeof(cyg_thread_info));
    while( cyg_thread_get_next(&thread, &id))
    {
        if(!cyg_thread_get_info(thread, id, &info[thread_num].info))
            continue;

        addflag = 0;
        gw_lst_scan(&display_list , pNode , thread_display_t *)
        {
            if(pNode->info.set_pri <= info[thread_num].info.set_pri)
                continue;
            gw_lst_insert(&display_list, (gw_node *)gw_lst_prev((gw_node *)pNode), (gw_node *)&info[thread_num]);
            addflag = 1;
            break;
        }

        if(!addflag)
            gw_lst_add(&display_list, (gw_node *)&info[thread_num]);
        thread_num++;
    }

    //gw_printf("\r\n==============================================================================");
    gw_printf("\r\n%-3s %-25s %-7s %-7s %-6s %-7s %-7s %-8s","ID","NAME","CUR_PRI","SET_PRI","SIZE",
                                                                            "CUR","MARGIN","STATE");
    //gw_printf("\r\n-----------------------------------------------------------------------------");
    gw_lst_scan(&display_list , pNode , thread_display_t *)
    {
        gw_printf("\r\n%-3d %-25s %-7d %-7d %-6d %-7d %-7d %s",pNode->info.id,
            pNode->info.name,pNode->info.cur_pri,pNode->info.set_pri,
            pNode->info.stack_size,pNode->info.stack_used,
            pNode->info.stack_size-pNode->info.stack_used,(pNode->info.state == 0)?"RUNNING":
                    (pNode->info.state == 1)?"SLEEPING":
                    (pNode->info.state == 2)?"COUNTSLEEP":
                    (pNode->info.state == 4)?"SUSPENDED":
                    (pNode->info.state == 8)?"CREATING":
                    (pNode->info.state == 16)?"EXITED":
                    (pNode->info.state == 3)?"SLEEPSET":"N/A");
        gw_thread_delay(1);
    }
    //gw_printf("\r\n==============================================================================\r\n");

    return;
}

#endif
#else
int gw_thread_create(unsigned int *thread_id,  const char *thread_name,
                     const void *function_pointer, void *param , unsigned int stack_size,
                     unsigned int priority, unsigned int flags)
{
    int					return_code = 0;
    pthread_attr_t		custom_attr;
    struct sched_param	priority_holder;
    int					possible_taskid;
    unsigned int		local_stack_size;
    int					i;

    *thread_id = GW_OSAL_MAX_THREAD;
    if ((thread_name == NULL) || (function_pointer == NULL) || (thread_id == NULL)) {
        gw_printf("\r\n thread create failed , cause some parameter is NULL");
        return GW_E_OSAL_INVALID_POINTER;
    }

    if (strlen(thread_name) > GW_OSAL_MAX_API_NAME) {
        gw_printf("\r\n thread name is too long");
        return GW_E_OSAL_ERR_NAME_TOO_LONG;
    }

    if (priority > GW_OSAL_MAX_PRI) {
        gw_printf("\r\n thread priority is out of range");
        return GW_E_OSAL_ERR_INVALID_PRIORITY;
    }

    pthread_mutex_lock(&gw_osal_thread_table_mut);
    for (possible_taskid = 0; possible_taskid < GW_OSAL_MAX_THREAD; possible_taskid++) {
        if (gw_osal_thread_table[possible_taskid].free == TRUE) {
            break;
        }
    }

    if (possible_taskid >= GW_OSAL_MAX_THREAD) {
        gw_printf("\r\n no free thread can be allocate");
        pthread_mutex_unlock(&gw_osal_thread_table_mut);
        return GW_E_OSAL_ERR_NO_FREE_IDS;
    }

    for (i = 0; i < GW_OSAL_MAX_THREAD; i++) {
        if ((gw_osal_thread_table[i].free == FALSE) &&
                (strcmp((char*) thread_name, gw_osal_thread_table[i].name) == 0)) {
            pthread_mutex_unlock(&gw_osal_thread_table_mut);
            return GW_E_OSAL_ERR_NAME_TAKEN;
        }
    }

    gw_osal_thread_table[possible_taskid].free = FALSE;
    pthread_mutex_unlock(&gw_osal_thread_table_mut);

    if (stack_size < PTHREAD_STACK_MIN)
        local_stack_size = PTHREAD_STACK_MIN;
    else
        local_stack_size = stack_size;

    pthread_attr_init(&custom_attr);
    pthread_attr_setinheritsched(&custom_attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setstacksize(&custom_attr, (size_t)local_stack_size);
    priority_holder.sched_priority = GW_OSAL_MAX_PRI - priority;
    pthread_attr_setschedparam(&custom_attr, &priority_holder);
    return_code = pthread_create(&(gw_osal_thread_table[possible_taskid].id),
                                 &custom_attr,
                                 function_pointer,
                                 (void*)param);
    if (return_code != 0) {
        pthread_mutex_lock(&gw_osal_thread_table_mut);
        memset(&gw_osal_thread_table[possible_taskid] , 0 , sizeof(osal_thread_record_t));
        gw_osal_thread_table[possible_taskid].free = TRUE;
        pthread_mutex_unlock(&gw_osal_thread_table_mut);
		gw_printf("\r\n Create thread %s failed.",thread_name);
        return(GW_E_OSAL_ERR);
    }

    *thread_id = possible_taskid;
    pthread_mutex_lock(&gw_osal_thread_table_mut);
    strcpy(gw_osal_thread_table[*thread_id].name, (char*) thread_name);
    gw_osal_thread_table[possible_taskid].stack_size = stack_size;
    gw_osal_thread_table[possible_taskid].priority = GW_OSAL_MAX_PRI - priority;
    pthread_mutex_unlock(&gw_osal_thread_table_mut);

    return GW_E_OSAL_OK;
}

int gw_thread_delay(unsigned int milli_second)
{
    if (milli_second < 10)
        milli_second = 10;
	usleep(milli_second * 1000);
	return GW_E_OSAL_OK;
}

gw_uint32 gw_thread_number()
{
    gw_int32 i;
    gw_uint32 count=0;

    pthread_mutex_lock(&gw_osal_thread_table_mut);
    for(i = 0; i < GW_OSAL_MAX_THREAD; i++)
    {
        if(gw_osal_thread_table[i].free == FALSE)
            count++;
    }
    pthread_mutex_unlock(&gw_osal_thread_table_mut);

    return count;
}

typedef struct
{
    gw_node node;
    cyg_thread_info info;
}thread_display_t;

void gw_thread_show()
{
    cyg_handle_t thread=0;
    cyg_uint16 id=0;
    thread_display_t info[32];
    gw_uint32 thread_num = 0;
    gw_list display_list;
    thread_display_t *pNode = NULL;
    int addflag = 0;

    gw_lst_init(&display_list , NULL);
    memset(&info[0] , 0 , 32*sizeof(cyg_thread_info));
    while( cyg_thread_get_next(&thread, &id))
    {
        if(!cyg_thread_get_info(thread, id, &info[thread_num].info))
            continue;

        addflag = 0;
        gw_lst_scan(&display_list , pNode , thread_display_t *)
        {
            if(pNode->info.set_pri <= info[thread_num].info.set_pri)
                continue;
            gw_lst_insert(&display_list, (gw_node *)gw_lst_prev((gw_node *)pNode), (gw_node *)&info[thread_num]);
            addflag = 1;
            break;
        }

        if(!addflag)
            gw_lst_add(&display_list, (gw_node *)&info[thread_num]);
        thread_num++;
    }

    //gw_printf("\r\n==============================================================================");
    gw_printf("\r\n%-3s %-25s %-7s %-7s %-6s %-7s %-7s %-8s","ID","NAME","CUR_PRI","SET_PRI","SIZE",
                                                                            "CUR","MARGIN","STATE");
    //gw_printf("\r\n-----------------------------------------------------------------------------");
    gw_lst_scan(&display_list , pNode , thread_display_t *)
    {
        gw_printf("\r\n%-3d %-25s %-7d %-7d %-6d %-7d %-7d %s",pNode->info.id,
            pNode->info.name,pNode->info.cur_pri,pNode->info.set_pri,
            pNode->info.stack_size,pNode->info.stack_used,
            pNode->info.stack_size-pNode->info.stack_used,(pNode->info.state == 0)?"RUNNING":
                    (pNode->info.state == 1)?"SLEEPING":
                    (pNode->info.state == 2)?"COUNTSLEEP":
                    (pNode->info.state == 4)?"SUSPENDED":
                    (pNode->info.state == 8)?"CREATING":
                    (pNode->info.state == 16)?"EXITED":
                    (pNode->info.state == 3)?"SLEEPSET":"N/A");
        gw_thread_delay(1);
    }
    //gw_printf("\r\n==============================================================================\r\n");

    return;
}

#endif

/*---------------------------------------------------------------------------------------
   Name: gw_semaphore_init

   Purpose: Creates a counting semaphore with initial value specified by
            sem_initial_value and name specified by sem_name. sem_id will be
            returned to the caller

   Returns: GW_E_OSAL_INVALID_POINTER if sen name or sem_id are NULL
            GW_E_OSAL_ERR_NAME_TOO_LONG if the name given is too long
            GW_E_OSAL_ERR_NO_FREE_IDS if all of the semaphore ids are taken
            GW_E_OSAL_ERR_NAME_TAKEN if this is already the name of a counting semaphore
            GW_E_OSAL_SEM_FAILURE if the OS call failed
            GW_E_OSAL_OK if success


   Notes: options is an unused parameter
---------------------------------------------------------------------------------------*/
#if 0
gw_int32 gw_semaphore_init(gw_uint32 *sem_id, const gw_int8 *sem_name, gw_uint32 sem_initial_value, gw_uint32 options)
{
    /* the current candidate for the new sem id */
    gw_uint32 possible_semid;

    if (sem_id == NULL || sem_name == NULL) {
        osal_printf("\r\n semaphore create failed cause some NULL parameter");
        return GW_E_OSAL_INVALID_POINTER;
    }

    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */

    if (strlen(sem_name) >= GW_OSAL_MAX_API_NAME) {
        osal_printf("\r\n semaphore name is too long");
        return GW_E_OSAL_ERR_NAME_TOO_LONG;
    }

    /* Check Parameters */
    cyg_mutex_lock(&osal_count_sem_table_mutex);

    for (possible_semid = 0; possible_semid < GW_OSAL_MAX_COUNT_SEM; possible_semid++) {
        if (gw_osal_count_sem_table[possible_semid].free == TRUE)
            break;
    }

    if ((possible_semid >= GW_OSAL_MAX_COUNT_SEM) ||
            (gw_osal_count_sem_table[possible_semid].free != TRUE)) {
        cyg_mutex_unlock(&osal_count_sem_table_mutex);
        osal_printf("\r\n no free semaphore slot");
        return GW_E_OSAL_ERR_NO_FREE_IDS;
    }

#if 0 /*Remove it for new requirement 2009-12-28*/
    /* Check to see if the name is already taken */
    for (i = 0; i < GW_OSAL_MAX_COUNT_SEM; i++) {
        if ((gw_osal_count_sem_table[i].free == FALSE) &&
                strcmp((gw_int8*)sem_name, gw_osal_count_sem_table[i].name) == 0) {
            cyg_mutex_unlock(&osal_count_sem_table_mutex);
            return GW_E_OSAL_ERR_NAME_TAKEN;
        }
    }
#endif

    gw_osal_count_sem_table[possible_semid].free = FALSE;
    cyg_mutex_unlock(&osal_count_sem_table_mutex);

    /* Create VxWorks Semaphore */
    cyg_semaphore_init(&gw_osal_count_sem_table[possible_semid].id,
                       sem_initial_value);

    /* Set the sem_id to the one that we found available */
    /* Set the name of the semaphore,creator and free as well */

    *sem_id = possible_semid;

    cyg_mutex_lock(&osal_count_sem_table_mutex);
    gw_osal_count_sem_table[*sem_id].free = FALSE;
    strcpy(gw_osal_count_sem_table[*sem_id].name , (gw_int8*) sem_name);
    gw_osal_count_sem_table[*sem_id].creator = gw_creator_find();
    cyg_mutex_unlock(&osal_count_sem_table_mutex);


    return GW_E_OSAL_OK;

}


/*--------------------------------------------------------------------------------------
     Name: gw_semaphore_destroy

    Purpose: Deletes the specified Counting Semaphore.

    Returns: GW_E_OSAL_ERR_INVALID_ID if the id passed in is not a valid counting semaphore
             GW_E_OSAL_ERR_SEM_NOT_FULL if the semahore is taken and cannot be deleted
             GW_E_OSAL_SEM_FAILURE the OS call failed
             GW_E_OSAL_OK if success

    Notes: Since we can't delete a semaphore which is currently locked by some task
           (as it may ber crucial to completing the task), the semaphore must be full to
           allow deletion.
---------------------------------------------------------------------------------------*/

gw_int32 gw_semaphore_destroy(gw_uint32 sem_id)
{
    /* Check to see if this sem_id is valid */
    if (sem_id >= GW_OSAL_MAX_COUNT_SEM || gw_osal_count_sem_table[sem_id].free == TRUE) {
        osal_printf("\r\n can not destroy an invalid semaphore");
        return GW_E_OSAL_ERR_INVALID_ID;
    }


    cyg_semaphore_destroy(&gw_osal_count_sem_table[sem_id].id);

    /* Remove the Id from the table, and its name, so that it cannot be found again */
    cyg_mutex_lock(&osal_count_sem_table_mutex);
    gw_osal_count_sem_table[sem_id].free = TRUE;
    strcpy(gw_osal_count_sem_table[sem_id].name , "");
    gw_osal_count_sem_table[sem_id].creator = GW_OSAL_UNINIT;
    memset(&gw_osal_count_sem_table[sem_id].id , 0 , sizeof(cyg_sem_t));
    cyg_mutex_unlock(&osal_count_sem_table_mutex);

    return GW_E_OSAL_OK;

}


/*---------------------------------------------------------------------------------------
    Name: gw_semaphore_post

    Purpose: The function  unlocks the semaphore referenced by sem_id by performing
             a semaphore unlock operation on that semaphore.If the semaphore value
             resulting from this operation is positive, then no threads were blocked
             waiting for the semaphore to become unlocked; the semaphore value is
             simply incremented for this semaphore.


    Returns: GW_E_OSAL_SEM_FAILURE the semaphore was not previously  initialized or is not
             in the array of semaphores defined by the system
             GW_E_OSAL_ERR_INVALID_ID if the id passed in is not a counting semaphore
             GW_E_OSAL_OK if success

---------------------------------------------------------------------------------------*/
gw_int32 gw_semaphore_post(gw_uint32 sem_id)
{
    /* Check Parameters */

    if (sem_id >= GW_OSAL_MAX_COUNT_SEM || gw_osal_count_sem_table[sem_id].free == TRUE) {
        osal_printf("\r\n can not post an invalid semaphore");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    /* Give VxWorks Semaphore */
    cyg_semaphore_post(&gw_osal_count_sem_table[sem_id].id);
    return GW_E_OSAL_OK;

}

/*---------------------------------------------------------------------------------------
    Name:    gw_semaphore_wait

    Purpose: The locks the semaphore referenced by sem_id by performing a
             semaphore lock operation on that semaphore.If the semaphore value
             is currently zero, then the calling thread may have 3 chocies:
             1) shall not return from the call until it either locks the semaphore or the call is
             interrupted by a signal.
             2) return GW_E_OSAL_SEM_FAILURE immediately
             3) wait until timeout

    Return:  GW_E_OSAL_SEM_FAILURE : the semaphore was not previously initialized
             or is not in the array of semaphores defined by the system
             GW_E_OSAL_ERR_INVALID_ID the Id passed in is not a valid countar semaphore
             GW_E_OSAL_SEM_FAILURE if the OS call failed
             GW_E_OSAL_OK if success

    Notes:   A timeout in ticks should be specified.
             Timeouts of WAIT_FOREVER (-1) and NO_WAIT (0) indicate to wait indefinitely or not to wait at all.

----------------------------------------------------------------------------------------*/

gw_int32 gw_semaphore_wait(gw_uint32 sem_id, gw_int32 timeout)
{
    /* msecs rounded to the closest system tick count */
    gw_int32 sys_ticks;

    /* Check Parameters */
    if (sem_id >= GW_OSAL_MAX_COUNT_SEM  || gw_osal_count_sem_table[sem_id].free == TRUE) {
        osal_printf("\r\n wait an invalid semaphore");
        return GW_E_OSAL_ERR_INVALID_ID;
    }
    /* Give VxWorks Semaphore */
    if (GW_OSAL_NO_WAIT == timeout) {
        if (!cyg_semaphore_trywait(&gw_osal_count_sem_table[sem_id].id)) {
            return GW_E_OSAL_ERR;
        }
    } else if (GW_OSAL_WAIT_FOREVER == timeout) {
        cyg_semaphore_wait(&gw_osal_count_sem_table[sem_id].id);
    } else {
        sys_ticks = gw_milli_to_ticks(timeout);

        if (!cyg_semaphore_timed_wait(&gw_osal_count_sem_table[sem_id].id , sys_ticks)) {
            return GW_E_OSAL_ERR;
        }
    }

    return GW_E_OSAL_OK;
}

gw_uint32 gw_semaphore_number()
{
    gw_int32 i;
    gw_uint32 count=0;

    cyg_mutex_lock(&osal_count_sem_table_mutex);
    for(i = 0; i < GW_OSAL_MAX_COUNT_SEM; i++)
    {
        if(gw_osal_count_sem_table[i].free == FALSE)
            count++;
    }
    cyg_mutex_unlock(&osal_count_sem_table_mutex);

    return count;
}
#else
int gw_semaphore_init
	(
    unsigned int 	*sem_id,
    const char 		*sem_name,
    unsigned int 	sem_initial_value,
    unsigned int 	options
	)
{
    unsigned int possible_semid;
    int Status;

    if (sem_id == NULL || sem_name == NULL) {
        gw_printf("\r\n semaphore create failed cause some NULL parameter");
        return GW_E_OSAL_INVALID_POINTER;
    }

    if (strlen(sem_name) > GW_OSAL_MAX_API_NAME) {
        gw_printf("\r\n semaphore name is too long");
        return GW_E_OSAL_ERR_NAME_TOO_LONG;
    }

    pthread_mutex_lock(&gw_osal_count_sem_table_mut);
    for (possible_semid = 0; possible_semid < GW_OSAL_MAX_COUNT_SEM; possible_semid++) {
        if (gw_osal_count_sem_table[possible_semid].free == TRUE)
            break;
    }
    if (possible_semid >= GW_OSAL_MAX_COUNT_SEM) {
        gw_printf("\r\n no free semaphore slot");
		diag_printf("%s %d sem_conut error\n",__func__,__LINE__);
        pthread_mutex_unlock(&gw_osal_count_sem_table_mut);
        return GW_E_OSAL_ERR_NO_FREE_IDS;
    }

    gw_osal_count_sem_table[possible_semid].free = FALSE;
    pthread_mutex_unlock(&gw_osal_count_sem_table_mut);

    Status = sem_init(&(gw_osal_count_sem_table[possible_semid].id) , 0 , sem_initial_value);
    if (Status == -1) {
        pthread_mutex_lock(&gw_osal_count_sem_table_mut);
        memset(&gw_osal_count_sem_table[possible_semid] , 0 , sizeof(osal_count_sem_record_t));
        gw_osal_count_sem_table[possible_semid].free = TRUE;
        pthread_mutex_unlock(&gw_osal_count_sem_table_mut);
		diag_printf("%s %d sem_init error\n",__func__,__LINE__);
        return GW_E_OSAL_ERR;
    }

    *sem_id = possible_semid;
    pthread_mutex_lock(&gw_osal_count_sem_table_mut);
    gw_osal_count_sem_table[*sem_id].free = FALSE;
    strcpy(gw_osal_count_sem_table[*sem_id].name , (char*)sem_name);
    pthread_mutex_unlock(&gw_osal_count_sem_table_mut);

    return GW_E_OSAL_OK;
}

int gw_semaphore_destroy
	(
    unsigned int sem_id
	)
{
    if (sem_id >= GW_OSAL_MAX_COUNT_SEM || gw_osal_count_sem_table[sem_id].free == TRUE) {
        gw_printf("\r\n can not destroy an invalid semaphore");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    if (sem_destroy(&(gw_osal_count_sem_table[sem_id].id)) != 0) {
        gw_printf("\r\n Destroy semaphore(%d) failed", gw_osal_count_sem_table[sem_id].id);
        return GW_E_OSAL_SEM_FAILURE;
    }

    pthread_mutex_lock(&gw_osal_count_sem_table_mut);
    memset(&gw_osal_count_sem_table[sem_id] , 0 , sizeof(osal_count_sem_record_t));
    gw_osal_count_sem_table[sem_id].free = TRUE;
    pthread_mutex_unlock(&gw_osal_count_sem_table_mut);

    return GW_E_OSAL_OK;
}

int gw_semaphore_post
	(
    unsigned int sem_id
	)
{
    int ret_val ;
    int    ret;

    if (sem_id >= GW_OSAL_MAX_COUNT_SEM || gw_osal_count_sem_table[sem_id].free == TRUE) {
        gw_printf("\r\n(sem_id %d ) can not post an invalid semaphore",sem_id);
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    ret = sem_post(&(gw_osal_count_sem_table[sem_id].id));
    if (ret != 0) {
        ret_val = GW_E_OSAL_SEM_FAILURE;
    } else {
        ret_val = GW_E_OSAL_OK;
    }

    return ret_val;
}

int gw_semaphore_wait(unsigned int sem_id, int timeout)
{
    int ret_val ;
    int    ret;
    int timeloop;

    if (sem_id >= GW_OSAL_MAX_COUNT_SEM  || gw_osal_count_sem_table[sem_id].free == TRUE) {
        gw_printf("\r\n(sen_id %d) wait an invalid semaphore",sem_id);
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    if (GW_OSAL_NO_WAIT == timeout) {
        ret = sem_trywait(&(gw_osal_count_sem_table[sem_id].id));
    } else if (GW_OSAL_WAIT_FOREVER == timeout) {
        ret = sem_wait(&(gw_osal_count_sem_table[sem_id].id));
    } else {
        for (timeloop = timeout; timeloop > 0; timeloop -= 100) {
            if (timeloop >= 100) {
                if (sem_trywait(&(gw_osal_count_sem_table[sem_id].id)) == -1)
                    usleep(100*1000);
                else
                    return GW_E_OSAL_OK;
            } else {
                if (sem_trywait(&(gw_osal_count_sem_table[sem_id].id)) == -1)
                    return GW_E_OSAL_SEM_TIMEOUT;
                else
                    return GW_E_OSAL_OK;
            }
        }
        return GW_E_OSAL_SEM_TIMEOUT;
    }

    if (ret == 0) {
        ret_val = GW_E_OSAL_OK;
    } else {
        ret_val = GW_E_OSAL_ERR;
    }

    return ret_val;
}

unsigned int gw_semaphore_number()
{
    int i;
    unsigned int count = 0;

    pthread_mutex_lock(&gw_osal_count_sem_table_mut);
    for (i = 0; i < GW_OSAL_MAX_COUNT_SEM; i++) {
        if (gw_osal_count_sem_table[i].free == FALSE)
            count++;
    }
    pthread_mutex_unlock(&gw_osal_count_sem_table_mut);

    return count;
}

#endif


/****************************************************************************************
                                  MUTEX API
****************************************************************************************/

/*---------------------------------------------------------------------------------------
    Name: gw_mutex_init

    Purpose: Creates a mutex semaphore initially full.

    Returns: GW_E_OSAL_INVALID_POINTER if sem_id or sem_name are NULL
             GW_E_OSAL_ERR_NAME_TOO_LONG if the sem_name is too long to be stored
             GW_E_OSAL_ERR_NO_FREE_IDS if there are no more free mutex Ids
             GW_E_OSAL_ERR_NAME_TAKEN if there is already a mutex with the same name
             GW_E_OSAL_SEM_FAILURE if the OS call failed
             GW_E_OSAL_OK if success

    Notes: the options parameter is not used in this implementation

---------------------------------------------------------------------------------------*/
#if 0
gw_int32 gw_mutex_init(gw_uint32 *mut_id, const gw_int8 *mut_name, gw_uint32 options)
{
    gw_uint32 possible_semid;

    /* Check Parameters */

    if (mut_id == NULL || mut_name == NULL) {
        osal_printf("\r\n Create mutex failed cause some parameter is NULL");
        return GW_E_OSAL_INVALID_POINTER;
    }

    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */

    if (strlen(mut_name) >= GW_OSAL_MAX_API_NAME) {
        osal_printf("\r\n Mutex name is too long");
        return GW_E_OSAL_ERR_NAME_TOO_LONG;
    }

    cyg_mutex_lock(&osal_mut_table_mutex);
    for (possible_semid = 0; possible_semid < GW_OSAL_MAX_MUTEX; possible_semid++) {
        if (gw_osal_mut_table[possible_semid].free == TRUE)
            break;
    }

    if ((possible_semid >= GW_OSAL_MAX_MUTEX) ||
            (gw_osal_mut_table[possible_semid].free != TRUE)) {
        osal_printf("\r\n no free mutex slot");
        cyg_mutex_unlock(&osal_mut_table_mutex);
        return GW_E_OSAL_ERR_NO_FREE_IDS;
    }

    gw_osal_mut_table[possible_semid].free = FALSE;
    cyg_mutex_unlock(&osal_mut_table_mutex);

    /* Create ECOS mutex */
    cyg_mutex_init(&gw_osal_mut_table[possible_semid].id);

    *mut_id = possible_semid;
    cyg_mutex_lock(&osal_mut_table_mutex);
    strcpy(gw_osal_mut_table[*mut_id].name, (gw_int8*)mut_name);
    gw_osal_mut_table[*mut_id].free = FALSE;
    gw_osal_mut_table[*mut_id].creator = gw_creator_find();
    cyg_mutex_unlock(&osal_mut_table_mutex);

    return GW_E_OSAL_OK;
}


/*--------------------------------------------------------------------------------------
     Name: gw_mutex_destroy

    Purpose: Deletes the specified Mutex Semaphore.

    Returns: GW_E_OSAL_ERR_INVALID_ID if the id passed in is not a valid mutex
             GW_E_OSAL_ERR_SEM_NOT_FULL if the mutex is empty
             GW_E_OSAL_SEM_FAILURE if the OS call failed
             GW_E_OSAL_OK if success

    Notes: The mutex must be full to take it, so we have to check for fullness

---------------------------------------------------------------------------------------*/

gw_int32 gw_mutex_destroy(gw_uint32 mut_id)
{
    /* Check to see if this mut_id is valid   */
    if (mut_id >= GW_OSAL_MAX_MUTEX || gw_osal_mut_table[mut_id].free == TRUE) {
        osal_printf("\r\n can not destroy an invalid mutex");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    cyg_mutex_destroy(&gw_osal_mut_table[mut_id].id);
    /* Delete its presence in the table */

    cyg_mutex_lock(&osal_mut_table_mutex);
    gw_osal_mut_table[mut_id].free = TRUE;
    memset(&gw_osal_mut_table[mut_id].id , 0 , sizeof(cyg_mutex_t));
    strcpy(gw_osal_mut_table[mut_id].name , "");
    gw_osal_mut_table[mut_id].creator = GW_OSAL_UNINIT;
    cyg_mutex_unlock(&osal_mut_table_mutex);


    return GW_E_OSAL_OK;

}


/*---------------------------------------------------------------------------------------
    Name: gw_mutex_unlock

    Purpose: The function releases the mutex object referenced by sem_id.The
             manner in which a mutex is released is dependent upon the mutex's type
             attribute.  If there are threads blocked on the mutex object referenced by
             mutex when this function is called, resulting in the mutex becoming
             available, the scheduling policy shall determine which thread shall
             acquire the mutex.

    Returns: GW_E_OSAL_OK if success
             GW_E_OSAL_SEM_FAILURE if the semaphore was not previously  initialized
             GW_E_OSAL_ERR_INVALID_ID if the id passed in is not a valid mutex

---------------------------------------------------------------------------------------*/

gw_int32 gw_mutex_unlock(gw_uint32 mut_id)
{
    /* Check Parameters */

    if (mut_id >= GW_OSAL_MAX_MUTEX || gw_osal_mut_table[mut_id].free == TRUE) {
        osal_printf("\r\n unlock an invalid mutex");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    /* Give VxWorks Semaphore */
    cyg_mutex_unlock(&gw_osal_mut_table[mut_id].id);
    return GW_E_OSAL_OK;

}

/*---------------------------------------------------------------------------------------
    Name: gw_mutex_lock

    Purpose: The mutex object referenced by mut_id shall be locked by calling this
             function. If the mutex is already locked, the calling thread shall
             block until the mutex becomes available. This operation shall return
             with the mutex object referenced by mutex in the locked state with the
             calling thread as its owner.

    Returns: GW_E_OSAL_OK if success
             GW_E_OSAL_SEM_FAILURE if the semaphore was not previously initialized or is
             not in the array of semaphores defined by the system
             GW_E_OSAL_ERR_INVALID_ID the id passed in is not a valid mutex
---------------------------------------------------------------------------------------*/
gw_int32 gw_mutex_lock(gw_uint32 mut_id)
{
    /* Check Parameters */

    if (mut_id >= GW_OSAL_MAX_MUTEX || gw_osal_mut_table[mut_id].free == TRUE) {
        osal_printf("\r\n lock an invalid mutex");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    /* Take VxWorks Semaphore */
    cyg_mutex_lock(&gw_osal_mut_table[mut_id].id);

    return GW_E_OSAL_OK;

}


/*---------------------------------------------------------------------------------------
    Name: gw_mutex_trylock

    Purpose: The mutex object referenced by mut_id shall be locked by calling this
             function. If the mutex is already locked, the calling thread will
             always return immediately rather than block, again returning success
             or failure.

    Returns: GW_E_OSAL_OK if success
             GW_E_OSAL_SEM_FAILURE if the semaphore was not previously initialized or is
             not in the array of semaphores defined by the system
             GW_E_OSAL_ERR_INVALID_ID the id passed in is not a valid mutex
---------------------------------------------------------------------------------------*/
gw_int32 gw_mutex_trylock(gw_uint32 mut_id)
{
    /* Check Parameters */

    if (mut_id >= GW_OSAL_MAX_MUTEX || gw_osal_mut_table[mut_id].free == TRUE) {
        osal_printf("\r\n trylock an invalid mutex");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    /* Take VxWorks Semaphore */
    if (!cyg_mutex_trylock(&gw_osal_mut_table[mut_id].id))
        return GW_E_OSAL_ERR;

    return GW_E_OSAL_OK;
}

gw_uint32 gw_mutex_number()
{
    gw_int32 i;
    gw_uint32 count=0;

    cyg_mutex_lock(&osal_mut_table_mutex);
    for(i = 0; i < GW_OSAL_MAX_MUTEX; i++)
    {
        if(gw_osal_mut_table[i].free == FALSE)
            count++;
    }
    cyg_mutex_unlock(&osal_mut_table_mutex);

    return count;
}
#else
int gw_mutex_init(unsigned int *mut_id, const char *mut_name, unsigned int options)
{
    int                 return_code;
    pthread_mutexattr_t mutex_attr ;
    unsigned int        possible_semid;
    int 				i;

    if (mut_id == NULL || mut_name == NULL) {
        gw_printf("\r\n Create mutex failed cause some parameter is NULL");
        return GW_E_OSAL_INVALID_POINTER;
    }

    if (strlen(mut_name) > GW_OSAL_MAX_API_NAME) {
        gw_printf("\r\n Mutex name is too long");
        return GW_E_OSAL_ERR_NAME_TOO_LONG;
    }

    pthread_mutex_lock(&gw_osal_mut_table_mut);
    for (possible_semid = 0; possible_semid < GW_OSAL_MAX_MUTEX; possible_semid++) {
        if (gw_osal_mut_table[possible_semid].free == TRUE)
            break;
    }

    if (possible_semid >= GW_OSAL_MAX_MUTEX) {
        gw_printf("\r\n no free mutex slot");
        pthread_mutex_unlock(&gw_osal_mut_table_mut);
        return GW_E_OSAL_ERR_NO_FREE_IDS;
    }

    for (i = 0; i < GW_OSAL_MAX_MUTEX; i++) {
        if ((gw_osal_mut_table[i].free == FALSE) &&
                strcmp((char*) mut_name, gw_osal_mut_table[i].name) == 0) {
            pthread_mutex_unlock(&gw_osal_mut_table_mut);
            return GW_E_OSAL_ERR_NAME_TAKEN;
        }
    }

    gw_osal_mut_table[possible_semid].free = FALSE;
    pthread_mutex_unlock(&gw_osal_mut_table_mut);

    pthread_mutexattr_init(&mutex_attr);
    return_code = pthread_mutex_init((pthread_mutex_t *) & gw_osal_mut_table[possible_semid].id, &mutex_attr);
    if (return_code != 0) {
        pthread_mutex_lock(&gw_osal_mut_table_mut);
        memset(&gw_osal_mut_table[possible_semid] , 0 , sizeof(osal_mut_record_t));
        gw_osal_mut_table[possible_semid].free = TRUE;
        pthread_mutex_unlock(&gw_osal_mut_table_mut);
        return GW_E_OSAL_ERR;
    } else {
        *mut_id = possible_semid;
        pthread_mutex_lock(&gw_osal_mut_table_mut);
        strcpy(gw_osal_mut_table[*mut_id].name, (char*) mut_name);
        gw_osal_mut_table[*mut_id].free = FALSE;
        pthread_mutex_unlock(&gw_osal_mut_table_mut);
        return GW_E_OSAL_OK;
    }
}

int gw_mutex_destroy(unsigned int mut_id)
{
    int status = -1;

    if (mut_id >= GW_OSAL_MAX_MUTEX || gw_osal_mut_table[mut_id].free == TRUE) {
        gw_printf("\r\n can not destroy an invalid mutex");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    status = pthread_mutex_destroy(&(gw_osal_mut_table[mut_id].id));
    if (status != 0) {
        gw_printf("\r\n delete mutex failed");
        return GW_E_OSAL_SEM_FAILURE;
    }

    pthread_mutex_lock(&gw_osal_mut_table_mut);
    memset(&gw_osal_mut_table[mut_id] , 0 , sizeof(osal_mut_record_t));
    gw_osal_mut_table[mut_id].free = TRUE;
    pthread_mutex_unlock(&gw_osal_mut_table_mut);

    return GW_E_OSAL_OK;
}

int gw_mutex_unlock(unsigned int mut_id)
{
    int ret_val ;

    if (mut_id >= GW_OSAL_MAX_MUTEX || gw_osal_mut_table[mut_id].free == TRUE) {
        gw_printf("\r\n unlock an invalid mutex");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    if (pthread_mutex_unlock(&(gw_osal_mut_table[mut_id].id))) {
        ret_val = GW_E_OSAL_SEM_FAILURE ;
    } else {
        ret_val = GW_E_OSAL_OK;
    }

    return ret_val;
}

int gw_mutex_lock(unsigned int mut_id)
{
    int ret_val;

    if (mut_id >= GW_OSAL_MAX_MUTEX || gw_osal_mut_table[mut_id].free == TRUE) {
        gw_printf("\r\n lock an invalid mutex");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    if (pthread_mutex_lock(&(gw_osal_mut_table[mut_id].id))) {
        ret_val = GW_E_OSAL_SEM_FAILURE;
    } else {
        ret_val = GW_E_OSAL_OK;
    }

    return ret_val;
}

int gw_mutex_trylock(unsigned int mut_id)
{
    int ret_val;

    if (mut_id >= GW_OSAL_MAX_MUTEX || gw_osal_mut_table[mut_id].free == TRUE) {
        gw_printf("\r\n trylock an invalid mutex");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    if (pthread_mutex_trylock(&(gw_osal_mut_table[mut_id].id))) {
        ret_val = GW_E_OSAL_SEM_FAILURE;
    } else {
        ret_val = GW_E_OSAL_OK;
    }

    return ret_val;
}

gw_uint32 gw_mutex_number()
{
    gw_int32 i;
    gw_uint32 count=0;

    pthread_mutex_lock(&gw_osal_mut_table_mut);
    for(i = 0; i < GW_OSAL_MAX_MUTEX; i++)
    {
        if(gw_osal_mut_table[i].free == FALSE)
            count++;
    }
    pthread_mutex_unlock(&gw_osal_mut_table_mut);

    return count;
}
#endif

/****************************************************************************************
                                MESSAGE QUEUE API
****************************************************************************************/

/*---------------------------------------------------------------------------------------
   Name: gw_queue_create

   Purpose: Create a message queue which can be refered to by name or ID

   Returns: GW_E_OSAL_INVALID_POINTER if a pointer passed in is NULL
            GW_E_OSAL_ERR_NAME_TOO_LONG if the name passed in is too long
            GW_E_OSAL_ERR_NO_FREE_IDS if there are already the max queues created
            GW_E_OSAL_ERR_NAME_TAKEN if the name is already being used on another queue
            GW_E_OSAL_ERR if the OS create call fails
            GW_E_OSAL_OK if success

   Notes: the flahs parameter is unused.
---------------------------------------------------------------------------------------*/
#if 0
gw_int32 gw_queue_create(gw_uint32 *queue_id, const gw_int8 *queue_name, gw_uint32 queue_depth, gw_uint32 data_size, gw_uint32 flags)
{
    gw_uint32 possible_qid;
    gw_int8 pool_name[16];

    if (queue_id == NULL || queue_name == NULL) {
        osal_printf("\r\n Create queue failed cause some parameter is NULL");
        return GW_E_OSAL_INVALID_POINTER;
    }

    if(queue_depth == 0 || data_size == 0) {
        osal_printf("\r\n Create queue with queue_depth %d and data_size %d",queue_depth,data_size);
        return GW_E_OSAL_ERR;
    }

    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */
    if (strlen(queue_name) >= GW_OSAL_MAX_API_NAME) {
        osal_printf("\r\n queue name is too long");
        return GW_E_OSAL_ERR_NAME_TOO_LONG;
    }

    /* Check Parameters */
    cyg_mutex_lock(&gw_os_queue_table_mutex);
    for (possible_qid = 0; possible_qid < GW_OSAL_MAX_QUEUE; possible_qid++) {
        if (gw_osal_queue_table[possible_qid].free == TRUE)
            break;
    }

    if (possible_qid >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[possible_qid].free != TRUE) {
        osal_printf("\r\n no free queue slot");
        cyg_mutex_unlock(&gw_os_queue_table_mutex);
        return GW_E_OSAL_ERR_NO_FREE_IDS;
    }

#if 0 /*Remove it for new requirement 2009-12-28*/
    /* Check to see if the name is already taken */
    for (i = 0; i < GW_OSAL_MAX_QUEUE; i++) {
        if ((gw_osal_queue_table[i].free == FALSE) &&
                strcmp((gw_int8*)queue_name, gw_osal_queue_table[i].name) == 0) {
            cyg_mutex_unlock(&gw_os_queue_table_mutex);
            return GW_E_OSAL_ERR_NAME_TAKEN;
        }
    }
#endif

    gw_osal_queue_table[possible_qid].free = FALSE;
    cyg_mutex_unlock(&gw_os_queue_table_mutex);
    /* Create VxWorks Message Queue and memory pool */
    if(queue_depth > CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE)
        queue_depth = CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE;

    memset(pool_name , 0 , sizeof(pool_name));
    sprintf(pool_name , "queue_p%d",possible_qid);
    if (GW_E_OSAL_OK != gw_mempool_create(&gw_osal_queue_table[possible_qid].mempool_id,
                                            pool_name,
                                            data_size + sizeof(gw_uint32),
                                            queue_depth)) {
        cyg_mutex_lock(&gw_os_queue_table_mutex);
        gw_osal_queue_table[possible_qid].free = TRUE;
        cyg_mutex_unlock(&gw_os_queue_table_mutex);
        osal_printf("\r\n Create ecos queue's mempool failed");
        return GW_E_OSAL_ERR;
    }

    cyg_mbox_create(&gw_osal_queue_table[possible_qid].id, &gw_osal_queue_table[possible_qid].mbox);
    /* check if message Q create failed */
    /* Set the queue_id to the id that was found available*/
    /* Set the name of the queue, and the creator as well */
    *queue_id = possible_qid;

    cyg_mutex_lock(&gw_os_queue_table_mutex);
    gw_osal_queue_table[*queue_id].free = FALSE;
    strcpy(gw_osal_queue_table[*queue_id].name, (gw_int8*) queue_name);
    gw_osal_queue_table[*queue_id].creator = gw_creator_find();
    gw_osal_queue_table[*queue_id].depth = queue_depth;
    gw_osal_queue_table[*queue_id].queue_size = data_size;
    gw_osal_queue_table[*queue_id].queue_type = GW_NORMAL_QUEUE_TYPE;
    cyg_mutex_unlock(&gw_os_queue_table_mutex);
    return GW_E_OSAL_OK;

}


/*--------------------------------------------------------------------------------------
    Name: gw_queue_delete

    Purpose: Deletes the specified message queue.

    Returns: GW_E_OSAL_ERR_INVALID_ID if the id passed in does not exist
             GW_E_OSAL_ERR if the OS call to delete the queue fails
             GW_E_OSAL_OK if success

    Notes: If There are messages on the queue, they will be lost and any subsequent
           calls to QueueGet or QueuePut to this queue will result in errors
---------------------------------------------------------------------------------------*/

gw_int32 gw_queue_delete(gw_uint32 queue_id)
{
    /* Check to see if the queue_id given is valid */

    if (queue_id >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[queue_id].free == TRUE ||
                    gw_osal_queue_table[queue_id].queue_type != GW_NORMAL_QUEUE_TYPE) {
        osal_printf("\r\n can not delete an invalid queue");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    /* Try to delete the queue */
    if (GW_E_OSAL_OK != gw_mempool_destroy(gw_osal_queue_table[queue_id].mempool_id)) {
        osal_printf("\r\n destroy queue's mempool failed");
        return GW_E_OSAL_ERR;
    }
    gw_mempool_destroy(gw_osal_queue_table[queue_id].mempool_id);
    cyg_mbox_delete(gw_osal_queue_table[queue_id].id);

    cyg_mutex_lock(&gw_os_queue_table_mutex);
    gw_osal_queue_table[queue_id].free = TRUE;
    strcpy(gw_osal_queue_table[queue_id].name, "");
    gw_osal_queue_table[queue_id].creator = GW_OSAL_UNINIT;
    gw_osal_queue_table[queue_id].id = 0;
    memset(&gw_osal_queue_table[queue_id].mbox , 0 , sizeof(cyg_mbox));

    cyg_mutex_unlock(&gw_os_queue_table_mutex);


    return GW_E_OSAL_OK;

}


/*---------------------------------------------------------------------------------------
   Name: gw_queue_get

   Purpose: Receive a message on a message queue.  Will pend or timeout on the receive.
   Returns: GW_E_OSAL_ERR_INVALID_ID if the given ID does not exist
            OSAL_ERR_INVALID_POINTER if a pointer passed in is NULL
            GW_E_OSAL_QUEUE_EMPTY if the Queue has no messages on it to be recieved
            GW_E_OSAL_QUEUE_TIMEOUT if the timeout was OSAL_PEND and the time expired
            GW_E_OSAL_QUEUE_INVALID_SIZE if the size copied from the queue was not correct
            GW_E_OSAL_OK if success
---------------------------------------------------------------------------------------*/

gw_int32 gw_queue_get(gw_uint32 queue_id, void *data, gw_uint32 size, gw_uint32 *size_copied, gw_int32 timeout)
{
    gw_uint8 *buf_ptr = 0;
    gw_uint8 *buf_will_be_cpy = NULL;
    gw_uint32 cpy_len = 0;
    gw_uint32 sys_ticks;

    /* Check Parameters */

    if (queue_id >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[queue_id].free == TRUE ||
                    gw_osal_queue_table[queue_id].queue_type != GW_NORMAL_QUEUE_TYPE) {
        osal_printf("\r\n can not get msg from an invalid queue");
        return GW_E_OSAL_ERR_INVALID_ID;
    } else {
        if ((data == NULL) || (size_copied == NULL)) {
            osal_printf("\r\n get msg failed cause some parameter is NULL");
            gw_osal_queue_table[queue_id].get_data_error++;
            gw_osal_queue_table[queue_id].get_error++;
            return GW_E_OSAL_INVALID_POINTER;
        }
    }

    /* Get Message From VxWorks Message Queue */

    if (timeout == GW_OSAL_WAIT_FOREVER) {
        buf_ptr = (gw_uint8 *)cyg_mbox_get(gw_osal_queue_table[queue_id].id);
        if(buf_ptr == NULL)
            gw_osal_queue_table[queue_id].get_error++;
    } else {
        if (timeout == GW_OSAL_NO_WAIT) {
            buf_ptr = (gw_uint8 *)cyg_mbox_tryget(gw_osal_queue_table[queue_id].id);
            if(buf_ptr == NULL)
                gw_osal_queue_table[queue_id].get_error++;
        } else {
            sys_ticks = gw_milli_to_ticks(timeout);
            buf_ptr = (gw_uint8 *)cyg_mbox_timed_get(gw_osal_queue_table[queue_id].id , cyg_current_time() + sys_ticks);
            if(buf_ptr == NULL)
            {
                gw_osal_queue_table[queue_id].get_timeout_error++;
                gw_osal_queue_table[queue_id].get_error++;
            }
        }
    }

    if (buf_ptr == NULL) {
        *size_copied = 0;
        return GW_E_OSAL_ERR;
    } else {
        buf_will_be_cpy = (gw_uint8 *)buf_ptr;
        cpy_len = *(gw_uint32 *)buf_will_be_cpy;
        if (cpy_len > size) {
            *size_copied = 0;
            gw_mem_free(buf_will_be_cpy);
            gw_osal_queue_table[queue_id].get_data_long++;
            gw_osal_queue_table[queue_id].get_error++;
            return GW_E_OSAL_ERR;
        }
        memcpy(data , buf_will_be_cpy + sizeof(gw_uint32) , cpy_len);
        *size_copied = cpy_len;
        gw_mem_free(buf_will_be_cpy);
    }

    return GW_E_OSAL_OK;
}


/*---------------------------------------------------------------------------------------
   Name: gw_queue_put

   Purpose: Put a message on a message queue.

   Returns: GW_E_OSAL_ERR_INVALID_ID if the queue id passed in is not a valid queue
            GW_E_OSAL_INVALID_POINTER if the data pointer is NULL
            GW_E_OSAL_QUEUE_FULL if the queue cannot accept another message
            GW_E_OSAL_ERR if the OS call returns an error
            GW_E_OSAL_OK if SUCCESS

   Notes: The flags parameter is not used.  The message put is always configured to
            immediately return an error if the receiving message queue is full.
---------------------------------------------------------------------------------------*/

gw_int32 gw_queue_put(gw_uint32 queue_id, void *data, gw_uint32 size, gw_int32 timeout, gw_uint32 flags)
{
    gw_uint8 *pbuf = NULL;
    gw_uint32 sys_ticks;
    gw_int32 queue_count = GW_OSAL_INVALID_QUEUE;

    /* Check Parameters */

    if (queue_id >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[queue_id].free == TRUE ||
                    gw_osal_queue_table[queue_id].queue_type != GW_NORMAL_QUEUE_TYPE) {
        osal_printf("\r\n can not put msg into an invalid queue");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    if (data == NULL) {
        osal_printf("\r\n can not put NULL msg into the queue");
        gw_osal_queue_table[queue_id].put_data_error++;
        gw_osal_queue_table[queue_id].put_error++;
        return GW_E_OSAL_INVALID_POINTER;
    }

    /* Send Message to VxWorks Message Queue */

    if(size > gw_osal_queue_table[queue_id].queue_size) {
        osal_printf("\r\n data size is greater than the queue's size");
        gw_osal_queue_table[queue_id].put_data_long++;
        gw_osal_queue_table[queue_id].put_error++;
        return GW_E_OSAL_ERR;
    }

    pbuf = gw_mem_malloc(gw_osal_queue_table[queue_id].mempool_id);
    if (pbuf == NULL) {
        gw_osal_queue_table[queue_id].put_full_error++;
        gw_osal_queue_table[queue_id].put_error++;
        return GW_E_OSAL_ERR;
    }
    memcpy(pbuf + sizeof(gw_uint32), data , size);
    *(gw_uint32 *)pbuf = size;

    if (timeout == GW_OSAL_WAIT_FOREVER) {
        cyg_mbox_put(gw_osal_queue_table[queue_id].id, (void *)pbuf);
    } else {
        if (timeout == GW_OSAL_NO_WAIT) {
            if (!cyg_mbox_tryput(gw_osal_queue_table[queue_id].id, (void *)pbuf)) {
                gw_mem_free(pbuf);
                gw_osal_queue_table[queue_id].put_full_error++;
                gw_osal_queue_table[queue_id].put_error++;
                gw_osal_queue_table[queue_id].peek_value = gw_osal_queue_table[queue_id].depth;
                return GW_E_OSAL_ERR;
            }
        } else {
            sys_ticks = gw_milli_to_ticks(timeout);

            if (!cyg_mbox_timed_put(gw_osal_queue_table[queue_id].id, (void *)pbuf , cyg_current_time() + sys_ticks)) {
                gw_mem_free(pbuf);
                gw_osal_queue_table[queue_id].put_timeout_error++;
                gw_osal_queue_table[queue_id].put_error++;
                gw_osal_queue_table[queue_id].peek_value = gw_osal_queue_table[queue_id].depth;
                return GW_E_OSAL_ERR;
            }
        }
    }


    queue_count = gw_queue_count(queue_id);
    if(gw_osal_queue_table[queue_id].peek_value <= queue_count)
        gw_osal_queue_table[queue_id].peek_value = queue_count;

    return GW_E_OSAL_OK;
}
#else
int gw_queue_create
	(
    unsigned int 	*queue_id,
    const char		*queue_name,
    unsigned int 	queue_depth,
    unsigned int 	data_size,
    unsigned int 	flags
	)
{
    unsigned int possible_qid;
    struct mq_attr queue_attr;
    int i;

    if (queue_id == NULL || queue_name == NULL) {
        gw_printf("\r\n Create queue failed cause some parameter is NULL");
        return GW_E_OSAL_INVALID_POINTER;
    }

    if (queue_depth == 0 || data_size == 0) {
        gw_printf("\r\n Create queue with queue_depth %d and data_size %d", queue_depth, data_size);
        return GW_E_OSAL_ERR;
    }

    if (strlen(queue_name) >= GW_OSAL_MAX_API_NAME) {
        gw_printf("\r\n queue name is too long");
        return GW_E_OSAL_ERR_NAME_TOO_LONG;
    }

    pthread_mutex_lock(&gw_os_queue_table_mut);
    for (possible_qid = 0; possible_qid < GW_OSAL_MAX_QUEUE; possible_qid++) {
        if (gw_osal_queue_table[possible_qid].free == TRUE)
            break;
    }

    if (possible_qid >= GW_OSAL_MAX_QUEUE) {
        gw_printf("\r\n no free queue slot");
        pthread_mutex_unlock(&gw_os_queue_table_mut);
        return GW_E_OSAL_ERR_NO_FREE_IDS;
    }

    for (i = 0; i < GW_OSAL_MAX_QUEUE; i++) {
        if ((gw_osal_queue_table[i].free == FALSE) &&
                strcmp((char*)queue_name, gw_osal_queue_table[i].name) == 0) {
            pthread_mutex_unlock(&gw_os_queue_table_mut);
            return GW_E_OSAL_ERR_NAME_TAKEN;
        }
    }

    gw_osal_queue_table[possible_qid].free = FALSE;

	memset(&queue_attr , 0 ,sizeof(struct mq_attr));
    queue_attr.mq_maxmsg = queue_depth;
    queue_attr.mq_msgsize = data_size;
    gw_osal_queue_table[possible_qid].id = mq_open(queue_name , O_CREAT | O_RDWR , 0777 , &queue_attr);
    if (gw_osal_queue_table[possible_qid].id == 0) {
        memset(&gw_osal_queue_table[possible_qid] , 0 , sizeof(osal_queue_record_t));
        gw_osal_queue_table[possible_qid].free = TRUE;
        pthread_mutex_unlock(&gw_os_queue_table_mut);
        return GW_E_OSAL_ERR;
    }
    *queue_id = possible_qid;

    gw_osal_queue_table[*queue_id].free = FALSE;
    strcpy(gw_osal_queue_table[*queue_id].name, (char*) queue_name);
    gw_osal_queue_table[*queue_id].depth = queue_depth;
    gw_osal_queue_table[*queue_id].queue_size = data_size;
	gw_osal_queue_table[*queue_id].queue_type = GW_NORMAL_QUEUE_TYPE;
    pthread_mutex_unlock(&gw_os_queue_table_mut);
    return GW_E_OSAL_OK;
}

int gw_queue_delete
	(
    unsigned int queue_id
	)
{
    if (queue_id >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[queue_id].free == TRUE) {
        gw_printf("\r\n can not delete an invalid queue");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    pthread_mutex_lock(&gw_os_queue_table_mut);

    mq_unlink(gw_osal_queue_table[queue_id].name);
    memset(&gw_osal_queue_table[queue_id] , 0 , sizeof(osal_queue_record_t));
    gw_osal_queue_table[queue_id].free = TRUE;
    pthread_mutex_unlock(&gw_os_queue_table_mut);

    return GW_E_OSAL_OK;
}

int gw_queue_get
	(
    unsigned int 	queue_id,
    void 			*data,
    unsigned int 	size,
    unsigned int 	*size_copied,
    int 			timeout
	)
{
    unsigned int msg_pri = 0;
    ssize_t recv_len = -1;
    struct timespec tv;

    if (queue_id >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[queue_id].free == TRUE) {
        gw_printf("\r\n can not get msg from an invalid queue");
        return GW_E_OSAL_ERR_INVALID_ID;
    } else {
        if ((data == NULL) || (size_copied == NULL)) {
            gw_printf("\r\n get msg failed cause some parameter is NULL");
            gw_osal_queue_table[queue_id].get_error++;
            gw_osal_queue_table[queue_id].get_data_error++;
            return GW_E_OSAL_INVALID_POINTER;
        }
    }

    if(timeout != -1)
    {
        gw_uint64 tim = 0;
        tim = gw_current_time();
        tim += timeout/10;
        tv.tv_sec = tim/100;
        tv.tv_nsec = (tim%100)*10*1000*1000;
    }

    if(timeout == GW_OSAL_WAIT_FOREVER)
        recv_len = mq_receive(gw_osal_queue_table[queue_id].id , (char *)data , size , &msg_pri);
    else if(timeout == GW_OSAL_NO_WAIT)
        recv_len = mq_timedreceive(gw_osal_queue_table[queue_id].id , (char *)data , size , &msg_pri, NULL);
    else
        recv_len = mq_timedreceive(gw_osal_queue_table[queue_id].id , (char *)data , size , &msg_pri,&tv);
    
    if (recv_len == -1) {
        gw_osal_queue_table[queue_id].get_error++;
        gw_osal_queue_table[queue_id].get_data_error++;
        return GW_E_OSAL_ERR;
    }
	
    *size_copied = (unsigned int)recv_len;
	if(gw_osal_queue_table[queue_id].cur_num > 0)
		gw_osal_queue_table[queue_id].cur_num--;

    return GW_E_OSAL_OK;
}

int gw_queue_put
	(
    unsigned int	queue_id,
    void 			*data,
    unsigned int 	size,
    int 			timeout,
    unsigned int 	priority
	)
{
    int ret = -1;

    if (queue_id >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[queue_id].free == TRUE) {
        gw_printf("\r\n can not put msg into an invalid queue");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    if (data == NULL) {
        gw_printf("\r\n can not put NULL msg into the queue");
        gw_osal_queue_table[queue_id].put_error++;
        gw_osal_queue_table[queue_id].put_data_error++;
        return GW_E_OSAL_INVALID_POINTER;
    }

    if (size > gw_osal_queue_table[queue_id].queue_size) {
        gw_printf("\r\n data size is greater than the queue's size, size %d, que_size %d", size, gw_osal_queue_table[queue_id].queue_size);
        gw_osal_queue_table[queue_id].put_error++;
        gw_osal_queue_table[queue_id].put_data_long++;
        return GW_E_OSAL_ERR;
    }

    #if 0
    ret = mq_send(gw_osal_queue_table[queue_id].id , (char *)data , size , priority);
    #else
    ret = mq_timedsend(gw_osal_queue_table[queue_id].id , (char *)data , size , priority, NULL);
    #endif
    if (ret == -1) {
		gw_osal_queue_table[queue_id].put_full_error++;
        return GW_E_OSAL_ERR;
    }

	gw_osal_queue_table[queue_id].cur_num++;
	if(gw_osal_queue_table[queue_id].peek_value <= gw_osal_queue_table[queue_id].cur_num)
        gw_osal_queue_table[queue_id].peek_value = gw_osal_queue_table[queue_id].cur_num;
	
    return GW_E_OSAL_OK;
}

#endif

void gw_queue_show_specific(gw_uint32 qid)
{
    osal_queue_record_t *pQueue=NULL;

    if(qid >= GW_OSAL_MAX_QUEUE)
        return;

    pQueue = &gw_osal_queue_table[qid];
    if(pQueue->free == TRUE || pQueue->queue_type != GW_NORMAL_QUEUE_TYPE)
        return;

    gw_printf("\r\n%-3d %-16s %-5d %-4d %-5d %-5d %-8d",qid,pQueue->name,pQueue->depth,pQueue->queue_size,
                                            gw_queue_count(qid),pQueue->peek_value,pQueue->put_full_error);
    return;
}

#if 0
void gw_queue_show(gw_uint32 queue_id)
{
    gw_int32 qid = 0;

    gw_printf("\r\n====================================================");
    gw_printf("\r\n%-3s %-16s %-5s %-4s %-5s %-5s %-8s","Id","Name","Depth","Size","Used","Peek","PutFail");
    gw_printf("\r\n----------------------------------------------------");
    if(GW_OSAL_INVALID_QUEUE == queue_id)
    {
        for(qid = 0 ; qid < GW_OSAL_MAX_QUEUE ; qid++)
        {
            if(gw_osal_queue_table[qid].free == TRUE || gw_osal_queue_table[qid].queue_type != GW_NORMAL_QUEUE_TYPE)
                continue;
            gw_queue_show_specific(qid);
        }
        gw_printf("\r\n====================================================");
    }
    else
    {
        qid = queue_id;
        if(gw_osal_queue_table[qid].free != TRUE && gw_osal_queue_table[qid].queue_type == GW_NORMAL_QUEUE_TYPE)
        {
            gw_queue_show_specific(qid);
            gw_printf("\r\n====================================================");
            gw_printf("\r\n>Details:");
            gw_printf("\r\n Mempool Id      : %d",gw_osal_queue_table[qid].mempool_id);
            gw_printf("\r\n Mempool Name    : %s",gw_pool_name_get(gw_osal_queue_table[qid].mempool_id));
            gw_printf("\r\n Put Error       : %d",gw_osal_queue_table[qid].put_error);
            gw_printf("\r\n   Full          : %d",gw_osal_queue_table[qid].put_full_error);
            gw_printf("\r\n   Timeout       : %d",gw_osal_queue_table[qid].put_timeout_error);
            gw_printf("\r\n   Data-is-NULL  : %d",gw_osal_queue_table[qid].put_data_error);
            gw_printf("\r\n   Size-too-long : %d",gw_osal_queue_table[qid].put_data_long);
            gw_printf("\r\n Get Error       : %d",gw_osal_queue_table[qid].get_error);
            gw_printf("\r\n   Timeout       : %d",gw_osal_queue_table[qid].get_timeout_error);
            gw_printf("\r\n   Data-is-NULL  : %d",gw_osal_queue_table[qid].get_data_error);
            gw_printf("\r\n   Size-too-long : %d",gw_osal_queue_table[qid].get_data_long);
        }
    }
    gw_printf("\r\n");
    return;
}
#endif

gw_uint32 gw_queue_number()
{
    gw_int32 i;
    gw_uint32 count=0;

    pthread_mutex_lock(&gw_os_queue_table_mut);
    for(i = 0; i < GW_OSAL_MAX_QUEUE; i++)
    {
        if(gw_osal_queue_table[i].free == FALSE && gw_osal_queue_table[i].queue_type == GW_NORMAL_QUEUE_TYPE)
            count++;
    }
    pthread_mutex_unlock(&gw_os_queue_table_mut);

    return count;
}


/*---------------------------------------------------------------------------------------
    Name: gw_queue_count

    Purpose: This function will pass back a pointer to structure that contains
             all of the relevant info (name and creator) about the specified queue.

    Returns: GW_E_OSAL_INVALID_POINTER if queue_prop is NULL
             GW_E_OSAL_ERR_INVALID_ID if the ID given is not  a valid queue
             GW_E_OSAL_OK if the info was copied over correctly
---------------------------------------------------------------------------------------*/

gw_int32 gw_queue_count(gw_uint32 queue_id)
{
    /* Check to see that the id given is valid */
    gw_int32 count = 0;

    if (queue_id >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[queue_id].free == TRUE ||
                gw_osal_queue_table[queue_id].queue_type != GW_NORMAL_QUEUE_TYPE)
        return GW_OSAL_INVALID_QUEUE;

    count = gw_osal_queue_table[queue_id].cur_num;

    return count;

}

gw_int32 gw_queue_type_get(gw_uint32 queue_id)
{
    if (queue_id >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[queue_id].free == TRUE)
        return GW_NORMAL_QUEUE_TYPE;

    return gw_osal_queue_table[queue_id].queue_type;
}

/****************************************************************************************
                                PRIORITY MESSAGE QUEUE API
****************************************************************************************/

/*---------------------------------------------------------------------------------------
   Name: gw_pri_queue_create

   Purpose: Create a priority message queue which can be refered to by name or ID

   Returns:
---------------------------------------------------------------------------------------*/

gw_int32 gw_pri_queue_create (gw_uint32 *queue_id, const gw_int8 *queue_name, gw_uint32 queue_depth, gw_uint32 data_size, gw_int32 pri_num)
{
    gw_uint32 possible_qid;
//    gw_int8 pri_queue_name[16];
    gw_int8 pri_queue_sem_name[16];
    gw_int32 i;

    if ( queue_id == NULL || queue_name == NULL) {
        osal_printf("\r\n Create pri queue failed cause some parameter is NULL");
        return GW_E_OSAL_INVALID_POINTER;
    }

    if(queue_depth == 0 || data_size == 0) {
        osal_printf("\r\n Create pri queue with queue_depth %d and data_size %d",queue_depth,data_size);
        return GW_E_OSAL_ERR;
    }

    if(pri_num <= 0 || pri_num > GW_OSAL_MAX_QUEUE_PRI)
    {
        osal_printf("\r\n priority is out of range");
        return GW_E_OSAL_ERR;
    }

    /* we don't want to allow names too long*/
    /* if truncated, two names might be the same */

    if (strlen(queue_name) >= GW_OSAL_MAX_API_NAME) {
        osal_printf("\r\n pri queue name is too long");
        return GW_E_OSAL_ERR_NAME_TOO_LONG;
    }


   /* Check Parameters */


   pthread_mutex_lock(&gw_os_queue_table_mut);
    for(possible_qid = 0; possible_qid < GW_OSAL_MAX_QUEUE; possible_qid++)
    {
        if (gw_osal_queue_table[possible_qid].free == TRUE)
            break;
    }

    if( possible_qid >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[possible_qid].free != TRUE)
    {
        osal_printf("\r\n no free queue slot");
        pthread_mutex_unlock(&gw_os_queue_table_mut);
        return GW_E_OSAL_ERR_NO_FREE_IDS;
    }

//    memset(pri_queue_name , 0 , sizeof(pri_queue_name));
//    sprintf(pri_queue_name , "pq_%s" , queue_name);

#if 0
    if(GW_E_OSAL_OK != gw_mempool_create(
                                            &gw_osal_queue_table[possible_qid].mempool_id,
                                            pri_queue_name,
                                            data_size+sizeof(gw_node)+sizeof(gw_uint32),
                                            queue_depth*pri_num))
    {
        osal_printf("\r\n create queue memory pool failed");
        pthread_mutex_unlock(&gw_os_queue_table_mut);
        return GW_E_OSAL_ERR;
    }
#endif

    memset(pri_queue_sem_name , 0 , sizeof(pri_queue_sem_name));
    sprintf(pri_queue_sem_name , "pq_sem_%d",possible_qid);
    if(GW_E_OSAL_OK != gw_semaphore_init(&gw_osal_queue_table[possible_qid].cnt_sem_id, pri_queue_sem_name, 0,0))
    {
#if 0
        gw_mempool_destroy(gw_osal_queue_table[possible_qid].mempool_id);
#endif
        pthread_mutex_unlock(&gw_os_queue_table_mut);
        return GW_E_OSAL_ERR;
    }

    for(i = 0 ; i < pri_num ; i++)
    {
        gw_int8 mutex_name[GW_OSAL_MAX_API_NAME+8];
        gw_lst_init(&gw_osal_queue_table[possible_qid].queue_list[i], NULL);
        memset(mutex_name , 0 , sizeof(mutex_name));
        sprintf(mutex_name , "mx_%s_%d",queue_name , i);
        gw_mutex_init(&gw_osal_queue_table[possible_qid].queue_mutex[i], mutex_name , 0);
    }
    gw_osal_queue_table[possible_qid].free = FALSE;
    pthread_mutex_unlock(&gw_os_queue_table_mut);

    /* Create VxWorks Message Queue */

    gw_osal_queue_table[possible_qid].id = (mqd_t)possible_qid;
    gw_osal_queue_table[possible_qid].pri_num = pri_num;

    /* Set the queue_id to the id that was found available*/
    /* Set the name of the queue, and the creator as well */

    *queue_id = possible_qid;

    pthread_mutex_lock(&gw_os_queue_table_mut);

    gw_osal_queue_table[*queue_id].free = FALSE;
    strcpy( gw_osal_queue_table[*queue_id].name, (gw_int8*) queue_name);
    gw_osal_queue_table[*queue_id].creator = gw_creator_find();
    gw_osal_queue_table[*queue_id].queue_size = data_size;
    gw_osal_queue_table[*queue_id].depth = queue_depth;
    gw_osal_queue_table[*queue_id].queue_type = GW_PRI_QUEUE_TYPE;
    pthread_mutex_unlock(&gw_os_queue_table_mut);

    return GW_E_OSAL_OK;

}

gw_int32 gw_pri_queue_put (gw_uint32 queue_id, void *data, gw_uint32 size, gw_int32 timeout, gw_int32 priority)
{
    gw_uint8 *pbuf = NULL;
    gw_uint32 total = 0;
    /* Check Parameters */

    if(queue_id >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[queue_id].free == TRUE ||
                            gw_osal_queue_table[queue_id].queue_type != GW_PRI_QUEUE_TYPE) {
        osal_printf("\r\n can not put msg into an invalid queue");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    if (data == NULL) {
        osal_printf("\r\n can not put NULL msg into the queue");
        gw_osal_queue_table[queue_id].put_data_error++;
        gw_osal_queue_table[queue_id].put_error++;
        return GW_E_OSAL_INVALID_POINTER;
    }

    if(size > gw_osal_queue_table[queue_id].queue_size) {
        osal_printf("\r\n data size is greater than the queue's size");
        gw_osal_queue_table[queue_id].put_data_long++;
        gw_osal_queue_table[queue_id].put_error++;
        return GW_E_OSAL_ERR;
    }

    if(priority < 0 || priority >= gw_osal_queue_table[queue_id].pri_num)
    {
        gw_osal_queue_table[queue_id].put_error++;
        return GW_E_OSAL_ERR;
    }

    if(gw_lst_count(&gw_osal_queue_table[queue_id].queue_list[priority]) >= gw_osal_queue_table[queue_id].depth)
    {
        gw_osal_queue_table[queue_id].put_full_error++;
        gw_osal_queue_table[queue_id].put_error++;
        return GW_E_OSAL_ERR;
    }

//    pbuf = gw_mem_malloc(gw_osal_queue_table[queue_id].mempool_id);
    pbuf = malloc(sizeof(gw_node)+sizeof(gw_uint32)+size);
    if(pbuf == NULL)
    {
        gw_osal_queue_table[queue_id].put_error++;
        return GW_E_OSAL_ERR;
    }
/*
    --------------------------------------------------
    |  gw_node(8bytes)    | size(4bytes)|         data('size'  bytes)  |
    --------------------------------------------------
*/
    *((gw_uint32 *)(pbuf+sizeof(gw_node))) = size;
    memcpy(pbuf + sizeof(gw_node)+sizeof(gw_uint32) , data , size);

    gw_mutex_lock(gw_osal_queue_table[queue_id].queue_mutex[priority]);

    gw_lst_add(&gw_osal_queue_table[queue_id].queue_list[priority] , (gw_node *)pbuf);

    if(gw_osal_queue_table[queue_id].pri_queue_peek[priority] <
        gw_lst_count(&gw_osal_queue_table[queue_id].queue_list[priority]))
        gw_osal_queue_table[queue_id].pri_queue_peek[priority] = gw_lst_count(&gw_osal_queue_table[queue_id].queue_list[priority]);

    total = gw_pri_queue_count(queue_id);
    if(total > gw_osal_queue_table[queue_id].peek_value)
        gw_osal_queue_table[queue_id].peek_value = total;

    gw_mutex_unlock(gw_osal_queue_table[queue_id].queue_mutex[priority]);

    gw_semaphore_post(gw_osal_queue_table[queue_id].cnt_sem_id);

    return GW_E_OSAL_OK;
}


gw_int32 gw_pri_queue_get (gw_uint32 queue_id, void *data, gw_uint32 size, gw_uint32 *size_copied, gw_int32 timeout)
{
    gw_uint8 *pbuf = NULL;
    gw_int32 ret = GW_E_OSAL_ERR;
    gw_int32 i;

    /* Check Parameters */

    if(queue_id >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[queue_id].free == TRUE ||
                        gw_osal_queue_table[queue_id].queue_type != GW_PRI_QUEUE_TYPE) {
        osal_printf("\r\n can not put msg into an invalid queue");
        return GW_E_OSAL_ERR_INVALID_ID;
    }

    if (data == NULL) {
        osal_printf("\r\n can not put NULL msg into the queue");
        gw_osal_queue_table[queue_id].get_data_error++;
        gw_osal_queue_table[queue_id].get_error++;
        return GW_E_OSAL_INVALID_POINTER;
    }

    ret = gw_semaphore_wait(gw_osal_queue_table[queue_id].cnt_sem_id, timeout);
    if(ret != GW_E_OSAL_OK)
    {
        gw_osal_queue_table[queue_id].get_error++;
        return GW_E_OSAL_ERR;
    }

    for(i = gw_osal_queue_table[queue_id].pri_num -1 ; i >= 0 ; i--)
    {
        if(gw_lst_count(&gw_osal_queue_table[queue_id].queue_list[i]) == 0)
            continue;

        gw_mutex_lock(gw_osal_queue_table[queue_id].queue_mutex[i]);
        pbuf = (gw_uint8 *)gw_lst_get(&gw_osal_queue_table[queue_id].queue_list[i]);
        gw_mutex_unlock(gw_osal_queue_table[queue_id].queue_mutex[i]);

        *size_copied = *((int *)(pbuf + sizeof(gw_node)));
        if(*size_copied > size)
        {
	   free(pbuf);
            gw_osal_queue_table[queue_id].get_data_long++;
            gw_osal_queue_table[queue_id].get_error++;
            return GW_E_OSAL_ERR;
        }
        memcpy(data , pbuf + sizeof(gw_node)+sizeof(gw_uint32) , *size_copied);
//        gw_mem_free(pbuf);
        free(pbuf);

        return GW_E_OSAL_OK;
    }

    gw_osal_queue_table[queue_id].get_error++;
    return GW_E_OSAL_ERR;
}

gw_uint32 gw_pri_queue_number()
{
    gw_int32 i;
    gw_uint32 count=0;

    pthread_mutex_lock(&gw_os_queue_table_mut);
    for(i = 0; i < GW_OSAL_MAX_QUEUE; i++)
    {
        if(gw_osal_queue_table[i].free == FALSE && gw_osal_queue_table[i].queue_type == GW_PRI_QUEUE_TYPE)
            count++;
    }
    pthread_mutex_unlock(&gw_os_queue_table_mut);

    return count;
}

gw_int32 gw_pri_queue_count (gw_uint32 queue_id)
{
    /* Check to see that the id given is valid */
    gw_int32 count = 0;
    gw_int32 i;

    if (queue_id >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[queue_id].free == TRUE ||
                            gw_osal_queue_table[queue_id].queue_type != GW_PRI_QUEUE_TYPE)
    {
        return GW_OSAL_INVALID_QUEUE;
    }

    for(i = 0 ; i < gw_osal_queue_table[queue_id].pri_num ; i++)
    {
        count += gw_lst_count(&gw_osal_queue_table[queue_id].queue_list[i]);
    }

    return count;
}

gw_int32 gw_pri_queue_max_priority(gw_uint32 queue_id)
{
    if (queue_id >= GW_OSAL_MAX_QUEUE || gw_osal_queue_table[queue_id].free == TRUE ||
                            gw_osal_queue_table[queue_id].queue_type != GW_PRI_QUEUE_TYPE)
    {
        return 0;
    }

    return gw_osal_queue_table[queue_id].pri_num-1;
}

void gw_pri_queue_show(gw_uint32 queue_id)
{
    gw_int32 i;
    osal_queue_record_t *pPriQueue = NULL;


    gw_printf("\r\n=============================================================");
    gw_printf("\r\n%-3s %-16s %-5s %-4s %-7s %-7s %-5s %-10s","Id","Name","Depth","Size","Pri-Cnt","Current","Peek","PutFail");
    gw_printf("\r\n-------------------------------------------------------------");
    if(queue_id == GW_OSAL_INVALID_QUEUE)
    {
        for(i = 0 ; i < GW_OSAL_MAX_QUEUE ; i++)
        {
            pPriQueue = &gw_osal_queue_table[i];
            if(pPriQueue->free == TRUE || pPriQueue->queue_type != GW_PRI_QUEUE_TYPE)
                continue;
            gw_printf("\r\n%-3d %-16s %-5d %-4d %-7d %-7d %-5d %-10d",
                                    i , pPriQueue->name,pPriQueue->depth,pPriQueue->queue_size , pPriQueue->pri_num,
                                    gw_pri_queue_count(i),pPriQueue->peek_value,pPriQueue->put_error);
        }

    }
    else
    {
        pPriQueue = &gw_osal_queue_table[queue_id];
        if(pPriQueue->free == TRUE || pPriQueue->queue_type != GW_PRI_QUEUE_TYPE)
            return;

        gw_printf("\r\n%-3d %-16s %-5d %-4d %-7d %-7d %-5d %-10d",
                                    queue_id , pPriQueue->name,pPriQueue->depth,pPriQueue->queue_size , pPriQueue->pri_num,
                                    gw_pri_queue_count(queue_id),pPriQueue->peek_value,pPriQueue->put_error);
        for(i = 0 ; i < pPriQueue->pri_num ; i++)
        {
            gw_printf("\r\n%-3s %-16s %-5s %-4s :%-6d %-7d %-5d %-10s",
                                "","","","",i,gw_lst_count(&pPriQueue->queue_list[i]),
                                pPriQueue->pri_queue_peek[i],"");
        }
    }
    gw_printf("\r\n=============================================================\r\n");
}


/****************************************************************************************
                                    INFO API
****************************************************************************************/

/*---------------------------------------------------------------------------------------
   Name: gw_current_time

   Purpose: This function returns the system time in tick.
---------------------------------------------------------------------------------------*/

gw_uint64 gw_current_time(void)
{
#ifdef CYG_LINUX
    return (gw_uint64) cyg_current_time();
#else
    return times(NULL);
#endif
}

void gw_timestamp_print()
{
	gw_uint64 current_time = 0;
	
	current_time = gw_current_time();
	gw_printf("[%12lld]", current_time*10);
}

/*---------------------------------------------------------------------------------------
   Name: gw_milli_to_ticks

   Purpose: This function accepts a time interval in milli_seconds as input an
            returns the tick equivalent is o.s. system clock ticks. The tick
            value is rounded up.  This algorthim should change to use a integer divide.
---------------------------------------------------------------------------------------*/

gw_int32 gw_milli_to_ticks(gw_uint32 milli_seconds)
{
    gw_uint32 sys_ticks = milli_seconds / 10;

    if(0 == sys_ticks)
        sys_ticks = 1;

    return sys_ticks;
    /*
     * this function can be modified - it gives a good approx without any
     * floating point (">>10" ~= "/1000")
    */
}


/*---------------------------------------------------------------------------------------
   Name: gw_tick_to_micros

   Purpose: This function returns the duration of a system tick in micro seconds.
---------------------------------------------------------------------------------------*/

gw_int32 gw_tick_to_micros()
{
    return 10000;

}


/*---------------------------------------------------------------------------------------
 * Name: gw_local_time_get
 *
 * Purpose: This functions get the local time of the machine its on
 * ------------------------------------------------------------------------------------*/
gw_int32 gw_local_time_get(osal_time_t *time_struct)
{
#if 0
    gw_int32 status;
    struct  timespec  time;

    if (time_struct == NULL)
        return GW_E_OSAL_INVALID_POINTER;

    status = clock_gettime(CLOCK_REALTIME, &time);
    if (status != OK)
        return GW_E_OSAL_ERR;

    time_struct->seconds = time.tv_sec;
    time_struct->microsecs = time.tv_nsec / 1000;
#endif

    return GW_E_OSAL_OK;
}

/*---------------------------------------------------------------------------------------
 * Name: gw_local_time_set
 *
 * Purpose: This function sets the local time of the machine its on
 * ------------------------------------------------------------------------------------*/

gw_int32 gw_local_time_set(osal_time_t *time_struct)
{

#if 0
    gw_int32 status;

    struct  timespec  time;

    if (time_struct == NULL)
        return GW_E_OSAL_INVALID_POINTER;

    time.tv_sec = time_struct->seconds;
    time.tv_nsec = (time_struct->microsecs) * 1000;

    status = clock_settime(CLOCK_REALTIME, &time);
    if (status != 0)
        return GW_E_OSAL_ERR;
#endif

    return GW_E_OSAL_OK;
}

/*---------------------------------------------------------------------------------------
 *  Name: gw_err_name_get()
---------------------------------------------------------------------------------------*/
gw_int32 gw_err_name_get(gw_int32 error_num, osal_err_name_t * err_name)
{
    osal_err_name_t local_name;
    gw_uint32 return_code;

    return_code = GW_E_OSAL_OK;

    if(err_name == NULL)
        return return_code;
    switch (error_num) {
    case GW_E_OSAL_OK:
        strcpy(local_name, "GW_E_OSAL_OK");
        break;
    case GW_E_OSAL_ERR:
        strcpy(local_name, "GW_E_OSAL_ERR");
        break;
    case GW_E_OSAL_INVALID_POINTER:
        strcpy(local_name, "GW_E_OSAL_INVALID_POINTER");
        break;
    case GW_E_OSAL_ERR_ADDR_MISALIGNED:
        strcpy(local_name, "OSAL_ADDRESS_MISALIGNED");
        break;
    case GW_E_OSAL_ERR_TIMEOUT:
        strcpy(local_name, "GW_E_OSAL_ERR_TIMEOUT");
        break;
    case GW_E_OSAL_INVALID_INT_NUM:
        strcpy(local_name, "GW_E_OSAL_INVALID_INT_NUM");
        break;
    case GW_E_OSAL_SEM_FAILURE:
        strcpy(local_name, "GW_E_OSAL_SEM_FAILURE");
        break;
    case GW_E_OSAL_SEM_TIMEOUT:
        strcpy(local_name, "GW_E_OSAL_SEM_TIMEOUT");
        break;
    case GW_E_OSAL_QUEUE_EMPTY:
        strcpy(local_name, "GW_E_OSAL_QUEUE_EMPTY");
        break;
    case GW_E_OSAL_QUEUE_FULL:
        strcpy(local_name, "GW_E_OSAL_QUEUE_FULL");
        break;
    case GW_E_OSAL_QUEUE_TIMEOUT:
        strcpy(local_name, "GW_E_OSAL_QUEUE_TIMEOUT");
        break;
    case GW_E_OSAL_QUEUE_INVALID_SIZE:
        strcpy(local_name, "GW_E_OSAL_QUEUE_INVALID_SIZE");
        break;
    case GW_E_OSAL_QUEUE_ID_ERROR:
        strcpy(local_name, "GW_E_OSAL_QUEUE_ID_ERROR");
        break;
    case GW_E_OSAL_ERR_NAME_TOO_LONG:
        strcpy(local_name, "GW_E_OSAL_ERR_NAME_TOO_LONG");
        break;
    case GW_E_OSAL_ERR_NO_FREE_IDS:
        strcpy(local_name, "GW_E_OSAL_ERR_NO_FREE_IDS");
        break;
    case GW_E_OSAL_ERR_NAME_TAKEN:
        strcpy(local_name, "GW_E_OSAL_ERR_NAME_TAKEN");
        break;
    case GW_E_OSAL_ERR_INVALID_ID:
        strcpy(local_name, "GW_E_OSAL_ERR_INVALID_ID");
        break;
    case GW_E_OSAL_ERR_NAME_NOT_FOUND:
        strcpy(local_name, "GW_E_OSAL_ERR_NAME_NOT_FOUND");
        break;
    case GW_E_OSAL_ERR_SEM_NOT_FULL:
        strcpy(local_name, "GW_E_OSAL_ERR_SEM_NOT_FULL");
        break;
    case GW_E_OSAL_ERR_INVALID_PRIORITY:
        strcpy(local_name, "GW_E_OSAL_ERR_INVALID_PRIORITY");
        break;

    default:
        strcpy(local_name, "ERROR_UNKNOWN");
        return_code = GW_E_OSAL_ERR;
    }

    strcpy((gw_int8*) err_name, local_name);


    return return_code;
}

/*---------------------------------------------------------------------------------------
 * gw_creator_find()
 *  This function will return the OSAL ID of the task that created the calling function;
 *  This is an internal call, not to be used by the user.
---------------------------------------------------------------------------------------*/
gw_uint32 gw_creator_find()
{
    unsigned int threadid;
#ifdef CYG_LINUX
    cyg_handle_t thread_id;
#else
    pthread_t thread_id;
#endif
    gw_int32 i;

#ifdef CYG_LINUX
    thread_id = cyg_thread_self();
#else
    thread_id = pthread_self();
#endif
    threadid = (unsigned int)thread_id;
    for (i = 0; i < GW_OSAL_MAX_THREAD; i++) {
        if (threadid == gw_osal_thread_table[i].id)
            break;
    }

    return i;
}


/* ---------------------------------------------------------------------------
 * Name: gw_printf
 *
 * Purpose: This function abstracts out the printf type statements. This is
 *          useful for using OS- specific thats that will allow non-polled
 *          print statements for the real time systems.
 *
 * Note:    This function uses a utility task that gets passed the print
 *          messages on a queue. This allows that task to block (if
 *          necessary), so the calling task does not.
 *
 ---------------------------------------------------------------------------*/

void gw_printf(const gw_int8 *String, ...)
{
    va_list ap;
    int ret;
	
//    extern int diag_vprintf(const char *fmt, va_list ap);

    va_start(ap, String);
    ret = diag_vprintf(String, ap);
    va_end(ap);

    return;
}
#ifndef CYG_LINUX
#define LOGLEN 1024
int cmd_display_onu_thread_info_cli(struct cli_def *cli, char *command, char *argv[], int argc)
{
	int i_ret = 0;
	int i_fd = 0;
	int i_count=0;
	int i_readnum = 0;
	unsigned char thread_info_buf[LOGLEN]={0};
    if(CLI_HELP_REQUESTED)
    {
        switch(argc)
        {
        case 1:
        	return gw_cli_arg_help(cli, 0,
        		"[show]","thread info show",
        		NULL);
        default:
            return gw_cli_arg_help(cli, argc > 0, NULL);
        }
    }
    if(1 ==  argc)
    {
    	if(strcmp(argv[0],"show") == 0)
    	{
			i_ret = call_gwdonu_if_api(LIB_IF_THREAD_INFO_GET, 1,&i_fd);
			if(i_ret != CLI_OK)
			{
				return CLI_ERROR;
			}
			gw_cli_print(cli,"%-16s %-15s %-8s %-8s %-8s %-8s\r\n","THREAD","ID","PPID","PID","PRI","STACK");
			gw_cli_print(cli,"---------------------------------------------------------------------------------------------------");
			for(i_count = 0; i_count < GW_OSAL_MAX_THREAD;i_count++)
			{
				if(gw_osal_thread_table[i_count].free == FALSE)
				{
					gw_cli_print(cli,"%-16s %-15d %-9d %-9d %-9d %-9d", gw_osal_thread_table[i_count].name, gw_osal_thread_table[i_count].id,
															  gw_osal_thread_table[i_count].ppid, gw_osal_thread_table[i_count].pid,
															  gw_osal_thread_table[i_count].priority, gw_osal_thread_table[i_count].stack_size);
				}
			}
			while(1)
			{
				memset(thread_info_buf,0,LOGLEN);
				i_readnum = read(i_fd,thread_info_buf,(LOGLEN-1));
				if(i_readnum == 0)
				{
					gw_cli_print(cli,"---------------------------------------------------------------------------------------------------");
					sleep(1);
					break;
				}
				if(i_readnum < 0)
				{
					gw_cli_print(cli,"--------------------please_try_agine_print_oam_log_info----------------------");
					sleep(1);
					break;
				}
				thread_info_buf[LOGLEN-1]='\0';
				sleep(1);
				gw_cli_print(cli,"%s",thread_info_buf);
			}
			close(i_fd);
    	}
    	else
    	{
    		gw_cli_print(cli,"%% input error\r\n");
    	}
    }
	return i_ret;
}
#endif
#ifdef CYG_LINUX

gw_uint32 gw_memory_usage()
{
    extern struct mallinfo mallinfo( void );
    struct mallinfo minfo;
    minfo = mallinfo();

    return (gw_uint32)(((minfo.arena - minfo.maxfree) *100)/minfo.arena);
}

#if 1
extern int tolower( int );
extern void (*_putc)(char c, void **param);
extern int _vprintf(void (*putc)(char c, void **param), void **param, const char *fmt, va_list ap);
extern bool mon_read_char_timeout(char *c);

#endif

void halt_do_help()
{
    gw_printf("\r\n%-5s- %-48s","d","display halt string");
    gw_printf("\r\n%-5s- %-48s","q","go on...");
    gw_printf("\r\n%-5s- %-48s","r","reset system");
}


#if 0

gw_int32 halt_flag = 0;
gw_int32 is_halt = 0;
extern cyg_bool imst_getc_nonblock(cyg_uint8* ch);
gw_int32 gw_halt(gw_int8 *String,...)
{
    va_list ap;
    //int ret;
    char c;
    gw_int8 *halt_prompt = "System-Halt->";
    char reason[256];

    if(halt_flag)
        return 1;

    is_halt = 1;
    gw_printf("\r\n***************************************");
    gw_printf("\r\n*                                     *");
    gw_printf("\r\n*         WARNING:System HALT!        *");
    gw_printf("\r\n*                                     *");
    gw_printf("\r\n***************************************");
    gw_printf("\r\n\r\nHalt Reason: ");
    memset(reason , 0 ,sizeof(reason));
    va_start(ap, String);
    vsprintf(reason , String, ap);
    va_end(ap);
    gw_printf("%s",reason);
    gw_printf("\n%s", halt_prompt);
    while (1)
    {
        c = 0;
        #if 0
        if(!mon_read_char_timeout(&c))
            continue;
        #else
        if(!imst_getc_nonblock(&c))
            continue;
        #endif

        switch(tolower(c))
        {
            case 'd':
                gw_printf("\r\n\r\nHalt Reason: ");
                gw_printf("%s\n",reason);
                break;
            case 'r':
                HAL_PLATFORM_RESET();
                break;
            case 'q':
                is_halt = 0;
                return 1;
            default:
                halt_do_help();
                break;
        }

        gw_printf("\n%s", halt_prompt);
    }

    return 1;
}
#endif

#endif

