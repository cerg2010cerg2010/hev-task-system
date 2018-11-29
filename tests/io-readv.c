/*
 ============================================================================
 Name        : io-readv.c
 Author      : Heiher <r@hev.cc>
 Copyright   : Copyright (c) 2018 everyone.
 Description : IO ReadV Test
 ============================================================================
 */

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <hev-task.h>
#include <hev-task-system.h>
#include <hev-task-io.h>

static void
task_entry (void *data)
{
    HevTask *task = hev_task_self ();
    int fd;
    char buf1[2], buf2[2];
    ssize_t size;
    struct iovec iov[2];

    fd = hev_task_io_open ("/dev/random", O_RDONLY);
    assert (fd >= 0);

    assert (hev_task_add_fd (task, fd, EPOLLIN) == 0);

    iov[0].iov_base = buf1;
    iov[0].iov_len = 2;
    iov[1].iov_base = buf2;
    iov[1].iov_len = 2;
    size = hev_task_io_readv (fd, iov, 2, NULL, NULL);
    assert (size >= 0);

    close (fd);
}

int
main (int argc, char *argv[])
{
    HevTask *task;

    assert (hev_task_system_init () == 0);

    task = hev_task_new (-1);
    assert (task);
    hev_task_run (task, task_entry, NULL);

    hev_task_system_run ();

    hev_task_system_fini ();

    return 0;
}
