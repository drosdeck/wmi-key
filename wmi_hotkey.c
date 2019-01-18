#include <linux/module.h>	 /* Needed by all modules */ 
#include <linux/kernel.h>	 /* Needed for KERN_INFO */ 
#include <linux/init.h>	 /* Needed for the macros */ 
//#include <linux/input/sparse-keymap.h>
//#include <linux/platform_device.h>
#include <linux/acpi.h>
//#include <linux/rfkill.h>
#include <linux/string.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL"); 

MODULE_AUTHOR("Edson Juliano Drosdeck"); 

MODULE_DESCRIPTION("A simple wmi!"); 

MODULE_VERSION("0.1"); 

char hotkey[5];
char *wmi_event_guid = "blaaa";

#define EEEPC_WMI_EVENT_GUID "ABBC0F72-8EA1-11D1-00A0-C90629100000"

module_param(wmi_event_guid,charp,0444);
MODULE_PARM_DESC(wmi_event_guid,"Teste de strind");

static int hotkey_proc_show(struct seq_file *m, void *v) {
	  seq_printf(m, "%s",hotkey);
	    return 0;
}

static int hotkey_proc_open(struct inode *inode, struct  file *file) {
	  return single_open(file, hotkey_proc_show, NULL);
}

static const struct file_operations hotkey_proc_fops = {
	  .owner = THIS_MODULE,
	  .open = hotkey_proc_open,
	   .read = seq_read,
	   .llseek = seq_lseek,
           .release = single_release,
};


static void eeepc_wmi_notify(u32 value, void *context)
    {
	struct acpi_buffer response = { ACPI_ALLOCATE_BUFFER, NULL };
	union acpi_object *obj;
	acpi_status status;
	int code;

	status = wmi_get_event_data(value, &response);
	if (status != AE_OK) {
	    pr_err("bad event status 0x%x\n", status);
	    return;
	}

	obj = (union acpi_object *)response.pointer;

	if (obj && obj->type == ACPI_TYPE_INTEGER) {
		code = obj->integer.value;

		snprintf(hotkey,5,"%d\n", code);
	}

	kfree(obj);
    }



static int __init hello_start(void) 
{
	int status;
        printk("parametro eeeeeeeeeeeeeeeeee %s",wmi_event_guid);
	printk(KERN_INFO "Loading hello module...\n"); 
	printk(KERN_INFO "Hello world\n"); 
        
	proc_create("wmi-hotkey", 0, NULL, &hotkey_proc_fops);

        if (!wmi_has_guid(wmi_event_guid)) {
	    printk("No known WMI GUID found\n");
	    return -ENODEV;
	}

	status = wmi_install_notify_handler(EEEPC_WMI_EVENT_GUID,
					eeepc_wmi_notify, NULL);
		if (ACPI_FAILURE(status)) {
	    	
		return -ENODEV;
		}
	return 0; 
} 

static void __exit hello_end(void) 
{ 
	printk(KERN_INFO "Goodbye Mr.\n"); 
        remove_proc_entry("wmi-hotkey",NULL);
	wmi_remove_notify_handler(EEEPC_WMI_EVENT_GUID);
} 

module_init(hello_start); 
module_exit(hello_end); 

