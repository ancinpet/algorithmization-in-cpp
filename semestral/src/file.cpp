#ifndef __PROGTEST__
#include "file.hpp"


string shellToString(const string & command) {
	string tmpString("");

	//System call returns exit value of command so we check if
	//command was successful, saving info to file in /tmp
	if (system((command + " > " + TYPEFILE).c_str()) != 0) {
		return tmpString;
	}

	//Open the file we saved data into for reading
	ifstream tmp(TYPEFILE, ifstream::in);

	//Try to read one line of the file, the file should always have only one line
	//because the system call creates the file and writes one line into it
	//but there is small chance that someone can remove the file before we open it.
	if (!getline(tmp, tmpString, '\n')) {
		tmp.close();
		return tmpString;
	}

	//Close the file handle since we don't need it anymore
	tmp.close();

	//Try to clean up (remove the file), if we can't the result can be unreliable
	if (!fs::remove(TYPEFILE)) {
		return "";
	}

	return tmpString;
}


fileType getFileType(const string & path) {

	//Gets MIME info from the file path
	string fileInfo = shellToString("file -b --mime-type -- \"" + path + "\"");	

	if (fileInfo.empty())
		return err;

	//Look for output from file system call and return file format accordingly
	if (fileInfo.find("text") != string::npos) {
		return txt;
	} else if (fileInfo.find("jpeg") != string::npos) {
		return jpeg;
	} else if (fileInfo.find("png") != string::npos) {
		return png;
	} else if (fileInfo.find("executable") != string::npos) {
		return bin;
	} else if (fileInfo.find("audio") != string::npos) {
		return snd;
	} else if (fileInfo.find("image") != string::npos) {
		return img;
	}  else if (fileInfo.find("7z") != string::npos) {
		return sz;
	}  else if (fileInfo.find("gzip") != string::npos) {
		return gzip;
	}  else if (fileInfo.find("zip") != string::npos) {
		return zip;
	}

	return otr;
}

///Function to get string of file format
pair<string, string> getFormatString(fileType format) {
	switch(format) {
		case txt:
			return make_pair("generic", "text");
		case jpeg:
			return make_pair("jpeg", "image");
		case png:
			return make_pair("png", "image");
		case bin:
			return make_pair("binary", "executable");
		case snd:
			return make_pair("generic", "sound");
		case img:
			return make_pair("other", "image");
		case sz:
			return make_pair("7z", "compressed");
		case gzip:
			return make_pair("gzip", "compressed");
		case zip:
			return make_pair("zip", "compressed");
		case otr:
			return make_pair("unrecognized", "a");
		default:
			return make_pair("unknown", "a");
	}
}

//CGenericFile
CGenericFile::CGenericFile(const fs::path & p) : m_filePath(p) {}
CGenericFile::~CGenericFile() {}

string CGenericFile::getPath() const {
	return m_filePath;
}

bool CGenericFile::validPath() const {
	bool valid = false;

	//Try to see if path exists, throws exception if we don't have permission
	try {
		valid = fs::exists(m_filePath);
	}
	catch (fs::filesystem_error & e) {
		return false;
	}

	//Check if path is regular file
	if (!fs::is_regular_file(m_filePath))
		return false;

	return valid;
}

//COtherFile
COtherFile::COtherFile(const fs::path & p) : CGenericFile::CGenericFile(p) {}
COtherFile::~COtherFile() {}

void COtherFile::info() const {
	cout << "File " << m_filePath << "\nis generic file with unrecognized format.\n"
		 << "File size is " << (fs::file_size(m_filePath) / 1000.f) << " KB.\n";
}

bool COtherFile::valid() const {
	//Check if path is still valid
	if (!this->validPath())
		return false;

	//Check if format is still valid
	if (getFileType(m_filePath) != otr)
		return false;

	return true;
}

//CTextFile
CTextFile::CTextFile(const fs::path & p) : CGenericFile::CGenericFile(p) {}
CTextFile::~CTextFile() {}

void CTextFile::info() const {
	string wordCount = shellToString("wc -w -- < \"" + string(m_filePath) + "\"");
	string lineCount = shellToString("wc -l -- < \"" + string(m_filePath) + "\"");

	cout << "File " << m_filePath << "\nis text file with "
		 << wordCount << " words and " << lineCount << " lines." << '\n'
		 << "File size is " << (fs::file_size(m_filePath) / 1000.f) << " KB.\n";
}

bool CTextFile::valid() const {
	//Check if path is still valid
	if (!this->validPath())
		return false;

	//Check if format is still valid
	if (getFileType(m_filePath) != txt)
		return false;

	return true;
}

