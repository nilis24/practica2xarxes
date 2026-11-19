/**************************************************************************/
/*                                                                        */
/* L'aplicació UEB amb sockets TCP/IP                                     */
/* Fitxer aDNSc.c que "implementa" la capa d'aplicació DNS (part          */
/* client), o més ben dit, que encapsula les crides a sistema de la       */
/* part DNS, en unes altres funcions més simples i entendedores: la       */
/* "nova" interfície de la capa DNS, en la part client.                   */
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

/* Definició de constants, p.e., XYZ                                      */

/* #define XYZ       1500                                                 */

/* Declaració de funcions INTERNES, és a dir, funcions que es fan servir  */
/* només en aquest fitxer, p.e., int FuncioInterna(arg1, arg2...);        */
/* Les seves definicions es troben al final del fitxer però les seves     */
/* declaracions es troben a l'inici del fitxer (i així les funcions       */
/* són conegudes des de l'inici fins al final del fitxer).                */

/* int FuncioInterna(arg1, arg2...);                                      */

/* Definició de funcions EXTERNES, és a dir, funcions que es criden des   */
/* d'altres fitxers, p.e., int DNSc_FuncioExterna(arg1, arg2...) { }      */
/* En termes de capes de l'aplicació, aquest conjunt de funcions externes */
/* formen la interfície de la capa DNS, en la part client.                */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/* int DNSc_FuncioExterna(arg1, arg2...)
{
	
} */

/* Donat el nom DNS "NomDNS" obté la corresponent @IP i l'escriu a "IP"   */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "NomDNS" és un "string" de C (vector de chars imprimibles acabat en    */
/* '\0') d'una longitud qualsevol.                                        */
/* "IP" és un "string" de C (vector de chars imprimibles acabat en        */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé;                                                       */
/* -1 si hi ha un error.                                                  */
int DNSc_ResolDNSaIP(const char *NomDNS, char *IP, char *TextRes)
{

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