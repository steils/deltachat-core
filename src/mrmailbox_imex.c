/*******************************************************************************
 *
 *                              Delta Chat Core
 *                      Copyright (C) 2017 Björn Petersen
 *                   Contact: r10s@b44t.com, http://b44t.com
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see http://www.gnu.org/licenses/ .
 *
 *******************************************************************************
 *
 * File:    mrmailbox_imex.c - Import and Export things
 *
 ******************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "mrmailbox.h"
#include "mrosnative.h"
#include "mraheader.h"
#include "mrapeerstate.h"
#include "mrtools.h"
#include "mre2ee.h"
#include "mre2ee_driver.h"


/*******************************************************************************
 * Import
 ******************************************************************************/


static int import_public_key(mrmailbox_t* mailbox, const char* addr, const char* public_key_file)
{
	/* mainly for testing: if the partner does not support Autocrypt,
	encryption is disabled as soon as the first messages comes from the partner */
	mraheader_t*    header = mraheader_new();
	mrapeerstate_t* peerstate = mrapeerstate_new();
	int             locked = 0, success = 0;

	if( addr==NULL || public_key_file==NULL || peerstate==NULL || header==NULL ) {
		goto cleanup;
	}

	/* create a fake autocrypt header */
	header->m_to               = safe_strdup(addr);
	header->m_prefer_encrypted = MRA_PE_YES;
	if( !mrkey_set_from_file(header->m_public_key, public_key_file, mailbox)
	 || !mre2ee_driver_is_valid_key(mailbox, header->m_public_key) ) {
		mrmailbox_log_warning(mailbox, 0, "No valid key found in \"%s\".", public_key_file);
		goto cleanup;
	}

	/* update/create peerstate */
	mrsqlite3_lock(mailbox->m_sql);
	locked = 1;

		if( mrapeerstate_load_from_db__(peerstate, mailbox->m_sql, addr) ) {
			mrapeerstate_apply_header(peerstate, header, time(NULL));
			mrapeerstate_save_to_db__(peerstate, mailbox->m_sql, 0);
		}
		else {
			mrapeerstate_init_from_header(peerstate, header, time(NULL));
			mrapeerstate_save_to_db__(peerstate, mailbox->m_sql, 1);
		}

		success = 1;

cleanup:
	if( locked ) { mrsqlite3_unlock(mailbox->m_sql); }
	mrapeerstate_unref(peerstate);
	mraheader_unref(header);
	return success;
}


