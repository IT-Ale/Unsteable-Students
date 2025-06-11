/***************************************************************************
*  Funzioni per lo svolgimento dei (Turni IA e non)
 ***************************************************************************/

#include "header.h"
#include "ANSI.h"

/**
 * scartaCartaMano:
 *  - Rimuove "cartaDaScartare" dalla mano del giocatore
 *  - Se la carta è MATRICOLA e la stiamo scartando DALLA MANO,
 *    va direttamente in "aulaStudio" (regola speciale).
 *  - Altrimenti la carta va nel "mazzoScarti".
 */
void scartaCartaMano(Carta **mazzoScarti, Carta **aulaStudio,Giocatore *giocatore, Carta *cartaDaScartare) {
    if (giocatore->carteMano == NULL || cartaDaScartare == NULL) {
    } else {
        // Rimuovo la carta dalla mano del giocatore
        giocatore->carteMano = rimuoviCartaDaLista(giocatore->carteMano, cartaDaScartare);

        // Se è una Matricola, la metto in aula (regola speciale)
        if (cartaDaScartare->tipologia == MATRICOLA) {
            *aulaStudio = aggiungiCartaInLista(*aulaStudio, cartaDaScartare);
            printf(BBLU "Carta " BYEL "%s" BBLU " aggiunta in Aula Studio (scartata dalla mano)\n" RESET,
                   cartaDaScartare->nome);
        } else {
            // Altrimenti va negli scarti
            *mazzoScarti = aggiungiCartaInLista(*mazzoScarti, cartaDaScartare);
            printf(BBLU "Carta " BYEL "%s" BBLU " scartata dalla mano con successo\n" RESET,
                   cartaDaScartare->nome);
        }
        // Aggiornamento log
        scriviLogTurno(giocatore->nome, "scarta", cartaDaScartare->nome);
    }
}
/**
 * scartaCartaAula:
 *  - Rimuove "cartaDaScartare" dall'aula del giocatore e la aggiunge a mazzoScarti.
 */
void scartaCartaAula(Carta **mazzoScarti, Carta **aulaStudio, Carta **mazzoPesca, Giocatore *giocatore, Carta *cartaDaScartare, bool isIA) {
    if (giocatore->aula == NULL || cartaDaScartare == NULL) {
    } else {
        // Rimuovo la carta dall'aula
        giocatore->aula = rimuoviCartaDaLista(giocatore->aula, cartaDaScartare);

        //Attivo effetto se c'è
        attivaEffettoCartaUscitaDaAula(giocatore, cartaDaScartare, mazzoPesca, aulaStudio, mazzoScarti, isIA);

        // Va negli scarti
        *mazzoScarti = aggiungiCartaInLista(*mazzoScarti, cartaDaScartare);
        printf(BBLU "Carta " BYEL "%s" BBLU " scartata dall'aula con successo\n" RESET,
               cartaDaScartare->nome);
        // Aggiornamento log
        scriviLogTurno(giocatore->nome, "scarta da Aula", cartaDaScartare->nome);
    }
}

/**
 * scartaCartaBonusMalus:
 *  - Rimuove "cartaDaScartare" dal bonusMalus del giocatore e la aggiunge a mazzoScarti.
 *  - Anche qui, nessuna regola speciale per Matricole, perché stiamo scartando
 *    da bonus/malus e non dalla mano.
 */
void scartaCartaBonusMalus(Carta **mazzoScarti, Giocatore *giocatore, Carta *cartaDaScartare) {
    if (giocatore->bonusMalus == NULL || cartaDaScartare == NULL) {
    } else {
        // Rimuovo la carta dal bonusMalus
        giocatore->bonusMalus = rimuoviCartaDaLista(giocatore->bonusMalus, cartaDaScartare);

        // La carta finisce nel mazzo scarti
        *mazzoScarti = aggiungiCartaInLista(*mazzoScarti, cartaDaScartare);
        printf(BBLU "Carta " BYEL "%s" BBLU " scartata da bonus/malus con successo\n" RESET,
               cartaDaScartare->nome);
        // Aggiornamento log
        scriviLogTurno(giocatore->nome, "scarta da Aula", cartaDaScartare->nome);
    }
}

/**
 * pescata:
 *  - Rimuove la "testa" di *mazzoPesca e la copia nella mano del giocatore.
 */

void pescata(Carta **mazzoPesca, Carta **mazzoScarti, Giocatore *giocatore) {
    if (*mazzoPesca == NULL) {
        printf(BRED "Il mazzo di pesca è vuoto! Mischiamo gli scarti e lo rigeneriamo...\n" RESET);

        if (*mazzoScarti == NULL) {
            printf(BRED "Anche il mazzo degli scarti è vuoto! Nessuna carta può essere pescata.\n" RESET);
            return;
        }

        // Trasferisco il mazzo degli scarti nel mazzo di pesca
        *mazzoPesca = *mazzoScarti;
        *mazzoScarti = NULL;

        // Mischio il mazzo di pesca
        mischiaMazzo(mazzoPesca);
        printf(BGRN "Nuovo mazzo di pesca pronto!\n" RESET);
    }

    if (*mazzoPesca == NULL) {
        printf("Mazzo di pesca vuoto\n");
        exit(1);
    }
    // Prendo la prima carta
    Carta *head = *mazzoPesca;
    // Rimuovo la carta dal mazzo
    *mazzoPesca = rimuoviCartaDaLista(*mazzoPesca, head);
    // Faccio una copia
    Carta *copia = copiaCarta(head);

    // Aggiungo la copia nella mano del giocatore
    giocatore->carteMano = aggiungiCartaInLista(giocatore->carteMano, copia);
    // Aggiornamento log
    scriviLogTurno(giocatore->nome, "pesca", copia->nome);
}

/**
 * pescataAulaStudio:
 *  - Rimuove la "testa" di *aulaStudio (che contiene Matricole) e la mette
 *    nell'aula del giocatore.
 */
void pescataAulaStudio(Carta **aulaStudio, Giocatore *giocatore) {
    if (*aulaStudio == NULL) {
        printf("Aula studio vuota\n");
        exit(1);
    }
    // Prendo la prima carta di aulaStudio
    Carta *head = *aulaStudio;
    // Rimuovo
    *aulaStudio = rimuoviCartaDaLista(*aulaStudio, head);
    // Copio
    Carta *copia = copiaCarta(head);

    // Aggiungo la copia all'aula del giocatore
    giocatore->aula = aggiungiCartaInLista(giocatore->aula, copia);
}

/**
 * resocontoGiocatore:
 *  - Stampa il nome del giocatore e i contenuti di mano, aula e bonus/malus.
 */
void resocontoGiocatore(Giocatore *giocatore) {
    if (giocatore == NULL) {
        printf("Giocatore non valido.\n");
    } else {
        // Titolo
        printf(BOLD "--- Turno di: ---" RESET "\n");
        printf(BYEL "  %s" RESET "\n", giocatore->nome);

        // Stampo mano
        printf(BLUHB "Carte in mano:" RESET "\n");
        stampaLista(giocatore->carteMano);

        // Stampo aula
        printf(GRNHB "Carte in aula:" RESET "\n");
        stampaLista(giocatore->aula);

        // Stampo bonus/malus
        printf(MAGHB "Carte bonus/malus:" RESET "\n");
        stampaLista(giocatore->bonusMalus);

        printf(BOLD "-----------------" RESET "\n");
    }
}