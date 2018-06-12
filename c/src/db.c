#include <assert.h>
#include <stddef.h>

#include <sqlite3.h>

#include "db.h"
#include "dqlite.h"
#include "lifecycle.h"

void dqlite__db_init(struct dqlite__db *db) {
	assert(db != NULL);

	dqlite__lifecycle_init(DQLITE__LIFECYCLE_DB);

	dqlite__error_init(&db->error);

	db->db = NULL;
	db->rc = SQLITE_OK;
	db->errmsg = NULL;
}

void dqlite__db_close(struct dqlite__db *db) {
	assert(db != NULL);

	dqlite__error_close(&db->error);

	dqlite__lifecycle_close(DQLITE__LIFECYCLE_DB);
}

/* TODO: do some validation of the name (e.g. can't begin with a slash) */
int dqlite__db_open(
	struct dqlite__db *db,
	const char *name,
	int flags,
	const char *vfs)
{
	assert(db != NULL);
	assert(name != NULL);
	assert(vfs != NULL);

	db->rc = sqlite3_open_v2(name, &db->db, flags, vfs);
	if (db->rc != SQLITE_OK) {
		db->errmsg = sqlite3_errmsg(db->db);
		return DQLITE_ENGINE;
	}

	return 0;
}

int dqlite__db_abort(struct dqlite__db *db)
{
	assert(db != NULL);

	if (db->db == NULL) {
		return 0;
	}

	db->rc = sqlite3_close(db->db);
	if (db->rc != SQLITE_OK) {
		db->errmsg = sqlite3_errmsg(db->db);
		return DQLITE_ENGINE;
	}

	return 0;
}
