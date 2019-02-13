.PHONY: all clean
all: papers/paper/figures/bothbound-cartoon-simple.pdf papers/paper/figures/bothbound-cartoon.pdf papers/paper/figures/onebound-cartoon-simple.pdf papers/paper/figures/onebound-cartoon.pdf papers/paper/figures/schematic-1-model.pdf papers/paper/figures/schematic-1-superimposed.pdf papers/paper/figures/schematic-2-model.pdf papers/paper/figures/schematic-2-superimposed.pdf papers/paper/figures/schematic-binding-angle.pdf papers/paper/figures/schematic-full.pdf papers/paper/figures/schematic-poststroke.pdf papers/paper/figures/schematic-prestroke.pdf papers/paper/figures/svg.fac papers/paper/paper.aux papers/waczak-thesis/modelExplanation/model_explanation.aux papers/waczak-thesis/thesis/thesis.aux src/.fac src/test-rng.o src/test_bothbound.o src/test_onebound.o

clean:
	rm -f "build/dynein_simulate.o" "build/dynein_struct_bothbound.o" "build/dynein_struct_onebound.o" "build/generate_stepping_data.o" "build/utilities.o" "data/paper_params.tex" "data/paper_trajectory_movie_data.txt" "data/paper_trajectory_stepping_data.txt" "data/paper_trajectory_stepping_parameters.tex" "data/paper_unbinding_probability__L-10,s-1.txt" "data/paper_unbinding_probability__L-20,s-1.txt" "data/paper_unbinding_probability__L-30,s-1.txt" "data/paper_unbinding_probability__L-40,s-1.txt" "data/paper_unbinding_probability__L-50,s-1.txt" "generate_stepping_data" "papers/paper/figures/bothbound-cartoon-simple.pdf" "papers/paper/figures/bothbound-cartoon.pdf" "papers/paper/figures/model-cartoon-simple.pdf" "papers/paper/figures/onebound-cartoon-simple.pdf" "papers/paper/figures/onebound-cartoon.pdf" "papers/paper/figures/schematic-1-model.pdf" "papers/paper/figures/schematic-1-superimposed.pdf" "papers/paper/figures/schematic-2-model.pdf" "papers/paper/figures/schematic-2-superimposed.pdf" "papers/paper/figures/schematic-binding-angle.pdf" "papers/paper/figures/schematic-full.pdf" "papers/paper/figures/schematic-poststroke.pdf" "papers/paper/figures/schematic-prestroke.pdf" "papers/paper/figures/svg.fac" "papers/paper/figures/winch-powerstroke-cartoon.pdf" "papers/paper/paper.aux" "papers/paper/paper.bbl" "papers/paper/paper.blg" "papers/paper/paper.log" "papers/paper/paper.out" "papers/paper/paper.pdf" "papers/waczak-thesis/modelExplanation/model_explanation.aux" "papers/waczak-thesis/modelExplanation/model_explanation.log" "papers/waczak-thesis/modelExplanation/model_explanation.pdf" "papers/waczak-thesis/thesis/thesis.aux" "papers/waczak-thesis/thesis/thesis.bbl" "papers/waczak-thesis/thesis/thesis.blg" "papers/waczak-thesis/thesis/thesis.log" "papers/waczak-thesis/thesis/thesis.pdf" "papers/waczak-thesis/thesis/thesis.toc" "plots/bothbound_long_stroke_angles_bd.pdf" "plots/bothbound_stroke_angles_bd.pdf" "plots/bothbound_stroke_angles_md.pdf" "plots/bothbound_stroke_tailx_positions.pdf" "plots/bothbound_stroke_taily_positions.pdf" "plots/burgess-model-figure.pdf" "plots/crystal-model-figure.pdf" "plots/grotjahn-model-figure.pdf" "plots/onebound_stroke_angles.pdf" "plots/onebound_stroke_tailx_positions.pdf" "plots/onebound_stroke_taily_positions.pdf" "plots/paper_displacement_vs_step_length.pdf" "plots/paper_force_vs_velocity.pdf" "plots/paper_initial_vs_final_displacement.pdf" "plots/paper_model_behavior.pdf" "plots/paper_trajectory_plot.pdf" "plots/paper_unbinding_probability_vs_displacement.pdf" "runlogs/paper_unbinding_probability__L-10,s-1.out" "runlogs/paper_unbinding_probability__L-20,s-1.out" "runlogs/paper_unbinding_probability__L-30,s-1.out" "runlogs/paper_unbinding_probability__L-40,s-1.out" "runlogs/paper_unbinding_probability__L-50,s-1.out" "runlogs/papertrajecto__k_b-9.5e+09,k_ub-1e+20,c-0,cb-0.08,cm-1.05,ct-0.36,ls-20.75,lt-23,seed-14,dt-1e-10.out" "scripts/dynein/draw/motor_domain.py" "scripts/dynein/draw/tail.py" "simulate_unbinding_rates" "src/.fac" "src/MersenneTwister.h.gch" "src/custom_simulation_parameters.h.gch" "src/default_parameters.h.gch" "src/dynein_simulate.o" "src/dynein_struct.h.gch" "src/dynein_struct_bothbound.o" "src/dynein_struct_onebound.o" "src/generate_stepping_data.o" "src/plotting_defaults.h.gch" "src/simulate_unbinding_rates.o" "src/simulation_defaults.h.gch" "src/test-rng.o" "src/test_bothbound.o" "src/test_onebound.o" "src/utilities.o"
