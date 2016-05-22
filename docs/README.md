# DNS Documentation

This folder contains all the necessary documentation for this project.

Most of the documentation is written in LaTeX. The supplied makefile can generate a PDF from these files.

### Dependencies
To make the documentation from the LaTeX source files, a list of dependencies are needed. 
To install these dependencies, run the `install_latex.sh` script in this folder.

## Compilation
The documentation has a custom makefile. With this makefile, the documentation can be compiled.
The make commands can be issued from root directory of this project.
All the build executables are made in a 'build' folder.

Make commands available:

- `make` or `make documentation`: Build the PDF file from the LaTeX files.
- `make clean`: Clean the build directory
- `make purge`: Delete the Build directory
