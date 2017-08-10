#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netfilter.h>
#include <linux/vmalloc.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Biche");
MODULE_DESCRIPTION("Network bandwidth stats per process");

static struct nf_hook_ops nfin;

static unsigned int hook_func_in(unsigned int hooknum,
	struct sk_buff *skb, const struct net_device *in,
	const struct net_device *out, int (*okfn)(struct sk_buff *))
{
	struct ethhdr *eth;
	struct iphdr *ip_header;

	/*if (in is not the correct device)
		return NF_ACCEPT;*/

	eth = (struct ethhdr*)skb_mac_header(skb);
	ip_header = (struct iphdr *)skb_network_header(skb);
	printk("src mac %pM, dst mac %pM\n", eth->h_source, eth->h_dest);
	printk("src IP addr:=%d.%d.%d.%d:%d\n", NIPQUAD(ip_headr->saddr));
	return NF_ACCEPT;
}

static int __init procnet_init(void)
{
	nfin.hook     = hook_func_in;
	nfin.hooknum  = NF_IP_LOCAL_IN;
	nfin.pf       = PF_INET;
	nfin.priority = NF_IP_PRI_FIRST;

	nf_register_hook(&nfin);

	return 0;
}

static void __exit procnet_cleanup(void)
{
	nf_unregister_hook(&nfin);
}

module_init(procnet_init);
module_exit(procnet_cleanup);
