#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <limits>

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "default_parameters.h"
#include "dynein_struct.h"
#include "simulation_defaults.h"

const bool display_step_info = false;
const bool display_progress = false;

extern movie_data_struct* on_crash_old_movie_data_global_ptr;
extern movie_data_struct* on_crash_new_movie_data_global_ptr;
extern char* crash_movie_file_name_global;

const bool frozen_in_bothbound = false;

bool am_making_movie = true;
bool am_debugging_onebound = false;

long num_movie_writes = 1e7;
// bytes per movie write: 213, 2000Mb bytes max movie size

static const long MAX_FILESIZE_PERMITTED = 1<<30; // <- bit shift
static int NUM_STEPS = 0;

void on_crash_write_movie_buffer();

struct job_msg_t {
  long long max_iteration;
  double start_time;
  char *run_msg;
  FILE *stepping_data_file;
  FILE *movie_data_file;
};

void check_for_quitting_conditions(double time_run, FILE* data_file, double first_binding_time, double last_unbinding_time) {
  // P(stepping k times in t seconds) = ((t/0.06)^k*e^(t/0.06)) / k!
  if (time_run > 0.3 and NUM_STEPS == 0 and am_exiting_on_improbable_stepping) {
    printf("Zero steps in 0.3 seconds. There's a 0.67%% chance of real dynein doing this. Exiting successfully\n");
    fprintf(data_file, "#EXIT SUCCESSFULLY!\n");
    fprintf(data_file, "#First binding time: %g\n", first_binding_time);
    fprintf(data_file, "#Last unbinding time: %g\n", last_unbinding_time);
    fprintf(data_file, "#Runtime: %g\n", time_run);
    exit(0);
  }
  if (time_run > 0.3 and NUM_STEPS >= 10 and am_exiting_on_improbable_stepping) {
    printf("Over 10 steps in 0.3 seconds. There's less than a 1.4%% chance of real dynein doing this. Exiting successfully!\n");
    fprintf(data_file, "#EXIT SUCCESSFULLY!\n");
    fprintf(data_file, "#First binding time: %g\n", first_binding_time);
    fprintf(data_file, "#Last unbinding time: %g\n", last_unbinding_time);
    fprintf(data_file, "#Runtime: %g\n", time_run);
    exit(0);
  }
  // if (time_run > 0.001 and NUM_STEPS > 5 and am_exiting_on_improbable_stepping) {
  //   printf("Over 5 steps in 0.001 seconds. There's less than a 1e-11 chance of real dynein doing this. Exiting successfully!\n");
  //   fprintf(data_file, "#EXIT SUCCESSFULLY!\n");
  //   fprintf(data_file, "#First binding time: %g\n", first_binding_time);
  //   fprintf(data_file, "#Last unbinding time: %g\n", last_unbinding_time);
  //   fprintf(data_file, "#Runtime: %g\n", time_run);
  //   exit(0);
  // }
  else if (time_run > 0.7 and am_exiting_on_improbable_stepping) {
    // printf("There's a 97%% chance of real dynein having stepped 5 times in 0.7 seconds, exiting successfully.\n");
    printf("Exiting normally after 0.7 seconds.\nExit successfully!\n");
    fprintf(data_file, "#EXIT SUCCESSFULLY!");
    fprintf(data_file, "#First binding time: %g\n", first_binding_time);
    fprintf(data_file, "#Last unbinding time: %g\n", last_unbinding_time);
    fprintf(data_file, "#Runtime: %g\n", time_run);
    exit(0);
  }
}

void zero_movie_struct(movie_data_struct* data) {
  for (int i = 0; i < MOVIE_BUFFER_SIZE; i++) {
    data[i].time = -1.0;
  }
}

