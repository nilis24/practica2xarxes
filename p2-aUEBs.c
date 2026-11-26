/**************************************************************************/
/*                                                                        */
/* L'aplicació UEB amb sockets TCP/IP                                     */
/* Fitxer aUEB.c que implementa la capa d'aplicació de UEB, sobre la      */
/* cap de transport TCP (fent crides a la "nova" interfície de la         */
/* capa TCP o "nova" interfície de sockets TCP), en la part servidora.    */
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
#include "p2-aUEBs.h"


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>   /* per gettimeofday, MS4 */

/* Definició de constants, p.e., XYZ                                      */
#define MAX_INFO1 9999
/* #define XYZ       1500                                                 */
/* Arrel configurable del lloc UEB (MS1)                                  */
static char ArrelLloc[1024] = "";  

/* Declaració de funcions INTERNES, és a dir, funcions que es fan servir  */
/* només en aquest fitxer, p.e., int FuncioInterna(arg1, arg2...);        */
/* Les seves definicions es troben al final del fitxer però les seves     */
/* declaracions es troben a l'inici del fitxer (i així les funcions       */
/* són conegudes des de l'inici fins al final del fitxer).                */

/* int FuncioInterna(arg1, arg2...);                                      */

int ConstiEnvMis(int SckCon, const char *tipus, const char *info1, int long1);
int RepiDesconstMis(int SckCon, char *tipus, char *info1, int *long1);

/* Definició de funcions EXTERNES, és a dir, funcions que es criden des   */
/* d'altres fitxers, p.e., int UEBs_FuncioExterna(arg1, arg2...) { }      */
/* En termes de capes de l'aplicació, aquest conjunt de funcions externes */
/* formen la interfície de la capa UEB, en la part client.                */

/* Descripció de la funció, dels arguments, valors de retorn, etc.        */
/* int UEBs_FuncioExterna(arg1, arg2...)
{
	
} */

/* Inicia el S UEB: crea un nou socket TCP "servidor" a una @IP local     */
/* qualsevol i al #port TCP “portTCPser”. Escriu l'identificador del      */
/* socket creat a "SckEsc".                                               */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si tot va bé;                                                       */
/* -1 si hi ha un error en la interfície de sockets.                      */
int UEBs_IniciaServ(int *SckEsc, int portTCPser, char *TextRes)
{
	 int sesc;
    int Codi;

    /* guardar arrel del lloc UEB                                          */
    if (Arrel != NULL && Arrel[0] != '\0') {
        strncpy(ArrelLloc, Arrel, sizeof(ArrelLloc) - 1);
        ArrelLloc[sizeof(ArrelLloc) - 1] = '\0';
    } else {
        ArrelLloc[0] = '\0'; /* arrel = carpeta on s'executa el S         */
    }

    sesc = TCP_CreaSockServidor("0.0.0.0", portTCPser);
    if (sesc == -1) {
        sprintf(TextRes, "UEBs_IniciaServ(): %s", T_ObteTextRes(&Codi));
        return -1;
    }

    *SckEsc = sesc;
    sprintf(TextRes, "UEBs_IniciaServ(): servidor escoltant al port %d", portTCPser);
    return 0;
}

/* Accepta una connexió d'un C UEB que arriba a través del socket TCP     */
/* "servidor" d'identificador "SckEsc".                                   */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  l'identificador del socket TCP connectat si tot va bé;                */
/* -1 si hi ha un error a la interfície de sockets.                       */
int UEBs_AcceptaConnexio(int SckEsc, char *TextRes)
{
	char IPrem[16];
    int portrem;
    int scon;
    int Codi;

    scon = TCP_AcceptaConnexio(SckEsc, IPrem, &portrem);
    if (scon == -1) {
        sprintf(TextRes, "UEBs_AcceptaConnexio(): %s", T_ObteTextRes(&Codi));
        return -1;
    }

    sprintf(TextRes, "UEBs_AcceptaConnexio(): connexio de %s:%d", IPrem, portrem);
    return scon;
	
}

