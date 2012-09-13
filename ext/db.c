
/*
  +------------------------------------------------------------------------+
  | Phalcon Framework                                                      |
  +------------------------------------------------------------------------+
  | Copyright (c) 2011-2012 Phalcon Team (http://www.phalconphp.com)       |
  +------------------------------------------------------------------------+
  | This source file is subject to the New BSD License that is bundled     |
  | with this package in the file docs/LICENSE.txt.                        |
  |                                                                        |
  | If you did not receive a copy of the license and are unable to         |
  | obtain it through the world-wide-web, please send an email             |
  | to license@phalconphp.com so we can send you a copy immediately.       |
  +------------------------------------------------------------------------+
  | Authors: Andres Gutierrez <andres@phalconphp.com>                      |
  |          Eduar Carvajal <eduar@phalconphp.com>                         |
  +------------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_phalcon.h"
#include "phalcon.h"

#include "Zend/zend_operators.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_interfaces.h"

#include "kernel/main.h"
#include "kernel/memory.h"

#include "kernel/operators.h"
#include "kernel/object.h"
#include "kernel/array.h"
#include "kernel/concat.h"
#include "kernel/fcall.h"
#include "kernel/exception.h"

/**
 * Phalcon\Db
 *
 * Phalcon\Db and its related classes provide a simple SQL database interface for Phalcon Framework.
 * The Phalcon\Db is the basic class you use to connect your PHP application to an RDBMS.
 * There is a different adapter class for each brand of RDBMS.
 *
 * This component is intended to lower level database operations. If you want to interact with databases using
 * higher level of abstraction use Phalcon\Mvc\Model.
 *
 * Phalcon\Db is an abstract class. You only can use it with a database adapter like Phalcon\Db\Adapter\Pdo
 *
 * <code>
 *
 *try {
 *
 *  $connection = new Phalcon\Db\Adapter\Pdo\Mysql(array(
 *     'host' => '192.168.0.11',
 *     'username' => 'sigma',
 *     'password' => 'secret',
 *     'dbname' => 'blog',
 *     'port' => '3306',
 *  ));
 *
 *  $result = $connection->query("SELECT * FROM robots LIMIT 5");
 *  $result->setFetchMode(Phalcon\Db::FETCH_NUM);
 *  while($robot = $result->fetchArray()){
 *    print_r($robot);
 *  }
 *
 *} catch(Phalcon\Db\Exception $e){
 *	echo $e->getMessage(), PHP_EOL;
 *}
 *
 * </code>
 */


/**
 * Phalcon\Db constructor
 *
 * @param array $descriptor
 */
PHP_METHOD(Phalcon_Db, __construct){

	zval *descriptor = NULL, *one = NULL, *connection_consecutive = NULL;
	zval *next_consecutive = NULL, *dialect_type = NULL, *dialect_class = NULL;
	zval *dialect_object = NULL;
	int eval_int;
	zend_class_entry *ce0;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &descriptor) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(one);
	ZVAL_LONG(one, 1);
	PHALCON_OBSERVE_VAR(connection_consecutive);
	phalcon_read_static_property(&connection_consecutive, SL("phalcon\\db"), SL("_connectionConsecutive") TSRMLS_CC);
	
	PHALCON_INIT_VAR(next_consecutive);
	phalcon_add_function(next_consecutive, connection_consecutive, one TSRMLS_CC);
	phalcon_update_static_property(SL("phalcon\\db"), SL("_connectionConsecutive"), next_consecutive TSRMLS_CC);
	phalcon_update_property_zval(this_ptr, SL("_connectionId"), connection_consecutive TSRMLS_CC);
	eval_int = phalcon_array_isset_string(descriptor, SL("dialectClass")+1);
	if (!eval_int) {
		PHALCON_INIT_VAR(dialect_type);
		phalcon_read_property(&dialect_type, this_ptr, SL("_dialectType"), PH_NOISY_CC);
		
		PHALCON_INIT_VAR(dialect_class);
		PHALCON_CONCAT_SV(dialect_class, "phalcon\\db\\dialect\\", dialect_type);
	} else {
		PHALCON_INIT_VAR(dialect_class);
		phalcon_array_fetch_string(&dialect_class, descriptor, SL("dialectClass"), PH_NOISY_CC);
	}
	
	ce0 = phalcon_fetch_class(dialect_class TSRMLS_CC);
	
	PHALCON_INIT_VAR(dialect_object);
	object_init_ex(dialect_object, ce0);
	PHALCON_CALL_METHOD_NORETURN(dialect_object, "__construct", PH_CHECK);
	phalcon_update_property_zval(this_ptr, SL("_dialect"), dialect_object TSRMLS_CC);
	phalcon_update_property_zval(this_ptr, SL("_descriptor"), descriptor TSRMLS_CC);
	
	PHALCON_MM_RESTORE();
}