void log_stepping_data(FILE* data_file, void* dyn, long long iteration, long long max_iteration, State s) {
  static State  last_state = NEARBOUND;
  static double first_bothbound_iteration = 0;
  static double last_bothbound_iteration = 0;
  static double last_bothbound_nmx = 0;
  static double last_bothbound_fmx = 0;
  static bool am_in_initial_partial_step = true;

  if (s == BOTHBOUND) {
    Dynein_bothbound* dyn_bb = (Dynein_bothbound*) dyn;
    if ((last_state == NEARBOUND or last_state == FARBOUND) and !am_in_initial_partial_step) {
      fprintf(data_file, "%14.12g %14.12g %18.15g %18.15g %18.15g %18.15g %18.15g %18.15g\n", last_bothbound_iteration*dt, iteration*dt, dyn_bb->get_nbx(), dyn_bb->get_fbx(),
	      last_bothbound_nmx, last_bothbound_fmx, dyn_bb->get_nmx(), dyn_bb->get_fmx());
      fflush(data_file);
      NUM_STEPS++;
      if (display_step_info) printf("\nSwitched to BB at %.1f%%!\n", ((double)iteration)/max_iteration*100);
    }
    else if ((last_state == NEARBOUND or last_state == FARBOUND) and am_in_initial_partial_step) {
      am_in_initial_partial_step = false;
      first_bothbound_iteration = iteration; // track the first time the model binds
      if (display_step_info) printf("\nInitial switch to BB at %.1f%%!\n", ((double)iteration)/max_iteration*100);
    }

    last_bothbound_iteration = iteration;
    last_bothbound_nmx = dyn_bb->get_nmx();
    last_bothbound_fmx = dyn_bb->get_fmx();
    last_state = BOTHBOUND;
  }
  else if (s == NEARBOUND) {
    if (last_state != NEARBOUND) {
      if (display_step_info) printf("\nSwitched from BB to NB at %.1f%%!\n", ((double)iteration)/max_iteration*100);
    }
    last_state = NEARBOUND;
  }
  else if (s == FARBOUND) {
    if (last_state != FARBOUND) {
      if (display_step_info) printf("\nSwitched from BB to FB at %.1f%%!\n", ((double)iteration)/max_iteration*100);
    }
    last_state = FARBOUND;
  }
  else if (s == UNBOUND) {
    if (last_state != UNBOUND) {
      fprintf(data_file, "%.4e %.4e %.4e %.4e\n", last_bothbound_iteration*dt, iteration*dt, 0.0, 0.0);
      fflush(data_file);
      if (display_step_info) printf("\nSwitched to UB at %.1f%%!\n", ((double)iteration)/max_iteration*100);
    }
    last_state = UNBOUND;
  }

  check_for_quitting_conditions(iteration*dt, data_file, first_bothbound_iteration*dt, last_bothbound_iteration*dt);
}