int mrmailbox_import_spec(mrmailbox_t* mailbox, const char* spec__) /* spec is a file, a directory or NULL for the last import */
{
	int            success = 0;
	char*          spec = NULL;
	char*          suffix = NULL;
	DIR*           dir = NULL;
	struct dirent* dir_entry;
	int            read_cnt = 0;
	char*          name;

	if( mailbox == NULL ) {
		return 0;
	}

	if( !mrsqlite3_is_open(mailbox->m_sql) ) {
        mrmailbox_log_error(mailbox, 0, "Import: Database not opened.");
		goto cleanup;
	}

	/* if `spec` is given, remember it for later usage; if it is not given, try to use the last one */
	if( spec__ )
	{
		spec = safe_strdup(spec__);
		mrsqlite3_lock(mailbox->m_sql);
			mrsqlite3_set_config__(mailbox->m_sql, "import_spec", spec);
		mrsqlite3_unlock(mailbox->m_sql);
	}
	else {
		mrsqlite3_lock(mailbox->m_sql);
			spec = mrsqlite3_get_config__(mailbox->m_sql, "import_spec", NULL); /* may still NULL */
		mrsqlite3_unlock(mailbox->m_sql);
		if( spec == NULL ) {
			mrmailbox_log_error(mailbox, 0, "Import: No file or folder given.");
			goto cleanup;
		}
	}

	suffix = mr_get_filesuffix_lc(spec);
	if( suffix && strcmp(suffix, "eml")==0 ) {
		/* import a single file */
		if( mrmailbox_import_eml_file(mailbox, spec) ) { /* errors are logged in any case */
			read_cnt++;
		}
	}
	else if( suffix && (strcmp(suffix, "pem")==0||strcmp(suffix, "asc")==0) ) {
		/* import a publix key */
		char* separator = strchr(spec, ' ');
		if( separator==NULL ) {
			mrmailbox_log_error(mailbox, 0, "Import: Key files must be specified as \"<addr> <key-file>\".");
			goto cleanup;
		}
		*separator = 0;
		if( import_public_key(mailbox, spec, separator+1) ) {
			read_cnt++;
		}
		*separator = ' ';
	}
	else {
		/* import a directory */
		if( (dir=opendir(spec))==NULL ) {
			mrmailbox_log_error(mailbox, 0, "Import: Cannot open directory \"%s\".", spec);
			goto cleanup;
		}

		while( (dir_entry=readdir(dir))!=NULL ) {
			name = dir_entry->d_name; /* name without path; may also be `.` or `..` */
			if( strlen(name)>=4 && strcmp(&name[strlen(name)-4], ".eml")==0 ) {
				char* path_plus_name = mr_mprintf("%s/%s", spec, name);
				mrmailbox_log_info(mailbox, 0, "Import: %s", path_plus_name);
				if( mrmailbox_import_eml_file(mailbox, path_plus_name) ) { /* no abort on single errors errors are logged in any case */
					read_cnt++;
				}
				free(path_plus_name);
            }
		}
	}

	mrmailbox_log_info(mailbox, 0, "Import: %i items read from \"%s\".", read_cnt, spec);
	if( read_cnt > 0 ) {
		mailbox->m_cb(mailbox, MR_EVENT_MSGS_CHANGED, 0, 0); /* even if read_cnt>0, the number of messages added to the database may be 0. While we regard this issue using IMAP, we ignore it here. */
	}

	/* success */
	success = 1;

	/* cleanup */
cleanup:
	if( dir ) {
		closedir(dir);
	}
	free(spec);
	free(suffix);
	return success;
}


int mrmailbox_import(mrmailbox_t* mailbox, int what, const char* dir_name)
{
	int            imported_count = 0, locked = 0;
	DIR*           dir_handle = NULL;
	struct dirent* dir_entry = NULL;
	char*          suffix = NULL;
	char*          path_plus_name = NULL;
	mrkey_t*       private_key = mrkey_new();
	mrkey_t*       public_key = mrkey_new();
	sqlite3_stmt*  stmt = NULL;
	char*          self_addr = NULL;

	if( mailbox==NULL || dir_name==NULL ) {
		goto cleanup;
	}

	if( what == MR_IMEX_SELF_KEYS )
	{
		if( (dir_handle=opendir(dir_name))==NULL ) {
			mrmailbox_log_error(mailbox, 0, "Import: Cannot open directory \"%s\".", dir_name);
			goto cleanup;
		}

		while( (dir_entry=readdir(dir_handle))!=NULL )
		{
			free(suffix);
			suffix = mr_get_filesuffix_lc(dir_entry->d_name);
			if( suffix==NULL || strcmp(suffix, "asc")!=0 ) {
				continue;
			}

			free(path_plus_name);
			path_plus_name = mr_mprintf("%s/%s", dir_name, dir_entry->d_name/* name without path; may also be `.` or `..` */);
			mrmailbox_log_info(mailbox, 0, "Checking: %s", path_plus_name);
			if( !mrkey_set_from_file(private_key, path_plus_name, mailbox) ) {
				mrmailbox_log_error(mailbox, 0, "Cannot read key from \"%s\".", path_plus_name);
				continue;
			}

			if( private_key->m_type!=MR_PRIVATE ) {
				continue; /* this is no error but quite normal as we always export the public keys together with the private ones */
			}

			if( !mre2ee_driver_is_valid_key(mailbox, private_key) ) {
				mrmailbox_log_error(mailbox, 0, "\"%s\" is no valid key.", path_plus_name);
				continue;
			}

			if( !mre2ee_driver_split_key(mailbox, private_key, public_key) ) {
				mrmailbox_log_error(mailbox, 0, "\"%s\" seems not to contain a private key.", path_plus_name);
				continue;
			}

			/* add keypair as default; before this, delete other keypairs with the same binary key and reset defaults */
			mrsqlite3_lock(mailbox->m_sql);
			locked = 1;

				stmt = mrsqlite3_prepare_v2_(mailbox->m_sql, "DELETE FROM keypairs WHERE public_key=? OR private_key=?;");
				sqlite3_bind_blob (stmt, 1, public_key->m_binary, public_key->m_bytes, SQLITE_STATIC);
				sqlite3_bind_blob (stmt, 2, private_key->m_binary, private_key->m_bytes, SQLITE_STATIC);
				sqlite3_step(stmt);
				sqlite3_finalize(stmt);
				stmt = NULL;

				mrsqlite3_execute__(mailbox->m_sql, "UPDATE keypairs SET is_default=0;");

				free(self_addr);
				self_addr = mrsqlite3_get_config__(mailbox->m_sql, "configured_addr", NULL);
				if( !mrkey_save_self_keypair__(public_key, private_key, self_addr, mailbox->m_sql) ) {
					mrmailbox_log_error(mailbox, 0, "Cannot save keypair.");
					goto cleanup;
				}

				imported_count++;

			mrsqlite3_unlock(mailbox->m_sql);
			locked = 0;
		}

	}
	else
	{
		goto cleanup;
	}

cleanup:
	if( locked ) { mrsqlite3_unlock(mailbox->m_sql); }
	if( dir_handle ) { closedir(dir_handle); }
	free(suffix);
	free(path_plus_name);
	mrkey_unref(private_key);
	mrkey_unref(public_key);
	if( stmt ) { sqlite3_finalize(stmt); }
	free(self_addr);
	return imported_count;
}


