
#include "event.h"

int main()
{
	prefix_event_base_t *base = prefix_event_base_new();

	prefix_event_base_dump(base);
}
