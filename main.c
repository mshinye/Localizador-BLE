#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringSerial.h>

int fd;

char* getstr(char* buff)
{
    int i = 0;
    int j = 0;

    //serialFlush(fd);

    while(!serialDataAvail(fd) && i<1000)
    {
        i++;
        delay(10);
    }

    if (i== 1000)
    {
        printf("sem dados no serial\n");
        buff = "erro";
        return(buff);
    }

    while(serialDataAvail(fd)>0)
    {
        delay(10);
        buff[j] = serialGetchar(fd);
        j++;
    }
    buff[j] = 0;

    printf("%s \n", buff); //debug

    return(buff);
}


int main ()
{
	int b = 0;
	int a = 0;
	int i = 0;

char check[1000] = "\0";

if ((fd = serialOpen ("/dev/ttyAMA0", 9600)) < 0)
{
	fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
	return 1 ;
}

fflush(stdout);
serialPuts (fd,"AT+DISI?"); //serve para acordar o modulo
getstr(check);
delay(500);
getstr(check);
serialFlush (fd) ;
printf("go\n");


//fim do bloco que copia os dados para a string check
  
}  
