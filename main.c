#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringSerial.h>

int main ()
{
	int fd ;
	int b = 0;
	int a = 0;
	int i = 0;
char mac[] = "78A5048C47AF"; //green module mac
char check[100] = "\0";

if ((fd = serialOpen ("/dev/ttyAMA0", 9600)) < 0)
{
	fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
	return 1 ;
}

fflush(stdout);
serialPuts (fd,"AT+START"); //serve para acordar o modulo
delay(500);
serialFlush (fd) ;
printf("go\n");

fflush(stdout);
serialPuts (fd,"AT+DISI?"); //serve para acordar o modulo
delay(500);
serialFlush (fd) ;
printf("go\n");




while(serialDataAvail(fd)==0); //espera a chegada de algum byte na porta serial

//o bloco abaixo recebe as informacoes do buffer e copia para a string check
i=0;
while((serialDataAvail(fd))!=0)
{
	delay(10);
	check[i] = serialGetchar(fd);
	i++;
}
check[i] = 0;
//fim do bloco que copia os dados para a string check

printf("%s \n", check); //debug  
}  