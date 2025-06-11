/***************************************************************************
 *  Contiene tutte le definizioni di strutture, enum e funzioni principali
 *  usate nel progetto. Include anche librerie standard e costanti di gioco.
 ***************************************************************************/

#ifndef STRUTTURE_H
#define STRUTTURE_H

#include <stdbool.h>   // Per bool
#include <stdio.h>     // Per printf, scanf, file...
#include <stdlib.h>    // Per malloc, free, rand...
#include <string.h>    // Per strcmp, strcpy...
#include <time.h>      // Per srand, time
#include <windows.h>   // Per enableANSI (funzioni Windows console)

///////////////////////////////////////////////////////////////////////////////
// Costanti di configurazione
///////////////////////////////////////////////////////////////////////////////

#define NOME 32            // Max lunghezza nome giocatore
#define DESC 256           // Max lunghezza descrizione carta
#define N 10               // Numero di volte per cui mischi il mazzo
#define NPESCATEINIZIALI 5 // Numero di pescate iniziali per ogni giocatore
#define NCARTEMAX 5        // Numero massimo di carte in mano a fine turno
#define STUDENTIVITTORIA 6 // Numero di studenti di informatica necessari per vincere
#define NMAXGIOCATORI 50 // Numero di giocatori salvabili
#define MAX_LINE 256

///////////////////////////////////////////////////////////////////////////////
// Enumerazioni
///////////////////////////////////////////////////////////////////////////////

/**
 * Tipologia della carta. Sono possibili diverse categorie (es. STUDENTE, MALUS, BONUS...).
 */
typedef enum {
    ALL,                // Valore generico
    STUDENTE,
    MATRICOLA,
    STUDENTE_SEMPLICE,
    LAUREANDO,
    BONUS,
    MALUS,
    MAGIA,
    ISTANTANEA
} TipoCarta;

/**
 * Azioni possibili degli effetti (GIOCA, SCARTA, ELIMINA...).
 */
typedef enum {
    GIOCA,
    SCARTA,
    ELIMINA,
    RUBA,
    PESCA,
    PRENDI,
    BLOCCA,
    SCAMBIA,
    MOSTRA,
    IMPEDIRE,
    INGEGNERE
} Azione;

/**
 * Quando si attiva l’effetto di una carta (SUBITO, INIZIO turno, FINE...).
 */
typedef enum {
    SUBITO,
    INIZIO,
    FINE,
    MAI,
    SEMPRE
} Quando;

/**
 * A chi è rivolto l’effetto (IO, TU, VOI, TUTTI).
 */
typedef enum {
    IO,
    TU,
    VOI,
    TUTTI
} TargetGiocatori;

///////////////////////////////////////////////////////////////////////////////
// Strutture dati
///////////////////////////////////////////////////////////////////////////////

/**
 * Descrive un singolo effetto di una carta.
 */
typedef struct effetto {
    Azione          azione;          // Azione (es. GIOCA, SCARTA...)
    TargetGiocatori targetGiocatore; // Target (IO, TU, VOI, TUTTI)
    TipoCarta       targetCarta;     // A che tipo di carta si riferisce (STUDENTE, MALUS, ALL, ecc.)
} Effetto;

/**
 * Struttura "Carta":
 *  - nome, descrizione
 *  - tipologia (STUDENTE, MATRICOLA, MALUS...)
 *  - array di effetti
 *  - quando si attiva
 *  - flag se gli effetti sono utilizzabili
 *  - next per la lista circolare
 */
typedef struct carta {
    char            nome[NOME];
    char            descrizione[DESC];
    TipoCarta       tipologia;
    int             numEffetti;
    Effetto*        effetti;             // Puntatore a un array di Effetto
    Quando          quando;              // Momento di esecuzione
    bool            effettiUtilizzabili; // Flag se effetti sono (o possono essere) attivi
    struct carta*   next;                // Lista circolare
} Carta;

