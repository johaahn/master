/***********************************************************************
 ** randn.h
 ***********************************************************************
 ** Copyright (c) SEAGNAL SAS
 **
 **
 ***********************************************************************/
#include "randn.h"

/* Génération d'un bruit gaussien (moyenne 0, écart-type 1) */
void f_get_randn2(float *pf_out1, float *pf_out2) {

	/* Variables aléatoire uniformément réparties dans ]0;1] */
	float f_u1 = (((float)rand())+1) / (((float)(RAND_MAX))+1);
	float f_u2 = (((float)rand())+1) / (((float)(RAND_MAX))+1);

	float f_R = sqrt(-2*log(f_u1));
	float f_t = 2.0*M_PI*f_u2;

	*pf_out1 = f_R * cos(f_t);
	*pf_out2 = f_R * sin(f_t);

}

void f_randn_seed(struct ST_RANDN * in_ps_this, long int in_i_seed) {
  srand48_r (in_i_seed, &(in_ps_this->s_drand_buf));
}

/* Génération d'un bruit gaussien (moyenne 0, écart-type 1) */
float f_randn(struct ST_RANDN * in_ps_this) {
	double f_u1, f_u2;

	/* Variables aléatoire uniformément réparties dans ]0;1] */
	drand48_r (&(in_ps_this->s_drand_buf), &f_u1);
	drand48_r (&(in_ps_this->s_drand_buf), &f_u2);
	//float f_u1 = (((float)x)+1) / (((float)(RAND_MAX))+1);
	//float f_u2 = (((float)y)+1) / (((float)(RAND_MAX))+1);

	float f_R = sqrt(-2*log(f_u1));
	float f_t = 2.0*M_PI*f_u2;

	return f_R * cos(f_t);

}


/* Génération d'une variable aléatoire suivant loi uniforme (entre 0 et 1) */
float f_rand(struct ST_RANDN * in_ps_this) {

	double f_u;
	drand48_r (&(in_ps_this->s_drand_buf), &f_u);

	return f_u;

}
