/***************************************************************************
 *  Gestisce la logica di un singolo turno di un giocatore IA
 ***************************************************************************/

#include "header.h"
#include "ANSI.h"

/**
 * turnoIA:
 *  - Salvataggio
 *  - Pescata obbligatoria all'inizio del turno.
 *  - Attiva eventuali effetti con "SEMPRE" e "INIZIO".
 *  - Prova a giocare una carta seguendo la logica IA (giocaCartaIA).
 *  - Se non gioca nulla, pesca ancora una carta.
 *  - Scarta automaticamente le carte in eccesso oltre NCARTEMAX.
 *  - Mostra il resoconto del turno.
 */
void turnoIA(Carta **mazzoPesca, Carta **aulaStudio, Giocatore **giocatore, Carta **mazzoScarti, char *nomeSalvataggio) {
    // Salvataggio inizio turno
    printf(BYEL "Salvataggio automatico della partita...\n" RESET);
    salvaPartita(nomeSalvataggio, *giocatore, *aulaStudio, *mazzoPesca, *mazzoScarti);
    pescata(mazzoPesca, mazzoScarti, *giocatore);
    resocontoGiocatore(*giocatore);

    attivaEffettiInizioTurnoIA(*giocatore, mazzoPesca, aulaStudio, mazzoScarti);

    int carteIniziali = contaCarte((*giocatore)->carteMano);
    giocaCartaIA(mazzoPesca, aulaStudio, mazzoScarti, *giocatore);
    int carteFinali = contaCarte((*giocatore)->carteMano);

    if (carteIniziali == carteFinali) {
        printf(BYEL "IA non ha giocato nessuna carta, pesca una carta.\n" RESET);
        pescata(mazzoPesca, mazzoScarti, *giocatore);
    }

    while (contaCarte((*giocatore)->carteMano) > NCARTEMAX) {
        Carta *cartaScarto = (*giocatore)->carteMano;
        printf(BYEL "IA scarta la carta: %s\n" RESET, cartaScarto->nome);
        scartaCartaMano(mazzoScarti, aulaStudio, *giocatore, cartaScarto);
    }

    resocontoGiocatore(*giocatore);
}

/**
 * giocaCartaIA:
 *  - IA: Gioca la carta più utile secondo questa logica:
 *    1. Se ha un LAUREANDO lo gioca sempre.
 *    2. Se un avversario ha > 3 studenti, cerca carte con ELIMINA (!IO), RUBA o MALUS+ELIMINA su IO.
 *    3. Altrimenti gioca secondo priorità: Studente Semplice > Bonus > Magia > Malus.
 */
void giocaCartaIA(Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti, Giocatore *giocatore) {
    bool haGiocato = false;

    if (giocatore != NULL && giocatore->carteMano != NULL) {
        // 1. Cerca un LAUREANDO
        Carta *iter = giocatore->carteMano;
        Carta *start = iter;
        bool trovato = false;

        do {
            if (!haGiocato && iter->tipologia == LAUREANDO) {
                Carta *carta = iter;
                giocatore->carteMano = rimuoviCartaDaLista(giocatore->carteMano, carta);
                giocatore->aula = aggiungiCartaInLista(giocatore->aula, carta);
                printf(BGRN "IA ha giocato il Laureando: %s\n" RESET, carta->nome);
                scriviLogTurno(giocatore->nome, "gioca", carta->nome);
                attivaEffetto(giocatore, carta, mazzoPesca, aulaStudio, mazzoScarti, true);
                haGiocato = true;
            }
            iter = iter->next;
        } while (!haGiocato && iter != start);

        // 2. Gioca tatticamente se c'è minaccia
        if (!haGiocato) {
            bool minaccia = false;
            Giocatore *g = giocatore->next;
            while (g != giocatore && !minaccia) {
                if (contaStudenti(g->aula) > 3) {
                    minaccia = true;
                }
                g = g->next;
            }

            if (minaccia) {
                Carta *scelta = NULL;
                iter = giocatore->carteMano;
                start = iter;

                do {
                    bool trovatoEffetto = false;
                    int i = 0;
                    while (i < iter->numEffetti && !trovatoEffetto) {
                        Azione a = iter->effetti[i].azione;
                        TargetGiocatori t = iter->effetti[i].targetGiocatore;
                        if ((a == ELIMINA && t != IO) || a == RUBA ||
                            (iter->tipologia == MALUS && a == ELIMINA && t == IO)) {
                            scelta = iter;
                            trovatoEffetto = true;
                        }
                        i++;
                    }
                    iter = iter->next;
                } while (scelta == NULL && iter != start);

                if (scelta != NULL) {
                    giocatore->carteMano = rimuoviCartaDaLista(giocatore->carteMano, scelta);

                    if (scelta->tipologia == BONUS || scelta->tipologia == MALUS)
                        giocatore->bonusMalus = aggiungiCartaInLista(giocatore->bonusMalus, scelta);
                    else if (scelta->tipologia == STUDENTE_SEMPLICE || scelta->tipologia == MATRICOLA)
                        giocatore->aula = aggiungiCartaInLista(giocatore->aula, scelta);
                    else
                        *mazzoScarti = aggiungiCartaInLista(*mazzoScarti, scelta);

                    printf(BGRN "IA ha giocato tatticamente: %s\n" RESET, scelta->nome);
                    scriviLogTurno(giocatore->nome, "gioca", scelta->nome);
                    attivaEffetto(giocatore, scelta, mazzoPesca, aulaStudio, mazzoScarti, true);
                    haGiocato = true;
                }
            }
        }

        // 3. Fallback classico
        if (!haGiocato) {
            TipoCarta priorita[] = {STUDENTE_SEMPLICE, BONUS, MAGIA, MALUS};
            Carta *cartaScelta = NULL;
            int i = 0;

            while (i < 4 && cartaScelta == NULL) {
                iter = giocatore->carteMano;
                start = iter;
                bool trovata = false;

                do {
                    if (!trovata && iter->tipologia == priorita[i]) {
                        cartaScelta = iter;
                        trovata = true;
                    }
                    iter = iter->next;
                } while (iter != start && !trovata);

                i++;
            }

            if (cartaScelta != NULL) {
                giocatore->carteMano = rimuoviCartaDaLista(giocatore->carteMano, cartaScelta);

                if (cartaScelta->tipologia == STUDENTE_SEMPLICE || cartaScelta->tipologia == MATRICOLA)
                    giocatore->aula = aggiungiCartaInLista(giocatore->aula, cartaScelta);
                else if (cartaScelta->tipologia == BONUS || cartaScelta->tipologia == MALUS)
                    giocatore->bonusMalus = aggiungiCartaInLista(giocatore->bonusMalus, cartaScelta);
                else
                    *mazzoScarti = aggiungiCartaInLista(*mazzoScarti, cartaScelta);

                printf(BGRN "IA ha giocato la carta: %s\n" RESET, cartaScelta->nome);
                scriviLogTurno(giocatore->nome, "gioca", cartaScelta->nome);
                attivaEffetto(giocatore, cartaScelta, mazzoPesca, aulaStudio, mazzoScarti, true);
            } else {
                printf(BYEL "IA non ha carte utili da giocare.\n" RESET);
            }
        }
    }
}


