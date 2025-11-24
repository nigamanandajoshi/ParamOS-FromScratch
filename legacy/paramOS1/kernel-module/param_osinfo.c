// Param OS - Simple Linux Kernel Module creating /proc/param_osinfo

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>
#include <linux/sched/signal.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nigamananda (Param OS)");
MODULE_DESCRIPTION("Param OS info module - /proc/param_osinfo");
MODULE_VERSION("1.0");

static struct proc_dir_entry *proc_entry;

/* Module parameter: PID to inspect (optional) */
static int pid = -1;
module_param(pid, int, 0444);
MODULE_PARM_DESC(pid, "PID to show info for in /proc/param_osinfo");

static int param_osinfo_show(struct seq_file *m, void *v)
{
    struct task_struct *task = NULL;

    seq_printf(m, "Param OS - Kernel Info\n");
    seq_printf(m, "-----------------------\n");
    seq_printf(m, "Kernel version: %s\n", utsname()->release);
    seq_printf(m, "Uptime (jiffies): %lu\n", jiffies);
    seq_printf(m, "HZ (timer frequency): %d\n", HZ);

    if (pid > 0) {
        rcu_read_lock();
        task = pid_task(find_vpid(pid), PIDTYPE_PID);
        if (task) {
            seq_printf(m, "\nProcess info for PID %d:\n", pid);
            seq_printf(m, "  Comm: %s\n", task->comm);
            seq_printf(m, "  State: %ld\n", task->state);
            seq_printf(m, "  Nice: %d\n", task_nice(task));
        } else {
            seq_printf(m, "\nNo task found for PID %d\n", pid);
        }
        rcu_read_unlock();
    } else {
        seq_puts(m, "\nNo PID provided. Load module with pid=<PID> to inspect.\n");
    }

    return 0;
}

static int param_osinfo_open(struct inode *inode, struct file *file)
{
    return single_open(file, param_osinfo_show, NULL);
}

static const struct proc_ops param_osinfo_fops = {
    .proc_open    = param_osinfo_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

static int __init param_osinfo_init(void)
{
    proc_entry = proc_create("param_osinfo", 0444, NULL, &param_osinfo_fops);
    if (!proc_entry) {
        pr_err("param_osinfo: failed to create /proc/param_osinfo\n");
        return -ENOMEM;
    }

    pr_info("param_osinfo: module loaded (pid=%d)\n", pid);
    return 0;
}

static void __exit param_osinfo_exit(void)
{
    if (proc_entry)
        proc_remove(proc_entry);

    pr_info("param_osinfo: module unloaded\n");
}

module_init(param_osinfo_init);
module_exit(param_osinfo_exit);
