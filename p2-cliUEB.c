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

#include "p2-aUEBc.h"
#include "p2-aDNSc.h"

/* Definició de constants, p.e., XYZ                                      */

/* #define XYZ       1500                                                 */

/* Declaració de funcions INTERNES, és a dir, funcions que es fan servir  */
/* només en aquest fitxer, p.e., int FuncioInterna(arg1, arg2...);        */
/* Les seves definicions es troben al final del fitxer però les seves     */
/* declaracions es troben a l'inici del fitxer (i així les funcions       */
/* són conegudes des de l'inici fins al final del fitxer).                */

/* int FuncioInterna(arg1, arg2...);                                      */

int desferURI(const char *uri, char *esq, char *nom_host, int *port, char *nom_fitx);

int main(int argc,char *argv[])
{
 /* Declaració de variables, p.e., int n;                                 */

 /* Expressions, estructures de control, crides a funcions, etc.          */

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
	
}