papers/paper/figures/bothbound-cartoon-simple.pdf : papers/paper/figures/bothbound-cartoon-simple.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf bothbound-cartoon-simple.pdf bothbound-cartoon-simple.svg
papers/paper/figures/bothbound-cartoon.pdf : papers/paper/figures/bothbound-cartoon.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf bothbound-cartoon.pdf bothbound-cartoon.svg
papers/paper/figures/model-cartoon-simple.pdf : papers/paper/figures/model-cartoon-simple.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf model-cartoon-simple.pdf model-cartoon-simple.svg
papers/paper/figures/onebound-cartoon-simple.pdf : papers/paper/figures/onebound-cartoon-simple.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf onebound-cartoon-simple.pdf onebound-cartoon-simple.svg
papers/paper/figures/onebound-cartoon.pdf : papers/paper/figures/onebound-cartoon.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf onebound-cartoon.pdf onebound-cartoon.svg
papers/paper/figures/schematic-1-model.pdf : papers/paper/figures/schematic-1-model.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf schematic-1-model.pdf schematic-1-model.svg
papers/paper/figures/schematic-1-superimposed.pdf : papers/paper/figures/schematic-1-superimposed.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf schematic-1-superimposed.pdf schematic-1-superimposed.svg
papers/paper/figures/schematic-2-model.pdf : papers/paper/figures/schematic-2-model.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf schematic-2-model.pdf schematic-2-model.svg
papers/paper/figures/schematic-2-superimposed.pdf : papers/paper/figures/schematic-2-superimposed.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf schematic-2-superimposed.pdf schematic-2-superimposed.svg
papers/paper/figures/schematic-binding-angle.pdf : papers/paper/figures/schematic-binding-angle.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf schematic-binding-angle.pdf schematic-binding-angle.svg
papers/paper/figures/schematic-full.pdf : papers/paper/figures/schematic-full.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf schematic-full.pdf schematic-full.svg
papers/paper/figures/schematic-poststroke.pdf : papers/paper/figures/schematic-poststroke.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf schematic-poststroke.pdf schematic-poststroke.svg
papers/paper/figures/schematic-prestroke.pdf : papers/paper/figures/schematic-prestroke.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf schematic-prestroke.pdf schematic-prestroke.svg
papers/paper/figures/winch-powerstroke-cartoon.pdf : papers/paper/figures/winch-powerstroke-cartoon.svg
	cd "papers/paper/figures" && inkscape -D --export-pdf winch-powerstroke-cartoon.pdf winch-powerstroke-cartoon.svg
papers/paper/figures/svg.fac : papers/paper/figures papers/paper/figures/fac-svgs.py
	cd "papers/paper/figures" && python3 fac-svgs.py > svg.fac
