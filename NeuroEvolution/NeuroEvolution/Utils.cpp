#include "Utils.h"
#include <fstream>

bool saveVectorToCsv(std::string name, std::vector<float>& data)
{
	int n = 1;
	std::ifstream f;

	do {
		if (f.is_open() == true) f.close();

		f = std::ifstream(name + "_" + std::to_string(n) + ".csv");
		n++;
	} while (f.good() && n < 100);

	f.close();

	if (n >= 100)
	{
		std::cout << "Could not find path, or files already exist." << std::endl;

		return false;
	}

	n--;

	std::fstream csvFile;
	csvFile.open(name + "_" + std::to_string(n) + ".csv", std::fstream::in | std::fstream::out | std::fstream::trunc);

	if (csvFile.is_open() == false)
	{
		std::cout << "Fstream failed to create file." << std::endl;

		return false;
	}

	for (std::vector<float>::iterator it = data.begin(); it != data.end(); ++it)
	{
		std::string str = std::to_string(*it);
		size_t pos = str.find(".");

		if (pos == -1)
		{
			csvFile << str << std::endl;
		}
		else {
			csvFile << str.substr(0, pos) << "," << str.substr(pos + 1, str.size()) << std::endl;
		}
	}

	csvFile.close();
	return true;
}