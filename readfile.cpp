#include "readfile.h"

void FileReader::load()
{
	const size_t datatoread = std::min<size_t>(buffer.size(), encsize - globalposition);
	f.readwhole(buffer.data(), datatoread);
	chacha20(buffer.data(), datatoread, key, (globalposition - position) / 64u, nonce);
	position = 0;
}

FileReader::FileReader(const std::string& filename, const std::string& password) :
	f(filename.c_str(), "rb"),
	encsize(fs::file_size(filename) - sizeof(nonce)),
	buffer(std::min<size_t>(encsize - 64, filebufferchunksize)),
	position(0),
	globalposition(64)
{
	shithash((const unsigned char*)password.data(), password.length(), key, 8);

	f.readwhole(nonce, sizeof(nonce));
	if constexpr (std::endian::native == std::endian::big)
	{
		byteswaparray(nonce);
	}

	f.skip(64);

	load();
}

void FileReader::read(unsigned char* buffer, size_t size)
{
	if (globalposition + size > encsize)
		throw std::runtime_error("Unexpected end of file");
	while (size)
	{
		const size_t datatoread = std::min<size_t>(size, this->buffer.size() - position);
		std::memcpy(buffer, &this->buffer[position], datatoread);
		position += datatoread;
		globalposition += datatoread;
		buffer += datatoread;
		size -= datatoread;
		if (position >= this->buffer.size())
		{
			load();
		}
	}
}

void unpackarchive(const fs::path& arc, const std::string& password)
{
	const fs::path folder = arc.parent_path() / arc.stem();
	try
	{
		FileReader fr(arc.string(), password);
		std::vector<unsigned char> buffer(std::min<size_t>(fs::file_size(arc) - 72, filebufferchunksize));
		while (!fr.atend())
		{
			std::string path;
			char c;
			while (fr.read((unsigned char*)&c, sizeof(c)), c) path += c;

			size_t size;
			fr.read((unsigned char*)&size, sizeof(size));

			const fs::path relpath = folder / path;
			fs::create_directories(relpath.parent_path());
			File f(relpath.string().c_str(), "wb");
			while (size)
			{
				const size_t sizetoread = std::min<size_t>(size, buffer.size());
				fr.read(buffer.data(), sizetoread);
				f.write(buffer.data(), sizetoread);
				size -= sizetoread;
			}
		}
	}
	catch (const std::exception& e)
	{
		fs::remove_all(folder);
		printCrashMessage(e.what());
	}
	catch (...)
	{
		fs::remove_all(folder);
		printCrashMessage("Okänt fel");
	}
	// Rensar
	File f(arc.string().c_str(), "wb");
}