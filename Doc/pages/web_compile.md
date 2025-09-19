\page web_compile Building Web Demo

Ensure [emsdk](https://emscripten.org/docs/getting_started/downloads.html) is installed and activated.

### Building

```shell
emcmake cmake -B build
cmake --build build
```

### Hosting

- Host the project folder using any web server (e.g., `python3 -m http.server`, `http-server`, etc.).
- Open `index.html` in your browser as the entry point.