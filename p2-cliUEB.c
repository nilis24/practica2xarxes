/**************************************************************************/
/*                                                                        */
/* L'aplicació UEB amb sockets TCP/IP                                     */
/* Fitxer cliUEB.c que implementa la interfície aplicació-usuari          */
/* d'un client de l'aplicació UEB, sobre la capa d'aplicació de           */
/* (la part client de) UEB (fent crides a la interfície de la part        */
/* client de la capa UEB).                                                */
/*                                                                        */
/* Autors:                                                                */
/* Data:                                                                  */
/*                                                                        */
/**************************************************************************/

/* Inclusió de llibreries, p.e. #include <stdio.h> o #include "meu.h"     */
/*  (si les funcions externes es cridessin entre elles, faria falta fer   */
/*   un #include del propi fitxer capçalera)                              */

/* #include <stdio.h>                                                     */
/* #include "meu.h"                                                       */

#include <stdio.h>      /* printf, scanf, sscanf                         */
#include <stdlib.h>     /* atoi, exit                                    */
#include <string.h>     /* strcpy, strcmp, strlen                        */
#include <unistd.h>     /* write, close                                  */
#include <fcntl.h>      /* open, O_CREAT, O_WRONLY, O_TRUNC              */
#include <sys/stat.h>   /* S_IRUSR, S_IWUSR, S_IRGRP, S_IROTH            */
#include "p2-aUEBc.h"
#include "p2-aDNSc.h"

/* Definició de constants, p.e., XYZ                                      */

#define PORT_TIPIC 4567

/* Declaració de funcions INTERNES, és a dir, funcions que es fan servir  */
/* només en aquest fitxer, p.e., int FuncioInterna(arg1, arg2...);        */
/* Les seves definicions es troben al final del fitxer però les seves     */
/* declaracions es troben a l'inici del fitxer (i així les funcions       */
/* són conegudes des de l'inici fins al final del fitxer).                */

/* int FuncioInterna(arg1, arg2...);                                      */
int desferURI(const char *uri, char *esq, char *nom_host, int *port, char *nom_fitx);

