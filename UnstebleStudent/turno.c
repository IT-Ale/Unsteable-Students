/***************************************************************************
 *  Gestisce la logica di un singolo turno di un giocatore Umano
 ***************************************************************************/

#include "header.h"
#include "ANSI.h"

/**
 * turno:
 *  - Salvataggio
 *  - Esegue le azioni di inizio turno per un giocatore (pescata obbligatoria).
 *  - Attiva eventuali effetti di inizio turno.
 *  - Chiede se pescare ancora (1) oppure giocare una carta (2).
 *  - Al termine del turno, esegue il salvataggio automatico e attende l'immissione di 0 per terminare.
 */
void turno(Carta **mazzoPesca, Carta **aulaStudio, Giocatore **giocatore, Carta **mazzoScarti, char *nomeSalvataggio) {
    int scelta;
    // Salvataggio inizio turno
    printf(BYEL "Salvataggio automatico della partita...\n" RESET);
    salvaPartita(nomeSalvataggio, *giocatore, *aulaStudio, *mazzoPesca, *mazzoScarti);
    // Pescata obbligatoria all'inizio del turno
    pescata(mazzoPesca, mazzoScarti, *giocatore);
    resocontoGiocatore(*giocatore);

    // Attiva eventuali effetti di inizio turno
    attivaEffettiInizioTurno(*giocatore, mazzoPesca, aulaStudio, mazzoScarti);

    // Chiede se pescare un'altra carta oppure giocare una carta
    printf("Inserisci 1 per pescare un'altra carta oppure 2 per giocare una carta:\n");
    do {
        scanf("%d", &scelta);
        if (scelta != 1 && scelta != 2)
            printf(BRED "Numero non valido\n" RESET);
    } while (scelta != 1 && scelta != 2);

    if (scelta == 2)
        giocaCarta(mazzoPesca, aulaStudio, mazzoScarti, *giocatore);
    else
        pescata(mazzoPesca, mazzoScarti, *giocatore);

    resocontoGiocatore(*giocatore);


    // Attende che l'utente inserisca 0 per terminare il turno
    printf("Inserisci 0 per terminare il turno:\n");
    do {
        scanf("%d", &scelta);
        if (scelta != 0)
            printf(BRED "Numero non valido\n" RESET);
    } while (scelta != 0);
}

/**
 * fineTurno:
 *  - Se il giocatore ha più di NCARTEMAX carte in mano, chiede di scartarne
 *    abbastanza per tornare a NCARTEMAX. Questo scarto è "dalla mano".
 */
void fineTurno(Carta **mazzoScarti, Carta **aulaStudio, Giocatore *giocatore, Carta **mazzoPesca) {
    int nCarteInMano = contaCarte(giocatore->carteMano);

    // Se ha meno di NCARTEMAX, nessuna scarto forzato
    if (nCarteInMano < NCARTEMAX) {
        printf("Non hai carte da scartare.\n");
    }

    // Finché il giocatore ha più di NCARTEMAX carte, deve scartarne
    while (nCarteInMano > NCARTEMAX) {
        printf("Hai %d carte in mano, devi scartarne %d\n",
               nCarteInMano, (nCarteInMano - NCARTEMAX));
        printf("Inserisci il numero della carta che vuoi scartare (1-%d):\n", nCarteInMano);

        int nCartaScarto;
        scanf("%d", &nCartaScarto);

        // Controllo input
        while (nCartaScarto < 1 || nCartaScarto > nCarteInMano) {
            printf(BRED "Numero carta non valido. Riprova (1-%d):" RESET "\n", nCarteInMano);
            scanf("%d", &nCartaScarto);
        }

        // Seleziono la carta
        Carta *cartaDaScartare = OttieniPosizioneCarta(giocatore->carteMano, nCartaScarto - 1);
        // Uso scartaCartaMano per rimuoverla dalla mano
        scartaCartaMano(mazzoScarti, aulaStudio, giocatore, cartaDaScartare);
        nCarteInMano = contaCarte(giocatore->carteMano);
    }
    // Incremento turno nel LOG
    incrementaTurnoLog();
}

/**
 * giocaCarta:
 *  - Permette di scegliere una carta dalla mano e "giocarla".
 *  - A seconda della tipologia, finisce in aula, bonusMalus o scarti.
 *  - Se la carta ha effetti SUBITO, li attiviamo.
 */