void log_angle_data(FILE* data_file, void* dyn, long long iteration, long long max_iteration, State s, bool longmode) {
  static State last_state = BOTHBOUND;
  static bool am_in_initial_partial_step = true;
  static long long int last_onebound_iteration = 0;
  static long long int last_bothbound_iteration = 0;
  static State last_onebound_state = BOTHBOUND;
  static long long int iters_in_this_step = 0;
  static double last_sum_foot_positions = 0;
  static long long int maxlines = 1e9;

  long long int SKIPITERS = 100;

  long long int BBLOGDURATION;
  if (longmode) {
    BBLOGDURATION = 1e7;
    SKIPITERS = 1e4; // note - this gets further downsampled in make_all_stepping_plots.py
  }
  else
    BBLOGDURATION = 1e4;

  long long int BYTES_PER_LINE = 80;

  static char* print_buffer = (char*) malloc((2*BBLOGDURATION/SKIPITERS+2) * BYTES_PER_LINE * sizeof(char));

  if (iters_in_this_step > 2*BBLOGDURATION/SKIPITERS+1) {
    perror("Error, somehow print buffer exceeded.");
    exit(1);
  }

  if (s == BOTHBOUND) {
    Dynein_bothbound* dyn_bb = (Dynein_bothbound*) dyn;

    if ((last_state == NEARBOUND or last_state == FARBOUND)) {
      sprintf(&print_buffer[iters_in_this_step*BYTES_PER_LINE], "NEWBINDING\n");
      iters_in_this_step++;
      if (am_in_initial_partial_step) am_in_initial_partial_step = false;
    }

    if (last_onebound_state == NEARBOUND and iteration - last_onebound_iteration < BBLOGDURATION and (iteration - last_onebound_iteration) % SKIPITERS == 0) {
      sprintf(&print_buffer[iters_in_this_step*BYTES_PER_LINE], "%s\t%14.12f%8.3f\t%8.3f\t%8.6f\t%8.6f\n",
	      "BBFROMNB", iteration*dt, dyn_bb->get_tx() - dyn_bb->get_nbx(), dyn_bb->get_ty(), dyn_bb->get_fma(), dyn_bb->get_fba());
      iters_in_this_step++;
    }

    else if (last_onebound_state == FARBOUND and iteration - last_onebound_iteration < BBLOGDURATION and (iteration - last_onebound_iteration) % SKIPITERS == 0) {
      sprintf(&print_buffer[iters_in_this_step*BYTES_PER_LINE], "%s\t%14.12f%8.3f\t%8.3f\t%8.6f\t%8.6f\n",
	      "BBFROMFB", iteration*dt, dyn_bb->get_tx() - dyn_bb->get_fbx(), dyn_bb->get_ty(), dyn_bb->get_nma(), dyn_bb->get_nba());
      iters_in_this_step++;
    }

    last_state = BOTHBOUND;
    last_bothbound_iteration = iteration;
  }

  else if (s == NEARBOUND or s == FARBOUND) {
    Dynein_onebound* dyn_ob = (Dynein_onebound*) dyn;
    if (last_state == BOTHBOUND) {
      if (dyn_ob->get_bbx() + dyn_ob->get_ubx() - last_sum_foot_positions > 10) {
	for (int i = 0; i < iters_in_this_step; i++) {
	  if (--maxlines > 0) {
	    fprintf(data_file, "%s", &print_buffer[i*BYTES_PER_LINE]);
	  }
	}
	fflush(data_file);
      }
      iters_in_this_step = 0;
      sprintf(&print_buffer[iters_in_this_step*BYTES_PER_LINE], "NEWUNBINDING\n"); // where should this be?
      iters_in_this_step++;
      last_sum_foot_positions = dyn_ob->get_bbx() + dyn_ob->get_ubx();
    }
    if (s == NEARBOUND) {
      last_state = NEARBOUND;
      last_onebound_state = NEARBOUND;
      last_onebound_iteration = iteration;

      if (iteration - last_bothbound_iteration < BBLOGDURATION and (iteration - last_bothbound_iteration) % SKIPITERS == 0) {
	double uma = dyn_ob->get_uma() + M_PI - dyn_ob->get_uba();
	sprintf(&print_buffer[iters_in_this_step*BYTES_PER_LINE], "%s\t%14.12f%8.3f\t%8.3f\t%8.6f\t%8.6f\n",
		"NEARBOUN", iteration*dt, dyn_ob->get_tx() - dyn_ob->get_bbx(), dyn_ob->get_ty(), uma, 0.0);
	iters_in_this_step++;
      }
    }
    if (s == FARBOUND) {
      last_state = FARBOUND;
      last_onebound_state = FARBOUND;
      last_onebound_iteration = iteration;

      if (iteration - last_bothbound_iteration < BBLOGDURATION and (iteration - last_bothbound_iteration) % SKIPITERS == 0) {
	double uma = dyn_ob->get_uma() + M_PI - dyn_ob->get_uba();
	sprintf(&print_buffer[iters_in_this_step*BYTES_PER_LINE], "%s\t%14.12f%8.3f\t%8.3f\t%8.6f\t%8.6f\n",
		"FARBOUND", iteration*dt, dyn_ob->get_tx() - dyn_ob->get_bbx(), dyn_ob->get_ty(), uma, 0.0);
	iters_in_this_step++;
      }
    }
  }
}

// on unbinding, print the past ob and bb log if the past step was > 10nm
// on unbinding, compare the current feet positions to the past unbinding positions to see if the past ob period created a large displacement
// when entering BB, compare the stored

// bb, far stepped, from nearbound: "bb", tx - nbx, ty, fma, fba
// bb, near stepped, from farbound: "bb", tx - fbx, ty, nma, fba
// print "STEP" to begin a new bothbound stroke
// only print the first 1e4 timesteps = 1us of the bothbound stroke
// if necessary, downsample the printing
// can set kub higher once we figure out the interesting timescale of the recently-bound angle

