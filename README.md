## Visualizador de DLib

Para compilar:
```bash
mkdir build/
cd build/
cmake ..
make
./ui
```

Precisa de: 
    dlib, libfmt, portable-file-dialogs, plf-nanotimer
```bash
cd lib/
# Clone/wget dependencies
git clone https://github.com/samhocevar/portable-file-dialogs
wget https://raw.githubusercontent.com/mattreecebentley/plf_nanotimer/refs/heads/master/plf_nanotimer.h
git clone https://github.com/davisking/dlib
git clone https://github.com/fmtlib/fmt
```
Sim isso tudo pode ser resolvido com FetchContent mas tenho preguiça de aprender CMake além do necessário.
