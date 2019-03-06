/*
 ============================================================================
 Name        : task-fd.c
 Author      : Heiher <r@hev.cc>
 Copyright   : Copyright (c) 2018 everyone.
 Description : Task FD Test
 ============================================================================
 */

#include <errno.h>
#include <unistd.h>
#include <stddef.h>
#include <assert.h>

#include <hev-task.h>
#include <hev-task-io.h>
#include <hev-task-io-pipe.h>
#include <hev-task-system.h>

static int fds[2];

static void
task1_entry (void *data)
{
    HevTask *task = hev_task_self ();
    int val;

    assert (hev_task_add_fd (task, fds[0], POLLOUT) == 0);
    assert (hev_task_mod_fd (task, fds[0], POLLIN) == 0);
    assert (hev_task_io_read (fds[0], &val, sizeof (val), NULL, NULL) ==
            sizeof (val));
retry:
    if (read (fds[0], &val, sizeof (val)) == -1 && errno == EAGAIN) {
        hev_task_yield (HEV_TASK_WAITIO);
        goto retry;
    }
    hev_task_del_fd (task, fds[0]);
}

static void
task2_entry (void *data)
{
    HevTask *task = hev_task_self ();
    int val;

    assert (hev_task_add_fd (task, fds[1], POLLOUT) == 0);
    assert (hev_task_io_write (fds[1], &val, sizeof (val), NULL, NULL) ==
            sizeof (val));
    hev_task_sleep (50);
    assert (hev_task_io_write (fds[1], &val, sizeof (val), NULL, NULL) ==
            sizeof (val));
    hev_task_del_fd (task, fds[1]);
}

int
main (int argc, char *argv[])
{
    HevTask *task;

    assert (hev_task_system_init () == 0);

    assert (hev_task_io_pipe_pipe (fds) == 0);

    task = hev_task_new (-1);
    assert (task);
    hev_task_run (task, task1_entry, NULL);

    task = hev_task_new (-1);
    assert (task);
    hev_task_run (task, task2_entry, NULL);

    hev_task_system_run ();

    close (fds[0]);
    close (fds[1]);

    hev_task_system_fini ();

    return 0;
}