void log_stepping_movie_data(FILE* data_file, void* dyn, State s, long long iteration) {
  static int buffer_position = 0;
  if (!am_only_writing_on_crash or (am_debugging_onebound and s != BOTHBOUND)) {
    if (--num_movie_writes > 0) {
      if (num_movie_writes == 1) printf("about to exceed movie printing line #\n");
      const char *format = "%d\t"
	"%.10g\t"
	"%.2g\t%.2g\t%.2g\t%.2g\t%.2g\t"
	"%g\t%g\t%g\t%g\t"
	"%g\t%g\t%g\t%g\t"
	"%g\t%g\t"
	"%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g\t%g"
	"\n";
      if (s == NEARBOUND or s == FARBOUND) {
	Dynein_onebound* dyn_ob = (Dynein_onebound*) dyn;
	onebound_forces dyn_ob_f = dyn_ob->get_internal();
	fprintf(data_file, format,
		dyn_ob->get_state(),
		iteration*dt,
		dyn_ob->PE_bba, dyn_ob->PE_bma, dyn_ob->PE_ta, dyn_ob->PE_uma, 0.0,
		dyn_ob->get_bbx(), dyn_ob->get_bby(), dyn_ob->get_bmx(), dyn_ob->get_bmy(),
		dyn_ob->get_tx(), dyn_ob->get_ty(), dyn_ob->get_umx(), dyn_ob->get_umy(),
		dyn_ob->get_ubx(), dyn_ob->get_uby(),
		dyn_ob_f.bbx, dyn_ob_f.bby, dyn_ob_f.bmx, dyn_ob_f.bmy, dyn_ob_f.tx,
		dyn_ob_f.ty, dyn_ob_f.umx, dyn_ob_f.umy, dyn_ob_f.ubx, dyn_ob_f.uby);
      }
      else if (s == BOTHBOUND) {
	Dynein_bothbound* dyn_bb = (Dynein_bothbound*) dyn;
	bothbound_forces dyn_bb_f = dyn_bb->get_internal();
	fprintf(data_file, format,
		BOTHBOUND,
		iteration*dt,
		dyn_bb->PE_nba, dyn_bb->PE_nma, dyn_bb->PE_ta, dyn_bb->PE_fma, dyn_bb->PE_fba,
		dyn_bb->get_nbx(), dyn_bb->get_nby(), dyn_bb->get_nmx(), dyn_bb->get_nmy(),
		dyn_bb->get_tx(), dyn_bb->get_ty(), dyn_bb->get_fmx(), dyn_bb->get_fmy(),
		dyn_bb->get_fbx(), dyn_bb->get_fby(),
		dyn_bb_f.nbx, dyn_bb_f.nby, dyn_bb_f.nmx, dyn_bb_f.nmy, dyn_bb_f.tx,
		dyn_bb_f.ty, dyn_bb_f.fmx, dyn_bb_f.fmy, dyn_bb_f.fbx, dyn_bb_f.fby);
      }
      else if (s != UNBOUND){
	printf("Unhandled state in stepping movie data generation!\n");
	exit(1);
      }
    }
  }
  else { // else write to on-crash-print buffer structs
    if (buffer_position == MOVIE_BUFFER_SIZE) {
      movie_data_struct* temp_ptr;
      temp_ptr = on_crash_old_movie_data_global_ptr;
      on_crash_old_movie_data_global_ptr = on_crash_new_movie_data_global_ptr;
      on_crash_new_movie_data_global_ptr = temp_ptr;
      zero_movie_struct(on_crash_new_movie_data_global_ptr);
      buffer_position = 0;
    }

    movie_data_struct* new_movie_buffer = on_crash_new_movie_data_global_ptr;

    if (s == NEARBOUND or s == FARBOUND) {
      Dynein_onebound* dyn_ob = (Dynein_onebound*) dyn;
      onebound_forces dyn_ob_f = dyn_ob->get_internal();
      new_movie_buffer[buffer_position].state = dyn_ob->get_state();
      new_movie_buffer[buffer_position].time = iteration*dt;
      new_movie_buffer[buffer_position].PE_1 = dyn_ob->PE_bba;
      new_movie_buffer[buffer_position].PE_2 = dyn_ob->PE_bma;
      new_movie_buffer[buffer_position].PE_3 = dyn_ob->PE_uma;
      new_movie_buffer[buffer_position].PE_4 = 0.0;

      new_movie_buffer[buffer_position].x_1 = dyn_ob->get_bbx();
      new_movie_buffer[buffer_position].x_2 = dyn_ob->get_bmx();
      new_movie_buffer[buffer_position].x_3 = dyn_ob->get_tx();
      new_movie_buffer[buffer_position].x_4 = dyn_ob->get_umx();
      new_movie_buffer[buffer_position].x_5 = dyn_ob->get_ubx();
      new_movie_buffer[buffer_position].y_1 = dyn_ob->get_bby();
      new_movie_buffer[buffer_position].y_2 = dyn_ob->get_bmy();
      new_movie_buffer[buffer_position].y_3 = dyn_ob->get_ty();
      new_movie_buffer[buffer_position].y_4 = dyn_ob->get_umy();
      new_movie_buffer[buffer_position].y_5 = dyn_ob->get_uby();

      new_movie_buffer[buffer_position].fx_1 = dyn_ob_f.bbx;
      new_movie_buffer[buffer_position].fx_2 = dyn_ob_f.bmx;
      new_movie_buffer[buffer_position].fx_3 = dyn_ob_f.tx;
      new_movie_buffer[buffer_position].fx_4 = dyn_ob_f.umx;
      new_movie_buffer[buffer_position].fx_5 = dyn_ob_f.ubx;
      new_movie_buffer[buffer_position].fy_1 = dyn_ob_f.bby;
      new_movie_buffer[buffer_position].fy_2 = dyn_ob_f.bmy;
      new_movie_buffer[buffer_position].fy_3 = dyn_ob_f.ty;
      new_movie_buffer[buffer_position].fy_4 = dyn_ob_f.umy;
      new_movie_buffer[buffer_position].fy_5 = dyn_ob_f.uby;
    }
    else if (s == BOTHBOUND) {
      Dynein_bothbound* dyn_bb = (Dynein_bothbound*) dyn;
      bothbound_forces dyn_bb_f = dyn_bb->get_internal();
      new_movie_buffer[buffer_position].state = BOTHBOUND;
      new_movie_buffer[buffer_position].time = iteration*dt;
      new_movie_buffer[buffer_position].PE_1 = dyn_bb->PE_nba;
      new_movie_buffer[buffer_position].PE_2 = dyn_bb->PE_nma;
      new_movie_buffer[buffer_position].PE_3 = dyn_bb->PE_fma;
      new_movie_buffer[buffer_position].PE_4 = dyn_bb->PE_fba;

      new_movie_buffer[buffer_position].x_1 = dyn_bb->get_nbx();
      new_movie_buffer[buffer_position].x_2 = dyn_bb->get_nmx();
      new_movie_buffer[buffer_position].x_3 = dyn_bb->get_tx();
      new_movie_buffer[buffer_position].x_4 = dyn_bb->get_fmx();
      new_movie_buffer[buffer_position].x_5 = dyn_bb->get_fbx();
      new_movie_buffer[buffer_position].y_1 = dyn_bb->get_nby();
      new_movie_buffer[buffer_position].y_2 = dyn_bb->get_nmy();
      new_movie_buffer[buffer_position].y_3 = dyn_bb->get_ty();
      new_movie_buffer[buffer_position].y_4 = dyn_bb->get_fmy();
      new_movie_buffer[buffer_position].y_5 = dyn_bb->get_fby();

      new_movie_buffer[buffer_position].fx_1 = dyn_bb_f.nbx;
      new_movie_buffer[buffer_position].fx_2 = dyn_bb_f.nmx;
      new_movie_buffer[buffer_position].fx_3 = dyn_bb_f.tx;
      new_movie_buffer[buffer_position].fx_4 = dyn_bb_f.fmx;
      new_movie_buffer[buffer_position].fx_5 = dyn_bb_f.fbx;
      new_movie_buffer[buffer_position].fy_1 = dyn_bb_f.nby;
      new_movie_buffer[buffer_position].fy_2 = dyn_bb_f.nmy;
      new_movie_buffer[buffer_position].fy_3 = dyn_bb_f.ty;
      new_movie_buffer[buffer_position].fy_4 = dyn_bb_f.fmy;
      new_movie_buffer[buffer_position].fy_5 = dyn_bb_f.fby;
    }
    buffer_position++;
  }
}

