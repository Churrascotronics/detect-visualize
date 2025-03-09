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
# Para compilar com benchmarks, coloque -DBENCHMARK=True após 'cmake ..'
cmake ..
# Para acelerar, use make -j[threads] onde [threads] é o numero de threads disponiveis no PC
# (ALTAMENTE RECOMENDADO, dlib demora muito para compilar numa thread só)
make && ./ui
```
### Opções do CMake:
(Definidas com `cmake .. -Dopção=valor`.)
Entre parenteses seus valores default.
 - `DEBUG` **\[True|(False)\]**
   - Mensagens de debug, automaticamente ativa BENCHMARK
 - `BENCHMARK` **\[True|(False)\]**
   - Mensagens acerca do timing do programa
 - `CMAKE_BUILD_TYPE` **\[(Release)|Debug|RelWithDebInfo|MinSizeRel\]**
   - Tipo de build, mais otimizado é Release, mais lento é Debug.

#### Addendum:
Por causa da maldição do cache do cmake, depois de buildar uma vez se você precisar dar build denovo e mudar alguma opção, é necessario ser explicito com o valor. Onde por default DEBUG=False, depois de uma build precisa fazer `cmake .. -DDEBUG=False`
