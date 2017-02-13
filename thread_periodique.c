/************************************************************************************************************************
 Nom ....... : thread_periodique.c
 Objet ..... : Execution périodique d'un thread 
 Auteur .... : LP
 Date ...... : 19/11/2010
 Compilation native  : gcc -Wall -o thread_periodique thread_periodique.c -lpthread
 Compilation croisée : $(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc -Wall -o thread_periodique thread_periodique.c -lpthread
*************************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>

#define SIGNAL SIGALRM
#define TIMER ITIMER_REAL
#define PERIODE 10000
#define PRIORITE 50
#define DISCIPLINE SCHED_RR
#define DUREE 10

int latence_max = 0; 
int avance_max = 0;


/* Gestion du signal d'interruption */
void interruption(int cause, siginfo_t *HowCome, void *ptr) 
{
  unsigned long long valeur_precedente, valeur_courante;
  int delai;
  static int flag = 0;
  static struct timeval date;

  /* Initialisation de l'horadatage */
  if (flag == 0)
  {
    gettimeofday(&date, NULL);
    flag = 1;
  }
  else	
  {
    valeur_precedente = date.tv_sec * 1000000 + date.tv_usec;
    gettimeofday(&date, NULL);
    valeur_courante = date.tv_sec * 1000000 + date.tv_usec;
    delai = (int)(valeur_courante - valeur_precedente);
    printf("%d microsec.\n", delai);
		
    if(delai - PERIODE > latence_max)
      latence_max =  delai - PERIODE;

    if (PERIODE - delai > avance_max)
      avance_max = PERIODE - delai;
  }
}


/* Fonction du thread */
void *thread(void *nom)	
{
  struct sched_param param;
  struct itimerval tempo;
  struct sigaction action;
		
  /* Déclaration du signal */
  action.sa_sigaction = interruption;
  sigemptyset(&action.sa_mask);
  action.sa_flags = SA_SIGINFO;

  if (sigaction(SIGNAL, &action, 0)) 
  {
    perror("sigaction");
    exit(1);
  }

  /* Parametrage de la temporisation */
  tempo.it_interval.tv_sec = 0;
  tempo.it_interval.tv_usec = PERIODE;
  tempo.it_value.tv_sec = 0;
  tempo.it_value.tv_usec = PERIODE;
  setitimer(TIMER, &tempo, NULL); 

  /* Paramétrage d'ordonnancement du thread */	
  param.sched_priority = PRIORITE;

  if (pthread_setschedparam(pthread_self(), DISCIPLINE, &param))
    perror("pthread_setschedparam");

  /* Durée d'exécution */
  sleep(DUREE);

  return NULL;
}


/* Routine principale */
int main(void)	
{ 
  pthread_t id_thread;
  pthread_attr_t attributs;

  pthread_attr_init(&attributs);
  pthread_create(&id_thread, &attributs, thread, NULL);
  pthread_join(id_thread, NULL);

  /* Résultats de latence */	
  printf("Latence maximale : %d\n", latence_max);
  printf("Avance maximale : %d\n", avance_max);

  return EXIT_SUCCESS;
}