papers/paper/paper.aux papers/paper/paper.bbl papers/paper/paper.blg papers/paper/paper.log papers/paper/paper.out papers/paper/paper.pdf : data/paper_params.tex papers/paper/figures/model-cartoon-simple.pdf papers/paper/figures/winch-powerstroke-cartoon.pdf papers/paper/paper.bib papers/paper/paper.tex papers/paper/pnas-new.bst papers/paper/pnas-new.cls papers/paper/pnasresearcharticle.sty papers/paper/widetext.sty plots/bothbound_long_stroke_angles_bd.pdf plots/bothbound_stroke_angles_bd.pdf plots/bothbound_stroke_angles_md.pdf plots/bothbound_stroke_tailx_positions.pdf plots/bothbound_stroke_taily_positions.pdf plots/burgess-model-figure.pdf plots/grotjahn-model-figure.pdf plots/paper_displacement_vs_step_length.pdf plots/paper_force_vs_velocity.pdf plots/paper_initial_vs_final_displacement.pdf plots/paper_model_behavior.pdf plots/paper_trajectory_plot.pdf plots/paper_unbinding_probability_vs_displacement.pdf plots/paper_stacked_displacement_histogram.pdf
	cd "papers/paper" && xelatex paper.tex && bibtex paper && xelatex paper.tex && xelatex paper.tex
papers/waczak-thesis/modelExplanation/model_explanation.aux papers/waczak-thesis/modelExplanation/model_explanation.log papers/waczak-thesis/modelExplanation/model_explanation.pdf : papers/waczak-thesis/modelExplanation/images/onebound.png papers/waczak-thesis/modelExplanation/model_explanation.tex
	cd "papers/waczak-thesis/modelExplanation" && pdflatex model_explanation.tex
papers/waczak-thesis/thesis/thesis.aux papers/waczak-thesis/thesis/thesis.bbl papers/waczak-thesis/thesis/thesis.blg papers/waczak-thesis/thesis/thesis.log papers/waczak-thesis/thesis/thesis.pdf papers/waczak-thesis/thesis/thesis.toc : papers/waczak-thesis/thesis/chapters/dynein.tex papers/waczak-thesis/thesis/chapters/introduction.tex papers/waczak-thesis/thesis/chapters/theory.tex papers/waczak-thesis/thesis/images/OSU_logo.png papers/waczak-thesis/thesis/images/onebound.pdf papers/waczak-thesis/thesis/images/schematic-2-superimposed.pdf papers/waczak-thesis/thesis/images/torque_angles.pdf papers/waczak-thesis/thesis/images/weihong_motor.png papers/waczak-thesis/thesis/references.bib papers/waczak-thesis/thesis/thesis.tex
	cd "papers/waczak-thesis/thesis" && pdflatex thesis.tex && bibtex thesis.aux && pdflatex thesis.tex && pdflatex thesis.tex
plots/paper_displacement_vs_step_length.pdf plots/paper_initial_vs_final_displacement.pdf : data data/paper_main_stepping_data-1.txt data/paper_main_stepping_data-10.txt data/paper_main_stepping_data-11.txt data/paper_main_stepping_data-12.txt data/paper_main_stepping_data-13.txt data/paper_main_stepping_data-14.txt data/paper_main_stepping_data-15.txt data/paper_main_stepping_data-16.txt data/paper_main_stepping_data-17.txt data/paper_main_stepping_data-18.txt data/paper_main_stepping_data-19.txt data/paper_main_stepping_data-2.txt data/paper_main_stepping_data-20.txt data/paper_main_stepping_data-21.txt data/paper_main_stepping_data-22.txt data/paper_main_stepping_data-23.txt data/paper_main_stepping_data-24.txt data/paper_main_stepping_data-25.txt data/paper_main_stepping_data-26.txt data/paper_main_stepping_data-27.txt data/paper_main_stepping_data-28.txt data/paper_main_stepping_data-29.txt data/paper_main_stepping_data-3.txt data/paper_main_stepping_data-30.txt data/paper_main_stepping_data-31.txt data/paper_main_stepping_data-32.txt data/paper_main_stepping_data-4.txt data/paper_main_stepping_data-5.txt data/paper_main_stepping_data-6.txt data/paper_main_stepping_data-7.txt data/paper_main_stepping_data-8.txt data/paper_main_stepping_data-9.txt scripts scripts/color_hist2.py scripts/dynein scripts/dynein/data.py
	python3 scripts/color_hist2.py -d paper_main && mv plots/initial-vs-final-displacement.pdf plots/paper_initial_vs_final_displacement.pdf &&  mv plots/initial-displacement-vs-step-length.pdf plots/paper_displacement_vs_step_length.pdf
