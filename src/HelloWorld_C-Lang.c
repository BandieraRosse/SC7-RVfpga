// #if defined(D_NEXYS_A7)
// #include <bsp_printf.h>
//    #include <bsp_mem_map.h>
//    #include <bsp_version.h>
// #else
//    PRE_COMPILED_MSG("no platform was defined")
// #endif
// #include <psp_api.h>

// #define WAIT_TIME 32*1000*1000 //大约一秒

// void rt_tick_increase(void)
// {
//    printfNexys("tick_increase\n");
// }

// int main(void);
// void entry()
// {
//    uartInit();
   

//    while(1);
//    main();
// }

// int main(void)
// {
//    int i;

//    /* Initialize Uart */
//    uartInit();

//    while(1){
//       /* Print "hello world" message on the serial output (be careful not all the printf formats are supported) */
// printfNexys("hello SC7\n");
//       /* Delay */
//       for (i=0;i<WAIT_TIME;i++){};
//    }

// }