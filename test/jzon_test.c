#include "../client/libs/jzon/Jzon.h"
#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

int main () {
    Jzon::Node node = Jzon::object ();
    node.add ("name", "value");
    node.add ("number", 20);
    node.add ("anothernumber", 15.3);
    node.add ("bool", true);

    {
        Jzon::Node array = Jzon::array ();
        array.add (1);
        array.add ("asdf");

        {
            Jzon::Node array_node = Jzon::object ();
            array_node.add ("key1", "val1");
            array_node.add ("key2", "val2");
            array.add (array_node);
        }

        node.add ("array", array);
    }

    {
        Jzon::Node subnode = Jzon::object ();
        subnode.add ("key1", "val1");
        subnode.add ("key2", "val2");
        node.add ("subnode", subnode);
    }

    Jzon::Writer writer;
    writer.writeStream (node, cout);
    return 0;
}
