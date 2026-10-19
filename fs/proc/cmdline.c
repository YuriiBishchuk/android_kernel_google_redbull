// SPDX-License-Identifier: GPL-2.0
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/string.h>
#include <asm/setup.h>

static char proc_cmdline_buf[COMMAND_LINE_SIZE];

static int cmdline_proc_show(struct seq_file *m, void *v)
{
	seq_puts(m, proc_cmdline_buf);
	seq_putc(m, '\n');
	return 0;
}

static int __init proc_cmdline_init(void)
{
	char *p;

	strlcpy(proc_cmdline_buf, saved_command_line, sizeof(proc_cmdline_buf));

	/* 1. Defeat FirstStageInit ForceNormalBoot() when misc is empty on cold boot */
	while ((p = strstr(proc_cmdline_buf, "androidboot.force_normal_boot=1")) != NULL) {
		p[strlen("androidboot.force_normal_boot=")] = '0';
	}

	/* 2. Replace console=null so pstore_console logs to console-ramoops-0 */
	while ((p = strstr(proc_cmdline_buf, "console=null")) != NULL) {
		memcpy(p, "console=tty0", 12);
	}

	/* 3. Guarantee androidboot.mode=recovery is visible to init on cold power-on */
	if (!strstr(proc_cmdline_buf, "androidboot.mode=recovery")) {
		strlcat(proc_cmdline_buf, " androidboot.mode=recovery", sizeof(proc_cmdline_buf));
	}

	proc_create_single("cmdline", 0, NULL, cmdline_proc_show);
	return 0;
}
fs_initcall(proc_cmdline_init);
