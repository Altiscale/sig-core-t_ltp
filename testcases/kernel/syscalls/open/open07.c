/*
 *
 *   Copyright (c) International Business Machines  Corp., 2001
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * NAME
 *	open07.c
 *
 * DESCRIPTION
 *	Test the open(2) system call to ensure that it sets ELOOP correctly.
 *
 * CALLS
 *	open()
 *
 * ALGORITHM
 *	1. Create a symbolic link to a file, and call open(O_NOFOLLOW). Check
 *	   that it returns ELOOP.
 *
 *	2. Create a symbolic link to a directory, and call open(O_NOFOLLOW).
 *	   Check that it returns ELOOP.
 *
 *	3. Create a symbolic link to a symbolic link, and call open(O_NOFOLLOW).
 *	   Check that it returns ELOOP.
 *
 *	4. Create a symbolic link to a symbolically linked directory, and call
 *	   open(O_NOFOLLOW). Check that it returns ELOOP.
 *
 *	5. Create a symbolic link to a directory, and call
 *         open("link/", O_NOFOLLOW). Check that it succeeds.
 *
 * USAGE:  <for command-line>
 *  open07 [-c n] [-e] [-i n] [-I x] [-P x] [-t]
 *     where,  -c n : Run n copies concurrently.
 *             -e   : Turn on errno logging.
 *             -i n : Execute test n times.
 *             -I x : Execute test for x seconds.
 *             -P x : Pause for x seconds between iterations.
 *             -t   : Turn on syscall timing.
 *
 * HISTORY
 *	07/2001 Ported by Wayne Boyer
 *
 * RESTRICTIONS
 *	None
 */
#define _GNU_SOURCE		/* for O_NOFOLLOW */
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "test.h"
#include "usctest.h"

static void setup(void);
static void cleanup(void);
static void setupfunc_test1();
static void setupfunc_test2();
static void setupfunc_test3();
static void setupfunc_test4();
static void setupfunc_test5();

char *TCID = "open07";
int TST_TOTAL = 5;

static int fd1, fd2;

static int exp_enos[] = { ELOOP, 0 };

static struct test_case_t {
	char *desc;
	char filename[100];
	int flags;
	int mode;
	void (*setupfunc) ();
	int exp_errno;
} TC[] = {
	{"Test for ELOOP on f2: f1 -> f2", {},
	 O_NOFOLLOW, 00700, setupfunc_test1, ELOOP},
	{"Test for ELOOP on d2: d1 -> d2", {},
	 O_NOFOLLOW, 00700, setupfunc_test2, ELOOP},
	{"Test for ELOOP on f3: f1 -> f2 -> f3", {},
	 O_NOFOLLOW, 00700, setupfunc_test3, ELOOP},
	{"Test for ELOOP on d3: d1 -> d2 -> d3", {},
	 O_NOFOLLOW, 00700, setupfunc_test4, ELOOP},
	{"Test for success on d2: d1 -> d2", {},
	 O_NOFOLLOW, 00700, setupfunc_test5, 0},
	{NULL, {}, 0, 0, NULL, 0}
};

int main(int ac, char **av)
{
	int lc;
	char *msg;
	int i;

	msg = parse_opts(ac, av, NULL, NULL);
	if (msg != NULL)
		tst_brkm(TBROK, NULL, "OPTION PARSING ERROR - %s", msg);

	setup();

	TEST_EXP_ENOS(exp_enos);

	/* run the setup routines for the individual tests */
	for (i = 0; i < TST_TOTAL; i++) {
		if (TC[i].setupfunc != NULL)
			TC[i].setupfunc();
	}

	for (lc = 0; TEST_LOOPING(lc); lc++) {
		tst_count = 0;

		for (i = 0; TC[i].desc != NULL; ++i) {
			TEST(open(TC[i].filename, TC[i].flags, TC[i].mode));

			if (TC[i].exp_errno != 0) {
				if (TEST_RETURN != -1) {
					tst_resm(TFAIL, "open succeeded "
						 "unexpectedly");
				}
				TEST_ERROR_LOG(TEST_ERRNO);

				if (TEST_ERRNO != TC[i].exp_errno) {
					tst_resm(TFAIL, "open returned "
						 "unexpected errno, expected: "
						 "%d, got: %d",
						 TC[i].exp_errno, TEST_ERRNO);
				} else {
					tst_resm(TPASS, "open returned "
						 "expected ELOOP error");
				}
			} else {
				if (TEST_RETURN == -1) {
					tst_resm(TFAIL, "open failed "
						 "unexpectedly with errno %d",
						 TEST_ERRNO);
				} else {
					tst_resm(TPASS, "open succeeded as "
						 "expected");
				}
			}

			if (TEST_RETURN != -1)
				close(TEST_RETURN);
		}
	}

	cleanup();
	tst_exit();
}

