PHP_ARG_ENABLE(phalcon, whether to enable phalcon framework, [ --enable-phalcon   Enable phalcon framework])

if test "$PHP_PHALCON" = "yes"; then
  AC_DEFINE(HAVE_PHALCON, 1, [Whether you have Phalcon Framework])
  PHP_NEW_EXTENSION(phalcon, phalcon.c kernel/main.c kernel/fcall.c kernel/require.c kernel/debug.c kernel/assert.c kernel/object.c kernel/array.c kernel/operators.c kernel/concat.c kernel/exception.c kernel/memory.c session/exception.c session/adapter/files.c session/bag.c loader.c di.c text.c mvc/router/exception.c mvc/router/route.c mvc/router.c mvc/micro.c mvc/dispatcher/exception.c mvc/view.c mvc/view/engine.c mvc/view/exception.c mvc/view/engine/php.c mvc/url.c mvc/controller.c mvc/application/exception.c mvc/url/exception.c mvc/dispatcher.c mvc/model.c mvc/micro/exception.c mvc/model/validator/uniqueness.c mvc/model/validator/exclusionin.c mvc/model/validator/regex.c mvc/model/validator/inclusionin.c mvc/model/validator/numericality.c mvc/model/validator/email.c mvc/model/query.c mvc/model/resultset/complex.c mvc/model/resultset/simple.c mvc/model/query/status.c mvc/model/query/lang.c mvc/model/exception.c mvc/model/criteria.c mvc/model/validator.c mvc/model/row.c mvc/model/transaction/exception.c mvc/model/transaction/failed.c mvc/model/transaction/manager.c mvc/model/metadata.c mvc/model/message.c mvc/model/manager.c mvc/model/metadata/memory.c mvc/model/metadata/apc.c mvc/model/metadata/session.c mvc/model/resultset.c mvc/model/transaction.c mvc/user/plugin.c mvc/user/component.c mvc/application.c config/exception.c config/adapter/ini.c exception.c db.c logger.c cache/exception.c cache/frontend/output.c cache/frontend/none.c cache/frontend/data.c cache/backend.c cache/backend/memcache.c cache/backend/apc.c cache/backend/file.c acl/exception.c acl/adapter/memory.c acl/role.c acl/resource.c paginator/exception.c paginator/adapter/model.c paginator/adapter/nativearray.c tag/exception.c tag/select.c filter/exception.c flash/direct.c flash/exception.c flash/session.c dispatcher.c translate.c db/profiler.c db/exception.c db/reference.c db/dialect.c db/adapter/pdo/mysql.c db/adapter/pdo/postgresql.c db/adapter/pdo/sqlite.c db/adapter/pdo.c db/profiler/item.c db/rawvalue.c db/column.c db/index.c db/result/pdo.c db/dialect/mysql.c db/dialect/postgresql.c db/dialect/sqlite.c tag.c http/request/exception.c http/request/file.c http/response/exception.c http/response/headers.c http/response.c http/request.c session.c version.c flash.c config.c filter.c di/factorydefault/cli.c di/exception.c di/injectable.c di/factorydefault.c events/event.c events/exception.c events/manager.c acl.c translate/exception.c translate/adapter/nativearray.c cli/task.c cli/router/exception.c cli/router.c cli/dispatcher/exception.c cli/console.c cli/dispatcher.c cli/console/exception.c logger/exception.c logger/adapter/file.c logger/item.c loader/exception.c mvc/model/query/parser.c mvc/model/query/scanner.c, $ext_shared)
fi
