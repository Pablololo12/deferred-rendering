#pragma once

extern float world_ph;
extern float world_th;
extern float world_ro;

extern bool world_fill;

void world_init(int,int);
void world_reshape(int,int);
void world_display(int,int,int,int,int);
void world_clean();
