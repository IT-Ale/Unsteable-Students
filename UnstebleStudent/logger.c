/***************************************************************************
 *  Logger per la cronologia della partita
 *  - Scrive gli eventi nel file log.txt
 *  - Gestisce il numero di turno in base al contenuto del file
 ***************************************************************************/

#include "header.h"
#include "ANSI.h"


/**
 * inizializzaLog:
 *  - Crea o resetta il file log.txt.
 *  - Scrive "Inizio della partita" come intestazione iniziale.
 */
void inizializzaLog() {
    FILE *fp = fopen("log.txt", "w");
    if (fp == NULL) {
        printf(BRED"Errore creazione log.txt"RESET);
        exit(1);
    }
    fprintf(fp, "Inizio della partita\n");
    fclose(fp);
}

/**
 * leggiUltimoTurno:
 *  - Legge l'ultimo numero di turno presente nel file log.txt.
 *  - Restituisce il numero massimo trovato, oppure 0 se il file è vuoto.
 */
int leggiUltimoTurno() {
    FILE *fp = fopen("log.txt", "r");
    int ultimoTurno = 0;
    if (fp == NULL) {
    }
    else {
        char riga[MAX_LINE];
        while (fgets(riga, MAX_LINE, fp) != NULL) {
            if (strncmp(riga, "TURNO", 5) == 0) {
                int turno;
                if (sscanf(riga, "TURNO %d:", &turno) == 1) {
                    if (turno > ultimoTurno) {
                        ultimoTurno = turno;
                    }
                }
            }
        }
        fclose(fp);
    }
    return ultimoTurno;
}

/**
 * turnoCorrente:
 *  - Restituisce il numero di turno corrente basandosi sul contenuto del log.
 *  - Se il file è vuoto, restituisce 1.
 */
int turnoCorrente() {
    int ultimo = leggiUltimoTurno();
    return ultimo > 0 ? ultimo : 1;
}

/**
 * scriviLogTurno:
 *  - Aggiunge una riga nel file log.txt per registrare un'azione.
 *  - Usa il turno corrente senza incrementarlo.

 */
void scriviLogTurno(char *nomeGiocatore, char *azione, char *nomeCarta) {
    int turno = turnoCorrente();
    FILE *fp = fopen("log.txt", "a");
    if (fp == NULL) {
        printf(BRED"Errore apertura log.txt"RESET);
        inizializzaLog();
    }

    fprintf(fp, "TURNO %d: %s %s %s\n", turno, nomeGiocatore, azione, nomeCarta);
    fclose(fp);
}

/**
 * incrementaTurnoLog:
 *  - Aggiunge una nuova riga TURNO N: vuota per segnalare l'inizio del nuovo turno.
 *  - Il numero N è uno in più rispetto all'ultimo turno presente nel file.
 */
void incrementaTurnoLog() {
    int turno = leggiUltimoTurno() + 1;
    FILE *fp = fopen("log.txt", "a");
    if (fp == NULL) {
        printf(BRED"Errore apertura log.txt"RESET);
        exit(1);
    }
    fprintf(fp, "TURNO %d:\n", turno);
    fclose(fp);
}
