/***************************************************************************
 *  Gestisce le statistiche di vittoria
 ***************************************************************************/

#include "header.h"

#define STATS_FILE "statistiche.txt"

/**
 * mostraStatistiche:
 *  - Legge il file "statistiche.txt" se esiste
 *  - Mostra il numero di vittorie per ogni personaggio
 *  - Se il file non esiste, lo segnala con un messaggio
 */
void mostraStatistiche() {
    FILE *f = fopen(STATS_FILE, "r");
    if (!f) {
        printf("\n\033[33mNessuna statistica trovata. Saranno create alla prima vittoria.\033[0m\n");
    } else {

        printf("\n\033[1mStatistiche vittorie:\033[0m\n");
        Statistica s;
        while (fscanf(f, "%[^,],%d,%d\n", s.nomePersonaggio, &s.vittorieManuale, &s.vittorieIA) == 3) {
            printf("  %s - Manuale: %d | IA: %d\n", s.nomePersonaggio, s.vittorieManuale, s.vittorieIA);
        }
        fclose(f);
    }
}
/**
 * aggiornaStatistiche:
 *  - Aggiorna il numero di vittorie di un personaggio
 *  - Se il personaggio è già presente, aggiorna il conteggio
 *  - Se non è presente, lo aggiunge alla lista
 *  - Salva tutto nuovamente nel file "statistiche.txt"
 *  - Confronto dei nomi fatto rimuovendo newline e spazi
 */
void aggiornaStatistiche(const char *nomeVincitore, bool isIA) {
    FILE *f = fopen(STATS_FILE, "r");
    Statistica lista[NMAXGIOCATORI];
    int n = 0;

    // Lettura file esistente se presente
    if (f) {
        while (fscanf(f, "%[^,],%d,%d\n", lista[n].nomePersonaggio, &lista[n].vittorieManuale, &lista[n].vittorieIA) == 3) {
            lista[n].nomePersonaggio[strcspn(lista[n].nomePersonaggio, "\r\n")] = 0; // rimuove newline
            n++;
        }
        fclose(f);
    }

    // Pulizia nome
    char nomePulito[NOME];
    strncpy(nomePulito, nomeVincitore, NOME);
    nomePulito[NOME - 1] = '\0';
    nomePulito[strcspn(nomePulito, "\r\n")] = 0;

    // Verifica se il nome è già presente
    int trovato = 0;
    int indiceTrovato = -1;
    for (int i = 0; i < n; i++) {
        if (!trovato && strcmp(lista[i].nomePersonaggio, nomePulito) == 0) {
            indiceTrovato = i;
            trovato = 1;
        }
    }

    // Se trovato, aggiorna
    if (trovato && indiceTrovato != -1) {
        if (isIA)
            lista[indiceTrovato].vittorieIA++;
        else
            lista[indiceTrovato].vittorieManuale++;
    }

    // Se non trovato, aggiungi nuova voce
    if (!trovato && n < NMAXGIOCATORI) {
        strncpy(lista[n].nomePersonaggio, nomePulito, NOME);
        lista[n].nomePersonaggio[NOME - 1] = '\0';
        lista[n].vittorieIA = isIA ? 1 : 0;
        lista[n].vittorieManuale = isIA ? 0 : 1;
        n++;
    }

    // Scrittura su file aggiornata
    f = fopen(STATS_FILE, "w");
    if (f != NULL) {
        for (int i = 0; i < n; i++) {
            fprintf(f, "%s,%d,%d\n", lista[i].nomePersonaggio, lista[i].vittorieManuale, lista[i].vittorieIA);
        }
        fclose(f);
    }
}

