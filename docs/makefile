# Makefile for Documentation
LATEX_PDF=xelatex
BIBTEX=biber
COPY_BIB=find -name "*.bib" -not -path "./$(BUILD_DIR)/*" | xargs cp -t $(BUILD_DIR)
LATEXFLAGS=-enable-write18
RM=rm -f
BUILD_DIR=build
COPY_BIB=find -name "*.bib" -not -path "./$(BUILD_DIR)/*" | xargs cp -t $(BUILD_DIR)
CHECK_BUILD=if [ ! -d "./$(BUILD_DIR)" ];then	\
				mkdir $(BUILD_DIR);     		\
			fi

# Generate PDF
latex_src=./main.tex
document_name=dynamic_network_of_speakers
latex_to_pdf=$(LATEX_PDF) $(LATEXFLAGS) -output-directory=$(BUILD_DIR) -jobname=$(document_name) $(latex_src)
documentation:$(latex_src)
			  $(CHECK_BUILD)
			  $(COPY_BIB)
			  $(latex_to_pdf)
			  cd $(BUILD_DIR)/;$(BIBTEX) $(document_name).bcf
			  $(latex_to_pdf)
			  $(latex_to_pdf)

# Clean build dir
clean:
	$(RM) ./$(BUILD_DIR)/*

# Purge build dir
purge:
	$(RM)r ./$(BUILD_DIR)
