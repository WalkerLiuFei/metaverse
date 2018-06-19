/**
 * Copyright (c) 2016-2018 mvs developers
 *
 * This file is part of metaverse-explorer.
 *
 * metaverse-explorer is free software: you can redistribute it and/or
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

#include <metaverse/explorer/json_helper.hpp>
#include <metaverse/explorer/dispatch.hpp>
#include <metaverse/explorer/extensions/commands/send.hpp>
#include <metaverse/explorer/extensions/command_extension_func.hpp>
#include <metaverse/explorer/extensions/command_assistant.hpp>
#include <metaverse/explorer/extensions/exception.hpp>
#include <metaverse/explorer/extensions/base_helper.hpp>

namespace libbitcoin {
namespace explorer {
namespace commands {


console_result send::invoke(Json::Value& jv_output,
    libbitcoin::server::server_node& node)
{
    auto& blockchain = node.chain_impl();
    auto acc = blockchain.is_account_passwd_valid(auth_.name, auth_.auth);
    if (!blockchain.is_valid_address(argument_.address))
        throw address_invalid_exception{std::string("invalid address : ") + argument_.address};

    // receiver
    std::vector<receiver_record> receiver{
        {argument_.address, "", argument_.amount, 0, utxo_attach_type::etp, attachment()}
    };
    if(!argument_.memo.empty())
        receiver.push_back({argument_.address, "", 0, 0, utxo_attach_type::message, attachment(0, 0, blockchain_message(argument_.memo))});
    auto send_helper = sending_etp(*this, blockchain, std::move(auth_.name), std::move(auth_.auth),
            "", std::move(receiver), argument_.fee);

    send_helper.exec();

    // json output
    auto tx = send_helper.get_transaction();
    jv_output =  config::json_helper(get_api_version()).prop_tree(tx, true);

    if(!argument_.remark.empty()) {
        blockchain.store_account_remark(acc->get_name(), tx.hash(), argument_.remark);
    }

    return console_result::okay;
}


} // namespace commands
} // namespace explorer
} // namespace libbitcoin

