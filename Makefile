CPPFLAGS = -std=c++98 -g -Werror -O2 -Wall
LIBRARIES = -lm

PAPERS = papers/fft_speedup/convolution_theorem.pdf papers/paper/paper.pdf papers/notes/notes.pdf papers/elliott-thesis/latex/capek.pdf
DRAW = scripts/dynein/draw/motor_domain.py scripts/dynein/draw/tail.py
HEADERS = $(wildcard src/*.h)

THESIS-PLOTS = plots/trajectory-plot_thesis.pdf plots/stepping_time_histogram_thesis.pdf plots/stepping_length_histogram_thesis.pdf

PAPER-PLOTS = plots/paper_trajectory_plot.pdf plots/stepping_time_histogram_paper.pdf plots/stepping_length_histogram_paper.pdf plots/paper_static_time_vs_length.pdf

all: generate_stepping_data public $(DRAW)

.PHONY: clean public

######### SRC stuff ##########
build/%.o: src/%.cpp $(HEADERS)
	$(CXX) -c $^ $(CPPFLAGS)
	mkdir -p build
	mv $*.o build/

generate_stepping_data: build/generate_stepping_data.o build/dynein_simulate.o \
			build/dynein_struct_onebound.o build/dynein_struct_bothbound.o \
			build/utilities.o
	$(CXX) -o generate_stepping_data $^

######### draw module stuff ##########
scripts/dynein/draw/motor_domain.py: scripts/dynein/draw/create_MD_array.py scripts/dynein/draw/outer_coords.txt
	cd scripts/dynein/draw && python create_MD_array.py

scripts/dynein/draw/tail.py: scripts/dynein/draw/tailDomain.py
	cd scripts/dynein/draw && python tailDomain.py

######### data ##########
data/thesis_stepping_data.txt data/thesis_movie_data.txt: scripts/dynein/run.py scripts/generate-thesis-data.py
	python3 scripts/generate-thesis-data.py

data/paper_trajectory_stepping_data.txt data/paper_trajectory_movie_data.txt: generate_stepping_data scripts/dynein/run.py scripts/generate-paper-trajectory-data.py
	python3 scripts/generate-paper-trajectory-data.py

# Taken out of make, added data file to repository:
# data/paper_histogram_stepping_data.txt: generate_stepping_data scripts/dynein/run.py scripts/histogram-helper.py
# 	python3 scripts/histogram-helper.py

######### fun plots ##########
plots/stepping_time_histogram_paper.pdf plots/stepping_length_histogram_paper.pdf plots/stepping_analysis_paper.pdf: scripts/paper-histogram-plt.py $(STATIC_DATA)
	python3 scripts/paper-histogram-plt.py
	mv plots/stepping_length_histogram.pdf plots/stepping_length_histogram_paper.pdf
	mv plots/stepping_time_histogram.pdf plots/stepping_time_histogram_paper.pdf
	mv plots/stepping_analysis.pdf plots/stepping_analysis_paper.pdf

plots/stepping_time_histogram_%.pdf: scripts/make_stepping_plots.py
	python3 scripts/make_stepping_plots.py $*
	mv plots/stepping_time_histogram.pdf plots/stepping_time_histogram_$*.pdf

######### paper plots ##########
STATIC_DATA = $(wildcard data/paper_static_stepping_data*.txt)
EXPONENTIAL_DATA = $(wildcard data/paper_exponential_stepping_data*.txt)
PAPER_DATA = $(STATIC_DATA) $(EXPONENTIAL_DATA)

plots/paper_static_step_length_histogram.pdf: scripts/make_paper_stepping_histograms.py $(STATIC_DATA)
	python3 scripts/make_paper_stepping_histograms.py -b static
	mv plots/stepping_length_histogram.pdf plots/paper_static_step_length_histogram.pdf

plots/paper_exponential_step_length_histogram.pdf: scripts/make_paper_stepping_histograms.py $(EXPONENTIAL_DATA)
	python3 scripts/make_paper_stepping_histograms.py -b exponential
	mv plots/stepping_length_histogram.pdf plots/paper_exponential_step_length_histogram.pdf

plots/paper_static_foot_order_histogram.pdf: scripts/make_paper_stepping_histograms.py $(STATIC_DATA)
	python3 scripts/make_paper_stepping_histograms.py -b static
	mv plots/stepping_analysis.pdf plots/paper_static_foot_order_histogram.pdf

plots/paper_exponential_foot_order_histogram.pdf: scripts/make_paper_stepping_histograms.py $(EXPONENTIAL_DATA)
	python3 scripts/make_paper_stepping_histograms.py -b exponential
	mv plots/stepping_analysis.pdf plots/paper_exponential_foot_order_histogram.pdf

plots/paper_static_time_vs_length.pdf: scripts/color_hist.py $(STATIC_DATA)
	python3 scripts/color_hist.py -a
	mv plots/time-vs-length-multiple-seeds.pdf plots/paper_static_time_vs_length.pdf

plots/paper_trajectory_plot.pdf: data/paper_trajectory_movie_data.txt scripts/paper-trajectory-plt.py $(DRAW)
	python3 scripts/paper-trajectory-plt.py data/paper_trajectory

######### thesis plots ##########
plots/trajectory-plot_thesis.pdf: data/thesis_movie_data.txt scripts/trajectory-plt.py $(DRAW)
	python3 scripts/trajectory-plt.py data/thesis_movie_data.txt
	mv plots/trajectory-plot.pdf plots/trajectory-plot_thesis.pdf

plots/stepping_time_histogram_thesis.pdf plots/stepping_length_histogram_thesis.pdf: scripts/make_stepping_plots.py data/thesis_stepping_data.txt
	python3 scripts/make_stepping_plots.py data/thesis_stepping_data.txt
	mv plots/stepping_length_histogram.pdf plots/stepping_length_histogram_thesis.pdf
	mv plots/stepping_time_histogram.pdf plots/stepping_time_histogram_thesis.pdf

######### papers ##########
PAPER_SVG_FIGURES = $(wildcard papers/*/figures/*.svg)
PAPER-FIGURES = $(patsubst %.svg,%.pdf,$(PAPER_SVG_FIGURES))

papers/elliott-thesis/figures/%.pdf: papers/elliott-thesis/figures/%.svg
	inkscape -D --export-pdf $(shell pwd)/$@ $(shell pwd)/$<

papers/paper/figures/%.pdf: papers/paper/figures/%.svg
	inkscape -D --export-pdf $(shell pwd)/$@ $(shell pwd)/$<

papers/fft_speedup/convolution_theorem.pdf: papers/fft_speedup/convolution_theorem.tex
	cd papers/fft_speedup && pdflatex convolution_theorem &&  pdflatex convolution_theorem

papers/elliott-thesis/latex/capek.pdf: papers/elliott-thesis/latex/thesis.tex $(THESIS-PLOTS)
	cd papers/elliott-thesis/latex && xelatex thesis.tex && bibtex thesis && xelatex thesis.tex && xelatex thesis.tex
	mv papers/elliott-thesis/latex/thesis.pdf papers/elliott-thesis/latex/capek.pdf

papers/paper/paper.pdf: papers/paper/paper.tex $(PAPER-FIGURES) $(PAPER-PLOTS)
	(cd papers/paper && xelatex paper.tex && bibtex paper && xelatex paper.tex && xelatex paper.tex) || (rm -f $@ && false)

papers/notes/notes.pdf: papers/notes/notes.tex
	cd papers/notes && xelatex notes.tex

public: $(PAPERS)
	cp -v $(PAPERS) public/

clean:
	rm -f build/*.o
	rm -f generate_stepping_data
	rm -f scripts/dynein/draw/motor_domain.py scripts/dynein/draw/tail.py
	rm -f scripts/*.pyc scripts/*/*.pyc
	rm -rf plots
	rm -f $(PAPER-FIGURES) $(THESIS-PLOTS) $(PAPER-PLOTS)
	rm -f public/*.pdf public/*.tex
	rm -f data/thesis_*
