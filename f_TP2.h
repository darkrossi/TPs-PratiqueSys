#ifndef _F_TP2_H
#define _F_TP2_H

void traitant_IT_32(void);

void tic_PIT(void);

void init_traitant_IT(uint32_t num_IT, void (*traitant)(void));

void set_clock(void);

void masque_IRQ(uint32_t num_IRQ, bool masque);

#endif				/* _F_TP2_H */
