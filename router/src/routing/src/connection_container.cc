/*
  Copyright (c) 2018, 2023, Oracle and/or its affiliates.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License, version 2.0,
  as published by the Free Software Foundation.

  This program is also distributed with certain software (including
  but not limited to OpenSSL) that is licensed under separate terms,
  as designated in a particular file or component or in included license
  documentation.  The authors of MySQL hereby grant you an additional
  permission to link the program and your derivative works with the
  separately licensed software that they have included with MySQL.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "connection_container.h"
#include "mysql/harness/logging/logging.h"

IMPORT_LOG_FUNCTIONS()

void ConnectionContainer::add_connection(
    std::shared_ptr<MySQLRoutingConnectionBase> connection) {
  connections_.put(connection.get(), std::move(connection));
}

unsigned ConnectionContainer::disconnect(const AllowedNodes &nodes) {
  unsigned number_of_disconnected_connections = 0;

#if 0
  for (const auto &allowed_node : nodes) {
    std::cerr << "allowed: " << allowed_node.address.str() << "\n";
  }
#endif

  auto mark_to_disconnect_if_not_allowed =
      [&allowed_nodes = nodes,
       &number_of_disconnected_connections](auto &connection_element) {
        auto *conn = connection_element.first;

        auto conn_ro_dest_id = conn->read_only_destination_id();
        auto conn_rw_dest_id = conn->read_write_destination_id();
#if 0
        std::cerr << "conn: "
                  << "ro: " << conn_ro_dest_id << ", "
                  << "rw: " << conn_rw_dest_id << "\n";
#endif

        bool ro_allowed{conn_ro_dest_id.empty()};
        bool rw_allowed{conn_rw_dest_id.empty()};

        for (const auto &allowed_node : allowed_nodes) {
          auto allowed_dest_id = allowed_node.address.str();

          if (allowed_dest_id == conn_ro_dest_id) ro_allowed = true;
          if (allowed_dest_id == conn_rw_dest_id) rw_allowed = true;

          // both are allowed.
          if (ro_allowed && rw_allowed) return;
        }

        const auto server_address = conn->get_server_address();
        const auto client_address = conn->get_client_address();

        log_info("Disconnecting client %s from server %s",
                 client_address.c_str(), server_address.c_str());
        conn->disconnect();

        ++number_of_disconnected_connections;
      };

  connections_.for_each(mark_to_disconnect_if_not_allowed);
#if 0
  std::cerr << "marked: " << number_of_disconnected_connections << "\n";
#endif

  return number_of_disconnected_connections;
}

MySQLRoutingConnectionBase *ConnectionContainer::get_connection(
    const std::string &client_endpoint) {
  MySQLRoutingConnectionBase *ret = nullptr;

  auto lookup = [&ret, &client_endpoint](auto &connection) {
    if (ret) return;
    const auto client_address = connection.first->get_client_address();
    if (client_address == client_endpoint) {
      ret = connection.first;
    }
  };

  connections_.for_each(lookup);

  return ret;
}

void ConnectionContainer::disconnect_all() {
  connections_.for_each(
      [](const auto &connection) { connection.first->disconnect(); });
}

void ConnectionContainer::remove_connection(
    MySQLRoutingConnectionBase *connection) {
  std::unique_lock<std::mutex> lk(connection_removed_cond_m_);

  connections_.erase(connection);

  connection_removed_cond_.notify_all();
}
