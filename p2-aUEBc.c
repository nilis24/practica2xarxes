/**************************************************************************/
/*                                                                        */
/* L'aplicació UEB amb sockets TCP/IP                                     */
/* Fitxer aUEBc.c que implementa la capa d'aplicació de UEB, sobre la     */
/* cap de transport TCP (fent crides a la "nova" interfície de la         */
/* capa TCP o "nova" interfície de sockets TCP), en la part client.       */
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

#include "p2-tTCP.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Definició de constants, p.e., XYZ                                      */

/* #define XYZ       1500                                                 */

/* Declaració de funcions INTERNES, és a dir, funcions que es fan servir  */
/* només en aquest fitxer, p.e., int FuncioInterna(arg1, arg2...);        */
/* Les seves definicions es troben al final del fitxer però les seves     */
/* declaracions es troben a l'inici del fitxer (i així les funcions       */
/* són conegudes des de l'inici fins al final del fitxer).                */

/* int FuncioInterna(arg1, arg2...);                                      */

int ConstiEnvMis(int SckCon, const char *tipus, const char *info1, int long1);
int RepiDesconstMis(int SckCon, char *tipus, char *info1, int *long1);
int RepExacte(int Sck, char *buf, int n);

/* Definició de funcions EXTERNES, és a dir, funcions que es criden des   */
/* d'altres fitxers, p.e., int UEBc_FuncioExterna(arg1, arg2...) { }      */
/* En termes de capes de l'aplicació, aquest conjunt de funcions externes */
/* formen la interfície de la capa UEB, en la part client.                */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/* int UEBc_FuncioExterna(arg1, arg2...)
{
	
} */

/* Crea un socket TCP "client" en una @IP i #port TCP local qualsevol, a  */
/* través del qual demana una connexió a un S UEB que escolta peticions   */
/* al socket TCP "servidor" d'@IP "IPser" i #portTCP "portTCPser".        */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "IPser" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud màxima de 16 chars (incloent '\0').               */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  l'identificador del socket TCP connectat si tot va bé;                */
/* -1 si hi ha un error a la interfície de sockets.                       */
int UEBc_DemanaConnexio(const char *IPser, int portTCPser, char *TextRes)
{
	int codi;
	int sck_client = TCP_CreaSockClient("0.0.0.0", 0);
    if(sck_client == -1) {
        sprintf(TextRes, "TCP_CreaSockClient(): %s", T_ObteTextRes(&codi));
        return -1;
    }

    int scon = TCP_DemanaConnexio(sck_client, IPser, portTCPser);
    if(scon == -1) {
        sprintf(TextRes, "TCP_DemanaConnexio(): %s", T_ObteTextRes(&codi));
        TCP_TancaSock(sck_client);
        return -1;
    }

    strcpy(TextRes, "Connexió establerta amb èxit");
    return sck_client;
}

