#include <stdio.h>
#include <stdlib.h>

#include "dynein_struct.h"

#define EPSILON 1e-12
int runtime = 0;

int equal(double v1, double v2) {
  if (fabs(v1) < EPSILON) { return fabs(v2) < EPSILON; }
  else return fabs(v1 - v2)/fabs(v1) <= EPSILON;
}

static int num_tests = 0;

int test(const char *msg, float one, float two) {

  num_tests += 1;
  if (equal(one, two)) {
    printf("%30s: pass, %g == %g.\n", msg, one, two);
    return 0;
  } else {
    printf("%30s: FAIL! %g != %g.\n", msg, one, two);
    return 1;
  }
}

int test_noteq(const char *msg, float one, float two) {

  num_tests += 1;
  if (!equal(one, two)) {
    printf("%30s: pass, %g != %g.\n", msg, one, two);
    return 0;
  } else {
    printf("%30s: FAIL! %g == %g.\n", msg, one, two);
    return 1;
  }
}

int main() {
  // Dynein in normal pentagon conformation, check if velocities agree with definitions.

  runtime = 1*runtime;

  forces no_forces = forces {0,0,0,0,0,
                             0,0,0,0,0};
  forces right_forces = forces {1,0,1,0,1,
                                0,1,0,1,0};
  forces up_forces = forces {0,1,0,1,0,1,
                             0,1,0,1};

  int num_failures = 0;
  {
    Dynein* dyn = new Dynein((108.0 / 180) * M_PI,
                             (36.0 / 180) * M_PI,
                             (144.0 / 180) * M_PI,
                             (72.0 / 180) * M_PI,
			     (State) LEFTBOUND,
			     (Mode) TEST_NO_INTERNAL_FORCES, &no_forces);

    printf("Test: Dynein pentagon conformation, no internal forces, no Brownian forces.\n");
    num_failures += test("Is d_blx zero", dyn->get_d_blx(), 0);
    num_failures += test("Is d_bly zero", dyn->get_d_bly(), 0);
    num_failures += test("Is d_mlx zero", dyn->get_d_mlx(), 0);
    num_failures += test("Is d_mly zero", dyn->get_d_mly(), 0);
    num_failures += test("Is d_mrx zero", dyn->get_d_mrx(), 0);
    num_failures += test("Is d_mry zero", dyn->get_d_mry(), 0);
    num_failures += test("Is d_brx zero", dyn->get_d_brx(), 0);
    num_failures += test("Is d_bry zero", dyn->get_d_bry(), 0);
    
    num_failures += test("Is d_bla zero", dyn->get_d_bla(), 0);
    num_failures += test("Is d_mla zero", dyn->get_d_mla(), 0);
    num_failures += test("Is d_mra zero", dyn->get_d_mra(), 0);
    num_failures += test("Is d_bra zero", dyn->get_d_bra(), 0);    
    
    free(dyn);
  }

  {
    Dynein* dyn = new Dynein((90.0 / 180) * M_PI,
                             (90.0 / 180) * M_PI,
                             -(90.0 / 180) * M_PI,
                             -(90.0 / 180) * M_PI,
			     (State) LEFTBOUND,
			     (Mode) TEST_NO_INTERNAL_FORCES, &no_forces);
    
    printf("\nTest: Dynein vertical conformation, no internal forces, no Brownian forces.\n");
    num_failures += test("Is brx zero", dyn->get_brx(), 0);
    num_failures += test("Is mrx zero", dyn->get_mrx(), 0);
    num_failures += test("Is tx zero", dyn->get_tx(), 0);
    num_failures += test("Is mlx zero", dyn->get_mlx(), 0);
    num_failures += test("Is blx zero", dyn->get_blx(), 0);
    
    num_failures += test("Is bry fully extended", dyn->get_bry(), 2*ls + 2*lt);
    num_failures += test("Is mry fully extended", dyn->get_mry(), 1*ls + 2*lt);
    num_failures += test("Is ty fully extended", dyn->get_ty(), 1*ls + 1*lt);
    num_failures += test("Is mly fully extended", dyn->get_mly(), 1*ls);
    num_failures += test("Is bly fully extended", dyn->get_bly(), 0);
    
    num_failures += test("Is d_blx zero", dyn->get_d_blx(), 0);
    num_failures += test("Is d_bly zero", dyn->get_d_bly(), 0);
    num_failures += test("Is d_mlx zero", dyn->get_d_mlx(), 0);
    num_failures += test("Is d_mly zero", dyn->get_d_mly(), 0);
    num_failures += test("Is d_mrx zero", dyn->get_d_mrx(), 0);
    num_failures += test("Is d_mry zero", dyn->get_d_mry(), 0);
    num_failures += test("Is d_brx zero", dyn->get_d_brx(), 0);
    num_failures += test("Is d_bry zero", dyn->get_d_bry(), 0);

    num_failures += test("Is d_bla zero", dyn->get_d_bla(), 0);
    num_failures += test("Is d_mla zero", dyn->get_d_mla(), 0);
    num_failures += test("Is d_mra zero", dyn->get_d_mra(), 0);
    num_failures += test("Is d_bra zero", dyn->get_d_bra(), 0);

    free(dyn);
  }

  {
    Dynein* dyn = new Dynein((0.0 / 180) * M_PI,
                             (0.0 / 180) * M_PI,
                             -(180.0 / 180) * M_PI,
                             -(180.0 / 180) * M_PI,
			     (State) LEFTBOUND,
			     (Mode) TEST_NO_INTERNAL_FORCES, &no_forces);
    
    printf("\nTest: Dynein horizontal conformation, no internal forces, no Brownian forces.\n");
    num_failures += test("Is bry zero", dyn->get_bry(), 0);
    num_failures += test("Is mry zero", dyn->get_mry(), 0);
    num_failures += test("Is ty zero", dyn->get_ty(), 0);
    num_failures += test("Is mly zero", dyn->get_mly(), 0);
    num_failures += test("Is bly zero", dyn->get_bly(), 0);

    num_failures += test("Is brx fully extended", dyn->get_brx(), 2*ls + 2*lt);
    num_failures += test("Is mrx fully extended", dyn->get_mrx(), 1*ls + 2*lt);
    num_failures += test("Is tx fully extended", dyn->get_tx(), 1*ls + 1*lt);
    num_failures += test("Is mlx fully extended", dyn->get_mlx(), 1*ls);
    num_failures += test("Is blx fully extended", dyn->get_blx(), 0);

    num_failures += test("Is d_blx zero", dyn->get_d_blx(), 0);
    num_failures += test("Is d_bly zero", dyn->get_d_bly(), 0);
    num_failures += test("Is d_mlx zero", dyn->get_d_mlx(), 0);
    num_failures += test("Is d_mly zero", dyn->get_d_mly(), 0);
    num_failures += test("Is d_mrx zero", dyn->get_d_mrx(), 0);
    num_failures += test("Is d_mry zero", dyn->get_d_mry(), 0);
    num_failures += test("Is d_brx zero", dyn->get_d_brx(), 0);

    num_failures += test("Is d_bla zero", dyn->get_d_bla(), 0);
    num_failures += test("Is d_mla zero", dyn->get_d_mla(), 0);
    num_failures += test("Is d_mra zero", dyn->get_d_mra(), 0);
    num_failures += test("Is d_bra zero", dyn->get_d_bra(), 0);
    
    free(dyn);
  }
  
  {
    Dynein* dyn = new Dynein(0.5*M_PI, 0.5*M_PI, 0.5*M_PI, 0.5*M_PI,
			     (State) LEFTBOUND,
			     (Mode) TEST_NO_INTERNAL_FORCES, &right_forces);

    printf("\nTest: Dynein vertical, no internal forces, Brownian forces in positive x direction.\n");

    num_failures += test("Is d_blx zero", dyn->get_d_blx(), 0);
    num_failures += test_noteq("Is d_mlx nonzero", dyn->get_d_mlx(), 0);
    num_failures += test_noteq("Is d_tx nonzero", dyn->get_d_tx(), 0);
    num_failures += test_noteq("Is d_mrx nonzero", dyn->get_d_mrx(), 0);
    num_failures += test_noteq("Is d_brx nonzero", dyn->get_d_brx(), 0);

    num_failures += test("Is d_bly zero", dyn->get_d_bly(), 0);
    num_failures += test("Is d_mly zero", dyn->get_d_mly(), 0);
    num_failures += test("Is d_ty zero", dyn->get_d_ty(), 0);
    num_failures += test("Is d_mry zero", dyn->get_d_mry(), 0);
    num_failures += test("Is d_bry zero", dyn->get_d_bry(), 0);

    free(dyn);
  }

  {
    Dynein* dyn = new Dynein(0.5*M_PI, 0.5*M_PI, 0.5*M_PI, 0.5*M_PI,
			     (State) LEFTBOUND,
			     (Mode) TEST_NO_INTERNAL_FORCES, &up_forces);

    printf("\nTest: Dynein vertical, no internal forces, Brownian forces in positive y direction.\n");

    num_failures += test("Is d_blx zero", dyn->get_d_blx(), 0);
    num_failures += test("Is d_mlx zero", dyn->get_d_mlx(), 0);
    num_failures += test("Is d_tx zero", dyn->get_d_tx(), 0);
    num_failures += test("Is d_mrx zero", dyn->get_d_mrx(), 0);
    num_failures += test("Is d_brx zero", dyn->get_d_brx(), 0);

    num_failures += test("Is d_bly zero", dyn->get_d_bly(), 0);
    num_failures += test("Is d_mly zero", dyn->get_d_mly(), 0);
    num_failures += test("Is d_ty zero", dyn->get_d_ty(), 0);
    num_failures += test("Is d_mry zero", dyn->get_d_mry(), 0);
    num_failures += test("Is d_bry zero", dyn->get_d_bry(), 0);

    free(dyn);
  }

  {
    Dynein* dyn = new Dynein(0, 0, 0, 0,
			     (State) LEFTBOUND,
			     (Mode) TEST_NO_INTERNAL_FORCES, &right_forces);

    printf("\nTest: Dynein horizontal, no internal forces, Brownian forces in positive x direction.\n");

    num_failures += test("Is d_blx zero", dyn->get_d_blx(), 0);
    num_failures += test("Is d_mlx zero", dyn->get_d_mlx(), 0);
    num_failures += test("Is d_tx zero", dyn->get_d_tx(), 0);
    num_failures += test("Is d_mrx zero", dyn->get_d_mrx(), 0);
    num_failures += test("Is d_brx zero", dyn->get_d_brx(), 0);

    num_failures += test("Is d_bly zero", dyn->get_d_bly(), 0);
    num_failures += test("Is d_mly zero", dyn->get_d_mly(), 0);
    num_failures += test("Is d_ty zero", dyn->get_d_ty(), 0);
    num_failures += test("Is d_mry zero", dyn->get_d_mry(), 0);
    num_failures += test("Is d_bry zero", dyn->get_d_bry(), 0);

    free(dyn);
  }


  {
    Dynein* dyn = new Dynein(0, 0, 0, 0,
			     (State) LEFTBOUND,
			     (Mode) TEST_NO_INTERNAL_FORCES, &up_forces);

    printf("\nTest: Dynein horizontal, no internal forces, Brownian forces in positive y direction.\n");

    num_failures += test("Is d_blx zero", dyn->get_d_blx(), 0);
    num_failures += test("Is d_mlx zero", dyn->get_d_mlx(), 0);
    num_failures += test("Is d_tx zero", dyn->get_d_tx(), 0);
    num_failures += test("Is d_mrx zero", dyn->get_d_mrx(), 0);
    num_failures += test("Is d_brx zero", dyn->get_d_brx(), 0);

    num_failures += test("Is d_bly zero", dyn->get_d_bly(), 0);
    num_failures += test_noteq("Is d_mly nonzero", dyn->get_d_mly(), 0);
    num_failures += test_noteq("Is d_ty nonzero", dyn->get_d_ty(), 0);
    num_failures += test_noteq("Is d_mry nonzero", dyn->get_d_mry(), 0);
    num_failures += test_noteq("Is d_bry nonzero", dyn->get_d_bry(), 0);

    free(dyn);
  }

  {
    Dynein* dyn = new Dynein((108.0 / 180) * M_PI,
                             (36.0 / 180) * M_PI,
                             (144.0 / 180) * M_PI,
                             (72.0 / 180) * M_PI,
			     (State) LEFTBOUND,
			     (Mode) TEST_NO_INTERNAL_FORCES, &right_forces);
    
    printf("\nTest: Dynein pentagon conformation, no internal forces, Brownian forces in positive x direction.\n");

    num_failures += test("Is d_blx zero", dyn->get_d_blx(), 0);
    num_failures += test_noteq("Is d_mlx nonzero", dyn->get_d_mlx(), 0);
    num_failures += test_noteq("Is d_mrx nonzero", dyn->get_d_mrx(), 0);
    num_failures += test_noteq("Is d_brx nonzero", dyn->get_d_brx(), 0);
    
    free(dyn);
  }
  
  {
    Dynein* dyn = new Dynein((108.0 / 180) * M_PI,
                             (36.0 / 180) * M_PI,
                             (144.0 / 180) * M_PI,
                             (72.0 / 180) * M_PI,
			     (State) LEFTBOUND,
			     (Mode) TEST_LEFT_INTERNAL_FORCES, &no_forces);
    
    printf("\nTest: Dynein pentagon conformation, internal forces in negative x direction, no Brownian forces.\n");

    num_failures += test("Is d_blx zero", dyn->get_d_blx(), 0);
    num_failures += test_noteq("Is d_mlx nonzero", dyn->get_d_mlx(), 0);
    num_failures += test_noteq("Is d_mrx nonzero", dyn->get_d_mrx(), 0);
    num_failures += test_noteq("Is d_brx nonzero", dyn->get_d_brx(), 0);
    
    free(dyn);
  }
  
  {
    Dynein* dyn = new Dynein((108.0 / 180) * M_PI,
                             (36.0 / 180) * M_PI,
                             (144.0 / 180) * M_PI,
                             (72.0 / 180) * M_PI,
  			     (State) LEFTBOUND,
  			     (Mode) PRE_POWERSTROKE, &no_forces);

    printf("Test: Dynein pentagon conformation, internal forces, no Brownian forces.\n");
    num_failures += test("Is d_blx zero", dyn->get_d_blx(), 0);
    num_failures += test("Is d_bly zero", dyn->get_d_bly(), 0);
    num_failures += test("Is d_mlx zero", dyn->get_d_mlx(), 0);
    num_failures += test("Is d_mly zero", dyn->get_d_mly(), 0);
    num_failures += test("Is d_mrx zero", dyn->get_d_mrx(), 0);
    num_failures += test("Is d_mry zero", dyn->get_d_mry(), 0);
    num_failures += test("Is d_brx zero", dyn->get_d_brx(), 0);
    num_failures += test("Is d_bry zero", dyn->get_d_bry(), 0);
    
    num_failures += test("Is d_bla zero", dyn->get_d_bla(), 0);
    num_failures += test("Is d_mla zero", dyn->get_d_mla(), 0);
    num_failures += test("Is d_mra zero", dyn->get_d_mra(), 0);
    num_failures += test("Is d_bra zero", dyn->get_d_bra(), 0);    
    
    free(dyn);
  }



  
  if (num_failures == 0) {
    printf("All %d tests pass!\n\n", num_tests);
  } else {
    printf("%d/%d tests fail!\n\n", num_failures, num_tests);
  }
  exit(num_failures);
}












