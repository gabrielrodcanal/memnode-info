#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/mmzone.h>
#include <linux/init.h>
#include <linux/kallsyms.h>
#include "memnode_info.h"
#include <linux/mm_types.h> /* mm_struct */
/* #include <asm-generic/page.h> */
/* #include <asm-generic/io.h> */
#include "/home/gabriel/linux-4.13/arch/x86/include/asm/pgtable.h" 
/* #include "/home/gabriel/linux-4.13/arch/x86/include/asm/pgtable_types.h" */
	
char ** vmstat_text; /* vector of names of stat counters */
struct page * m_map;

MODULE_AUTHOR("Gabriel Rodríguez Canal");
MODULE_LICENSE("GPL");


static void print_node_info(struct seq_file *m, struct pglist_data *pgdat) {
	int i;
	/* seq_printf(m, "UMA node basic info\n===================\n"); */
	seq_printf(m, "\n%s", NODE_INFO);
	print_sep(m, NODE_INFO);
	seq_printf(m, "Maximum number of zones: %d\n", MAX_NR_ZONES);
	seq_printf(m, "Number of zones: %d\n", pgdat->nr_zones);
	seq_printf(m, "PFN start of node: %lu\n", pgdat->node_start_pfn);
	seq_printf(m, "Total number of physical pages: %lu\n",
		pgdat->node_present_pages);
	seq_printf(m, "Total size of physical page range, including holes: %lu\n",
		pgdat->node_spanned_pages);
	seq_printf(m, "Node id: %d\n", pgdat->node_id); /* should be 0 */
	seq_printf(m, "kswapd max order: %d\n", pgdat->kswapd_order);
	seq_printf(m, "kswapd failures: %d\n", pgdat->kswapd_failures);
	seq_printf(m, "Total reserved pages (not available to userspace): "
		"%lu\n", pgdat->totalreserve_pages);	
	seq_printf(m, "Inactive ratio (LRU): %u\n", pgdat->inactive_ratio);
	
	seq_printf(m, "\nNode stats\n---------------\n");
	for(i = 0; i < NR_VM_NODE_STAT_ITEMS; i++) {
		seq_printf(m, "%s: %lu\n", 
			vmstat_text[i + NR_VM_ZONE_STAT_ITEMS], 
			pgdat->vm_stat[i].counter);
		/* seq_printf(m, "%s: %lu\n", vmstat_text[i],
			atomic_long_read(&pgdat->vm_stat[i].counter)); */
	}
}

static void print_pages_info(struct seq_file *m) {
	int i;
	int mask;
	char *cr0_flags[] = {"PE", "MP", "EM", "TS", "ET", "NE", NULL, NULL,
		NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,  "WP", NULL,
 		"AM", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		 NULL, "NW", "CD", "PG"};
	char *cr4_flags[] = {"VME", "PVI", "TSD", "DE", "PSE", "PAE", "MCE",
		"PGE", "PCE", "OSFXSR", "OSXMMEXCPT", "UMIP", "LA57",
		"VMXE", "SMXE", NULL, "FSGSBASE", "PCIDE", "OSXSAVE", NULL, 
		"SMEP", "SMAP", "PKE"};



	/* Control registers values */
	#ifdef __x86_64__
    	u64 cr0, cr3, cr4;
    	__asm__ __volatile__ (
		"mov %%cr0, %%rax\n\t"
		"mov %%eax, %0\n\t"
		"mov %%cr3, %%rax\n\t"
		"mov %%eax, %1\n\t"
        	"mov %%cr4, %%rax\n\t"
        	"mov %%eax, %2\n\t"
    	: "=m" (cr0), "=m" (cr3), "=m" (cr4)
    	: /* no input */
    	: "%rax"
    	);
	#elif defined(__i386__)
    	u32 cr0, cr3, cr4;
    	__asm__ __volatile__ (
		"mov %%cr0, %%eax\n\t"
		"mov %%eax, %0\n\t"
        	"mov %%cr3, %%eax\n\t"
        	"mov %%eax, %1\n\t"
		"mov%%cr4, %%eax\n\t"
		"mov%%eax, %%eax\n\t"
    	: "=m" (cr0), "=m" (cr3), "=m" (cr4)
    	: /* no input */
    	: "%eax"
    	);
	#endif
	
	seq_printf(m, "Mem map flags: %lu\n", m_map->flags);
	seq_printf(m, "Mapping member address %p\n", m_map->mapping);
	seq_printf(m, "s_mem address: %p\n", m_map->s_mem);
	seq_printf(m, "compund_mapcount: %p\n", &m_map->compound_mapcount);
	seq_printf(m, "compound_mapcount: %d\n", 
		m_map->compound_mapcount.counter);
	seq_printf(m, "index adr: %p\n", &m_map->index);
	seq_printf(m, "freelist adr: %p\n", m_map->freelist);
	seq_printf(m, "counters adr: %p\n", &m_map->counters);
	seq_printf(m, "mapcount adr: %p\n", &m_map->_mapcount);
	seq_printf(m, "active adr: %p\n", &m_map->active);
	/* seq_printf(m, "inuse adr: %p\n", &m_map->inuse); */
	seq_printf(m, "units adr: %p\n", &m_map->units); 
	seq_printf(m, "dev_pagemap adr: %p\n", m_map->pgmap);
	seq_printf(m, "next adr: %p\n", m_map->next);
	seq_printf(m, "Next page flags: %lu\n", m_map->next->flags);
	m_map++;
	seq_printf(m, "Mem map +1 flags: %lu\n", m_map->flags);
	m_map++;
	seq_printf(m, "Mem map +2 flags: %lu\n", m_map->flags);
	seq_printf(m, "CR3 addr (phys) = 0x%0x\n", cr3);
	seq_printf(m, "Physical frame  number PDBR addr: 0x%0x\n", cr3 >> 12);
	seq_printf(m, "Virtual PDBR addr: 0x%0x\n", (cr3 >> 12) + PAGE_OFFSET);
	/* pgd_cr3 = (pgd_t *)phys_to_virt(cr3>>12); */
	/* pgd_cr3 = (pgd_t *)ioremap(cr3>>12, sizeof(pgd_t)); */
	/* seq_printf(m, "Virtual PDBR adr (macro): %lu\n", virt_to_phys(pgd_cr3)); */
	/* seq_printf(m, "Virtual PDBR adr allocated: 0x%0x\n", pgd_cr3);
	seq_printf(m, "pgd value: %lu\n", pgd_cr3->pgd); */
	
	
	seq_printf(m, "Number of page table levels: %d\n", 
		CONFIG_PGTABLE_LEVELS);

	seq_printf(m, "\nCR4 value: 0x%0x\n", cr4);

	mask = 1;
	seq_printf(m, "\nCR4 flags\n---------\n");	
	for(i = 0; i < NR_CR4_FLAGS; i++) {	
		if(i == 15 || i == 19) {
			mask <<= 1;
			continue;
		}
		seq_printf(m, "%s: %s\n", cr4_flags[i], 
			((cr4 & mask) >> i == 1) ? "true" : "false");
		mask <<= 1;
	}

	seq_printf(m, "\nCR0 value: 0x%0x\n", cr0);
	mask = 1;
	seq_printf(m, "\nCR0 flags\n---------\n");	
	for(i = 0; i < NR_CR0_FLAGS; i++) {
		if((i > 5 && i < 16) || i == 17 || (i > 18 && i < 29)) {
			mask <<= 1;
			continue;
		}
		seq_printf(m, "%s: %s\n", cr0_flags[i],
			((cr0 & mask) >> i == 1) ? "true" : "false");
		mask <<= 1;
	}
}