/*******************************************************************************
 * Export
 ******************************************************************************/


typedef struct mrexportthreadparam_t
{
	mrmailbox_t* m_mailbox;
	int          m_what;
	char*        m_dir;
} mrexportthreadparam_t;


static pthread_t s_export_thread;
static int       s_export_thread_created = 0;


static void export_key_to_asc_file(mrmailbox_t* mailbox, const char* dir, int id, const mrkey_t* key, int is_default)
{
	char* file_content = mrkey_render_asc(key);
	char* file_name;
	if( is_default ) {
		file_name = mr_mprintf("%s/%s-key-default.asc", dir, key->m_type==MR_PUBLIC? "public" : "private");
	}
	else {
		file_name = mr_mprintf("%s/%s-key-%i.asc", dir, key->m_type==MR_PUBLIC? "public" : "private", id);
	}
	mrmailbox_log_info(mailbox, 0, "Exporting key %s", file_name);
	mr_delete_file(file_name, mailbox);
	if( !mr_write_file(file_name, file_content, strlen(file_content), mailbox) ) {
		mrmailbox_log_error(mailbox, 0, "Cannot write key to %s", file_name);
	}
	else {
		mailbox->m_cb(mailbox, MR_EVENT_EXPORT_FILE_WRITTEN, (uintptr_t)file_name, (uintptr_t)"application/pgp-keys");
	}
	free(file_content);
	free(file_name);
}


static int export_self_keys(mrmailbox_t* mailbox, const char* dir)
{
	sqlite3_stmt* stmt = NULL;
	int           success = 0, id = 0, is_default = 0;
	mrkey_t*      public_key = mrkey_new();
	mrkey_t*      private_key = mrkey_new();
	int           locked = 0;

	mrsqlite3_lock(mailbox->m_sql);
	locked = 1;

		if( (stmt=mrsqlite3_prepare_v2_(mailbox->m_sql, "SELECT id, public_key, private_key, is_default FROM keypairs;"))==NULL ) {
			goto cleanup;
		}

		while( sqlite3_step(stmt)==SQLITE_ROW ) {
			id = sqlite3_column_int(         stmt, 0  );
			mrkey_set_from_stmt(public_key,  stmt, 1, MR_PUBLIC);
			mrkey_set_from_stmt(private_key, stmt, 2, MR_PRIVATE);
			is_default = sqlite3_column_int( stmt, 3  );
			export_key_to_asc_file(mailbox, dir, id, public_key,  is_default);
			export_key_to_asc_file(mailbox, dir, id, private_key, is_default);
		}

		success = 1;

cleanup:
	if( locked ) { mrsqlite3_unlock(mailbox->m_sql); }
	if( stmt ) { sqlite3_finalize(stmt); }
	mrkey_unref(public_key);
	mrkey_unref(private_key);
	return success;
}


