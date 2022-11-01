#ifndef __PROGTEST__
#include "command.hpp"
#include "file.hpp"


//CCommand
CCommand::CCommand(const fs::path & p) : m_cmdPath(p) {}
CCommand::~CCommand() {}

string CCommand::getPath() const {
	return m_cmdPath;
}

//CPerm
CPerm::CPerm(const fs::path & p) : CCommand::CCommand(p) {}
CPerm::~CPerm() {}

bool CPerm::execute() const {
	//Path should always be valid, get permissions of path
	fs::perms tmp = fs::status(m_cmdPath).permissions();	

	//Print permission, similar to "ls", pretty much the same as documentation example
    cout << "Permissions of " << m_cmdPath << " are\n"
    	 << ((tmp & fs::perms::owner_read) != fs::perms::none ? "r" : "-")
         << ((tmp & fs::perms::owner_write) != fs::perms::none ? "w" : "-")
         << ((tmp & fs::perms::owner_exec) != fs::perms::none ? "x" : "-")
         << ((tmp & fs::perms::group_read) != fs::perms::none ? "r" : "-")
         << ((tmp & fs::perms::group_write) != fs::perms::none ? "w" : "-")
         << ((tmp & fs::perms::group_exec) != fs::perms::none ? "x" : "-")
         << ((tmp & fs::perms::others_read) != fs::perms::none ? "r" : "-")
         << ((tmp & fs::perms::others_write) != fs::perms::none ? "w" : "-")
         << ((tmp & fs::perms::others_exec) != fs::perms::none ? "x" : "-")
         << endl;

	return true;
}

string CPerm::getName() const {
	return "perm";
}

//CFileSysInfo
CFileSysInfo::CFileSysInfo(const fs::path & p) : CCommand::CCommand(p) {}
CFileSysInfo::~CFileSysInfo() {}

bool CFileSysInfo::execute() const {
	//Always get the space on disk we are currently on, current_path will always be valid
	//m_cmdPath holds information that this was executed on filesystem
	fs::space_info fileSystem = fs::space(fs::current_path());
 
 	//Print out all the info and also approximate conversion with precision loss.
 	//For disks, GB should be good scale
    cout << "Capacity: " << fileSystem.capacity << " B (~"
    		  << setprecision(3) << fileSystem.capacity / 1000000000.f << " GB)\n"
              << "Free space: " << fileSystem.free << " B (~"
              << setprecision(3) << fileSystem.free / 1000000000.f << " GB)\n"
              << "Available space: " << fileSystem.available << " B (~"
              << setprecision(3) << fileSystem.available / 1000000000.f << " GB)" << endl;

	return true;
}

string CFileSysInfo::getName() const {
	return "size";
}

//CSize
CSize::CSize(const fs::path & p) : CCommand::CCommand(p) {}
CSize::~CSize() {}

bool CSize::execute() const {
	//Path should always be valid, but can point to device etc...
	uintmax_t tmpSize = 0;

	//If path is a directory, we need to iterate through all the folders
	//inside the path folder and sum all regular file sizes
	if (fs::is_directory(m_cmdPath)) {
		uintmax_t tmpSize = 0;
		uintmax_t tmpCheck = 0;
		bool isRegular = false;

		//Iterate through every subdirectory and skip the ones that don't have permission to read
    	for (auto & val: fs::recursive_directory_iterator(m_cmdPath, fs::directory_options::skip_permission_denied)) {

    		//Check if we can see the file type
    		try {
				isRegular = fs::is_regular_file(val);
   			}
   			catch(fs::filesystem_error& e) {
   				continue;
    		}

    		//Only count regular files since symbolic links are technically not inside the folder
    		if (isRegular) {
    			tmpCheck = fs::file_size(val);
    			//In some paths like /dev, the size returned by fs::file_size is ~104 TB
    			if (tmpCheck < 10000000000000) {
    				tmpSize += tmpCheck;
    			}
	    	}
    	}

    	//Print out size of the path and also convert it according to file size and round.
    	//MB should be good scale and for small files prints in KB
        cout << "Size of " << m_cmdPath << " is\n" << tmpSize << " B (~" << setprecision(3) << fixed
        	 << (tmpSize >= 1000000 ? tmpSize / 1000000.f : tmpSize / 1000.f)
        	 << (tmpSize >= 1000000 ? " MB)" : " KB)") << endl;

	} else {
		//Try if we can get the file size and print it
		try {
			tmpSize = fs::file_size(m_cmdPath);
   		}
   		catch(fs::filesystem_error& e) {
   			cout << "Path is not regular file, directory or symbolic link." << endl;
   			return false;
    	}

    	//Print out size of the path and also convert it according to file size and round.
    	//MB should be good scale and for small files prints in KB
        cout << "Size of " << m_cmdPath << " is\n" << tmpSize << " B (~" << setprecision(3)
        	 << (tmpSize >= 1000000 ? tmpSize / 1000000.f : tmpSize / 1000.f)
        	 << (tmpSize >= 1000000 ? " MB)" : " KB)") << endl;
	}

	return true;
}

