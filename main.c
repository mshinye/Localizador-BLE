#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringSerial.h>
#include <stdlib.h>
#include "distance_rssi.h"
#include <mysql/mysql.h>
#include <time.h>
#define	WAITING_TIME 1000
#define SAMPLE 5

int fd;
int flag1=0,flag2=0;
double DistCache1[5];
double DistCache2[5];

double  median(int n, double x[]) {
	double temp;
	int i, j;
	// the following two loops sort the array x in ascending order
	for(i=0; i<n-1; i++) {
		for(j=i+1; j<n; j++) {
			if(x[j] < x[i]) {
				// swap elements
				temp = x[i];
				x[i] = x[j];
				x[j] = temp;
			}
		}
	}

	if(n%2==0) {
		// if there is an even number of elements, return mean of the two elements in the middle
		return((x[n/2] + x[n/2 - 1]) / 2.0);
	} else {
		// else return the element in the middle
		return x[n/2];
	}
}

int countSubstr(char substr[10],char strtst[1000])
{
	int count;
	char *p=strtst;

	while (p = strstr(p, substr))
	{
		count++;
		p++;
	}

	// printf("\nFound %d occurrences \n",count);
	return count;   
}

char* getstr(char* buff)
{
	int i = 0;
	int j = 0;

	//serialFlush(fd);

	while(!serialDataAvail(fd) && i<WAITING_TIME)
	{
		i++;
		delay(10);
	}

	if (i== WAITING_TIME)
	{
		printf("Sem dados no serial\n");
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

   // printf("\n%s\n", buff); //debug

    return(buff);
}
int  insert_update()
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
    serialPuts (fd,"AT+DISI?");
    getstr(check);
    delay(10);
    getstr(check);
    int QtdDisp=countSubstr("OK+DISC:",check);
    //printf("\nTamanho total: %d\n",strlen(check));
    serialFlush (fd) ;

    int  Rssi[QtdDisp], MeasPower[QtdDisp];
    char MacAdr[QtdDisp][15];
    char Rssi_char[QtdDisp][6];
    char *p; 
    char  MeasPower_char[QtdDisp][5];
    int cnt;
    double Dist[QtdDisp];

    for(cnt=0;cnt<QtdDisp;cnt++)
    {
        p=check;
        p=p+58+(cnt*78);
        strncpy(MeasPower_char[cnt],p,2);
        MeasPower_char[cnt][2]='\0';
        p=p+3;
        strncpy(MacAdr[cnt],p,12);
        MacAdr[cnt][12]='\0';
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
        Dist[cnt]=getDistance_RSSI(Rssi[cnt],(MeasPower[cnt]-256));
        //Dist[cnt]=getDistance_RSSI(Rssi[cnt],-59);
    }


    for(cnt=0;cnt<QtdDisp;cnt++)
        printf("\nMAC: %s Meas.Power: %d RSSI: %d Distancia: %lf",MacAdr[cnt],MeasPower[cnt],Rssi[cnt],Dist[cnt]);

    printf("\n");
    for(cnt=0;cnt<QtdDisp;cnt++)
        if(strcmp("88C25532CF72",MacAdr[cnt])==0)
        {
            DistCache1[flag1]=Dist[cnt];
            flag1++;
        }
        else if(strcmp("0CF3EE031CB2",MacAdr[cnt])==0)
        {
            DistCache2[flag2]=Dist[cnt];
            flag2++;
        }


    if(flag1==SAMPLE || flag2==SAMPLE)
    {
        MYSQL con;
        mysql_init(&con);

        if(mysql_real_connect(&con,"192.168.0.25","root","admin","LOCBLE",0,NULL,0)) {
            //printf("\nConectado com sucesso!\n");
        } 
        else
        {
            printf("\nConexao falhou!\n");
        }

        time_t t = time(NULL);
        struct tm tm = *localtime(&t);
        char datetime[30];
        char test[30];

        sprintf(datetime,"%d",tm.tm_year+1900);
        strcat(datetime,"-");
        sprintf(test,"%d",(tm.tm_mon+1));
        strcat(datetime,test);
        sprintf(test,"%d",tm.tm_mday);
        strcat(datetime,"-");
        strcat(datetime,test);
        sprintf(test,"%d",tm.tm_hour);
        strcat(datetime," ");
        strcat(datetime,test);
        sprintf(test,"%d",tm.tm_min);
        strcat(datetime,":");
        strcat(datetime,test);
        sprintf(test,"%d",tm.tm_sec);
        strcat(datetime,":");
        strcat(datetime,test);

        char deviceid='1';

        char query[300];

        if(flag1==SAMPLE)
            //printf("INSERT INTO LOG VALUES (\"%s\",%c,\"%s\",%f)",datetime,deviceid,MacAdr[cnt],median(5,DistCache1));
            sprintf(query,"INSERT INTO LOG VALUES (\"%s\",%c,\"%s\",%f)",datetime,deviceid,"88C25532CF72",median(SAMPLE,DistCache1));

        if(flag2==SAMPLE)
            sprintf(query,"INSERT INTO LOG VALUES (\"%s\",%c,\"%s\",%f)",datetime,deviceid,"0CF3EE031CB2",median(SAMPLE,DistCache2));

        if(mysql_query(&con,query))
        {
            printf("\nErro! %s\n",mysql_error(&con));
        }

        flag1=0;
        flag2=0;
        mysql_close(&con);
        memset(MacAdr,0,QtdDisp);
        printf("\n");
    }
}

int main()
{
    while(1)
    {
        insert_update();
        delay(1000);
    }
}

