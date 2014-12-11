#include <cpu.h>
#include <inttypes.h>
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include <stdlib.h>

#include "f_TP3_4_5_6.h"

extern double time_double;
extern table_processus tab_processus;
extern int indice_dernier_elt_tab_processus;
processus processus_elu;

/* Retourne le temps écoulé depuis le début */
uint32_t nbr_secondes(){
	return (uint32_t)time_double;
}

int32_t cree_processus(char *nom, void (*code)(void), int etat){

	if( indice_dernier_elt_tab_processus < TAILLE_TABLE_PROCESSUS-1){ /* Si on peut encore ajouter un processus dans la table de processus */
		processus proc;

		indice_dernier_elt_tab_processus++; /* On incrémente l'indice du dernier élément du tableau (initialisé à -1) */
		int i = indice_dernier_elt_tab_processus;

		proc.pid = i; /* Pid */
		strcpy(proc.nom, nom); /* Nom */
		proc.etat = etat; /* Etat */

		tab_processus[i] = proc; /* On ajoute le processus dans la table des processus */

		if(*code != NULL){ /* Si on veut créer le processus qui sera le premier appelé alors on n'initialise pas ses registres */
			tab_processus[i].zone_save[1] = (uint32_t)&tab_processus[i].pile[511];
			tab_processus[i].pile[511] = (uint32_t)*code;
		}

		return i;
	}
	else return -1;
}

void idle()
{
    for (;;) {
        sti();
        hlt();
        cli();
    }
}

void proc1(void)
{
    for (int32_t i = 0; i < 2; i++) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(), mon_nom(), mon_pid());
        dors(2);
    }
    fin_processus();
}

void proc2(void)
{
    for (int32_t i = 0; i < 3; i++) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(), mon_nom(), mon_pid());
        dors(3);
    }
    fin_processus();
}

void proc3(void)
{
    for (int32_t i = 0; i < 5; i++) {
        printf("[temps = %u] processus %s pid = %i\n", nbr_secondes(), mon_nom(), mon_pid());
        dors(5);
    }
    fin_processus();
}

void ordonnance(){
	int i, j, k;

	if(indice_dernier_elt_tab_processus != 0){ /* Si on a plus qu'un processus */

		for(i=0; i<=indice_dernier_elt_tab_processus; i++){ /* On parcourt tous les processus */

			if (tab_processus[i].pid == processus_elu.pid){ /* Si on tombe sur le processus élu ou qui vient d'être endormi */

				if (tab_processus[i].etat == ELU) tab_processus[i].etat = ACTIVABLE; /* Si il était élu alors on le met en activable */

				for(j = i+1; j <= i+indice_dernier_elt_tab_processus+1; j++){ /* On part à la recherche d'un processus activable */
					k = j%(indice_dernier_elt_tab_processus+1); 

					if (tab_processus[k].etat == ENDORMI){ /* Si on tombe sur un processus endormi */
						uint32_t reveil = tab_processus[k].wake_up_time;

						if (reveil <= nbr_secondes()){ /* On regarde si il peut être reveillé */

							tab_processus[k].wake_up_time = 0;				
							tab_processus[k].etat = ELU;

							processus_elu = tab_processus[k];

							ctx_sw(tab_processus[i].zone_save, tab_processus[k].zone_save);
							return;
						}
					}

					if (tab_processus[k].etat == ACTIVABLE){ /* Si on tombe sur un processus activable */				
				
						tab_processus[k].etat = ELU;

						processus_elu = tab_processus[k];

						ctx_sw(tab_processus[i].zone_save, tab_processus[k].zone_save);
						return;
					}
				}
			}

			if (tab_processus[i].etat == ENDORMI){ /* Si on tombe sur un processus endormi alors on voit si on peut le réveiller */
				uint32_t reveil = tab_processus[i].wake_up_time;

				if (reveil <= nbr_secondes()){ /* Si son réveil a déjà sonné alors on le rend activable */
					tab_processus[i].etat = ACTIVABLE;
					tab_processus[i].wake_up_time = 0;
				}
			}
		}
	}
}

/* Retourn le pid du processus élu */
int32_t mon_pid(void){
	return processus_elu.pid;
}

/* Retourne le nom du processus élu */
char* mon_nom(void){
	return processus_elu.nom;
}

/* Endors un processus et attend le prochain tic d'horloge */
void dors(uint32_t nbr_secs){
	int indice_processus_elu = processus_elu.pid;	

	sti();
    
	tab_processus[indice_processus_elu].etat = ENDORMI;
	tab_processus[indice_processus_elu].wake_up_time = nbr_secondes() + nbr_secs;

	while(nbr_secondes() < tab_processus[indice_processus_elu].wake_up_time){
		hlt();
	}

	cli();
}

/* Met fin à un processus en changeant son état en MORT puis fait appel à ordonnance */
void fin_processus(void){
	int indice_processus_elu = processus_elu.pid;
   
	tab_processus[indice_processus_elu].etat = MORT;

	ordonnance();
}
