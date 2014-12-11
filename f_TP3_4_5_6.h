#ifndef _F_TP3_4_5_6_H
#define _F_TP3_4_5_6_H

#include "init.h"

void ctx_sw(int[], int[]);

uint32_t nbr_secondes();

int32_t cree_processus(char *nom, void (*code)(void), int etat);

void idle(void);
void proc1(void);
void proc2(void);
void proc3(void);

void ordonnance();

int32_t mon_pid(void);

char* mon_nom(void);

void dors(uint32_t nbr_secs);

void fin_processus(void);

#endif				/* _F_TP3_4_5_6_H */
