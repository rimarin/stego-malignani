/*##############################################################################
  ###### NOME: StegoMalignani                                   ################
  ###### AUTORE: Riccardo Marin                                 ################
  ###### ULTIMO AGGIORNAMENTO (v 1.2): 30/04/18 10:00           ################
  ###### DESCRIZIONE: Programma per permette di scrivere e      ################
  ###### leggere messaggi sulle immagini in formato bmp         ################
  ###### sfruttando il bit meno significativo del blu           ################
  ###### di ogni pixel.                                         ################
  ##############################################################################

  NOTE: allo stato attuale i messaggi nascosti sono facilmente individuabili 
	da steganalisi (attacco statistico). E' necessaria maggiore sicurezza.
	Possibili miglioramenti:
		- scrittura in pixel casuali sparsi per l'immagine
*/

#include <cstdlib>      // libreria standard
#include <iostream>     // libreria standard
#include <stdio.h>      // libreria standard
#include <fstream>      // per input/output file
#include <string>       // per manipolare stringhe
#include "EasyBMP.h"    // per leggere e scrivere dati nelle immagini
#define numVers "1.2"   // numero versione attuale

using namespace std;   
     	
void mostraUtilizzo()   // funzione che spiega l'utilizzo del programma
{
    cout << "Utilizzo: ./StegoMalignani -comando percorsoImmagineBMP\n\n";
    cout << "\t -r/--rivela     Comando per rivelare dati nascosti nell'immagine\n";
    cout << "\t -n/--nascondi   Comando per nascondere dati nell'immagine\n";
    cout << "\t -f/--file       Per specificare un file da/verso cui direzionare i dati (opzionale)\n";
}

void cripta (string &testo, string &chiave, string &cifrato) {   // realizza cifratura vigenere con xor    
       string chiaveEstesa = testo;

       for(int i = 0; i < testo.length(); i++) {   // ciclo per tutta la lunghezza del testo 
           chiaveEstesa.at(i) = chiave.at(i%(chiave.length()));  // chiave ripetuta se lunghezza testo > lunghezza chiave
           // quando avviene xor tra valori uguali, ottengo 0, che verrebbe interpretato come fine stringa
           // tramite un artificio si può ovviare questo problema:
           cifrato.at(i) = ((testo.at(i)-'0') ^ (chiaveEstesa.at(i)-'0')) + '0';      
	}
}  

