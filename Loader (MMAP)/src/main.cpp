#include <inc.hpp>

void t_com() {
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main(const int argc, char** argv)
{
	if (argc != 2 || std::filesystem::path(argv[1]).extension().string().compare(".sys"))
	{
		std::cout << "[-] Incorrect usage" << std::endl;
		return -1;
	}

	const std::string driver_path = argv[1];

	if (!std::filesystem::exists(driver_path))
	{
		std::cout << "[-] File " << driver_path << " doesn't exist" << std::endl;
		return -1;
	}

	HANDLE iqvw64e_device_handle = intel_driver::Load();

	if (!iqvw64e_device_handle || iqvw64e_device_handle == INVALID_HANDLE_VALUE)
	{
		std::cout << "[-] Failed to load driver iqvw64e.sys" << std::endl;
		return -1;
	}

	auto retMapDriver = kdmapper::MapDriver(iqvw64e_device_handle, driver_path);

	printf("[ ] retMapDriver: %#llx" "\n", (unsigned long long)retMapDriver);

	system("pause");

	intel_driver::Unload(iqvw64e_device_handle);

	return 0;
}