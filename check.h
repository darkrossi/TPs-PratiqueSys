#ifndef CHECK_H
#define CHECK_H

#include <inttypes.h>

/* Teste la valeur de retour de ptr_mem(15, 15). */
void check_ptr_mem(uint16_t *(*ptr_mem)(uint32_t, uint32_t));

/* Appelle efface_ecran() et vérifie que l'écran est bien rempli
 * d'espaces (donc vide). */
void check_empty_screen(void (*empty_screen)(void));

/* Appelle place_curseur(15, 15) et vérifie la position du curseur. */
void check_set_cursor(void (*set_cursor)(uint32_t, uint32_t));

/* Appelle ecrit_car(15, 15, '0', 1, 1, 1) et vérifie la valeur du mot
 * de 16 bits correspondant. */
void check_write_char(void (*write_char)(uint32_t, uint32_t, char, uint32_t, uint32_t, uint32_t));

/* Sauvegarde le contenu de l'écran, appelle defilement() puis vérifie
 * si le nouveau contenu de l'écran correspond à l'écran sauvegardé
 * décalé d'une ligne vers le haut. */
void check_scrolling(void (*scroll_down)(void));

/* Appelle init_traitant_IT(num_IT, traitant) et vérifie la valeur de
 * l'entrée de l'IDT correspondante. */
void check_idt_entry(void (*init_traitant_IT)(uint32_t, void (*traitant)(void)), uint32_t num_IT, void (*traitant)(void));

/* Appelle masque_IRQ(0, false) et vérifie la valeur du masque des
 * IRQs. */
void check_IRQ(void (*masque_IRQ)(uint32_t, bool));

#endif /* CHECK_H */
