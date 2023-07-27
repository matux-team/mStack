#include <core/mem-pool.h>
#include <string.h>

typedef struct Data
{
	uint32_t data1;
	char data2;
	double data3;
}data_t;

int main()
{
	MemPool<data_t> pool = MemPool<data_t>(4);

	data_t fake_ = {
			.data1 = 0x1234,
			.data2 = 0xAA,
			.data3 = 0.001,
	};

	void* p = pool.Alloc();

	memcpy(p, &fake_, sizeof(data_t));

	data_t* dataGet = (data_t*)p;

	pool.Free(p);

	while(1){}
	return 0;
}
