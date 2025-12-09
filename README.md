# Ray Tracing
Roger Dornas Oliveira

Wallace Eduardo Pereira

Este projeto é uma implementação de um Ray Tracer em C++, capaz de renderizar cenas 3D com iluminação global básica, incluindo sombras, reflexão e refração. O projeto não utiliza bibliotecas gráficas externas (como OpenGL ou DirectX) para o cálculo de luz, realizando toda a matemática vetorial e de interseção do zero e exportando o resultado em formato de imagem PPM.

## Funcionalidades Implementadas
- Primitivas Geométricas: esferas e poliedros covexos
- Câmera
- Modelo de Iluminação e Sombreamento
- Texturização e Materiais

## Estrutura do Projeto
O projeto segue uma arquitetura modularizada:

```text
PROJETO_RAIZ/
│
├── Makefile           # Script de automação de compilação
├── README.md          # Documentação
│
├── include/           # Cabeçalhos (.h)
│   ├── camera.h       # Lógica da câmera e geração de raios
│   ├── object.h       # Classe base abstrata para objetos
│   ├── sphere.h       # Derivado de Object
│   ├── polyhedron.h   # Derivado de Object (planos)
│   ├── scene.h        # Estruturas de Luz, Pigmento, Acabamento e Cena
│   ├── ray.h          # Definição do Raio
│   └── vec3.h         # Biblioteca matemática vetorial
│
├── src/               # Código Fonte (.cpp)
│   ├── main.cpp       # Loop principal de Ray Casting e output
│   └── parser.cpp     # Leitor de arquivos de cena e texturas
│
├── obj/               # Arquivos objeto intermediários (criado automaticamente)
└── bin/               # Executável final (criado automaticamente)
```

## Como Compilar
Este projeto utiliza um Makefile para gerenciar a compilação. Certifique-se de ter o compilador g++ instalado. Abra o terminal na raiz do projeto e execute:

```text
make
```

Isso criará as pastas obj/ e bin/ e gerará o executável raytracer dentro da pasta bin/.

Para limpar arquivos temporários:

```text
make clean
```


## Como Executar
O programa deve ser executado via linha de comando, recebendo um arquivo de descrição de cena como entrada.

```text
./bin/raytracer <arquivo_cena> [nome_saida.ppm]
```

Você pode rodar diretamente pelo Makefile passando os argumentos:

```text
make run cena.in output.ppm
```
