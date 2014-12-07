#include <cpu.h>
#include <inttypes.h>
#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include <stdlib.h>

#include "segment.h"
#include "fonctions.h"

int time = 0;
double time_double = 0;

uint16_t* ptr_mem(uint32_t lig, uint32_t col) {
	return (uint16_t*) (0xB8000 + 2*(lig*80+col));	
}

void ecrit_car(uint32_t lig, uint32_t col, char c, uint32_t cl, uint32_t cf, uint32_t ct) {
	uint16_t* adresse = ptr_mem(lig,col);
	uint8_t ascii = (uint8_t)c;
	uint8_t param = (cl << 7) | (cf << 4) | (ct);
	*adresse= (param << 8) + ascii;
}

void place_curseur(uint32_t lig, uint32_t col) {
	uint16_t port_de_commande = 0x3D4;
	uint16_t port_de_donnees = 0x3D5;
	uint16_t position = col + lig * 80;
	uint8_t poids_faible = position & 0b11111111;
	uint8_t poids_fort = (position) >> 8;
	outb(0x0F, port_de_commande);
	outb(poids_faible, port_de_donnees);
	outb(0x0E, port_de_commande);
	outb(poids_fort, port_de_donnees);
}

uint16_t position_curseur(){
	uint16_t port_de_commande = 0x3D4;
	uint16_t port_de_donnees = 0x3D5;
	uint16_t position;
	uint8_t poids_faible;
	uint8_t poids_fort;

	outb(0x0F, port_de_commande);
	poids_faible = inb(port_de_donnees);
	outb(0x0E, port_de_commande);
	poids_fort = inb(port_de_donnees);

	position = (poids_fort << 8) + poids_faible;
	return position;
}

void efface_ecran(void){ //80 colonne 25 lignes
	uint32_t i, j;
	for(i=0; i<80; i++) {
		for(j=0; j<25; j++) {
			// place_curseur(j,i);
			ecrit_car(j,i,' ',0,4,0);
		}
	}
}

void traite_car(char c) {

	uint16_t position = position_curseur();
	
	if (c>=32 && c<=126) {
		ecrit_car(position/80,position%80,c,0,4,0);
	}

	else if (c == 8) {
		if (position%80 != 0) { 
			place_curseur(position/80,0);
		}
	}
	else if (c == 9) {
		if (position%80 + 8 < 80) { 
			place_curseur(position/80,position%80 + 8);
		}
	}
	else if (c == 10) {
		place_curseur(position/80+1, 0);
	}
	else if (c == 12) {
		efface_ecran();
		place_curseur(0, 0);
		
	}
	else if (c == 13) {
		place_curseur(0, 0);
	}
}

void defilement(void) {
	int i,j;
	uint16_t* adress_src, *adress_dest;
	for(i=0; i<24;i++) { // ligne
		for(j=0; j<80;j++) { // colonne
			adress_src = ptr_mem(i+1,j);
			adress_dest = ptr_mem(i,j);
			adress_dest = (uint16_t*)memmove((uint16_t*)adress_dest, (uint16_t*)adress_src, 2);
		}
	}
	for(j=0; j<80;j++) { // colonne
		ecrit_car(24,j,' ',0,4,0);
	}
	
}

void console_putbytes(char *chaine, int32_t taille) {
	int i = 1;
	uint16_t position = position_curseur();

	while (i-1<taille) {
		if((position+i)/80 > 24){
			defilement();
			position = position - 80;
		}
		
		place_curseur((position+i)/80, (position+i-1)%80);
		traite_car(chaine[i-1]);
		i++;
	}
}

void affiche_droite(char* chaine) { // CHECKED
	int i = 0;	
	int taille = 0;

	uint16_t position = position_curseur();

	place_curseur(1, 80);
	while (chaine[i] != '\0') {
		i++;
	}
	taille = i;
	for(i=taille; i>=0; i--) {
		traite_car(chaine[i]);
		place_curseur(1, 80 -(taille - i)-1);
	}

	place_curseur(position/80, position%80);
}

void tic_PIT(void) {
	char message[8] = "";
	outb(0x20, 0x20); // Le contrôleur d'interruptions peut écouter d'autres interruptions éventuelles
	time_double += 1./CLOCKFREQ;
	
	time = (int)time_double;
	
	sprintf(message, "%02d:%02d:%02d",time/3600,time/60, time%60);

	affiche_droite(message);
	ordonnance();
}

uint32_t nbr_secondes(){
	return (uint32_t)time_double;
}

void init_traitant_IT(uint32_t num_IT, void (*traitant)(void)) { // CHECKED
	uint32_t poids_faible_adresse_traitant = (uint32_t)traitant & 0b1111111111111111;
	uint32_t poids_forts_adresse_traitant = (uint32_t)traitant >> 16;

	uint32_t entree_IDT_1 = ((uint32_t)KERNEL_CS << 16) + poids_faible_adresse_traitant;
	uint32_t entree_IDT_2 = (poids_forts_adresse_traitant << 16) + 0x8E00;

	uint32_t* adresse1 = (uint32_t*)(0x1000 + num_IT*8);	
	uint32_t* adresse2 = (uint32_t*)(0x1000 + num_IT*8+4);
	*adresse1 = entree_IDT_1;
	*adresse2 = entree_IDT_2;
}

