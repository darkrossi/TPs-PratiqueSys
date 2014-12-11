#include "f_TP1.h"

#include <cpu.h>
#include <inttypes.h>
#include "string.h"
#include "stdbool.h"

uint16_t* ptr_mem(uint32_t lig, uint32_t col) {
	return (uint16_t*) (0xB8000 + 2*(lig*80+col));	
}

void ecrit_car(uint32_t lig, uint32_t col, char c, uint32_t cl, uint32_t cf, uint32_t ct) {
	uint16_t* adresse = ptr_mem(lig,col); /* On récupère l'adresse associée au pixel lig, col */
	uint8_t ascii = (uint8_t)c; /* Code ascii du caractère c */
	uint8_t param = (cl << 7) | (cf << 4) | (ct);
	*adresse= (param << 8) + ascii; /* On copie à l'adresse du pixel sa nouvelle valeur */
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

	for(i=0; i<80; i++) { /* On lit en ligne */
		for(j=0; j<25; j++) { /* Puis en colonne */
			ecrit_car(j,i,' ',0,4,0); /* On écrit un espace dans tous les pixels de l'écran */
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

	for(i=0; i<24;i++) { /* On parcourt en ligne */
		for(j=0; j<80;j++) { /* On parcourt en colonne */
			adress_src = ptr_mem(i+1,j); /* On stocke l'adresse dans laquelle est stockée les informations sur le pixel (i+1,j) */
			adress_dest = ptr_mem(i,j); /* On stocke l'adresse dans laquelle est stockée les informations sur le pixel (i,j) */
			memmove((uint16_t*)adress_dest, (uint16_t*)adress_src, 2); /* On remplace les données à l'adresse destination par les 
				données à l'adresse source */
		}
	}

	for(j=0; j<80;j++) { /* On vide la dernière ligne */
		ecrit_car(24,j,' ',0,4,0);
	}
	
}

void console_putbytes(char *chaine, int32_t taille) {
	int i = 1; /* Variable temporaire représentant le nombre de caractère qui a été écrit */
	uint16_t position = position_curseur(); /* On lit la position actuelle du curseur */

	while (i-1<taille) { /* On incrémente i à chaque boucle */ 
		if((position+i)/80 > 24){ /* Si position + i représente une ligne qui sort de l'écran alors on fait défiler le texte de l'écran */
			defilement();
			position = position - 80;
		}
		
		place_curseur((position+i)/80, (position+i-1)%80); /* On place le curseur puis on écrit le caractère donné */
		traite_car(chaine[i-1]);
		i++;
	}
}

void affiche_droite(char* chaine) {
	int i = 0;	
	int taille = 0;

	uint16_t position = position_curseur(); /* On garde la position initiale du curseur */

	place_curseur(1, 80); /* On place le curseur tout en haut à droite de l'écran */
	while (chaine[i] != '\0') { /* On lit toute la chaine de caractères pour en connaitre sa taille */
		i++;
	}
	taille = i;

	for(i=taille; i>=0; i--) { /* On lit la chaîne de caractères de droite à gauche pour commencer à l'écrire sur le bord */
		traite_car(chaine[i]);
		place_curseur(1, 80 -(taille - i)-1);
	}

	place_curseur(position/80, position%80); /* On replace le curseur à sa position initiale */
}
