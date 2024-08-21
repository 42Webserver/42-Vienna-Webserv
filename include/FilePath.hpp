#ifndef FILEPATH_H
# define FILEPATH_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

class FilePath
{
private:

	std::string	m_string;
	struct stat	m_sb;
	bool		m_statValid;
	void		setStat(void);

public:

	FilePath(void);
	FilePath(const FilePath& other);
	FilePath(const std::string& str);
	FilePath& operator=(const FilePath& other);
	FilePath& operator=(const std::string& str);
	~FilePath();

	//append with / between
	void				append(const std::string& path);
	void				append(const std::string& path, std::size_t start, std::size_t n = std::string::npos);
	void				clear(void);
	const char*			c_str() const;
	const std::string&	str() const;
	void				remove_filename(void);
	void				remove_last(void);
	void				replace_filename(const std::string& filename);
	void				replace_extension(const std::string& extension);
	std::string 		extension(void) const;
	std::string			filename(void) const;
	mode_t				getMode(void) const;
	bool				exists(void) const;
	bool				isFile(void) const;
	bool				isDir(void) const;
	bool				isExec(void) const;
	bool				isRead(void) const;
	bool				isChr(void) const;
};

std::ostream&	operator<<(std::ostream& os, const FilePath& fp);

#endif // !FILEPATH_H
