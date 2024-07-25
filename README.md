# Luminara-Raytracing

## Documentação: 

# Sistema de Raytracing

Este é um exemplo básico de implementação de um sistema de traçado de raios utilizando C++ e CUDA. O sistema renderiza uma cena simples contendo esferas e triângulos com diferentes materiais.

## Estrutura do Projeto

O projeto consiste nos seguintes arquivos principais:

### Os arquivos `main.cpp` e `main.cu`

Este é o arquivo principal que configura a cena, cria a câmera e executa o traçado de raios para cada pixel da imagem final.

### O diretório `utils` contém `utils.h`, `types.h` e `logs.h`

Contêm funções utilitárias, como operações matemáticas, geração de números aleatórios e macros úteis para facilitar o desenvolvimento.

### O diretório `Window`

Contém funções e classes para criar e gerenciar uma janela com GLFW e usar OpenGL para renderizar a texturar do raytracing, assim mostrando ao usuário o resultado do código.

### O diretório `raytracing/geometry`

Este diretório contém as classes e funções relacionadas à geometria:

- **`vec3.h`**: Define a classe `vec3` para representação e operações vetoriais 3D.
- **`ray.h`**: Define a classe `ray` para representar um raio no espaço.


### O diretório `raytracing/objects`

- **`hittable.h`**: Contêm as classes e estruturas que definem os objetos "hittables" na cena (objetos que podem ser atingidos por raios)
- **`triangle.h`**: Define a classe `triangle` que herda de `hittable` e representa um triângulo com funcionalidade de interseção usando o algoritmo de Möller-Trumbore.
- **`sphere.h`**: Define a classe `sphere` que herda de `hittable` e representa uma esfera com funcionalidade de interseção usando a fórmula matemática padrão.


### O arquivo `materials.h`

Este arquivo contém as classes relacionadas aos materiais aplicados às superfícies:

- **`material.h`**: Define a classe base `material` e as classes `lambertian`, `metal` e `dielectric` com método virtual `scatter` para determinar o comportamento do raio ao atingir uma superfície.
- **`lambertian`**: Classe `lambertian`, um tipo de material difuso que espalha a luz de forma uniforme.
- **`metal`**: Classe `metal`, que representa um material metálico refletivo.
- **`dielectric`**: Classe `dielectric`, que representa um material dielétrico transparente (como vidro) com cálculos de reflexão e refração usando a fórmula de Fresnel.

### O arquivo `camera.h`

Define a classe Camera que representa a câmera usada para renderizar a cena:
- Camera: Configura a posição, orientação e campo de visão da câmera, além de gerar raios para cada pixel da imagem.

### Os arquivos `worlds.h` e `worlds_cuda.cu` 

Definem os objetos de cada cena (esferas e triângulos), no momento temos 2 cenas prontas:
- simple_world
- book_cover_world

## Funcionamento

1. **Inicialização da Cena**: No arquivo `main.cpp`, os objetos da cena (esferas e triângulos) são inicializados com suas posições, tamanhos e materiais.

2. **Traçado de Raios**: Para cada pixel na imagem final, um raio é lançado da câmera através da cena. O raio é testado para interseções com todos os objetos da cena (esferas e triângulos).

3. **Cálculo da Iluminação**: Quando um raio atinge um objeto, dependendo do material do objeto, o raio pode ser disperso (difuso), refletido (metálico) ou refratado (dielétrico). A cor resultante é calculada com base na reflexão da luz, sombras e iluminação ambiente.

4. **Acúmulo e Renderização**: As cores calculadas para cada pixel são acumuladas para formar a imagem final, que é então exibida na tela ou salva em um arquivo de imagem.

## Compilação e Execução

Para compilar e executar o projeto olhe o arquivo Makefile 

- Versão de CPU: make render 
- Versão de GPU (CUDA): make render_cuda
- Perfilamento: maker profile_render_cuda

---

Este README.md fornece uma visão geral dos componentes e funcionamento do sistema de raytracing implementado nos arquivos fornecidos. Ele serve como um guia inicial para entender como os diferentes elementos se unem para criar uma cena renderizada usando técnicas de traçado de raios simples.


[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/IwHO6ydp)