static int export_backup(mrmailbox_t* mailbox)
{
	int success = 0, locked = 0, closed = 0;

	/* get a fine backup file name (the name includes the date so that multiple backup instances are possible) */

	//TODO

	/* temporary lock and close the source (we just make a copy of the whole file, this is the fastest and easiest approach) */
	mrsqlite3_lock(mailbox->m_sql);
	locked = 1;
	mrsqlite3_close__(mailbox->m_sql);
	closed = 1;

	/* copy file to backup directory */

	//TODO

	/* unlock and re-open the source and make it availabe again for the normal use */
	mrsqlite3_open__(mailbox->m_sql, mailbox->m_dbfile);
	closed = 0;
	mrsqlite3_unlock(mailbox->m_sql);
	locked = 0;

	/* add all files as blobs to the database copy (this does not require the source to be locked) */

	//TODO

	/* done */
	success = 1;

//cleanup:
	if( closed ) { mrsqlite3_open__(mailbox->m_sql, mailbox->m_dbfile); }
	if( locked ) { mrsqlite3_unlock(mailbox->m_sql); }
	return success;
}


static void* export_thread_entry_point(void* entry_arg)
{
	int                    success = 0;
	mrexportthreadparam_t* thread_param = (mrexportthreadparam_t*)entry_arg;

	mrosnative_setup_thread(thread_param->m_mailbox); /* must be first */
	mrmailbox_log_info(thread_param->m_mailbox, 0, "Backup-thread started.");

	if( !mrsqlite3_is_open(thread_param->m_mailbox->m_sql) ) {
        mrmailbox_log_error(thread_param->m_mailbox, 0, "Export: Database not opened.");
		goto cleanup;
	}

	if( !mre2ee_make_sure_private_key_exists(thread_param->m_mailbox) ) {
		mrmailbox_log_error(thread_param->m_mailbox, 0, "Export: Cannot create private key or private key not available.");
		goto cleanup;
	}

	mr_create_folder(thread_param->m_dir, thread_param->m_mailbox);

	if( thread_param->m_what == MR_IMEX_SELF_KEYS ) {
		if( !export_self_keys(thread_param->m_mailbox, thread_param->m_dir) ) { /* export all secret and public keys */
			goto cleanup;
		}
	}
	else if( thread_param->m_what == MR_EXPORT_BACKUP ) {
		if( !export_backup(thread_param->m_mailbox) ) {
			goto cleanup;
		}
	}

	success = 1;

cleanup:
	mrmailbox_log_info(thread_param->m_mailbox, 0, "Backup-thread ended.");
	thread_param->m_mailbox->m_cb(thread_param->m_mailbox, MR_EVENT_EXPORT_ENDED, success, 0);
	s_export_thread_created = 0;
	free(thread_param->m_dir);
	free(thread_param);
	mrosnative_unsetup_thread(thread_param->m_mailbox); /* must be very last */
	return NULL;
}


void mrmailbox_export(mrmailbox_t* mailbox, int what, const char* dir)
{
	mrexportthreadparam_t* thread_param;

	if( mailbox==NULL || mailbox->m_sql==NULL || dir==NULL ) {
		return;
	}

	if( s_export_thread_created ) {
		mrmailbox_log_warning(mailbox, 0, "Already exporting.");
		return;
	}
	s_export_thread_created = 1;

	memset(&s_export_thread, 0, sizeof(pthread_t));
	thread_param = calloc(1, sizeof(mrexportthreadparam_t));
	thread_param->m_mailbox = mailbox;
	thread_param->m_what = what;
	thread_param->m_dir = safe_strdup(dir);
	pthread_create(&s_export_thread, NULL, export_thread_entry_point, thread_param);
}