/* Obté el fitxer de nom "NomFitx" del S UEB a través de la connexió TCP  */
/* d'identificador "SckCon". Escriu els bytes del fitxer a "Fitx" i la    */
/* longitud del fitxer en bytes a "LongFitx".                             */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "NomFitx" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud <= 10000 chars (incloent '\0').                   */
/* "Fitx" és un vector de chars (bytes) qualsevol (recordeu que en C,     */
/* un char és un enter de 8 bits) d'una longitud <= 9999 bytes.           */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si el fitxer existeix al servidor;                                  */
/*  1 la petició és ERRònia (p.e., el fitxer no existeix);                */
/* -1 si hi ha un error a la interfície de sockets;                       */
/* -2 si protocol és incorrecte (longitud camps, tipus de peticio, etc.); */
/* -3 si l'altra part tanca la connexió.                                  */
int UEBc_ObteFitxer(int SckCon, const char *NomFitx, char *Fitx, int *LongFitx, char *TextRes)
{
    int codi;
    int res;
    char tipus[4];
    int long_nom = strlen(NomFitx);

    /* --- BLOC 1: validar el nom del fitxer --- */
    if(NomFitx[0] != '/' || long_nom > 9999) {        /* no comença per '/' o long_nom > 9999 */
        strcpy(TextRes, "nom de fitxer incorrecte: ha de començar per '/' i tenir <= 9999 chars");
        return -2;
    }

    /* --- BLOC 2: enviar la petició OBT --- */
    res = ConstiEnvMis(SckCon, "OBT", NomFitx, long_nom);
    if(res == -1) {
        sprintf(TextRes, "ConstiEnvMis(): %s", T_ObteTextRes(&codi));
        return -1;
    }
    if(res == -2) {
        strcpy(TextRes, "ConstiEnvMis(): camps del missatge OBT incorrectes");
        return -2;
    }

    /* --- BLOC 3: rebre la resposta (COR o ERR) --- */
    res = RepiDesconstMis(SckCon, tipus, Fitx, LongFitx);
    if(res == -1) {
        sprintf(TextRes, "RepiDesconstMis(): %s", T_ObteTextRes(&codi));
        return -1;
    }
    if(res == -2) {
        strcpy(TextRes, "RepiDesconstMis(): resposta mal formada (protocol incorrecte)");
        return -2;
    }
    if(res == -3) {
        strcpy(TextRes, "el servidor ha tancat la connexió");
        return -3;
    }

    /* --- BLOC 4: mirar quin tipus de resposta és --- */
    if(strcmp(tipus, "COR") == 0) {
        sprintf(TextRes, "fitxer %s obtingut (%d bytes)", NomFitx, *LongFitx);
        return 0;
    }
    if(strcmp(tipus, "ERR") == 0) {
        /* Fitx conté "codi + espai + text", sense '\0' */
        int n = *LongFitx;
        if(n > 199) n = 199;
        memcpy(TextRes, Fitx, n);
        TextRes[n] = '\0';
        return 1;
    }
    /* si arriba aquí, el tipus no és ni COR ni ERR */
    strcpy(TextRes, "resposta inesperada del servidor (ni COR ni ERR)");
    return -2;
}

/* Tanca la connexió TCP d'identificador "SckCon".                        */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*   0 si tot va bé;                                                      */
/*  -1 si hi ha un error a la interfície de sockets.                      */
int UEBc_TancaConnexio(int SckCon, char *TextRes)
{
	int codi;
	int res = TCP_TancaSock(SckCon);
    if(res == -1) {
        sprintf(TextRes, "TCP_TancaSock(): %s", T_ObteTextRes(&codi));
        return -1;
    }

    strcpy(TextRes, "Connexió tancada amb èxit");
    return 0;
}