void set_clock(void) { // CHECKED
	uint32_t QUARTZ = 0x1234DD;	
	uint32_t val_outb1 = (QUARTZ / CLOCKFREQ) % 256;
	uint32_t val_outb2 = (QUARTZ / CLOCKFREQ) >> 8;
	
	outb(0x34,0x43);
	outb(val_outb1, 0x40);
	outb(val_outb2, 0x40);
}

void masque_IRQ(uint32_t num_IRQ, bool masque) { // CHECKED
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

processus newProcessus(int pid, char nom[25], int etat) {
	processus proc;

	proc.pid = pid;
	strcpy(proc.nom, nom);
	proc.etat = etat;
	
	return proc;
}

/*void idle(void){
    for (int i = 0; i < 3; i++) {
        printf("[idle] je tente de passer la main a proc1...\n");
        ctx_sw(tab_processus[0].zone_save, tab_processus[1].zone_save);
        printf("[idle] proc1 m'a redonne la main\n");
    }
    printf("[idle] je bloque le systeme\n");
    hlt();
}

void proc1(void){
    int i;
    for (i = 0; i<5; i++) {
        printf("[proc1] idle m'a donne la main\n");
        printf("[proc1] je tente de lui la redonner...\n");
        ctx_sw(tab_processus[1].zone_save, tab_processus[0].zone_save);
    }
}*/

/*void idle(void)
{
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100000000; i++);
        ordonnance();
    }
}

void proc1(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100000000; i++);
        ordonnance(); 
    }
}

void proc2(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100000000; i++);
        ordonnance(); 
    }
}

void proc3(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100000000; i++);
        ordonnance(); 
    }
}*/

/*void idle(void)
{
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100 * 1000 * 1000; i++);
        sti();
        hlt();
        cli();
    }
}

void proc1(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100 * 1000 * 1000; i++);
        sti();
        hlt();
        cli();
    }
}

void proc2(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100 * 1000 * 1000; i++);
        sti();
        hlt();
        cli();
    }
}

void proc3(void) {
    for (;;) {
        printf("[%s] pid = %i\n", mon_nom(), mon_pid());
        for (int32_t i = 0; i < 100 * 1000 * 1000; i++);
        sti();
        hlt();
        cli();
    }
}*/

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

	if(indice_dernier_elt_tab_processus != 0){

		for(i=0; i<=indice_dernier_elt_tab_processus; i++){ // On parcourt tous les processus

			if (tab_processus[i].pid == processus_elu.pid){ // Si on tombe sur le processus élu ou qui vient d'être endormi

				if (tab_processus[i].etat == ELU) tab_processus[i].etat = ACTIVABLE; // Si il était élu alors on le met en activable

				for(j = i+1; j <= i+indice_dernier_elt_tab_processus+1; j++){ // On part à la recherche d'un processus activable
					k = j%(indice_dernier_elt_tab_processus+1);

					if (tab_processus[k].etat == ENDORMI){ // Si on tombe sur un processus endormi
						uint32_t reveil = tab_processus[k].wake_up_time;

						if (reveil <= nbr_secondes()){ // On regarde si il peut être reveillé 

							tab_processus[k].wake_up_time = 0;				
							tab_processus[k].etat = ELU;

							processus_elu = tab_processus[k];

							ctx_sw(tab_processus[i].zone_save, tab_processus[k].zone_save); // On sort
							return;
						}
					}

					if (tab_processus[k].etat == ACTIVABLE){ // Si on tombe sur un processus activable				
				
						tab_processus[k].etat = ELU;

						processus_elu = tab_processus[k];

						ctx_sw(tab_processus[i].zone_save, tab_processus[k].zone_save); // On sort
						return;
					}
				}
			}

			if (tab_processus[i].etat == ENDORMI){ // Si on tombe sur un processus endormi alors on voit si on peut le réveiller 
				uint32_t reveil = tab_processus[i].wake_up_time;
				if (reveil <= nbr_secondes()){
					tab_processus[i].etat = ACTIVABLE;
					tab_processus[i].wake_up_time = 0;
				}
			}
		} // FIN FOR
	}
}

int32_t mon_pid(void){
	return processus_elu.pid;
}

char* mon_nom(void){
	return processus_elu.nom;
}

int32_t cree_processus(void (*code)(void), char *nom){

	if( indice_dernier_elt_tab_processus < TAILLE_TABLE_PROCESSUS-1){
		indice_dernier_elt_tab_processus++;
		int i = indice_dernier_elt_tab_processus;

		tab_processus[i] = newProcessus(i, nom, ACTIVABLE);
		tab_processus[i].zone_save[1] = (uint32_t)&tab_processus[i].pile[511];
		tab_processus[i].pile[511] = (uint32_t)*code;

		return i;
	}
	else return -1;
}

void dors(uint32_t nbr_secs){
	int i;	

	sti();
    
	for(i=0; i<=indice_dernier_elt_tab_processus; i++){

		if (tab_processus[i].pid == processus_elu.pid){
			tab_processus[i].etat = ENDORMI;
			tab_processus[i].wake_up_time = nbr_secondes() + nbr_secs;
			break;
		}
	}

	while(nbr_secondes() < tab_processus[i].wake_up_time){
		hlt();
	}

	cli();
}

void fin_processus(void){
	int i;	
    
	for(i=0; i<=indice_dernier_elt_tab_processus; i++){

		if (tab_processus[i].pid == processus_elu.pid){
			tab_processus[i].etat = MORT;
			break;
		}
	}

	ordonnance();
}
