#include "analyze.h"

#include "FreeRTOS.h"
#include "task.h"

void printFreeRTOSStats(void) {
    TaskStatus_t *pxTaskStatusArray;
    volatile UBaseType_t uxArraySize, x;
    uint32_t ulTotalRunTime;

    // Take a snapshot of the number of tasks in case it changes while this function is executing.
    uxArraySize = uxTaskGetNumberOfTasks();

    // Allocate a TaskStatus_t structure for each task. 
    pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

    if (pxTaskStatusArray != NULL) {
        // Generate raw status information about each task.
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalRunTime);

        // Print each task's name, state, priority, stack high water mark, and number
        printf("Task\t\tState\tPrio\tStack\tTask#\n");
        for (x = 0; x < uxArraySize; x++) {
            printf("%s\t\t%c\t%u\t%u\t%u\n",
                   pxTaskStatusArray[x].pcTaskName,
                   pxTaskStatusArray[x].eCurrentState,
                   (unsigned int) pxTaskStatusArray[x].uxCurrentPriority,
                   (unsigned int) pxTaskStatusArray[x].usStackHighWaterMark,
                   (unsigned int) pxTaskStatusArray[x].xTaskNumber);
        }

        // The array is no longer needed, free the memory it consumes.
        vPortFree(pxTaskStatusArray);
    }

    // Print the total and free heap memory available.
    printf("Total heap: %u\n", (unsigned int) configTOTAL_HEAP_SIZE);
    printf("Free heap: %u\n", (unsigned int) xPortGetFreeHeapSize());
}
