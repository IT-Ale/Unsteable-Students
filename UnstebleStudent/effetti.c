/***************************************************************************
 *  Contiene la logica per attivare gli effetti di una carta giocata
 *  (GIOCA, SCARTA, ELIMINA, PESCA, ...).
 *  Include anche la funzione per scegliere il giocatore bersaglio.
 ***************************************************************************/

#include "header.h"
#include "ANSI.h"

/**
 * attivaEffetto:
 *  - Riceve la cartaGiocata (che potrebbe avere più effetti).
 *  - Per ogni effetto, esegue la funzione corrispondente.
 */
void attivaEffetto(Giocatore *giocatoreCorrente, Carta *cartaGiocata, Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti, bool isIA) {
    bool effettoBloccato = false;

    for (int i = 0; i < cartaGiocata->numEffetti && !effettoBloccato; i++) {
        Effetto e = cartaGiocata->effetti[i];

        if (e.targetGiocatore == IO) {
            eseguiEffetto(e, giocatoreCorrente, giocatoreCorrente, mazzoPesca, aulaStudio, mazzoScarti, isIA);
        }

        else if (e.targetGiocatore == TU) {
            Giocatore *bersaglio = scegliGiocatre(giocatoreCorrente, isIA);

            if (!effettoBlocca(bersaglio, mazzoScarti, isIA)) {
                eseguiEffetto(e, bersaglio, giocatoreCorrente, mazzoPesca, aulaStudio, mazzoScarti, isIA);
            } else {
                printf(BBLU "L'effetto e' stato annullato da %s con una carta 'MAI'!\n" RESET, bersaglio->nome);
                effettoBloccato = true;

                if (cartaGiocata->tipologia == BONUS || cartaGiocata->tipologia == MALUS) {
                    giocatoreCorrente->bonusMalus = rimuoviCartaDaLista(giocatoreCorrente->bonusMalus, cartaGiocata);
                    *mazzoScarti = aggiungiCartaInLista(*mazzoScarti, cartaGiocata);
                    printf(BRED "La carta '%s' e' stata scartata negli scarti dopo essere stata bloccata!\n" RESET, cartaGiocata->nome);
                }
            }
        }

        else if (e.targetGiocatore == VOI || e.targetGiocatore == TUTTI) {
            Giocatore *iter = giocatoreCorrente->next;
            bool interrotto = false;

            do {
                if (!effettoBlocca(iter, mazzoScarti, isIA)) {
                    printf(BBLU "Attivazione effetto per il giocatore %s.\n" RESET, iter->nome);
                    eseguiEffetto(e, iter, giocatoreCorrente, mazzoPesca, aulaStudio, mazzoScarti, isIA);
                } else {
                    printf(BRED "Il giocatore %s ha bloccato l'effetto.\n" RESET, iter->nome);
                    effettoBloccato = true;
                    interrotto = true;

                    if (cartaGiocata->tipologia == BONUS || cartaGiocata->tipologia == MALUS) {
                        giocatoreCorrente->bonusMalus = rimuoviCartaDaLista(giocatoreCorrente->bonusMalus, cartaGiocata);
                        *mazzoScarti = aggiungiCartaInLista(*mazzoScarti, cartaGiocata);
                        printf(BRED "La carta '%s' e' stata scartata negli scarti dopo essere stata bloccata!\n" RESET, cartaGiocata->nome);
                    }
                }

                iter = iter->next;
            } while (iter != giocatoreCorrente && !interrotto);

            if (e.targetGiocatore == TUTTI && !effettoBloccato) {
                eseguiEffetto(e, giocatoreCorrente, giocatoreCorrente, mazzoPesca, aulaStudio, mazzoScarti, isIA);
            }
        }

        if (effettoBloccato) {
            printf(BRED "Tutti gli effetti successivi della carta '%s' sono stati annullati.\n" RESET, cartaGiocata->nome);
        }
    }
}


/**
 * Funzione che esegue l'effetto in base al tipo di azione
 */
void eseguiEffetto(Effetto e, Giocatore *g, Giocatore *c, Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti, bool isIA) {
    if (g == NULL)
        exit(1);

    switch (e.azione) {
        case GIOCA:
            effettoGiocaCarta(e, g, mazzoPesca, aulaStudio, mazzoScarti, isIA);
        break;
        case SCARTA:
            effettoScarta(e, g, mazzoScarti, aulaStudio, isIA);
        break;
        case ELIMINA:
            effettoElimina(e, g, mazzoScarti, aulaStudio, mazzoPesca, isIA);
        break;
        case RUBA:
            effettoRuba(e, g, c, mazzoScarti, aulaStudio, isIA);
        break;
        case PESCA:
            effettoPesca(e, g, mazzoPesca, mazzoScarti);
        break;
        case PRENDI:
            effettoPrendi(e, g, c, isIA);
        break;
        case SCAMBIA:
            effettoScambia(e, g, c);
        break;
        case MOSTRA:
            effettoMostra(e, g);
        break;
        default:
            printf(BRED "Errore: Azione effetto non riconosciuta\n" RESET);
        break;
    }
}

