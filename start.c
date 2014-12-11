#include <cpu.h>
#include "inttypes.h"
#include "string.h"
#include "stdio.h"
#include "segment.h"

#include "init.h" /* Implémente les structures de données et les constantes */

#include "f_TP1.h" /* Inclue les fonctions liés au TP1 sur la gestion de l'écran */
#include "f_TP2.h"
#include "f_TP3_4_5_6.h"

#include <check.h>


/* Initialiation des variables globales */
double time_double = 0; /* Temps écoulé de type double */
int indice_dernier_elt_tab_processus = -1; /* Indice du dernier processus créé */
table_processus tab_processus; /* La table des processus */
processus processus_elu; /* Variable temporaire désignant le processus élu à un instant t */


extern void proc1(void);
extern void proc2(void);
extern void proc3(void);
extern void traitant_IT_32(void);


void kernel_start(void){

	/* Initialisation de l'écran et du curseur */
	efface_ecran();
	place_curseur(0,0);

	/* Initialisation des interruptions */
	masque_IRQ(0, false);
	init_traitant_IT(32, traitant_IT_32);

	/* Initialisation de l'horloge */
	set_clock();

	/* Initialisation des processus */
	
	cree_processus("idle", NULL, ELU);
	cree_processus("proc_proc1", proc1, ACTIVABLE);
	cree_processus("proc_proc2", proc2, ACTIVABLE);
	cree_processus("proc_proc3", proc3, ACTIVABLE);

	processus_elu = tab_processus[0];

	
	/* Démarrage du processus par defaut */
	idle();

 	while(1) {
		hlt();
	}
}
