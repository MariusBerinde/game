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
## confronto tempi parall 
### sequenziale 
- init :   millisec 0.000035
- init (old): 0.000077
- controllo regole 1,2 : 0.000008
- controllo regole 1,2 : 0.000023(con pc vecchio)
## parallelo 
- init :  0.031805
- init (old):0.000465
- controllo regole 1,2 :0.041979
- controllo regole 1,2 :0.112164(con pc vecchio)
- simula turno con ottimizzazione -o2 0.000795 ms
                  

è normale avere delle prestazioni peggiori per numero così piccoli


## test tempi di esecuzione sequenziale con e senza -O2 

velocità di esecuzione senza parallizzazione  millisec 0.000080 (senza)
velocità di esecuzione senza parallizzazione  millisec 0.000020 (con)