/**
 * Sceglie un altro giocatore per alcuni effetti (TU).
 */
Giocatore *scegliGiocatre(Giocatore *caller, bool isIA) {
    char nomeScelto[NOME];
    Giocatore *current;
    if (!isIA) {
        int trovato;
        do {
            printf("Inserisci il nome del giocatore da scegliere: ");
            scanf(" %[^\n]", nomeScelto);
            while (getchar() != '\n');
            if (strcmp(caller->nome, nomeScelto) == 0) {
                printf(BRED "Non puoi scegliere te stesso! Riprova.\n" RESET);
            } else {
                current = caller->next;
                trovato = 0;
                do {
                    if (strcmp(current->nome, nomeScelto) == 0) {
                        trovato = 1;
                    } else {
                        current = current->next;
                    }
                } while (!trovato && current != caller);
                if (!trovato) {
                    printf(BRED "Giocatore non trovato, riprova.\n" RESET);
                }
            }
        } while (!trovato);
    } else {
        // IA: scegli il giocatore con più studenti in aula (minaccia maggiore)
        Giocatore *target = NULL;
        int maxStudenti = -1;
        Giocatore *iter = caller->next;
        do {
            int studenti = contaCarte(iter->aula);
            if (studenti > maxStudenti) {
                maxStudenti = studenti;
                target = iter;
            }
            iter = iter->next;
        } while (iter != caller);
        current = target;
    }
    return current;
}

/**
 * effettoPesca:
 *  - In base a targetGiocatore (IO, TU, VOI, TUTTI),
 *    pesca dal mazzoPesca per il giocatore corrispondente.
 */
void effettoPesca(Effetto effettoCorrente, Giocatore *giocatore, Carta **mazzoPesca, Carta **mazzoScarti) {
    printf(BBLU "Attivazione effetto: Pesca per %s\n" RESET, giocatore->nome);
    pescata(mazzoPesca, mazzoScarti, giocatore);
    resocontoGiocatore(giocatore);
}


/**
 * effettoScarta:
 *  - Fa scartare "dalla mano" la carta (targetGiocatore: IO, TU, VOI, TUTTI).
 *  - NOTA: qui scartare significa proprio scartare dalla mano, usando scartaCartaMano.
 */
void effettoScarta(Effetto effettoCorrente, Giocatore *giocatore, Carta **mazzoScarti, Carta **aulaStudio, bool isIA) {
    printf(BBLU "Attivazione effetto: Scarta per %s\n" RESET, giocatore->nome);

    int numCarte = contaCarte(giocatore->carteMano);
    if (numCarte == 0) {
        printf("Il giocatore %s non ha carte da scartare.\n", giocatore->nome);
    } else {
        stampaLista(giocatore->carteMano);
        int scelta;

        if (!isIA) {
            printf("Scegli il numero della carta da scartare (1-%d): ", numCarte);
            scanf("%d", &scelta);
            while (scelta < 1 || scelta > numCarte) {
                printf(BRED "Numero non valido, riprova: " RESET);
                scanf("%d", &scelta);
            }

            Carta *cartaDaScartare = OttieniPosizioneCarta(giocatore->carteMano, scelta - 1);
            scartaCartaMano(mazzoScarti, aulaStudio, giocatore, cartaDaScartare);

        } else {
            // IA: seleziona in base alla priorità
            TipoCarta priorita[] = {MAGIA, MALUS, BONUS, STUDENTE_SEMPLICE, LAUREANDO};
            Carta *cartaDaScartare = NULL;
            int i = 0;

            while (i < 5 && cartaDaScartare == NULL) {
                Carta *tmp = giocatore->carteMano;
                if (tmp != NULL) {
                    Carta *start = tmp;
                    bool trovata = false;
                    do {
                        if (!trovata && tmp->tipologia == priorita[i]) {
                            cartaDaScartare = tmp;
                            trovata = true;
                        }
                        tmp = tmp->next;
                    } while (tmp != start && !trovata);
                }
                i++;
            }

            if (cartaDaScartare == NULL) {
                cartaDaScartare = giocatore->carteMano;
            }

            scartaCartaMano(mazzoScarti, aulaStudio, giocatore, cartaDaScartare);
        }
    }
}



