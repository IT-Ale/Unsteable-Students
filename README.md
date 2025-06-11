# Unsteable-Students
<br>

> ### **Table of Content**
>  1. [Spiegazione file sorgente](#spiega-file-sorgente)
>  1. [Descrizione e scopo strutture aggiuntive](#descrizione-e-scopo-strutture-aggiuntive)
>  1. [Descrizione flusso di gioco](#descrizione-flusso-di-gioco)
>  1. [Descrizione logica AI](#descrizione-logica-ai)
>      - [Scelta carte](#scelta-carte)
>      - [Strategia di gioco](#strategia-di-gioco)

<br>

<br>
<br>

## Spiegazione file sorgente
- **caricaMazzo.c**: Carica il mazzo.txt creando una lista circolare di carte. <br>
- **header.h**: Contiene tutte le strutture/enumerazioni e i prototipi dei sottoprogrammi. <br>
- **main.c**: Gestisce il flusso di gioco. <br>
- **chiusura.c**: Dealloca tutte le liste circolari create. <br>
- **preparazione.c**: Prepara l'inizio del gioco creando le varie liste (mazzi, giocatori). <br>
- **salvataggio.c**: Gestisce tutte le funzioni di salvataggio, salvando nel file indicato dall'utente. <br>
- **statistiche.c**: Gestisce le statistiche delle vittorie, salvando e leggendo i risultati delle partite in un file. <br>
- **logger.c** : Gestisce la creazione del file log.txt, per tenere traccia delle giocate effettuate <br>
- **ANSI.h** : Macro per la formattazione del testo <br>
- **turno.c** : Gestisce lo svolgimento del turno di un giocatore umano <br>
- **turnoIA.c** : Gestisce lo svolgimento del turno di un giocatore IA <br>
- **liste.c** : Implementa le funzioni per gestire le liste circolari <br>
- **funzioni.c**: Implementa le funzioni per lo svolgimento dei turni <br>
- **effetti.c**: Contiene tutte le funzioni dei vari effetti usati dall IA e non <br>
<br>

## Descrizione e scopo strutture aggiuntive

### Struttura `Statistica` 
typedef struct {   <br>
    char nomePersonaggio[NOME]; // Nome del personaggio <br>
    int vittorieManuale;        // Numero di vittorie in partite manuali <br>
    int vittorieIA;             // Numero di vittorie da parte dell'IA <br>
} Statistica <br>
<br>

## Descrizione flusso di gioco

> 
### **1. Avvio programma (`main.c`)**<br>
- All'avvio del programma, l'utente **inserisce il nome del file di salvataggio**.<br>
- Viene controllata l’**esistenza di un salvataggio precedente**:<br>
  - Se esiste, l'utente può scegliere di **caricare la partita salvata** o **iniziare una nuova partita**.<br>
  - Se non esiste o si sceglie una nuova partita, si passa alla fase di **preparazione**.<br>

### **2. Preparazione (`preparazione.c`)**<br>
- Viene caricato il **mazzo completo dal file `mazzo.txt`**.<br>
- Il mazzo viene **mischiato casualmente**.<br>
- Dal mazzo intero vengono separate le carte tipo **`MATRICOLA`**, creando il mazzo **`Aula Studio`**.<br>
- Il resto delle carte forma il mazzo di pesca (**`mazzoPesca`**).<br>
- Vengono creati da **2 a 4 giocatori**, con nomi forniti dall'utente.<br>
- Ad ogni giocatore vengono distribuite **5 carte** dal mazzo di pesca e una carta **`MATRICOLA`** dall'`Aula Studio`.<br>

### **3. Loop principale del gioco (`main.c`)**<br>
Il loop principale continua finché non si verifica una **condizione di vittoria**:<br>

Per ogni turno (**generico**):<br>
  - Viene effettuato il **salvataggio automatico della partita**.<br>
- Il giocatore corrente effettua una **pescata obbligatoria** dal mazzo di pesca.<br>
- Vengono attivati eventuali effetti delle carte con attivazione **"INIZIO"** o **"SEMPRE"**.<br>
- Il giocatore può scegliere se **pescare un'altra carta** oppure **giocare una carta** dalla propria mano, attivandone gli effetti.<br>
- Alla fine del turno:<br>
  - Se il giocatore ha **più di 5 carte in mano**, deve scartarne fino a raggiungere il limite massimo di 5 carte.<br>
  - Si verifica se un giocatore ha raggiunto la **condizione di vittoria** (6 o più studenti nell'aula senza la carta **`INGEGNERE`** attiva).<br>
  - Se viene soddisfatta, il gioco termina aggiornando le statistiche e annunciando il vincitore.<br>

### **4. Conclusione del gioco (`chiusura.c`)**<br>
- Al termine della partita vengono liberate tutte le risorse allocate durante il gioco:<br>
  - **Mazzo di pesca**, **Aula studio**, **Mazzo degli scarti**.<br>
  - Liste dei giocatori e delle carte che hanno in **mano**, nell'**aula** e nel **bonus/malus**.<br>
- Viene stampato un **messaggio di conferma** della chiusura del programma.<br>

<br>

## Descrizione logica AI

### **Scelta carte**<br>
Durante il proprio turno, l'IA seleziona automaticamente la carta più opportuna da giocare, seguendo una logica basata su priorità e situazione di gioco corrente:<br>

- **Priorità Alta**:<br>
  - Se possiede una carta di tipo **`LAUREANDO`**, questa carta viene giocata immediatamente per avvicinarsi rapidamente alla condizione di vittoria.<br>

- **Priorità Tattica**:<br>
  - Se rileva una **minaccia immediata** (un avversario con **più di 3 studenti nell'aula**), l'IA cerca di contrastarlo utilizzando carte che abbiano effetti come:<br>
    - **`ELIMINA`** (con bersaglio avversario).<br>
    - **`RUBA`** (carte che sottraggono risorse agli avversari).<br>
    - Carte **`MALUS`** che penalizzano direttamente gli avversari.<br>

- **Priorità Standard (fallback)**:<br>
  - Se non ci sono minacce evidenti o carte di alta priorità disponibili, l'IA segue una gerarchia di scelta delle carte più utili:<br>
    1. Carte **`STUDENTE_SEMPLICE`** (aumentano numero studenti in aula).<br>
    2. Carte **`BONUS`** (effetti positivi persistenti).<br>
    3. Carte **`MAGIA`** (effetti speciali temporanei).<br>
    4. Carte **`MALUS`** (utilizzate in assenza di opzioni migliori).<br>

- **Se nessuna carta risulta utile**:<br>
  - L'IA opta per **pescare automaticamente una carta aggiuntiva** dal mazzo di pesca, aumentando così le opzioni disponibili nei turni successivi.<br>

### **Strategia di gioco generale**<br>
La strategia globale seguita dall'IA si basa sui seguenti punti chiave:<br>

- **Massimizzare velocemente il numero di studenti nell'aula** per raggiungere la condizione di vittoria (6 studenti senza la carta **`INGEGNERE`** attiva).<br>
- **Monitorare e contrastare gli avversari più forti**, ovvero quelli più vicini alla vittoria.<br>
- **Utilizzare con saggezza le carte effetto**, riservando effetti negativi come **`ELIMINA`** o **`RUBA`** per rallentare gli avversari in vantaggio.<br>
- **Gestire automaticamente lo scarto** a fine turno, mantenendo sempre un massimo di **5 carte in mano** e scartando automaticamente le carte meno utili secondo la priorità: **`MAGIA` → `MALUS` → `BONUS` → `STUDENTE_SEMPLICE` → `LAUREANDO`**.<br>

