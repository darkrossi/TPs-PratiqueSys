#include <cpu.h>
#include <inttypes.h>
#include "string.h"
#include "stdbool.h"

#define CLOCKFREQ 50
#define ELU 1
#define ACTIVABLE 2
#define ENDORMI 3
#define MORT 4
#define TAILLE_PILE_EXECUTION 512
#define TAILLE_TABLE_PROCESSUS 4

typedef struct processus {
	int pid;
	char nom[25];
	int etat;
	int zone_save[5];
	int pile[TAILLE_PILE_EXECUTION];
	uint32_t wake_up_time;
} processus;

typedef processus table_processus[TAILLE_TABLE_PROCESSUS];

table_processus tab_processus;
int indice_dernier_elt_tab_processus;
processus processus_elu;

processus newProcessus(int pid, char nom[25], int etat);

void traitant_IT_32(void);

void ctx_sw(int[], int[]);

uint16_t* ptr_mem(uint32_t lig, uint32_t col);

void ecrit_car(uint32_t lig, uint32_t col, char c, uint32_t cl, uint32_t cf, uint32_t ct);

void place_curseur(uint32_t lig, uint32_t col);

uint16_t position_curseur();

void efface_ecran(void);

void traite_car(char c);

void defilement(void);

void console_putbytes(char *chaine, int32_t taille);

void affiche_droite(char* chaine);

void tic_PIT(void);

uint32_t nbr_secondes();

void init_traitant_IT(uint32_t num_IT, void (*traitant)(void));

void set_clock(void);

void masque_IRQ(uint32_t num_IRQ, bool masque);

processus newProcessus(int pid, char nom[25], int etat);

void idle(void);

void proc1(void);

void proc2(void);

void proc3(void);

void ordonnance();

int32_t mon_pid(void);

char* mon_nom(void);

int32_t cree_processus(void (*code)(void), char *nom);

void dors(uint32_t nbr_secs);

void fin_processus(void);
