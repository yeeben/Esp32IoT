#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "sensor_data.h"

#define COMP_LOOP_PERIOD    5000
#define SPIN_ITER   350000  //actual CPU cycles consumed will depend on compiler optimization
#define CORE0       0
// only define xCoreID CORE1 as 1 if this is a multiple core processor target, else define it as tskNO_AFFINITY
#define CORE1       ((CONFIG_FREERTOS_NUMBER_OF_CORES > 1) ? 1 : tskNO_AFFINITY)

const static char *TAG = "create task example";
static volatile bool timed_out;

static void spin_iteration(int spin_iter_num)
{
    for (int i = 0; i < spin_iter_num; i++) {
        __asm__ __volatile__("NOP");
    }
}

static void spin_task(void *arg)
{
    // convert arg pointer from void type to int type then dereference it
    int task_id = (int)arg;
    ESP_LOGI(TAG, "created task#%d", task_id);
    while (!timed_out) {
        spin_iteration(SPIN_ITER);
        vTaskDelay(pdMS_TO_TICKS(150));
    }

    vTaskDelete(NULL);
}

int create_task_entry_func()
{
    timed_out = false;
    // pin 2 tasks on same core and observe in-turn execution,
    // and pin another task on the other core to observe "simultaneous" execution
    int task_id0 = 0, task_id1 = 1, task_id2 = 2, task_id3 = 3;
    xTaskCreatePinnedToCore(spin_task, "pinned_task0_core0", 4096, (void*)task_id0, TASK_PRIO_3, NULL, CORE0);
    xTaskCreatePinnedToCore(spin_task, "pinned_task1_core0", 4096, (void*)task_id1, TASK_PRIO_3, NULL, CORE0);
    xTaskCreatePinnedToCore(spin_task, "pinned_task2_core1", 4096, (void*)task_id2, TASK_PRIO_3, NULL, CORE1);
    // Create a unpinned task with xCoreID = tskNO_AFFINITY, which can be scheduled on any core, hopefully it can be observed that the scheduler moves the task between the different cores according to the workload
    xTaskCreatePinnedToCore(spin_task, "unpinned_task", 4096, (void*)task_id3, TASK_PRIO_2, NULL, tskNO_AFFINITY);

    // time out and stop running after 5 seconds
    vTaskDelay(pdMS_TO_TICKS(COMP_LOOP_PERIOD));
    timed_out = true;
    // delay to let tasks finish the last loop
    vTaskDelay(500 / portTICK_PERIOD_MS);
    return 0;
}