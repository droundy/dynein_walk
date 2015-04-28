#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "dynein_struct.h"

//Display output of generated dynein_motion_functions.cpp file for various conditions
//For use in testing accuracy of replace.py translation

     double fblx = 1;	 double fbly = 1;
	 double fmlx = 1;	 double fmly = 1;
	 double ftx = 1;	 double fty = 1;
	 double fmrx = 1;	 double fmry = 1;
	 double fbrx = 1;	 double fbry = 1;

Dynein* initProtein(Dynein* dyn, double bla_value, double mla_value, double mra_value, double bra_value) {
	dyn->set_blx(0);
	dyn->set_bly(0);
	
	dyn->set_d_bla(0);
	dyn->set_d_mla(0);
	dyn->set_d_mra(0);
	dyn->set_d_bra(0);
	
	dyn->set_bla(bla_value);
	dyn->set_mla(mla_value);
	dyn->set_mra(mra_value);
	dyn->set_bra(bra_value);
}

int main() {
	
	const double lt = 10.0;
	const double ls = 10.0;

	const double kt  = 1.0;
	const double kml = 1.0;
	const double kmr = 1.0;
	const double kbl = 1.0;
	const double kbr = 1.0;
		
	const double mb = 1.0;
	const double mm = 1.0;
	const double mt = 1.0;

	const double ba = (108.0 / 180) * M_PI;
	const double ma = (108.0 / 180) * M_PI;
	const double ta = (108.0 / 180) * M_PI;

	const double bla_value = (108.0 / 180) * M_PI;
	const double mla_value = (36.0 / 180) * M_PI;
	const double mra_value = (36.0 / 180) * M_PI;
	const double bra_value = (100.0 / 180) * M_PI;
	
	Dynein* dyn = (Dynein*) malloc(sizeof(Dynein));
	initProtein(dyn, bla_value, mla_value, mra_value, bra_value);
	
	dyn->set_state(LEFTBOUND);
	printf("\n\nC Leftbound Accelerations:\n\n");
	printf("dd_bla: %E\n", dyn->get_dd_bla());
	printf("dd_mla: %E\n", dyn->get_dd_mla());
	printf("dd_mra: %E\n", dyn->get_dd_mra());
	printf("dd_bra: %E\n", dyn->get_dd_bra());
	
	dyn->set_state(RIGHTBOUND);
	printf("\n\nC Rightbound Accelerations:\n\n");
	printf("dd_bla: %E\n", dyn->get_dd_bla());
	printf("dd_mla: %E\n", dyn->get_dd_mla());
	printf("dd_mra: %E\n", dyn->get_dd_mra());
	printf("dd_bra: %E\n", dyn->get_dd_bra());
	
	free(dyn);
	dyn = NULL;
	
}


