#include <retesteth/configs/Options.h>
using namespace std;
using namespace dataobject;

namespace retesteth::options
{
string const ethereumjs_config = R"({
    "name":"EthereumJS Transition Tool",
    "socketType":"tranition-tool",
    "socketAddress":"start.sh",
    "initializeTime" : "5",
    "calculateBasefee" : true,
    "forks":[
       "Frontier",
       "Homestead",
       "EIP150",
       "EIP158",
       "Byzantium",
       "Constantinople",
       "ConstantinopleFix",
       "Istanbul",
       "Berlin",
       "London",
       "Merge"
    ],
    "additionalForks":[
       "FrontierToHomesteadAt5",
       "HomesteadToEIP150At5",
       "EIP158ToByzantiumAt5",
       "HomesteadToDaoAt5",
       "ByzantiumToConstantinopleFixAt5",
       "BerlinToLondonAt5",
       "ArrowGlacier"
    ],
    "fillerSkipForks" : [
        "Merge+3540+3670",
        "Merge+3860",
        "Merge+3855",
        "MergeToShanghaiAt5",
        "Shanghai"
    ],
    "exceptions":{
       "AddressTooShort":"",
       "AddressTooLong":"",
       "NonceMax":"",
       "NonceTooLong":"",
       "InvalidVRS":"",
       "InvalidV":"",
       "InvalidR":"",
       "InvalidS":"",
       "InvalidChainID":"",
       "ECRecoveryFail":"",
       "InvalidStateRoot":"",
       "ExtraDataTooBig":"",
       "InvalidData":"",
       "InvalidDifficulty":"",
       "InvalidDifficulty2":"",
       "InvalidDifficulty_TooLarge":"",
       "InvalidGasLimit":"",
       "InvalidGasLimit2":"",
       "InvalidGasLimit3":"",
       "InvalidGasLimit4":"",
       "InvalidGasLimit5":"",
       "InvalidValue":"",
       "InvalidGasPrice":"",
       "InvalidMaxPriorityFeePerGas":"",
       "InvalidMaxFeePerGas":"",
       "InvalidNonce":"",
       "InvalidTo":"",
       "GasLimitPriceProductOverflow":"",
       "TooMuchGasUsed":"",
       "TooMuchGasUsed2":"",
       "LeadingZerosGasLimit":"",
       "LeadingZerosGasPrice":"",
       "LeadingZerosValue":"",
       "LeadingZerosNonce":"",
       "LeadingZerosR":"",
       "LeadingZerosS":"",
       "LeadingZerosV":"",
       "LeadingZerosDataSize":"",
       "LeadingZerosNonceSize":"",
       "InvalidNumber":"",
       "InvalidTimestampEqualParent":"",
       "InvalidTimestampOlderParent":"",
       "InvalidLogBloom":"",
       "InvalidGasUsed":"",
       "InvalidGasUsed2":"",
       "InvalidBlockMixHash":"",
       "InvalidBlockNonce":"",
       "UnknownParent":"",
       "UnknownParent2":"",
       "InvalidReceiptsStateRoot":"",
       "InvalidTransactionsRoot":"",
       "InvalidUnclesHash":"",
       "InvalidUncleParentHash":"",
       "UncleInChain":"",
       "UncleIsAncestor":"",
       "UncleParentIsNotAncestor":"",
       "TooManyUncles":"",
       "UncleIsBrother":"",
       "OutOfGas":"",
       "SenderNotEOA":"",
       "IntrinsicGas":"",
       "ExtraDataIncorrectDAO":"",
       "InvalidTransactionVRS":"",
       "BLOCKHEADER_VALUE_TOOLARGE":"",
       "TRANSACTION_VALUE_TOOLARGE":"",
       "TRANSACTION_VALUE_TOOSHORT":"",
       "TR_NonceHasMaxValue":"",
       "OVERSIZE_RLP":"",
       "RLP_TooFewElements":"",
       "RLP_TooManyElements":"",
       "RLP_InputContainsMoreThanOneValue":"",
       "RLP_VALUESIZE_MORE_AVAILABLEINPUTLENGTH":"",
       "RLP_ELEMENT_LARGER_CONTAININGLIST_UNDERSIZE":"",
       "RLP_ELEMENT_LARGER_CONTAININGLIST_OVERSIZE":"",
       "RLP_ExpectedInputList_EXTBLOCK":"",
       "RLP_InvalidArg0_UNMARSHAL_BYTES":"",
       "RLP_ExpectedInputList_HEADER_DECODEINTO_BLOCK_EXTBLOCK":"",
       "RLP_InputList_TooManyElements_HEADER_DECODEINTO_BLOCK_EXTBLOCK_HEADER":"",
       "RLP_InputList_TooManyElements_TXDATA_DECODEINTO_BLOCK_EXTBLOCK_TXS0":"",
       "RLP_InputString_TooShort_ADDRESS_DECODEINTO_BLOCK_EXTBLOCK_HEADER_COINBASE":"",
       "RLP_InputString_TooShort_ADDRESS_DECODEINTO_BLOCK_EXTBLOCK_HEADER_COINBASE2":"",
       "RLP_InputString_TooShort_ADDRESS_DECODEINTO_BLOCK_EXTBLOCK_TXS0_RECIPIENT":"",
       "RLP_InputString_TooLong_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_ROOT":"",
       "RLP_InputString_TooLong_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_ROOT2":"",
       "RLP_InputString_TooLong_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_MIXDIGEST":"",
       "RLP_InputString_TooLong_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_MIXDIGEST2":"",
       "RLP_InputString_TooLong_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_PARENTHASH":"",
       "RLP_InputString_TooLong_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_PARENTHASH2":"",
       "RLP_InputString_TooLong_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_RECEIPTHASH":"",
       "RLP_InputString_TooLong_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_RECEIPTHASH2":"",
       "RLP_InputString_TooLong_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_TXHASH":"",
       "RLP_InputString_TooLong_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_UNCLEHASH":"",
       "RLP_InputString_TooLong_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_UNCLEHASH2":"",
       "RLP_InputString_TooLong_UINT64_DECODEINTO_BLOCK_EXTBLOCK_HEADER_GASLIMIT":"",
       "RLP_InputString_TooLong_UINT64_DECODEINTO_BLOCK_EXTBLOCK_HEADER_GASUSED":"",
       "RLP_InputString_TooLong_UINT64_DECODEINTO_BLOCK_EXTBLOCK_HEADER_TIME":"",
       "RLP_InputString_TooLong_UINT64_DECODEINTO_BLOCK_EXTBLOCK_TXS0_GASLIMIT":"",
       "RLP_InputString_TooShort_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_RECEIPTHASH":"",
       "RLP_InputString_TooShort_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_RECEIPTHASH2":"",
       "RLP_InputString_TooShort_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_ROOT":"",
       "RLP_InputString_TooShort_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_MIXDIGEST":"",
       "RLP_InputString_TooShort_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_MIXDIGEST2":"",
       "RLP_InputString_TooShort_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_PARENTHASH":"",
       "RLP_InputString_TooShort_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_PARENTHASH2":"",
       "RLP_InputString_TooShort_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_UNCLEHASH":"",
       "RLP_InputString_TooShort_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_UNCLEHASH2":"",
       "RLP_InputString_TooShort_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_TXHASH":"",
       "RLP_InputString_TooShort_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_TXHASH2":"",
       "RLP_InputString_TooShort_BLOOM_DECODEINTO_BLOCK_EXTBLOCK_HEADER_BLOOM":"",
       "RLP_NonCanonicalINT_LeadingZeros_BIGINT_DECODEINTO_BLOCK_EXTBLOCK_HEADER_DIFFICULTY":"",
       "RLP_NonCanonicalINT_LeadingZeros_BIGINT_DECODEINTO_BLOCK_EXTBLOCK_HEADER_DIFFICULTY2":"",
       "RLP_NonCanonicalINT_LeadingZeros_UINT64_DECODEINTO_BLOCK_EXTBLOCK_HEADER_GASLIMIT":"",
       "RLP_NonCanonicalINT_LeadingZeros_UINT64_DECODEINTO_BLOCK_EXTBLOCK_HEADER_GASLIMIT2":"",
       "RLP_NonCanonicalINT_LeadingZeros_UINT64_DECODEINTO_BLOCK_EXTBLOCK_HEADER_GASUSED":"",
       "RLP_NonCanonicalINT_LeadingZeros_UINT64_DECODEINTO_BLOCK_EXTBLOCK_HEADER_GASUSED2":"",
       "RLP_NonCanonicalINT_LeadingZeros_UINT64_DECODEINTO_BLOCK_EXTBLOCK_HEADER_TIME":"",
       "RLP_NonCanonicalINT_LeadingZeros_UINT64_DECODEINTO_BLOCK_EXTBLOCK_HEADER_TIME2":"",
       "RLP_NonCanonicalINT_LeadingZeros_UINT64_DECODEINTO_BLOCK_EXTBLOCK_TXS0_GASLIMIT":"",
       "RLP_NonCanonicalINT_LeadingZeros_UINT64_DECODEINTO_BLOCK_EXTBLOCK_TXS0_GASLIMIT2":"",
       "RLP_NonCanonicalINT_LeadingZeros_BIGINT_DECODEINTO_BLOCK_EXTBLOCK_HEADER_NUMBER":"",
       "RLP_NonCanonicalINT_LeadingZeros_BIGINT_DECODEINTO_BLOCK_EXTBLOCK_HEADER_NUMBER2":"",
       "RLP_NonCanonicalINT_LeadingZeros_BIGINT_DECODEINTO_BLOCK_EXTBLOCK_TXS0_TXDATA_PRICE":"",
       "RLP_NonCanonicalINT_LeadingZeros_BIGINT_DECODEINTO_BLOCK_EXTBLOCK_TXS0_TXDATA_R":"",
       "RLP_NonCanonicalINT_LeadingZeros_BIGINT_DECODEINTO_BLOCK_EXTBLOCK_TXS0_TXDATA_S":"",
       "RLP_InputString_TooLong_BLOOM_DECODEINTO_BLOCK_EXTBLOCK_HEADER_BLOOM":"",
       "RLP_ExpectedInputString_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_PARENTHASH":"",
       "RLP_ExpectedInputString_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_RECEIPTHASH":"",
       "RLP_ExpectedInputString_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_ROOT":"",
       "RLP_ExpectedInputString_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_MIXDIGEST":"",
       "RLP_ExpectedInputString_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_TXHASH":"",
       "RLP_ExpectedInputString_HASH_DECODEINTO_BLOCK_EXTBLOCK_HEADER_UNCLEHASH":"",
       "RLP_ExpectedInputString_ADDRESS_DECODEINTO_BLOCK_EXTBLOCK_HEADER_COINBASE":"",
       "RLP_ExpectedInputString_ADDRESS_DECODEINTO_BLOCK_EXTBLOCK_TX0_RECIPIENT":"",
       "RLP_InputString_TooLong_ADDRESS_DECODEINTO_BLOCK_EXTBLOCK_HEADER_COINBASE":"",
       "RLP_InputString_TooLong_ADDRESS_DECODEINTO_BLOCK_EXTBLOCK_TXS0_RECIPIENT":"",
       "RLP_ExpectedInputString_BIGINT_DECODEINTO_BLOCK_EXTBLOCK_HEADER_DIFFICULTY":"",
       "RLP_ExpectedInputString_BIGINT_DECODEINTO_BLOCK_EXTBLOCK_TXS0_TXR":"",
       "RLP_ExpectedInputString_BIGINT_DECODEINTO_BLOCK_EXTBLOCK_TXS0_TXS":"",
       "RLP_ExpectedInputString_UINT64_DECODEINTO_BLOCK_EXTBLOCK_HEADER_GASLIMIT":"",
       "RLP_ExpectedInputString_UINT64_DECODEINTO_BLOCK_EXTBLOCK_HEADER_GASUSED":"",
       "RLP_ExpectedInputString_UINT64_DECODEINTO_BLOCK_EXTBLOCK_HEADER_TIME":"",
       "RLP_ExpectedInputString_UINT64_DECODEINTO_BLOCK_EXTBLOCK_TXS0_GASLIMIT":"",
       "RLP_ExpectedInputString_NONCE_DECODEINTO_BLOCK_EXTBLOCK_HEADER_NONCE":"",
       "RLP_ExpectedInputString_UINT8_DECODEINTO_BLOCK_EXTBLOCK_TXS0_PAYLOAD":"",
       "RLP_InputString_TooLong_BLOCKNONCE_DECODEINTO_BLOCK_EXTBLOCK_HEADER_NONCE":"",
       "RLP_InputString_TooLong_BLOCKNONCE_DECODEINTO_BLOCK_EXTBLOCK_HEADER_NONCE2":"",
       "RLP_NonCanonical_SizeInfo_EXTBLOCK":"",
       "RLP_ExpectedInputList_TRANSACTION_DECODEINTO_BLOCK_EXTBLOCK_TXS":"",
       "RLP_ExpectedInputList_HEADER_DECODEINTO_BLOCK_EXTBLOCK_UNCLES":"",
       "RLP_ExpectedInputList_TXDATA_DECODEINTO_BLOCK_EXTBLOCK_TXS0":"",
       "RLP_Error_EOF":"",
       "RLP_Error_RLP_Size":"",
       "RLP_Error_Size_Information":"",
       "LegacyBlockImportImpossible":"",
       "LegacyBlockImportImpossible2":"",
       "LegacyBlockBaseFeeTransaction":"",
       "1559BlockImportImpossible_HeaderIsLegacy":"",
       "1559BlockImportImpossible_BaseFeeWrong":"",
       "1559BlockImportImpossible_InitialBaseFeeWrong":"",
       "1559BlockImportImpossible_TargetGasLow":"",
       "1559BlockImportImpossible_TargetGasHigh":"",
       "1559BlockImportImpossible_InitialGasLimitInvalid":"",
       "TR_IntrinsicGas":"",
       "TR_NoFunds":"",
       "TR_NoFundsValue":"",
       "TR_FeeCapLessThanBlocks":"",
       "TR_GasLimitReached":"",
       "TR_NonceTooHigh":"",
       "TR_NonceTooLow":"",
       "TR_TypeNotSupported":"",
       "TR_TipGtFeeCap":"",
       "TR_TooShort":"",
       "1559BaseFeeTooLarge":"",
       "1559PriorityFeeGreaterThanBaseFee":"",
       "2930AccessListAddressTooLong":"",
       "2930AccessListAddressTooShort":"",
       "2930AccessListStorageHashTooLong":"",
       "1559LeadingZerosBaseFee":"",
       "1559LeadingZerosPriorityFee":"",
       "2930AccessListStorageHashTooShort":"",
       "3675PoWBlockRejected":"",
       "3675PoSBlockRejected":"",
       "3675PreMerge1559BlockRejected":""
    }
})";