/**
 * effettoGiocaCarta:
 *  - Permette di giocare (dalla mano) una carta e metterla in aula o bonus/malus a seconda della tipologia.
 */
void effettoGiocaCarta(Effetto effettoCorrente, Giocatore *giocatore, Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti, bool isIA) {
    printf(BBLU "Attivazione effetto: Gioca Carta" RESET "\n");
    stampaLista(giocatore->carteMano);
    if (isIA == false) {
        giocaCarta(mazzoPesca, aulaStudio, mazzoScarti, giocatore);
    } else {
        giocaCartaIA(mazzoPesca, aulaStudio, mazzoScarti, giocatore);
    }
}

/**
 * scartaDaAula:
 *  - Fa scegliere al giocatore "g" quale carta rimuovere dalla sua aula,
 *    e poi la scarta con la funzione scartaCartaAula(...).
 */
void scartaDaAula(Giocatore *g, Carta **mazzoScarti, Carta **aulaStudio, Carta **mazzoPesca, bool isIA) {
    int scelta, numCarte;

    printf("Aula di %s:\n", g->nome);
    stampaLista(g->aula);

    numCarte = contaCarte(g->aula);
    if (numCarte == 0) {
        printf("Il giocatore %s non ha carte da scartare in aula.\n", g->nome);
    } else {
        Carta *cartaDaScartare = NULL;

        if (!isIA) {
            printf("Scegli il numero della carta da scartare (1-%d): ", numCarte);
            scanf("%d", &scelta);
            while (scelta < 1 || scelta > numCarte) {
                printf(RED "Numero non valido, riprova:\n" RESET);
                scanf("%d", &scelta);
            }
            cartaDaScartare = OttieniPosizioneCarta(g->aula, scelta - 1);
        } else {
            // IA: cerca una carta con effetto "FINE"
            Carta *iter = g->aula;
            bool trovata = false;

            if (iter != NULL) {
                Carta *start = iter;
                do {
                    if (!trovata && iter->quando == FINE) {
                        cartaDaScartare = iter;
                        trovata = true;
                    }
                    iter = iter->next;
                } while (iter != start && !trovata);
            }

            if (cartaDaScartare == NULL) {
                cartaDaScartare = g->aula;
            }
        }

        scartaCartaAula(mazzoScarti, aulaStudio, mazzoPesca, g, cartaDaScartare, isIA);
    }
}


/**
 * scartaDaBonusMalus:
 *  - Fa scegliere una carta di tipo "targetType" (BONUS o MALUS)
 *    dalla lista bonusMalus del giocatore g, e la scarta negli scarti.
 */
void scartaDaBonusMalus(Giocatore *g, Carta **mazzoScarti, TipoCarta targetType, bool isIA) {
    int tot = contaCarte(g->bonusMalus);
    bool haCarte = (tot > 0);

    if (!haCarte) {
        printf("Il giocatore %s non ha carte in Bonus/Malus.\n", g->nome);
    }

    if (haCarte) {
        int countOk = 0;
        Carta *temp = g->bonusMalus;
        if (temp != NULL) {
            do {
                if (temp->tipologia == targetType) {
                    countOk++;
                }
                temp = temp->next;
            } while (temp != g->bonusMalus);
        }

        if (countOk == 0) {
            printf("Non ci sono carte di tipo %s da scartare per %s.\n",
                   (targetType == BONUS ? "BONUS" : "MALUS"), g->nome);
        } else {
            Carta **elenco = (Carta **)malloc(countOk * sizeof(Carta *));
            bool erroreAllocazione = (elenco == NULL);

            if (erroreAllocazione) {
                printf(BRED "Errore allocazione memoria" RESET "\n");
                exit(1);
            }

            if (!erroreAllocazione) {
                temp = g->bonusMalus;
                int i = 0;
                do {
                    if (temp->tipologia == targetType) {
                        elenco[i] = temp;
                        i++;
                    }
                    temp = temp->next;
                } while (temp != g->bonusMalus);

                printf("Carte di tipo %s nel bonus/malus di %s:\n",
                       (targetType == BONUS ? "BONUS" : "MALUS"), g->nome);
                for (int j = 0; j < countOk; j++) {
                    printf("  %d) %s\n", j + 1, elenco[j]->nome);
                }

                Carta *cartaDaScartare = NULL;

                if (!isIA) {
                    int scelta = 0;
                    printf("Scegli il numero della carta da scartare (1-%d): ", countOk);
                    scanf("%d", &scelta);
                    while (scelta < 1 || scelta > countOk) {
                        printf(RED "Numero non valido, riprova:\n" RESET);
                        scanf("%d", &scelta);
                    }
                    cartaDaScartare = elenco[scelta - 1];
                } else {
                    bool trovata = false;
                    int i = 0;
                    while (i < countOk && !trovata) {
                        bool effettoImportante = false;
                        int j = 0;
                        while (j < elenco[i]->numEffetti && !effettoImportante) {
                            Azione azione = elenco[i]->effetti[j].azione;
                            if (azione == RUBA || azione == GIOCA) {
                                effettoImportante = true;
                            }
                            j++;
                        }
                        if (!effettoImportante && !trovata) {
                            cartaDaScartare = elenco[i];
                            trovata = true;
                        }
                        i++;
                    }

                    if (!trovata) {
                        cartaDaScartare = elenco[0];
                    }
                }

                if (cartaDaScartare != NULL) {
                    scartaCartaBonusMalus(mazzoScarti, g, cartaDaScartare);
                }

                free(elenco);
            }
        }
    }
}

