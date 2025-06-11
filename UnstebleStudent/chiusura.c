/***************************************************************************
 *  chiusura.c
 *  Contiene la logica per terminare il gioco (fineGioco) e liberare
 *  la memoria delle liste (deallocaCarte, deallocaGiocatori).
 ***************************************************************************/

#include "header.h"
#include "ANSI.h"

/**
 * fineGioco:
 *  - Dealloca mazzoPesca, aulaStudio, mazzoScarti e i giocatori
 *  - Stampa messaggi di conferma
 */
void fineGioco(Carta *mazzoPesca, Carta *aulaStudio, Giocatore *giocatori, Carta *mazzoScarti) {
    // Deallocazione delle varie liste
    deallocaCarte(mazzoPesca);
    printf(BGRN "Mazzo di Pesca deallocato" RESET "\n");

    deallocaCarte(aulaStudio);
    printf(BGRN "Aula Studio deallocata" RESET "\n");

    deallocaCarte(mazzoScarti);
    printf(BGRN "Mazzo Scarti deallocato" RESET "\n");

    deallocaGiocatori(giocatori);
    printf(BGRN "Giocatori deallocati" RESET "\n");
}

/**
 * deallocaCarte:
 *  - Dealloca l'intera lista circolare di Carte.
 *  - Prima spezza la circolarità, poi libera ogni nodo uno a uno.
 */
void deallocaCarte(Carta *mazzo) {
    bool procedi = (mazzo != NULL);

    if (procedi) {
        // Caso con un solo nodo
        bool soloUno = (mazzo->next == mazzo);

        if (soloUno) {
            if (mazzo->effetti != NULL) {
                free(mazzo->effetti);
            }
            mazzo->next = NULL;
            free(mazzo);
        } else {
            // Cerco l'ultimo e rompo il cerchio
            Carta *ultimo = mazzo;
            while (ultimo->next != mazzo) {
                ultimo = ultimo->next;
            }
            ultimo->next = NULL;

            // Deallocazione dei nodi
            Carta *temp = mazzo;
            while (temp != NULL) {
                Carta *daEliminare = temp;
                temp = temp->next;

                if (daEliminare->effetti != NULL) {
                    free(daEliminare->effetti);
                }
                daEliminare->next = NULL;
                free(daEliminare);
            }
        }
    }
}


/**
 * deallocaGiocatori:
 *  - Dealloca la lista circolare di giocatori (carteMano, aula, bonusMalus).
 */
void deallocaGiocatori(Giocatore *listaGiocatori) {
    bool procedi = (listaGiocatori != NULL);

    if (procedi) {
        bool soloUno = (listaGiocatori->next == listaGiocatori);

        if (soloUno) {
            deallocaCarte(listaGiocatori->carteMano);
            deallocaCarte(listaGiocatori->aula);
            deallocaCarte(listaGiocatori->bonusMalus);
            listaGiocatori->next = NULL;
            free(listaGiocatori);
        } else {
            // Rompo il cerchio
            Giocatore *ultimo = listaGiocatori;
            while (ultimo->next != listaGiocatori) {
                ultimo = ultimo->next;
            }
            ultimo->next = NULL;

            // Dealloca tutti
            Giocatore *temp = listaGiocatori;
            while (temp != NULL) {
                Giocatore *daEliminare = temp;
                temp = temp->next;

                deallocaCarte(daEliminare->carteMano);
                deallocaCarte(daEliminare->aula);
                deallocaCarte(daEliminare->bonusMalus);

                daEliminare->next = NULL;
                free(daEliminare);
            }
        }
    }
}

