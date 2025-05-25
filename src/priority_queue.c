#include "print.h"
#include "process.h"
#include "priority_queue.h"
#include <string.h>

// 私有函数声明，用于内部逻辑处理
static void priority_queue_realloc(PriorityQueue *pq);                    // 扩容优先队列
static void priority_queue_adjust_head(PriorityQueue *pq);                // 调整堆顶
static void priority_queue_adjust_tail(PriorityQueue *pq);                // 调整堆尾（插入后上浮）
static int priority_queue_compare(PriorityQueue *pq, int pos1, int pos2); // 比较两个节点的大小，支持最大堆/最小堆
static void priority_queue_swap(KeyValue **nodes, int pos1, int pos2);    // 交换两个节点

// KeyValue 结构体相关函数

/**
 * 创建一个新的 KeyValue 对象
 *
 * @param key   键值
 * @param value 数据指针
 * @return      新创建的 KeyValue 实例
 */
KeyValue *key_value_new(int key, void *value)
{
    KeyValue *pkv = (KeyValue *)kmalloc(sizeof(KeyValue));
    pkv->_key = key;
    pkv->_value = value;
    return pkv;
}

/**
 * 释放一个 KeyValue 实例
 *
 * @param kv         待释放的 KeyValue 实例
 * @param freevalue  自定义内存释放函数，用于释放 value 字段
 */
void key_value_free(KeyValue *kv, void (*freevalue)(void *))
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

// PriorityQueue 结构体相关函数

/**
 * 创建一个新的优先队列
 *
 * @param priority 队列类型（PRIORITY_MIN: 最小堆，PRIORITY_MAX: 最大堆）
 * @return         新创建的优先队列实例
 */
PriorityQueue *priority_queue_new(int priority)
{
    PriorityQueue *pq = (PriorityQueue *)kmalloc(sizeof(PriorityQueue));
    pq->_capacity = 11; // 默认初始容量
    pq->_size = 0;
    pq->_priority = priority;

    pq->_nodes = (KeyValue **)kmalloc(sizeof(KeyValue *) * pq->_capacity);
    return pq;
}

/**
 * 释放优先队列及其所有元素
 *
 * @param pq         待释放的优先队列
 * @param freevalue  自定义 value 内存释放函数
 */
void priority_queue_free(PriorityQueue *pq, void (*freevalue)(void *))
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

/**
 * 获取堆顶元素（不删除）
 *
 * @param pq 优先队列
 * @return   堆顶元素，若为空则返回 NULL
 */
const KeyValue *priority_queue_top(PriorityQueue *pq)
{
    if (pq->_size > 0)
        return pq->_nodes[0];
    return NULL;
}

/**
 * 弹出堆顶元素（并删除）
 *
 * @param pq 优先队列
 * @return   被弹出的 KeyValue 实例
 */
KeyValue *priority_queue_dequeue(PriorityQueue *pq)
{
    KeyValue *pkv = NULL;
    if (pq->_size > 0)
    {
        pkv = pq->_nodes[0];
        priority_queue_adjust_head(pq); // 删除堆顶后调整堆结构
    }
    return pkv;
}

/**
 * 向优先队列中添加一个元素
 *
 * @param pq 优先队列
 * @param kv 待添加的 KeyValue 元素
 */
void priority_queue_enqueue(PriorityQueue *pq, KeyValue *kv)
{
    pq->_nodes[pq->_size] = kv;
    priority_queue_adjust_tail(pq); // 插入后上浮调整
    if (pq->_size >= pq->_capacity)
        priority_queue_realloc(pq); // 容量不足时扩容
}

/**
 * 获取优先队列当前元素个数
 *
 * @param pq 优先队列
 * @return   当前元素数量
 */
int priority_queue_size(PriorityQueue *pq)
{
    return pq->_size;
}

/**
 * 判断优先队列是否为空
 *
 * @param pq 优先队列
 * @return   若为空返回非零值，否则返回 0
 */
int priority_queue_empty(PriorityQueue *pq)
{
    return pq->_size <= 0;
}

/**
 * 打印优先队列内容（调试用）
 *
 * @param pq 优先队列
 */
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

// 私有函数实现

/**
 * 扩容优先队列的存储空间（当容量不足时调用）
 *
 * @param pq 优先队列
 */
static void priority_queue_realloc(PriorityQueue *pq)
{
    const size_t new_capacity = pq->_capacity * 2;
    const size_t new_size = sizeof(KeyValue *) * new_capacity;

    KeyValue **new_nodes = (KeyValue **)kmalloc(new_size);
    if (!new_nodes)
    {
        panic("Memory reallocation failed for priority queue");
    }

    if (pq->_nodes)
    {
        memcpy(new_nodes, pq->_nodes, sizeof(KeyValue *) * pq->_size);
        kfree(pq->_nodes);
    }

    pq->_capacity = new_capacity;
    pq->_nodes = new_nodes;

    memset(pq->_nodes + pq->_size, 0, sizeof(KeyValue *) * (new_capacity - pq->_size));
}

/**
 * 删除堆顶元素后的结构调整（下沉操作）
 *
 * @param pq 优先队列
 */
static void priority_queue_adjust_head(PriorityQueue *pq)
{
    priority_queue_swap(pq->_nodes, 0, pq->_size - 1);
    pq->_size--;

    int i = 0;
    while (1)
    {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smallest = i;

        if (left < pq->_size && priority_queue_compare(pq, left, smallest) < 0)
        {
            smallest = left;
        }

        if (right < pq->_size && priority_queue_compare(pq, right, smallest) < 0)
        {
            smallest = right;
        }

        if (smallest != i)
        {
            priority_queue_swap(pq->_nodes, i, smallest);
            i = smallest;
        }
        else
        {
            break;
        }
    }
}

/**
 * 添加元素后的结构调整（上浮操作）
 *
 * @param pq 优先队列
 */
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

/**
 * 比较两个节点的键值，根据最大堆或最小堆决定比较方向
 *
 * @param pq   优先队列
 * @param pos1 第一个节点位置
 * @param pos2 第二个节点位置
 * @return     比较结果
 */
static int priority_queue_compare(PriorityQueue *pq, int pos1, int pos2)
{
    int adjust = -1;
    int r = pq->_nodes[pos1]->_key - pq->_nodes[pos2]->_key;
    if (pq->_priority == PRIORITY_MAX)
        r *= adjust;
    return r;
}

/**
 * 交换两个节点
 *
 * @param nodes 节点数组
 * @param pos1  第一个节点位置
 * @param pos2  第二个节点位置
 */
static void priority_queue_swap(KeyValue **nodes, int pos1, int pos2)
{
    KeyValue *temp = nodes[pos1];
    nodes[pos1] = nodes[pos2];
    nodes[pos2] = temp;
}