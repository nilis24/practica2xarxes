/**************************************************************************/
/*                                                                        */
/* L'aplicació UEB amb sockets TCP/IP                                     */
/* Fitxer serUEB.c que implementa la interfície aplicació-administrador   */
/* d'un servidor de l'aplicació UEB, sobre la capa d'aplicació de         */
/* (la part servidora de) UEB (fent crides a la interfície de la part     */
/* servidora de la capa UEB).                                             */
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

#include "p2-aUEBs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>   /* MS5: SIGINT */
#include <stdarg.h>   /* MS2: variadic log */
#include <time.h>     /* MS2: data/hora al log */
#include <unistd.h>   /* MS5: close() */


#define CFG_FILE "p2-serUEB.cfg"
#define LOG_FILE "serUEB.log"

/* Definició de constants, p.e., XYZ                                      */
static int  g_SckEsc = -1;
static int  g_SckCon = -1;
static FILE *g_log   = NULL;
/* #define XYZ       1500   

                                                */

/* Declaració de funcions INTERNES, és a dir, funcions que es fan servir  */
/* només en aquest fitxer, p.e., int FuncioInterna(arg1, arg2...);        */
/* Les seves definicions es troben al final del fitxer però les seves     */
/* declaracions es troben a l'inici del fitxer (i així les funcions       */
/* són conegudes des de l'inici fins al final del fitxer).                */
static void escriure_log(const char *fmt, ...);
static void gestor_SIGINT(int sig);

/* int FuncioInterna(arg1, arg2...);                                      */

int main(int argc,char *argv[])
{
 /* Declaració de variables, p.e., int n;                                 */
  int SckEsc;
    int SckCon;
    int port;
    char linia[200];
    char opcio[100], valor[150];
    FILE *fp;
    char TextRes[200];
    char TipusPeticio[4];
    char NomFitx[10050];
    int ret;

    char IPloc[16], IPrem[16];
    int portloc, portrem;
    char Arrel[1024] = "";

    double TempsEnv, Vef;

 /* Expressions, estructures de control, crides a funcions, etc.          */
 int SckEsc;
    int SckCon;
    int port;
    char linia[200];
    char opcio[100], valor[150];
    FILE *fp;
    char TextRes[200];
    char TipusPeticio[4];
    char NomFitx[10050];
    int ret;

    char IPloc[16], IPrem[16];
    int portloc, portrem;
    char Arrel[1024] = "";

    double TempsEnv, Vef;

    /* Obrir fitxer de log (MS2)                                           */
    g_log = fopen(LOG_FILE, "a");
    if (!g_log) {
        /* Si no podem obrir log, seguim funcionant igualment              */
        fprintf(stderr, "No s'ha pogut obrir el fitxer de log %s\n", LOG_FILE);
    }

    /* Registrar gestor de SIGINT (MS5)                                    */
    signal(SIGINT, gestor_SIGINT);

    /* Llegir fitxer de configuracio                                      */
    fp = fopen(CFG_FILE, "r");
    if (fp == NULL) {
        printf("No s'ha pogut obrir el fitxer de configuracio %s\n", CFG_FILE);
        escriure_log("ERROR: No s'ha pogut obrir el fitxer de configuracio %s", CFG_FILE);
        if (g_log) fclose(g_log);
        return -1;
    }
    port = 0;
    while (fgets(linia, sizeof(linia), fp) != NULL) {
        if (sscanf(linia, "%s %s", opcio, valor) == 2) {
            if (strcmp(opcio, "numportTCP") == 0) {
                port = atoi(valor);
            } else if (strcmp(opcio, "Arrel") == 0) {
                strncpy(Arrel, valor, sizeof(Arrel) - 1);
                Arrel[sizeof(Arrel) - 1] = '\0';
            }
        }
    }
    fclose(fp);

    if (port == 0) {
        printf("Port TCP no trobat al fitxer de configuracio\n");
        escriure_log("ERROR: Port TCP no trobat al fitxer de configuracio");
        if (g_log) fclose(g_log);
        return -1;
    }

    /* Iniciar servidor UEB (socket d'escolta). MS1: passar Arrel         */
    if (UEBs_IniciaServ(&SckEsc, port, Arrel, TextRes) == -1) {
        printf("%s\n", TextRes);
        escriure_log("ERROR: %s", TextRes);
        if (g_log) fclose(g_log);
        return -1;
    }
    printf("%s\n", TextRes);
    escriure_log("Servidor iniciat al port %d, Arrel=\"%s\"", port, Arrel);

    g_SckEsc = SckEsc; /* per MS5                                         */

    /* Servidor iteratiu                                                   */
    while (1) {
        /* Esperar connexio                                                */
        SckCon = UEBs_AcceptaConnexio(SckEsc, TextRes);
        if (SckCon == -1) {
            printf("%s\n", TextRes);
            escriure_log("ERROR acceptant connexio: %s", TextRes);
            continue;
        }
        printf("%s\n", TextRes);
        escriure_log("Connexio acceptada: %s", TextRes);

        g_SckCon = SckCon; /* MS5                                         */

        /* Trobar adreces sockets local i remot                            */
        if (UEBs_TrobaAdrSckConnexio(SckCon, IPloc, &portloc, IPrem, &portrem, TextRes) != 0) {
            printf("UEBs_TrobaAdrSckConnexio(): %s\n", TextRes);
            escriure_log("ERROR UEBs_TrobaAdrSckConnexio(): %s", TextRes);
            strcpy(IPloc, "?.?.?.?");
            portloc = 0;
        }

        /* Dins d'una connexio, servir peticions successives               */
        while (1) {
            TempsEnv = 0.0;
            Vef      = 0.0;

            ret = UEBs_ServeixPeticio(SckCon, TipusPeticio, NomFitx,
                                      TextRes, &TempsEnv, &Vef);

            if (ret == -3) {
                printf("Client ha tancat la connexio: %s\n", TextRes);
                escriure_log("Client ha tancat la connexio: %s", TextRes);
                break;
            } else if (ret == -1 || ret == -2 || ret == -4) {
                printf("Error servint peticio UEB: %s\n", TextRes);
                escriure_log("ERROR servint peticio UEB: %s", TextRes);
                break;
            } else {
                /* 0 = fitxer servit, 1 = fitxer no trobat (ERR ja enviat) */
                printf("Peticio UEB: %s %s, C=%s:%d, S=%s:%d --> %s\n",
                       TipusPeticio,
                       NomFitx,
                       IPrem, portrem,
                       IPloc, portloc,
                       TextRes);

                escriure_log("Peticio UEB: %s %s, C=%s:%d, S=%s:%d --> %s",
                             TipusPeticio,
                             NomFitx,
                             IPrem, portrem,
                             IPloc, portloc,
                             TextRes);

                /* MS4: si s'ha servit fitxer (ret == 0), mostrar temps i vef */
                if (ret == 0 && TempsEnv > 0.0) {
                    printf("Temps enviament vist pel S: %.6f segons, vef: %.2f bps\n",
                           TempsEnv, Vef);
                    escriure_log("Temps enviament: %.6f s, vef: %.2f bps",
                                 TempsEnv, Vef);
                }
            }
        }

        /* Tancar connexio amb aquest client                               */
        UEBs_TancaConnexio(SckCon, TextRes);
        printf("%s\n", TextRes);
        escriure_log("%s", TextRes);

        g_SckCon = -1;
    }

    /* No arribem aquí normalment                                         */
    if (g_log) fclose(g_log);
    return 0;
}