plots/burgess-model-figure.pdf plots/crystal-model-figure.pdf plots/grotjahn-model-figure.pdf : data papers/paper/figures/model-raw-images/burgess-fig-4-cropped.png papers/paper/figures/model-raw-images/grotjahn-model-figure.png papers/paper/figures/model-raw-images/pymol-cytoplasmic-superimpose.png papers/paper/figures/model-raw-images/redwine-supplemental-cropped.png scripts scripts/dynein scripts/dynein/draw scripts/dynein/draw/motor_domain.py scripts/dynein/draw/tail.py scripts/generate-paper-model-figures.py
	python3 scripts/generate-paper-model-figures.py
build build/dynein_simulate.o build/dynein_struct_bothbound.o build/dynein_struct_onebound.o build/generate_stepping_data.o build/utilities.o data/paper_trajectory_movie_data.txt data/paper_trajectory_stepping_data.txt data/paper_trajectory_stepping_parameters.tex runlogs/papertrajecto__k_b-9.5e+09,k_ub-1e+20,c-0,cb-0.08,cm-1.05,ct-0.36,ls-20.75,lt-23,seed-14,dt-1e-10.out src/MersenneTwister.h.gch src/custom_simulation_parameters.h.gch src/default_parameters.h.gch src/dynein_struct.h.gch src/plotting_defaults.h.gch src/simulation_defaults.h.gch : . Makefile data generate_stepping_data papers papers/elliott-thesis/figures papers/paper/figures scripts scripts/dynein scripts/generate-paper-trajectory-data.py scripts/generate-stepping-data.py src src/MersenneTwister.h src/custom_simulation_parameters.h src/default_parameters.h src/dynein_simulate.cpp src/dynein_struct.h src/dynein_struct_bothbound.cpp src/dynein_struct_onebound.cpp src/generate_stepping_data.cpp src/plotting_defaults.h src/simulation_defaults.h src/utilities.cpp
	python3 scripts/generate-paper-trajectory-data.py

# data/paper_unbinding_probability__L-10,s-1.txt: simulate_unbinding_rates scripts/generate-paper-unbinding-rate-data.py data/paper_params.py scripts/dynein/run.py
# 	rm -f data/paper_unbinding_probability__*
# 	python3 scripts/generate-paper-unbinding-rate-data.py

# # Taken out of make, added data file to repository:
# # data/paper_histogram_stepping_data.txt: generate_stepping_data scripts/dynein/run.py scripts/histogram-helper.py scripts/dynein/run.py
# # 	python3 scripts/histogram-helper.py

# # Taken out of make, added data file to repository:
# # data/paper_stroke_angle_data_*.txt: generate_stepping_data scripts/dynein/run.py scripts/generate-angle-data.py
# # 	python3 scripts/generate-angle-data.py

# # Taken out of make, added data file to repository:
# # data/paper_long_stroke_angle_data_*.txt: generate_stepping_data scripts/dynein/run.py scripts/generate-angle-data.py
# # 	python3 scripts/generate-angle-data.py long

# ######### fun plots ##########
# plots/stepping_time_histogram_paper.pdf plots/stepping_length_histogram_paper.pdf: scripts/paper-histogram-plt.py $(STATIC_DATA)
# 	python3 scripts/paper-histogram-plt.py
# 	mv plots/stepping_length_histogram.pdf plots/stepping_length_histogram_paper.pdf
# 	mv plots/stepping_time_histogram.pdf plots/stepping_time_histogram_paper.pdf

# plots/stepping_time_histogram_%.pdf: scripts/make_stepping_plots.py
# 	python3 scripts/make_stepping_plots.py $*
# 	mv plots/stepping_time_histogram.pdf plots/stepping_time_histogram_$*.pdf

# ######### paper plots ##########
# PAPER_DATA = $(wildcard data/paper_main_stepping_data*.txt)
# PAPER_FORCE_DATA = $(wildcard data/paper_force_stepping_data*.txt)

