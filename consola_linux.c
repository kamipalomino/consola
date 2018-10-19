#include "consola_linux.h"



COMANDOS comandos[] =
{
  { "cd", com_cd, "Cambiar el directorio por /DIR" },
  { "delete", com_delete, "Delete FILE" },
  { "help", com_ayuda, "Muestra esta ayuda" },
  { "?", com_ayuda, "Muestra esta ayuda" },
  { "lista", com_lista, "Lista los archivos del directorio" },
  { "ls", com_lista, "Lista los archivos del directorio" },
  { "pwd", com_pwd, "Direcorio actual" },
  { "salir", com_salir, "Sale de la consola" },
  { "renombra", com_renombrar, "Renombrar FILE to NOMBRE_NUEVO" },
  { "estado", com_estado,"Muestra el estado del archivo" },
  { "ver", com_ver, "Ver contenido del FILE" },
  { (char *)NULL, (Function *)NULL, (char *)NULL },
};

void logConsola(){
	log_consola = log_create("consola_cami.log"," Soy la Consola de Cam :) ",true,LOG_LEVEL_TRACE);
}
char *dupstr (int s){
  char *r;
  r = xmalloc (strlen (s) + 1);
  strcpy (r, s);
  return (r);
}

void memoria(){
	lineas = malloc(l_syscom*(sizeof(char)));
	argumentos= malloc(l_syscom* (sizeof(char)));
	syscom = malloc((sizeof(char))*l_syscom);
}

int ejecutar_linea (char* line){
  register int i;
  COMANDOS *comando;
  char *word;
  /* Isolate the comando word. */
  i = 0;
  while (line[i] && whitespace(line[i]))
    i++;
  word = line + i;

  while (line[i] && !whitespace (line[i]))
    i++;

  if (line[i])
    line[i++] = '\0';

  comando = buscar_comando(word);

  if (!comando){
      /*fprintf ();
      //loggear(log, "LOG_LEVEL_WARNING"," %s: No such comando for FileMan. %s \n", word);*/
      log_warning(log_consola,"%s: No such comando for FileMan. %s \n ",word);/*
      //log_debug(log,("<%s>: No such comando for FileMan. <%s> \n ",stderr,  word) );*/
      return (-1);
    };

 while (whitespace (line[i]))
    i++;

  word = line + i;

  return ((*(comando->funcion)) (word));
}

COMANDOS* buscar_comando (char *nombre){
  register int i;

  for (i = 0; comandos[i].nombre; i++)
    if (strcmp (nombre, comandos[i].nombre) == 0)
      return (&comandos[i]);

  return ((COMANDOS *)NULL);
}

char* stripwhite (char* string){
  register char *s, *t;

  for (s = string; whitespace(*s); s++);
    
  if (*s == 0)
    return (s);

  t = s + strlen (s) - 1;
  while (t > s && whitespace(*t))
    t--;
  *++t = '\0';

  free(t);
  return(s);
}

void initialize_readline(){
  /* Allow conditional parsing of the ~/.inputrc file. */
  rl_readline_name = "Hola Cami: ";


  rl_attempted_completion_function = (CPPFunction *)completar_nombre();
}

char **completar_nombre(char* text, int start, int end){
  char **matches;

  matches = (char **)NULL;
  if (start == 0){
    matches = completion_matches (text, comando_generador);
  };

  return (matches);
}

char* comando_generador (char* text, int state){
  static int list_index, len;
  char *nombre_nuevo;

  if (!state){
      list_index = 0;
      len = strlen(text);
    };

  while (nombre_nuevo = comandos[list_index].nombre){
      list_index++;

      if (strncmp (nombre_nuevo, text, len) == 0){
        return (dupstr(nombre_nuevo));
      };
    };


  return ((char *)NULL);
}

/* **************************************************************** */
/*                                                                  */
/*                       FileMan comandos                           */
/*                                                                  */
/* **************************************************************** */


void* com_lista (char* arg){
  if (!arg)
    arg = "";

  sprintf (syscom, "ls -FClg %s", arg);
  log_info(log_consola,"ls -FClg %s", arg);
  return (system (syscom));
}

