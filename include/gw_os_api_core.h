

#ifndef _GW_OS_API_CORE_H_
#define _GW_OS_API_CORE_H_

#include "gw_types.h"
#include "stdarg.h"   /* for va_list */
#include "string.h"
#include "stdlib.h"
#include "stdio.h"


/* Define constants for semaphore */
#define GW_OSAL_SEM_FULL               (1)
#define GW_OSAL_SEM_EMPTY              (0)

/* define thread stack size class*/
#define GW_OSAL_THREAD_STACK_SIZE_TINY     (4*1024)
#define GW_OSAL_THREAD_STACK_SIZE_NORMAL	(8*1024)
#define GW_OSAL_THREAD_STACK_SIZE_HUGE		(16*1024)

/*define thread priority class*/
#ifdef CYG_LINUX
#define GW_OSAL_THREAD_PRIO_NORMAL		(10)
#define GW_OSAL_THREAD_PRIO_HIGH		(6)
#define GW_OSAL_THREAD_PRIO_LOW			(15)
#else
#define GW_OSAL_THREAD_PRIO_NORMAL      (100)
#define GW_OSAL_THREAD_PRIO_HIGH        (50)
#define GW_OSAL_THREAD_PRIO_LOW         (150)
#endif


/* Defines for Semaphore, Queue Timeout parameters */
#define GW_OSAL_NO_WAIT          (0)
#define GW_OSAL_WAIT_FOREVER     (-1)

#define GW_OSAL_INVALID_QUEUE  (-1)
#define GW_OSAL_INVALID_THREAD  (-1)

#define osal_printf if(osal_debug) gw_printf
/*  tables for the properties of objects */

/* threads */
typedef struct
{
    gw_int8 name [GW_OSAL_MAX_API_NAME];
    gw_uint32 creator;
    gw_uint32 stack_size;
    gw_uint32 priority;
    gw_uint32 os_thread_id;
}osal_thread_prop_t;

/* Counting Semaphores */
typedef struct
{
    gw_int8 name [GW_OSAL_MAX_API_NAME];
    gw_uint32 creator;
}osal_count_sem_prop_t;

/* Mutexes */
typedef struct
{
    gw_int8 name [GW_OSAL_MAX_API_NAME];
    gw_uint32 creator;
}osal_mut_prop_t;


/* struct for gw_local_time_get() */
typedef struct
{
    gw_uint32 seconds;
    gw_uint32 microsecs;
}osal_time_t;


/* This typedef is for the gw_err_name_get function, to ensure
 * everyone is making an array of the same length */
typedef gw_int8 osal_err_name_t[35];


/**************************************************************************
                                                        Initialization of OSAL API
**************************************************************************/
void gw_osal_core_init(
                    void
                    );

/**************************************************************************
                                                        Thread API
**************************************************************************/
gw_int32 gw_thread_create(
                    gw_uint32        *thread_id,
                    const gw_int8   *thread_name,      /* Thread name's length must be less than 20-bytes*/
                    const void       *function_pointer, /*Entry of the thread you create*/
                    void                *param,                  /*parameter required by function_pointer*/
                    gw_uint32       stack_size,              /* Stack size */
                    gw_uint32       priority,                   /* Range 0~255 , 0 is the highest priority , 255 is the lowest priority*/
                    gw_uint32       flags                        /* Don't support , you can ignore it */
                    );

gw_int32 gw_thread_delete(gw_uint32 thread_id);

gw_int32 gw_thread_delay(
                    gw_uint32 milli_second                 /* in millisecond , For Ecos and Linux , the minimum value should be 10,*/
                    );                                                   /* for Vxworks , the minimum value should be 17    */

gw_uint32 gw_thread_number();

void gw_thread_show();


/**************************************************************************
                                                    Semaphore API
**************************************************************************/
gw_int32 gw_semaphore_init(
                    gw_uint32       *sem_id,                /* pointer of semaphore */
                    const gw_int8 *sem_name,          /* semaphore's name */
                    gw_uint32       sem_initial_value,/* counter semaphore's count */
                    gw_uint32       options                  /* Don't support , you can ignore it now */
                    );
gw_int32 gw_semaphore_destroy(
                    gw_uint32 sem_id
                    );
gw_int32 gw_semaphore_post(
                    gw_uint32 sem_id
                    );
gw_int32 gw_semaphore_wait(
                    gw_uint32 sem_id,
                    gw_int32 timeout                   /* Three types value : GW_OSAL_NO_WAIT  , GW_OSAL_WAIT_FOREVER or milliseconds */
                    );
gw_uint32 gw_semaphore_number();


