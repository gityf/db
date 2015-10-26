/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The source file of class GdbmViewer.
*/
#include <iostream>
#include "gdbmdb.hpp"
#include "log/log.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Usage:" << argv[0] << " dbfile" << endl;
        return 0;
    }

    std::map<string, string> keyVals;

    GdbmDB gdbmDb;
    gdbmDb.open(argv[1]);
    gdbmDb.dump(keyVals);
    for (std::map<string, string>::iterator it = keyVals.begin();
        it != keyVals.end(); ++it) {
        cout << "key:"   << it->first
             << ", value:" << it->second << endl;
    }
    return 0;
}