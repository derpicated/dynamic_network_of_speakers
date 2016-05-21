#!/bin/bash
# Install LaTeX and needed deps for the documentation

VERSION="0.1.0"

echo "-- Installing LaTeX dependencies for this project v$VERSION"
echo

# Get package manager
DNF_CMD=$(which dnf)
APT_GET_CMD=$(which apt-get)

# Packages per manager
# DNF
DNF_PACKAGE_NAMES=(
    texlive
    texlive-tools
    texlive-babel
    texlive-csquotes
    texlive-biblatex
    texlive-biblatex-apa
    texlive-geometry
    texlive-titling
    texlive-fancyhdr
    texlive-tocloft
    texlive-changepage
    texlive-fontspec
    texlive-titlesec
    texlive-fancyvrb
    texlive-booktabs
    texlive-hyperref
    texlive-xcolor
    texlive-toolbox
    texlive-caption
    texlive-lipsum
    texlive-xetex
    texlive-euenc
    texlive-wrapfig
    texlive-dirtytalk
    texlive-collection-fontsrecommended
    pandoc
)
# apt-get
DEB_PACKAGE_NAMES=(
    texlive
    texlive-latex-extra
    texlive-fonts-recommended
    texlive-bibtex-extra
    texlive-xetex
    latex-xcolor
    lmodern
    biber
    pandoc
)

if [[ ! -z $DNF_CMD ]]; then
    echo "Installing with DNF"
    sudo dnf install ${DNF_PACKAGE_NAMES[@]}
elif [[ ! -z $APT_GET_CMD ]]; then
    echo "Installing with APT"
    sudo apt-get install ${DEB_PACKAGE_NAMES[@]} --no-install-recommends # No recommended docs of 500+ MB!
else
    echo "package manager not supported"
    exit 1;
fi

echo
echo "-- Done installing the dependencies"
echo