/* Donat el socket TCP “connectat” d’identificador “SckCon”, troba        */
/* l’adreça del socket local, omplint “IPloc” i “portTCPloc” amb          */
/* respectivament, la seva @IP i #port TCP, i troba l'adreça del socket   */
/* remot amb qui està connectat, omplint “IPrem” i  “portTCPrem” amb      */
/* respectivament, la seva @IP i #port TCP.                               */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "IPloc" i "IPrem" són "strings" de C (vector de chars imprimibles      */
/* acabats en '\0') d'una longitud màxima de 16 chars (incloent '\0').    */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*   0 si tot va bé;                                                      */
/*  -1 si hi ha un error a la interfície de sockets.                      */
int UEBc_TrobaAdrSckConnexio(int SckCon, char *IPloc, int *portTCPloc, char *IPrem, int *portTCPrem, char *TextRes)
{
    int codi;

    /* Adreça del socket local (C) */
    if(TCP_TrobaAdrSockLoc(SckCon, IPloc, portTCPloc) == -1) {
        sprintf(TextRes, "TCP_TrobaAdrSockLoc(): %s", T_ObteTextRes(&codi));
        return -1;
    }

    /* Adreça del socket remot (S) */
    if(TCP_TrobaAdrSockRem(SckCon, IPrem, portTCPrem) == -1) {
        sprintf(TextRes, "TCP_TrobaAdrSockRem(): %s", T_ObteTextRes(&codi));
        return -1;
    }

    strcpy(TextRes, "adreces dels sockets obtingudes");
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

/* "Construeix" un missatge de PUEB a partir dels seus camps tipus,       */
/* long1 i info1, escrits, respectivament a "tipus", "long1" i "info1"    */
/* (que té una longitud de "long1" bytes), i l'envia a través del         */
/* socket TCP “connectat” d’identificador “SckCon”.                       */
/*                                                                        */
/* "tipus" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud de 4 chars (incloent '\0').                       */
/* "info1" és un vector de chars (bytes) qualsevol (recordeu que en C,    */
/* un char és un enter de 8 bits) d'una longitud <= 9999 bytes.           */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé;                                                       */
/* -1 si hi ha un error a la interfície de sockets;                       */
/* -2 si protocol és incorrecte (longitud camps, tipus de peticio).       */
int ConstiEnvMis(int SckCon, const char *tipus, const char *info1, int long1)
{
	char missatge[10007];
    /* Si els camps no són correctes, error de protocol */
    if(strlen(tipus) != 3 || long1 < 0 || long1 > 9999) {
        return -2;
    }

    /* Es construeix el missatge: tipus (3 B) + long1 (4 B) + info1 */
    sprintf(missatge, "%s%04d", tipus, long1);
    memcpy(missatge+7, info1, long1);

    /* S'envia el missatge */
    int bytes_enviats = TCP_Envia(SckCon, missatge, 7 + long1);
    if(bytes_enviats == -1) {
        return -1;
    }
    return 0;
}

/* Rep a través del socket TCP “connectat” d’identificador “SckCon” un    */
/* missatge de PUEB i el "desconstrueix", és a dir, obté els seus camps   */
/* tipus, long1 i info1, que escriu, respectivament a "tipus", "long1"    */
/* i "info1" (que té una longitud de "long1" bytes).                      */
/*                                                                        */
/* "tipus" és un "string" de C (vector de chars imprimibles acabat en     */
/* '\0') d'una longitud de 4 chars (incloent '\0').                       */
/* "info1" és un vector de chars (bytes) qualsevol (recordeu que en C,    */
/* un char és un enter de 8 bits) d'una longitud <= 9999 bytes.           */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé;                                                       */
/* -1 si hi ha un error a la interfície de sockets;                       */
/* -2 si protocol és incorrecte (longitud camps, tipus de peticio);       */
/* -3 si l'altra part tanca la connexió.                                  */
int RepiDesconstMis(int SckCon, char *tipus, char *info1, int *long1)
{
    char cap[7];        /* capçalera: tipus (3 B) + long1 (4 B)           */
    char long_str[5];   /* long1 en ASCII + '\0'                          */
    int r, i;

    /* 1) Es rep la capçalera (exactament 7 bytes) */
    r = RepExacte(SckCon, cap, 7);
    if(r == -1) return -1;
    if(r == 0)  return -3;

    /* 2) Es treu el tipus i es comprova que sigui vàlid */
    memcpy(tipus, cap, 3);
    tipus[3] = '\0';
    if(strcmp(tipus, "OBT") != 0 && strcmp(tipus, "COR") != 0 && strcmp(tipus, "ERR") != 0) {
        return -2;
    }

    /* 3) Es treu long1 i es comprova que siguin 4 dígits */
    memcpy(long_str, cap + 3, 4);
    long_str[4] = '\0';
    for(i = 0; i < 4; i++) {
        if(!isdigit((unsigned char)long_str[i])) return -2;
    }
    *long1 = atoi(long_str);

    /* 4) Es rep info1 (exactament long1 bytes), si n'hi ha */
    if(*long1 > 0) {
        r = RepExacte(SckCon, info1, *long1);
        if(r == -1) return -1;
        if(r == 0)  return -3;
    }

    return 0;
}

/* Rep a través del socket TCP "connectat" d'identificador "Sck"          */
/* exactament "n" bytes, que escriu a "buf". Com que TCP és orientat a    */
/* flux, un sol TCP_Rep() pot retornar menys bytes dels demanats, i per   */
/* això es repeteix TCP_Rep() fins a tenir-los tots.                      */
/*                                                                        */
/* Retorna:                                                               */
/*  n si tot va bé;                                                       */
/*  0 si l'altra part tanca la connexió;                                  */
/* -1 si hi ha un error a la interfície de sockets.                       */
int RepExacte(int Sck, char *buf, int n)
{
    int rebuts = 0;
    int r;

    while(rebuts < n) {
        r = TCP_Rep(Sck, buf + rebuts, n - rebuts);
        if(r == -1) return -1;
        if(r == 0)  return 0;
        rebuts = rebuts + r;
    }
    return n;
}