# PAPER-PLOTS = plots/paper_trajectory_plot.pdf plots/paper_displacement_vs_step_length.pdf plots/paper_initial_vs_final_displacement.pdf plots/paper_foot_order_histogram.pdf plots/paper-trajectory-movie.mp4 plots/paper_model_behavior.pdf plots/paper_force_vs_velocity.pdf plots/paper_unbinding_probability_vs_displacement.pdf plots/stacked_displacement_histogram.pdf

# plots/paper_model_behavior.pdf plots/paper_foot_order_histogram.pdf plots/paper_force_vs_velocity.pdf plots/paper_unbinding_probability_vs_displacement.pdf plots/stacked_displacement_histogram.pdf: scripts/make_all_stepping_plots.py $(PAPER_DATA) data/paper_unbinding_probability__L-10,s-1.txt $(PAPER_FORCE_DATA)
# 	python3 scripts/make_all_stepping_plots.py -d data -b paper_main
# 	mv plots/model_behavior.pdf plots/paper_model_behavior.pdf
# 	mv plots/unbinding_probability_vs_displacement.pdf plots/paper_unbinding_probability_vs_displacement.pdf
# 	mv plots/force_vs_velocity.pdf plots/paper_force_vs_velocity.pdf

# plots/paper_initial_vs_final_displacement.pdf plots/paper_displacement_vs_step_length.pdf: scripts/color_hist2.py $(PAPER_DATA)
# 	python3 scripts/color_hist2.py -d paper_main
# 	mv plots/initial-vs-final-displacement.pdf plots/paper_initial_vs_final_displacement.pdf
# 	mv plots/initial-displacement-vs-step-length.pdf plots/paper_displacement_vs_step_length.pdf

# plots/paper_trajectory_plot.pdf: data/paper_trajectory_movie_data.txt scripts/paper-trajectory-plt.py $(DRAW)

