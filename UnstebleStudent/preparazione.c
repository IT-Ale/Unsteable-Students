/***************************************************************************
 *  Contiene funzioni per creare e mischiare i mazzi (aulaStudio, mazzoPesca)
 *  e per la creazione della lista di giocatori.
 ***************************************************************************/

#include "header.h"
#include "ANSI.h"

/**
 * mischiaMazzo:
 *  - Conta le carte in *mazzo
 *  - Esegue N volte uno shuffle, scambiando casualmente le posizioni delle carte
 *    (anche "scambiando" i next per rimanere una lista circolare coerente).
 */
void mischiaMazzo(Carta **mazzo) {
    int numCarte = contaCarte(*mazzo);
    bool mescola = (numCarte >= 2);

    if (mescola) {
        srand((unsigned) time(NULL));

        for (int k = 0; k < N; k++) {
            for (int i = numCarte - 1; i > 0; i--) {
                int j = rand() % (i + 1);
                if (i != j) {
                    // Ottieni riferimenti alle posizioni i e j
                    Carta *cartaI = OttieniPosizioneCarta(*mazzo, i);
                    Carta *cartaJ = OttieniPosizioneCarta(*mazzo, j);

                    // Scambio contenuto
                    Carta temp = *cartaI;
                    *cartaI = *cartaJ;
                    *cartaJ = temp;

                    // Scambio i puntatori next
                    Carta *tempNext = cartaI->next;
                    cartaI->next = cartaJ->next;
                    cartaJ->next = tempNext;
                }
            }
        }

        printf(BGRN "Mazzo mischiato" RESET "\n");
    }
}


/**
 * preparazione:
 *  - Carica il mazzo "intero" da file
 *  - Mischia
 *  - Crea "aulaStudio" estraendo le carte di tipo MATRICOLA
 *  - Crea "mazzoPesca" con il resto
 *  - Dealloca il mazzo intero temporaneo
 *  - Da' a ogni giocatore NPESCATEINIZIALI carte e 1 "matricola" in aula
 */
void preparazione(Carta **mazzoPesca, Carta **aulaStudio, Giocatore **giocatori) {
    // Carica e mischia
    Carta *mazzo = caricaMazzo();
    mischiaMazzo(&mazzo);
    Carta *mazzoScarti = NULL;
    // Crea l'aulaStudio (solo carte di tipo MATRICOLA)
    *aulaStudio = creazioneAulaStudio(mazzo);
    // Crea il mazzoPesca (tutto il resto)
    *mazzoPesca = creazioneMazzoPesca(mazzo);

    printf(BGRN "Aula Studio creata con successo" RESET "\n");
    printf(BGRN "Mazzo Pesca creato con successo" RESET "\n");

    // Dealloco il mazzo temporaneo
    deallocaCarte(mazzo);

    // Ora do a ogni giocatore le carte iniziali
    Giocatore *temp = *giocatori;
    do {
        // Pescate iniziali
        for (int i = 0; i < NPESCATEINIZIALI; i++) {
            pescata(mazzoPesca, mazzoScarti, temp);
        }
        // Pescata di 1 carta "matricola" dall'aula
        pescataAulaStudio(aulaStudio, temp);

        temp = temp->next;
    } while (temp != *giocatori);
}

/**
 * creazioneGiocatori:
 *  - Chiede all'utente quanti giocatori (2-4)
 *  - Per ognuno, chiede il nome e crea il giocatore in una lista circolare
 *  - Ritorna la lista di giocatori
 */
