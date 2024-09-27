# Progetto prog mobile
Progetto di programmazione parallela in cui sviluppo una simulazione del gioco "Game of life"
il progetto ha 2 cartelle principali:
- sequenziale
- parallelo

## Dettagli gioco 
prendi una matrice $n \times m$ , ogni cella può essere viva o morta.
il gioco simula l'evoluzione di queste celle , in cui ogni cella viva interagisce con quelle vicine a lei 

 ogni casella interagisce con le 8 vicine nel seguente modo
- se la cella viva ha 1 vicino o nessuno nel turno successivo muore
- ogni cella con 4 o più vicini nel turno successivo muore 
- ogni cella con con 2 o 3 vicini sopravvive al turno successivo
- ogni cella vuota che ha 3 vicini vivi si attiva al p

per lo sviluppo avrò bisogno di :
- una mappa su cui inserire le mie pedine
- la lista delle pedine vive
- ogni pedina ha un parametro con il numero di vicini 
- per il gioco in generale devo avere il numero di turni 
- devo disegnare la mappa 
- leggo la configurazione iniziale da file 
