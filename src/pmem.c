#include "print.h"
#include "string.h"
#include "types.h"
#include "pmem.h"
#include <stdbool.h>
#include <stddef.h>

#define ALIGNMENT 8 // 8字节对齐
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

char heap[1024 * 1024]; // 1MB堆空间

typedef struct mem_block
{
    size_t size;
    bool free;
    struct mem_block *next;
} mem_block_t;

struct mem_struct
{
    mem_block_t freelist; // 带哨兵节点的空闲链表
} mem;

uint64 _mem_start, _mem_end;

// 初始化内存管理系统
void pmem_init()
{
    // 初始化堆空间
    _mem_start = (uint64)heap;
    _mem_end = (uint64)heap + sizeof(heap);

    // 初始化空闲链表
    mem_block_t *init_block = (mem_block_t *)heap;
    init_block->size = sizeof(heap) - sizeof(mem_block_t);
    init_block->free = true;
    init_block->next = NULL;

    mem.freelist.next = init_block;
    mem.freelist.size = 0; // 哨兵节点不参与分配

    LOG("PMEM initialized with %dKB heap\n", sizeof(heap) / 1024);
}

// 内存分配核心函数
void *pmem_alloc(size_t size)
{
    if (size == 0)
        return NULL;

    // 计算对齐后的总需求空间（包含块头）
    size_t required_size = ALIGN(size + sizeof(mem_block_t));

    mem_block_t *current = mem.freelist.next;
    mem_block_t *prev = &mem.freelist;

    while (current)
    {
        if (current->free && current->size >= required_size)
        {
            // 找到足够大的空闲块
            if (current->size > required_size + sizeof(mem_block_t))
            {
                // 空间足够大，进行分割
                mem_block_t *new_block = (mem_block_t *)((char *)current + required_size);
                new_block->size = current->size - required_size;
                new_block->free = true;
                new_block->next = current->next;

                current->size = required_size;
                current->next = new_block;
            }

            current->free = false;
            prev->next = current->next;   // 从空闲链表移除
            return (void *)(current + 1); // 返回块头之后的数据区
        }
        prev = current;
        current = current->next;
    }

    // 内存不足
    LOG("Memory allocation failed for size: %d\n", size);
    return NULL;
}

// 内存释放核心函数
void pmem_free(void *ptr)
{
    if (!ptr)
        return;

    // 获取块头指针
    mem_block_t *block = (mem_block_t *)ptr - 1;
    block->free = true;

    // 合并相邻空闲块
    mem_block_t *current = mem.freelist.next;
    mem_block_t *prev = &mem.freelist;

    // 按地址顺序插入到空闲链表
    while (current && current < block)
    {
        prev = current;
        current = current->next;
    }

    block->next = current;
    prev->next = block;

    // 前向合并
    if (prev != &mem.freelist && prev->free)
    {
        if ((char *)prev + sizeof(mem_block_t) + prev->size == (char *)block)
        {
            prev->size += sizeof(mem_block_t) + block->size;
            prev->next = block->next;
            block = prev;
        }
    }

    // 后向合并
    mem_block_t *next_block = block->next;
    if (next_block && next_block->free)
    {
        if ((char *)block + sizeof(mem_block_t) + block->size == (char *)next_block)
        {
            block->size += sizeof(mem_block_t) + next_block->size;
            block->next = next_block->next;
        }
    }
}

// 分配内存接口
void *kmalloc(uint64 size)
{
    void *ptr = pmem_alloc(size);
    if (!ptr)
    {
        LOG("kmalloc failed for size: %d\n", size);
    }
    return ptr;
}

// 释放内存接口
void kfree(void *ptr)
{
    pmem_free(ptr);
}