void giocaCarta(Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti, Giocatore *giocatore) {
    if (giocatore->carteMano == NULL) {
        printf(BRED "Non hai carte in mano da giocare.\n" RESET);
    } else {
        stampaLista(giocatore->carteMano);
        int scelta;
        int nCarte = contaCarte(giocatore->carteMano);

        printf("Scegli il numero della carta da giocare (1-%d): ", nCarte);
        scanf("%d", &scelta);
        while (scelta < 1 || scelta > nCarte) {
            printf(RED "Numero non valido, riprova: " RESET);
            scanf("%d", &scelta);
        }
        Carta *cartaSelezionata = OttieniPosizioneCarta(giocatore->carteMano, scelta - 1);
        //Aggiornamento log
        scriviLogTurno(giocatore->nome, "gioca", cartaSelezionata->nome);
        //  Qui il controllo effetto IMPEDIRE
        if (BloccataDaImpedire(giocatore, cartaSelezionata)) {
            printf(BRED "Giocata annullata.\n" RESET);
        } else {
            // Procedi con la giocata come normalmente
            giocatore->carteMano = rimuoviCartaDaLista(giocatore->carteMano, cartaSelezionata);

            // Dove inserire la carta giocata
            if (cartaSelezionata->tipologia == BONUS || cartaSelezionata->tipologia == MALUS) {
                giocatore->bonusMalus = aggiungiCartaInLista(giocatore->bonusMalus, cartaSelezionata);
            } else {
                giocatore->aula = aggiungiCartaInLista(giocatore->aula, cartaSelezionata);
            }

            printf(BGRN "Carta '%s' giocata con successo!\n" RESET, cartaSelezionata->nome);

            // Attivazione degli effetti della carta
            if (cartaSelezionata->quando == SUBITO || cartaSelezionata->quando == SEMPRE) {
                attivaEffetto(giocatore, cartaSelezionata, mazzoPesca, aulaStudio, mazzoScarti,false);
            }
        }
    }
}

/**
 * attivaEffettiInizioTurno:
 *  - Attiva tutti gli effetti delle carte con "quando == "SEMPRE"
 *  - Scorre le carte del giocatore in aula e bonus/malus
 *  - Se una carta ha un effetto con "quando == INIZIO", lo attiva dopo la pescata
 *  - Se l'attivazione è opzionale, chiede al giocatore se vuole attivarla
 */
void attivaEffettiInizioTurno(Giocatore *giocatore, Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti) {
    // ---------------------------
    // 1. EFFETTI SEMPRE (TUTTI)
    // ---------------------------
    Giocatore *iter = giocatore;
    do {
        // Aula
        Carta *carta = iter->aula;
        Carta *start = carta;
        if (carta != NULL) {
            do {
                if (carta->quando == SEMPRE) {
                    printf(BBLU "[SEMPRE] Attivazione effetto per '%s' (%s)\n" RESET, carta->nome, iter->nome);
                    attivaEffetto(iter, carta, mazzoPesca, aulaStudio, mazzoScarti, false);
                }
                carta = carta->next;
            } while (carta != NULL && carta != start);
        }

        // BonusMalus
        carta = iter->bonusMalus;
        start = carta;
        if (carta != NULL) {
            do {
                if (carta->quando == SEMPRE) {
                    printf(BBLU "[SEMPRE] Attivazione effetto per '%s' (%s)\n" RESET, carta->nome, iter->nome);
                    attivaEffetto(iter, carta, mazzoPesca, aulaStudio, mazzoScarti, false);
                }
                carta = carta->next;
            } while (carta != NULL && carta != start);
        }

        iter = iter->next;
    } while (iter != giocatore);

    // ---------------------------
    // 2. EFFETTI INIZIO (SOLO TUO)
    // ---------------------------

    // Aula
    if (giocatore->aula != NULL) {
        Carta *cartaCorrente = giocatore->aula;
        Carta *primaCarta = giocatore->aula;
        do {
            if (cartaCorrente->quando == INIZIO) {
                bool attivareEffetto = true;
                if (cartaCorrente->effettiUtilizzabili) {
                    printf(BYEL "Vuoi attivare l'effetto della carta '%s'? (1=Si, 0=No)\n" RESET, cartaCorrente->nome);
                    int scelta;
                    do {
                        scanf("%d", &scelta);
                        if (scelta != 0 && scelta != 1) {
                            printf(BRED "Scelta non valida, inserisci 1 o 0.\n" RESET);
                        }
                    } while (scelta != 0 && scelta != 1);
                    attivareEffetto = (scelta == 1);
                }

                if (attivareEffetto) {
                    printf(BBLU "[INIZIO] Attivazione effetto di '%s'\n" RESET, cartaCorrente->nome);
                    attivaEffetto(giocatore, cartaCorrente, mazzoPesca, aulaStudio, mazzoScarti, false);
                }
            }
            cartaCorrente = cartaCorrente->next;
        } while (cartaCorrente != NULL && cartaCorrente != primaCarta);
    }

    // Bonus/Malus
    if (giocatore->bonusMalus != NULL) {
        Carta *cartaCorrente = giocatore->bonusMalus;
        Carta *primaCarta = giocatore->bonusMalus;
        do {
            if (cartaCorrente->quando == INIZIO) {
                bool attivareEffetto = true;
                if (cartaCorrente->effettiUtilizzabili) {
                    printf(BYEL "Vuoi attivare l'effetto della carta '%s'? (1=Si, 0=No)\n" RESET, cartaCorrente->nome);
                    int scelta;
                    do {
                        scanf("%d", &scelta);
                        if (scelta != 0 && scelta != 1) {
                            printf(BRED "Scelta non valida, inserisci 1 o 0.\n" RESET);
                        }
                    } while (scelta != 0 && scelta != 1);
                    attivareEffetto = (scelta == 1);
                }

                if (attivareEffetto) {
                    printf(BBLU "[INIZIO] Attivazione effetto di '%s'\n" RESET, cartaCorrente->nome);
                    attivaEffetto(giocatore, cartaCorrente, mazzoPesca, aulaStudio, mazzoScarti, false);
                }
            }
            cartaCorrente = cartaCorrente->next;
        } while (cartaCorrente != NULL && cartaCorrente != primaCarta);
    }

}