static void setupfunc_test1()
{
	char file1[100], file2[100];

	sprintf(file1, "open03.1.%d", getpid());
	sprintf(file2, "open03.2.%d", getpid());
	fd1 = creat(file1, 00700);
	if (fd1 < 0)
		tst_brkm(TBROK, cleanup, "creat(2) failed: errno: %d", errno);

	if (symlink(file1, file2) < 0)
		tst_brkm(TBROK, cleanup, "symlink(2) failed: errno: %d", errno);

	strcpy(TC[0].filename, file2);
}

static void setupfunc_test2()
{
	char file1[100], file2[100];

	sprintf(file1, "open03.3.%d", getpid());
	sprintf(file2, "open03.4.%d", getpid());
	if (mkdir(file1, 00700) < 0)
		tst_brkm(TBROK, cleanup, "mkdir(2) failed: errno: %d", errno);

	if (symlink(file1, file2) < 0)
		tst_brkm(TBROK, cleanup, "symlink(2) failed: errno: %d", errno);

	strcpy(TC[1].filename, file2);
}

static void setupfunc_test3()
{
	char file1[100], file2[100], file3[100];

	sprintf(file1, "open03.5.%d", getpid());
	sprintf(file2, "open03.6.%d", getpid());
	sprintf(file3, "open03.7.%d", getpid());
	fd2 = creat(file1, 00700);
	if (fd2 < 0)
		tst_brkm(TBROK, cleanup, "creat(2) failed: errno: %d", errno);

	if (symlink(file1, file2) < 0)
		tst_brkm(TBROK, cleanup, "symlink(2) failed: errno: %d", errno);

	if (symlink(file2, file3) < 0)
		tst_brkm(TBROK, cleanup, "symlink(2) failed: errno: %d", errno);

	strcpy(TC[2].filename, file3);
}

static void setupfunc_test4()
{
	char file1[100], file2[100], file3[100];

	sprintf(file1, "open03.8.%d", getpid());
	sprintf(file2, "open03.9.%d", getpid());
	sprintf(file3, "open03.10.%d", getpid());
	if (mkdir(file1, 00700) < 0)
		tst_brkm(TBROK, cleanup, "mkdir(2) failed: errno: %d", errno);

	if (symlink(file1, file2) < 0)
		tst_brkm(TBROK, cleanup, "symlink(2) failed: errno: %d", errno);

	if (symlink(file2, file3) < 0)
		tst_brkm(TBROK, cleanup, "symlink(2) failed: errno: %d", errno);

	strcpy(TC[3].filename, file3);
}

static void setupfunc_test5()
{
	char file1[100], file2[100];

	sprintf(file1, "open11.3.%d", getpid());
	sprintf(file2, "open12.4.%d", getpid());
	if (mkdir(file1, 00700) < 0)
		tst_brkm(TBROK, cleanup, "mkdir(2) failed: errno: %d", errno);

	if (symlink(file1, file2) < 0)
		tst_brkm(TBROK, cleanup, "symlink(2) failed: errno: %d", errno);

	strcpy(TC[4].filename, file2);
	strcat(TC[4].filename, "/");
}

static void setup(void)
{
	umask(0);

	tst_sig(NOFORK, DEF_HANDLER, cleanup);

	TEST_PAUSE;

	tst_tmpdir();
}

static void cleanup(void)
{
	TEST_CLEANUP;
	close(fd1);
	close(fd2);

	tst_rmdir();
}