/**
 * scartaDaAll:
 *  - Chiede al giocatore se vuole scartare dall'aula o dal bonus/malus.
 *  - Se sceglie bonus/malus, chiede se scartare BONUS o MALUS.
 */
void scartaDaAll(Giocatore *g, Carta **mazzoScarti, Carta **aulaStudio, Carta **mazzoPesca, bool isIA) {
    int scelta;
    if (isIA == false) {
        do {
            printf("\nVuoi scartare una carta dall' " BGRN "Aula" RESET " (1) "
                   "oppure dal " BMAG "Bonus/Malus" RESET " (2)?\n");
            scanf("%d", &scelta);

            if (scelta == 1) {
                scartaDaAula(g, mazzoScarti, aulaStudio, mazzoPesca, isIA);
            }
            else if (scelta == 2) {
                int scelta2;
                do {
                    printf("\nVuoi scartare una carta di tipo " BGRN "BONUS" RESET " (1) "
                           "oppure " BRED "MALUS" RESET " (2)?\n");
                    scanf("%d", &scelta2);

                    if (scelta2 == 1) {
                        scartaDaBonusMalus(g, mazzoScarti, BONUS, isIA);
                    }
                    else if (scelta2 == 2) {
                        scartaDaBonusMalus(g, mazzoScarti, MALUS, isIA);
                    } else {
                        printf(BRED "Scelta non valida.\n" RESET);
                    }
                } while (scelta2 != 1 && scelta2 != 2);
            }
            else {
                printf(BRED "Scelta non valida.\n" RESET);
            }

        } while (scelta != 1 && scelta != 2);
    } else {
        // IA : se ha carte nell'aula scarta dall'aula, altrimenti scarta un bonus
        if (g->aula != NULL) {
            scartaDaAula(g, mazzoScarti, aulaStudio, mazzoPesca, isIA);
        } else {
            scartaDaBonusMalus(g, mazzoScarti, BONUS, isIA);
        }
    }
}


/**
 * effettoElimina:
 *  - Elimina carte dall'aula o bonus/malus, in base a targetCarta (STUDENTE, BONUS, MALUS, ALL)
 *  - targetGiocatore = IO, TU, VOI, TUTTI
 */
void effettoElimina(Effetto effettoCorrente, Giocatore *giocatore, Carta **mazzoScarti, Carta **aulaStudio, Carta **mazzoPesca, bool isIA) {
    printf(BBLU "Attivazione effetto: Elimina\n" RESET);

    switch (effettoCorrente.targetCarta) {
        case STUDENTE:
            scartaDaAula(giocatore, mazzoScarti, aulaStudio, mazzoPesca, isIA);
        break;
        case BONUS:
            scartaDaBonusMalus(giocatore, mazzoScarti, BONUS, isIA);
        break;
        case MALUS:
            scartaDaBonusMalus(giocatore, mazzoScarti, MALUS, isIA);
        break;
        case ALL:
            scartaDaAll(giocatore, mazzoScarti, aulaStudio, mazzoPesca, isIA);
        break;
        default:
            printf(BRED "Tipo carta da eliminare non riconosciuto\n" RESET);
    }
}