/**
 * Sets the event manager
 *
 * @param Phalcon\Events\Manager $eventsManager
 */
PHP_METHOD(Phalcon_Db, setEventsManager){

	zval *events_manager = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &events_manager) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	phalcon_update_property_zval(this_ptr, SL("_eventsManager"), events_manager TSRMLS_CC);
	
	PHALCON_MM_RESTORE();
}

/**
 * Returns the internal event manager
 *
 * @return Phalcon\Events\Manager
 */
PHP_METHOD(Phalcon_Db, getEventsManager){

	zval *events_manager = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(events_manager);
	phalcon_read_property(&events_manager, this_ptr, SL("_eventsManager"), PH_NOISY_CC);
	
	RETURN_CCTOR(events_manager);
}

/**
 * Returns the first row in a SQL query result
 *
 *<code>
 *	//Getting first robot
 *	$robot = $connection->fecthOne("SELECT * FROM robots");
 *	print_r($robot);
 *
 *	//Getting first robot with associative indexes only
 *	$robot = $connection->fecthOne("SELECT * FROM robots", Phalcon\Db::FETCH_ASSOC);
 *	print_r($robot);
 *</code>
 *
 * @param string $sqlQuery
 * @param int $fetchMode
 * @return array
 */
PHP_METHOD(Phalcon_Db, fetchOne){

	zval *sql_query = NULL, *fetch_mode = NULL, *result = NULL, *row = NULL, *empty_row = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &sql_query, &fetch_mode) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!fetch_mode) {
		PHALCON_ALLOC_ZVAL_MM(fetch_mode);
		ZVAL_LONG(fetch_mode, 2);
	}
	
	PHALCON_INIT_VAR(result);
	PHALCON_CALL_METHOD_PARAMS_1(result, this_ptr, "query", sql_query, PH_NO_CHECK);
	if (Z_TYPE_P(result) == IS_OBJECT) {
		PHALCON_CALL_METHOD_PARAMS_1_NORETURN(result, "setfetchmode", fetch_mode, PH_NO_CHECK);
		
		PHALCON_INIT_VAR(row);
		PHALCON_CALL_METHOD(row, result, "fetcharray", PH_NO_CHECK);
		
		RETURN_CCTOR(row);
	}
	
	PHALCON_INIT_VAR(empty_row);
	array_init(empty_row);
	
	RETURN_CTOR(empty_row);
}

/**
 * Dumps the complete result of a query into an array
 *
 *<code>
 *	//Getting all robots
 *	$robots = $connection->fetchAll("SELECT * FROM robots");
 *	foreach($robots as $robot){
 *		print_r($robot);
 *	}
 *
 *	//Getting all robots with associative indexes only
 *	$robots = $connection->fetchAll("SELECT * FROM robots", Phalcon\Db::FETCH_ASSOC);
 *	foreach($robots as $robot){
 *		print_r($robot);
 *	}
 *</code>
 *
 * @param string $sqlQuery
 * @param int $fetchMode
 * @return array
 */
PHP_METHOD(Phalcon_Db, fetchAll){

	zval *sql_query = NULL, *fetch_mode = NULL, *results = NULL, *result = NULL;
	zval *row = NULL;
	zval *r0 = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &sql_query, &fetch_mode) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!fetch_mode) {
		PHALCON_ALLOC_ZVAL_MM(fetch_mode);
		ZVAL_LONG(fetch_mode, 2);
	}
	
	PHALCON_INIT_VAR(results);
	array_init(results);
	
	PHALCON_INIT_VAR(result);
	PHALCON_CALL_METHOD_PARAMS_1(result, this_ptr, "query", sql_query, PH_NO_CHECK);
	if (Z_TYPE_P(result) == IS_OBJECT) {
		PHALCON_CALL_METHOD_PARAMS_1_NORETURN(result, "setfetchmode", fetch_mode, PH_NO_CHECK);
		ph_cycle_start_0:
			
			PHALCON_INIT_VAR(r0);
			PHALCON_CALL_METHOD(r0, result, "fetcharray", PH_NO_CHECK);
			PHALCON_CPY_WRT(row, r0);
			if (!zend_is_true(row)) {
				goto ph_cycle_end_0;
			}
			phalcon_array_append(&results, row, PH_SEPARATE TSRMLS_CC);
			goto ph_cycle_start_0;
		ph_cycle_end_0:
		if(0){}
	}
	
	
	RETURN_CTOR(results);
}

