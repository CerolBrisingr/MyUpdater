#include <openssl/evp.h>
#include <openssl/objects.h>
#include <iostream>

void my_callback(const OBJ_NAME* obj, void* arg)
{
	std::cout << "Digest: " << obj->name << '\n';}

int main()
{
	void* my_arg;
	OpenSSL_add_all_digests(); //make sure they're loaded

	my_arg = NULL;
	OBJ_NAME_do_all_sorted(OBJ_NAME_TYPE_MD_METH, my_callback, my_arg);
	return 0;
}
