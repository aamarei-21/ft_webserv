#pragma once

#include <iostream>
#include "Request.hpp"
#include <fstream>
#include <exception>
#include "Config.hpp"
#include "Worker.hpp"
#include "CGIprocess.hpp"
#include "Response.hpp"
#include "Cache.hpp"
#include "utils.hpp"
#include "pages.hpp"

class Tests {
    Config config;
    Worker worker;
public:
    Tests(Config &conf, char **env): config(conf), worker(conf, env) {}
    void test_request();
    void test_cgj();
    void test_cgj2();
    void test_cache();
    void test_autoindex();
    void test_getPair();
};