/**
 * Inserts data into a table using custom RBDM SQL syntax
 *
 * <code>
 * //Inserting a new robot
 * $success = $connection->insert(
 *     "robots",
 *     array("Astro Boy", 1952),
 *     array("name", "year")
 * );
 *
 * //Next SQL sentence is sent to the database system
 * INSERT INTO `robots` (`name`, `year`) VALUES ("Astro boy", 1952);
 * </code>
 *
 * @param 	string $table
 * @param 	array $values
 * @param 	array $fields
 * @return 	boolean
 */
PHP_METHOD(Phalcon_Db, insert){

	zval *table = NULL, *values = NULL, *fields = NULL, *number_values = NULL, *exception_message = NULL;
	zval *exception = NULL, *placeholders = NULL, *insert_values = NULL;
	zval *value = NULL, *str_value = NULL, *comma = NULL, *joined_values = NULL;
	zval *joined_fields = NULL, *insert_sql = NULL, *success = NULL;
	HashTable *ah0;
	HashPosition hp0;
	zval **hd;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|z", &table, &values, &fields) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!fields) {
		PHALCON_ALLOC_ZVAL_MM(fields);
		ZVAL_NULL(fields);
	}
	
	if (Z_TYPE_P(values) != IS_ARRAY) { 
		PHALCON_THROW_EXCEPTION_STR(phalcon_db_exception_ce, "The second parameter for insert isn't an Array");
		return;
	}
	
	PHALCON_INIT_VAR(number_values);
	phalcon_fast_count(number_values, values TSRMLS_CC);
	if (phalcon_compare_strict_long(number_values, 0 TSRMLS_CC)) {
		PHALCON_INIT_VAR(exception_message);
		PHALCON_CONCAT_SVS(exception_message, "Unable to insert into ", table, " without data");
		
		PHALCON_INIT_VAR(exception);
		object_init_ex(exception, phalcon_db_exception_ce);
		PHALCON_CALL_METHOD_PARAMS_1_NORETURN(exception, "__construct", exception_message, PH_CHECK);
		phalcon_throw_exception(exception TSRMLS_CC);
		return;
	}
	
	PHALCON_INIT_VAR(placeholders);
	array_init(placeholders);
	
	PHALCON_INIT_VAR(insert_values);
	array_init(insert_values);
	
	if (!phalcon_valid_foreach(values TSRMLS_CC)) {
		return;
	}
	
	ah0 = Z_ARRVAL_P(values);
	zend_hash_internal_pointer_reset_ex(ah0, &hp0);
	
	ph_cycle_start_0:
	
		if(zend_hash_get_current_data_ex(ah0, (void**) &hd, &hp0) != SUCCESS){
			goto ph_cycle_end_0;
		}
		
		PHALCON_GET_FOREACH_VALUE(value);
		
		if (Z_TYPE_P(value) == IS_OBJECT) {
			PHALCON_INIT_VAR(str_value);
			PHALCON_CALL_FUNC_PARAMS_1(str_value, "strval", value);
			phalcon_array_append(&placeholders, str_value, PH_SEPARATE TSRMLS_CC);
		} else {
			if (Z_TYPE_P(value) == IS_NULL) {
				phalcon_array_append_string(&placeholders, SL("null"), PH_SEPARATE TSRMLS_CC);
			} else {
				phalcon_array_append_string(&placeholders, SL("?"), PH_SEPARATE TSRMLS_CC);
				phalcon_array_append(&insert_values, value, PH_SEPARATE TSRMLS_CC);
			}
		}
		
		zend_hash_move_forward_ex(ah0, &hp0);
		goto ph_cycle_start_0;
	
	ph_cycle_end_0:
	
	PHALCON_INIT_VAR(comma);
	ZVAL_STRING(comma, ", ", 1);
	
	PHALCON_INIT_VAR(joined_values);
	phalcon_fast_join(joined_values, comma, placeholders TSRMLS_CC);
	if (Z_TYPE_P(fields) == IS_ARRAY) { 
		PHALCON_INIT_VAR(joined_fields);
		phalcon_fast_join(joined_fields, comma, fields TSRMLS_CC);
		
		PHALCON_INIT_VAR(insert_sql);
		PHALCON_CONCAT_SVSVSVS(insert_sql, "INSERT INTO ", table, " (", joined_fields, ") VALUES (", joined_values, ")");
	} else {
		PHALCON_INIT_VAR(insert_sql);
		PHALCON_CONCAT_SVSVS(insert_sql, "INSERT INTO ", table, " VALUES (", joined_values, ")");
	}
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_2(success, this_ptr, "execute", insert_sql, insert_values, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

/**
 * Updates data on a table using custom RBDM SQL syntax
 *
 * <code>
 * //Updating existing robot
 * $success = $connection->update(
 *     "robots",
 *     array("name")
 *     array("New Astro Boy"),
 *     "id = 101"
 * );
 *
 * //Next SQL sentence is sent to the database system
 * UPDATE `robots` SET `name` = "Astro boy" WHERE id = 101
 * </code>
 *
 * @param 	string $table
 * @param 	array $fields
 * @param 	array $values
 * @param 	string $whereCondition
 * @return 	boolean
 */
PHP_METHOD(Phalcon_Db, update){

	zval *table = NULL, *fields = NULL, *values = NULL, *where_condition = NULL;
	zval *placeholders = NULL, *update_values = NULL, *value = NULL, *position = NULL;
	zval *field = NULL, *set_clause_part = NULL, *comma = NULL, *set_clause = NULL;
	zval *update_sql = NULL, *success = NULL;
	HashTable *ah0;
	HashPosition hp0;
	zval **hd;
	char *hash_index;
	uint hash_index_len;
	ulong hash_num;
	int hash_type;
	int eval_int;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz|z", &table, &fields, &values, &where_condition) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!where_condition) {
		PHALCON_ALLOC_ZVAL_MM(where_condition);
		ZVAL_NULL(where_condition);
	}
	
	PHALCON_INIT_VAR(placeholders);
	array_init(placeholders);
	
	PHALCON_INIT_VAR(update_values);
	array_init(update_values);
	
	if (!phalcon_valid_foreach(values TSRMLS_CC)) {
		return;
	}
	
	ah0 = Z_ARRVAL_P(values);
	zend_hash_internal_pointer_reset_ex(ah0, &hp0);
	
	ph_cycle_start_0:
	
		if(zend_hash_get_current_data_ex(ah0, (void**) &hd, &hp0) != SUCCESS){
			goto ph_cycle_end_0;
		}
		
		PHALCON_INIT_VAR(position);
		PHALCON_GET_FOREACH_KEY(position, ah0, hp0);
		PHALCON_GET_FOREACH_VALUE(value);
		
		eval_int = phalcon_array_isset(fields, position);
		if (eval_int) {
			PHALCON_INIT_VAR(field);
			phalcon_array_fetch(&field, fields, position, PH_NOISY_CC);
			if (Z_TYPE_P(value) == IS_OBJECT) {
				PHALCON_INIT_VAR(set_clause_part);
				PHALCON_CONCAT_VSV(set_clause_part, field, " = ", value);
				phalcon_array_append(&placeholders, set_clause_part, PH_SEPARATE TSRMLS_CC);
			} else {
				if (Z_TYPE_P(value) == IS_NULL) {
					PHALCON_INIT_VAR(set_clause_part);
					PHALCON_CONCAT_VS(set_clause_part, field, " = null");
				} else {
					PHALCON_INIT_VAR(set_clause_part);
					PHALCON_CONCAT_VS(set_clause_part, field, " = ?");
					phalcon_array_append(&update_values, value, PH_SEPARATE TSRMLS_CC);
				}
				phalcon_array_append(&placeholders, set_clause_part, PH_SEPARATE TSRMLS_CC);
			}
		} else {
			PHALCON_THROW_EXCEPTION_STR(phalcon_db_exception_ce, "The number of values in the update is not the same as fields");
			return;
		}
		
		zend_hash_move_forward_ex(ah0, &hp0);
		goto ph_cycle_start_0;
	
	ph_cycle_end_0:
	
	PHALCON_INIT_VAR(comma);
	ZVAL_STRING(comma, ", ", 1);
	
	PHALCON_INIT_VAR(set_clause);
	phalcon_fast_join(set_clause, comma, placeholders TSRMLS_CC);
	if (Z_TYPE_P(where_condition) != IS_NULL) {
		PHALCON_INIT_VAR(update_sql);
		PHALCON_CONCAT_SVSVSV(update_sql, "UPDATE ", table, " SET ", set_clause, " WHERE ", where_condition);
	} else {
		PHALCON_INIT_VAR(update_sql);
		PHALCON_CONCAT_SVSV(update_sql, "UPDATE ", table, " SET ", set_clause);
	}
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_2(success, this_ptr, "execute", update_sql, update_values, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

/**
 * Deletes data from a table using custom RBDM SQL syntax
 *
 * <code>
 * //Deleting existing robot
 * $success = $connection->delete(
 *     "robots",
 *     "id = 101"
 * );
 *
 * //Next SQL sentence is generated
 * DELETE FROM `robots` WHERE `id` = 101
 * </code>
 *
 * @param  string $table
 * @param  string $whereCondition
 * @param  array $placeholders
 * @return boolean
 */
PHP_METHOD(Phalcon_Db, delete){

	zval *table = NULL, *where_condition = NULL, *placeholders = NULL;
	zval *sql = NULL, *success = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|zz", &table, &where_condition, &placeholders) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!where_condition) {
		PHALCON_ALLOC_ZVAL_MM(where_condition);
		ZVAL_NULL(where_condition);
	}
	
	if (!placeholders) {
		PHALCON_ALLOC_ZVAL_MM(placeholders);
		ZVAL_NULL(placeholders);
	}
	
	if (Z_TYPE_P(where_condition) != IS_NULL) {
		PHALCON_INIT_VAR(sql);
		PHALCON_CONCAT_SVSV(sql, "DELETE FROM ", table, " WHERE ", where_condition);
	} else {
		PHALCON_INIT_VAR(sql);
		PHALCON_CONCAT_SV(sql, "DELETE FROM ", table);
	}
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_2(success, this_ptr, "execute", sql, placeholders, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

/**
 * Gets a list of columns
 *
 * @param array $columnList
 * @return string
 */
PHP_METHOD(Phalcon_Db, getColumnList){

	zval *column_list = NULL, *dialect = NULL, *transformed_list = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &column_list) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(transformed_list);
	PHALCON_CALL_METHOD_PARAMS_1(transformed_list, dialect, "getcolumnlist", column_list, PH_NO_CHECK);
	
	RETURN_CCTOR(transformed_list);
}

/**
 * Appends a LIMIT clause to $sqlQuery argument
 *
 * <code>$connection->limit("SELECT * FROM robots", 5);</code>
 *
 * @param  	string $sqlQuery
 * @param 	int $number
 * @return 	string
 */
PHP_METHOD(Phalcon_Db, limit){

	zval *sql_query = NULL, *number = NULL, *dialect = NULL, *sql = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &sql_query, &number) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_2(sql, dialect, "limit", sql_query, number, PH_NO_CHECK);
	
	RETURN_CCTOR(sql);
}

/**
 * Generates SQL checking for the existence of a schema.table
 *
 * <code>$connection->tableExists("blog", "posts")</code>
 *
 * @param string $tableName
 * @param string $schemaName
 * @return string
 */
PHP_METHOD(Phalcon_Db, tableExists){

	zval *table_name = NULL, *schema_name = NULL, *dialect = NULL, *sql = NULL;
	zval *fetch_num = NULL, *num = NULL, *first = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &table_name, &schema_name) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!schema_name) {
		PHALCON_ALLOC_ZVAL_MM(schema_name);
		ZVAL_NULL(schema_name);
	}
	
	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_2(sql, dialect, "tableexists", table_name, schema_name, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(fetch_num);
	phalcon_get_class_constant(fetch_num, phalcon_db_ce, SL("FETCH_NUM") TSRMLS_CC);
	
	PHALCON_INIT_VAR(num);
	PHALCON_CALL_METHOD_PARAMS_2(num, this_ptr, "fetchone", sql, fetch_num, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(first);
	phalcon_array_fetch_long(&first, num, 0, PH_NOISY_CC);
	
	RETURN_CCTOR(first);
}

/**
 * Generates SQL checking for the existence of a schema.view
 *
 * <code>$connection->viewExists("active_users", "posts")</code>
 *
 * @param string $viewName
 * @param string $schemaName
 * @return string
 */
PHP_METHOD(Phalcon_Db, viewExists){

	zval *view_name = NULL, *schema_name = NULL, *exists = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &view_name, &schema_name) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!schema_name) {
		PHALCON_ALLOC_ZVAL_MM(schema_name);
		ZVAL_NULL(schema_name);
	}
	
	PHALCON_INIT_VAR(exists);
	PHALCON_CALL_METHOD_PARAMS_2(exists, this_ptr, "tableexists", view_name, schema_name, PH_NO_CHECK);
	
	RETURN_CCTOR(exists);
}