/**
 * effettoRuba:
 *  - Gestisce l'effetto "RUBA", che permette di prendere una carta
 *    dall'aula o dal bonus/malus di un altro giocatore e metterla
 *    nella propria aula o zona bonus/malus.
 *  - targetGiocatore: TU (singolo bersaglio scelto) o VOI (tutti i giocatori
 *    tranne chi ruba).
 *  - targetCarta: STUDENTE, BONUS, MALUS, ALL (come effettoElimina).
*/
void effettoRuba(Effetto effettoCorrente, Giocatore *bersaglio, Giocatore *corrente, Carta **mazzoScarti, Carta **aulaStudio, bool isIA) {
    printf(BBLU "Attivazione effetto: Ruba" RESET "\n");

    bool procedi = true;

    if (effettoCorrente.targetGiocatore == TU) {
        if (bersaglio == NULL) {
            printf(BRED "Nessun giocatore valido selezionato.\n" RESET);
            procedi = false;
        } else {
            if (effettoCorrente.targetCarta == STUDENTE) {
                rubaDaAula(bersaglio, corrente, isIA);
            } else if (effettoCorrente.targetCarta == BONUS) {
                rubaDaBonusMalus(bersaglio, corrente, BONUS, isIA);
            } else if (effettoCorrente.targetCarta == MALUS) {
                rubaDaBonusMalus(bersaglio, corrente, MALUS, isIA);
            } else if (effettoCorrente.targetCarta == ALL) {
                rubaDaAll(bersaglio, corrente, isIA);
            }
        }
    }

    if (procedi && effettoCorrente.targetGiocatore == VOI) {
        Giocatore *iter = corrente->next;
        while (iter != corrente) {
            if (effettoCorrente.targetCarta == STUDENTE) {
                rubaDaAula(iter, corrente, isIA);
            } else if (effettoCorrente.targetCarta == BONUS) {
                rubaDaBonusMalus(iter, corrente, BONUS, isIA);
            } else if (effettoCorrente.targetCarta == MALUS) {
                rubaDaBonusMalus(iter, corrente, MALUS, isIA);
            } else if (effettoCorrente.targetCarta == ALL) {
                rubaDaAll(iter, corrente, isIA);
            }
            iter = iter->next;
        }
    }
}


/**
 * rubaDaAula:
 *  - Ruba una carta dall'aula del bersaglio e la mette nell'aula del ladro.
 */
void rubaDaAula(Giocatore *bersaglio, Giocatore *chiRuba, bool isIA) {
    int scelta, numCarte;
    printf("\n-- RUBA DA AULA --\n");
    printf("Aula di %s:\n", bersaglio->nome);
    stampaLista(bersaglio->aula);

    numCarte = contaCarte(bersaglio->aula);
    bool haCarte = (numCarte > 0);

    if (!haCarte) {
        printf("Il giocatore %s non ha carte in aula.\n", bersaglio->nome);
    }

    if (haCarte && !isIA) {
        printf("Scegli il numero della carta da rubare (1-%d): ", numCarte);
        scanf("%d", &scelta);
        while (scelta < 1 || scelta > numCarte) {
            printf(RED "Numero non valido, riprova:\n" RESET);
            scanf("%d", &scelta);
        }

        Carta *cartaRubata = OttieniPosizioneCarta(bersaglio->aula, scelta - 1);
        bersaglio->aula = rimuoviCartaDaLista(bersaglio->aula, cartaRubata);
        chiRuba->aula = aggiungiCartaInLista(chiRuba->aula, cartaRubata);

        printf(BBLU "Carta " BYEL "%s" BBLU " rubata da %s e aggiunta all'aula di %s\n" RESET,
               cartaRubata->nome, bersaglio->nome, chiRuba->nome);
    }

    if (haCarte && isIA) {
        // IA: cerca di rubare una carta con effetto FINE, altrimenti prende la prima disponibile
        Carta *cartaDaRubare = NULL;
        Carta *iter = bersaglio->aula;

        if (iter != NULL) {
            Carta *start = iter;
            do {
                if (iter->quando == FINE) {
                    cartaDaRubare = iter;
                }
                iter = iter->next;
            } while (iter != start && cartaDaRubare == NULL);

            // Se nessuna carta ha effetto FINE, prendi la prima
            if (cartaDaRubare == NULL) {
                cartaDaRubare = bersaglio->aula;
            }

            bersaglio->aula = rimuoviCartaDaLista(bersaglio->aula, cartaDaRubare);
            chiRuba->aula = aggiungiCartaInLista(chiRuba->aula, cartaDaRubare);
            printf(BGRN "IA ha rubato la carta '%s' dall'aula di %s\n" RESET, cartaDaRubare->nome, bersaglio->nome);
        }
    }
}