//CImageFile
CImageFile::CImageFile(const fs::path & p, const string & format) :
					   CGenericFile::CGenericFile(p), m_format(format) {}
CImageFile::~CImageFile() {}

void CImageFile::info() const {
	string resolution;

	if (m_format == "png") {
		resolution = shellToString("file -b -- \"" + string(m_filePath) + "\" | cut -d, -f2 | tr -d ' '");
	} else if (m_format == "jpeg") {
		resolution = shellToString("file -b -- \"" + string(m_filePath) + "\" | cut -d, -f8 | tr -d ' '");
	} else {
		resolution = shellToString("file -b -- \"" + string(m_filePath) + "\" | cut -d, -f3 | tr -d ' '");
	}

	cout << "File " << m_filePath << "\nis image file in " << m_format << " format with "
		 << resolution << " resolution.\n"
		 << "File size is " << (fs::file_size(m_filePath) / 1000.f) << " KB.\n";
}

bool CImageFile::valid() const {
	//Check if path is still valid
	if (!this->validPath())
		return false;

	//Check if format is still valid
	fileType tmp = getFileType(m_filePath);
	if (tmp != png && tmp != jpeg && tmp != img)
		return false;

	return true;
}

//CBinaryFile
CBinaryFile::CBinaryFile(const fs::path & p) : CGenericFile::CGenericFile(p) {}
CBinaryFile::~CBinaryFile() {}

void CBinaryFile::info() const {
	string bInfo = shellToString("file -b -- \"" + string(m_filePath) + "\" | cut -d, -f1,2,6");

	cout << "Binary file " << m_filePath << "\nis " << bInfo << ".\n"
		 << "File size is " << (fs::file_size(m_filePath) / 1000.f) << " KB.\n";
}

bool CBinaryFile::valid() const {
	//Check if path is still valid
	if (!this->validPath())
		return false;

	//Check if format is still valid
	if (getFileType(m_filePath) != bin)
		return false;

	return true;
}

//CSoundFile
CSoundFile::CSoundFile(const fs::path & p) : CGenericFile::CGenericFile(p) {}
CSoundFile::~CSoundFile() {}

void CSoundFile::info() const {
	string sndInfo = shellToString("file -b -- \"" + string(m_filePath) + "\" | cut -d, -f5,5");

	cout << "File " << m_filePath << "\nis " << sndInfo << " sound file.\n"
		 << "File size is " << (fs::file_size(m_filePath) / 1000.f) << " KB.\n";
}

bool CSoundFile::valid() const {
	//Check if path is still valid
	if (!this->validPath())
		return false;

	//Check if format is still valid
	if (getFileType(m_filePath) != snd)
		return false;

	return true;
}

//CZipFile
CZipFile::CZipFile(const fs::path & p, const string & format) :
					   CGenericFile::CGenericFile(p), m_format(format) {}
CZipFile::~CZipFile() {}

void CZipFile::info() const {
	string zipInfo = shellToString("file -bZ -- \"" + string(m_filePath) + "\" | cut -d, -f1");

	cout << "File " << m_filePath << "\nis compressed file in " << m_format << " format.\n"
		 << "Trying to peak in, archive contains " << zipInfo << ".\n"
		 << "File size is " << (fs::file_size(m_filePath) / 1000.f) << " KB.\n";
}

bool CZipFile::valid() const {
	//Check if path is still valid
	if (!this->validPath())
		return false;

	//Check if format is still valid
	fileType tmp = getFileType(m_filePath);
	if (tmp != sz && tmp != gzip && tmp != zip)
		return false;

	return true;
}

//CFileManager
CFileManager::CFileManager() {}

CFileManager::~CFileManager() {
	//Free all files
	for (auto it : m_cache) {
		delete it;
	}
}

void CFileManager::print() {
	cout << "Printing info of " << m_cache.size() << " files:\n";

	for (auto val : m_cache) {
		if (val->valid()) {
			cout << string(20, '-') << '\n';
			val->info();
		} else {
			cout << string(20, '-') << '\n' << "File " << val->getPath() << " is no longer valid.\n";
		}
	}

	cout << string(20, '-') << endl;
}

void CFileManager::clear() {
	for (auto it = m_cache.begin(); it != m_cache.end(); ++it) {
		delete *it;
	}

	m_cache.clear();
}

void CFileManager::add(CGenericFile * toAdd) {
	//Check for duplicites
	if (!m_cache.insert(toAdd).second)
		delete toAdd;
}

#endif /* __PROGTEST__ */