-- Copyright (c) 2014, 2023, Oracle and/or its affiliates.
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License, version 2.0,
-- as published by the Free Software Foundation.
--
-- This program is designed to work with certain software (including
-- but not limited to OpenSSL) that is licensed under separate terms,
-- as designated in a particular file or component or in included license
-- documentation.  The authors of MySQL hereby grant you an additional
-- permission to link the program and your derivative works with the
-- separately licensed software that they have either included with
-- the program or referenced in the documentation.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License, version 2.0, for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA

DROP PROCEDURE IF EXISTS ps_setup_disable_consumer;

DELIMITER $$

CREATE DEFINER='mysql.sys'@'localhost' PROCEDURE ps_setup_disable_consumer (
        IN consumer VARCHAR(128)
    )
    COMMENT '
Description
-----------

Disables consumers within Performance Schema 
matching the input pattern.

Parameters
-----------

consumer (VARCHAR(128)):
  A LIKE pattern match (using "%consumer%") of consumers to disable

Example
-----------

To disable all consumers:

mysql> CALL sys.ps_setup_disable_consumer(\'\');
+--------------------------+
| summary                  |
+--------------------------+
| Disabled 15 consumers    |
+--------------------------+
1 row in set (0.02 sec)

To disable just the event_stage consumers:

mysql> CALL sys.ps_setup_disable_comsumers(\'stage\');
+------------------------+
| summary                |
+------------------------+
| Disabled 3 consumers   |
+------------------------+
1 row in set (0.00 sec)
'
    SQL SECURITY INVOKER
    NOT DETERMINISTIC
    MODIFIES SQL DATA
BEGIN
    UPDATE performance_schema.setup_consumers
       SET enabled = 'NO'
     WHERE name LIKE CONCAT('%', consumer, '%');

    SELECT CONCAT('Disabled ', @rows := ROW_COUNT(), ' consumer', IF(@rows != 1, 's', '')) AS summary;
END$$

DELIMITER ;