string CSize::getName() const {
	return "size";
}

//CTime
CTime::CTime(const fs::path & p) : CCommand::CCommand(p) {}
CTime::~CTime() {}

bool CTime::execute() const {
	/*Command using C functions, the ctime, mtime and atime are usually the same
	struct stat fileStat;

	if (stat(m_cmdPath.c_str(), &fileStat) == -1) {
		cout << "Could not get file status of " << m_cmdPath << endl;
		return false;
    }

    cout << "Time information about " << m_cmdPath << " is\n"
    	 << "Create time: " << ctime(&fileStat.st_ctime)
    	 << "Access time: " << ctime(&fileStat.st_atime)
    	 << "Modify time: " << ctime(&fileStat.st_mtime) << endl;
	*/

    //Second method using the filesystem library
    auto modifyTime = fs::last_write_time(m_cmdPath);
    //decltype gets the modifyTime type (std::chrono::time_point with long template)
    //Converts the modifyTime to timestamp
    time_t tmp = decltype(modifyTime)::clock::to_time_t(modifyTime);
    //Prints timestamp, for some reason all timestamp to string functions print \n
    cout << "Modify time of " << m_cmdPath << " is\n" << ctime(&tmp);

	return true;
}

string CTime::getName() const {
	return "time";
}

//CTree
CTree::CTree(const fs::path & p, const size_t depth) : CCommand::CCommand(p), m_depth(depth) {}
CTree::~CTree() {}

bool CTree::execute() const {
	auto end = fs::recursive_directory_iterator();

	//Different text for max depth and set depth
	if (m_depth == numeric_limits<size_t>::max()) {
   		cout << "Displaying hierarchy of " << m_cmdPath << " with full depth" << endl;
	} else {
    	cout << "Displaying hierarchy of " << m_cmdPath << " with depth " << m_depth << endl;
	}

	//Iterate through every subdirectory and skip the ones that don't have permission to read
    for (auto it = fs::recursive_directory_iterator(m_cmdPath,
     	fs::directory_options::skip_permission_denied); it != end; ) {

    	//If depth is bigger than wanted depth, pop the directory one level up
    	//so it doesn't go through all subdirectories with no effect - increases
    	//print speed by 1000% when displaying entire file system hierarchy 
	    if ((size_t)it.depth() > m_depth) {
	    	//Pop can bring iterator to end, this is why there isn't ++it in the for loop
	    	it.pop();
	    	continue;
	    }

	    //If current print path is symbolic link, try to read its target and display it
	    if(is_symlink(it->path())) {
	    	string linkPath;
	    	//Throws exception when symbolic link cannot be read
	    	try {
	    		linkPath = read_symlink(it->path());
	    	}
	    	catch(fs::filesystem_error& e) {
	    		//Still print the symbolic link but without target
	    		cout << string(4 * ((it.depth() == 0 ? 1 : it.depth()) - 1), ' ')
	        	 	 << (it.depth() == 0 ? " " : " \\__ ")
	        	 	 << string(it->path().filename()) << " -> " << "no permission to read link" << '\n';

	        	//Iterator needs to be increased else this would be infinite loop
	        	++it;
	    		continue;
	    	}

	    	//Print symbolic link format with target of the symbolic link
	        cout << string(4 * ((it.depth() == 0 ? 1 : it.depth()) - 1), ' ')
	        	 << (it.depth() == 0 ? " " : " \\__ ")
	        	 << string(it->path().filename()) << " -> " << linkPath << '\n';
	    } else {
	    	//Print any other file_type
	        cout << string(4 * ((it.depth() == 0 ? 1 : it.depth()) - 1), ' ')
	        	 << (it.depth() == 0 ? " " : " \\__ ")
	        	 << string(it->path().filename()) << '\n';
	    }

	    //Increase iterator at the very end so it doesn't get stuck in infinite loop
	    ++it;
    }

	return true;
}

