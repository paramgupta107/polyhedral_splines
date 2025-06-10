# WebAssembly Demo

## Table of Contents
- [Prerequisites](#prerequisites)
- [Building](#building)
- [Hosting](#hosting)
- [Cite as](#cite-as)

# Prerequisites

- Ensure [emsdk](https://emscripten.org/docs/getting_started/downloads.html) is installed and activated.

# Building

\`\`\`shell
emcmake cmake -B build
cmake --build build
\`\`\`

# Hosting

- Host the project folder using any web server (e.g., `python3 -m http.server`, `http-server`, etc.).
- Open `index.html` in your browser as the entry point.

# Cite as

<i>Bi-cubic splines for polyhedral control nets</i>,  
Jörg Peters, Kyle Lo, Kestutis Karciauskas  
TOMS, 2023  
[paper](https://www.cise.ufl.edu/research/SurfLab/papers/23PolySpl_TOMS.pdf)  
[documentation](https://www.cise.ufl.edu/research/SurfLab/papers/23PolySpl_TOMS_Doc.pdf)
