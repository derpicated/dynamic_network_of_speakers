# makefile for pdflatex
# pdflatex cant support some things
#PDF_LATEX=pdflatex
PDF_LATEX=xelatex
#PDF_LATEX=latexmk -pdf
#BIBTEX=bibtex
BIBTEX=biber
LATEXFLAGS=-enable-write18
RM=rm -f
BUILD_DIR=build
PDF_DIR=pdf
COPY_BIB=find -name "*.bib" -not -path "./$(BUILD_DIR)/*" | xargs cp -t $(BUILD_DIR)
CHECK_BUILD=if [ ! -d "./$(BUILD_DIR)" ];then \
		mkdir $(BUILD_DIR);\
	    fi

# Generate PDF
pdf_src=./main.tex
pdf_name=document
gen_pdf=$(PDF_LATEX) $(LATEXFLAGS) -output-directory=$(BUILD_DIR) -jobname=$(pdf_name) $(pdf_src) ./$(BUILD_DIR)/*
pdf:$(pdf_src)
	$(CHECK_BUILD)
	$(COPY_BIB)
	$(gen_pdf)
	cd $(BUILD_DIR)/;$(BIBTEX) $(pdf_name).bcf
	#cd $(BUILD_DIR)/;$(BIBTEX) $(pdf_name)
	$(gen_pdf)
	$(gen_pdf)

# Remove logs
clean_logs:
	find $(BUILD_DIR) -type f -not -name '*pdf' | xargs rm

# Clean build dir
clean:
	$(RM) ./$(BUILD_DIR)/*

# Purge build dir
purge:
	$(RM)r ./$(BUILD_DIR)
