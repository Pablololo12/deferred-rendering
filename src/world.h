#pragma once

extern float world_ph;
extern float world_th;
extern float world_ro;

extern bool world_fill;

void world_init();
void world_reshape(int w,int h);
void world_display(int w,int h);
void world_clean();
