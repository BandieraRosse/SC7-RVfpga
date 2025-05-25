#include "types.h"
#include "test.h"
#include "print.h"
#include "process.h"
#include "string.h"
#include "cpu.h"
#include "figlet.h"
#include "board.h"
#include "priority_queue.h"

#include "bsp_printf.h"
#include "psp_timers_eh1.h"
#include "psp_api.h"

int tid3;
extern char *lk;
extern void exit();

/* 线程 1 的入口函数 */
static void pq_entry()
{
    int i;
    PriorityQueue *pq_min = priority_queue_new(PRIORITY_MIN);

    int a[] = {1, 9, 7, 8, 5, 4, 3, 2, 1, 100, 50, 17};

    for (i = 0; i < sizeof(a) / sizeof(int); ++i)
    {
        KeyValue *kv = key_value_new(a[i], NULL);
        priority_queue_enqueue(pq_min, kv);
    }
    printf("小根堆： ");
    priority_queue_print(pq_min);
    priority_queue_free(pq_min, NULL);

    PriorityQueue *pq_max = priority_queue_new(PRIORITY_MAX);

    int b[] = {1, 9, 7, 8, 5, 4, 3, 2, 1, 100, 50, 17};

    for (i = 0; i < sizeof(b) / sizeof(int); ++i)
    {
        KeyValue *kv = key_value_new(b[i], NULL);
        priority_queue_enqueue(pq_max, kv);
    }
    printf("大根堆： ");
    priority_queue_print(pq_max);
    priority_queue_free(pq_max, NULL);

    exit();
}
void exitApp3()
{
    while (1)
    {
        if (READ_SW() >> 14 == 1)
        {
            deleteproc(tid3);
            PRINT_COLOR(RED_COLOR_PRINT, "------------------------------pq_test end-------------------------------------------\n");
            wakeup(lk);
            delay(1000);
            exit();
        }
        delay(1000);
        yield();
    }
}
int pq_test(void)
{
    PRINT_COLOR(RED_COLOR_PRINT, "------------------------------pq_test start-------------------------------------------\n");
    int tid = sc7_create_process(&exitApp3, 14);
    if (tid != NULL)
        sc7_start_process(tid);
    tid3 = sc7_create_process(&pq_entry, 14);
    if (tid3 != NULL)
        sc7_start_process(tid3);

    return 0;
}
