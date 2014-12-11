#include <cpu.h>
#include <inttypes.h>
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include <stdlib.h>

#include "segment.h"
#include "f_TP2.h" 
#include "f_TP1.h" /* On doit récupérer les fonctions liées à l'affiche à l'écran */
#include "f_TP3_4_5_6.h" /* On doit récupérer la fonction ordonnance */

#include "init.h"

extern double time_double;


void tic_PIT(void) {
	char message[8] = "";
	outb(0x20, 0x20); /* Le contrôleur d'interruptions peut écouter d'autres interruptions éventuelles */
	time_double += 1./CLOCKFREQ;
	
	int time = (int)time_double;
	
	sprintf(message, "%02d:%02d:%02d",time/3600,time/60, time%60); /* On copie l'heure dans le char* message */

	affiche_droite(message); /* On affiche le temps écoulé en haut à droite de l'écran */

	ordonnance();
}

void init_traitant_IT(uint32_t num_IT, void (*traitant)(void)) {
	uint32_t poids_faible_adresse_traitant = (uint32_t)traitant & 0b1111111111111111;
	uint32_t poids_forts_adresse_traitant = (uint32_t)traitant >> 16;

	uint32_t entree_IDT_1 = ((uint32_t)KERNEL_CS << 16) + poids_faible_adresse_traitant;
	uint32_t entree_IDT_2 = (poids_forts_adresse_traitant << 16) + 0x8E00;

	uint32_t* adresse1 = (uint32_t*)(0x1000 + num_IT*8);	
	uint32_t* adresse2 = (uint32_t*)(0x1000 + num_IT*8+4);
	*adresse1 = entree_IDT_1;
	*adresse2 = entree_IDT_2;
}

void set_clock(void) {
	uint32_t QUARTZ = 0x1234DD;	
	uint32_t val_outb1 = (QUARTZ / CLOCKFREQ) % 256;
	uint32_t val_outb2 = (QUARTZ / CLOCKFREQ) >> 8;
	
	outb(0x34,0x43);
	outb(val_outb1, 0x40);
	outb(val_outb2, 0x40);
}

void masque_IRQ(uint32_t num_IRQ, bool masque) {
	uint8_t current_mask = inb(0x21);
	uint8_t value;

	if (masque) {
		value = current_mask | (0x1 << num_IRQ);
	}
	else {
		value = current_mask & ~(0x1 << num_IRQ);
	}

	outb(value,0x21);
}