/* Serveix una petició UEB d'un C a través de la connexió TCP             */
/* d'identificador "SckCon". A "TipusPeticio" hi escriu el tipus de       */
/* petició (p.e., OBT) i a "NomFitx" el nom del fitxer de la petició.     */
/* Escriu un text que descriu el resultat de la funció a "TextRes".       */
/*                                                                        */
/* "TipusPeticio" és un "string" de C (vector de chars imprimibles acabat */
/* en '\0') d'una longitud de 4 chars (incloent '\0').                    */
/* "NomFitx" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud <= 10000 chars (incloent '\0').                   */
/* "TextRes" és un "string" de C (vector de chars imprimibles acabat en   */
/* '\0') d'una longitud màxima de 200 chars (incloent '\0').              */
/*                                                                        */
/* Retorna:                                                               */
/*  0 si el fitxer existeix al servidor;                                  */
/*  1 la petició és ERRònia (p.e., el fitxer no existeix);                */
/* -1 si hi ha un error a la interfície de sockets;                       */
/* -2 si protocol és incorrecte (longitud camps, tipus de peticio, etc.); */
/* -3 si l'altra part tanca la connexió;                                  */
/* -4 si hi ha problemes amb el fitxer de la petició (p.e., nomfitxer no  */
/*  comença per /, fitxer no es pot llegir, fitxer massa gran, etc.).     */
int UEBs_ServeixPeticio(int SckCon, char *TipusPeticio, char *NomFitx, char *TextRes)
{
	char tipus[4];
    char info1[MAX_INFO1 + 1];
    int long1;
    int ret;
    int fd;
    int nbytes;
    char fitxer[MAX_INFO1]; /* com diu el protocol, max 9999 */
    struct stat st;

    struct timeval t1, t2;
    double dt = 0.0;
    double vef = 0.0;

    if (TempsEnviament) *TempsEnviament = 0.0;
    if (Velocitat)      *Velocitat      = 0.0;

    /* Rebre i desconstruir missatge PUEB                                  */
    ret = RepiDesconstMis(SckCon, tipus, info1, &long1);
    if (ret == -1) {
        sprintf(TextRes, "UEBs_ServeixPeticio(): error sockets en rebre");
        return -1;
    } else if (ret == -2) {
        sprintf(TextRes, "UEBs_ServeixPeticio(): protocol incorrecte en rebre");
        return -2;
    } else if (ret == -3) {
        sprintf(TextRes, "UEBs_ServeixPeticio(): el client ha tancat la connexio");
        return -3;
    }

    /* copiar per retornar a dalt                                          */
    strcpy(TipusPeticio, tipus);
    info1[long1] = '\0';
    strcpy(NomFitx, info1);

    /* Només acceptem OBT                                                  */
    if (strcmp(tipus, "OBT") != 0) {
        /* protocol incorrecte: MS3 codi "2 tipus desconegut"              */
        ConstiEnvMis(SckCon, "ERR", "2 tipus desconegut",
                     (int)strlen("2 tipus desconegut"));
        sprintf(TextRes, "UEBs_ServeixPeticio(): tipus desconegut");
        return -2;
    }

    /* MS3: nom ha de començar per "/"                                    */
    if (info1[0] != '/') {
        ConstiEnvMis(SckCon, "ERR", "3 nom no comenca per /",
                     (int)strlen("3 nom no comenca per /"));
        sprintf(TextRes, "UEBs_ServeixPeticio(): nom no comenca per /");
        return -4;
    }

    /* Construir camí real segons l'arrel (MS1)                            */
    char nomreal[2048];
    if (ArrelLloc[0] != '\0') {
        /* Arrel configurable + path demanat                               */
        snprintf(nomreal, sizeof(nomreal), "%s%s", ArrelLloc, info1);
    } else {
        /* Compatibilitat: arrel = cwd i traiem "/" inicial                */
        snprintf(nomreal, sizeof(nomreal), "%s", info1 + 1);
    }

    /* Mirar si el fitxer existeix i mida                                  */
    if (stat(nomreal, &st) == -1) {
        /* no existeix: MS3 codi "1 fitxer no trobat"                      */
        ConstiEnvMis(SckCon, "ERR", "1 fitxer no trobat",
                     (int)strlen("1 fitxer no trobat"));
        sprintf(TextRes, "UEBs_ServeixPeticio(): fitxer no trobat");
        return 1;
    }
    if (st.st_size > MAX_INFO1) {
        /* massa gran: MS3 codi "4 fitxer massa gran"                      */
        ConstiEnvMis(SckCon, "ERR", "4 fitxer massa gran",
                     (int)strlen("4 fitxer massa gran"));
        sprintf(TextRes, "UEBs_ServeixPeticio(): fitxer massa gran");
        return -4;
    }

    /* Obrir fitxer i llegir                                               */
    fd = open(nomreal, O_RDONLY);
    if (fd == -1) {
        ConstiEnvMis(SckCon, "ERR", "1 fitxer no trobat",
                     (int)strlen("1 fitxer no trobat"));
        sprintf(TextRes, "UEBs_ServeixPeticio(): no s'ha pogut obrir");
        return 1;
    }

    nbytes = (int)read(fd, fitxer, MAX_INFO1);
    close(fd);
    if (nbytes < 0) {
        ConstiEnvMis(SckCon, "ERR", "5 error llegint fitxer",
                     (int)strlen("5 error llegint fitxer"));
        sprintf(TextRes, "UEBs_ServeixPeticio(): error llegint fitxer");
        return -4;
    }

    /* MS4: mesurar el temps d'enviament de la resposta COR               */
    gettimeofday(&t1, NULL);
    ret = ConstiEnvMis(SckCon, "COR", fitxer, nbytes);
    gettimeofday(&t2, NULL);

    if (ret < 0) {
        sprintf(TextRes, "UEBs_ServeixPeticio(): error enviant COR");
        return -1;
    }

    dt = (double)(t2.tv_sec - t1.tv_sec) +
         (double)(t2.tv_usec - t1.tv_usec) / 1e6;

    if (dt > 0.0) {
        vef = (8.0 * (double)nbytes) / dt; /* bits per segon              */
    } else {
        vef = 0.0;
    }

    if (TempsEnviament) *TempsEnviament = dt;
    if (Velocitat)      *Velocitat      = vef;

    sprintf(TextRes,
            "UEBs_ServeixPeticio(): fitxer %s servit (%d bytes)",
            nomreal, nbytes);
    return 0;
	
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
int UEBs_TancaConnexio(int SckCon, char *TextRes)
{
	 int ret;
    int Codi;

    ret = TCP_TancaSock(SckCon);
    if (ret == -1) {
        sprintf(TextRes, "UEBs_TancaConnexio(): %s", T_ObteTextRes(&Codi));
        return -1;
    }

    sprintf(TextRes, "UEBs_TancaConnexio(): connexio tancada");
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
int UEBs_TrobaAdrSckConnexio(int SckCon, char *IPloc, int *portTCPloc, char *IPrem, int *portTCPrem, char *TextRes)
{
	 int ret;
    int Codi;

    ret = TCP_TrobaAdrSockLoc(SckCon, IPloc, portTCPloc);
    if (ret == -1) {
        sprintf(TextRes, "UEBs_TrobaAdrSckConnexio(): %s", T_ObteTextRes(&Codi));
        return -1;
    }
    ret = TCP_TrobaAdrSockRem(SckCon, IPrem, portTCPrem);
    if (ret == -1) {
        sprintf(TextRes, "UEBs_TrobaAdrSckConnexio(): %s", T_ObteTextRes(&Codi));
        return -1;
    }

    sprintf(TextRes, "UEBs_TrobaAdrSckConnexio(): ok");
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
	char mis[3 + 4 + MAX_INFO1];
    char longstr[5];
    int total;

    if (strlen(tipus) != 3) {
        return -2;
    }
    if (long1 < 0 || long1 > MAX_INFO1) {
        return -2;
    }

    memcpy(mis, tipus, 3);
    sprintf(longstr, "%.4d", long1);
    memcpy(mis + 3, longstr, 4);

    if (long1 > 0 && info1 != NULL) {
        memcpy(mis + 7, info1, long1);
    }

    total = 3 + 4 + long1;

    if (TCP_Envia(SckCon, mis, total) == -1) {
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
/*  0 si tot va bé,                                                       */
/* -1 si hi ha un error a la interfície de sockets;                       */
/* -2 si protocol és incorrecte (longitud camps, tipus de peticio);       */
/* -3 si l'altra part tanca la connexió.                                  */
int RepiDesconstMis(int SckCon, char *tipus, char *info1, int *long1)
{
	 char cap[7];
    int n;
    int l1;
    char longstr[5];

    n = TCP_Rep(SckCon, cap, 7);
    if (n == 0) {
        return -3;
    }
    if (n == -1 || n != 7) {
        return -1;
    }

    memcpy(tipus, cap, 3);
    tipus[3] = '\0';

    memcpy(longstr, cap + 3, 4);
    longstr[4] = '\0';
    l1 = atoi(longstr);
    if (l1 < 0 || l1 > MAX_INFO1) {
        return -2;
    }

    if (l1 > 0) {
        n = TCP_Rep(SckCon, info1, l1);
        if (n == 0) return -3;
        if (n == -1 || n != l1) return -1;
        *long1 = l1;
    } else {
        *long1 = 0;
    }

    return 0;
}
