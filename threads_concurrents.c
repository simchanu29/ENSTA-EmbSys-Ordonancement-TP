/****************************************************************************************************************************
 Nom ....... : threads_concurrents.c
 Objet ..... : Execution simultanée de deux threads (en root) 
 Auteur .... : LP
 Date ...... : 18/11/2010
 Compilation native  : gcc -Wall -o threads_concurrents threads_concurrents.c -lpthread
 Compilation croisée : $(ARMADEUS_TOOLCHAIN_PATH)/arm-linux-gcc -Wall -o threads_concurrents threads_concurrents.c -lpthread
*****************************************************************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <sys/time.h>

#define HAUTE_PRIORITE 70
#define BASSE_PRIORITE 1
#define FIFO SCHED_FIFO
#define RR SCHED_RR
#define SIGNAL SIGALRM
#define TIMER ITIMER_REAL
#define PERIODE 1000
#define DUREE 3000

int latence_max = 0; 
int avance_max = 0;
unsigned long duree_thread_hp;
unsigned long duree_thread_bp;
struct timeval date_debut;


/* Gestion du signal d'interruption */
void interruption(int cause, siginfo_t *HowCome, void *ptr) 
{
  unsigned long long valeur_precedente, valeur_courante;
  int periode;
  static int flag = 0;
  struct timeval date_courante;
	
  /* Initialisation de l'horadatage */
  if (flag == 0)
  {
    gettimeofday(&date_debut, NULL);
    date_courante = date_debut;
    flag = 1;
  }
  else	
  {
    valeur_precedente = date_courante.tv_sec * 1000000 + date_courante.tv_usec;
    gettimeofday(&date_courante, NULL);
    valeur_courante = date_courante.tv_sec * 1000000 + date_courante.tv_usec;
    periode = (int)(valeur_courante - valeur_precedente);
    printf("%d microsec.\n", periode);
  		
    if (periode - PERIODE > latence_max)
      latence_max =  periode - PERIODE;

    if (PERIODE - periode > avance_max)
      avance_max = PERIODE - periode;
  }
}


/* Fonction du thread de haute priorite */
void *thread_hp(void *id)	
{
  int i, j;
  int discipline = FIFO;
  struct sched_param param;
  struct itimerval tempo;
  struct sigaction action;
  struct timeval date_courante;
		
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
  param.sched_priority = HAUTE_PRIORITE;

  if (pthread_setschedparam(pthread_self(), discipline, &param))
    perror("Thread HP : pthread_setschedparam");

  if (!pthread_getschedparam(pthread_self(), &discipline, &param))
    printf("HP -- Execution %s/%d\n", (discipline == SCHED_FIFO ? "FIFO" : 
           (discipline == SCHED_RR ? "RR" : 
           (discipline == SCHED_OTHER ? "OTHER" : "inconnu"))), 
           param.sched_priority);
  else
    perror("Thread HP : pthread_getschedparam");

  /* Traitement */
  for (i = 0; i < DUREE; i++)
    for (j = 0; j < DUREE; j++)

  /* Calcul du temps écoulé */
  gettimeofday(&date_courante, NULL);
  duree_thread_hp = (date_courante.tv_sec * 1000000 + date_courante.tv_usec) - (date_debut.tv_sec * 1000000 + date_debut.tv_usec);

  return NULL;
}


/* Fonction du thread de basse priorite */
void *thread_bp(void *id)	
{
  int i, j;
  int discipline = FIFO;
  struct sched_param param;
  struct itimerval tempo;
  struct sigaction action;
  struct timeval date_courante;
		
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
  param.sched_priority = BASSE_PRIORITE;

  if (pthread_setschedparam(pthread_self(), discipline, &param))
    perror("Thread BP : pthread_setschedparam");

  if (!pthread_getschedparam(pthread_self(), &discipline, &param))
    printf("BP -- Execution %s/%d\n", (discipline == SCHED_FIFO ? "FIFO" : 
           (discipline == SCHED_RR ? "RR" : 
           (discipline == SCHED_OTHER ? "OTHER" : "inconnu"))),
	   param.sched_priority);
  else
    perror("Thread BP : pthread_getschedparam");

  /* Traitement */
  for (i = 0; i < DUREE; i++)
    for (j = 0; j < DUREE; j++)

  /* Calcul du temps écoulé */
  gettimeofday(&date_courante, NULL);
  duree_thread_bp = (date_courante.tv_sec * 1000000 + date_courante.tv_usec) - (date_debut.tv_sec * 1000000 + date_debut.tv_usec);
	
  return NULL;
}


/* Routine principale */
int main(void)	
{
  pthread_t id_thread_hp, id_thread_bp;
  pthread_attr_t attributs;

  pthread_attr_init(&attributs);
  pthread_create(&id_thread_hp, &attributs, thread_hp, (void *)1);
  pthread_create(&id_thread_bp, &attributs, thread_bp, (void *)2);
  pthread_join(id_thread_hp, NULL);
  pthread_join(id_thread_bp, NULL);	

  /* Résultat de latence */	
  printf("Latence maximale : %d\n", latence_max);
  printf("Avance maximale : %d\n", avance_max);

  /* Affichage des durées d'exécution */
  printf("Duree execution thread haute priorite : %ld\n", duree_thread_hp);
  printf("Duree execution thread basse priorite : %ld\n", duree_thread_bp);

  return EXIT_SUCCESS;
}
