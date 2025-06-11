cmake_minimum_required(VERSION 3.30)
project(UnstebleStudent C)

set(CMAKE_C_STANDARD 99)

add_executable(UnstebleStudent main.c caricaMazzo.c preparazione.c chiusura.c Turno.c effetti.c liste.c ANSI.h salvataggio.c turnoIA.c
        statistiche.c
        funzioni.c logger.c
)
