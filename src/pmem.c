#include "print.h"
#include "string.h"
#include "types.h"
#include "pmem.h"
#include <stdbool.h>
#include <stddef.h>

// 内存对齐设置为 8 字节对齐
#define ALIGNMENT 8
// 宏定义：将 size 对齐到 ALIGNMENT 的整数倍
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))

// 静态堆内存池大小为 512KB
char heap[512 * 1024]; // 512KB 堆空间

// 内存块结构体，用于管理每个分配的内存块
typedef struct mem_block {
    size_t size;        // 块大小（不含下一个指针）
    bool free;          // 是否空闲
    struct mem_block *next; // 下一个块
} mem_block_t;

// 内存系统主结构，包含空闲链表
struct mem_struct {
    mem_block_t freelist; // 带哨兵节点的空闲链表
} mem;

// 记录堆起始和结束地址
uint64 _mem_start, _mem_end;

/**
 * @brief 初始化内存管理系统
 *
 * 设置堆的起始/结束地址，并初始化空闲链表。
 */
void pmem_init() {
    // 初始化堆空间地址范围
    _mem_start = (uint64)heap;
    _mem_end = (uint64)heap + sizeof(heap);

    // 初始化初始空闲块
    mem_block_t *init_block = (mem_block_t *)heap;
    init_block->size = sizeof(heap) - sizeof(mem_block_t); // 可用大小减去结构体头
    init_block->free = true;
    init_block->next = NULL;

    // 设置空闲链表的哨兵节点
    mem.freelist.next = init_block;
    mem.freelist.size = 0; // 哨兵节点不参与实际分配

    LOG("PMEM initialized with %dKB heap\n", sizeof(heap) / 1024);
}

/**
 * @brief 核心内存分配函数
 *
 * @param size 请求分配的字节数
 * @return     分配成功的内存指针，失败返回 NULL
 */
// 修改后的 pmem_alloc 函数关键部分
void *pmem_alloc(size_t size) {
    if (size == 0)
        return NULL;

    // 对齐用户请求的size，并计算所需总空间（用户数据 + 块头）
    size_t aligned_size = ALIGN(size);
    size_t required_total_size = aligned_size + sizeof(mem_block_t);

    mem_block_t *current = mem.freelist.next;
    mem_block_t *prev = &mem.freelist;

    while (current) {
        // 当前块的用户可用空间需 >= 对齐后的用户请求size，并且总空间足够
        if (current->free && current->size >= aligned_size &&
            (current->size + sizeof(mem_block_t)) >= required_total_size) {
            
            // 计算实际需要分割的总空间
            size_t allocate_total_size = required_total_size;
            
            // 分割块的条件：剩余空间足够容纳块头+最小数据（例如ALIGNMENT）
            if (current->size > aligned_size + sizeof(mem_block_t) + ALIGNMENT) {
                allocate_total_size = ALIGN(aligned_size + sizeof(mem_block_t));
                
                mem_block_t *new_block = (mem_block_t *)((char *)current + allocate_total_size);
                new_block->size = current->size - allocate_total_size;
                new_block->free = true;
                new_block->next = current->next;

                current->size = allocate_total_size - sizeof(mem_block_t); // 用户可用空间为分配的总空间 - 块头
                current->next = new_block;
            } else {
                // 不分割，全部分配
                allocate_total_size = current->size + sizeof(mem_block_t);
            }

            current->free = false;
            prev->next = current->next; // 从空闲链表移除
            return (void *)(current + 1);
        }
        prev = current;
        current = current->next;
    }

    LOG("Memory allocation failed for size: %d\n", size);
    return NULL;
}

/**
 * @brief 核心内存释放函数
 *
 * @param ptr 要释放的内存指针
 */
void pmem_free(void *ptr) {
    if (!ptr)
        return;

    // 获取块头地址
    mem_block_t *block = (mem_block_t *)ptr - 1;

    block->free = true;

    mem_block_t *current = mem.freelist.next;
    mem_block_t *prev = &mem.freelist;

    // 按照地址顺序插入到空闲链表中
    while (current && current < block) {
        prev = current;
        current = current->next;
    }

    block->next = current;
    prev->next = block;

    // 合并前向相邻块
    if (prev != &mem.freelist && prev->free) {
        if ((char *)prev + sizeof(mem_block_t) + prev->size == (char *)block) {
            prev->size += sizeof(mem_block_t) + block->size;
            prev->next = block->next;
            block = prev;
        }
    }

    // 合并后向相邻块
    mem_block_t *next_block = block->next;
    if (next_block && next_block->free) {
        if ((char *)block + sizeof(mem_block_t) + block->size == (char *)next_block) {
            block->size += sizeof(mem_block_t) + next_block->size;
            block->next = next_block->next;
        }
    }
}

/**
 * @brief 外部接口：分配内存
 *
 * @param size 请求分配的大小
 * @return     成功返回内存指针，失败记录日志
 */
void *kmalloc(uint64 size) {
    void *ptr = pmem_alloc(size);
    if (!ptr) {
        LOG("kmalloc failed for size: %d\n", size);
    }
    return ptr;
}

/**
 * @brief 外部接口：释放内存
 *
 * @param ptr 要释放的内存指针
 */
void kfree(void *ptr) {
    pmem_free(ptr);
}