string CTree::getName() const {
	return "tree " + ( m_depth == numeric_limits<size_t>::max() ? "-1" : to_string(m_depth));
}

//CFile
CFile::CFile(const fs::path & p, CFileManager * ptr) :
			 CCommand::CCommand(p), m_ptrCache(ptr) {}
CFile::~CFile() {}

bool CFile::execute() const {
	fs::file_status tmpStat = fs::status(m_cmdPath);
	fileType tmp;

	//Switch between file types defined by C++17 filesystem, for regular files print format
	switch (tmpStat.type()) {
		case fs::file_type::regular:
			//Get type of file, using enum for more understandable code
			tmp = getFileType(m_cmdPath);
			if (tmp == err) {
				cout << "Path " << m_cmdPath << " is invalid." << endl;
			} else {
				//Function returns pair of strings containing format and encoding
				pair<string, string> fileFormat = getFormatString(tmp);
				cout << "File " << m_cmdPath << " is\n" << fileFormat.second
					 << " file with " << fileFormat.first << " format." << endl;

				switch (tmp) {
					case txt:
						m_ptrCache->add(new CTextFile(m_cmdPath));
						break;
					case jpeg:
						m_ptrCache->add(new CImageFile(m_cmdPath, "jpeg"));
						break;
					case png:
						m_ptrCache->add(new CImageFile(m_cmdPath, "png"));
						break;
					case img:
						m_ptrCache->add(new CImageFile(m_cmdPath, "unknown"));
						break;
					case bin:
						m_ptrCache->add(new CBinaryFile(m_cmdPath));
						break;
					case snd:
						m_ptrCache->add(new CSoundFile(m_cmdPath));
						break;
					case sz:
						m_ptrCache->add(new CZipFile(m_cmdPath, "7z"));
						break;
					case gzip:
						m_ptrCache->add(new CZipFile(m_cmdPath, "gzip"));
						break;
					case zip:
						m_ptrCache->add(new CZipFile(m_cmdPath, "zip"));
						break;
					default:
						m_ptrCache->add(new COtherFile(m_cmdPath));
						break;
				}
			}
			break;

		case fs::file_type::directory:
			cout << "File " << m_cmdPath << " is a directory." << endl;
			break;

		case fs::file_type::symlink:
			cout << "File " << m_cmdPath << " is a symbolic link." << endl;
			break;

		case fs::file_type::block:
			cout << "File " << m_cmdPath << " is a block device." << endl;
			break;

		case fs::file_type::character:
			cout << "File " << m_cmdPath << " is a character device." << endl;
			break;

		case fs::file_type::fifo:
			cout << "File " << m_cmdPath << " is a pipe." << endl;
			break;

		case fs::file_type::socket:
			cout << "File " << m_cmdPath << " is a socket." << endl;
			break;

		default:
			cout << "File " << m_cmdPath << " is unknown." << endl;
			return false;
	}

	return true;
}

string CFile::getName() const {
	return "file";
}

//CSearch
CSearch::CSearch(const fs::path & p, const size_t depth, const string & args)
				: CCommand::CCommand(p), m_depth(depth), m_fileTypes(args) {}
CSearch::~CSearch() {}

