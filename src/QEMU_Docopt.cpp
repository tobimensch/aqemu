#include <iostream>

#include "QEMU_Docopt.h"

void parse_qemu( int argc, char *argv[] )
{
    QString version = QString("qemu ") + "2.0";
    std::map<std::string, docopt::value> args
        = docopt::docopt(QEMU_USAGE,
                         { argv + 1, argv + argc },
                         true,               // show help if requested
                          qPrintable( version ) );  // version string

    for(auto const& arg : args) {
        std::cout << arg.first <<  arg.second << std::endl;
    }
}
