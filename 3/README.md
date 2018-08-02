#### Build

```sh
$ ./cmd.sh build
```
(или cmake + make)
#### Run

```sh
$ ./cmd.sh run
```
#### Clean

```sh
$ ./cmd.sh clean
```

#### Deps
GLFW, Assimp, Opengl lib

Установка зависимостей, если есть apt : 
```sh
# ./cmd.sh deps
```

#### Controls

WASD + Mouse - movement

GH - change gamma 

X - remove 10 lights

Z - add 10 lights

M - change mode (0 - composition, than gbuffer texs : 1 - position, 2 - normal, 3 - diffuse, 4 - spec) 