string const ethereumjs_start = R"(#!/bin/sh
curl -X POST -d "${1} ${2} ${3} ${4} ${5} ${6} ${7} ${8} ${9} ${10} ${11} ${12} ${13} ${14} ${15} ${16} ${17} ${18} ${19} ${20}" --silent http://localhost:3000/
)";
string const ethereumjs_setup = R"(#!/bin/sh
dir=$(pwd)
cd $ETHEREUMJS_PATH/packages/vm
npx ts-node test/retesteth/transition-cluster.ts &> /dev/null &
cd $dir
)";
string const ethereumjs_stop = R"(#!/bin/sh
killall node
)";

genethereumjscfg::genethereumjscfg()
{
    {
        spDataObject obj;
        (*obj)["path"] = "ethereumjs/config";
        (*obj)["content"] = ethereumjs_config;
        map_configs.addArrayObject(obj);
    }
    {
        spDataObject obj;
        (*obj)["exec"] = true;
        (*obj)["path"] = "ethereumjs/setup.sh";
        (*obj)["content"] = ethereumjs_setup;
        map_configs.addArrayObject(obj);
    }
    {
        spDataObject obj;
        (*obj)["exec"] = true;
        (*obj)["path"] = "ethereumjs/start.sh";
        (*obj)["content"] = ethereumjs_start;
        map_configs.addArrayObject(obj);
    }
    {
        spDataObject obj;
        (*obj)["exec"] = true;
        (*obj)["path"] = "ethereumjs/stop.sh";
        (*obj)["content"] = ethereumjs_stop;
        map_configs.addArrayObject(obj);
    }
}
}  // namespace retesteth::options
