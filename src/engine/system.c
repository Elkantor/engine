#pragma once

typedef struct app app_t;

int appKickstart(int _argc, char** _argv);
void appUpdate(app_t* _app);
void appStart(app_t* _app);
void appStop(app_t* _app);
void appInit(app_t* _app, const char* _name, const int _width, const int _height);
