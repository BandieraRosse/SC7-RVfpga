#include "print.h"
#include "process.h"
#include "priority_queue.h"
#include <string.h>
// Private Functions
static void priority_queue_realloc(PriorityQueue *pq);
static void priority_queue_adjust_head(PriorityQueue *pq);
static void priority_queue_adjust_tail(PriorityQueue *pq);
static int priority_queue_compare(PriorityQueue *pq,
                                  int pos1,
                                  int pos2);
static void priority_queue_swap(KeyValue **nodes,
                                int pos1,
                                int pos2);
// Functions of KeyValue Struct
KeyValue *key_value_new(int key,
                        void *value)
{
    KeyValue *pkv = (KeyValue *)kmalloc(sizeof(KeyValue));
    pkv->_key = key;
    pkv->_value = value;
    return pkv;
}
void key_value_free(KeyValue *kv,
                    void (*freevalue)(void *))
{
    if (kv)
    {
        if (freevalue)
        {
            freevalue(kv->_value);
        }
        kfree(kv);
    }
}

// Functions of PriorityQueue Struct
PriorityQueue *priority_queue_new(int priority)
{
    PriorityQueue *pq = (PriorityQueue *)kmalloc(sizeof(PriorityQueue));
    pq->_capacity = 11; // default initial value
    pq->_size = 0;
    pq->_priority = priority;

    pq->_nodes = (KeyValue **)kmalloc(sizeof(KeyValue *) * pq->_capacity);
    return pq;
}
void priority_queue_free(PriorityQueue *pq,
                         void (*freevalue)(void *))
{
    int i;
    if (pq)
    {
        for (i = 0; i < pq->_size; ++i)
            key_value_free(pq->_nodes[i], freevalue);
        kfree(pq->_nodes);
        kfree(pq);
    }
}
const KeyValue *priority_queue_top(PriorityQueue *pq)
{
    if (pq->_size > 0)
        return pq->_nodes[0];
    return NULL;
}
KeyValue *priority_queue_dequeue(PriorityQueue *pq)
{
    KeyValue *pkv = NULL;
    if (pq->_size > 0)
    {
        pkv = pq->_nodes[0];
        priority_queue_adjust_head(pq);
    }
    return pkv;
}
void priority_queue_enqueue(PriorityQueue *pq,
                            KeyValue *kv)
{
    printf("add key:%d\n", kv->_key);
    pq->_nodes[pq->_size] = kv;
    priority_queue_adjust_tail(pq);
    if (pq->_size >= pq->_capacity)
        priority_queue_realloc(pq);
}
int priority_queue_size(PriorityQueue *pq)
{
    return pq->_size;
}
int priority_queue_empty(PriorityQueue *pq)
{
    return pq->_size <= 0;
}
void priority_queue_print(PriorityQueue *pq)
{
    int i;
    KeyValue *kv;
    printf("data in the pq->_nodes\n");
    for (i = 0; i < pq->_size; ++i)
        printf("%d ", pq->_nodes[i]->_key);
    printf("\n");

    printf("dequeue all data\n");
    while (!priority_queue_empty(pq))
    {
        kv = priority_queue_dequeue(pq);
        printf("%d ", kv->_key);
    }
    printf("\n");
}
static void priority_queue_realloc(PriorityQueue *pq)
{
    const size_t new_capacity = pq->_capacity * 2;
    const size_t new_size = sizeof(KeyValue *) * new_capacity;

    // 分配新内存（使用自定义内存分配器）
    KeyValue **new_nodes = (KeyValue **)kmalloc(new_size);
    if (!new_nodes)
    {
        panic("Memory reallocation failed for priority queue");
    }

    // 复制旧数据到新内存
    if (pq->_nodes)
    {
        memcpy(new_nodes, pq->_nodes, sizeof(KeyValue *) * pq->_size);

        // 释放旧内存（使用自定义内存释放）
        kfree(pq->_nodes);
    }

    // 更新队列属性
    pq->_capacity = new_capacity;
    pq->_nodes = new_nodes;

    // 初始化新分配的内存（可选）
    memset(pq->_nodes + pq->_size, 0,
           sizeof(KeyValue *) * (new_capacity - pq->_size));
}
static void priority_queue_adjust_head(PriorityQueue *pq)
{
    // 步骤1：交换堆顶和最后一个元素
    priority_queue_swap(pq->_nodes, 0, pq->_size - 1);
    // 步骤2：减小堆大小（相当于删除原堆顶元素）
    pq->_size--;

    // 步骤3：从新堆顶开始下沉调整
    int i = 0;
    while (1)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        // 比较左子节点（注意检查左子节点是否在堆范围内）
        if (left < pq->_size &&
            priority_queue_compare(pq, left, smallest) < 0)
        {
            smallest = left;
        }

        // 比较右子节点（注意检查右子节点是否在堆范围内）
        if (right < pq->_size &&
            priority_queue_compare(pq, right, smallest) < 0)
        {
            smallest = right;
        }

        // 若需要交换则继续下沉
        if (smallest != i)
        {
            priority_queue_swap(pq->_nodes, i, smallest);
            i = smallest;
        }
        else
        {
            break; // 堆性质已满足
        }
    }
}
static void priority_queue_adjust_tail(PriorityQueue *pq)
{
    int i, parent, child;

    i = pq->_size - 1;
    pq->_size++;
    while (i > 0)
    {
        child = i;
        parent = (child - 1) / 2;

        if (priority_queue_compare(pq, parent, child) > 0)
        {
            priority_queue_swap(pq->_nodes, child, parent);
            i = parent;
        }
        else
            break;
    }
}

static int priority_queue_compare(PriorityQueue *pq,
                                  int pos1,
                                  int pos2)
{
    int adjust = -1;
    int r = pq->_nodes[pos1]->_key - pq->_nodes[pos2]->_key;
    if (pq->_priority == PRIORITY_MAX)
        r *= adjust;
    return r;
}
static void priority_queue_swap(KeyValue **nodes,
                                int pos1,
                                int pos2)
{
    KeyValue *temp = nodes[pos1];
    nodes[pos1] = nodes[pos2];
    nodes[pos2] = temp;
}
