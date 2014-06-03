#include "file_generation.h"
#include "file_manipulation.h"
#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

char* str_to_arr(std::string &a)
{
	char* k = new char[a.size() + 1];
	memcpy(k, a.c_str(), a.size() + 1);
	return k;
}

int main(int argc, char** argv)
{
	auto t0 = std::chrono::high_resolution_clock::now();
	bool new_file_out = false;
	char file_path[1025], small_file_prefix[1025], output[1025];
	size_t num_lines, digit_length, small_file_count = 0;

	printf("Enter the file name: ");
	scanf("%s", file_path);

	printf("Enter the output file: ");
	scanf("%s", file_path);

	printf("Enter number of lines per small file: ");
	scanf("%d", &num_lines);

	printf("Enter the prefix of the smaller files: ");
	scanf("%s", small_file_prefix);

	printf("Enter smaller files suffix length: ");
	scanf("%d", &digit_length);

	printf("Chunking down file '%s' ...", file_path);
	/* chunk down large file */
	chunk_down_file(file_path, output, num_lines, digit_length, small_file_count);

	/*  Note: The idea of the last size_t& is to start a new thread for every new file generated.
	This is not yet implemented. */

	printf("Sorting chuncked down files ...");
	/* Sort smaller files */
	std::vector<std::thread> file_sorters;
	std::vector<char*> file_names;
	auto file_n_len = strlen(small_file_prefix);
	std::string cur_file;
	for (size_t i = 0; i < small_file_count; i++)
	{
		file_names.push_back(str_to_arr(make_string(small_file_prefix, i, file_n_len + digit_length)));
		file_sorters.push_back(std::thread(sort_file, file_names[i]));
	}
	for (size_t i = 0; i < small_file_count; i++)
	{
		file_sorters.back().join();
		file_sorters.pop_back();
	}
	for (std::vector<char*>::iterator i = file_names.begin(); i != file_names.end(); i++)
	{
		delete *i;
	}

	/* merge sorted smaller files */
	printf("Merging sorted files ...");
	merge_files(output, small_file_prefix, digit_length, small_file_count);

	auto t1 = std::chrono::high_resolution_clock::now();
	auto total_ms = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0);
	std::cout << "---" << std::endl;
	std::cout << "Total time: " << total_ms.count() << " nanoseconds" << std::endl;
	return 0;
}