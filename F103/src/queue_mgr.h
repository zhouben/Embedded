#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef void (* advance_callback)(void* context, void* target_context);
typedef struct _queue_mgr{
    unsigned int capacity;
    unsigned int buffer_ba;
    unsigned int buffer_unit_size;
    unsigned int producer_index;
    unsigned int consumer_index;
    unsigned int context_ba;
    unsigned int context_unit_size;
    advance_callback producer_cb;
    advance_callback consumer_cb;
} QUEUE_MGR;

__inline unsigned int queue_is_empty(QUEUE_MGR *mgr)
{
    return (mgr->producer_index == mgr->consumer_index) ? 1 : 0;
};
__inline unsigned int queue_is_full(QUEUE_MGR *mgr)
{
    return (((mgr->producer_index + 1) % mgr->capacity) == mgr->consumer_index) ? 1 : 0;
}

unsigned int queue_mgr_init(QUEUE_MGR* mgr,
    unsigned int entry_num,
    unsigned int buffer_ba, unsigned int buffer_unit_size,
    unsigned int context_ba, unsigned int context_unit_size,
    advance_callback producer_cb, advance_callback consumer_cb);
/*
 * consumer_index++
 * invoke consumer_cb with context
 */
unsigned int queue_advance_consumer(QUEUE_MGR* mgr, void* context);
/*
 * producer_index++
 * invoke producer_cb with context
 */
unsigned int queue_advance_producer(QUEUE_MGR* mgr, void* context);

/*
 * query information for producer, including current buffer address, size, context
 */
unsigned int query_info_for_producer(QUEUE_MGR* mgr, unsigned int* addr, unsigned int* siz, void** context);
unsigned int query_info_for_consumer(QUEUE_MGR* mgr, unsigned int* addr, unsigned int* siz, void** context);

#endif
