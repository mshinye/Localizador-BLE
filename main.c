#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringSerial.h>
#include <stdlib.h>

int fd;

int countSubstr(char substr[10],char strtst[1000])
{
    int count;
    char *p=strtst;

    while (p = strstr(p, substr))
    {
        count++;
	p++;
    }

    printf("\nFound %d occurrences \n",count);
    return count;   
}

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

    printf("\n%s\n", buff); //debug

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
int QtdDisp=countSubstr("OK+DISC:",check);
printf("\nTamanho total: %d\n",strlen(check));
serialFlush (fd) ;

int  Rssi[QtdDisp], MeasPower[QtdDisp];
char MacAdr[QtdDisp][15];
char Rssi_char[QtdDisp][6];
char *p; 
char  MeasPower_char[QtdDisp][5];
int cnt;

for(cnt=0;cnt<QtdDisp;cnt++)
{
	p=check;
	p=p+58+(cnt*78);
	strncpy(MeasPower_char[cnt],p,2);
	MeasPower_char[cnt][2]='\0';
	p=p+3;
	strncpy(MacAdr[cnt],p,12);
	MacAdr[cnt][13]='\0';
	p=p+14;
	strncpy(Rssi_char[cnt],p,4);
	Rssi_char[cnt][3]='\0';
}

//for(cnt=0;cnt<QtdDisp;cnt++)
	//printf("\nMAC: %s Meas.Power: %s RSSI: %s",MacAdr[cnt],MeasPower_char[cnt],Rssi_char[cnt]);

for(cnt=0;cnt<QtdDisp;cnt++)
{
	MeasPower[cnt]=strtol(MeasPower_char[cnt],&p,16);
	//printf("\nMeas Power Decimal %d",MeasPower[cnt]);
	Rssi[cnt]=atoi(Rssi_char[cnt])*-1;
	//printf("\nRSSI %d",Rssi[cnt]);
}

for(cnt=0;cnt<QtdDisp;cnt++)
	printf("\nMAC: %s Meas.Power: %d RSSI: %d",MacAdr[cnt],MeasPower[cnt],Rssi[cnt]);


printf("\n");
}