/**
 * Struttura "Giocatore":
 *  - nome
 *  - carteMano (lista circolare)
 *  - aula (lista circolare)
 *  - bonusMalus (lista circolare)
 *  - next (lista circolare di giocatori)
 */
typedef struct giocatore {
    char                nome[NOME];
    Carta*              carteMano;
    Carta*              aula;
    Carta*              bonusMalus;
    struct giocatore*   next;
} Giocatore;


/**
 * Struttura "Statistica":
 *  - nomePersonaggio: nome del personaggio a cui appartiene la statistica
 *  - vittorieManuale: numero di vittorie ottenute in partite giocate manualmente
 *  - vittorieIA: numero di vittorie ottenute dall'intelligenza artificiale con quel personaggio
 */
typedef struct {
    char nomePersonaggio[NOME];     // Nome del personaggio
    int vittorieManuale;            // Numero di vittorie in partite manuali
    int vittorieIA;                 // Numero di vittorie da parte dell'IA
} Statistica;

///////////////////////////////////////////////////////////////////////////////
// Prototipi delle funzioni
///////////////////////////////////////////////////////////////////////////////

void enableANSI();  // Abilita la stampa con codice ANSI su Windows

// Caricamento e preparazione
Carta* caricaMazzo();                                 // Carica le carte da file
void preparazione(Carta **mazzoPesca, Carta **aulaStudio, Giocatore **giocatori);

// Creazione e gestione liste
Carta* copiaCarta(Carta *orig);
Carta* aggiungiCartaInLista(Carta *mazzo, Carta *nuovaCarta);
Carta* rimuoviCartaDaLista(Carta *mazzo, Carta *cartaDaRimuovere);
int contaCarte(Carta *mazzo);
int contaGiocatori(Giocatore *lista);
Carta* OttieniPosizioneCarta(Carta *mazzo, int posizione);
void stampaLista(Carta *mazzo);

// Giocatori
Giocatore* creazioneGiocatori();
Giocatore* aggiungiGiocatoreInLista(Giocatore *giocatori, Giocatore *nuovoGiocatore);
void visualizzaGiocatori(Giocatore *listaGiocatore);
void deallocaGiocatori(Giocatore *listaGiocatori);

// Turno Generico
void pescata(Carta **mazzoPesca, Carta **mazzoScarti, Giocatore *giocatore);
void pescataAulaStudio(Carta **aulaStudio, Giocatore *giocatore);
void resocontoGiocatore(Giocatore *giocatore);
void attivaEffetto(Giocatore *giocatoreCorrente, Carta *cartaGiocata, Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti, bool isIA);
void eseguiEffetto(Effetto e, Giocatore *g, Giocatore *c, Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti, bool isIA);
void attivaEffettoCartaUscitaDaAula(Giocatore *giocatore, Carta *cartaUscita, Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti, bool isIA);
Giocatore* scegliGiocatre(Giocatore *lista, bool isIA);

// Turno Giocatore
void turno(Carta **mazzoPesca, Carta **aulaStudio, Giocatore **giocatore, Carta **mazzoScarti, char *nomeSalvataggio);
void giocaCarta(Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti, Giocatore *giocatore);
void fineTurno(Carta **mazzoScarti, Carta **aulaStudio, Giocatore *giocatore, Carta **mazzoPesca);
void attivaEffettiInizioTurno(Giocatore *giocatore, Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti);

// Turno IA
void turnoIA(Carta **mazzoPesca, Carta **aulaStudio, Giocatore **giocatore, Carta **mazzoScarti, char *nomeSalvataggio);
void giocaCartaIA(Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti, Giocatore *giocatore);
void fineTurnoIA(Carta **mazzoScarti, Carta **aulaStudio, Giocatore *giocatore, Carta **mazzoPesca);
void attivaEffettiInizioTurnoIA(Giocatore *giocatore, Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti);