plots/bothbound_long_stroke_angles_bd.pdf plots/bothbound_stroke_angles_bd.pdf plots/bothbound_stroke_angles_md.pdf plots/bothbound_stroke_tailx_positions.pdf plots/bothbound_stroke_taily_positions.pdf plots/onebound_stroke_angles.pdf plots/onebound_stroke_tailx_positions.pdf plots/onebound_stroke_taily_positions.pdf plots/paper_force_vs_velocity.pdf plots/paper_model_behavior.pdf plots/paper_unbinding_probability_vs_displacement.pdf plots/paper_stacked_displacement_histogram.pdf scripts/__pycache__ : data data/paper_force_stepping_data-F--1.0,s-1.txt data/paper_force_stepping_data-F--1.0,s-2.txt data/paper_force_stepping_data-F--1.0,s-3.txt data/paper_force_stepping_data-F--1.0,s-4.txt data/paper_force_stepping_data-F--10.0,s-1.txt data/paper_force_stepping_data-F--10.0,s-2.txt data/paper_force_stepping_data-F--10.0,s-3.txt data/paper_force_stepping_data-F--10.0,s-4.txt data/paper_force_stepping_data-F--2.0,s-1.txt data/paper_force_stepping_data-F--2.0,s-2.txt data/paper_force_stepping_data-F--2.0,s-3.txt data/paper_force_stepping_data-F--2.0,s-4.txt data/paper_force_stepping_data-F--3.0,s-1.txt data/paper_force_stepping_data-F--3.0,s-2.txt data/paper_force_stepping_data-F--3.0,s-3.txt data/paper_force_stepping_data-F--3.0,s-4.txt data/paper_force_stepping_data-F--4.0,s-1.txt data/paper_force_stepping_data-F--4.0,s-2.txt data/paper_force_stepping_data-F--4.0,s-3.txt data/paper_force_stepping_data-F--4.0,s-4.txt data/paper_force_stepping_data-F--5.0,s-1.txt data/paper_force_stepping_data-F--5.0,s-2.txt data/paper_force_stepping_data-F--5.0,s-3.txt data/paper_force_stepping_data-F--5.0,s-4.txt data/paper_force_stepping_data-F--6.0,s-1.txt data/paper_force_stepping_data-F--6.0,s-2.txt data/paper_force_stepping_data-F--6.0,s-3.txt data/paper_force_stepping_data-F--6.0,s-4.txt data/paper_force_stepping_data-F--7.0,s-1.txt data/paper_force_stepping_data-F--7.0,s-2.txt data/paper_force_stepping_data-F--7.0,s-3.txt data/paper_force_stepping_data-F--7.0,s-4.txt data/paper_force_stepping_data-F--8.0,s-1.txt data/paper_force_stepping_data-F--8.0,s-2.txt data/paper_force_stepping_data-F--8.0,s-3.txt data/paper_force_stepping_data-F--8.0,s-4.txt data/paper_force_stepping_data-F-0.0,s-1.txt data/paper_force_stepping_data-F-0.0,s-2.txt data/paper_force_stepping_data-F-0.0,s-3.txt data/paper_force_stepping_data-F-0.0,s-4.txt data/paper_force_stepping_data-F-4.0,s-1.txt data/paper_force_stepping_data-F-4.0,s-2.txt data/paper_force_stepping_data-F-4.0,s-3.txt data/paper_force_stepping_data-F-4.0,s-4.txt data/paper_long_stroke_angle_data_1.txt data/paper_long_stroke_angle_data_2.txt data/paper_long_stroke_angle_data_3.txt data/paper_long_stroke_angle_data_4.txt data/paper_main_stepping_data-1.txt data/paper_main_stepping_data-10.txt data/paper_main_stepping_data-11.txt data/paper_main_stepping_data-12.txt data/paper_main_stepping_data-13.txt data/paper_main_stepping_data-14.txt data/paper_main_stepping_data-15.txt data/paper_main_stepping_data-16.txt data/paper_main_stepping_data-17.txt data/paper_main_stepping_data-18.txt data/paper_main_stepping_data-19.txt data/paper_main_stepping_data-2.txt data/paper_main_stepping_data-20.txt data/paper_main_stepping_data-21.txt data/paper_main_stepping_data-22.txt data/paper_main_stepping_data-23.txt data/paper_main_stepping_data-24.txt data/paper_main_stepping_data-25.txt data/paper_main_stepping_data-26.txt data/paper_main_stepping_data-27.txt data/paper_main_stepping_data-28.txt data/paper_main_stepping_data-29.txt data/paper_main_stepping_data-3.txt data/paper_main_stepping_data-30.txt data/paper_main_stepping_data-31.txt data/paper_main_stepping_data-32.txt data/paper_main_stepping_data-4.txt data/paper_main_stepping_data-5.txt data/paper_main_stepping_data-6.txt data/paper_main_stepping_data-7.txt data/paper_main_stepping_data-8.txt data/paper_main_stepping_data-9.txt data/paper_stroke_angle_data_1.txt data/paper_stroke_angle_data_2.txt data/paper_stroke_angle_data_3.txt data/paper_stroke_angle_data_4.txt data/paper_stroke_angle_data_5.txt data/paper_unbinding_probability__L-10,s-1.txt data/paper_unbinding_probability__L-20,s-1.txt data/paper_unbinding_probability__L-30,s-1.txt data/paper_unbinding_probability__L-40,s-1.txt data/paper_unbinding_probability__L-50,s-1.txt data/yildiz_tracking_data.txt scripts scripts/basic_units.py scripts/dynein scripts/dynein/draw scripts/dynein/draw/motor_domain.py scripts/dynein/draw/tail.py scripts/make_all_stepping_plots.py
	python3 scripts/make_all_stepping_plots.py -d data -b paper_main &&	mv plots/model_behavior.pdf plots/paper_model_behavior.pdf &&	mv plots/unbinding_probability_vs_displacement.pdf plots/paper_unbinding_probability_vs_displacement.pdf &&	mv plots/force_vs_velocity.pdf plots/paper_force_vs_velocity.pdf
plots/paper_trajectory_plot.pdf : data/paper_trajectory_movie_data.txt data/paper_trajectory_stepping_parameters.tex scripts scripts/dynein scripts/dynein/draw scripts/dynein/draw/motor_domain.py scripts/dynein/draw/tail.py scripts/paper-trajectory-plt.py

	python3 scripts/paper-trajectory-plt.py data/paper_trajectory
data/paper_params.tex : data scripts scripts/parameters-to-latex.py
	python3 scripts/parameters-to-latex.py