/**
 * fineTurnoIA:
 *  - Se l'IA ha più di NCARTEMAX carte in mano, scarta automaticamente
 *    le carte meno utili secondo un ordine di priorità.
 */
void fineTurnoIA(Carta **mazzoScarti, Carta **aulaStudio, Giocatore *giocatore, Carta **mazzoPesca) {
    TipoCarta priorita[] = {MAGIA, STUDENTE_SEMPLICE, MALUS, BONUS, LAUREANDO};

    while (contaCarte(giocatore->carteMano) > NCARTEMAX) {
        Carta *cartaScarto = NULL;
        int i = 0;

        while (i < 5 && cartaScarto == NULL) {
            Carta *iter = giocatore->carteMano;
            Carta *start = iter;
            bool trovata = false;

            do {
                if (!trovata && iter->tipologia == priorita[i]) {
                    cartaScarto = iter;
                    trovata = true;
                }
                iter = iter->next;
            } while (iter != start && !trovata);

            i++;
        }

        if (cartaScarto == NULL) {
            cartaScarto = giocatore->carteMano;
        }

        printf(BYEL "L'IA scarta: %s\n" RESET, cartaScarto->nome);
        scartaCartaMano(mazzoScarti, aulaStudio, giocatore, cartaScarto);
    }

    // Incremento turno nel log
    incrementaTurnoLog();
}


/**
 * attivaEffettiInizioTurnoIA:
 *  - Attiva tutti gli effetti SEMPRE per tutti i giocatori
 *  - Attiva effetti INIZIO solo per il giocatore IA
 *  - L'IA attiva automaticamente gli effetti opzionali (non chiede conferma)
 */
void attivaEffettiInizioTurnoIA(Giocatore *giocatore, Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti) {
    Giocatore *iter = giocatore;
    do {
        // Effetti SEMPRE - aula
        Carta *carta = iter->aula;
        Carta *start = carta;
        if (carta != NULL) {
            do {
                if (carta->quando == SEMPRE) {
                    attivaEffetto(iter, carta, mazzoPesca, aulaStudio, mazzoScarti, true);
                }
                carta = carta->next;
            } while (carta != NULL && carta != start);
        }

        // Effetti SEMPRE - bonus/malus
        carta = iter->bonusMalus;
        start = carta;
        if (carta != NULL) {
            do {
                if (carta->quando == SEMPRE) {
                    attivaEffetto(iter, carta, mazzoPesca, aulaStudio, mazzoScarti, true);
                }
                carta = carta->next;
            } while (carta != NULL && carta != start);
        }

        iter = iter->next;
    } while (iter != giocatore);

    // Effetti INIZIO - solo IA - con attivazione random 50%
    Carta *cartaCorrente = giocatore->aula;
    Carta *primaCarta = giocatore->aula;
    if (cartaCorrente != NULL) {
        do {
            if (cartaCorrente->quando == INIZIO && (rand() % 2 == 0)) {
                attivaEffetto(giocatore, cartaCorrente, mazzoPesca, aulaStudio, mazzoScarti, true);
            }
            cartaCorrente = cartaCorrente->next;
        } while (cartaCorrente != NULL && cartaCorrente != primaCarta);
    }

    cartaCorrente = giocatore->bonusMalus;
    primaCarta = giocatore->bonusMalus;
    if (cartaCorrente != NULL) {
        do {
            // Logica IA random 33% di ATTIVARE
            if (cartaCorrente->quando == INIZIO && (rand() % 3 == 0)) {
                attivaEffetto(giocatore, cartaCorrente, mazzoPesca, aulaStudio, mazzoScarti, true);
            }
            cartaCorrente = cartaCorrente->next;
        } while (cartaCorrente != NULL && cartaCorrente != primaCarta);
    }
}