/**
 * rubaDaBonusMalus:
 *  - Ruba una carta di tipo BONUS/MALUS e la mette automaticamente
 *    nella zona Bonus/Malus di chi ruba.
*/
void rubaDaBonusMalus(Giocatore *bersaglio, Giocatore *chiRuba, TipoCarta targetType, bool isIA) {
    int tot = contaCarte(bersaglio->bonusMalus);
    bool procedi = (tot > 0);

    if (!procedi) {
        printf("\nIl giocatore %s non ha carte in Bonus/Malus.\n", bersaglio->nome);
    }

    int countOk = 0;
    Carta *temp = bersaglio->bonusMalus;

    if (procedi) {
        if (temp != NULL) {
            do {
                if (temp->tipologia == targetType) {
                    countOk++;
                }
                temp = temp->next;
            } while (temp != bersaglio->bonusMalus);
        }

        if (countOk == 0) {
            printf("Non ci sono carte di tipo %s da rubare a %s.\n",
                   (targetType == BONUS ? "BONUS" : "MALUS"), bersaglio->nome);
            procedi = false;
        }
    }

    Carta **elenco = NULL;
    bool allocato = false;

    if (procedi) {
        elenco = (Carta **) malloc(countOk * sizeof(Carta *));
        if (elenco != NULL) {
            allocato = true;
        } else {
            printf(BRED "Errore allocazione memoria" RESET "\n");
            procedi = false;
        }
    }

    if (procedi && allocato) {
        temp = bersaglio->bonusMalus;
        int i = 0;
        do {
            if (temp->tipologia == targetType) {
                elenco[i] = temp;
                i++;
            }
            temp = temp->next;
        } while (temp != bersaglio->bonusMalus);

        printf("\n-- RUBA DA BONUS/MALUS --\n");
        printf("Carte di tipo %s nel bonus/malus di %s:\n",
               (targetType == BONUS ? "BONUS" : "MALUS"), bersaglio->nome);
        for (int j = 0; j < countOk; j++) {
            printf("  %d) %s\n", j + 1, elenco[j]->nome);
        }

        Carta *cartaRubata = NULL;

        if (!isIA) {
            int scelta;
            printf("Scegli il numero della carta da rubare (1-%d): ", countOk);
            scanf("%d", &scelta);
            while (scelta < 1 || scelta > countOk) {
                printf(RED "Numero non valido, riprova:\n" RESET);
                scanf("%d", &scelta);
            }
            cartaRubata = elenco[scelta - 1];
        } else {
            int i = 0;
            bool trovata = false;
            while (i < countOk && !trovata) {
                Carta *c = elenco[i];
                int j = 0;
                while (j < c->numEffetti && !trovata) {
                    Azione a = c->effetti[j].azione;
                    if (a == RUBA || a == GIOCA) {
                        cartaRubata = c;
                        trovata = true;
                    }
                    j++;
                }
                i++;
            }

            if (!trovata) {
                cartaRubata = elenco[0];
            }

            printf(BGRN "IA ha rubato la carta '%s' (%s) da %s\n" RESET,
                   cartaRubata->nome,
                   (targetType == BONUS ? "BONUS" : "MALUS"),
                   bersaglio->nome);
        }

        // Rubo la carta e aggiorno le liste
        if (cartaRubata != NULL) {
            bersaglio->bonusMalus = rimuoviCartaDaLista(bersaglio->bonusMalus, cartaRubata);
            chiRuba->bonusMalus = aggiungiCartaInLista(chiRuba->bonusMalus, cartaRubata);
        }

        if (!isIA && cartaRubata != NULL) {
            printf(BBLU "Carta " BYEL "%s" BBLU " rubata da %s e aggiunta alla zona Bonus/Malus di %s\n" RESET,
                   cartaRubata->nome, bersaglio->nome, chiRuba->nome);
        }

        free(elenco);
    }
}


/**
 * rubaDaAll:
 *  - Permette di scegliere se rubare dall'aula o dal Bonus/Malus.
*/
void rubaDaAll(Giocatore *bersaglio, Giocatore *chiRuba, bool isIA) {
    if (isIA == false) {
        int scelta;
        printf("\nVuoi rubare una carta dall' " BGRN "Aula" RESET " (1) oppure dal " BMAG "Bonus/Malus" RESET " (2)?\n");
        scanf("%d", &scelta);

        while (scelta != 1 && scelta != 2) {
            printf(BRED "Scelta non valida.\n" RESET);
            scanf("%d", &scelta);
        }

        if (scelta == 1) {
            rubaDaAula(bersaglio, chiRuba, isIA);
        } else {
            int sceltaTipo;
            printf("\nVuoi rubare una carta di tipo " BGRN "BONUS" RESET " (1) oppure " BRED "MALUS" RESET " (2)?\n");
            scanf("%d", &sceltaTipo);

            while (sceltaTipo != 1 && sceltaTipo != 2) {
                printf(BRED "Scelta non valida.\n" RESET);
                scanf("%d", &sceltaTipo);
            }

            rubaDaBonusMalus(bersaglio, chiRuba, (sceltaTipo == 1 ? BONUS : MALUS), isIA);
        }
    } else {
        // IA: prima prova ad accedere all’aula
        if (contaCarte(bersaglio->aula) > 0) {
            rubaDaAula(bersaglio, chiRuba, isIA);
        }
        // Se non ci sono carte in aula, prova con bonus
        else if (contaCarte(bersaglio->bonusMalus) > 0) {
            int haBonus = 0, haMalus = 0;
            Carta *tmp = bersaglio->bonusMalus;
            if (tmp != NULL) {
                do {
                    if (tmp->tipologia == BONUS) haBonus = 1;
                    if (tmp->tipologia == MALUS) haMalus = 1;
                    tmp = tmp->next;
                } while (tmp != bersaglio->bonusMalus);
            }

            if (haBonus) {
                rubaDaBonusMalus(bersaglio, chiRuba, BONUS, isIA);
            } else if (haMalus) {
                rubaDaBonusMalus(bersaglio, chiRuba, MALUS, isIA);
            } else {
                printf(BYEL "IA: Nessuna carta utile da rubare da %s\n" RESET, bersaglio->nome);
            }
        } else {
            printf(BYEL "IA: Nessuna carta da rubare da %s\n" RESET, bersaglio->nome);
        }
    }
}

