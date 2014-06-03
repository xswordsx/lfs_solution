#include "file_manipulation.h"
#include "file_generation.h" // make_string
#include <algorithm>		 // std::make_heap, std::sort_heap, std::pop_heap
#include <fstream>			 // std::ifstream, std::ofstream
#include <functional>		 // std::less
#include <vector>			 // std::vector
#include <queue>			 // std::priority_queue
#include <string>			 // std::string

bool mHeap::operator() (mHeap &a, mHeap &b)  { return !(strcmp(a.value, b.value) <= 0); }
bool std::less<char*>::operator() (char* const &a, char* const &b) const { return (strcmp(a, b) < 0); }

void sort_file(const char* const &file_path)
{
	using namespace std;

	vector<char*> lines;
	string current_line;
	ifstream file(file_path, ios::in);

	if (file.is_open())
	{
		for (size_t i = 0; file.good(); i++)
		{
			getline(file, current_line);
			lines.push_back(new char[current_line.size() + 1]);
			memcpy(lines[i], current_line.c_str(), current_line.size() + 1);
		}
		file.close();
		remove(file_path);
		/* calculations-heavy part */
		make_heap(lines.begin(), lines.end(), less<char*>());
		sort_heap(lines.begin(), lines.end(), less<char*>());

		ofstream ofile(file_path, ios::out);
		for (size_t i = 0; i < lines.size(); i++)
		{
			ofile << lines[i];
			if (i != lines.size() - 1)
				ofile << '\n';
			delete lines[i];
		}
		ofile.close();
	}
	else
		return;
}

void merge_files(const char* const file_path, const char* const small_files, size_t digits, size_t num_files)
{
	using namespace std;

	vector<ifstream> sorted_files;
	vector<mHeap> min_elem_node;

	string current_small_file_name;
	mHeap min_element;
	string current_elem;

	size_t small_files_name_len = strlen(small_files) + digits;
	size_t min_from = 0;

	priority_queue<mHeap, vector<mHeap>, mHeap> min_lines;

	ofstream output_file(file_path, ios::out | ios::trunc);

	/* Open files and initialize ifstreams and char arrays */
	for (size_t i = 0; i < num_files; i++)
	{
		current_small_file_name = make_string(small_files, i, small_files_name_len);

		sorted_files.push_back(ifstream(current_small_file_name.c_str(), ios::in));

		getline(sorted_files[i], current_elem);

		min_elem_node.push_back(mHeap());
		min_elem_node[i].value = new char[101];
		min_elem_node[i].enqueue_from = i;

		memcpy(min_elem_node[i].value, current_elem.c_str(), current_elem.size() + 1);
		min_lines.push(min_elem_node[i]);
	}

	/* While there are lines to be read / sorted - do so. */
	while (!min_lines.empty())
	{
		min_element = min_lines.top();
		output_file << min_element.value;
		output_file << '\n';

		min_lines.pop();
		if (sorted_files[min_element.enqueue_from].good() && !min_lines.empty())
		{
			getline(sorted_files[min_element.enqueue_from], current_elem, '\n');
			memcpy(min_element.value, current_elem.c_str(), current_elem.size() + 1);
			min_lines.push(min_element);
		}
	}

	for (size_t i = 0; i < num_files; i++)
	{
		sorted_files[i].close();
	}
	output_file.close();
}

void chunk_down_file(const char* const large_file_name,
					 const char* const smaller_files_prefix,
					 size_t lines_per_file, 
					 size_t digit_prefix_len,
					 size_t &file_count)
{
	using namespace std;

	ifstream large_file(large_file_name, ios::in);
	ofstream* current_small_file;

	size_t line_count = 0;
	size_t small_files_count = 0;
	size_t s_file_name_len = strlen(smaller_files_prefix);

	string current_small_file_name = make_string(smaller_files_prefix, 0, s_file_name_len + digit_prefix_len);
	string buffer;


	current_small_file = new ofstream(current_small_file_name, ios::out);

	while (getline(large_file, buffer, '\r'))
	{
		(*current_small_file) << buffer;
		line_count++;

		if (line_count != lines_per_file)
			*current_small_file << '\n';

		if (line_count == lines_per_file)
		{
			current_small_file->close();
			delete current_small_file;

			small_files_count++;

			current_small_file_name = make_string(smaller_files_prefix, small_files_count, s_file_name_len + digit_prefix_len);
			current_small_file = new ofstream(current_small_file_name, ios::out | ios::trunc);
			file_count++;
			line_count = 0;
		}
	}

	current_small_file->close();
	delete current_small_file;
	large_file.close();
}