Giocatore* creazioneGiocatori() {
    int numGiocatori;
    Giocatore *listaGiocatori = NULL;

    // Chiedi il numero di giocatori (2-4)
    do {
        printf("Inserisci il numero di giocatori (2-4):\n");
        scanf("%d", &numGiocatori);
        if (numGiocatori < 2 || numGiocatori > 4) {
            printf(BRED "I giocatori possono essere un minimo di 2 o un massimo di 4\n" RESET);
        }
    } while (numGiocatori < 2 || numGiocatori > 4);

    // Crea i giocatori
    for (int i = 0; i < numGiocatori; i++) {
        Giocatore *nuovo = (Giocatore*) malloc(sizeof(Giocatore));
        if (nuovo == NULL) {
            printf(BRED "Errore allocazione memoria per Giocatore" RESET "\n");
            exit(1);
        }
        // Chiedi il nome
        printf("Inserisci il nome del %d giocatore:\n", i + 1);
        scanf(" %[^\n]", nuovo->nome);

        // Inizializza le liste
        nuovo->carteMano = NULL;
        nuovo->aula = NULL;
        nuovo->bonusMalus = NULL;
        nuovo->next = NULL;

        // Aggiungilo alla lista circolare
        listaGiocatori = aggiungiGiocatoreInLista(listaGiocatori, nuovo);
    }
    printf(BGRN "Aggiunta giocatori in corso...." RESET "\n");
    return listaGiocatori;
}

/**
 * Aggiunge un giocatore "nuovoGiocatore" alla lista circolare "listaGiocatori".
 */
Giocatore* aggiungiGiocatoreInLista(Giocatore *listaGiocatori, Giocatore *nuovoGiocatore) {
    if (listaGiocatori == NULL) {
        // Primo giocatore
        nuovoGiocatore->next = nuovoGiocatore; // circolare
        listaGiocatori = nuovoGiocatore;
    } else {
        // Cerco l'ultimo
        Giocatore *temp = listaGiocatori;
        while (temp->next != listaGiocatori) {
            temp = temp->next;
        }
        temp->next = nuovoGiocatore;
        nuovoGiocatore->next = listaGiocatori;
    }

    return listaGiocatori;
}


/**
 * Stampa i giocatori della lista circolare, con un indice incrementale.
 */
void visualizzaGiocatori(Giocatore *listaGiocatore) {
    bool stampa = (listaGiocatore != NULL);

    if (stampa) {
        Giocatore *temp = listaGiocatore;
        int count = 1;

        printf(BOLD "--- Giocatori ---" RESET "\n");
        do {
            printf("  G:%d = %s\n", count, temp->nome);
            temp = temp->next;
            count++;
        } while (temp != listaGiocatore);
        printf(BOLD "-----------------" RESET "\n");
    }
}


/**
 * creazioneAulaStudio:
 *  - Scorre l'intero "mazzo", copiando SOLO le carte di tipo MATRICOLA in una
 *    nuova lista "aulaStudio".
 *  - Non rimuove nulla da "mazzo", perché poi verrà usato.
 */
Carta* creazioneAulaStudio(Carta *mazzo) {
    Carta *aulaStudio = NULL;

    if (mazzo != NULL) {
        Carta *temp = mazzo;
        do {
            if (temp->tipologia == MATRICOLA) {
                Carta *nuova = copiaCarta(temp);
                aulaStudio = aggiungiCartaInLista(aulaStudio, nuova);
            }
            temp = temp->next;
        } while (temp != mazzo);
    }

    return aulaStudio;
}


/**
 * creazioneMazzoPesca:
 *  - Scorre l'intero "mazzo", copiando TUTTO TRANNE le carte di tipo MATRICOLA
 *    in una nuova lista "mazzoPesca".
 */
Carta* creazioneMazzoPesca(Carta *mazzo) {
    Carta *mazzoPesca = NULL;

    if (mazzo != NULL) {
        Carta *temp = mazzo;

        do {
            if (temp->tipologia != MATRICOLA) {
                Carta *nuova = copiaCarta(temp);
                mazzoPesca = aggiungiCartaInLista(mazzoPesca, nuova);
            }
            temp = temp->next;
        } while (temp != mazzo);
    }

    return mazzoPesca;
}

