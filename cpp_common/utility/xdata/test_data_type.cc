#include "xdata.h"
#include "filesource.h"



static bool _write_test()
{
    XData data;

    XData::AttachToFile(data, "./xdata.txt", XData::SerializeOperation::W);

    data["abcd"] = "cc";
    data["1"]["2"]["3"]["4"]["5"]["6"] = 1;

    XData sub;
    sub["a"] = 123;
    sub["b"].append("a");

    data["key"] = std::move(sub);

    return 0;
}

static bool _read_test()
{
    XData data;
    XData::AttachToFile(data, "./xdata.txt", XData::SerializeOperation::RW);

    
    printf("%s\n", data["abcd"].as_string());
    printf("%d\n", data["1"]["2"]["3"]["4"]["5"]["6"].as_int());
    printf("%d\n", data["key"]["a"].as_int());
    printf("%s\n", data["key"]["b"].first_child()->as_string());
    
    return true;
}



int main()
{
    //_read_test();
    _write_test();

    return 0;
}