/**
 * Effetto Prendi:
 *  - Permette di scegliere una carta dalla mano di un avversario scelto e rubarla.
*/
void effettoPrendi(Effetto effettoCorrente, Giocatore *bersaglio, Giocatore *corrente, bool isIA) {
    printf(BBLU "Attivazione effetto: Prendi\n" RESET);

    printf("Mano di %s:\n", bersaglio->nome);
    stampaLista(bersaglio->carteMano);

    int numCarte = contaCarte(bersaglio->carteMano);
    bool haCarte = (numCarte > 0);

    if (haCarte) {
        Carta *cartaRubata = NULL;

        if (!isIA) {
            int scelta;
            printf("Scegli il numero della carta da rubare (1-%d): ", numCarte);
            scanf("%d", &scelta);
            while (scelta < 1 || scelta > numCarte) {
                printf(BRED "Numero non valido, riprova: " RESET);
                scanf("%d", &scelta);
            }
            cartaRubata = OttieniPosizioneCarta(bersaglio->carteMano, scelta - 1);
        } else {
            TipoCarta priorita[] = {LAUREANDO, STUDENTE_SEMPLICE, BONUS, MALUS, MAGIA};
            int i = 0;
            bool trovata = false;

            while (i < 5 && !trovata) {
                Carta *tmp = bersaglio->carteMano;
                if (tmp != NULL) {
                    Carta *start = tmp;
                    bool matchInterno = false;
                    do {
                        if (!matchInterno && tmp->tipologia == priorita[i]) {
                            cartaRubata = tmp;
                            matchInterno = true;
                            trovata = true;
                        }
                        tmp = tmp->next;
                    } while (tmp != start && !matchInterno);
                }
                i++;
            }

            if (!trovata) {
                cartaRubata = bersaglio->carteMano;
            }
        }

        if (cartaRubata != NULL) {
            bersaglio->carteMano = rimuoviCartaDaLista(bersaglio->carteMano, cartaRubata);
            corrente->carteMano = aggiungiCartaInLista(corrente->carteMano, cartaRubata);

            if (!isIA) {
                printf(BBLU "Hai rubato la carta " BYEL "%s" BBLU " a %s.\n" RESET, cartaRubata->nome, bersaglio->nome);
            } else {
                printf(BGRN "IA ha rubato la carta '%s' dalla mano di %s\n" RESET, cartaRubata->nome, bersaglio->nome);
            }
        }

    } else {
        printf(BRED "Il giocatore %s non ha carte da rubare.\n" RESET, bersaglio->nome);
    }
}


/**
 * effettoMostra:
 *  - Funzione che, in base a targetGiocatore (IO, TU, VOI, TUTTI),
 *    mostra (stampa) la mano di chi è coinvolto.
*/
void effettoMostra(Effetto effettoCorrente, Giocatore *giocatore) {
    printf(BBLU "Attivazione effetto: Mostra carte di %s\n" RESET, giocatore->nome);
    stampaLista(giocatore->carteMano);
}

/**
 * effettoScambia:
 *  - Permette di scambiare la mano del giocatore che ha attivato la carta
 *    con un altro giocatore “target” (scelto se targetGiocatore == TU).
 *  - Se il targetGiocatore == IO, si interpreta che lo scambio è con se stesso
 *    (quindi non ha molto senso), oppure puoi gestirlo come "nessuna azione".
 */
