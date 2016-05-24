#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "libfahw.h"

#define DRIVER_MODULE "matrix_gpio_int"

static struct sensor dev[] = {
        {
                GPIO_PIN(7),
                IRQ_TYPE_EDGE_FALLING,
        }
};
int devFD = -1;

void intHandler(int signNum)
{
    if (signNum == SIGINT) {
        printf("Quit waiting\n");
        sensorDeinit(devFD);
        system("rmmod "DRIVER_MODULE);
    }
    exit(0);
}

int main(int argc, char ** argv)
{
    int i, board;
    int retSize = -1;
    char value[ARRAY_SIZE(dev)];

    if ((board = boardInit()) < 0)
        printf("Fail to init board\n");
    
    if (argc == 2) {
        dev[0].pin = atoi(argv[1]);
    }
    printf("Using GPIO_PIN(%d)\n", dev[0].pin);
    system("modprobe "DRIVER_MODULE);
    signal(SIGINT, intHandler);
    if (board == BOARD_NANOPI_T2)
        dev[0].pin = GPIO_PIN(15);
    if ((devFD =sensorInit(dev, ARRAY_SIZE(dev))) == -1) {
        printf("Fail to init sensor\n");
        return -1;
    }
    printf("Waiting event...\n");
    if ((retSize = sensorRead(devFD, value, ARRAY_SIZE(dev))) == -1) {
        printf("Fail to read sensors\n");
    }
    if (retSize > 0) {
        i = 0;
        for (i=0; i<retSize; i++) {
            printf("dev[%d] value: %d\n", i, value[i]);
        }
    }
    sensorDeinit(devFD);
    system("rmmod "DRIVER_MODULE);
    return 0;
}