void stepping_data_callback(void* dyn, State s, void *job_msg_, data_union *job_data, long long iteration) {
  job_msg_t job_msg = *(job_msg_t *)job_msg_;

  if (iteration % 1000 == 0) {
    if (ftell(stdout) > MAX_FILESIZE_PERMITTED) {
      printf("ERROR: stdout has gotten too big.  Exiting!\n");
      exit(1);
    }
    if (job_msg.movie_data_file && ftell(job_msg.movie_data_file) > MAX_FILESIZE_PERMITTED) {
      printf("ERROR: movie_data_file has gotten too big.  Exiting!\n");
      exit(1);
    }
  }

  if (angle_logging_mode) {
    log_angle_data(job_msg.stepping_data_file, dyn, iteration, job_msg.max_iteration, s, false);
  } else if (long_angle_logging_mode) {
    log_angle_data(job_msg.stepping_data_file, dyn, iteration, job_msg.max_iteration, s, true);
  } else {
    log_stepping_data(job_msg.stepping_data_file, dyn, iteration, job_msg.max_iteration, s);
  }

  if ((am_making_movie or am_debugging_onebound) && iteration % int(stepping_movie_framerate/dt) == 0)
    log_stepping_movie_data(job_msg.movie_data_file, dyn, s, iteration);

  if (iteration % (long long) (0.01 / dt) == 0) { // print time every tenth of a second.
    fprintf(job_msg.stepping_data_file, "#[time: %g]\n", iteration*dt);
    printf("#[time: %g]\n", iteration*dt);
  }

  if (job_msg.max_iteration > 0) {
    if (iteration % (int)5e5 == 0 && iteration != job_msg.max_iteration && display_progress) {
      printf("Stepping data progress (%s): %lld / %lld, %g%%\n", job_msg.run_msg,
             iteration, job_msg.max_iteration, ((double) iteration) / job_msg.max_iteration * 100);
    }

    if (iteration == job_msg.max_iteration and display_progress) {
      printf("Finished generating stepping data (%s), process took %g seconds\n", job_msg.run_msg,
             ((double) clock() - job_msg.start_time) / CLOCKS_PER_SEC);
    }
  }
  else if (job_msg.max_iteration == 0 and iteration % (int)5e5 == 0) {
    //printf("Stepping data progress (%s): %.2g seconds\n", run_msg, iteration*dt);
    fflush(job_msg.stepping_data_file);
  }
}