void nascondi(BMP &img, char percorso[], bool &usaFile, char percorsoFile[])
{
      img.ReadFromFile(percorso);

      // INPUT MESSAGGIO DA CRIPTARE 
      string testo;            // testo completo da nascondere
      string buffer;           // contenitore parziale/temporaneo
      string chiave;           // chiave cifratura del testo
      string cifrato;          // testo cifrato
      
      if(usaFile == true)      // input da file di testo
      {
         ifstream fileTesto;   // apro il file
         fileTesto.open(percorsoFile);
	 while (getline (fileTesto,buffer))  // salvo una riga alla volta fino alla fine del file
         {      
	     testo = testo + buffer + "\n";
	     buffer = "";
	 }
	 fileTesto.close();    // chiudo il file	
	 cout << "Input ricevuto dal file\n";
      }
      
      else
      {
           cout << "Inserisci il testo da nascondere, scrivi \"<end>\" per terminare:\n";
           cin >> buffer;
	   do 
	   {
		testo = testo + buffer + "\n";
		buffer = "";
		cin >> buffer;
	   } while(buffer != "<end>");
       }

      cout << "Inserisci la chiave di cifratura:\n";
      cin >> chiave;

      cifrato = testo;   // stringa cifrata della stessa lunghezza del testo in chiaro

      cripta(testo, chiave, cifrato);

      // SCRIVO SULL'IMMAGINE IL MESSAGGIO 

      int x=0;           // coordinata x del pixel
      int y=0;           // coordinata y del pixel

      int altezza=img.TellHeight();        // salvo altezza immagine
      int larghezza=img.TellWidth();       // salvo larghezza immagine

      for(int i=0;i<cifrato.length();i++)    // scorro tutto il testo
      {
          char carattere = cifrato.at(i);    // prendo un carattere alla volta

          for(int j=0;j<8;j++)
          {
               int blu = (int) img(x,y)->Blue; // estraggo il valore del blu

               blu &= ~1;                   // conservo 7 bit e azzero il meno significativo
               blu |= (carattere >> j) & 1; // metto in OR il risultato con il bit da scrivere

               img(x,y)->Blue = blu;        // sovrascrivo il valore blu
        
               x++;                         // aggiorno coordinate (mi sposto a destra)

               if(x==larghezza)             // vado a capo (sono arrivato all'ultimo pixel in larghezza)
               {
                   x=0;                     // riparto dalla prima colonna
                   y++;                     // linea successiva
               }
          }
      }

      // INSERISCO IL FINE STRINGA 

      for(int j=0;j<8;j++)                  // un carattere = 8 bit
      {
          int blu = (int) img(x,y)->Blue;   // estraggo il valore del blu

          blu &= ~1;                        // valore /0 = 00000000

          img(x,y)->Blue = blu;             // sovrascrivo il valore blu

          x++;                              // aggiorno coordinate (mi sposto a destra)

          if(x==larghezza)                  // vado a capo (sono arrivato all'ultimo pixel in larghezza)
          {
                x=0;                        // riparto dalla prima colonna
                y++;                        // linea successiva
          }
      }

      img.WriteToFile(percorso);            // output immagine
}

void rivela(BMP &img, char percorso[], bool &usaFile, char percorsoFile[])
{
      img.ReadFromFile(percorso);

      char carattere = '\0';    // singolo carattere decifrato, valore \0 = 00000000
      string testo = "";        // azzero inizialmente il testo
      string chiave;
      string decifrato;

      int x=0;           // coordinata x del pixel
      int y=0;           // coordinata y del pixel

      int altezza=img.TellHeight();        // salvo altezza immagine
      int larghezza=img.TellWidth();       // salvo larghezza immagine

      do    // scorro tutto il testo finchè non trovo il fine stringa (vedi condizione while)
      {
          for(int j=0;j<8;j++)                 // 8 bit = 1 byte = 1 carattere
          {
               int blu = (int) img(x,y)->Blue; // estraggo il valore del blu

               carattere = carattere << 1;     // shifto a sinistra
               carattere |= blu & 1;           // isolo l'ultimo bit

               x++;                            // aggiorno coordinate (mi sposto a destra nei pixel)

               if(x==larghezza)                // vado a capo (sono arrivato all'ultimo pixel in larghezza)
               {
                   x=0;                        // riparto dalla prima colonna
                   y++;                        // linea successiva
               }
           }

          // inverto ordine bit
          carattere = (carattere & 0xF0) >> 4 | (carattere & 0x0F) << 4;
          carattere = (carattere & 0xCC) >> 2 | (carattere & 0x33) << 2;
          carattere = (carattere & 0xAA) >> 1 | (carattere & 0x55) << 1;

          testo += carattere;                  // aggiungo il carattere in coda al testo da decifrare

      } while(carattere != '\0');              // in scrittura ogni messaggio ha un fine stringa: \0, ovvero 00000000

      cout << "Inserire chiave di cifratura:\n";
      cin >> chiave;

      decifrato = testo;   // stringa in chiaro della stessa lunghezza del testo cifrato

      cripta(testo, chiave, decifrato);

      for(int i = 0; i < decifrato.length(); i++)  // rimozione degli a capo
      {
          if (decifrato.at(i) == '\n') decifrato.at(i) = ' ';
      }
     
      decifrato.at(decifrato.length()-1) = ' ';  // rimozione del fine stringa
 
      // COMUNICO ALL'UTENTE IL RISULTATO 

      if(usaFile == true)   // nel file di testo
      {
	 // apro il file
         ofstream fileTesto;
         fileTesto.open(percorsoFile);
	 fileTesto << decifrato;   // ciò che ho decriptato, lo salvo nel file
	 fileTesto.close();    // chiudo il file	
	 cout << "Output inserito nel file.\n";
      }
      
      else cout << "Messaggio letto: " << decifrato; // oppure direttamente nel terminale
}

