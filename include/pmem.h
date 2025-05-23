#ifndef __PMEM_H_
#define __PMEM_H_

#define PGSIZE 8 // bytes per page
#define PAGE_NUM  (16 * 1024)

#define PGROUNDUP(sz) (((sz) + PGSIZE - 1) & ~(PGSIZE - 1))
#define PGROUNDDOWN(a) (((a)) & ~(PGSIZE - 1))

void pmem_init();
void *pmem_alloc_pages(int npages);
void pmem_free_pages(void *ptr, int npages);
void *kmalloc(uint64 size);
void *kcalloc(uint n, uint64 size);
void kfree(void *ptr);
void * kalloc(void);

#endif