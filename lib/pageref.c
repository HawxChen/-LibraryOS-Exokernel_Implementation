#include <inc/lib.h>

int
pageref(void *v)
{
	pte_t pte;

	if (!(vpd[PDX(v)] & PTE_P))
		return 0;
	pte = vpt[PGNUM(v)];
	if (!(pte & PTE_P))
		return 0;
	return pages[PGNUM(pte)].pp_ref;
}
