#include <retesteth/configs/Options.h>
using namespace std;
using namespace dataobject;

namespace retesteth::options
{
const string default_MergeToShanghaiAt5_config = R"({
    "params" : {
       "homesteadForkBlock" : "0x00",
       "EIP150ForkBlock" : "0x00",
       "EIP158ForkBlock" : "0x00",
       "byzantiumForkBlock" : "0x00",
       "constantinopleForkBlock" : "0x00",
       "constantinopleFixForkBlock" : "0x00",
       "istanbulForkBlock" : "0x00",
       "berlinForkBlock" : "0x00",
       "londonForkBlock" : "0x00",
       "arrowGlacierForkBlock" : "0x00",
       "terminalTotalDifficulty" : "0x00",
       "shanghaiForkBlock" : "0x05",
       "chainID" : "0x01"
    },
    "accounts" : {
    }
})";

const string t8ntool_MergeToShanghaiAt5_config = R"({
    "params" : {
        "fork" : "MergeToShanghaiAt5",
        "constantinopleForkBlock" : "0x00",
        "byzantiumForkBlock" : "0x00",
        "homesteadForkBlock" : "0x00",
        "terminalTotalDifficulty" : "0x00",
        "chainID" : "0x01"
    },
    "accounts" : {
    }
})";

genMergeToShanghaiAt5Cfg::genMergeToShanghaiAt5Cfg()
{
    spDataObject obj(new DataObject());
    (*obj)["path"] = "besu/genesis/MergeToShanghaiAt5.json";
    (*obj)["content"] = default_MergeToShanghaiAt5_config;
    map_configs.addArrayObject(obj);

    spDataObject obj2(new DataObject());
    (*obj2)["path"] = "default/genesis/MergeToShanghaiAt5.json";
    (*obj2)["content"] = t8ntool_MergeToShanghaiAt5_config;
    map_configs.addArrayObject(obj2);
}
}  // namespace retesteth::options