/**
 * Returns a SQL modified with a FOR UPDATE clause
 *
 * @param string $sqlQuery
 * @return string
 */
PHP_METHOD(Phalcon_Db, forUpdate){

	zval *sql_query = NULL, *dialect = NULL, *sql = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &sql_query) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_1(sql, dialect, "forupdate", sql_query, PH_NO_CHECK);
	
	RETURN_CCTOR(sql);
}

/**
 * Returns a SQL modified with a LOCK IN SHARE MODE clause
 *
 * @param string $sqlQuery
 * @return string
 */
PHP_METHOD(Phalcon_Db, sharedLock){

	zval *sql_query = NULL, *dialect = NULL, *sql = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &sql_query) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_1(sql, dialect, "sharedlock", sql_query, PH_NO_CHECK);
	
	RETURN_CCTOR(sql);
}

/**
 * Creates a table using MySQL SQL
 *
 * @param string $tableName
 * @param string $schemaName
 * @param array $definition
 * @return boolean
 */
PHP_METHOD(Phalcon_Db, createTable){

	zval *table_name = NULL, *schema_name = NULL, *definition = NULL;
	zval *exception_message = NULL, *exception = NULL, *columns = NULL;
	zval *number_columns = NULL, *dialect = NULL, *sql = NULL, *success = NULL;
	int eval_int;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &table_name, &schema_name, &definition) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (Z_TYPE_P(definition) != IS_ARRAY) { 
		PHALCON_INIT_VAR(exception_message);
		PHALCON_CONCAT_SVS(exception_message, "Invalid definition to create the table '", table_name, "'");
		
		PHALCON_INIT_VAR(exception);
		object_init_ex(exception, phalcon_db_exception_ce);
		PHALCON_CALL_METHOD_PARAMS_1_NORETURN(exception, "__construct", exception_message, PH_CHECK);
		phalcon_throw_exception(exception TSRMLS_CC);
		return;
	}
	eval_int = phalcon_array_isset_string(definition, SL("columns")+1);
	if (!eval_int) {
		PHALCON_THROW_EXCEPTION_STR(phalcon_db_exception_ce, "The table must contain at least one column");
		return;
	}
	
	PHALCON_INIT_VAR(columns);
	phalcon_array_fetch_string(&columns, definition, SL("columns"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(number_columns);
	phalcon_fast_count(number_columns, columns TSRMLS_CC);
	if (phalcon_compare_strict_long(number_columns, 0 TSRMLS_CC)) {
		PHALCON_THROW_EXCEPTION_STR(phalcon_db_exception_ce, "The table must contain at least one column");
		return;
	}
	
	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_3(sql, dialect, "createtable", table_name, schema_name, definition, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_1(success, this_ptr, "execute", sql, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

/**
 * Drops a table from a schema/database
 *
 * @param string $tableName
 * @param   string $schemaName
 * @param boolean $ifExists
 * @return boolean
 */
PHP_METHOD(Phalcon_Db, dropTable){

	zval *table_name = NULL, *schema_name = NULL, *if_exists = NULL, *dialect = NULL;
	zval *sql = NULL, *success = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|z", &table_name, &schema_name, &if_exists) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!if_exists) {
		PHALCON_ALLOC_ZVAL_MM(if_exists);
		ZVAL_BOOL(if_exists, 1);
	}
	
	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_3(sql, dialect, "droptable", table_name, schema_name, if_exists, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_1(success, this_ptr, "execute", sql, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

/**
 * Adds a column to a table
 *
 * @param string $tableName
 * @param 	string $schemaName
 * @param Phalcon\Db\Column $column
 * @return boolean
 */
PHP_METHOD(Phalcon_Db, addColumn){

	zval *table_name = NULL, *schema_name = NULL, *column = NULL, *dialect = NULL;
	zval *sql = NULL, *success = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &table_name, &schema_name, &column) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_3(sql, dialect, "addcolumn", table_name, schema_name, column, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_1(success, this_ptr, "execute", sql, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

/**
 * Modifies a table column based on a definition
 *
 * @param string $tableName
 * @param string $schemaName
 * @param Phalcon\Db\Column $column
 * @return 	boolean
 */
PHP_METHOD(Phalcon_Db, modifyColumn){

	zval *table_name = NULL, *schema_name = NULL, *column = NULL, *dialect = NULL;
	zval *sql = NULL, *success = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &table_name, &schema_name, &column) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_3(sql, dialect, "modifycolumn", table_name, schema_name, column, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_1(success, this_ptr, "execute", sql, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

/**
 * Drops a column from a table
 *
 * @param string $tableName
 * @param string $schemaName
 * @param string $columnName
 * @return 	boolean
 */
PHP_METHOD(Phalcon_Db, dropColumn){

	zval *table_name = NULL, *schema_name = NULL, *column_name = NULL;
	zval *dialect = NULL, *sql = NULL;
	zval *r0 = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &table_name, &schema_name, &column_name) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_3(sql, dialect, "dropcolumn", table_name, schema_name, column_name, PH_NO_CHECK);
	
	PHALCON_ALLOC_ZVAL_MM(r0);
	PHALCON_CALL_METHOD_PARAMS_1(r0, this_ptr, "execute", sql, PH_NO_CHECK);
	RETURN_CTOR(r0);
}

/**
 * Adds an index to a table
 *
 * @param string $tableName
 * @param string $schemaName
 * @param DbIndex $index
 * @return 	boolean
 */
PHP_METHOD(Phalcon_Db, addIndex){

	zval *table_name = NULL, *schema_name = NULL, *index = NULL, *dialect = NULL;
	zval *sql = NULL, *success = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &table_name, &schema_name, &index) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_3(sql, dialect, "addindex", table_name, schema_name, index, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_1(success, this_ptr, "execute", sql, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

/**
 * Drop an index from a table
 *
 * @param string $tableName
 * @param string $schemaName
 * @param string $indexName
 * @return 	boolean
 */
PHP_METHOD(Phalcon_Db, dropIndex){

	zval *table_name = NULL, *schema_name = NULL, *index_name = NULL;
	zval *dialect = NULL, *sql = NULL, *success = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &table_name, &schema_name, &index_name) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_3(sql, dialect, "dropindex", table_name, schema_name, index_name, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_1(success, this_ptr, "execute", sql, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

/**
 * Adds a primary key to a table
 *
 * @param string $tableName
 * @param string $schemaName
 * @param Phalcon\Db\Index $index
 * @return 	boolean
 */
PHP_METHOD(Phalcon_Db, addPrimaryKey){

	zval *table_name = NULL, *schema_name = NULL, *index = NULL, *sql = NULL;
	zval *t0 = NULL;
	zval *r0 = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &table_name, &schema_name, &index) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_ALLOC_ZVAL_MM(t0);
	phalcon_read_property(&t0, this_ptr, SL("_dialect"), PH_NOISY_CC);
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_3(sql, t0, "addprimarykey", table_name, schema_name, index, PH_NO_CHECK);
	
	PHALCON_ALLOC_ZVAL_MM(r0);
	PHALCON_CALL_METHOD_PARAMS_1(r0, this_ptr, "execute", sql, PH_NO_CHECK);
	RETURN_CTOR(r0);
}

/**
 * Drops primary key from a table
 *
 * @param string $tableName
 * @param string $schemaName
 * @return 	boolean
 */
PHP_METHOD(Phalcon_Db, dropPrimaryKey){

	zval *table_name = NULL, *schema_name = NULL, *dialect = NULL, *sql = NULL;
	zval *success = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &table_name, &schema_name) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_2(sql, dialect, "dropprimarykey", table_name, schema_name, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_1(success, this_ptr, "execute", sql, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

/**
 * Adds a foreign key to a table
 *
 * @param string $tableName
 * @param string $schemaName
 * @param Phalcon\Db\Reference $reference
 * @return boolean true
 */
PHP_METHOD(Phalcon_Db, addForeignKey){

	zval *table_name = NULL, *schema_name = NULL, *reference = NULL, *dialect = NULL;
	zval *sql = NULL, *success = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &table_name, &schema_name, &reference) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_3(sql, dialect, "addforeignkey", table_name, schema_name, reference, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_1(success, this_ptr, "execute", sql, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

/**
 * Drops a foreign key from a table
 *
 * @param string $tableName
 * @param string $schemaName
 * @param string $referenceName
 * @return boolean true
 */
PHP_METHOD(Phalcon_Db, dropForeignKey){

	zval *table_name = NULL, *schema_name = NULL, *reference_name = NULL;
	zval *dialect = NULL, *sql = NULL, *success = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &table_name, &schema_name, &reference_name) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_3(sql, dialect, "dropforeignkey", table_name, schema_name, reference_name, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(success);
	PHALCON_CALL_METHOD_PARAMS_1(success, this_ptr, "execute", sql, PH_NO_CHECK);
	
	RETURN_CCTOR(success);
}

/**
 * Returns the SQL column definition from a column
 *
 * @param Phalcon\Db\Column $column
 * @return string
 */
PHP_METHOD(Phalcon_Db, getColumnDefinition){

	zval *column = NULL, *dialect = NULL, *definition = NULL;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &column) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(definition);
	PHALCON_CALL_METHOD_PARAMS_1(definition, dialect, "getcolumndefinition", column, PH_NO_CHECK);
	
	RETURN_CCTOR(definition);
}

/**
 * List all tables on a database
 *
 * <code> print_r($connection->listTables("blog") ?></code>
 *
 * @param string $schemaName
 * @return array
 */
PHP_METHOD(Phalcon_Db, listTables){

	zval *schema_name = NULL, *dialect = NULL, *sql = NULL, *fetch_num = NULL, *tables = NULL;
	zval *all_tables = NULL, *table = NULL, *table_name = NULL;
	HashTable *ah0;
	HashPosition hp0;
	zval **hd;

	PHALCON_MM_GROW();
	
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|z", &schema_name) == FAILURE) {
		PHALCON_MM_RESTORE();
		RETURN_NULL();
	}

	if (!schema_name) {
		PHALCON_ALLOC_ZVAL_MM(schema_name);
		ZVAL_NULL(schema_name);
	}
	
	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	PHALCON_INIT_VAR(sql);
	PHALCON_CALL_METHOD_PARAMS_1(sql, dialect, "listtables", schema_name, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(fetch_num);
	phalcon_get_class_constant(fetch_num, phalcon_db_ce, SL("FETCH_NUM") TSRMLS_CC);
	
	PHALCON_INIT_VAR(tables);
	PHALCON_CALL_METHOD_PARAMS_2(tables, this_ptr, "fetchall", sql, fetch_num, PH_NO_CHECK);
	
	PHALCON_INIT_VAR(all_tables);
	array_init(all_tables);
	
	if (!phalcon_valid_foreach(tables TSRMLS_CC)) {
		return;
	}
	
	ah0 = Z_ARRVAL_P(tables);
	zend_hash_internal_pointer_reset_ex(ah0, &hp0);
	
	ph_cycle_start_0:
	
		if(zend_hash_get_current_data_ex(ah0, (void**) &hd, &hp0) != SUCCESS){
			goto ph_cycle_end_0;
		}
		
		PHALCON_GET_FOREACH_VALUE(table);
		
		PHALCON_INIT_VAR(table_name);
		phalcon_array_fetch_long(&table_name, table, 0, PH_NOISY_CC);
		phalcon_array_append(&all_tables, table_name, PH_SEPARATE TSRMLS_CC);
		
		zend_hash_move_forward_ex(ah0, &hp0);
		goto ph_cycle_start_0;
	
	ph_cycle_end_0:
	
	
	RETURN_CTOR(all_tables);
}

/**
 * Return descriptor used to connect to the active database
 *
 * @return string
 */
PHP_METHOD(Phalcon_Db, getDescriptor){

	zval *descriptor = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(descriptor);
	phalcon_read_property(&descriptor, this_ptr, SL("_descriptor"), PH_NOISY_CC);
	
	RETURN_CCTOR(descriptor);
}

/**
 * Gets the active connection unique identifier
 *
 * @return string
 */
PHP_METHOD(Phalcon_Db, getConnectionId){

	zval *connection_id = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(connection_id);
	phalcon_read_property(&connection_id, this_ptr, SL("_connectionId"), PH_NOISY_CC);
	
	RETURN_CCTOR(connection_id);
}

/**
 * Active SQL statement in the object
 */
PHP_METHOD(Phalcon_Db, getSQLStatement){

	zval *sql_statement = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(sql_statement);
	phalcon_read_property(&sql_statement, this_ptr, SL("_sqlStatement"), PH_NOISY_CC);
	
	RETURN_CCTOR(sql_statement);
}

/**
 * Returns type of database system the adapter is used for
 *
 * @return string
 */
PHP_METHOD(Phalcon_Db, getType){

	zval *type = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(type);
	phalcon_read_property(&type, this_ptr, SL("_type"), PH_NOISY_CC);
	
	RETURN_CCTOR(type);
}

/**
 * Returns the name of the dialect used
 *
 * @return string
 */
PHP_METHOD(Phalcon_Db, getDialectType){

	zval *dialect_type = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(dialect_type);
	phalcon_read_property(&dialect_type, this_ptr, SL("_dialectType"), PH_NOISY_CC);
	
	RETURN_CCTOR(dialect_type);
}

/**
 * Returns internal dialect instance
 *
 * @return Phalcon\Db\Dialect
 */
PHP_METHOD(Phalcon_Db, getDialect){

	zval *dialect = NULL;

	PHALCON_MM_GROW();
	PHALCON_INIT_VAR(dialect);
	phalcon_read_property(&dialect, this_ptr, SL("_dialect"), PH_NOISY_CC);
	
	RETURN_CCTOR(dialect);
}

