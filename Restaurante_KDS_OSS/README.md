# Restaurante KDS OSS

## 1. Descripción del proyecto

Restaurante KDS OSS es un sistema desarrollado en C++ que permite gestionar pedidos de un restaurante.

Este proyecto fue desarrollado como parte de un trabajo académico de Ingeniería en Sistemas.

## 2. Tecnologías utilizadas

- Lenguaje de programación: C++.
- Estándar: C++17.
- Herramienta de compilación: CMake.
- Control de versiones: Git.
- Repositorio: GitHub.

## 3. Requisitos para ejecutar el proyecto

- Un compilador compatible con C++17 (por ejemplo, GCC).
- CMake versión 3.10 o superior.

## 4. Estructura del proyecto

- `main.cpp`: contiene el código principal del sistema.
- `CMakeLists.txt`: contiene la configuración necesaria para compilar el proyecto.
- `pedidos.txt`: archivo utilizado para almacenar datos de prueba.
- `.gitignore`: especifica los archivos que no deben incluirse en el repositorio.

## 5. Instrucciones de compilación

Desde la terminal, dentro de la carpeta del proyecto, ejecutar:

```bash
cmake -S . -B build
cmake --build build
```

## 6. Ejecución

En Windows, ejecutar:

```powershell
.\build\Debug\restaurante.exe
```

Si el ejecutable se genera directamente dentro de `build`, ejecutar:

```powershell
.\build\restaurante.exe
```

## 7. Datos de prueba

El proyecto incluye el archivo `pedidos.txt`, que contiene datos utilizados para realizar pruebas del sistema.

## 8. Versión del proyecto

Versión final: 1.0.0

