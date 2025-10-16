#!/bin/bash
# Para entrar al archivo: cd version2/secuencial
# Para dar permisos: chmod +x automatizacion.sh
# Para ejecutar: ./automatizacion.sh

# Versión 2 (Optimizada con Tiling)
gcc matriz.c ../../matriz_utils/matriz_utils.c -o matriz_sec_opt # NUEVO: Secuencial Optimizado