static void print_zone_info(struct seq_file *m, struct zone *zone) {
	int i;
	/* seq_printf(m, "\nZone %s information\n=====================\n",
		zone->name); */
	seq_printf(m, "\nZone %s\n----------------\n", zone->name);
	seq_printf(m, "PFN of start of zone: %lu\n", zone->zone_start_pfn);
	seq_printf(m, "Managed pages (present pages managed by the buddy "
		"system: %lu\n", zone->managed_pages);
	seq_printf(m, "Spanned pages (total pages spanned by the zone, "
		"including holes: %lu\n", zone->spanned_pages);
	seq_printf(m, "Present pages (physical pages existing within the "
		"zone: %lu\n", zone->present_pages);
	seq_printf(m, "Initialised: %d\n", zone->initialized);
	seq_printf(m, "Zone flags: %lu\n", zone->flags);
	seq_printf(m, "Percpu drift mark (watermark): %lu\n", 
		zone->percpu_drift_mark);
	seq_printf(m, "Contiguous: %s\n", 
		(zone->contiguous == 1) ? "true":"false");
	/* vmstat_text = (char **)kallsyms_lookup_name("vmstat_text"); */

	seq_printf(m, "\nZone stats\n----------------\n");
	for(i = 0; i < NR_VM_ZONE_STAT_ITEMS; i++) {
		seq_printf(m, "%s: %lu\n", vmstat_text[i], 
			zone->vm_stat[i].counter);
	}	
}

static int memnode_info_proc_show(struct seq_file *m, void *v) {
	int nid, i;
	struct pglist_data *pgdat;
	struct zone *zone;
	nid = 0;	/* only UMA node */
	pgdat = NODE_DATA(nid);	
	
	print_node_info(m, pgdat);
	seq_printf(m, "\n%s", ZONE_INFO);
	print_sep(m, ZONE_INFO);
	zone = pgdat->node_zones;
	for(i = 0; i < pgdat->nr_zones; i++) { 
		print_zone_info(m, zone);		
		zone++;
	}

	seq_printf(m, "\n%s", PAGES_INFO);
	/* seq_printf(m, "%s", SEP_FIELD); */
	print_sep(m, PAGES_INFO);
	print_pages_info(m);

	return 0;
}

static int memnode_info_proc_open(struct inode *inode, struct file *file) {
	return single_open(file, memnode_info_proc_show, NULL);
}

static struct file_operations memnode_info_proc_fops = {
	.open	 = memnode_info_proc_open,
	.read	 = seq_read,
	.llseek	 = seq_lseek,
	.release = single_release,
};

static int __init proc_memnode_info_init(void) {
	vmstat_text = (char **)kallsyms_lookup_name("vmstat_text");
	/* m_map = (struct page *)kallsyms_lookup_name("mem_map"); */
	m_map = (struct page *)vmemmap_base;
	printk(KERN_INFO "Mem_map address: %p\n", m_map);
	proc_create(PROC_NAME, 0, NULL, &memnode_info_proc_fops);
	
	/*
	printk(KERN_INFO "/proc/%s inserted\n", PROC_NAME);
	printk(KERN_INFO "cr3 = 0x%8.8X\n", cr3);
	printk(KERN_INFO "cr3 = 0X%0X\n", cr3);
	*/
	return 0;
}

static void __exit proc_memnode_info_exit(void) {
	remove_proc_entry(PROC_NAME, NULL);
	printk(KERN_INFO "/proc/%s removed\n", PROC_NAME);
}

void print_sep(struct seq_file *m, char field[]) {
	int nr_chr;
	for(nr_chr = 0; field[nr_chr] != 0; nr_chr++)
		seq_printf(m, "=");
	seq_printf(m, "\n");
}
	
module_init(proc_memnode_info_init);
module_exit(proc_memnode_info_exit);
