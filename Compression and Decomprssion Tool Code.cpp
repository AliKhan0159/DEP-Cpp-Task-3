#include <iostream>
#include <fstream>
#include <vector>
#include <zlib.h>

class FileCompressor {
public:
	// Compresses the input file to the output file
	void compressFile(const std::string &inputFile, const std::string &outputFile) {
		std::ifstream file(inputFile, std::ios::binary);
		if (!file) {
			throw std::runtime_error("Could not open input file: " + inputFile);
		}

		// Read file data into a vector
		std::vector<char> buffer((std::istreambuf_iterator<char>(file)), {});
		file.close();

		uLongf compressedSize = compressBound(buffer.size());
		std::vector<char> compressedData(compressedSize);

		// Compress the data
		if (compress(reinterpret_cast<Bytef *>(compressedData.data()), &compressedSize,
			reinterpret_cast<const Bytef *>(buffer.data()), buffer.size()) != Z_OK) {
			throw std::runtime_error("Compression failed.");
		}

		// Write compressed data to the output file
		std::ofstream outFile(outputFile, std::ios::binary);
		outFile.write(compressedData.data(), compressedSize);
		outFile.close();

		std::cout << "File compressed successfully to: " << outputFile << std::endl;
	}

	// Decompresses the input file to the output file
	void decompress(const std::string &inputFile, const std::string &outputFile) {
		std::ifstream file(inputFile, std::ios::binary);
		if (!file) {
			throw std::runtime_error("Could not open input file: " + inputFile);
		}

		// Read compressed file data into a vector
		std::vector<char> compressedBuffer((std::istreambuf_iterator<char>(file)), {});
		file.close();

		uLongf decompressedSize = compressedBuffer.size() * 4; // Estimate the size
		std::vector<char> decompressedData(decompressedSize);

		// Decompress the data
		while (uncompress(reinterpret_cast<Bytef *>(decompressedData.data()), &decompressedSize,
			reinterpret_cast<const Bytef *>(compressedBuffer.data()), compressedBuffer.size()) == Z_BUF_ERROR) {
			// Increase buffer size and try again
			decompressedSize *= 2;
			decompressedData.resize(decompressedSize);
		}

		// Write decompressed data to the output file
		std::ofstream outFile(outputFile, std::ios::binary);
		outFile.write(decompressedData.data(), decompressedSize);
		outFile.close();

		std::cout << "File decompressed successfully to: " << outputFile << std::endl;
	}
};

int main() {
	FileCompressor compressor;
	std::string inputFile = "example.txt";            // Replace with your input file
	std::string compressedFile = "example.zlib";      // Output compressed file
	std::string decompressedFile = "example_decompressed.txt"; // Output decompressed file

	try {
		compressor.compressFile(inputFile, compressedFile);
		compressor.decompress(compressedFile, decompressedFile);
	}
	catch (const std::runtime_error &e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}