int com_ver(char* arg){
  if (!argumentos_validos("ver", arg)){
	  log_warning(log_consola, "Faltan argumentos?? \n");
    return (EXIT_FAILURE);
  }else{
  sprintf (syscom, "more %s", arg);
  return (system (syscom));
  };

}

int com_renombrar (char* arg){
  too_dangerous("renombrar");
  return (1);
}

int com_estado (char* arg){
  struct stat finfo;

  if (!argumentos_validos("estado", arg))
    return (1);

  if (stat (arg, &finfo) == -1){
      perror (arg);
      return (1);
    }

  printf ("Estadísticas `%s':\n", arg);
  log_info(log_consola,"Estadísticas `%s':\n", arg);
  printf ("%s has %d link%s, and is %d byte%s in length.\n", arg,
	  	  finfo.st_nlink,
          (finfo.st_nlink == 1) ? "" : "s",
          finfo.st_size,
          (finfo.st_size == 1) ? "" : "s");
  log_info(log_consola, "%s has %d link%s, and is %d byte%s in length.\n", arg,
      finfo.st_nlink,
      (finfo.st_nlink == 1) ? "" : "s",
      finfo.st_size,
      (finfo.st_size == 1) ? "" : "s");

  printf ("\n Inode último cambio at: %s", ctime (&finfo.st_ctime));
  log_trace(log_consola,"\n Inode último cambio at: %s", ctime (&finfo.st_ctime));

  printf ("\n Último acceso at: %s", ctime (&finfo.st_atime));
  log_trace(log_consola,"\n Último acceso at: %s", ctime (&finfo.st_atime));
  printf ("\n Última modificación at: %s", ctime (&finfo.st_mtime));
  log_trace(log_consola,"\n Última modificación at: %s", ctime (&finfo.st_mtime));
  return (0);
}

int com_delete (char* arg){
  too_dangerous ("delete");
  return (1);
}

/* Print out help for ARG, or for all of the comandos if ARG is
   not present. */
int com_ayuda (char* arg){
  register int i;
  int printed = 0;

  for (i = 0; comandos[i].nombre; i++){
      if (!*arg || (strcmp (arg, comandos[i].nombre) == 0)){
          printf ("%s\t\t%s.\n", comandos[i].nombre, comandos[i].docExtra);
          printed++;
        }
    }

  if (!printed){
      printf ("No comandos match `%s'.  Possibilties are:\n", arg);

      for (i = 0; comandos[i].nombre; i++){

          if (printed == 6){
              printed = 0;
              printf ("\n");
            };

          printf ("%s\t", comandos[i].nombre);
          printed++;
        };

      if (printed)
        printf ("\n");
    }
  return (0);
}

int com_cd(char *arg){
  if (chdir (arg) == -1) {
      perror (arg);

      return (EXIT_FAILURE);
    };

  com_pwd ("");
  return (0);
}

/* Print out the current working directory. */
int com_pwd (char* ignore){
  char* dir = calloc(100, sizeof(char));
  char* s = calloc(100, (sizeof(char)+2));

  s = getwd(dir);
  if (s == 0) {

      log_error(log_consola, "Error getting pwd: %s\n", dir);
      free(s);
      free(dir);
      return (EXIT_FAILURE);
    };

  printf ("Current directory is %s\n", dir);
  free(dir);
  free(s);
  return (0);
}

/* The user wishes to quit using this program.  Just set DONE non-zero. */
int com_salir (char* arg){
  done = 1;
  log_trace(log_consola, "Saliendo de la terminal CAM \n");
  return (0);
}


int too_dangerous (char* caller){
  fprintf (stderr, "%s: Too dangerous for me to distribute.  Write it yourself.\n", caller);
  log_warning(log_consola,"%s: Peligro!!! Peligro!!!  Write it yourself.\n", caller);
  return(1);
}


int argumentos_validos(char *caller, char* arg){
  if (!arg || !*arg){
      fprintf (stderr, "%s: Se requiere argumento.\n", caller);
      log_warning(log_consola, "%s: Faltan Agumentos \n", caller);
      return (0);
    }

  return (1);
}
