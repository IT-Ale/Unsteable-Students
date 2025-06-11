/***************************************************************************
 *  Implementa le funzioni per gestire le liste circolari di Carte,
 *  come aggiungere una carta, rimuovere, copiare e stampare.
 ***************************************************************************/

#include "header.h"
#include "ANSI.h"

/**
 * aggiungiCartaInLista:
 *  - Inserisce una nuova carta alla fine della lista circolare.
 *  - Se la lista è vuota, la nuova carta punta a sé stessa.
 *  - Ritorna il nuovo puntatore alla lista.
 */
Carta *aggiungiCartaInLista(Carta *mazzo, Carta *nuovaCarta) {
    if (mazzo == NULL) {
        nuovaCarta->next = nuovaCarta;
        mazzo = nuovaCarta;
    } else {
        Carta *temp = mazzo;
        while (temp->next != mazzo) {
            temp = temp->next;
        }
        temp->next = nuovaCarta;
        nuovaCarta->next = mazzo;
    }

    return mazzo;
}


/**
 * rimuoviCartaDaLista:
 *  - Rimuove una carta specifica da una lista circolare.
 *  - Se la carta è l'unica presente, la lista diventa NULL.
 *  - Non viene fatto free: gestito esternamente.
 */
Carta *rimuoviCartaDaLista(Carta *mazzo, Carta *cartaDaRimuovere) {
    bool daRimuovere = false;

    if (mazzo != NULL && cartaDaRimuovere != NULL) {
        // Caso con una sola carta
        if (mazzo->next == mazzo) {
            if (mazzo == cartaDaRimuovere) {
                cartaDaRimuovere->next = NULL;
                mazzo = NULL;
            }
        } else {
            Carta *current = mazzo;
            Carta *prev = NULL;
            bool cicloCompletato = false;

            do {
                if (!daRimuovere && current == cartaDaRimuovere) {
                    if (current == mazzo) {
                        Carta *ultimo = mazzo;
                        while (ultimo->next != mazzo) {
                            ultimo = ultimo->next;
                        }
                        mazzo = mazzo->next;
                        ultimo->next = mazzo;
                    } else if (prev != NULL) {
                        prev->next = current->next;
                    }
                    cartaDaRimuovere->next = NULL;
                    daRimuovere = true;
                }

                prev = current;
                current = current->next;
                cicloCompletato = (current == mazzo);
            } while (!cicloCompletato && !daRimuovere);
        }
    }

    return mazzo;
}


/**
 * OttieniPosizioneCarta:
 *  - Restituisce la carta nella posizione indicata (0-based).
 *  - Non controlla se la posizione è valida: deve essere < numero carte.
 */
Carta* OttieniPosizioneCarta(Carta *mazzo, int posizione) {
    Carta *temp = mazzo;
    for (int i = 0; i < posizione; i++) {
        temp = temp->next;
    }
    return temp;
}

/**
 * contaCarte:
 *  - Conta quante carte sono presenti nella lista circolare.
 *  - Ritorna 0 se lista nulla.
 */
int contaCarte(Carta *mazzo) {
    int count = 0;

    if (mazzo != NULL) {
        Carta *temp = mazzo;
        do {
            count++;
            temp = temp->next;
        } while (temp != mazzo);
    }

    return count;
}

/**
 * copiaCarta:
 *  - Effettua una copia profonda di una carta (incluse informazioni di effetti).
 *  - Il campo next rimane NULL (non la inserisce in lista).
 */
Carta* copiaCarta(Carta *orig) {
    Carta *copia = (Carta*) malloc(sizeof(Carta));
    if (copia == NULL) {
        printf(BRED "Errore allocazione memoria per copia carta" RESET "\n");
        exit(1);
    }

    strcpy(copia->nome, orig->nome);
    strcpy(copia->descrizione, orig->descrizione);
    copia->tipologia = orig->tipologia;
    copia->numEffetti = orig->numEffetti;
    copia->quando = orig->quando;
    copia->effettiUtilizzabili = orig->effettiUtilizzabili;

    if (orig->numEffetti > 0) {
        copia->effetti = (Effetto*) malloc(orig->numEffetti * sizeof(Effetto));
        if (copia->effetti == NULL) {
            printf(BRED "Errore allocazione memoria per effetti nella copia" RESET "\n");
            exit(1);
        }
        for (int i = 0; i < orig->numEffetti; i++) {
            copia->effetti[i] = orig->effetti[i];
        }
    } else {
        copia->effetti = NULL;
    }

    copia->next = NULL;
    return copia;
}

/**
 * stampaLista:
 *  - Stampa tutte le carte nella lista, incluse descrizioni ed effetti.
 *  - Se la lista è vuota, stampa "Nulla".
 */
