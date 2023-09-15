# Luta de Robôs

Projeto final de Computação Gráfica
O Trabalho consiste em modelar dois lutadores. O aluno deve escolher uma modalidade de luta
e modelar dois lutadores que podem (ou não) lutar um contra o outro.
Pode ser feito também uma espécie de coreografia ("kata").
A cena deve conter ao menos:

-   dois lutadores;
-   movimentos de chute, socos e giro;
-   rolamento (ou algo como ponte);
-   espaço da luta (ringue, por exemplo);
-   tratamento de colisão de objetos;
-   música.

## Desenvolvimento

Utilização da biblioteca GLUT e FREEGLUT para modelagem e animação dos objetos da cena.

## Manual de funcionamento
> Todas as teclas são letras minúsculas

Lista de comandos utilizados para visualizar a cena:

-   `Q` Tecla para distanciar a câmera da cena
-   `E` Tecla para aproximar a câmera da cena
-   `W - S` Teclas para subir ou descer a câmera da cena
-   `A - D` Teclas para girar a câmera em 360º em torno da cena
-   `O` Tecla para resetar a câmera da cena

### Menu

Lista dos botões do menu

-   `2` Inicia a animação

### Animação

-   `IDLE`: Animação inicial dos robôs, posição de início da luta em modo de defesa.
-   `SOCO`: Animação em que os robôs tomam posição de luta prontos para desferir socos
-   `CHUTE`: Animação em que os robôs tomam posição de luta prontos para desferir chutes
-   `CAMBALHOTA`: Animação em que os robôs dão um salto e giram no mesmo lugar

## Criado por

João Pedro