data/paper_unbinding_probability__L-10,s-1.txt data/paper_unbinding_probability__L-20,s-1.txt data/paper_unbinding_probability__L-30,s-1.txt data/paper_unbinding_probability__L-40,s-1.txt data/paper_unbinding_probability__L-50,s-1.txt runlogs/paper_unbinding_probability__L-10,s-1.out runlogs/paper_unbinding_probability__L-20,s-1.out runlogs/paper_unbinding_probability__L-30,s-1.out runlogs/paper_unbinding_probability__L-40,s-1.out runlogs/paper_unbinding_probability__L-50,s-1.out : data scripts scripts/dynein scripts/dynein/run.py scripts/generate-paper-unbinding-rate-data.py simulate_unbinding_rates
	rm -f data/paper_unbinding_probability__* && python3 scripts/generate-paper-unbinding-rate-data.py
scripts/dynein/draw/motor_domain.py : scripts/dynein/draw/create_MD_array.py scripts/dynein/draw/outer_coords.txt
	cd "scripts/dynein/draw" && python create_MD_array.py
scripts/dynein/draw/tail.py : scripts/dynein/draw/tailDomain.py
	cd "scripts/dynein/draw" && python tailDomain.py
src/dynein_simulate.o : src/MersenneTwister.h src/dynein_simulate.cpp src/dynein_struct.h src/simulation_defaults.h
	cd "src" && g++ -c dynein_simulate.cpp -std=c++98 -g -Werror -O2 -Wall
src/dynein_struct_bothbound.o : src/MersenneTwister.h src/dynein_struct.h src/dynein_struct_bothbound.cpp
	cd "src" && g++ -c dynein_struct_bothbound.cpp -std=c++98 -g -Werror -O2 -Wall
src/dynein_struct_onebound.o : src/MersenneTwister.h src/dynein_struct.h src/dynein_struct_onebound.cpp
	cd "src" && g++ -c dynein_struct_onebound.cpp -std=c++98 -g -Werror -O2 -Wall
src/generate_stepping_data.o : src/MersenneTwister.h src/default_parameters.h src/dynein_struct.h src/generate_stepping_data.cpp src/simulation_defaults.h
	cd "src" && g++ -c generate_stepping_data.cpp -std=c++98 -g -Werror -O2 -Wall
src/simulate_unbinding_rates.o : src/MersenneTwister.h src/default_parameters.h src/dynein_struct.h src/simulate_unbinding_rates.cpp src/simulation_defaults.h
	cd "src" && g++ -c simulate_unbinding_rates.cpp -std=c++98 -g -Werror -O2 -Wall
src/test-rng.o : src/MersenneTwister.h src/test-rng.cpp
	cd "src" && g++ -c test-rng.cpp -std=c++98 -g -Werror -O2 -Wall
src/test_bothbound.o : src/MersenneTwister.h src/default_parameters.h src/dynein_struct.h src/test_bothbound.cpp
	cd "src" && g++ -c test_bothbound.cpp -std=c++98 -g -Werror -O2 -Wall
src/test_onebound.o : src/MersenneTwister.h src/default_parameters.h src/dynein_struct.h src/test_onebound.cpp
	cd "src" && g++ -c test_onebound.cpp -std=c++98 -g -Werror -O2 -Wall
src/utilities.o : src/MersenneTwister.h src/custom_simulation_parameters.h src/dynein_struct.h src/plotting_defaults.h src/simulation_defaults.h src/utilities.cpp
	cd "src" && g++ -c utilities.cpp -std=c++98 -g -Werror -O2 -Wall
generate_stepping_data : src/dynein_simulate.o src/dynein_struct_bothbound.o src/dynein_struct_onebound.o src/generate_stepping_data.o src/utilities.o
	cd "src" && g++ -o ../generate_stepping_data generate_stepping_data.o dynein_simulate.o dynein_struct_onebound.o dynein_struct_bothbound.o utilities.o
simulate_unbinding_rates : src/dynein_simulate.o src/dynein_struct_bothbound.o src/dynein_struct_onebound.o src/simulate_unbinding_rates.o src/utilities.o
	cd "src" && g++ -o ../simulate_unbinding_rates simulate_unbinding_rates.o dynein_simulate.o dynein_struct_onebound.o dynein_struct_bothbound.o utilities.o
src/.fac : src src/configure.py
	cd "src" && python3 configure.py > .fac
