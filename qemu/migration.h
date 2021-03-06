/*
 * QEMU live migration
 *
 * Copyright IBM, Corp. 2008
 *
 * Authors:
 *  Anthony Liguori   <aliguori@us.ibm.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2.  See
 * the COPYING file in the top-level directory.
 *
 */

#ifndef QEMU_MIGRATION_H
#define QEMU_MIGRATION_H

#define MIG_STATE_ERROR		-1
#define MIG_STATE_COMPLETED	0
#define MIG_STATE_CANCELLED	1
#define MIG_STATE_ACTIVE	2

typedef struct MigrationState MigrationState;

struct MigrationState
{
    /* FIXME: add more accessors to print migration info */
    void (*cancel)(MigrationState *s);
    int (*get_status)(MigrationState *s);
    int (*release)(MigrationState *s);
};

typedef struct FdMigrationState FdMigrationState;

struct FdMigrationState
{
    MigrationState mig_state;
    int64_t bandwidth_limit;
    QEMUFile *file;
    int fd;
    int detach;
    int state;
    int save_started;
    int (*get_error)(struct FdMigrationState*);
    int (*close)(struct FdMigrationState*);
    int (*write)(struct FdMigrationState*, const void *, size_t);
    void (*done)(struct FdMigrationState*, int state);
    void *opaque;
};

void qemu_start_incoming_migration(const char *uri);

void do_migrate(int detach, const char *uri);

void do_migrate_cancel(void);

void do_migrate_set_speed(const char *value);

uint64_t migrate_max_downtime(void);

void do_migrate_set_downtime(const char *value);

void do_info_migrate(void);

int exec_start_incoming_migration(const char *host_port);

MigrationState *exec_start_outgoing_migration(const char *host_port,
					     int64_t bandwidth_limit,
					     int detach);

int tcp_start_incoming_migration(const char *host_port);
#ifdef CONFIG_SPICE
void tcp_migration_register_interface(void);
#endif

MigrationState *tcp_start_outgoing_migration(const char *host_port,
					     int64_t bandwidth_limit,
					     int detach);

void migrate_fd_error(FdMigrationState *s);

void migrate_fd_cleanup(FdMigrationState *s);

void migrate_fd_put_notify(void *opaque);

ssize_t migrate_fd_put_buffer(void *opaque, const void *data, size_t size);

void migrate_fd_connect(FdMigrationState *s);

void migrate_fd_put_ready(void *opaque);

int migrate_fd_get_status(MigrationState *mig_state);

void migrate_fd_cancel(MigrationState *mig_state);

int migrate_fd_release(MigrationState *mig_state);

void migrate_fd_wait_for_unfreeze(void *opaque);

int migrate_fd_close(void *opaque);

static inline FdMigrationState *migrate_to_fms(MigrationState *mig_state)
{
    return container_of(mig_state, FdMigrationState, mig_state);
}

#endif