/* Definició de funcions INTERNES, és a dir, d'aquelles que es faran      */
/* servir només en aquest mateix fitxer. Les seves declaracions es troben */
/* a l'inici d'aquest fitxer.                                             */
/* Escriu al log amb marca temporal (MS2)                                 */
static void escriure_log(const char *fmt, ...)
{
    if (!g_log) return;

    time_t ara = time(NULL);
    struct tm *tm = localtime(&ara);
    char bufhora[64];

    if (tm) {
        strftime(bufhora, sizeof(bufhora), "%Y-%m-%d %H:%M:%S", tm);
        fprintf(g_log, "[%s] ", bufhora);
    }

    va_list ap;
    va_start(ap, fmt);
    vfprintf(g_log, fmt, ap);
    va_end(ap);

    fprintf(g_log, "\n");
    fflush(g_log);
}

/* Gestor de SIGINT per acabar suaument (MS5)                              */
static void gestor_SIGINT(int sig)
{
    (void)sig; /* no utilitzat */

    fprintf(stdout, "\nRebut SIGINT (CTRL+C). Aturant servidor suaument...\n");
    if (g_log) {
        fprintf(g_log, "Aturant servidor per SIGINT\n");
        fflush(g_log);
    }

    if (g_SckCon != -1) {
        close(g_SckCon);
        g_SckCon = -1;
    }
    if (g_SckEsc != -1) {
        close(g_SckEsc);
        g_SckEsc = -1;
    }

    if (g_log) {
        fclose(g_log);
        g_log = NULL;
    }

    _exit(0); /* sortida immediata i neta                                  */
 
 
 

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
