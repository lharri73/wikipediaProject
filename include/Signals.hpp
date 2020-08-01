#pragma once

#include <execinfo.h>
#include <glob.h>
#include <cstdio>
#include <signal.h>
#include <string>
#include <unistd.h>

void handler(int sig);
void signal_handler(int signal);
void cleanup();
