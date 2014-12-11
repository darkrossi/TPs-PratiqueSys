#ifndef _INIT_H
#define _INIT_H

#include <cpu.h>
#include <inttypes.h>
#include "stdbool.h"

#define CLOCKFREQ 50

#define ELU 1
#define ACTIVABLE 2
#define ENDORMI 3
#define MORT 4

#define TAILLE_PILE_EXECUTION 512
#define TAILLE_TABLE_PROCESSUS 4

/* Structure de données : processus */
typedef struct processus {
	int pid;
	char nom[25];
	int etat;
	int zone_save[5];
	int pile[TAILLE_PILE_EXECUTION];
	uint32_t wake_up_time;
} processus;

/* Structure de données : table_processus */
typedef processus table_processus[TAILLE_TABLE_PROCESSUS];

#endif				/* _INIT_H */
