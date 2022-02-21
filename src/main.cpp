#include "MainProc.hpp"
// #include "Tests.hpp"

// void tests(Config &conf, char **env) {
//     std::cout << "********* Start tests *********" << std::endl;
//     Tests tester(conf, env);
//     tester.test_request();
//     tester.test_getPair();
//     tester.test_cgj();
//     // tester.test_cgj2();
//     tester.test_cache();
//     tester.test_autoindex();
//     std::cout << "\n****************************\n" << std::endl;
// }

int main(int ac, char **av, char **env) {

    std::cout << "Start parse config" << std::endl;
    Config conf;
    try {
        conf.parse_config(ac, av);
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        std::cerr << "Program Web Server closed with error Parse config\n";
        return 1;
    }

    // tests(conf, env);

    std::cout << "Start servers init" << std::endl;
    MainProc serv_pool(conf, env);
    serv_pool.serv_init();

    int run_f = 1;

    std::cout << "Start servers (For Exit press - Ctrl+C)" << std::endl;
    serv_pool.main_loop(run_f);

    // Убираем за собой
    serv_pool.sockets_close();

    std::cout << "Program Web Server closed\n";

    return 0;
}
