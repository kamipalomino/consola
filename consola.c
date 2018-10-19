/*
 ============================================================================
 Name        : consola.c
 Author      : cami
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
*/
#include "consola_linux.h"


int main (int argc, char** argv){
	logConsola();
	memoria();


  nombre_programa = argv[0];

  initialize_readline();

  while(done != 1){
	  lineas = readline("Hola Cami: ");
      if (!lineas){
    	  log_error(log_consola,"AlgoSalioMal");
    	  //chauMemoria();
    	  done = 1;
    	  break;
      };

      argumentos = stripwhite(lineas);

      if (*argumentos){
          add_history(argumentos);
          ejecutar_linea(argumentos);
      };

      free (lineas);
    };
  //chauMemoria();
  exit (0);
}