bool CSearch::execute() const {
	auto end = fs::recursive_directory_iterator();

	//Parse string into separate words and insert them into set to force uniqueness
    istringstream tmpStream(m_fileTypes);
	set<string> fileTokens;
	copy(istream_iterator<string>(tmpStream), istream_iterator<string>(), inserter(fileTokens, fileTokens.begin()));

	//We need to try and parse the strings into the enum, this holds types we look for
	set<fileType> searchTokens;

	//Supported types: txt, jpeg, png, bin, sound, img, 7z, gzip, zip, other
	if (fileTokens.find("txt") != fileTokens.end())
		searchTokens.insert(txt);
	if (fileTokens.find("jpeg") != fileTokens.end())
		searchTokens.insert(jpeg);
	if (fileTokens.find("png") != fileTokens.end())
		searchTokens.insert(png);
	if (fileTokens.find("bin") != fileTokens.end())
		searchTokens.insert(bin);
	if (fileTokens.find("sound") != fileTokens.end())
		searchTokens.insert(snd);
	if (fileTokens.find("img") != fileTokens.end())
		searchTokens.insert(img);
	if (fileTokens.find("7z") != fileTokens.end())
		searchTokens.insert(sz);
	if (fileTokens.find("gzip") != fileTokens.end())
		searchTokens.insert(gzip);
	if (fileTokens.find("zip") != fileTokens.end())
		searchTokens.insert(zip);
	if (fileTokens.find("other") != fileTokens.end())
		searchTokens.insert(otr);

	//If we didn't find any supported format from user
	if (searchTokens.empty()) {
		cout << "Couldn't find single valid format from input." << endl;
		return false;
	}

	//Differet text for max depth and set depth
	if (m_depth == numeric_limits<size_t>::max()) {
   		cout << "Looking for formats: ";
   		for (auto & val : searchTokens) {
   			cout << fileTypeDef[val] << ' ';
   		}
   		cout << "\nin: " << m_cmdPath << "\nwith full depth\n";
	} else {
   		cout << "Looking for formats: ";
   		for (auto & val : searchTokens) {
   			cout << fileTypeDef[val] << ' ';
   		}
   		cout << "\nin: " << m_cmdPath << "\nwith depth: " << m_depth << '\n' << endl;
	}


	bool printedDir = false;
	int tmpDepth = -1;
	//Iterate through every subdirectory and skip the ones that don't have permission to read
    for (auto it = fs::recursive_directory_iterator(m_cmdPath,
     	fs::directory_options::skip_permission_denied); it != end; ) {

    	//If depth is bigger than wanted depth, pop the directory one level up
    	//so it doesn't go through all subdirectories with no effect - increases
    	//print speed by 1000% when displaying entire file system hierarchy 
	    if ((size_t)it.depth() > m_depth) {
	    	//Pop can bring iterator to end, this is why there isn't ++it in the for loop
	    	it.pop();
	    	continue;
	    }

	    //If recursion depth changes we need to print directory
	    if(tmpDepth != it.depth()) {
	    	printedDir = false;
	    	tmpDepth = it.depth();
	    }

	    //Only regular file can have file format. If the file is regular,
	    //look if the file format is in the token set.
	    if(is_regular_file(it->path()) && searchTokens.find(
	       getFileType(it->path())) != searchTokens.end()) {

	    	//If we have already printed the directory, only print files.
	    	//Otherwise print the directory and the file name
	    	if (printedDir) {
	    		cout << it->path().filename() << '\n';
	    	} else {
	    		cout << string(it->path().parent_path());
	    		printedDir = true;
	    		cout << "/\n" << it->path().filename() << '\n';
	    	}
	    }

	    //Increase iterator at the very end so it doesn't get stuck in infinite loop
	    ++it;
    }
    //This search can take a long time because of the system call
    cout << "Finished looking for files." << endl;

	return true;
}

string CSearch::getName() const {
	return "find " + ( m_depth == numeric_limits<size_t>::max() ?
		   "-1" : to_string(m_depth)) + " " + m_fileTypes + " on";
}

#endif /* __PROGTEST__ */