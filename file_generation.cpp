#include "file_generation.h"
#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

std::string make_string(const std::string& a_prefix, size_t a_suffix, size_t a_max_length)
{
	std::ostringstream result;
	result << a_prefix <<
		std::setfill('0') <<
		std::setw(a_max_length - a_prefix.length()) <<
		a_suffix;
	return result.str();
}

void create_file(size_t const &lines, const char* const file_path, size_t const &line_length)
{
	register std::ofstream file(file_path);
	register const char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	register int current_line_length = 0;

	std::random_device device;
	register std::uniform_int_distribution<> distribution(0, sizeof(alphabet) / sizeof(*alphabet) - 2);
	register std::uniform_int_distribution<> line_distribution(1, line_length);
	register std::default_random_engine engine(device());

	register size_t i = 0;

	register size_t j = 0;
	for (i = 1; i <= lines; i++)
	{
		current_line_length = line_distribution(engine);
		for (j = current_line_length; j != 0; j--)
			file << alphabet[distribution(engine)];
		if(i != lines)
			file << '\n';
	}
	file.close();
}

void concurent_generation(size_t& const cpu_count, size_t& const lines, const char* const file_path, size_t const &line_length, size_t const &tmp_files_prefix_len)
{
	size_t file_max_count = strlen(file_path) + tmp_files_prefix_len;
	std::vector<std::thread> threads;
	std::string current_file;

	/* 1 Thread for each CPU */
	for (size_t i = 1; i < cpu_count; ++i) 
	{
		current_file = make_string(file_path, i, file_max_count);
		threads.push_back(std::thread(create_file,
			lines / cpu_count,
			current_file.c_str(),
			line_length));
	}

	/* The first thread can get a little extra work if (lines % cpu_count) is not 0 */
	current_file = make_string(file_path, 0, file_max_count);
	threads.push_back(std::thread(create_file,
		((lines / cpu_count) + (lines % cpu_count)),
		current_file.c_str(),
		line_length));

	/* Write all the small files */
	for (size_t i = 0; i < cpu_count; i++)
		threads[i].join();

	std::ofstream main_file(file_path, std::ofstream::out);
	std::ifstream smaller_files;
	std::string line;
	for (size_t i = 0; i < cpu_count; i++)
	{
		current_file = make_string(file_path, i, file_max_count);
		smaller_files.open(current_file.c_str(), std::ios::in);

		while (std::getline(smaller_files, line))
		{
			main_file << line;
			if (i != cpu_count - 1)
				main_file << '\n';
			else if (smaller_files.good())
				main_file << '\n';
		}

		/* Tidy-up your work space */
		smaller_files.close();
		remove(current_file.c_str());
	}
}
