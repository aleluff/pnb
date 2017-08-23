#include <linux/ip.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/vmalloc.h>

extern unsigned int loc_ports[MAX_PORTS];
extern unsigned int rem_ports[MAX_PORTS];

static struct nf_hook_ops nfin;
static struct nf_hook_ops nfout;

static void set_port_size(int *ports, int port_num, int size)
{
	ports[port_num] += size;
}

static unsigned int hook_func_in(
	void *priv,
	struct sk_buff *skb,
	const struct nf_hook_state *state)
{
	set_port_size(loc_ports, skb_transport_header(skb)[0], skb->len);

	return NF_ACCEPT;
}

static unsigned int hook_func_out(
	void *priv,
	struct sk_buff *skb,
	const struct nf_hook_state *state)
{
	set_port_size(rem_ports, skb_transport_header(skb)[1], skb->len);

	return NF_ACCEPT;
}

static void init_hook(void)
{
	nfin.hook     = hook_func_in;
	nfin.hooknum  = NF_INET_LOCAL_IN;
	nfin.pf       = PF_INET;
	nfin.priority = NF_IP_PRI_FIRST;

	nfout = nfin;
	nfout.hook = hook_func_out;
	nfout.hooknum = NF_INET_LOCAL_OUT;

	nf_register_hook(&nfin);
	nf_register_hook(&nfout);
}

static void clean_hook(void)
{
	nf_unregister_hook(&nfin);
	nf_unregister_hook(&nfout);
}
