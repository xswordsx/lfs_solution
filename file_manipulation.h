#ifndef FILE_MANIPULATION_H
#define FILE_MANIPULATION_H

struct mHeap
{
	char* value;
	size_t enqueue_from;
	bool operator() (mHeap &a, mHeap &b);
};

/**
	Sorts the file with complete file path <file_path> in lexicographical order.
	Pre:
		<file_path> needs to be an existig file, not used by other proccesses.
		<num_lines> have to be the number of lines of the file.
	Post:
		/!\ If the function fails on some way, the unsorted file [will] be lost! /!\
*/
void sort_file(const char* const &file_path);

/**
	Merges several small sorted files with complete file paths '<small_files><X>'
	into one large sorted file with complete file path <file_path>. 
	||X|| is digits and X is in the range [0, num_files).
		Example: [digits = 4, num_files = 2] => Possible X: {0000, 0001}
	Pre:
		All small files need to be sorted and not opened by another proccess.
		<file_path> needs to be a writable file.
	Post:
		<file_path> will be a sorted file. Any file that had a complete path <file_path> will be overriden.
*/
void merge_files(const char* const file_path, const char* const small_files, size_t digits, size_t num_files);

/**
	Creates several files from one larger file.
	<large_file_name> - complete path to the file that needs to be chuncked down.
	<smaller_files_prefix> - The complete file path (up to the suffix) of the smaller files.
	<file_added> - changes to 'true' when a new small file is created.
*/
void chunk_down_file(const char* const large_file_name, const char* const smaller_files_prefix, size_t lines_per_file, size_t digit_prefix_len, size_t &file_added);
#endif