// Effetti
void effettoPesca(Effetto effettoCorrente, Giocatore *giocatore, Carta **mazzoPesca, Carta **mazzoScarti);
void effettoScarta(Effetto effettoCorrente, Giocatore *giocatore, Carta **mazzoScarti, Carta **aulaStudio, bool isIA);
void effettoGiocaCarta(Effetto effettoCorrente, Giocatore *giocatore, Carta **mazzoPesca, Carta **aulaStudio, Carta **mazzoScarti, bool isIA);
void effettoElimina(Effetto effettoCorrente, Giocatore *giocatore, Carta **mazzoScarti, Carta **aulaStudio, Carta **mazzoPesca, bool isIA);
void effettoMostra(Effetto effettoCorrente, Giocatore *giocatore);
void effettoScambia(Effetto effettoCorrente,  Giocatore *bersaglio, Giocatore *corrente);
void effettoRuba(Effetto effettoCorrente, Giocatore *bersaglio, Giocatore *corrente, Carta **mazzoScarti, Carta **aulaStudio, bool isIA);
void effettoPrendi(Effetto effettoCorrente, Giocatore *bersaglio, Giocatore *corrente, bool isIA);
bool effettoBlocca(Giocatore *bersaglio, Carta **mazzoScarti, bool isIA);
bool BloccataDaImpedire(Giocatore *g, Carta *cartaDaGiocare);

// Ruba carte specifico per zona
void rubaDaAll(Giocatore *bersaglio, Giocatore *chiRuba, bool isIA);
void rubaDaBonusMalus(Giocatore *bersaglio, Giocatore *chiRuba, TipoCarta targetType, bool isIA);
void rubaDaAula(Giocatore *bersaglio, Giocatore *chiRuba, bool isIA);

// Scarto di carte specifico per zona
void scartaCartaMano (Carta **mazzoScarti, Carta **aulaStudio, Giocatore *giocatore, Carta *cartaDaScartare);
void scartaCartaAula(Carta **mazzoScarti, Carta **aulaStudio, Carta **mazzoPesca, Giocatore *giocatore, Carta *cartaDaScartare, bool isIA);
void scartaCartaBonusMalus (Carta **mazzoScarti, Giocatore *giocatore, Carta *cartaDaScartare);

// Funzioni di supporto per scartare
void scartaDaAula(Giocatore *g, Carta **mazzoScarti, Carta **aulaStudio, Carta **mazzoPesca, bool isIA);
void scartaDaBonusMalus(Giocatore *g, Carta **mazzoScarti, TipoCarta targetType, bool isIA);
void scartaDaAll(Giocatore *g, Carta **mazzoScarti, Carta **aulaStudio, Carta **mazzoPesca, bool isIA);

// Mazzi
Carta* creazioneAulaStudio(Carta *mazzo);
Carta* creazioneMazzoPesca (Carta *mazzo);
void mischiaMazzo(Carta **mazzo);

// Deallocazione e fine gioco
void fineGioco(Carta *mazzoPesca, Carta *aulaStudio, Giocatore *giocatori, Carta *mazzoScarti);
void deallocaCarte(Carta *mazzo);

// Funzione di utilità
int contaStudenti(Carta *aula);
bool haCartaIngenerizzazione(Giocatore *g);

//Salvataggio
void salvaListaCarte(Carta *lista, FILE *file);
Carta *caricaListaCarte(FILE *file);
void salvaPartita(char *nomeFile, Giocatore *giocatori, Carta *aulaStudio, Carta *mazzoPesca, Carta *mazzoScarti);
Giocatore *caricaPartita(char *nomeFile, Carta **aulaStudio, Carta **mazzoPesca, Carta **mazzoScarti);

//Statistiche
void mostraStatistiche();
void aggiornaStatistiche(const char *nomeVincitore, bool isIA);

// logger.c
void inizializzaLog();
void scriviLogTurno(char *nomeGiocatore, char *azione, char *nomeCarta);
int leggiUltimoTurno();
void incrementaTurnoLog();

#endif // STRUTTURE_H
