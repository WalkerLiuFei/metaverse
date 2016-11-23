/**
 * Copyright (c) 2011-2015 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin-explorer.
 *
 * libbitcoin-explorer is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option)
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <bitcoin/explorer/commands/input-sign.hpp>

#include <iostream>
#include <cstdint>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/explorer/utility.hpp>

using namespace bc;
using namespace bc::chain;
using namespace bc::config;
using namespace bc::explorer;
using namespace bc::explorer::commands;

// The BX_INPUT_SIGN_FAILED condition uncovered by test.
// This is because a vector to produce the failure is not known.
console_result input_sign::invoke(std::ostream& output, std::ostream& error)
{
    // Bound parameters.
    const auto anyone_can_pay = get_anyone_option();
    const auto index = get_index_option();
    const auto sign_type = get_sign_type_option();
    const tx_type& tx = get_transaction_argument();
    const ec_secret& private_key = get_ec_private_key_argument();
    const chain::script& contract = get_contract_argument();

    if (index >= tx.inputs.size())
    {
        error << BX_INPUT_SIGN_INDEX_OUT_OF_RANGE << std::flush;
        return console_result::failure;
    }

    uint8_t hash_type = (signature_hash_algorithm)sign_type;
    if (anyone_can_pay)
        hash_type |= signature_hash_algorithm::anyone_can_pay;

    endorsement endorse;
    if (!chain::script::create_endorsement(endorse, private_key,
        contract, tx, index, hash_type))
    {
        error << BX_INPUT_SIGN_FAILED << std::flush;
        return console_result::failure;
    }

    output << base16(endorse) << std::flush;
    return console_result::okay;
}
