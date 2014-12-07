#include <cpu.h>
#include "inttypes.h"
#include "string.h"
#include "stdio.h"

#include "segment.h"
#include "fonctions.h"

#include <check.h>

/*extern uint16_t* ptr_mem(uint32_t lig, uint32_t col);
extern void efface_ecran(void);
extern void ecrit_car(uint32_t lig, uint32_t col, char c, uint32_t cl, uint32_t cf, uint32_t ct);
extern void place_curseur(uint32_t lig, uint32_t col);
extern void defilement(void);
extern void init_traitant_IT(uint32_t num_IT, void (*traitant)(void));
extern void traitant_IT_32(void);
extern void masque_IRQ(uint32_t num_IRQ, bool masque);

void kernel_start(void)
{
	check_empty_screen(efface_ecran);
	check_ptr_mem(ptr_mem);
	check_set_cursor(place_curseur);
	check_write_char(ecrit_car);
	// void (*write_char)(uint32_t, uint32_t, char, uint32_t, uint32_t, uint32_t))
	check_scrolling(defilement);
	check_idt_entry(init_traitant_IT, 32, traitant_IT_32);
	check_IRQ(masque_IRQ);

    sti();
    while (1) {
        hlt();
    }
}*/

extern void proc1(void);
extern void proc2(void);
extern void proc3(void);

extern void traitant_IT_32(void);

void kernel_start(void){

	efface_ecran();
	place_curseur(0,0);
	indice_dernier_elt_tab_processus = 0;

	masque_IRQ(0, false);
	
	init_traitant_IT(32, traitant_IT_32);
	set_clock();

	// initialisation des structures de processus
	processus proc_idle = newProcessus(indice_dernier_elt_tab_processus,"idle", ELU);
	tab_processus[indice_dernier_elt_tab_processus] = proc_idle;

	processus_elu = proc_idle;

	// demarrage du processus par defaut

	cree_processus(proc1, "proc_proc1");
	cree_processus(proc2, "proc_proc2");
	cree_processus(proc3, "proc_proc3");
	

	idle();

 	while (1) {
		// cette fonction arrete le processeur
		hlt();
	}

}

// *************************************************** INTERRUPTIONS *************************************************


/*void kernel_start(void)
{
	efface_ecran();
	affiche_droite("Essai start wazaaaa");

	masque_IRQ(0, false);
	
	init_traitant_IT(32, traitant_IT_32);
	set_clock();
	
	sti();
    while (1) {
        // cette fonction arrete le processeur
        hlt();
    }
}*/



