#include "EthereumBlock.h"
#include <libdevcore/CommonIO.h>
#include <libdevcore/SHA3.h>
using namespace std;
using namespace dev;

namespace test::teststruct
{
void EthereumBlock::recalculateUncleHash()
{
    // Transaction has is calculated from transaction trie, not from list
    // Transaction List
    /*
    RLPStream transactionList(m_transactions.size());
    for (auto const& tr : m_transactions)
        transactionList.appendRaw(tr.asRLPStream().out());
    */

    // Uncle List
    RLPStream uncleList(m_uncles.size());
    for (auto const& un : m_uncles)
        uncleList.appendRaw(un->asRLPStream().out());

    // FH32 newTxHash("0x" + toString(dev::sha3(transactionList.out())));
    // m_header.getContent().setTransactionHash(newTxHash);

    FH32 newUnHash("0x" + dev::toString(dev::sha3(uncleList.out())));
    m_header.getContent().setUnclesHash(newUnHash);

    FH32 newHeaderHash("0x" + dev::toString(dev::sha3(m_header->asRLPStream().out())));
    m_header.getContent().setHeaderHash(newHeaderHash);
}

BYTES const EthereumBlock::getRLP() const
{
    try
    {
        // RLP of a block
        bool const isShanghai =  (m_header->type() == BlockType::BlockHeaderShanghai || m_forceWithdrawalsRLP)
                                && !m_forceNoWithdrawalsRLP;
        RLPStream stream(isShanghai ? 4 : 3);
        stream.appendRaw(m_header->asRLPStream().out());

        // Transaction list
        RLPStream transactionList(m_transactions.size());
        for (auto const& tr : m_transactions)
            transactionList.appendRaw(tr->asRLPStream().out());
        stream.appendRaw(transactionList.out());

        // Uncle list
        RLPStream uncleList(m_uncles.size());
        for (auto const& un : m_uncles)
            uncleList.appendRaw(un->asRLPStream().out());
        stream.appendRaw(uncleList.out());

        // Withdrawals
        if (isShanghai)
        {
            RLPStream withdrawalsList(m_withdrawals.size());
            for (auto const& wt : m_withdrawals)
                withdrawalsList.appendRaw(wt->asRLPStream().out());
            stream.appendRaw(withdrawalsList.out());
        }

        return BYTES(dev::toHexPrefixed(stream.out()));
    }
    catch (std::exception const& _ex)
    {
        ETH_ERROR_MESSAGE(string("EthereumBlock::getRLP() ") + _ex.what());
    }
    return BYTES(DataObject());
}

DebugVMTrace const& EthereumBlockState::getTrTrace(FH32 const& _hash) const
{
    if (m_transactionsTrace.count(_hash))
        return m_transactionsTrace.at(_hash);
    else
        ETH_WARNING("Transaction trace not found! (" + _hash.asString() + ")");
    static DebugVMTrace empty("", "", FH32::zero(), "");
    return empty;
}

}  // namespace teststruct