void effettoScambia(Effetto effettoCorrente, Giocatore *bersaglio, Giocatore *corrente) {
    bool procedi = true;

    if (effettoCorrente.targetGiocatore == TU) {
        if (bersaglio == NULL) {
            printf(BRED "Nessun giocatore trovato, scambio annullato.\n" RESET);
            procedi = false;
        }

        if (procedi) {
            // Stampo info
            printf(BYEL "%s" RESET " scambia la mano con " BYEL "%s\n" RESET,
                   corrente->nome, bersaglio->nome);

            // Eseguo lo scambio dei puntatori alle carte in mano
            Carta *tempMano = corrente->carteMano;
            corrente->carteMano = bersaglio->carteMano;
            bersaglio->carteMano = tempMano;

            printf("Scambio completato!\n");
        }
    } else {
        printf(BRED "Effetto SCAMBIA: ERRORE.\n" RESET);
    }
}


/**
 * effettoBlocca:
 *  - Se il giocatore bersaglio ha una carta con nome "MAI" nella mano,
 *    può giocarla per annullare l'attivazione dell'effetto.
 *  - Non si può attivare un "MAI" in risposta a un altro "MAI".
 *  - Ritorna true se l'effetto viene bloccato, altrimenti false.
 */
bool effettoBlocca(Giocatore *bersaglio, Carta **mazzoScarti, bool isIA) {
    bool effettoAnnullato = false;
    Carta *cartaMai = NULL;

    // Cerca la carta "MAI" nella mano del bersaglio
    if (bersaglio->carteMano != NULL) {
        Carta *temp = bersaglio->carteMano;
        Carta *start = temp;
        bool trovato = false;
        do {
            if (!trovato && strcmp(temp->nome, "MAI") == 0) {
                cartaMai = temp;
                trovato = true;
            }
            temp = temp->next;
        } while (temp != start);
    }

    if (!isIA) {
        if (cartaMai != NULL) {
            // Chiede al giocatore umano se vuole annullare
            printf(BGRN "Giocatore %s, vuoi annullare l'effetto giocando la carta 'MAI'? (1=Si, 0=No)\n" RESET, bersaglio->nome);
            int scelta = -1;
            while (scelta != 0 && scelta != 1) {
                scanf("%d", &scelta);
                if (scelta != 0 && scelta != 1) {
                    printf(BMAG "Scelta non valida, inserisci 1 per annullare o 0 per lasciare attivo l'effetto.\n" RESET);
                }
            }

            if (scelta == 1) {
                printf(BBLU "Carta 'MAI' giocata! Effetto annullato.\n" RESET);
                bersaglio->carteMano = rimuoviCartaDaLista(bersaglio->carteMano, cartaMai);
                *mazzoScarti = aggiungiCartaInLista(*mazzoScarti, cartaMai);
                effettoAnnullato = true;
            }
        }
    } else {
        // L'IA annulla sempre
        if (cartaMai != NULL) {
            bersaglio->carteMano = rimuoviCartaDaLista(bersaglio->carteMano, cartaMai);
            *mazzoScarti = aggiungiCartaInLista(*mazzoScarti, cartaMai);
            effettoAnnullato = true;
        }
    }

    return effettoAnnullato;
}

/**
 * attivaEffettoCartaUscitaDaAula:
 *  - Attiva eventuali effetti con "quando == FINE".
 */
void attivaEffettoCartaUscitaDaAula(Giocatore *giocatore, Carta *cartaUscita, Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti, bool isIA) {
    if (cartaUscita->quando == FINE) {
        printf(BBLU "Attivazione effetto perché la carta '%s' e' uscita dall'aula\n" RESET, cartaUscita->nome);
        attivaEffetto(giocatore, cartaUscita, mazzoPesca, aulaStudio, mazzoScarti, isIA);
    }
}

/**
 * BloccataDaImpedire:
 *  - Verifica se una carta che il giocatore vuole giocare è bloccata da
 *    un effetto di tipo IMPEDIRE presente nella sua lista bonus/malus.
 +    blocca la giocata e stampa un messaggio
 */
bool BloccataDaImpedire(Giocatore *g, Carta *cartaDaGiocare) {
    bool bloccata = false;

    // Controllo validità dei parametri
    if (g != NULL && cartaDaGiocare != NULL && g->bonusMalus != NULL) {
        Carta *temp = g->bonusMalus;
        Carta *start = temp;

        do {
            for (int i = 0; i < temp->numEffetti; i++) {
                Effetto e = temp->effetti[i];
                if (!bloccata && e.azione == IMPEDIRE && e.targetCarta == cartaDaGiocare->tipologia) {
                    printf(BRED "Non puoi giocare la carta '%s' perché e' bloccata da un effetto IMPEDIRE (%s)\n" RESET,
                           cartaDaGiocare->nome, temp->nome);
                    bloccata = true;
                }
            }
            temp = temp->next;
        } while (temp != NULL && temp != start && !bloccata);
    }

    return bloccata;
}


