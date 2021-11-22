#include "writefile.h"

FileWriter::FileWriter(const std::string& filename, const std::string& password) :
	f(filename.c_str(), "wb"),
	buffer(filebufferchunksize),
	position(0),
	globalposition(0)
{
	shithash((const unsigned char*)password.data(), password.length(), key, 8);
	
	nonce[0] = uid(rengine);
	nonce[1] = uid(rengine);
	if constexpr (std::endian::native == std::endian::big)
	{
		byteswaparray(nonce);
		f.write(nonce, sizeof(nonce));
		byteswaparray(nonce);
	}
	else
	{
		f.write(nonce, sizeof(nonce));
	}

	for (int i = 0; i < 16; i++)
	{
		hash[i] = 0;
	}
	f.write(hash, sizeof(hash));
}

void FileWriter::write(const unsigned char* data, size_t size)
{
	while (size)
	{
		const size_t datatocopy = std::min<size_t>(size, buffer.size() - position);
		std::memcpy(&buffer[position], data, datatocopy);
		position += datatocopy;
		globalposition += datatocopy;
		data += datatocopy;
		size -= datatocopy;
		if (position >= buffer.size())
		{
			flush();
		}
	}
}

void FileWriter::finalise()
{
	flush();
	shithashfinalise(hash, 16);
	if constexpr (std::endian::native == std::endian::big)
	{
		byteswaparray(hash);
	}
	chacha20((unsigned char*)hash, sizeof(hash), key, 0, nonce);
	f.seekl(sizeof(nonce));
	f.write(hash, sizeof(hash));
}

void packfolder(const fs::path& arc, const std::string& password)
{
	const fs::path folder = arc.parent_path() / arc.stem();
	try
	{
		std::vector<unsigned char> buffer(filebufferchunksize);
		FileWriter fw(arc.string(), password);
		for (const auto& i : fs::recursive_directory_iterator(folder))
		{
			if (!i.is_directory())
			{
				const std::string name = fs::relative(i.path(), folder).string();
				fw.write((const unsigned char*)name.c_str(), name.length() + 1);
				const size_t filesize = fs::file_size(i);
				fw.write((const unsigned char*)&filesize, sizeof(filesize));
				File f(i.path().string().c_str(), "rb");
				while (const size_t readcount = f.read(buffer.data(), buffer.size()))
					fw.write(buffer.data(), readcount);
			}
		}
		fw.finalise();
	}
	catch (const std::exception& e)
	{
		fs::remove(arc);
		printCrashMessage(e.what());
	}
	catch (...)
	{
		fs::remove(arc);
		printCrashMessage("Okänt fel");
	}
	fs::remove_all(folder);
}