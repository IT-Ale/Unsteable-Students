/***************************************************************************
 *  Si occupa di caricare il mazzo di carte dal file "mazzo.txt", leggendo
 *  i dati di ogni carta e duplicandoli per il numero di copie indicato.
 ***************************************************************************/

#include "header.h"
#include "ANSI.h"

/**
 * leggiCarta:
 *  - Legge da file una singola carta e restituisce una struttura temporanea.
 *  - NON la inserisce in lista (next = NULL), servirà una copiaCarta() a parte.
 *  - Legge nome, descrizione, tipologia, effetti, attivazione e flag.
 */
Carta leggiCarta(FILE *file) {
    Carta cartaTemp;    // Carta temporanea
    int temp;           // Variabile di appoggio per booleani

    // Legge il nome
    if (fscanf(file, " %[^\n]", cartaTemp.nome) != 1) {
        printf(BRED "Errore nella lettura del nome carta" RESET "\n");
        strcpy(cartaTemp.nome, "ERRORE");
    }

    // Legge la descrizione
    if (fscanf(file, " %[^\n]", cartaTemp.descrizione) != 1) {
        printf(BRED "Errore nella lettura della descrizione" RESET "\n");
        strcpy(cartaTemp.descrizione, "ERRORE");
    }

    // Legge la tipologia (intero)
    if (fscanf(file, "%d", &cartaTemp.tipologia) != 1) {
        printf(BRED "Errore nella lettura della tipologia" RESET "\n");
        cartaTemp.tipologia = ALL;
    }

    // Legge il numero di effetti
    if (fscanf(file, "%d", &cartaTemp.numEffetti) != 1) {
        printf(BRED "Errore nella lettura del numEffetti" RESET "\n");
        cartaTemp.numEffetti = 0;
    }

    // Se ci sono effetti, alloca la memoria e leggili
    if (cartaTemp.numEffetti > 0) {
        cartaTemp.effetti = (Effetto*) malloc(cartaTemp.numEffetti * sizeof(Effetto));
        if (cartaTemp.effetti == NULL) {
            printf(BRED "Errore allocazione effetti" RESET "\n");
            cartaTemp.numEffetti = 0;
            exit(1);
        }
        for (int i = 0; i < cartaTemp.numEffetti; i++) {
            if (fscanf(file, "%d %d %d",
                       (int*)&cartaTemp.effetti[i].azione,
                       (int*)&cartaTemp.effetti[i].targetGiocatore,
                       (int*)&cartaTemp.effetti[i].targetCarta) != 3)
            {
                printf(BRED "Errore nella lettura di un effetto" RESET "\n");
            }
        }
    } else {
        cartaTemp.effetti = NULL;
    }

    // Legge il "quando" (intero convertito in enum)
    if (fscanf(file, "%d", &cartaTemp.quando) != 1) {
        printf(BRED "Errore nella lettura del 'quando'" RESET "\n");
        cartaTemp.quando = ALL; // valore di fallback
    }

    // Legge se gli effetti sono utilizzabili (0 o 1)
    if (fscanf(file, "%d", &temp) != 1) {
        printf(BRED "Errore nella lettura di effettiUtilizzabili" RESET "\n");
        temp = 0;
    }
    cartaTemp.effettiUtilizzabili = (temp != 0);

    // Non gestiamo qui la parte "next", perché la carta non è ancora in lista
    cartaTemp.next = NULL;

    return cartaTemp;
}

/**
 * caricaMazzo:
 *  - Apre "mazzo.txt" e legge quante copie (qCarta) fare per ogni carta.
 *  - Per ogni carta letta da leggiCarta(), crea qCarta copie reali e le inserisce.
 *  - Restituisce il mazzo completo come lista circolare.
 */
Carta *caricaMazzo() {
    FILE *file = fopen("mazzo.txt", "r");
    if (file == NULL) {
        printf(BRED "Errore apertura file mazzo.txt" RESET "\n");
        exit(1);
    }

    Carta *mazzo = NULL;
    int qCarta;

    // Legge finché trova un intero (qCarta)
    while (fscanf(file, "%d", &qCarta) == 1) {
        // Leggi la "base" della carta da file
        Carta base = leggiCarta(file);

        // Per qCarta volte, crea una copia e aggiungila al mazzo
        for (int i = 0; i < qCarta; i++) {
            Carta *copia = copiaCarta(&base);
            mazzo = aggiungiCartaInLista(mazzo, copia);
        }
    }

    fclose(file);
    return mazzo;
}
