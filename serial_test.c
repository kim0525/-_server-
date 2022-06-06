#include <stdio.h>
#include <errno.h>
#include <wiringSerial.h>

int main(void)
{
    int fd;
    int data;

    fd = serialOpen("/dev/ttyS0", 115200);

    if(fd < 0){
        fprintf(stderr, "Failed to open serial device: %s\n", strerror(errno));
    }
    printf("\nRaspberry Pi UART Test\n");

    while(1){
        data = serialGetchar(fd);
        printf ("\nRecv Data: %c", (char)data);
        serialPutchar(fd, data);
        serialPuts(fd, "\n");
        fflush(stdout);
    }



    return 0;

}