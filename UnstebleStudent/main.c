//Nome: Alessio Casertano (a.casertano@studenti.unica.it)
//Matricola: 60/61/66543
//Tipologia progetto: avanzato

/***************************************************************************
 *  Punto di ingresso del programma. Qui creiamo i giocatori, prepariamo
 *  i mazzi, eseguiamo i turni e controlliamo la vittoria.
 ***************************************************************************/

#include "header.h"
#include "ANSI.h"

int main() {
    srand(time(NULL));  // Inizializza il seme del random

    enableANSI();
    printf(BGRN "Avvio del gioco...\n" RESET);

    // ⚠Se vuoi forzare un salvataggio da codice, scrivilo qui:
    char nomeSalvataggio[NOME] = ""; // esempio: "salvataggio.sav";

    if (strlen(nomeSalvataggio) == 0) {
        printf(BGRN "Inserisci il nome del salvataggio da usare (senza estensione):\n" RESET);
        scanf("%s", nomeSalvataggio);
        while (getchar() != '\n');
        strcat(nomeSalvataggio, ".sav");
    }

    Carta *mazzoPesca = NULL;
    Carta *aulaStudio = NULL;
    Carta *mazzoScarti = NULL;
    Giocatore *giocatoriTemp = NULL;
    Giocatore *giocatori = NULL;

    // Prova a caricare il salvataggio
    giocatoriTemp = caricaPartita(nomeSalvataggio, &aulaStudio, &mazzoPesca, &mazzoScarti);

    if (giocatoriTemp != NULL) {
        printf(BWHT "E' stato trovato un salvataggio valido per '%s'. "BBLU"Vuoi caricarlo (1) "BWHT"o"BCYN" iniziare una nuova partita (0)?\n" RESET, nomeSalvataggio);
        int sceltaSalvataggio;
        do {
            if (scanf("%d", &sceltaSalvataggio) != 1 || (sceltaSalvataggio != 0 && sceltaSalvataggio != 1)) {
                printf(BRED "Input non valido. Inserisci 1 per caricare o 0 per nuova partita.\n" RESET);
                while (getchar() != '\n');
            }
        } while (sceltaSalvataggio != 0 && sceltaSalvataggio != 1);

        if (sceltaSalvataggio == 1) {
            giocatori = giocatoriTemp;
            printf(BGRN "Partita caricata con successo!\n" RESET);
        }
    }

    if (giocatori == NULL) {
        mostraStatistiche();
        printf(BBLU "Creazione nuova partita...\n" RESET);
        inizializzaLog();
        giocatori = creazioneGiocatori();
        visualizzaGiocatori(giocatori);
        preparazione(&mazzoPesca, &aulaStudio, &giocatori);
    }

    // IA o utente
    int scelta;
    printf(BWHT "Vuoi far giocare la partita dall'IA? ("BBLU"1=Si, "BCYN"0=No"BWHT")\n" RESET);
    do {
        if (scanf("%d", &scelta) != 1) {
            printf(BRED "Input non valido. inserisci 1 per IA o 0 per giocatore umano.\n" RESET);
            while (getchar() != '\n');
        } else if (scelta != 0 && scelta != 1) {
            printf(BRED "Scelta non valida, riprova.\n" RESET);
        }
    } while (scelta != 0 && scelta != 1);

    // Main loop
    bool giocoFinito = false;
    Giocatore *current = giocatori;

    while (!giocoFinito) {
        if (scelta == 0) {
            turno(&mazzoPesca, &aulaStudio, &current, &mazzoScarti, nomeSalvataggio);
            fineTurno(&mazzoScarti, &aulaStudio, current, &mazzoPesca);
        } else {
            turnoIA(&mazzoPesca, &aulaStudio, &current, &mazzoScarti, nomeSalvataggio);
            fineTurnoIA(&mazzoScarti, &aulaStudio, current, &mazzoPesca);
        }

        // Controlla vittoria
        Giocatore *iter = current;
        Giocatore *vincitore = NULL;
        do {
            if (contaStudenti(iter->aula) >= STUDENTIVITTORIA && !haCartaIngenerizzazione(iter)) {
                vincitore = iter;
            }
            iter = iter->next;
        } while (iter != current && vincitore == NULL);

        if (vincitore != NULL) {
            printf(BGRN "Il vincitore è: " BBLU "%s\n" RESET, vincitore->nome);
            aggiornaStatistiche(vincitore->nome, scelta == 1);
            giocoFinito = true;
        }

        current = current->next;
    }

    fineGioco(mazzoPesca, aulaStudio, giocatori, mazzoScarti);
    return 0;
}




/**
 * enableANSI:
 *  - Abilita la stampa con codici ANSI (colori) nella console di Windows.
 */
void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    // Aggiunge l'opzione ENABLE_VIRTUAL_TERMINAL_PROCESSING
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

/**
 * contaStudenti:
 *  - Funzione per contare le carte "studente" presenti nell’aula
 */
int contaStudenti(Carta *aula) {
    int count = 0;
    if (aula != NULL) {
        Carta *temp = aula;
        do {
            if (temp->tipologia == STUDENTE ||
                temp->tipologia == MATRICOLA ||
                temp->tipologia == STUDENTE_SEMPLICE ||
                temp->tipologia == LAUREANDO)
            {
                count++;
            }
            temp = temp->next;
        } while (temp != aula);
    }
    return count;
}

/**
 * haCartaIngenerizzazione:
 *  - Funzione per verificare se nella lista bonus/malus è presente la carta "ingengerizzazione"
 */
bool haCartaIngenerizzazione(Giocatore *g) {
    bool trovato = false;
    if (g->bonusMalus != NULL) {
        Carta *temp = g->bonusMalus;
        do {
            if (strcmp(temp->nome, "Ingengerizzazione") == 0) {
                trovato = true;
            }
            temp = temp->next;
        } while (temp != g->bonusMalus && !trovato);
    }
    return trovato;
}
