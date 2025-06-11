/***************************************************************************
 *  Gestisce il salvataggio e il caricamento della partita
 ***************************************************************************/

#include "header.h"

/**
 * salvaListaCarte:
 *  - Salva su file una lista circolare di carte.
 *  - Scrive prima il numero totale di carte.
 *  - Scrive ogni carta e i suoi effetti (se presenti).
 */
void salvaListaCarte(Carta *lista, FILE *file) {
    int count = contaCarte(lista);
    fwrite(&count, sizeof(int), 1, file);

    bool eseguiScrittura = (count > 0);

    if (eseguiScrittura) {
        Carta *temp = lista;
        for (int i = 0; i < count; i++) {
            fwrite(temp, sizeof(Carta), 1, file);
            if (temp->numEffetti > 0 && temp->effetti != NULL) {
                fwrite(temp->effetti, sizeof(Effetto), temp->numEffetti, file);
            }
            temp = temp->next;
        }
    }
}


/**
 * caricaListaCarte:
 *  - Carica una lista di carte da file binario.
 *  - Legge il numero di carte, poi ogni carta e i suoi effetti.
 *  - Restituisce la lista ricostruita.
 */
Carta *caricaListaCarte(FILE *file) {
    int count;
    Carta *lista = NULL;

    fread(&count, sizeof(int), 1, file);

    if (count > 0) {
        for (int i = 0; i < count; i++) {
            Carta *c = malloc(sizeof(Carta));
            fread(c, sizeof(Carta), 1, file);

            if (c->numEffetti > 0) {
                c->effetti = malloc(sizeof(Effetto) * c->numEffetti);
                fread(c->effetti, sizeof(Effetto), c->numEffetti, file);
            } else {
                c->effetti = NULL;
            }

            c->next = NULL;
            lista = aggiungiCartaInLista(lista, c);
        }
    }

    return lista;
}

/**
 * salvaPartita:
 *  - Salva l'intera partita su file binario.
 *  - Scrive i dati di tutti i giocatori (nome, carte), aulaStudio, mazzoPesca, mazzoScarti.
 */
void salvaPartita(char *nomeFile, Giocatore *giocatori, Carta *aulaStudio, Carta *mazzoPesca, Carta *mazzoScarti) {
    FILE *file = fopen(nomeFile, "wb");
    bool fileValido = (file != NULL);
    if (fileValido) {
        int numGiocatori = contaGiocatori(giocatori);
        fwrite(&numGiocatori, sizeof(int), 1, file);

        Giocatore *temp = giocatori;
        for (int i = 0; i < numGiocatori; i++) {
            fwrite(temp->nome, sizeof(char), NOME, file);
            salvaListaCarte(temp->carteMano, file);
            salvaListaCarte(temp->aula, file);
            salvaListaCarte(temp->bonusMalus, file);
            temp = temp->next;
        }

        salvaListaCarte(aulaStudio, file);
        salvaListaCarte(mazzoPesca, file);
        salvaListaCarte(mazzoScarti, file);
        fclose(file);

        printf("Partita salvata in %s\n", nomeFile);
    } else {
        printf("Errore nell'apertura del file %s per il salvataggio.\n", nomeFile);
    }
}


/**
 * caricaPartita:
 *  - Carica lo stato di gioco da un file di salvataggio.
 *  - Ricostruisce i giocatori e le liste di carte.
 *  - Restituisce la lista dei giocatori caricati.
 */
Giocatore *caricaPartita(char *nomeFile, Carta **aulaStudio, Carta **mazzoPesca, Carta **mazzoScarti) {
    Giocatore *giocatori = NULL;
    FILE *file = fopen(nomeFile, "rb");

    bool fileValido = (file != NULL);

    if (fileValido) {
        int numGiocatori;
        fread(&numGiocatori, sizeof(int), 1, file);

        for (int i = 0; i < numGiocatori; i++) {
            Giocatore *g = malloc(sizeof(Giocatore));
            fread(g->nome, sizeof(char), NOME, file);
            g->carteMano = caricaListaCarte(file);
            g->aula = caricaListaCarte(file);
            g->bonusMalus = caricaListaCarte(file);
            g->next = NULL;
            giocatori = aggiungiGiocatoreInLista(giocatori, g);
        }

        *aulaStudio = caricaListaCarte(file);
        *mazzoPesca = caricaListaCarte(file);
        *mazzoScarti = caricaListaCarte(file);

        fclose(file);
        printf("File di salvataggio usato: %s\n", nomeFile);
    } else {
        printf("Errore nell'apertura del file %s per il caricamento.\n", nomeFile);
    }

    return giocatori;
}