/**************************************************************************
                                                    Mutex API
**************************************************************************/
gw_int32 gw_mutex_init(
                    gw_uint32 *mut_id,              /* Pointer of mutex */
                    const gw_int8 *mut_name,  /* Mutex's name */
                    gw_uint32 options                /* Don't support , you can ignore it now */
                    );
gw_int32 gw_mutex_destroy(
                    gw_uint32 mut_id
                    );
gw_int32 gw_mutex_unlock(
                    gw_uint32 mut_id
                    );
gw_int32 gw_mutex_lock(
                    gw_uint32 mut_id
                    );
gw_int32 gw_mutex_trylock(
                    gw_uint32 mut_id
                    );
gw_uint32 gw_mutex_number();

/**************************************************************************
                                                    Queue API
**************************************************************************/
gw_int32 gw_queue_create(
                        gw_uint32 *queue_id,                /* Pointer of queue_id */
                        const gw_int8 *queue_name,    /* Queue's name */
                        gw_uint32 queue_depth,           /* Max message can be put into the queue */
                        gw_uint32 data_size,                /* The max length of each message */
                        gw_uint32 flags);                       /* Don't support , you can ignore it now */
gw_int32 gw_queue_delete(
                        gw_uint32 queue_id
                        );
gw_int32 gw_queue_get(
                        gw_uint32 queue_id,
                        void *data,                               /* Message in the queue will be copied to the buffer which pointer is 'data',
                                                                         so you must insure for the buffer's length , the buffer's lenght must equal or
                                                                         greater than the length of the message which has been put in the queue. */
                        gw_uint32 size,                         /* data's size , It must equal or greater than '*size_copied' */
                        gw_uint32 *size_copied,          /* the actual lenght  which have been copied to the buffer 'data' */
                        gw_int32 timeout                      /* The maximum interval you can wait , if there is no message in the queue. */
                                                                        /* Three types value : GW_OSAL_NO_WAIT  , GW_OSAL_WAIT_FOREVER or milliseconds */
                        );
gw_int32 gw_queue_put(
                        gw_uint32 queue_id,
                        void *data,                             /* data which will be copied to the queue */
                        gw_uint32 size,                       /* size of data , it must equal or less than the queue's size*/
                        gw_int32 timeout,                   /* Maximum interval you can wait , if the queue is full */
                        gw_uint32 flags                      /* Don't support , you can ignore it */
                        );
gw_int32 gw_queue_count(
                        gw_uint32 queue_id               /* Get the total message in the queue */
                        );
void gw_queue_show(gw_uint32 queue_id);
gw_uint32 gw_queue_number();
gw_int32 gw_queue_type_get(gw_uint32 queue_id);

gw_int32 gw_pri_queue_create (
                        gw_uint32 *queue_id,
                        const gw_int8 *queue_name,
                        gw_uint32 queue_depth,
                        gw_uint32 data_size,
                        gw_int32 pri_num
                        );
gw_int32 gw_pri_queue_put (
                        gw_uint32 queue_id,
                        void *data,
                        gw_uint32 size,
                        gw_int32 timeout,
                        gw_int32 priority
                        );
gw_int32 gw_pri_queue_get (
                        gw_uint32 queue_id,
                        void *data,
                        gw_uint32 size,
                        gw_uint32 *size_copied,
                        gw_int32 timeout
                        );
gw_uint32 gw_pri_queue_number();
gw_int32 gw_pri_queue_count (gw_uint32 queue_id);
gw_int32 gw_pri_queue_max_priority(gw_uint32 queue_id);
void gw_pri_queue_show(gw_uint32 queue_id);


/**************************************************************************
                                                   gw_current_time
**************************************************************************/
gw_uint64 gw_current_time(void);
void gw_timestamp_print();


/**************************************************************************
                                                   OS Time/Tick related API
**************************************************************************/
gw_int32 gw_milli_to_ticks(
                        gw_uint32 milli_seconds
                        );
gw_int32 gw_tick_to_micros(
                        void
                        );
gw_int32 gw_local_time_get(
                        osal_time_t *time_struct
                        );
gw_int32 gw_local_time_set(
                        osal_time_t *time_struct
                        );


/**************************************************************************
                                                  API for a useful debugging function
**************************************************************************/
gw_int32 gw_err_name_get(
                        gw_int32 error_num,
                        osal_err_name_t* err_name
                        );


/**************************************************************************
                                             Abstraction for printf statements
**************************************************************************/
void gw_printf(const gw_int8 *string, ...);

gw_uint32 gw_memory_usage();

gw_int32 gw_halt(gw_int8 *String,...);

#ifndef diag_vprintf
#define diag_vprintf vprintf
#endif

#ifndef diag_vsprintf
#define diag_vsprintf vsprintf
#endif

#ifndef diag_printf
#define diag_printf printf
#endif

#endif