int main(int argc,char *argv[])
{
 char uri[100];
 char esquema[100], nom_host[100], nom_fitx[100];
 int port;
 int n;
 char opcio[20];
 char IPser[16];
 char IPser_actual[16] = "";
 int port_actual = 0;
 int SckCon = -1;
 char TextRes[200];
 char fitxer[10000];
 int long_fitxer;
 char IPloc[16], IPrem[16];
 int portloc, portrem;
 int file;
 char nomfitxer[200];
 
 do {
    printf("\n");
    printf("obtenir o sortir? ");
    scanf("%s", opcio);
    if(strcmp(opcio, "obtenir")==0) {
        /* Es demana un URI */
        printf("\n");
        printf("URI: ");
        scanf("%s",uri);
 
        /* Es desfà l'URI, mètode 1 */ 
        n = desferURI(uri,esquema,nom_host,&port,nom_fitx);
        if (n == -1) {
            printf("Error: l'URI no és vàlid. Format: pueb://[nomDNS o @IP]:[#portTCP o res]/[path del fitxer]\n");
            continue;
        }

        if(port==0) {
            port = PORT_TIPIC;
        }

        strcpy(IPser, nom_host);

        /* Si hi ha connexió oberta però amb un altre S, es tanca */
        if(SckCon != -1 && (strcmp(IPser, IPser_actual) != 0 || port != port_actual)) {
            if(UEBc_TancaConnexio(SckCon, TextRes) == -1) {
                printf("Error: %s\n", TextRes);
            }
            SckCon = -1;
        }

        /* Si no hi ha connexió oberta, se'n demana una de nova */
        if(SckCon == -1) {
            SckCon = UEBc_DemanaConnexio(IPser, port, TextRes);
            if(SckCon == -1) {
                printf("Error: %s\n", TextRes);
                continue;
            }
            strcpy(IPser_actual, IPser);
            port_actual = port;
        }

        /* 6a) Es mostra la informació de la petició i dels sockets */
        printf("obtenir %s\n", uri);
        printf("S: %s:%d, fitxer: %s\n", IPser, port, nom_fitx);
        if(UEBc_TrobaAdrSckConnexio(SckCon, IPloc, &portloc, IPrem, &portrem, TextRes) == -1) {
            printf("Error: %s\n", TextRes);
        } else {
            printf("Sockets: C %s:%d  S %s:%d\n", IPloc, portloc, IPrem, portrem);
        }

        /* 6b) Es demana el fitxer al S */
        n = UEBc_ObteFitxer(SckCon, nom_fitx, fitxer, &long_fitxer, TextRes);
        if(n == 0) {
            /* Fitxer rebut: s'aboca a pantalla (pot no acabar en '\0') */
            printf("%s\n", TextRes);
            fflush(stdout);   /* que surti abans el text del printf que el write */
            write(1, fitxer, long_fitxer);
            printf("\n");
            /* 6c) Es desa el fitxer rebut a disc, amb el mateix nom però sense */
            /* la "/" inicial ("/" és la carpeta on s'executa el C)           */
            strcpy(nomfitxer, nom_fitx + 1);
            file = open(nomfitxer, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            /* 2n argument: obrir un fitxer per escriure-hi, que si no existeix es creï, i que si */
            /* existeix es trunqui a 0 bytes (s'esborri); 3r argument: permisos 644, de fet 00644 */
            if(file == -1) {
                printf("Error: no s'ha pogut desar el fitxer %s\n", nomfitxer);
            } else {
                n = write(file, fitxer, long_fitxer);
                if(n == -1) {
                    printf("Error: no s'ha pogut escriure el fitxer %s\n", nomfitxer);
                } else {
                    printf("Fitxer desat a %s\n", nomfitxer);
                }
                close(file);
            }
        } else if(n == 1) {
            /* El S ha respost ERR (p.e., el fitxer no existeix) */
            printf("Resposta ERR del servidor: %s\n", TextRes);
        } else {
            /* -1, -2 o -3: la connexió ja no és fiable, es tanca */
            printf("Error: %s\n", TextRes);
            UEBc_TancaConnexio(SckCon, TextRes);
            SckCon = -1;
        }
    }
 } while (strcmp(opcio, "sortir") != 0);

 if(SckCon != -1) {
    if(UEBc_TancaConnexio(SckCon, TextRes) == -1) {
        printf("Error: %s\n", TextRes);
    }
 }
 
 return 0;
}

/* Definició de funcions INTERNES, és a dir, funcions que es fan servir   */
/* només en aquest fitxer, p.e., int FuncioInterna(arg1, arg2...) { }     */
/* Les seves definicions es troben al final del fitxer però les seves     */
/* declaracions es troben a l'inici del fitxer (i així les funcions       */
/* són conegudes des de l'inici fins al final del fitxer).                */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/* int FuncioInterna(arg1, arg2...)
{
	
} */

/* Desfà l'URI "uri" en les seves parts: l'esquema (protocol) "esq", el   */
/* nom DNS (o l'@IP) "nom_host", el número de port "port" i el nom del    */
/* fitxer "nom_fitxer".                                                   */
/*                                                                        */
/* L'URI ha de tenir la forma "esq://nom_host:port/nom_fitxer" o bé       */
/* sense el número de port "esq://nom_host/nom_fitxer", i llavors port    */
/* s'emplena amb el valor 0.                                              */
/*                                                                        */
/* "uri", "esq", "nom_host" i "nom_fitxer" són "strings" de C (vector de  */
/* chars imprimibles acabat en '\0') d'una longitud suficient.            */
/*                                                                        */
/* Retorna:                                                               */
/*  el nombre de parts de l'URI que s'han assignat, 4 si l'URI tenia      */
/*  número de port o 3 si no en tenia.                                    */
/*  -1 en la resta de casos.                                              */
int desferURI(const char *uri, char *esq, char *nom_host, int *port, char *nom_fitx)
{
 int nassignats;
 char port_str[100];
 
 strcpy(esq,"");
 strcpy(nom_host,"");
 *port = 0;
 strcpy(nom_fitx,"");
 
 /* Es prova una assignació de 4 valors */
 nassignats = sscanf(uri,"%[^:]://%[^:]:%[^/]%s",esq,nom_host,port_str,nom_fitx);

 /* URIs amb #port, p.e., esq://host:port/fitx, tindran 4 valors assignats */
 if(nassignats==4)
 {
  *port = atoi(port_str);
  return nassignats;
 }  
  
 /* URIs sense #port, p.e., esq://host/fitx, tindran 2 valors assignats; */
 /* llavors es fa port = 0 i ...                                         */
 if(nassignats==2)
  *port = 0;
 /* ... es prova una nova assignació, ara de 3 valors */
 nassignats = sscanf(uri,"%[^:]://%[^/]%s",esq,nom_host,nom_fitx);
 
 if(nassignats==3) 
  return nassignats;
 
 /* No s'han assignat ni 4 ni 3 parts de l'URI */
 return -1;
}