int main(int argc, char *argv[])      
{
	BMP img;
	string percorsoImg = "";      
	bool usaFile = false;
	string strPercorsoFile;
	char percorsoFile[20];  
	char percorso[20];  

        // PARSING PARAMETRI AVVIO

        if (argc < 2 || argc == 4 || argc > 5)    // numero di parametri insufficiente o eccessivo
	{
           cout << "Numero argomenti errato.\n";
	   mostraUtilizzo();
	   return 1;
        }

        if ((argv[1] == string("-h")) || (argv[1] == string("--help")))  // help, che mostra versione e autore, oltre all'utilizzo
	{
           mostraUtilizzo();
           cout << "\nVersione " << numVers << " | Riccardo Marin | riccardomarin23@gmail.com | 0x264834Ebf2Ac311429cFb66C587734Ea742586D5\n";
	   return 0;
	}

	else if( (argv[1] != string("-r")) && (argv[1] != string("--rivela")) && (argv[1] != string("-n")) && (argv[1] != string("--nascondi")) ) // l'utente digita un comando inesistente
	{
             cout << "La tua richiesta non e' chiara.\n";
	     mostraUtilizzo();
             return 1; 
	}        

	if (argc == 2)         // non è stato inserito il percorso immagine
	{
	   cout << "Devi specificare il percorso dell'immagine.\n";
           cout << "[Esempio: ./StegoMalignani -n areaTest/desktop.bmp]\n";
	   return 1;
	}

	else if (argc == 5)   // vengono utilizzati i file di testo (-f file.txt)
	{
 	    if ((argv[3] == string("-f")) || (argv[3] == string("--file"))) 
	    {		
		strPercorsoFile = argv[4];
		// fstream vuole il percorso come vettore di caratteri
        	strcpy (percorsoFile,strPercorsoFile.c_str());    // converto stringa in array di char
		
		// apro il file
                fstream fileTesto;
                fileTesto.open(percorsoFile, ios::in);
 
		// controllo esistenza file
                if (fileTesto.good() == true)  usaFile = true;
		
        	else                                                // se non trovo il file
        	{
             	     cout << "Il file di testo non esiste!\n\n";       // comunico errore
                     return 1;        
		}
		fileTesto.close();       // chiudo file
	    }

	    else  // l'utente digita un comando inesistente
	    {
                cout << "La tua richiesta non e' chiara.\n";
	        mostraUtilizzo();
                return 1; 
	    }	  		 	
        }

	percorsoImg = argv[2]; // salvo il parametro successivo (percorso immagine)
        strcpy (percorso,percorsoImg.c_str());    // converto stringa in array di char

        // apro il file
        ifstream file;
        file.open(percorso, ios::in);
 
        if (file.good() == true)          // controllo esistenza file
        {
	       // chiama una delle due funzioni disponibili (per inserire o estrarre dati dall'immagine) in base ai parametri passati dall'utente 
               if( (argv[1] == string("-r")) || (argv[1] == string("--rivela")) ) rivela(img, percorso, usaFile, percorsoFile);
               else if( (argv[1] == string("-n")) || (argv[1] == string("--nascondi")) ) nascondi(img, percorso, usaFile, percorsoFile);
          
               cout << "\nOperazioni completate con successo!\n";
	       return 0;
        }

        else                                                // se non trovo il file
        {
             cout << "Il file dell'immagine non esiste!\n"; // comunico errore
             file.close();                                  // chiudo file
             return 1;        
	}
}
