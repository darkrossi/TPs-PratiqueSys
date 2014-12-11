#ifndef _F_TP1_H
#define _F_TP1_H

#include "init.h"


uint16_t* ptr_mem(uint32_t lig, uint32_t col);

void ecrit_car(uint32_t lig, uint32_t col, char c, uint32_t cl, uint32_t cf, uint32_t ct);

void place_curseur(uint32_t lig, uint32_t col);

uint16_t position_curseur();

void efface_ecran(void);

void traite_car(char c);

void defilement(void);

void console_putbytes(char *chaine, int32_t taille);

void affiche_droite(char* chaine);


#endif				/* _F_TP1_H */