void stampaLista(Carta *mazzo) {
    bool stampa = true;

    if (mazzo == NULL) {
        printf("Nulla\n");
        stampa = false;
    }

    if (stampa) {
        Carta *temp = mazzo;
        int count = 1;
        do {
            printf("Carta %d:\n", count);
            printf("  Nome:" YEL " %s" RESET "\n", temp->nome);
            printf("  Descrizione: %s\n", temp->descrizione);

            // Stampa la tipologia
            switch (temp->tipologia) {
                case ALL:
                    printf("  Tipo: Carta ALL\n");
                break;
                case STUDENTE:
                    printf("  Tipo: Carta STUDENTE\n");
                break;
                case MATRICOLA:
                    printf("  Tipo: Carta MATRICOLA\n");
                break;
                case STUDENTE_SEMPLICE:
                    printf("  Tipo: Carta STUDENTE_SEMPLICE\n");
                break;
                case LAUREANDO:
                    printf("  Tipo: Carta LAUREANDO\n");
                break;
                case BONUS:
                    printf("  Tipo: Carta BONUS\n");
                break;
                case MALUS:
                    printf("  Tipo: Carta MALUS\n");
                break;
                case MAGIA:
                    printf("  Tipo: Carta MAGIA\n");
                break;
                case ISTANTANEA:
                    printf("  Tipo: Carta ISTANTANEA\n");
                break;
                default:
                    printf("  Tipo: " BRED "Errore" RESET "\n");
                break;
            }

            printf("  Numero di Effetti: %d\n", temp->numEffetti);

            if (temp->numEffetti > 0) {
                printf("  Effetti:\n");
                for (int i = 0; i < temp->numEffetti; i++) {
                    Effetto e = temp->effetti[i];
                    printf("    - Azione: ");
                    switch (e.azione) {
                        case GIOCA:
                            printf("Giocare una carta dalla mano");
                        break;
                        case SCARTA:
                            printf("Scartare una carta dalla mano");
                        break;
                        case ELIMINA:
                            printf("Eliminare una carta da aula o bonus/malus");
                        break;
                        case RUBA:
                            printf("Prendere una carta da aula o bonus/malus e metterla nella propria aula");
                        break;
                        case PESCA:
                            printf("Pescare una carta dal mazzo di pesca");
                        break;
                        case PRENDI:
                            printf("Rubare una carta dalla mano di un giocatore e metterla nella propria");
                        break;
                        case BLOCCA:
                            printf("Impedisce di subire un effetto");
                        break;
                        case SCAMBIA:
                            printf("Scambia la propria mano con quella di un altro giocatore");
                        break;
                        case MOSTRA:
                            printf("Mostra la propria mano a tutti");
                        break;
                        case IMPEDIRE:
                            printf("Non permette di utilizzare determinate tipologie di carte");
                        break;
                        case INGEGNERE:
                            printf("Trasforma gli studenti di informatica in studenti di ingegneria");
                        break;
                        default:
                            printf(BRED "Errore Azione" RESET);
                        break;
                    }

                    printf(", Target Giocatore: ");
                    switch (e.targetGiocatore) {
                        case IO:
                            printf("IO");
                        break;
                        case TU:
                            printf("ALTRO GIOCATORE");
                        break;
                        case VOI:
                            printf("VOI");
                        break;
                        case TUTTI:
                            printf("TUTTI");
                        break;
                        default:
                            printf(BRED "Errore TargetGiocatore" RESET);
                        break;
                    }

                    printf(", Target Carta: ");
                    switch (e.targetCarta) {
                        case ALL:
                            printf("TUTTE LE CARTE");
                        break;
                        case STUDENTE:
                            printf("STUDENTE");
                        break;
                        case MATRICOLA:
                            printf("MATRICOLA");
                        break;
                        case STUDENTE_SEMPLICE:
                            printf("STUDENTE SEMPLICE");
                        break;
                        case LAUREANDO:
                            printf("LAUREANDO");
                        break;
                        case BONUS:
                            printf("BONUS");
                        break;
                        case MALUS:
                            printf("MALUS");
                        break;
                        case MAGIA:
                            printf("MAGIA");
                        break;
                        case ISTANTANEA:
                            printf("ISTANTANEA");
                        break;
                        default:
                            printf(BRED "Errore TargetCarta" RESET);
                        break;
                    }
                    printf("\n");
                }
            }

            printf("  Quando si attiva: ");
            switch (temp->quando) {
                case SUBITO:
                    printf("Appena giocata\n");
                break;
                case INIZIO:
                    printf("Inizio turno\n");
                break;
                case FINE:
                    printf("Quando la carta esce dall'aula\n");
                break;
                case MAI:
                    printf("Mai\n");
                break;
                case SEMPRE:
                    printf("Sempre attivo\n");
                break;
                default:
                    printf(BRED "Errore Quando" RESET "\n");
                break;
            }

            if (temp->effettiUtilizzabili)
                printf("  Opzionale: " BGRN "Si" RESET "\n");
            else
                printf("  Opzionale: " BRED "No" RESET "\n");

            printf(BHCYN "----------------------------\n" RESET);

            temp = temp->next;
            count++;
        } while (temp != mazzo);
    }
}


/**
 * contaGiocatori:
 *  - Conta quanti giocatori sono presenti nella lista circolare.
 */
int contaGiocatori(Giocatore *lista) {
    int count = 0;
    if (lista != NULL) {
        count = 1;
        Giocatore *temp = lista->next;
        while (temp != NULL && temp != lista) {
            count++;
            temp = temp->next;
        }
    }
    return count;
}