void set_input_variables(int argc, char** argv, char* run_name, bool* am_making_movie, double* runtime) {
  char c;
  char* label;
  *run_name = 0;
  label = 0;

  static struct option long_options[] =
    {
      {"ls",       required_argument,    0, 'a'},
      {"lt",       required_argument,    0, 'b'},
      {"cb",       required_argument,    0, 'c'},
      {"cm",       required_argument,    0, 'd'},
      {"ct",       required_argument,    0, 'e'},
      {"T",        required_argument,    0, 'f'},
      {"name",     required_argument,    0, 'g'},
      {"seed",     required_argument,    0, 'h'},
      {"k_b",      required_argument,    0, 'i'},
      {"k_ub",     required_argument,    0, 'j'},
      {"k_ub_ob",  required_argument,    0, 'k'},
      {"runtime",  required_argument,    0, 'l'},
      {"label",    required_argument,    0, 'm'},
      {"dt",       required_argument,    0, 'n'},
      {"framerate",required_argument,    0, 'o'},
      {"eqb",      required_argument,    0, 'p'},
      {"eqmpre",   required_argument,    0, 'q'},
      {"eqmpost",  required_argument,    0, 'r'},
      {"eqt",      required_argument,    0, 's'},
      {"exp-unbinding-constant",      required_argument,    0, 't'},
      {"rt",       required_argument,    0, 'u'},
      {"rm",       required_argument,    0, 'v'},
      {"rb",       required_argument,    0, 'w'},
      {"force",    required_argument,    0, 'x'},
      {"nomovie",  no_argument, (int*) am_making_movie, 0},
      {"crash-movie", no_argument, (int*) &am_only_writing_on_crash, true},
      {"onebound-debugging", no_argument, (int*) &am_debugging_onebound, true},
      {"full-gibbs-transitions", no_argument, (int*) &binding_mode, GIBBS_FULL},
      {"exp-binding", no_argument, (int*) &binding_mode, EXPONENTIAL_UNBINDING},
      {"angle-logging-mode", no_argument, (int*) &angle_logging_mode, true},
      {"long-angle-logging-mode", no_argument, (int*) &long_angle_logging_mode, true},
      {0, 0, 0, 0}
    };

  int option_index = 0;

  while ((c = getopt_long(argc, argv, "a:b:c:d:e:f:", long_options, &option_index)) != -1) {
    switch (c) {
    case 0:
      if (long_options[option_index].flag != 0) // option set a flag
	break;
      else {
	printf ("unknown option %s", long_options[option_index].name);
	if (optarg)
	  printf (" with arg %s", optarg);
	printf ("\n");
	break;
      }
    case 'a':
      Ls = strtod(optarg, NULL);
      break;
    case 'b':
      Lt = strtod(optarg, NULL);
      break;
    case 'c':
      cb = strtod(optarg, NULL);
      break;
    case 'd':
      cm = strtod(optarg, NULL);
      break;
    case 'e':
      ct = strtod(optarg, NULL);
      break;
    case 'f':
      T = strtod(optarg, NULL);
      break;
    case 'g':
      strcpy(run_name, optarg);
      break;
    case 'h':
      RAND_INIT_SEED = atoi(optarg);
      break;
    case 'i':
      low_affinity_binding_rate = strtod(optarg, NULL);
      break;
    case 'j':
      low_affinity_unbinding_rate = strtod(optarg, NULL);
      break;
    // case 'k':
    //   high_affinity_unbinding_rate = strtod(optarg, NULL);
    //   break;
    case 'l':
      *runtime = strtod(optarg, NULL);
      break;
    case 'm':
      label = optarg;
      break;
    case 'n':
      dt = strtod(optarg, NULL);
      break;
    case 'o':
      stepping_movie_framerate = strtod(optarg, NULL);
      break;
    case 'p':
      onebound_post_powerstroke_internal_angles.bba = strtod(optarg, NULL) * M_PI / 180.0;
      bothbound_pre_powerstroke_internal_angles.nba = strtod(optarg, NULL) * M_PI / 180.0;
      bothbound_pre_powerstroke_internal_angles.fba = strtod(optarg, NULL) * M_PI / 180.0;
      break;
    case 'q': // pre-powerstroke
      onebound_post_powerstroke_internal_angles.uma = strtod(optarg, NULL) * M_PI / 180.0;
      break;
    case 'r': // post-powerstroke
      onebound_post_powerstroke_internal_angles.bma = strtod(optarg, NULL) * M_PI / 180.0;
      bothbound_pre_powerstroke_internal_angles.nma = strtod(optarg, NULL) * M_PI / 180.0;
      bothbound_pre_powerstroke_internal_angles.fma = strtod(optarg, NULL) * M_PI / 180.0;
      break;
    case 's':
      onebound_post_powerstroke_internal_angles.ta = strtod(optarg, NULL) * M_PI / 180.0;
      bothbound_pre_powerstroke_internal_angles.ta = strtod(optarg, NULL) * M_PI / 180.0;
      break;
    case 't':
      exponential_unbinding_angle_constant = strtod(optarg, NULL);
      printf("unbinding exponential constant: %g\n", exponential_unbinding_angle_constant);
      break;
    case 'u':
      fake_radius_t = strtod(optarg, NULL);
      gt = fake_radius_t*6*M_PI*water_viscosity_mu; // kg / s
      break;
    case 'v':
      fake_radius_m = strtod(optarg, NULL);
      gm = fake_radius_m*6*M_PI*water_viscosity_mu; // kg / s
      break;
    case 'w':
      fake_radius_b = strtod(optarg, NULL);
      gb = fake_radius_b*6*M_PI*water_viscosity_mu; // kg / s
      break;
    case 'x':
      tail_force = strtod(optarg, NULL) * 0.6022 / atp_in_kJ_per_mol; // conversion for our force units: 1 (dG ATP kJ / mol / nm) = atp_in_kJ_per_mol * 1e-11 / 6.022 N
      break;
    case '?':
      printf("Some other unknown getopt error.\n");
      exit(EXIT_FAILURE);
    default:
      printf("Default case in getopt: uh-oh!\n");
      exit(EXIT_FAILURE);
    }
  }

  if (*run_name == 0) {
    if (label == 0) {
      sprintf(run_name, "k_b-%g,k_ub-%g,c-%g,cb-%g,cm-%g,ct-%g,dt-%g",
	      low_affinity_binding_rate, low_affinity_unbinding_rate, exponential_unbinding_angle_constant,
	      cb, cm, ct, dt);
    }
    else {
      sprintf(run_name, "%s__k_b-%g,k_ub-%g,c-%g,cb-%g,cm-%g,ct-%g,ls-%g,lt-%g,seed-%g,dt-%g", label,
	      low_affinity_binding_rate, low_affinity_unbinding_rate, exponential_unbinding_angle_constant,
	      cb, cm, ct, Ls, Lt, RAND_INIT_SEED, dt);
    }
  }

  if (optind != argc) {
    printf("Improper usage. Example: ./generate_stepping_data --label test --Ls 34.5 --T 55 --constant-write\n");
    exit(EXIT_FAILURE);
  }
}

