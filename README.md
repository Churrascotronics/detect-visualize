## Visualizador de DLib

### Dependencias 
    - dlib, libfmt, portable-file-dialogs, plf-nanotimer
```bash
mkdir lib/
cd lib/
# Clone/wget dependencies
wget https://raw.githubusercontent.com/mattreecebentley/plf_nanotimer/refs/heads/master/plf_nanotimer.h
git clone https://github.com/samhocevar/portable-file-dialogs --depth=1
git clone https://github.com/davisking/dlib --depth=1
git clone https://github.com/fmtlib/fmt --depth=1
git clone https://github.com/raysan5/raylib --depth=1
```
Sim isso tudo pode ser resolvido com FetchContent mas tenho preguiça de aprender CMake além do necessário.

### Para compilar:
```bash
mkdir build/
cd build/
cmake ..
make
./ui
```
