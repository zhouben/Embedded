#include "queue_mgr.h"

unsigned int queue_mgr_init(QUEUE_MGR* mgr,
    unsigned int entry_num,
    unsigned int buffer_ba, unsigned int buffer_unit_size,
    unsigned int context_ba, unsigned int context_unit_size,
    advance_callback producer_cb, advance_callback consumer_cb)
{
    mgr->capacity = entry_num;
    mgr->buffer_ba = buffer_ba;
    mgr->buffer_unit_size = buffer_unit_size;
    mgr->producer_index = 0;
    mgr->consumer_index = 0;
    mgr->context_ba = context_ba;
    mgr->context_unit_size = context_unit_size;
    mgr->producer_cb = producer_cb;
    mgr->consumer_cb = consumer_cb;
    return 0;
}

unsigned int queue_advance_consumer(QUEUE_MGR* mgr, void* context)
{

    if (mgr->consumer_cb)
    {
        mgr->consumer_cb(context, (void*)(mgr->context_ba + (mgr->consumer_index) * mgr->context_unit_size));
    }
    mgr->consumer_index = (++mgr->consumer_index) % mgr->capacity;
    return 0;
}
unsigned int queue_advance_producer(QUEUE_MGR* mgr, void* context)
{
    if (mgr->producer_cb)
    {
        mgr->producer_cb(context, (void*)(mgr->context_ba + mgr->producer_index * mgr->context_unit_size));
    }
    mgr->producer_index = (++mgr->producer_index) % mgr->capacity;
    return 0;
}
unsigned int query_info_for_producer(QUEUE_MGR* mgr, unsigned int* addr, unsigned int* siz, void** context)
{
    if (addr)    *addr = mgr->buffer_ba + mgr->producer_index * mgr->buffer_unit_size;
    if (siz)     *siz = mgr->buffer_unit_size;
    if (context) *context = (void*)(mgr->context_ba + (mgr->producer_index * mgr->context_unit_size));
    return 0;
}
unsigned int query_info_for_consumer(QUEUE_MGR* mgr, unsigned int* addr, unsigned int* siz, void** context)
{
    if (addr)    *addr = mgr->buffer_ba + mgr->consumer_index * mgr->buffer_unit_size;
    if (siz)     *siz = mgr->buffer_unit_size;
    if (context) *context = (void*)(mgr->context_ba + (mgr->consumer_index * mgr->context_unit_size));
    return 0;
}