void sig_handler_print_movie_buffer(int signum) {
  if (am_making_movie) {
    printf("Received sigusr1, writing to data file and turning off printing.\n");
    on_crash_write_movie_buffer();
    am_making_movie = false;
    am_only_writing_on_crash = false;
  }
  else {
    printf("Turning on printing mode.\n");
    am_making_movie = true;
    am_only_writing_on_crash = true;
  }
}

int main(int argc, char** argv) {
  setvbuf(stdout, 0, _IONBF, 0);

  char* run_name = new char[200];
  am_making_movie = true;

  crash_movie_file_name_global = new char[1000];

  double runtime = 0;

  if (am_debugging_onebound) {
    printf("turning off am_only_writing_on_crash for onebound-debugging mode\n");
    am_only_writing_on_crash = false;
  }

  set_input_variables(argc, argv, run_name, &am_making_movie, &runtime);

  if (runtime == 0 and am_making_movie and not am_only_writing_on_crash) {
    printf("error,value of am_only_writing: %d\n", (int)am_only_writing_on_crash);
    printf("Error: run settings would cause indefinite movie data printing and fill up the disk!\n");
    exit(EXIT_FAILURE);
  }

  if (am_only_writing_on_crash) {
    struct sigaction new_action;

    new_action.sa_handler = sig_handler_print_movie_buffer;
    sigemptyset (&new_action.sa_mask);
    new_action.sa_flags = 0;

    sigaction(SIGUSR1, &new_action, NULL);
  }

  char *stepping_data_fname = new char[200];
  char *stepping_config_fname = new char[200];

  char *movie_data_fname = new char[200];
  char *movie_config_fname = new char[200];

  sprintf(stepping_data_fname, "data/stepping_data_%s.txt", run_name);
  sprintf(stepping_config_fname, "data/stepping_config_%s.txt", run_name);
  sprintf(movie_data_fname, "data/stepping_movie_data_%s.txt", run_name);
  sprintf(movie_config_fname, "data/stepping_movie_config_%s.txt", run_name);

  //technically only need this if am_only_writing_on_crash is on, but do it just in case we turn it on later
  on_crash_old_movie_data_global_ptr = new movie_data_struct[MOVIE_BUFFER_SIZE];
  on_crash_new_movie_data_global_ptr = new movie_data_struct[MOVIE_BUFFER_SIZE];
  zero_movie_struct(on_crash_old_movie_data_global_ptr);
  sprintf(crash_movie_file_name_global, "data/stepping_movie_data_%s.txt", run_name);
  write_movie_config(movie_config_fname, iterations*dt);

  write_config_file(stepping_config_fname, 0, "");

  printf("fake_radius_t: %g\n", fake_radius_t);
  printf("gt should be : %g\n", fake_radius_t*6*M_PI*water_viscosity_mu);
  printf("gt: %g\n\n", gt);
  printf("fake_radius_m: %g\n", fake_radius_m);
  printf("gm should be : %g\n", fake_radius_m*6*M_PI*water_viscosity_mu);
  printf("gm: %g\n\n", gm);
  printf("fake_radius_b: %g\n", fake_radius_b);
  printf("gb should be : %g\n", fake_radius_b*6*M_PI*water_viscosity_mu);
  printf("gb: %g\n\n", gb);
  job_msg_t job_msg;
  job_msg.max_iteration = 0;
  job_msg.start_time = clock();
  job_msg.run_msg = run_name;
  job_msg.stepping_data_file = fopen(stepping_data_fname, "w");
  job_msg.movie_data_file = 0;

  if (am_making_movie or am_debugging_onebound) {
    job_msg.movie_data_file = fopen(movie_data_fname, "w");
    if (!job_msg.movie_data_file) {
      printf("Error opening %s!\n", movie_data_fname);
      exit(1);
    }
    setvbuf(job_msg.movie_data_file, NULL, _IOLBF, 0); // turn on line-buffering
    fprintf(job_msg.movie_data_file, "#State\ttime\tPE_b1\tPE_m1\tPE_t\tPE_m2\tPE_b2\t"
            "x1\ty1\tx2\ty2\tx3\ty3\tx4\ty4\tx5\ty5\t"
            "fx1\tfy1\tfx2\tfy2\tfx3\tfy3\tfx4\tfy4\tfx5\tfy5\n");
  }

  fprintf(job_msg.stepping_data_file, "# command line:");
  for (int i=0; i<argc; i++) {
    fprintf(job_msg.stepping_data_file, " %s", argv[i]);
  }

  printf("\n\n\n*********%s*********\n", run_name);
  fprintf(job_msg.stepping_data_file, "\n\n\n\n#********%s********\n", run_name);
  if (angle_logging_mode) fprintf(job_msg.stepping_data_file, "#state, time, tx displacement, ty displacement, recently bound motor angle, recently bound binding angle\n");
  else fprintf(job_msg.stepping_data_file, "#time_unbind, time_bind, nbx_bind, fbx_bind, nmx_unbind, fmx_unbind, nmx_bind, fmx_bind\n");
  if (errno) {
    perror("Error opening stepping data or movie file.\n");
    exit(errno);
  }

  onebound_equilibrium_angles eq = onebound_post_powerstroke_internal_angles;
  double init_position[] = {eq.bba,
			    eq.bma - M_PI + eq.bba,
			    eq.ta + eq.bma - M_PI + eq.bba + 0.00001,
			    eq.ta + eq.bma + eq.bba - eq.uma,
			    0.0, 0.0};

  printf("Initial conditions: %g %g %g %g\n", init_position[0], init_position[1], init_position[2], init_position[3]);

  simulate(runtime, RAND_INIT_SEED, NEARBOUND, init_position, stepping_data_callback, &job_msg, NULL);

  fclose(job_msg.stepping_data_file);
  if (job_msg.movie_data_file) fclose(job_msg.movie_data_file);

  delete[] on_crash_old_movie_data_global_ptr;
  delete[] on_crash_new_movie_data_global_ptr;

  return EXIT_SUCCESS;
}
